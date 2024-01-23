
#include "globals.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fifo.h"
#include "text.h"
#include "uart.h"
#include "lcd.h"
#include "pins.h"
#include "controller.h"

#define SVC_TIME_SCALE 6			// timerscale

#define PIN_BKLIGHT (1 << SW_BK)
#define PIN_NEXTSVC (1 << SW_NEXT)

#define BUFFER_MAX		30
#define QUOTED_MAX		2
#define CMD_INIT		"@:"

#define ST_READY		0x00
#define ST_COMMAND		0x10
#define ST_PAGECOUNT	0x20
#define ST_PAGETIME		0x30
#define ST_SVC_STATE	0x40
#define ST_SVC_STATE2	0x50
#define ST_SVC_TIMEOUT	0x60
#define ST_SVC_NAME		0x70
#define ST_DEBUG		0x80
#define ST_DISPLAYMODE	0x90
#define ST_RESET		0xF0

volatile uint8_t	_svc_timerscale = 0;
volatile uint8_t	_ser_serviceId = 0;
volatile uint8_t	_ser_state = ST_READY;
volatile uint8_t	_ser_debug = 0;
volatile uint8_t	_ser_quote = 0;

Text _ser_buffer;
Text _ser_quoted;

FiFo rb;
CtrlData ctrl;

DPin _sw_light;
DPin _sw_next;

const char* codes[CTRL_MAX_STATECOUNT] = {
	"  n/a", " okay", "  ???", " warn", "error"
};

uint8_t sign[CTRL_MAX_STATECOUNT] = {
	0xA5, 0xFF, '?', 'W',  'E'
};

ISR(TIMER0_OVF_vect)
{
	_svc_timerscale++;
	
	// ensure a real second
	if (_svc_timerscale > SVC_TIME_SCALE) {
		_svc_timerscale = 0;
		ctrl_tick(&ctrl);
	}
	
	// blink display on error if not manually enabled
	if (ctrl_lightState(&ctrl) == light_blink) {
		lcd_bkinv();
	}
}

#define debug(c) if (_ser_debug) { uart_putc(c); uart_putc(' '); }
#define on(c) if ((_ser_state & 0xF0) == c) 

void handleReset() {
	text_reset(&_ser_buffer);
	text_reset(&_ser_quoted);
	_ser_quote = 0;
	
	_ser_serviceId = 0;
	
	debug('<')
	_ser_state = ST_READY;
}

void setup() {
	
	// disable interrupts
	cli();

	_ser_debug = 0;
	handleReset();	

	text_init(&_ser_buffer, BUFFER_MAX);
	text_init(&_ser_quoted, QUOTED_MAX);
	
	fifo_init(&rb, 30);
	uart_init(&rb);
	
	ctrl_init(&ctrl);
	
	// initalize the lcd display.
	lcd_init();
	lcd_control(on, off, off);
	lcd_puts("Overall: ");
	
	// enable timer
	// 2MHz / 1024 / 256 = 7,62Hz
	TCCR0= (1<<CS02)|(1<<CS00);     // prescaler 1024
	TIMSK= (1<<TOIE0);				// set timer interrupt
	
	// set in port for light switch
	pin_d(DDRC, (PIN_BKLIGHT | PIN_NEXTSVC));
	pin_s(PORTC, (PIN_BKLIGHT | PIN_NEXTSVC));
	
	pin_init(&_sw_light, &PINC, PIN_BKLIGHT);
	pin_init(&_sw_next, &PINC, PIN_NEXTSVC);
	
	// enable interrupts
	sei();
}

void update_display() {
		
	CtrlState overall = undefined;
	lcd_sets(0, 1, "                    ");

	for (uint8_t i = 0; i < CTRL_MAX_PAGECOUNT; i++) {
	
		if (i < ctrl.maxPages) {			
			if (ctrl.entries[i].state > overall) {
				overall = ctrl.entries[i].state;
			}

			if ( i == ctrl.pageNext ) {
				lcd_setc(i, 2, '_'); // marker

				lcd_sets(0, 1, ctrl.entries[i].name);
				lcd_putc(':');
				
				if ((ctrl.entries[i].value[0] == '\0') || (ctrl.entries[i].state == invalid)) {
					lcd_sets(15, 1, codes[ctrl.entries[i].state]);
				} else {
					lcd_sets(15, 1, ctrl.entries[i].value);
				}
				
			} else {
				lcd_setc(i, 2, ' '); // marker
			}

			// reduce expiration
			ctrl_serviceExpiry(&ctrl, i, ctrl.maxScreenTime);
						
			lcd_setc(i, 3, sign[ctrl.entries[i].state]); // sign
		} else {
			lcd_setc(i, 2, ' ');	// marker
			lcd_setc(i, 3, ' ');	// sign
		}		
	}
	
	// set overall status
	lcd_sets(15, 0, codes[overall]);
	
	ctrl_pageShowing(&ctrl, overall);
}

void handleReady() {		
	uint8_t v = _ser_state & 0x0F;	
	
	// too less chars?
	if (_ser_buffer.pos < (v + 1)) return;
	
	// check init string
	if ( _ser_buffer.buffer[ v ] != CMD_INIT[v] ) {
		text_reset(&_ser_buffer);
		return;
	}
		
	if (v == 1) {
		debug('>')
		_ser_state = ST_COMMAND;
		text_reset(&_ser_buffer);
	} else {
		_ser_state++;
	}
}

void handleDebug() {
	if (_ser_debug == 0) {		
		_ser_debug = 1;		
	} else {
		_ser_debug = 0;
	}	
	
	_ser_state = ST_RESET;
}

void handleCommand() {
	if (text_len(&_ser_buffer) < 1) return;

	uint8_t c = text_at(&_ser_buffer, 0);
	text_reset(&_ser_buffer);

	debug(c)	
	switch (c) {
		case 'd':
			_ser_state = ST_DEBUG;
			break;

		case 's':
			_ser_state = ST_SVC_STATE;
			break;

		case 'c':
			_ser_state = ST_SVC_STATE2;
			break;
						
		case 'n':
			_ser_state = ST_SVC_NAME;
			break;

		case 'e':
			_ser_state = ST_SVC_TIMEOUT;
			break;
					
		case 'p':
			_ser_state = ST_PAGECOUNT;
			break;

		case 't':
			_ser_state = ST_PAGETIME;
			break;
			
		case 'm':
			_ser_state = ST_DISPLAYMODE;
			break;
					
		default:	// reset
			debug('?')
			_ser_state = ST_RESET;
			break;
	}	

}

void handlePageCount() {
	if (text_len(&_ser_buffer) < 2) return;
	uint8_t v = text_tol(&_ser_buffer, 16);
					
	ctrl_configNumPages(&ctrl, v);	
	_ser_state = ST_RESET;
}

void handlePageTime() {
	
	if (text_len(&_ser_buffer) < 2) return;
	uint8_t v = text_tol(&_ser_buffer, 16);

	ctrl_configScreenTime(&ctrl, v);
	_ser_state = ST_RESET;
}

void handleDisplayMode() {
	
	if (text_len(&_ser_buffer) < 2) return;
	uint8_t v = text_tol(&_ser_buffer, 16);

	ctrl_configMode(&ctrl, v);
	_ser_state = ST_RESET;
}

void handleSvcTimeout() {
	
	if (text_len(&_ser_buffer) < 4) return;
	uint16_t v = text_tol(&_ser_buffer, 16);
				
	ctrl_configServiceExpiry(&ctrl, v);
	_ser_state = ST_RESET;
}

/*
 * Read a name with max len
 * @return 0 if name is not yet finished
 * @return > 0, where result is size of text
 */
uint16_t readName(uint8_t maxlen) {
	if (text_len(&_ser_buffer) == 0) {
		return 0xFF00;
	}

	uint8_t lastPos = _ser_buffer.pos - 1;
	if (( lastPos == maxlen ) || ( text_at(&_ser_buffer, lastPos) == '!'))
	{
		return lastPos;
	} 
	
	return 0xFF00;	
}

void handleSvcState(uint8_t custom) {
	uint8_t s = _ser_state & 0x0F;
	uint16_t v = 0;
	
	// set service state
	switch (s) {
		case 0: // read service id
			if (text_len(&_ser_buffer) < 2) return;
		
			_ser_serviceId = text_tol(&_ser_buffer, 16);
			text_reset(&_ser_buffer);
			_ser_state++;
			break;
		
		case 1: // read service state at 1
			if (text_len(&_ser_buffer) < 2) return;

			v = text_tol(&_ser_buffer, 16);
			ctrl_serviceState(&ctrl, _ser_serviceId, v);
			
			if (custom > 0) {
				text_reset(&_ser_buffer);
				_ser_state++;
			} else {
				_ser_state = ST_RESET;				
			}			
			break;
		
		case 2:
			v = readName(CTRL_MAX_STATELEN);
			if ((v & 0xFF00) == 0) {
				ctrl_serviceStateValue(&ctrl, _ser_serviceId, (const char*)_ser_buffer.buffer, v);
				_ser_state = ST_RESET;
			}
			break;	
	}	
}

void handleSvcName() {

	if ((_ser_state & 0x0F) == 0) {
		if (text_len(&_ser_buffer) < 2) return;
		
		_ser_serviceId = text_tol(&_ser_buffer, 16);
		text_reset(&_ser_buffer);
		_ser_state++;
	} else {
		uint16_t len = readName(CTRL_MAX_NAMELEN);
		if ((len & 0xFF00) == 0)
		{
			ctrl_configServiceName(&ctrl, _ser_serviceId, (const char*)_ser_buffer.buffer, len);
			_ser_state = ST_RESET;
		} 		
	}
}

void readNextChar() {
	uint16_t c = fifo_get(&rb);
	if ((c & 0xFF00) != 0) return;
	
	// use enter for reset
	if ((c == 13) || (c == 10)) {
		text_reset(&_ser_buffer);
		return;
	}

	// handle quote
	if (_ser_quote == 0) {						
		// start quoting?
		if (c == '\\') {
			text_reset(&_ser_quoted);
			_ser_quote = 1;
		} else {
			text_push(&_ser_buffer, c);
		}	
	} else {
		if (c == '\\') {	// double backslash
			text_push(&_ser_buffer, c);
			_ser_quote = 0;			
		} else {
			if (text_len(&_ser_quoted) < 2) {
				text_push(&_ser_quoted, c);
			} else {
				uint8_t v = text_tol(&_ser_quoted, 16);
				text_push(&_ser_buffer, v);		// push quoted char to buffer
				text_push(&_ser_buffer, c);		// push current char to buffer
				// debug(v);
				_ser_quote = 0;
			}
		}		
	}
}

int main(void)
{	
	setup();
	_delay_ms(10);
				
	while (1){

		// next button?
		if (pin_psd(&_sw_next)) {
			ctrl_pageNext(&ctrl);
		}

		// light button?
		if (pin_psd(&_sw_light)) {
			ctrl_lightActivate(&ctrl);
		} 

		// display update
		if (ctrl_pageNeedUpdate(&ctrl)) {
			update_display();
		}
		
		// display on/off
		switch (ctrl_lightState(&ctrl)) {
			case light_on:
				lcd_bk(on); break;
				
			case light_off:
				lcd_bk(off); break;
				
			default:
				break;
		}
		
		// read the next char, if available
		readNextChar();
						
		// --------------------------------------------------------------------
		// states 
		// --------------------------------------------------------------------

		on( ST_READY ) {	// wait for header		
			handleReady(); 
			continue;
		}
		
		on( ST_COMMAND ) {	// wait for command
			handleCommand();
			continue;
		}

		on( ST_PAGECOUNT ) { // set page count
			handlePageCount();
			continue;			
		}

		on( ST_PAGETIME ) {	// set page timer
			handlePageTime();	
			continue;
		}
				
		on( ST_SVC_TIMEOUT ) {	// set service validation timeout
			handleSvcTimeout(); 
			continue;
		}
			
		on( ST_SVC_STATE ) {
			handleSvcState(0);
			continue;
		}

		on( ST_SVC_STATE2 ) {
			handleSvcState(1);
			continue;
		}
				
		on( ST_SVC_NAME ) {
			handleSvcName();
			continue;
		}
		
		on( ST_DEBUG ) {
			handleDebug();
			continue;
		}
		
		on ( ST_DISPLAYMODE ) {
			handleDisplayMode();
			continue;
		}
		
		on( ST_RESET ) {
			handleReset();
			continue;
		}
	}
		
	return 0;
}

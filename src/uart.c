/*
 * uart.c
 *
 * Created: 03.01.2024 07:08:13
 *  Author: waldwuffel
 */ 

#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/setbaud.h>

FiFo* _uartReadBuffer;

// UDR received some data
ISR(USART_RXC_vect) {
	uint8_t v = UDR;
	fifo_put(_uartReadBuffer, v);	
	// uart_putc(v);
}

void uart_init(FiFo* rb) {
			
	_uartReadBuffer = rb;
		
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	
	#if USE_2X
		UCSRA |= (1 << U2X);
	#else
		UCSRA &= ~(1 << U2X);
	#endif
	
	// enable send + interrupt
	UCSRB |= _BV(TXEN); // | _BV(TXCIE);
		
	// enable receive + interrupt
	UCSRB |= _BV(RXEN) | _BV(RXCIE);
		
	// Frame Format: Asynchron 8N1
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0); // 8bit
		
}

void uart_putc(unsigned char c) {
	while (!(UCSRA & (1<<UDRE)));	// wait for sending is possible
	UDR = c;						// write a single byte
}

void uart_puts(const char* s) {
	while ((*s) != 0) {
		uart_putc(*s);
		s++;
	}
}

void uart_putn(const char* s, uint8_t len) {
	for (int i = 0; i < len; i++) {		
		uart_putc(s[i]);
	}
}

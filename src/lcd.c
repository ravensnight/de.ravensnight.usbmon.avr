/*
 * lcd.c
 *
 * Created: 03.01.2024 12:22:22
 *  Author: waldwuffel
 */ 

#include "lcd.h"
#include "pins.h"

#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
	#error "F_CPU not set!"
#endif

/**
 * Define pin masks
 */
#define LCD_MASK_DATA	(0x0F << LCD_DATA)
#define LCD_MASK_RS		(1 << LCD_RS)
#define LCD_MASK_RW		(1 << LCD_RW)
#define LCD_MASK_EN		(1 << LCD_EN)
#define LCD_MASK_BK		(1 << LCD_BK)

/**
 * Define commands masks
 */
#define LCD_CMD_CLEAR	0x01
#define LCD_CMD_HOME	0x02
#define LCD_CMD_MODE	0x04
#define LCD_CMD_DISP	0x08
#define LCD_CMD_SHIFT	0x10
#define LCD_CMD_FUNCT	0x20
#define LCD_CMD_CGADDR	0x40
#define LCD_CMD_DDADDR	0x80

#define LCD_MASK_BF		0x80

uint8_t lcd_read() {

	uint8_t res = 0;

	pin_d(LCD_DDR, LCD_MASK_DATA)	// set whole data line to READ	
	
	pin_d(LCD_PORT, LCD_MASK_RS)
	pin_s(LCD_PORT, LCD_MASK_RW)	

	// read higher nibble	
	
	pin_s(LCD_PORT, LCD_MASK_EN)
	_delay_us(40);
		
	res = ((LCD_PIN >> LCD_DATA) << 4);

	pin_d(LCD_PORT, LCD_MASK_EN)
	_delay_us(40);

	// read lower nibble
	
	pin_s(LCD_PORT, LCD_MASK_EN)
	_delay_us(40);
	
	res |= ((LCD_PIN >> LCD_DATA) & 0x0F);		
	
	pin_d(LCD_PORT, LCD_MASK_EN)

	return res;
}

/**
 * Wait until display is ready
 */
void lcd_wait() {	
	uint8_t bf = 0;
	do {
		bf = lcd_read();
	} while (bf & LCD_MASK_BF);
}

void lcd_out4(Flag rs, Flag rw, uint8_t data) {		
	lcd_wait();
	
	pin_s(LCD_DDR, LCD_MASK_RS | LCD_MASK_RW | LCD_MASK_DATA)

	pin_v(LCD_PORT, LCD_MASK_RS, rs)
	pin_v(LCD_PORT, LCD_MASK_RW, rw)
	pin_d(LCD_PORT, LCD_MASK_DATA)							// delete old data
	pin_s(LCD_PORT, ((data << LCD_DATA) & LCD_MASK_DATA))	// assign new data
	
	pin_s(LCD_PORT, LCD_MASK_EN)
	_delay_us(40);	
	pin_d(LCD_PORT, LCD_MASK_EN)
}

void lcd_cmd(uint8_t data) {		
	lcd_out4(off, off, data >> 4);		
	lcd_out4(off, off, data & 0xF);		
}

void lcd_data(uint8_t data) {	
	lcd_out4(on, off, data >> 4);
	lcd_out4(on, off, data & 0xF);
}

void lcd_init() {

	// set DDR
	uint8_t bits = (LCD_MASK_DATA | LCD_MASK_RW | LCD_MASK_EN | LCD_MASK_RS | LCD_MASK_BK);
	
	LCD_DDR |= bits;	
	LCD_PORT &= ~bits;
	
	_delay_ms(10);	// wait at startup
	
	// initialize, incl. 4bit version (1st byte only)
	lcd_out4(off, off, LCD_CMD_FUNCT >> 4);
	
	// enable multiline
	lcd_cmd( LCD_CMD_FUNCT | ( 1 << 3 ));
	
	// display mode
	lcd_control(on, on, off);
	
	// delete display and home
	lcd_clear();
}

void lcd_clear() {
	lcd_cmd(LCD_CMD_CLEAR);	
	_delay_ms(2);
}

void lcd_home() {
	lcd_cmd(LCD_CMD_HOME);	
	_delay_ms(2);
}

void lcd_mode(Flag addrIncr, Movable shift) {
	uint8_t val = LCD_CMD_MODE;
	
	if (addrIncr) val |= 0x02;
	if (shift == Display) val |= 0x01;
	
	lcd_cmd(val);	
}

void lcd_control(Flag dispOn, Flag showCursor, Flag blink) {
	uint8_t val = LCD_CMD_DISP;
	
	if (dispOn) val |= 0x04;
	if (showCursor) val |= 0x02;
	if (blink) val|= 0x01;
	
	lcd_cmd(val);
}

void lcd_shift(Movable movable, Flag moveRight) {
	uint8_t val = LCD_CMD_SHIFT;

	if (movable == Display)	val |= 0x08;
	if (moveRight) val |= 0x04;
	
	lcd_cmd(val);	
}

void lcd_putc(uint8_t ascii) {
	lcd_data(ascii);	
}

void lcd_puts(const char* s) {
	while (*s != 0) {
		lcd_data(*s);
		s++;
	}
}

void lcd_pos( uint8_t x, uint8_t y )
{
	uint8_t val = LCD_CMD_DDADDR;
	
	switch (y)
	{
		case 0:  
			val |= (0x00 + x);
			break;
		
		case 1:    
			val |= (0x40 + x);
			break;
		
		case 2: 
			val |= (0x14 + x);
			break;
		
		case 3:    
			val |= (0x54 + x);
			break;
		
		default:
			return;
	}
	
	lcd_cmd(val);
}

void lcd_bk(Flag onOff) {
	pin_v(LCD_PORT, LCD_MASK_BK, onOff);
}

void lcd_bkinv() {
	if (LCD_PORT & LCD_MASK_BK) {
		LCD_PORT &= ~LCD_MASK_BK;
	} else{
		LCD_PORT |= LCD_MASK_BK;
	}
}

void lcd_sets(uint8_t x, uint8_t y, const char *s) {
	lcd_pos(x, y);
	lcd_puts(s);
}

void lcd_setc(uint8_t x, uint8_t y, uint8_t c) {
	lcd_pos(x, y);
	lcd_putc(c);
}

/*
 * display.h
 * Created: 03.01.2024 12:22:05
 */ 

#ifndef LCD_H_
#define LCD_H_

#include "globals.h"

#include <avr/io.h>

typedef enum _Flag { off = 0, on = 1 } Flag;
typedef enum _Movable { Cursor = 0, Display = 1 } Movable;

#ifndef LCD_DDR
#error "LCD_DDR  must be defined!"
#endif

#ifndef LCD_PORT
#error "LCD_PORT must be defined!"
#endif

#ifndef LCD_PIN
#error "LCD_PIN must be defined!"
#endif

#ifndef LCD_DATA
#error "LCD_DATA must be defined!"
#endif

#ifndef LCD_RS
#error "LCD_RS must be defined!"
#endif

#ifndef LCD_RW
#error "LCD_RW must be defined!"
#endif

#ifndef LCD_EN
#error "LCD_EN must be defined!"
#endif

#ifndef LCD_BK
#error "LCD_BK must be defined!"
#endif

/**
 * Initialize display.
 */
void lcd_init();

void lcd_clear();

void lcd_home();

void lcd_mode(Flag addrIncr, Movable shift);

void lcd_control(Flag dispOn, Flag showCursor, Flag blink);

void lcd_shift(Movable movable, Flag moveRight);

void lcd_putc(uint8_t ascii);

void lcd_puts(const char* s);

void lcd_setc(uint8_t x, uint8_t y, uint8_t c);

void lcd_sets(uint8_t x, uint8_t y, const char *s);

void lcd_pos(uint8_t x, uint8_t y);

void lcd_bk(Flag onOff);

void lcd_bkinv();

#endif /* LCD_H_ */
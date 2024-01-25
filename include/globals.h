/*
 * cpu.h
 *
 * Created: 03.01.2024 12:40:50
 *  Author: ravensnight
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

// #define F_CPU 2000000UL			// 2 MHz
#define BAUD 9600UL				// 96700baud
#define PINS_DEBOUNCE_CYCLES 4

// LDS display definitions
#define LCD_PORT	PORTB		// register to be used for writing
#define LCD_PIN		PINB		// register to be used for reading
#define LCD_DDR		DDRB		// data direction register
#define LCD_DATA	PB2			// start bit for 4-bit data 
#define LCD_BK		PB6			// background light switch		
#define LCD_RW      PB7			// RW bit
#define LCD_RS      PB1			// RS bit
#define LCD_EN      PB0			// Enable bit

// Switch/Button pins
#define SW_BK       PC5         // Backlight switch
#define SW_NEXT     PC4         // Flip to next Page

#endif /* GLOBALS_H_ */
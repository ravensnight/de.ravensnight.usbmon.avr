/*
 * uart.h
 *
 * Created: 03.01.2024 07:07:12
 *  Author: ravensnight
 */ 


#ifndef UART_H_
#define UART_H_

#include "globals.h"
#include "fifo.h"

void uart_init(FiFo* rb);
	
void uart_putc(unsigned char c);

void uart_puts(const char* s);

void uart_putn(const char* s, uint8_t len);

#endif /* UART_H_ */
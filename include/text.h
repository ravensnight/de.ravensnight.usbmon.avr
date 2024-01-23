/*
 * buffer.h
 *
 * Created: 21.01.2024 10:06:03
 *  Author: waldwuffel
 */ 


#ifndef TEXT_H_
#define TEXT_H_

#include <avr/io.h>

typedef struct _Text {
		
	uint8_t size;
	unsigned char* buffer;	
	volatile uint8_t pos;

} Text;

void text_init(Text* t, uint8_t size);

void text_push(Text* t, uint8_t c);

void text_reset(Text* t);

uint8_t text_len(Text* t);

uint8_t text_at(Text* t, uint8_t pos);

long text_tol(Text* t, uint8_t base);

#endif /* TEXT_H_ */
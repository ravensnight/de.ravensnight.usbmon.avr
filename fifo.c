/*
 * fifo.c
 *
 * Created: 02.01.2024 15:57:56
 *  Author: waldwuffel
 */ 

#include "fifo.h"

#include <stdlib.h>
#include <string.h>

uint8_t _fifo_inc(FiFo* f, uint8_t start, uint8_t by) {
	uint8_t res = start;
	
	for (int i = 0; i < by; i++) {
		res += 1;
		if (res >= f->size) res = 0;		
	}
	
	return res;
}

void fifo_clear(FiFo* f) {
	f->pos_r = 0;
	f->pos_w = 0;
}

void fifo_init(FiFo* f, uint8_t size) {
	f->buffer = malloc(size);
	f->size = size;	
	fifo_clear(f);
}

uint8_t fifo_isEmpty(FiFo* f) {
	return (f->pos_r == f->pos_w);
}

uint8_t fifo_isFull(FiFo* f) {
	uint8_t next = f->pos_w + 1;
	return (next == f->pos_r) || ((next == f->size) && (f->pos_r == 0));
}

uint8_t fifo_shift(FiFo* f) {
	return fifo_shiftn(f, 1);
}

uint8_t fifo_shiftn(FiFo* f, uint8_t len) {
	
	uint8_t n = fifo_avl(f);
	if (len > n) {
		n = len;
	}

	f->pos_r = _fifo_inc(f, f->pos_r, n);
	return n;
}

uint16_t fifo_get(FiFo* f) {
	if (fifo_isEmpty(f)) return 0xFF00;
	
	unsigned char c = f->buffer[f->pos_r];	
	f->pos_r = _fifo_inc(f, f->pos_r, 1);
	
	return c;
}

uint8_t fifo_put(FiFo* f, unsigned char c) {
	if (fifo_isFull(f)) return 0;
	
	f->buffer[ f->pos_w ] = c;
	f->pos_w = _fifo_inc(f, f->pos_w, 1);
	
	return 1;
}

uint8_t fifo_avl(FiFo* f) {
	
	if (f->pos_r == f->pos_w) {
		return 0; // empty
	}
	
	if (f->pos_r < f->pos_w) {
		return f->pos_w - f->pos_r;
	}
	
	return (f->size - f->pos_r) + f->pos_w - 1;
}

uint8_t fifo_peek(FiFo* f, char* buffer, uint8_t size) {
	
	uint8_t pos = f->pos_r;
	uint8_t read = 0;
		
	while (read < size) {
				
		if (pos == f->pos_w) {
			// no more bytes to read;
			break;
		}
		
		buffer[read] = f->buffer[pos];
		pos = _fifo_inc(f, pos, 1);
		
		read++;
	}
	
	buffer[read] = '\0';
	return read;
}

uint8_t fifo_read(FiFo* f, char* buffer, uint8_t len) {
	if (fifo_avl(f) < len) {
		return 0;
	}
	
	for (int i = 0; i < len; i++) {
		buffer[i] = fifo_get(f);
	}
	
	buffer[len] = '\n';
	return len;
}

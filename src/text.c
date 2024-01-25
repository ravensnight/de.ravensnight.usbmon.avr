/*
 * text.c
 *
 * Created: 21.01.2024 13:59:51
 *  Author: ravensnight
 */ 

#include "text.h"

#include <stdlib.h>

void text_init(Text* t, uint8_t size) {
	t->size = size;
	t->pos = 0;
	t->buffer[0] = '\0';
	t->buffer = malloc(size + 1);
}

void text_push(Text* t, uint8_t c) {
	if (t->pos >= t->size) return;
	
	t->buffer[t->pos] = c;
	t->buffer[t->pos + 1] = '\0';
	t->pos += 1;
}

void text_reset(Text* t) {
	t->pos = 0;
	t->buffer[0] = '\0';
}

uint8_t text_len(Text* t) {
	return t->pos;
}

uint8_t text_at(Text* t, uint8_t pos) {
	if (t->pos > pos) {
		return t->buffer[pos];
	}
	
	return '\0';
}

long text_tol(Text* t, uint8_t base) {
	return strtol((const char*) t->buffer, NULL, base);
}

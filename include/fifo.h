/*
 * fifo.h
 *
 * Created: 02.01.2024 15:51:38
 *  Author: waldwuffel
 */ 


#ifndef FIFO_H_
#define FIFO_H_

#include <avr/io.h>

typedef struct _FiFo {
	
	uint8_t size;
	unsigned char* buffer;
	
	volatile uint8_t pos_r;
	volatile uint8_t pos_w;
	
} FiFo;

/**
 * Initialize the FiFo with the given size and reset all pointers.
 */
void fifo_init(FiFo* f, uint8_t size);

/**
 * @return 1 if buffer is empty otherwise 0.
 */
uint8_t fifo_isEmpty(FiFo* f);

/**
 * @return 1 if buffer cannot receive anymore chars or 0 if buffer is not yet full.
 */
uint8_t fifo_isFull(FiFo* f);

/**
 * Write some character to buffer.
 * @return 1 if successful or 0 if failed.
 */ 
uint8_t fifo_put(FiFo* f, unsigned char c);

/**
 * Read a single byte from buffer, if available.
 * @return the lower byte will contain the character read. the higher byte will be 0x00, if result was okay.
 */
uint16_t fifo_get(FiFo* f);

/**
 * Provide the number of bytes available
 * @return the number of bytes available
 */
uint8_t fifo_avl(FiFo* f);

/**
 * Read a certain number of bytes in to the given buffer.
 * @return the number of bytes read
 */
uint8_t fifo_peek(FiFo* f, char* buffer, uint8_t size);

/**
 * Shift as single byte in FiFo in read pointer, if available. 
 * @return The number of bytes skipped.
 */
uint8_t fifo_shift(FiFo* f);

/**
 * Skip <size> bytes in FiFo in read pointer, if available. 
 * @return The number of bytes skipped.
 */
uint8_t fifo_shiftn(FiFo* f, uint8_t size);

/**
 * Reset the fifo buffer to empty.
 */
void fifo_clear(FiFo* f);

/**
 * Read a given number of bytes from FiFo. If the number of bytes read
 * matches the number of bytes requested, the fifo is pushed forward to the next bytes.
 * @return 0, if number of bytes could not be read otherwise 1.
 */
uint8_t fifo_read(FiFo* f, char* buffer, uint8_t size);

#endif /* FIFO_H_ */
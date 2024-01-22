/*
 * pins.h
 *
 * Created: 05.01.2024 14:02:56
 *  Author: waldwuffel
 */ 


#ifndef PINS_H_
#define PINS_H_

#include <avr/io.h>

#ifndef PINS_DEBOUNCE_CYCLES
	#define PINS_DEBOUNCE_CYCLES 8
#endif

// ----------------------------------------------------------------------------
// Simple Pin functions
// ----------------------------------------------------------------------------
#define pin_s(reg, mask) reg |= (mask);
#define pin_d(reg, mask) reg &= ~(mask);
#define pin_v(reg, mask, v) if (v) { reg |= (mask); } else { reg &= ~(mask); }

// ----------------------------------------------------------------------------
// Debounced Pin
// ----------------------------------------------------------------------------

typedef struct _DPin {
	volatile uint8_t *reg;
	uint8_t pin;
	uint8_t dbounce;
} DPin;

void pin_init(DPin* dpin, volatile uint8_t* reg, uint8_t mask);

uint8_t pin_psd(DPin* dpin);

#endif /* PINS_H_ */
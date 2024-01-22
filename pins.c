/*
 * pins.c
 *
 * Created: 05.01.2024 14:08:24
 *  Author: waldwuffel
 */ 

#include "pins.h"

void pin_init(DPin* dpin, volatile uint8_t* reg, uint8_t mask) {
	dpin->pin = mask;
	dpin->reg = reg;
	dpin->dbounce = 0;
}

uint8_t pin_psd(DPin* dpin) {
	if (dpin->dbounce == 0) {						// was not pressed
		if ((*dpin->reg &= dpin->pin) == 0) {
			dpin->dbounce++;
		}
	}
	else if (dpin->dbounce < PINS_DEBOUNCE_CYCLES) {				// was somehow pressed
		if ((*dpin->reg &= dpin->pin) == 0) {
			dpin->dbounce++;
		} else {
			dpin->dbounce--;
		}
	
		if (dpin->dbounce == PINS_DEBOUNCE_CYCLES) {
			return 1;
		}
	}
	else {	// 0xFF									// accepted
		if (((*dpin->reg) &= dpin->pin) > 0) {
			dpin->dbounce--;
		}
	}
	
	return 0;
}

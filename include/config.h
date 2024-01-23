/*
 * config.h
 *
 * Created: 06.01.2024 10:55:07
 *  Author: waldwuffel
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#include "fifo.h"

#define S_WAITING		0x00
#define S_READY			0x01
#define S_SVC_COUNT		0x02
#define S_SVC_TIMEOUT	0x03
#define S_SVC_NAME		0x04
#define S_SVC_STATE		0x05
#define S_SVC_VIEWTIME	0x06

void config_init(uint8_t *statevar) {
	*statevar = S_WAITING;	
}

uint8_t config_waiting(FiFo* f, uint8_t val) {
	
	if (fifo_peek(&rb, buffer, 2) < 2) 
	
	if (strncmp(buffer, "@:", 2) == 0) {
		uart_putln("Header recognized. Wait for command.");
		_ser_state = header;
		fifo_skip(&rb, 2);
		} else {
		fifo_skip(&rb, 1);
	}
		
}

uint8_t config_ready(FiFo* f,  uint8_t val) {
	
}

uint8_t config_svcCount(FiFo* f, uint8_t val) {
	
}

uint8_t config_svcState(FiFo* f, uint8_t val) {
	
}

void config_nextstate(FiFo* f, uint8_t *statevar) {

	uint8_t s = (*statevar) >> 4;
	uint8_t v = (*statevar) & 0x0F;

	switch (s) {
		
		case S_WAITING:
			*statevar = config_waiting(f, v);
			break;

		case S_READY:
			*statevar = config_ready(f, v);
			break;

		case S_SVC_COUNT:
			*statevar = config_svcCount(f, v);
			break;

		case S_SVC_TIMEOUT:
			*statevar = config_svcTimeout(f, v);
			break;

		case S_SVC_NAME:
			*statevar = config_svcName(f, v);
			break;

		case S_SVC_STATE:
			*statevar = config_svcState(f, v);
			break;
		
		case S_SVC_VIEWTIME:
			*statevar = config_svcViewTime(f, v);
			break;

	}

}



#endif /* CONFIG_H_ */
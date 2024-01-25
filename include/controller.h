/*
 * controller.h
 *
 * Created: 07.01.2024 11:53:17
 *  Author: ravensnight
 */ 


#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <avr/io.h>

#define CTRL_MAX_PAGECOUNT		20
#define CTRL_MAX_NAMELEN		14
#define CTRL_MAX_STATECOUNT		5
#define CTRL_MAX_STATELEN		5

#define CTRL_DEFAULT_SCREENTIME		3	
#define CTRL_DEFAULT_SVCTIMEOUT		60
#define CTRL_DEFAULT_LIGHTTIME		40
#define CTRL_DEFAULT_MODE			0
	
typedef enum _CtrlLight {
	light_off = 0, light_on = 1, light_blink = 2
} CtrlLight;
	
typedef enum _CtrlState {
	undefined = 0,
	okay = 1,
	invalid = 2,
	warning = 3,
	error = 4
} CtrlState;

typedef struct _CtrlEntry {
	char name[CTRL_MAX_NAMELEN + 1];	
	char value[CTRL_MAX_STATELEN + 1];
	volatile CtrlState	state;
	volatile uint16_t	expiry;
} CtrlEntry;

typedef struct _CtrlData {
	
	volatile CtrlState	stateOverall;		
	CtrlEntry	entries[CTRL_MAX_PAGECOUNT];
		
	volatile uint8_t	pageNext;
	volatile uint8_t	pageCurrent;		
	volatile uint8_t	cntScreenTime;
	volatile uint8_t	cntLightTime;

	uint8_t		maxPages;
	uint16_t	maxValidTime;
	uint8_t		maxScreenTime;
	uint8_t		maxLightTime;
	uint8_t		mode;
	
} CtrlData;

/**
 * Reset all data using configured max settings.
 */
void ctrl_reset(CtrlData* d);

/**
 * Initialize the whole configuration and reset all volatile data
 */
void ctrl_init(CtrlData* d);

void ctrl_configScreenTime(CtrlData* d, uint8_t time);

/**
 * Modes:
 * 0 .. default mode
 * 1 .. night mode / no blinking
 */
void ctrl_configMode(CtrlData* d, uint8_t mode);

void ctrl_configNumPages(CtrlData* d, uint8_t num);

void ctrl_configServiceExpiry(CtrlData* d, uint16_t expiry);

uint8_t ctrl_configServiceName(CtrlData* d, uint8_t sid, const char* buffer, uint8_t len);

/**
 * Switch to a next page
 */
void ctrl_pageNext(CtrlData* d);
/**
 * Return 1 if a page update is required, otherwise 0
 */
uint8_t ctrl_pageNeedUpdate(CtrlData* d);

/**
 * Showing page
 */
void ctrl_pageShowing(CtrlData* d, CtrlState overallState);

/**
 * Reduce the expiry by a given value. In case the value is expired, its state is changed to invalid.
 */
void ctrl_serviceExpiry(CtrlData* d, uint8_t entry, uint8_t reduceBy);

void ctrl_serviceState(CtrlData* d, uint8_t entry, uint8_t val);

void ctrl_serviceStateValue(CtrlData* d, uint8_t entry, const char* buffer, uint8_t len);

/**
 * Check, if light is still active.
 */
CtrlLight ctrl_lightState(CtrlData* d);

/**
 * Start the light
 */
void ctrl_lightActivate(CtrlData* d);

/**
 * Next clock tick happened.
 */
void ctrl_tick(CtrlData* d);

#endif /* CONTROLLER_H_ */
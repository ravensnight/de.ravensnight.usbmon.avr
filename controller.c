/*
 * controller.c
 *
 * Created: 07.01.2024 11:53:31
 *  Author: waldwuffel
 */ 

#include "controller.h"

#include <stdio.h>
#include <string.h>

/**
 * Reset all data using configured max settings.
 */
void ctrl_reset(CtrlData* d) {
	d->stateOverall = undefined;
	d->pageCurrent = d->maxPages;
	d->pageNext = 0;

	d->cntScreenTime = d->maxScreenTime;	// current page time is running.
	d->cntLightTime = 0;					// light is of by default
	
	for (uint8_t i = 0; i < CTRL_MAX_PAGECOUNT; i++) {
		sprintf(d->entries[i].name, "Service %02d", i);
		d->entries[i].value[0] = '\0';		
		d->entries[i].state = undefined;
		d->entries[i].expiry = d->maxValidTime;
	}
}

/**
 * Initialize the whole configuration and reset all volatile data
 */
void ctrl_init(CtrlData* d) {
	d->maxPages = CTRL_MAX_PAGECOUNT;
	d->maxScreenTime = CTRL_DEFAULT_SCREENTIME;
	d->maxLightTime = CTRL_DEFAULT_LIGHTTIME;
	d->maxValidTime = CTRL_DEFAULT_SVCTIMEOUT;
	d->mode = CTRL_DEFAULT_MODE;
	
	ctrl_reset(d);
}

uint8_t ctrl_configServiceName(CtrlData* d, uint8_t sid, const char* buffer, uint8_t len) {
	if (sid >= CTRL_MAX_PAGECOUNT) return 0;
	
	uint8_t l = len;
	if (l > CTRL_MAX_NAMELEN) l = CTRL_MAX_NAMELEN;
	
	strncpy(d->entries[sid].name, buffer, l);
	d->entries[sid].name[l] = '\0';
	
	return 1;
}

void ctrl_configServiceExpiry(CtrlData* d, uint16_t expiry) {
	d->maxValidTime = expiry;	
}

void ctrl_configScreenTime(CtrlData* d, uint8_t time) {
	if (time == 0) {
		d->maxScreenTime = CTRL_DEFAULT_SCREENTIME;
	} else {
		d->maxScreenTime = time;
	}
}

void ctrl_configMode(CtrlData* d, uint8_t mode) {
	d->mode = mode;
	if (d->mode > 1) {
		d->mode = CTRL_DEFAULT_MODE;	// default mode.
	}
}


void ctrl_configNumPages(CtrlData* d, uint8_t num) {	
	if ((num >= CTRL_MAX_PAGECOUNT) || (num == 0)) {
		d->maxPages = CTRL_MAX_PAGECOUNT;
	} else {
		d->maxPages = num;
	}
	
	d->pageNext = 0;
}

void ctrl_serviceExpiry(CtrlData* d, uint8_t entry, uint8_t by) {
	if (d->maxValidTime == 0) return;
	if (entry >= CTRL_MAX_PAGECOUNT) return;
		
	if (d->entries[entry].expiry > by) {
		d->entries[entry].expiry -= by;
	} else {
		d->entries[entry].expiry = 0;
		if (d->entries[entry].state == okay) {
			d->entries[entry].state = invalid;
		}
	}
}

void ctrl_serviceState(CtrlData* d, uint8_t entry, uint8_t val) {
	if (entry >= CTRL_MAX_PAGECOUNT) return;
	if (val >= CTRL_MAX_STATECOUNT) {
		d->entries[entry].state = invalid;
		d->entries[entry].expiry = 0;
	} else {
		d->entries[entry].state = val;
		d->entries[entry].expiry = d->maxValidTime;
	}	
}

void ctrl_serviceStateValue(CtrlData* d, uint8_t entry, const char* buffer, uint8_t len) {
	if (entry >= CTRL_MAX_PAGECOUNT) return;
		
	uint8_t l = len;
	if (l > CTRL_MAX_STATELEN) l = CTRL_MAX_STATELEN;	
	strncpy(d->entries[entry].value, buffer, l);
	d->entries[entry].value[l] = '\0';
}


/**
 * Switch to a next page
 */
void ctrl_pageNext(CtrlData* d) {
	if (d->maxPages == 1) {
		d->pageNext = 0;
		d->pageCurrent = 1;
	} else {
		d->pageNext++;
		if (d->pageNext >= d->maxPages) {
			d->pageNext = 0;
		}
	}	
}

/**
 * Return 1 if a page update is required, otherwise 0
 */
uint8_t ctrl_pageNeedUpdate(CtrlData* d) {
	return (d->pageNext != d->pageCurrent);
}

void ctrl_pageShowing(CtrlData* d, CtrlState overallState) {
	d->stateOverall = overallState;
	d->pageCurrent = d->pageNext;
}

/**
 * Check, if light is still active.
 */
CtrlLight ctrl_lightState(CtrlData* d) {
	if (d->cntLightTime >0) {
		return light_on;
	}
	
	if ((d->maxValidTime > 0) && (d->mode == 0)) {
		if (d->stateOverall > okay) {
			return light_blink;
		}
	}
	
	return light_off;
}

/**
 * Start the light
 */
void ctrl_lightActivate(CtrlData* d) {
	d->cntLightTime = d->maxLightTime;
}

/**
 * Next clock tick happened.
 */
void ctrl_tick(CtrlData* d) {	
	
	// page timer
	if (d->cntScreenTime > 0) d->cntScreenTime--;
	if (d->cntScreenTime == 0) {					
		d->cntScreenTime = d->maxScreenTime;
		ctrl_pageNext(d);
	}	
	
	// light timer
	if (d->cntLightTime > 0) d->cntLightTime--;
}


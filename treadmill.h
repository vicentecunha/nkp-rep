/*
  interface-rev2: treadmill.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _treadmill_h_
#define _treadmill_h_

#include "leds.h"
#include "switches.h"
#include "utils.h"

/*==================================================*/
/* TYPEDEFS                                         */
/*==================================================*/

typedef struct treadmill_t{
	treadmill_e treadmill;
	unsigned int speed_kmph_times10;
	unsigned int inclination;
	unsigned int targetSpeed_kmph_times10;
	unsigned int targetInclination;
	unsigned int maxSpeed_kmph_times10;
	unsigned int maxInclination;
	bool isRunning;
	unsigned long partialMicros;
	unsigned long totalSeconds;
	unsigned long totalDistance_km;
	unsigned char dac;
	leds_t* leds;
	unsigned char inclinationEncoder;
	pulseConstants_t* pulseConstants;
	int acc;
} treadmill_t;

/*==================================================*/
/* PUBLIC FUNCTIONS																	*/
/*==================================================*/

treadmill_t treadmill_init(switches_t*, leds_t*);
void treadmill_update(treadmill_t*);
void treadmill_stop(treadmill_t*);
void treadmill_setDac(treadmill_t*, unsigned char);
void treadmill_setInclinationUp(treadmill_t*, bool);
void treadmill_setInclinationDown(treadmill_t*, bool);
void treadmill_resetInclination(treadmill_t*);
void treadmill_settleSpeed(treadmill_t*);
void treadmill_speedUp(treadmill_t*);
void treadmill_speedDown(treadmill_t*);
void treadmill_setEmergencyStop(treadmill_t*, bool);
void treadmill_resetSpeed(treadmill_t*);
void treadmill_updateSpeed(treadmill_t*);
void treadmill_updateAcc(treadmill_t*);

#endif

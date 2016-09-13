/*
  interface-rev2: timer0.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _timer0_h_
#define _timer0_h_

#include "utils.h"

/*==================================================*/
/* TYPEDEFS                                         */
/*==================================================*/

typedef enum timer_configuration_e {
	TIMER,
	COUNTER
} timer_configuration_e;

typedef enum timer_mode_e {
	_13b,
	_16b,
	_8b_AUTORELOAD,
	BAUDRATE_GENERATOR,
	HALTED
} timer_mode_e;

typedef struct timer0_t {
	bool isRunning;
	bool timerGating;
	timer_configuration_e configuration;
	timer_mode_e timerMode;
	unsigned long overflowPeriod_us;
	unsigned int overflowCounts;
	unsigned int reloadValue;
} timer0_t;

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

timer0_t timer0_init(
	timer_configuration_e configuration,
	timer_mode_e timerMode,
	unsigned long overflowPeriod_us,
	bool timerGating
);

void timer0_startTimer(timer0_t*);
void timer0_stopTimer(timer0_t*);
void timer0_waitMicros(timer0_t*, long micros);
	
/*==================================================*/
/* EXTERNABLE VARIABLES                             */
/*==================================================*/

extern volatile bool timer0_flag;

#endif
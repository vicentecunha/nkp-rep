/*
  interface-rev2: timer1.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _timer1_h_
#define _timer1_h_

#include "utils.h"
#include "timer0.h"

/*==================================================*/
/* TYPEDEFS                                         */
/*==================================================*/

typedef struct timer1_t {
	bool isRunning;
	bool timerGating;
	timer_configuration_e configuration;
	timer_mode_e timerMode;
	unsigned long overflowPeriod_us;
	unsigned int overflowCounts;
	unsigned int reloadValue;
} timer1_t;

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

timer1_t timer1_init(
	timer_configuration_e configuration,
	timer_mode_e timerMode,
	unsigned long overflowPeriod_us,
	unsigned long baudrate,
	bool timerGating
);

void timer1_startTimer(timer1_t*);
		
#endif
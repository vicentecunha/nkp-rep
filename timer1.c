/*
  interface-rev2: timer1.c
  Author: Vicente Cunha
  Date: August 2016
*/

#include "timer1.h"
#include <REG51.H>

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

timer1_t timer1_init(
	timer_configuration_e configuration,
	timer_mode_e timerMode,
	unsigned long overflowPeriod_us,
	unsigned long baudrate,
	bool timerGating)
{
	timer1_t myTimer;

	myTimer.isRunning = false;
	myTimer.configuration = configuration;
	myTimer.timerMode = timerMode;
	myTimer.timerGating = timerGating;

	/* AUTORELOAD VALUES */

	switch (timerMode) {
		case _13b:
			myTimer.overflowPeriod_us = overflowPeriod_us;
			myTimer.overflowCounts = myTimer.overflowPeriod_us*XTAL_FREQ_MHz/12;
			myTimer.reloadValue = 0x2000 - myTimer.overflowCounts;
			break;
		case _16b:
			myTimer.overflowPeriod_us = overflowPeriod_us;
			myTimer.overflowCounts = myTimer.overflowPeriod_us*XTAL_FREQ_MHz/12;
			myTimer.reloadValue = 0x10000 - myTimer.overflowCounts;
			break;
		case _8b_AUTORELOAD:
			myTimer.overflowPeriod_us = overflowPeriod_us;
			myTimer.overflowCounts = myTimer.overflowPeriod_us*XTAL_FREQ_MHz/12;
			myTimer.reloadValue = 0x100 - myTimer.overflowCounts;
			break;
		case BAUDRATE_GENERATOR:
			myTimer.overflowCounts = (XTAL_FREQ_Hz/384)/baudrate;
			myTimer.reloadValue = 0x100 - myTimer.overflowCounts;
			myTimer.overflowPeriod_us = myTimer.overflowCounts*12/XTAL_FREQ_MHz;
			break;
		default:
			myTimer.overflowCounts = 0x10000;
			myTimer.overflowPeriod_us = myTimer.overflowCounts*12/XTAL_FREQ_MHz;
			myTimer.reloadValue = 0x00;
			break;
	}

	/* TIMER GATING */

	if (timerGating) TMOD |=  (1 << 7);
	else             TMOD &= ~(1 << 7);

	/* TIMER CONFIGURATION */

	switch (configuration) {
		case TIMER:   TMOD &= ~(1 << 6); break;
		case COUNTER: TMOD |=  (1 << 6); break;
	}

	/* TIMER MODE */

	switch (timerMode) {
		case _13b:
			TMOD &= ~((1 << 5)|(1 << 4));
			TH1 = myTimer.reloadValue >> 8;
			TL1 = myTimer.reloadValue;
			break;
		case _16b:
			TMOD &= ~(1 << 5);
			TMOD |=  (1 << 4);
			TH1 = myTimer.reloadValue >> 8;
			TL1 = myTimer.reloadValue;
			break;
		case _8b_AUTORELOAD:
			TMOD |=  (1 << 5);
			TMOD &= ~(1 << 4);
			TH1 = myTimer.reloadValue;
			TL1 = myTimer.reloadValue;
			break;
		case BAUDRATE_GENERATOR:
			TMOD |=  (1 << 5);
			TMOD &= ~(1 << 4);
			TH1 = myTimer.reloadValue;
			TL1 = myTimer.reloadValue;
			break;
		case HALTED:
			TMOD |= ((1 << 5)|(1 << 4));
			break;
	}

	return myTimer;
}

void timer1_startTimer(timer1_t* myTimer)
{
	if (myTimer->isRunning) return; //timer already running
	TR1 = 1;
	myTimer->isRunning = true;
}
/*
void timer1_stopTimer(timer1_t* myTimer)
{
	if (!(myTimer->isRunning)) return; //timer already stopped
	TR1 = 0;
	myTimer->isRunning = false;
}
*/
/*
void timer1_waitMicros(timer0_t* myTimer, long micros)
{
	timer1_startTimer(myTimer);
	while (micros > 0) {
		if (TF1) {
			micros -= myTimer->overflowPeriod_us;
			TF1 = 0;
		}
	}
	timer1_stopTimer(myTimer);
}
*/
/*
unsigned int timer1_getCounts(timer1_t* myTimer)
{
	switch (myTimer->timerMode) {
		case _13b: return TL1 + (TH1 << 5);
		case _16b: return TL1 + (TH1 << 8);
		case _8b_AUTORELOAD: return TL1;
		default: return 0;
	}
}
*/

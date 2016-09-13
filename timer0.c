/*
  interface-rev2: timer0.c
  Author: Vicente Cunha
  Date: August 2016
*/

#include "main.h"
#include "timer0.h"
#include <REG51.H>

/*==================================================*/
/* INTERRUPTS                                       */
/*==================================================*/

volatile bool timer0_flag = false;
volatile int accUpdater = 0;

void timer0() interrupt 1
{
	TH0 = myTimer0.reloadValue >> 8;
	TL0 = myTimer0.reloadValue;
	timer0_flag = true;

	treadmill_updateSpeed(&myTreadmill);
	if (++accUpdater >= 2) {
		treadmill_updateAcc(&myTreadmill);
		accUpdater = 0;
	}

	mySerialParser.timeout_us -=
		(mySerialParser.state != IDLE) ? myTimer0.overflowPeriod_us : 0;
	mySerialParser.discTimeout_us -=
		(mySerialParser.commDiscStop) ? myTimer0.overflowPeriod_us : 0;

	if (mySerialParser.timeout_us < 0 || mySerialParser.discTimeout_us < 0) {
		treadmill_stop(&myTreadmill);
		mySerialParser.state = IDLE;
	}

	if (myTreadmill.isRunning) {
		myTreadmill.partialMicros += myTimer0.overflowPeriod_us;
		if (myTreadmill.partialMicros >= 1000000) {
			myTreadmill.partialMicros = 0;
			myTreadmill.totalSeconds++;
		}
	}
	TF0 = 0;
}

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

timer0_t timer0_init(
	timer_configuration_e configuration,
	timer_mode_e timerMode,
	unsigned long overflowPeriod_us,
	bool timerGating)
{
	timer0_t myTimer;

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
		default:
			myTimer.overflowCounts = 0x10000;
			myTimer.overflowPeriod_us = myTimer.overflowCounts*12/XTAL_FREQ_MHz;
			myTimer.reloadValue = 0x00;
			break;
	}

	/* TIMER GATING */

	if (timerGating) TMOD |=  (1 << 3);
	else             TMOD &= ~(1 << 3);

	/* TIMER CONFIGURATION */

	switch (configuration) {
		case TIMER:   TMOD &= ~(1 << 2); break;
		case COUNTER: TMOD |=  (1 << 2); break;
	}

	/* TIMER MODE */

	switch (timerMode) {
		case _13b:
			TMOD &= ~((1 << 1)|(1 << 0));
			TH0 = myTimer.reloadValue >> 8;
			TL0 = myTimer.reloadValue;
			break;
		case _16b:
			TMOD &= ~(1 << 1);
			TMOD |=  (1 << 0);
			TH0 = myTimer.reloadValue >> 8;
			TL0 = myTimer.reloadValue;
			break;
		case _8b_AUTORELOAD:
			TMOD |=  (1 << 1);
			TMOD &= ~(1 << 0);
			TH0 = myTimer.reloadValue;
			TL0 = myTimer.reloadValue;
			break;
		case HALTED:
			TMOD |= ((1 << 1)|(1 << 0));
			break;
	}

	return myTimer;
}

void timer0_startTimer(timer0_t* myTimer)
{
	if (myTimer->isRunning) return; //timer already running
	TR0 = 1;
	myTimer->isRunning = true;
}

void timer0_stopTimer(timer0_t* myTimer)
{
	if (!(myTimer->isRunning)) return; //timer already stopped
	TR0 = 0;
	myTimer->isRunning = false;
}

void timer0_waitMicros(timer0_t* myTimer, long micros)
{
	timer0_startTimer(myTimer);
	while (micros > 0) {
		if (TF0) {
			micros -= myTimer->overflowPeriod_us;
			TF0 = 0;
		}
	}
	timer0_stopTimer(myTimer);
}

/*
unsigned int timer0_getCounts(timer0_t* myTimer)
{
	switch (myTimer->timerMode) {
		case _13b: return TL0 + (TH0 << 5);
		case _16b: return TL0 + (TH0 << 8);
		case _8b_AUTORELOAD: return TL0;
		default: return 0;
	}
}
*/

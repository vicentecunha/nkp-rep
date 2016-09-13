/*
  interface-rev2: treadmill.c
  Author: Vicente Cunha
  Date: August 2016
*/

#include "interrupts.h"
#include "treadmill.h"
#include <REG51.h>

xdata unsigned char DAC _at_ 0x9000;

treadmill_t treadmill_init(switches_t* mySwitches, leds_t* leds)
{
	treadmill_t myTreadmill;

	myTreadmill.treadmill = mySwitches->treadmill;
	myTreadmill.speed_kmph_times10 = 0;
	myTreadmill.inclination = 0;
	myTreadmill.targetSpeed_kmph_times10 = 0;
	myTreadmill.targetInclination = 0;
	myTreadmill.maxSpeed_kmph_times10 = mySwitches->maxSpeed_kmph_times10;
	myTreadmill.maxInclination = 260;
	myTreadmill.isRunning = false;
	myTreadmill.partialMicros = 0;
	myTreadmill.totalSeconds = 0;
	myTreadmill.dac = 0x00;
	myTreadmill.leds = leds;
	myTreadmill.inclinationEncoder = (P1 & 0x01);
	myTreadmill.pulseConstants = &(mySwitches->pulseConstants);
	myTreadmill.acc = 0;

	return myTreadmill;
}

void treadmill_resetSpeed(treadmill_t* myTreadmill)
{
	myTreadmill->targetSpeed_kmph_times10 = 0;
	if (!(myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_12kmph ||
			myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_16kmph_OU_24kmph ||
			myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_18kmph)) {
		treadmill_setDac(myTreadmill, 0x00);
		myTreadmill->speed_kmph_times10 = 0;
	}
}

void treadmill_updateSpeed(treadmill_t* myTreadmill)
{
	myTreadmill->speed_kmph_times10 =
		(speedCounter*30)/(myTreadmill->pulseConstants->numberOfPulses_kmph);
	speedCounter = 0;
}

void treadmill_updateAcc(treadmill_t* myTreadmill)
{
	if (myTreadmill->targetSpeed_kmph_times10 > myTreadmill->speed_kmph_times10)
		myTreadmill->acc += 1;
	else if (myTreadmill->targetSpeed_kmph_times10 < myTreadmill->speed_kmph_times10)
		myTreadmill->acc -= 1;
}

void treadmill_setEmergencyStop(treadmill_t* myTreadmill, bool b)
{
	if (b) {
		leds_turnLedOn(myTreadmill->leds, PROT);
		myTreadmill->isRunning = false;
		if (myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_12kmph ||
				myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_16kmph_OU_24kmph ||
				myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_18kmph) {
			P1 |= (1 << 3);
		} else P1 &= ~(1 << 3);
	} else {
		leds_turnLedOff(myTreadmill->leds, PROT);
		if (myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_12kmph ||
				myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_16kmph_OU_24kmph ||
				myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_18kmph) {
			P1 &= ~(1 << 3);
		} else P1 |= (1 << 3);
	}
}

void treadmill_setDac(treadmill_t* myTreadmill, unsigned char dac)
{
	myTreadmill->dac = dac;
	DAC = myTreadmill->dac;
}

void treadmill_settleSpeed(treadmill_t* myTreadmill)
{
	treadmill_setDac(myTreadmill, 0xFF);
	leds_turnLedOff(myTreadmill->leds, VEL_UP);
	leds_turnLedOff(myTreadmill->leds, VEL_DOWN);
}

void treadmill_speedUp(treadmill_t* myTreadmill)
{
	treadmill_setDac(myTreadmill, 0xFD);
	leds_turnLedOff(myTreadmill->leds, VEL_DOWN);
	leds_turnLedOn(myTreadmill->leds, VEL_UP);
}

void treadmill_speedDown(treadmill_t* myTreadmill)
{
	treadmill_setDac(myTreadmill, 0xFE);
	leds_turnLedOff(myTreadmill->leds, VEL_UP);
	leds_turnLedOn(myTreadmill->leds, VEL_DOWN);
}

void treadmill_update(treadmill_t* myTreadmill)
{
	unsigned char inclinationEncoder;
	int auxInt;

	/* UPDATE SPEED */

	if (!myTreadmill->isRunning) treadmill_resetSpeed(myTreadmill);
	if (myTreadmill->speed_kmph_times10 > myTreadmill->targetSpeed_kmph_times10 + 5) {
		if (myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_12kmph ||
				myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_16kmph_OU_24kmph ||
				myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_18kmph) {
			treadmill_speedDown(myTreadmill);
			while (myTreadmill->targetSpeed_kmph_times10 < myTreadmill->speed_kmph_times10);
			treadmill_settleSpeed(myTreadmill);
		} else if (myTreadmill->speed_kmph_times10 > myTreadmill->acc) {
			leds_turnLedOn(myTreadmill->leds, VEL_DOWN);
			auxInt = ((myTreadmill->speed_kmph_times10 - myTreadmill->acc)/4) + 1;
			if (auxInt > myTreadmill->dac) treadmill_setDac(myTreadmill, 0x00);
			else treadmill_setDac(myTreadmill, myTreadmill->dac - auxInt);
		} else leds_turnLedOff(myTreadmill->leds, VEL_DOWN);
	} else if (myTreadmill->targetSpeed_kmph_times10 > myTreadmill->speed_kmph_times10 + 5) {
		if (myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_12kmph ||
				myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_16kmph_OU_24kmph ||
				myTreadmill->treadmill == ESTEIRA_COM_INVERSOR_18kmph) {
			treadmill_speedUp(myTreadmill);
			while (myTreadmill->targetSpeed_kmph_times10 > myTreadmill->speed_kmph_times10);
			treadmill_settleSpeed(myTreadmill);
		} else if (myTreadmill->acc > myTreadmill->speed_kmph_times10) {
			leds_turnLedOn(myTreadmill->leds, VEL_UP);
			auxInt = ((myTreadmill->acc - myTreadmill->speed_kmph_times10)/4) + 1;
			if (myTreadmill->treadmill == WEG_SUPER_ATL_30kmph && (auxInt + myTreadmill->dac) > 511)
				treadmill_setDac(myTreadmill, 511);
			else if ((auxInt + myTreadmill->dac) > 255)
				treadmill_setDac(myTreadmill, 255);
			else treadmill_setDac(myTreadmill, myTreadmill->dac + auxInt);
		} else leds_turnLedOff(myTreadmill->leds, VEL_UP);
	}

	/* UPDATE INCLINATION */

	if ((!(myTreadmill->isRunning) || myTreadmill->targetInclination == 0) &&
		!(P1 & (1 << 1))) treadmill_resetInclination(myTreadmill);
	else if (myTreadmill->targetInclination > myTreadmill->inclination) {
		treadmill_setInclinationUp(myTreadmill, true);
		while (myTreadmill->targetInclination > myTreadmill->inclination) {
			inclinationEncoder = (P1 & 0x01);
			if (myTreadmill->inclinationEncoder != inclinationEncoder) {
				myTreadmill->inclinationEncoder = inclinationEncoder;
				if (myTreadmill->inclinationEncoder) myTreadmill->inclination += 1;
			}
		}
		treadmill_setInclinationUp(myTreadmill, false);
	} else if (myTreadmill->targetInclination < myTreadmill->inclination) {
		treadmill_setInclinationDown(myTreadmill, true);
		while (myTreadmill->targetInclination < myTreadmill->inclination) {
			inclinationEncoder = (P1 & 0x01);
			if (myTreadmill->inclinationEncoder != inclinationEncoder) {
				myTreadmill->inclinationEncoder = inclinationEncoder;
				if (myTreadmill->inclinationEncoder) myTreadmill->inclination -= 1;
			}
		}
		treadmill_setInclinationDown(myTreadmill, false);
	}
}

void treadmill_stop(treadmill_t* myTreadmill)
{
	myTreadmill->isRunning = false;
	myTreadmill->targetSpeed_kmph_times10 = 0;
	myTreadmill->targetInclination = 0;
}

void treadmill_setInclinationUp(treadmill_t* myTreadmill, bool b)
{
	if (b) {
		P1 |=  (1 << 6); // disable inclination down
		P1 &= ~(1 << 5); // enable  inclination up
		leds_turnLedOff(myTreadmill->leds, INC_DOWN);
		leds_turnLedOn (myTreadmill->leds, INC_UP);
	} else {
		P1 |= (1 << 5); // disable inclination up
		leds_turnLedOff(myTreadmill->leds, INC_UP);
	}
}

void treadmill_setInclinationDown(treadmill_t* myTreadmill, bool b)
{
	if (b) {
		P1 |=  (1 << 5); // disable inclination up
		P1 &= ~(1 << 6); // enable  inclination down
		leds_turnLedOff(myTreadmill->leds, INC_UP);
		leds_turnLedOn(myTreadmill->leds, INC_DOWN);
	} else {
		P1 |= (1 << 6); // disable inclination down
		leds_turnLedOff(myTreadmill->leds, INC_DOWN);
	}
}

void treadmill_resetInclination(treadmill_t* myTreadmill)
{
	unsigned int debouncer = 0;
	myTreadmill->targetInclination = 0;
	treadmill_setInclinationDown(myTreadmill, true);
	while (debouncer < 4) {
		if (P1 & (1 << 1)) debouncer++;
		else debouncer = 0;
	}
	treadmill_setInclinationDown(myTreadmill, false);
	myTreadmill->inclination = 0;
}

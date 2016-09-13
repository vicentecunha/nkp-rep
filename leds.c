/*
  interface-rev2: leds.c
  Author: Vicente Cunha
  Date: August 2016
*/

#include "leds.h"
#include "interrupts.h"
#include <REG51.H>

xdata	unsigned char	LEDS_OUTPUT _at_ 0x8100;

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

void leds_turnLedOff(leds_t* myLeds, leds_e led)
{
	myLeds->state |= led;
	LEDS_OUTPUT = myLeds->state;
}

void leds_turnLedOn(leds_t* myLeds, leds_e led)
{
	myLeds->state &= ~led;
	LEDS_OUTPUT = myLeds->state;
}

leds_t leds_init()
{
	leds_t myLeds;
	
	myLeds.ledsArray[0] = CPU;
	myLeds.ledsArray[1] = COMM;
	myLeds.ledsArray[2] = VEL_UP;
	myLeds.ledsArray[3] = VEL_DOWN;
	myLeds.ledsArray[4] = INC_UP;
	myLeds.ledsArray[5] = INC_DOWN;
	myLeds.ledsArray[6] = PROT;
	
	myLeds.state = 0xFF; // turn off all leds
	LEDS_OUTPUT = myLeds.state ;
	
	return myLeds;
}

void leds_test(leds_t* myLeds, timer0_t* myTimer)
{
	int i = 0;

	for (i = 0; i < 7; i++) {
		leds_turnLedOn(myLeds, myLeds->ledsArray[i]);
		timer0_waitMicros(myTimer, 250000);
	}

	for (i = 0; i < 7; i++) {
		leds_turnLedOff(myLeds, myLeds->ledsArray[i]);
		timer0_waitMicros(myTimer, 250000);
	}
}

void leds_showNibble(leds_t* myLeds, unsigned char nibble)
{
	int i = 0;	
	for (i = 0; i < 4; i++) {
		if (nibble & (1 << i)) leds_turnLedOn (myLeds, myLeds->ledsArray[6-i]);
		else	                 leds_turnLedOff(myLeds, myLeds->ledsArray[6-i]);
	}
}
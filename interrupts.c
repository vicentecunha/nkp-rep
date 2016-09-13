/*
  interface-rev2: interrupts.c
  Author: Vicente Cunha
  Date: August 2016
*/

#include "interrupts.h"
#include <REG51.H>

/*==================================================*/
/* INTERRUPTS                                       */
/*==================================================*/

volatile unsigned int speedCounter = 0;

void int0() interrupt 0
{
	speedCounter++;
}

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

/*
void interrupts_disableInterrupt(interrupts_e myInterrupt)
{
	IE &= ~myInterrupt;
}
*/

void interrupts_enableInterrupt(interrupts_e myInterrupt)
{
	IE |= myInterrupt;
}

void interrupts_setTriggerType(
	interrupts_e myInterrupt,
	interrupts_triggerType_e myTriggerType)
{
	switch (myInterrupt) {
		case EXTERNAL0:
			if (myTriggerType == LEVEL) IT0 = 0;
			else IT0 = 1;
			break;
		case EXTERNAL1:
			if (myTriggerType == LEVEL) IT1 = 0;
			else IT1 = 1;
			break;
		default: // do nothing
			break;
	}
}

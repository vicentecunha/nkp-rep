/*
  interface-rev2: interrupts.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _interrupts_h_
#define _interrupts_h_

/*==================================================*/
/* TYPEDEFS                                         */
/*==================================================*/

typedef enum interrupts_e {
	GLOBAL    = 0x80,
	SERIAL    = 0x10,
	TIMER1    = 0x08,
	EXTERNAL1 = 0x04,
	TIMER0    = 0x02,
	EXTERNAL0 = 0x01
} interrupts_e;

typedef enum interrupts_triggerType_e {
	LEVEL,
	EDGE
} interrupts_triggerType_e;

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

void interrupts_disableInterrupt(interrupts_e);
void interrupts_enableInterrupt(interrupts_e);
void interrupts_setTriggerType(interrupts_e, interrupts_triggerType_e);

/*==================================================*/
/* EXTERNABLE VARIABLES															*/
/*==================================================*/

extern volatile unsigned int speedCounter;

#endif

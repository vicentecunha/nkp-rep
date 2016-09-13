/*
  interface-rev2: leds.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _leds_h_
#define _leds_h_

#include "timer0.h"

/*==================================================*/
/* TYPEDEFS                                         */
/*==================================================*/

typedef enum leds_e {
	CPU      = 0x40,
	COMM     = 0x20,
	VEL_UP   = 0x01,
	VEL_DOWN = 0x02,
	INC_UP   = 0x04,
	INC_DOWN = 0x08,
	PROT     = 0x10
} leds_e;

typedef struct leds_t {
	leds_e ledsArray[7];
	unsigned char state;
} leds_t;

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

leds_t leds_init();
void leds_turnLedOff(leds_t*, leds_e);
void leds_turnLedOn(leds_t*, leds_e);
void leds_test(leds_t*, timer0_t*);
void leds_showNibble(leds_t*, unsigned char);

#endif

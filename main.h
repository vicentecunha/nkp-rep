/*
  interface-rev2: main.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _main_h_
#define _main_h_

#include "timer0.h"
#include "timer1.h"
#include "switches.h"
#include "leds.h"
#include "serial.h"
#include "serialParser.h"
#include "treadmill.h"

extern volatile timer0_t myTimer0;
extern volatile timer1_t myTimer1; // baudrate generator
extern volatile switches_t mySwitches;
extern volatile leds_t myLeds;
extern volatile serial_t mySerial;
extern volatile serialParser_t mySerialParser;
extern volatile treadmill_t myTreadmill;

#endif

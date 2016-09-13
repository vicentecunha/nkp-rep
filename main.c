/*
  interface-rev2: main.c
  Author: Vicente Cunha
  Date: August 2016
*/

#include "main.h"
#include "interrupts.h"
#include "utils.h"
#include <REG51.H>

volatile timer0_t myTimer0;
volatile timer1_t myTimer1; // baudrate generator
volatile switches_t mySwitches;
volatile leds_t myLeds;
volatile serial_t mySerial;
volatile serialParser_t mySerialParser;
volatile treadmill_t myTreadmill;

void main()
{
	unsigned int ledCounter;

	mySwitches = switches_init();
	myLeds = leds_init();
	myTimer0 = timer0_init(TIMER, _16b, 100000, false);
	if ((mySwitches.protocol == 1) ||
		((mySwitches.protocol >= 6) && (mySwitches.protocol <= 9)))
		myTimer1 = timer1_init(TIMER, BAUDRATE_GENERATOR, 0, 9600, false);    // baudrate: 9600
	else myTimer1 = timer1_init(TIMER, BAUDRATE_GENERATOR, 0, 4800, false); // baudrate: 4800

	mySerial = serial_init(_8b_UART, false);
	mySerialParser = serialParser_init(mySwitches.protocol);
	myTreadmill = treadmill_init(&mySwitches, &myLeds);

	if (myTreadmill.treadmill == ESTEIRA_COM_INVERSOR_12kmph ||
			myTreadmill.treadmill == ESTEIRA_COM_INVERSOR_16kmph_OU_24kmph ||
			myTreadmill.treadmill == ESTEIRA_COM_INVERSOR_18kmph)
		treadmill_settleSpeed(&myTreadmill);

	treadmill_resetSpeed(&myTreadmill);
	treadmill_setEmergencyStop(&myTreadmill, false);
	if (mySwitches.treadmill == WEG_SUPER_ATL_30kmph) P3 &= ~(1 << 1);
	treadmill_setInclinationUp(&myTreadmill, false);
	treadmill_setInclinationDown(&myTreadmill, false);
	treadmill_resetInclination(&myTreadmill);

	leds_test(&myLeds, &myTimer0);
	leds_showNibble(&myLeds, mySwitches.treadmill);
	timer0_waitMicros(&myTimer0, 500000);
	leds_showNibble(&myLeds, mySwitches.protocol);
	timer0_waitMicros(&myTimer0, 500000);
	leds_showNibble(&myLeds, 0x00);

	interrupts_setTriggerType(EXTERNAL0, EDGE);
	interrupts_enableInterrupt(EXTERNAL0);
	interrupts_enableInterrupt(TIMER0);
	interrupts_enableInterrupt(SERIAL);
	interrupts_enableInterrupt(GLOBAL);
	timer0_startTimer(&myTimer0);
	timer1_startTimer(&myTimer1);
	serial_enableRX();

	ledCounter = 0;
	while(true) {
		treadmill_update(&myTreadmill);

		//speed debug:
		/*
		serial_sendChar('S');
		serial_sendChar(myTreadmill.speed_kmph_times10/100 + '0');
		serial_sendChar((myTreadmill.speed_kmph_times10%100)/10 + '0');
		serial_sendChar(myTreadmill.speed_kmph_times10%10 + '0');
		serial_sendChar('\r');
		serial_sendChar('\n');
		serial_sendChar('T');
		serial_sendChar(myTreadmill.targetSpeed_kmph_times10/100 + '0');
		serial_sendChar((myTreadmill.targetSpeed_kmph_times10%100)/10 + '0');
		serial_sendChar(myTreadmill.targetSpeed_kmph_times10%10 + '0');
		serial_sendChar('\r');
		serial_sendChar('\n');
		*/
		
		serial_sendChar(0xFF);
		serial_sendChar(0xFF);

		if (timer0_flag) {
			if (++ledCounter >= 5) {
				ledCounter = 0;
				if (myLeds.state & CPU) leds_turnLedOn(&myLeds, CPU);
				else leds_turnLedOff(&myLeds, CPU);
			}
			timer0_flag = false;
		}

		if (serialFlag) {
			leds_turnLedOn(&myLeds, COMM);
			serialFlag = false;
		} else leds_turnLedOff(&myLeds, COMM);
	}
}

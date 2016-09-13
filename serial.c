/*
  interface-rev2: serial.c
  Author: Vicente Cunha
  Date: August 2016
*/

#include "serial.h"
#include "REG51.h"
#include "main.h"

/*==================================================*/
/* INTERRUPTS                                       */
/*==================================================*/

volatile bool serialFlag = false;

void serial() interrupt 4
{
	if (RI) {
		serialParser_parse(&mySerialParser, &myTreadmill, SBUF);
		serialFlag = true;
		RI = 0;
	}
}

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

serial_t serial_init(
	serial_serialMode_e serialMode,
	bool multiprocComm)
{
	serial_t mySerial;

	mySerial.serialMode = serialMode;
	mySerial.multiprocComm = multiprocComm;

	/* SERIAL MODE */

	switch (serialMode) {
		case SHIFT_REGISTER:
			SM0 = 0;
			SM1 = 0;
			break;
		case _8b_UART:
			SM0 = 0;
			SM1 = 1;
			break;
		case _9b_UART:
			SM0 = 1;
			SM1 = 0;
			break;
		case _39b_UART:
			SM0 = 1;
			SM1 = 1;
			break;
	}

	/* MULTIPROCESSOR COMMUNICATION */

	SM2 = multiprocComm;

	return mySerial;
}

void serial_enableRX()
{
	REN = 1;
}

void serial_sendChar(char arg)
{
	SBUF = arg;
	while(!TI);
	TI = 0;
}

void serial_sendBuf(char* buf)
{
	int i = 0;
	while (buf[i] != 0) {
		serial_sendChar(buf[i]);
		i++;
	}
}

/*
void serial_sendN(char* buf, int n)
{
	int i = 0;
	for (i = 0; i < n; i++) {
		serial_sendChar(buf[i]);
	}
}
*/

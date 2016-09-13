/*
  interface-rev2: serialParser.c
  Author: Vicente Cunha
  Date: August 2016
*/

#include "serialParser.h"
#include "serial.h"

serialParser_t serialParser_init(protocol_e protocol)
{
	int i;
	serialParser_t mySerialParser;

	mySerialParser.protocol = protocol;
	mySerialParser.state = IDLE;
	mySerialParser.bufIndex = 0;
	mySerialParser.xAckDataFlag = false;
	mySerialParser.commDiscStop = false;
	mySerialParser.imperial = false;
	for (i = 0; i < RX_BUF_LEN; i++) mySerialParser.buf[i] = 0;
	for (i = 0; i < TX_BUF_LEN; i++) mySerialParser.txBuf[i] = 0;
	mySerialParser.timeout_us = SERIAL_TIMEOUT_us;
	mySerialParser.discTimeout_us = DISCONN_TIMEOUT_us;

	return mySerialParser;
}

/*==================================================*/
/* INBRAMED                                         */
/*==================================================*/

void serialParser_inbramed(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;

	if (mySerialParser->state == IDLE) {
		if (myChar == 0xA0) {
			myTreadmill->isRunning = true;
			mySerialParser->state = PARSING;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 0;
		} else treadmill_stop(myTreadmill);
	} else if (mySerialParser->state == PARSING) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 1) {
			auxInt = ((unsigned int)(mySerialParser->buf[0])*10)/15;
			if (auxInt <= myTreadmill->maxSpeed_kmph_times10)
				myTreadmill->targetSpeed_kmph_times10 = auxInt;

			auxInt = ((unsigned int)(mySerialParser->buf[1])*10)/2;
			if (auxInt <= myTreadmill->maxInclination)
				myTreadmill->targetInclination = (auxInt*10)/25;

			//debug:
			/*
			mySerialParser->txBuf[0] = mySerialParser->buf[0];
			mySerialParser->txBuf[1] = mySerialParser->buf[1];
			mySerialParser->sendN = 2;
			*/
			mySerialParser->state = IDLE;
		}
	}
}

/*==================================================*/
/* TYPE 2		                                        */
/*==================================================*/

void serialParser_type2(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;

	if (mySerialParser->state == IDLE) {
		if (myChar == 0xA0) {
			myTreadmill->isRunning = true;
			mySerialParser->state = PARSING;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 0;
		} else treadmill_stop(myTreadmill);
	} else if (mySerialParser->state == PARSING) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 9) {
			if (mySerialParser->buf[0] == 0xA3) {
				auxInt  = ((unsigned int)(mySerialParser->buf[1])-'0')*1000;
				auxInt += ((unsigned int)(mySerialParser->buf[2])-'0')*100;
				auxInt += ((unsigned int)(mySerialParser->buf[3])-'0')*10;
				auxInt += ((unsigned int)(mySerialParser->buf[4])-'0');
				if (auxInt <= myTreadmill->maxSpeed_kmph_times10)
					myTreadmill->targetSpeed_kmph_times10 = auxInt;
			}

			if (mySerialParser->buf[5] == 0xA4) {
				auxInt  = ((unsigned int)(mySerialParser->buf[6])-'0')*1000;
				auxInt += ((unsigned int)(mySerialParser->buf[7])-'0')*100;
				auxInt += ((unsigned int)(mySerialParser->buf[8])-'0')*10;
				auxInt += ((unsigned int)(mySerialParser->buf[9])-'0');
				if (auxInt <= myTreadmill->maxInclination)
						myTreadmill->targetInclination = auxInt;
			}

			mySerialParser->state = IDLE;
		}
	}
}

/*==================================================*/
/* TYPE 3																						*/
/*==================================================*/

void serialParser_type3(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;

	if (mySerialParser->state == IDLE) {
		if ('B' <= myChar <= 'D') treadmill_stop(myTreadmill);
		else if (myChar == 'A'){
			myTreadmill->isRunning = true;
			mySerialParser->state = PARSING_SPEED;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 0;
		} else if (myChar == 'H') {
			myTreadmill->isRunning = true;
			mySerialParser->state = PARSING_INCLINATION;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 0;
		}
	} else if (mySerialParser->state == PARSING_SPEED) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 3) {
			mySerialParser->state = IDLE;
			auxInt  = ((unsigned int)(mySerialParser->buf[0])-'0')*1000;
			auxInt += ((unsigned int)(mySerialParser->buf[1])-'0')*100;
			auxInt += ((unsigned int)(mySerialParser->buf[2])-'0')*10;
			auxInt += ((unsigned int)(mySerialParser->buf[3])-'0');
			if (auxInt <= myTreadmill->maxSpeed_kmph_times10)
				myTreadmill->targetSpeed_kmph_times10 = auxInt;
		}
	} else if (mySerialParser->state == PARSING_INCLINATION) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 3) {
			mySerialParser->state = IDLE;
			auxInt  = ((unsigned int)(mySerialParser->buf[0])-'0')*1000;
			auxInt += ((unsigned int)(mySerialParser->buf[1])-'0')*100;
			auxInt += ((unsigned int)(mySerialParser->buf[2])-'0')*10;
			auxInt += ((unsigned int)(mySerialParser->buf[3])-'0');
			if (auxInt <= myTreadmill->maxInclination)
				myTreadmill->targetInclination = (auxInt*4)/10;
		}
	}
}

/*==================================================*/
/* TYPE 4		                                        */
/*==================================================*/

void serialParser_type4(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;
	int i;

	mySerialParser->discTimeout_us = DISCONN_TIMEOUT_us;
	if (mySerialParser->state == IDLE) {

		if (myChar == 0xA2 || myChar == 0xAA) {
			treadmill_stop(myTreadmill);
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA0) {
			myTreadmill->isRunning = true;
			mySerialParser->commDiscStop = true;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA1) {
			myTreadmill->isRunning = true;
			mySerialParser->commDiscStop = false;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA8) {
			if (!myTreadmill->isRunning) {
				myTreadmill->partialMicros = 0;
				myTreadmill->totalSeconds = 0;
			}
			myTreadmill->totalDistance_km = 0;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xAB) {
			myTreadmill->targetSpeed_kmph_times10 = 20;
			myTreadmill->targetInclination = 0;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xAC) {
			mySerialParser->xAckDataFlag = (mySerialParser->xAckDataFlag) ? false : true;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC0) {
			mySerialParser->txBuf[0] = myChar+0x10;
			if (!(myTreadmill->isRunning)) mySerialParser->txBuf[1] = '1';
			else if (mySerialParser->commDiscStop) mySerialParser->txBuf[1] = '2';
			else mySerialParser->txBuf[1] = '3';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[2] = myChar;
				mySerialParser->txBuf[3] = 0;
			} else mySerialParser->txBuf[2] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC1) {
			auxInt = KMPH_TO_MPH(myTreadmill->speed_kmph_times10);
			if (myTreadmill->isRunning && auxInt < 5) auxInt = 5;
			mySerialParser->txBuf[0] = myChar + 0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC2) {
			auxInt = ((myTreadmill->inclination)*25)/10;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC3) {
			auxInt = KMPH_TO_MPH(myTreadmill->targetSpeed_kmph_times10);
			if (myTreadmill->isRunning && auxInt < 5) auxInt = 5;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC4) {
			auxInt = ((myTreadmill->targetInclination)*25)/10;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC6) {
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = (myTreadmill->totalSeconds)/1000       + '0';
			mySerialParser->txBuf[2] = ((myTreadmill->totalSeconds)%1000)/100 + '0';
			mySerialParser->txBuf[3] = ((myTreadmill->totalSeconds)%100)/10   + '0';
			mySerialParser->txBuf[4] = (myTreadmill->totalSeconds)%10         + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC7) {
			auxInt = KMPH_TO_MPH(myTreadmill->totalDistance_km);
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC8 || myChar == 0xC9) {
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[3] = myChar;
				mySerialParser->txBuf[4] = 0;
			} else mySerialParser->txBuf[3] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC5 || (myChar >= 0xCA && myChar <= 0xCD)) {
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = '0';
			mySerialParser->txBuf[3] = '0';
			mySerialParser->txBuf[4] = '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA6) {
			mySerialParser->state = PARSING_PROTOCOL;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA7) {
			mySerialParser->state = PARSING_STAGE;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA5) {
			mySerialParser->state = PARSING_TIME;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA9) {
			mySerialParser->state = PARSING_WEIGHT;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA3) {
			mySerialParser->state = PARSING_SPEED;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA4) {
			mySerialParser->state = PARSING_INCLINATION;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else {
			mySerialParser->txBuf[0] = 0xBF;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

	} else if (mySerialParser->state == PARSING_PROTOCOL ||
							mySerialParser->state == PARSING_STAGE) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 2) {
			mySerialParser->state = IDLE;
			// NOT IMPLEMENTED, DO NOTHING
			mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
			if (mySerialParser->xAckDataFlag) {
				for (i = 0; i < 3; i++)
					mySerialParser->txBuf[i+1] = mySerialParser->buf[i];
				mySerialParser->txBuf[4] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	} else if (mySerialParser->state == PARSING_TIME ||
							mySerialParser->state == PARSING_WEIGHT) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			// NOT IMPLEMENTED, DO NOTHING
			mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
			if (mySerialParser->xAckDataFlag) {
				for (i = 0; i < 5; i++)
					mySerialParser->txBuf[i+1] = mySerialParser->buf[i];
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	} else if (mySerialParser->state == PARSING_SPEED) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			auxInt  = ((unsigned int)(mySerialParser->buf[1])-'0')*1000;
			auxInt += ((unsigned int)(mySerialParser->buf[2])-'0')*100;
			auxInt += ((unsigned int)(mySerialParser->buf[3])-'0')*10;
			auxInt += ((unsigned int)(mySerialParser->buf[4])-'0');
			if (auxInt <= KMPH_TO_MPH(myTreadmill->maxSpeed_kmph_times10)) {
				mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
				myTreadmill->targetSpeed_kmph_times10 = MPH_TO_KMPH(auxInt);
			} else mySerialParser->txBuf[0] = 0xBE; // data out of range
			if (mySerialParser->xAckDataFlag) {
				for (i = 0; i < 5; i++)
					mySerialParser->txBuf[i+1] = mySerialParser->buf[i];
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	} else if (mySerialParser->state == PARSING_INCLINATION) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			auxInt  = ((unsigned int)(mySerialParser->buf[1])-'0')*1000;
			auxInt += ((unsigned int)(mySerialParser->buf[2])-'0')*100;
			auxInt += ((unsigned int)(mySerialParser->buf[3])-'0')*10;
			auxInt += ((unsigned int)(mySerialParser->buf[4])-'0');
			if (auxInt <= myTreadmill->maxInclination) {
				mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
				myTreadmill->targetInclination = (auxInt*4)/10;
			} else mySerialParser->txBuf[0] = 0xBE; // data out of range
			if (mySerialParser->xAckDataFlag) {
				for (i = 0; i < 5; i++)
					mySerialParser->txBuf[i+1] = mySerialParser->buf[i];
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	}
}

/*==================================================*/
/* TYPE 5		                                        */
/*==================================================*/

void serialParser_type5(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;

	mySerialParser->discTimeout_us = DISCONN_TIMEOUT_us;
	if (mySerialParser->state == IDLE) {

		if (myChar == 0xA2 || myChar == 0xAA) {
			treadmill_stop(myTreadmill);
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA0) {
			myTreadmill->isRunning = true;
			mySerialParser->commDiscStop = true;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA1) {
			myTreadmill->isRunning = true;
			mySerialParser->commDiscStop = false;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA8) {
			if (!myTreadmill->isRunning) {
				myTreadmill->partialMicros = 0;
				myTreadmill->totalSeconds = 0;
			}
			myTreadmill->totalDistance_km = 0;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xAB) {
			myTreadmill->targetSpeed_kmph_times10 = 20;
			myTreadmill->targetInclination = 0;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC0) {
			mySerialParser->txBuf[0] = myChar+0x10;
			if (!(myTreadmill->isRunning)) mySerialParser->txBuf[1] = '1';
			else if (mySerialParser->commDiscStop) mySerialParser->txBuf[1] = '2';
			else mySerialParser->txBuf[1] = '3';
			mySerialParser->txBuf[2] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC1) {
			auxInt = KMPH_TO_MPH(myTreadmill->speed_kmph_times10);
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC2) {
			auxInt = ((myTreadmill->inclination)*25)/10;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC3) {
			auxInt = KMPH_TO_MPH(myTreadmill->targetSpeed_kmph_times10);
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC4) {
			auxInt = ((myTreadmill->targetInclination)*25)/10;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC6) {
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = (myTreadmill->totalSeconds)/1000       + '0';
			mySerialParser->txBuf[2] = ((myTreadmill->totalSeconds)%1000)/100 + '0';
			mySerialParser->txBuf[3] = ((myTreadmill->totalSeconds)%100)/10   + '0';
			mySerialParser->txBuf[4] = (myTreadmill->totalSeconds)%10         + '0';
			mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC7) {
			auxInt = KMPH_TO_MPH(myTreadmill->totalDistance_km);
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA6) {
			mySerialParser->state = PARSING_PROTOCOL;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA7) {
			mySerialParser->state = PARSING_STAGE;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA5) {
			mySerialParser->state = PARSING_TIME;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA9) {
			mySerialParser->state = PARSING_WEIGHT;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA3) {
			mySerialParser->state = PARSING_SPEED;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA4) {
			mySerialParser->state = PARSING_INCLINATION;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else {
			mySerialParser->txBuf[0] = 0xBF;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

	} else if (mySerialParser->state == PARSING_PROTOCOL ||
							mySerialParser->state == PARSING_STAGE) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 2) {
			// NOT IMPLEMENTED, DO NOTHING
			mySerialParser->txBuf[0] = 0xBF;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
			mySerialParser->state = IDLE;
		}
	} else if (mySerialParser->state == PARSING_TIME ||
							mySerialParser->state == PARSING_WEIGHT) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			// NOT IMPLEMENTED, DO NOTHING
			mySerialParser->txBuf[0] = 0xBF;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
			mySerialParser->state = IDLE;
		}
	} else if (mySerialParser->state == PARSING_SPEED) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			auxInt  = ((unsigned int)(mySerialParser->buf[1])-'0')*1000;
			auxInt += ((unsigned int)(mySerialParser->buf[2])-'0')*100;
			auxInt += ((unsigned int)(mySerialParser->buf[3])-'0')*10;
			auxInt += ((unsigned int)(mySerialParser->buf[4])-'0');
			if (auxInt <= KMPH_TO_MPH(myTreadmill->maxSpeed_kmph_times10)) {
				mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
				myTreadmill->targetSpeed_kmph_times10 = MPH_TO_KMPH(auxInt);
			} else mySerialParser->txBuf[0] = 0xBE; // data out of range
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	} else if (mySerialParser->state == PARSING_INCLINATION) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			auxInt  = ((unsigned int)(mySerialParser->buf[1])-'0')*1000;
			auxInt += ((unsigned int)(mySerialParser->buf[2])-'0')*100;
			auxInt += ((unsigned int)(mySerialParser->buf[3])-'0')*10;
			auxInt += ((unsigned int)(mySerialParser->buf[4])-'0');
			if (auxInt <= myTreadmill->maxInclination) {
				mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
				myTreadmill->targetInclination = (auxInt*4)/10;
			} else mySerialParser->txBuf[0] = 0xBE; // data out of range
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	}
}

/*==================================================*/
/* TYPE 6		                                        */
/*==================================================*/

void serialParser_type6(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;

	if (mySerialParser->state == IDLE) {

		if (myChar == 'R' || myChar == 'S') {
			treadmill_stop(myTreadmill);
			mySerialParser->txBuf[0] = '%';
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'M') {
			mySerialParser->imperial = false;
			mySerialParser->txBuf[0] = '%';
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'I') {
			mySerialParser->imperial = true;
			mySerialParser->txBuf[0] = '%';
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'D') {
			mySerialParser->txBuf[0]  = myChar;
			mySerialParser->txBuf[1]  = (myTreadmill->speed_kmph_times10)/100      + '0';
			mySerialParser->txBuf[2]  = ((myTreadmill->speed_kmph_times10)%100)/10 + '0';
			mySerialParser->txBuf[3]  = (myTreadmill->speed_kmph_times10)%10       + '0';
			auxInt = ((myTreadmill->targetInclination)*25)/10;
			mySerialParser->txBuf[4]  = auxInt/100      + '0';
			mySerialParser->txBuf[5]  = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[6]  = auxInt%10       + '0';
			mySerialParser->txBuf[7]  = (myTreadmill->totalSeconds)/1000       + '0';
			mySerialParser->txBuf[8]  = ((myTreadmill->totalSeconds)%1000)/100 + '0';
			mySerialParser->txBuf[9]  = ((myTreadmill->totalSeconds)%100)/10   + '0';
			mySerialParser->txBuf[10] = (myTreadmill->totalSeconds)%10         + '0';
			if (mySerialParser->imperial) auxInt = KMPH_TO_MPH(myTreadmill->totalDistance_km);
			else auxInt = myTreadmill->totalDistance_km;
			mySerialParser->txBuf[11] =                   '0';
			mySerialParser->txBuf[12] = auxInt/100      + '0';
			mySerialParser->txBuf[13] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[14] = auxInt%10       + '0';
			mySerialParser->txBuf[15] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'T') {
			mySerialParser->txBuf[0] = myChar;
			mySerialParser->txBuf[1] = (myTreadmill->totalSeconds)/1000       + '0';
			mySerialParser->txBuf[2] = ((myTreadmill->totalSeconds)%1000)/100 + '0';
			mySerialParser->txBuf[3] = ((myTreadmill->totalSeconds)%100)/10   + '0';
			mySerialParser->txBuf[4] = (myTreadmill->totalSeconds)%10         + '0';
			if (mySerialParser->imperial) auxInt = KMPH_TO_MPH(myTreadmill->totalDistance_km);
			else auxInt = myTreadmill->totalDistance_km;
			mySerialParser->txBuf[5] =                   '0';
			mySerialParser->txBuf[6] = auxInt/100      + '0';
			mySerialParser->txBuf[7] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[8] = auxInt%10       + '0';
			mySerialParser->txBuf[9] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'C') {
			if (!(myTreadmill->isRunning)) {
				myTreadmill->partialMicros = 0;
				myTreadmill->totalSeconds = 0;
			}
			myTreadmill->totalDistance_km = 0;
			mySerialParser->txBuf[0] = '%';
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'V') {
			mySerialParser->state = PARSING;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 0;
		}

		else {
			mySerialParser->txBuf[0] = '?';
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

	} else if (mySerialParser->state == PARSING) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 5) {
			mySerialParser->state = IDLE;

			auxInt  = (mySerialParser->buf[0] - '0')*100;
			auxInt += (mySerialParser->buf[1] - '0')*10;
			auxInt += (mySerialParser->buf[2] - '0');

			if (mySerialParser->imperial && auxInt <= KMPH_TO_MPH(myTreadmill->maxSpeed_kmph_times10))
				myTreadmill->targetSpeed_kmph_times10 = MPH_TO_KMPH(auxInt);
			else if (auxInt <= myTreadmill->maxSpeed_kmph_times10)
				myTreadmill->targetSpeed_kmph_times10 = auxInt;

			auxInt  = (mySerialParser->buf[3] - '0')*100;
			auxInt += (mySerialParser->buf[4] - '0')*10;
			auxInt += (mySerialParser->buf[5] - '0');

			if (auxInt <= myTreadmill->maxInclination)
				myTreadmill->targetInclination = (auxInt*4)/10;

			myTreadmill->isRunning = true;
			mySerialParser->txBuf[0] = '%';
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	}
}

/*==================================================*/
/* TYPE 7		                                        */
/*==================================================*/

void serialParser_type7(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;

	if (mySerialParser->state == IDLE) {
		if (myChar == 0xAA) treadmill_stop(myTreadmill);
		else if (myChar == 0xA0) {
			myTreadmill->isRunning = true;
			mySerialParser->state = PARSING;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 0;
		}
	} else if (mySerialParser->state == PARSING) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 1) {
			mySerialParser->state = IDLE;
			if (mySerialParser->buf[0] <= myTreadmill->maxSpeed_kmph_times10)
				myTreadmill->targetSpeed_kmph_times10 = mySerialParser->buf[0];
			auxInt = (((unsigned int)(mySerialParser->buf[1]))*10)/2;
			if (auxInt <= myTreadmill->maxInclination)
				myTreadmill->targetInclination = (auxInt*10)/25;
		}
	}
}

/*==================================================*/
/* TYPE 8		                                        */
/*==================================================*/

void serialParser_type8(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;

	if (mySerialParser->state == IDLE) {
		if (myChar == 0xAA) treadmill_stop(myTreadmill);
		else if (myChar == 0xA0) {
			myTreadmill->isRunning = true;
			mySerialParser->state = PARSING;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 0;
		}
	} else if (mySerialParser->state == PARSING) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 2) {
			mySerialParser->state = IDLE;

			auxInt  = ((unsigned int)(mySerialParser->buf[0]))*0x100;
			auxInt += (unsigned int)(mySerialParser->buf[1]);

			if (auxInt <= myTreadmill->maxSpeed_kmph_times10)
				myTreadmill->targetSpeed_kmph_times10 = auxInt;

			auxInt = (((unsigned int)(mySerialParser->buf[2]))*10)/2;
			if (auxInt <= myTreadmill->maxInclination)
				myTreadmill->targetSpeed_kmph_times10 = (auxInt*10)/25;
		}
	}
}

/*==================================================*/
/* TYPE 9		                                        */
/*==================================================*/

void serialParser_type9(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;

	if (mySerialParser->state == IDLE) {
		if (myChar == 'B' || myChar == 'H') {
			//NOT IMPLEMENTED, DO NOTHING
		}

		else if (myChar == 'A') mySerialParser->imperial = true;
		else if (myChar == 'K') mySerialParser->imperial = false;

		else if (myChar == 'E' || myChar == 'N') treadmill_stop(myTreadmill);

		else if (myChar == 'W') {
			auxInt = (myTreadmill->targetInclination*25)/10;
			mySerialParser->txBuf[0]  = 'S';
			mySerialParser->txBuf[1]  = myTreadmill->targetSpeed_kmph_times10/100      + '0';
			mySerialParser->txBuf[2]  = (myTreadmill->targetSpeed_kmph_times10%100)/10 + '0';
			mySerialParser->txBuf[3]  = myTreadmill->targetSpeed_kmph_times10%10       + '0';
			mySerialParser->txBuf[4]  = '0';
			mySerialParser->txBuf[5]  = 0x10;
			mySerialParser->txBuf[6]  = 'G';
			mySerialParser->txBuf[7]  = auxInt/100      + '0';
			mySerialParser->txBuf[8]  = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[9]  = auxInt%10       + '0';
			mySerialParser->txBuf[10] = '0';
			mySerialParser->txBuf[11] = 0x10;
			mySerialParser->txBuf[12] = (mySerialParser->imperial) ? 'A' : 'K';
			mySerialParser->txBuf[13] = 0x10;
			mySerialParser->txBuf[14] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'T') {
			mySerialParser->txBuf[0] = 'K';
			mySerialParser->txBuf[1] = '6';
			mySerialParser->txBuf[2] = 0x10;
			mySerialParser->txBuf[3] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'V') {
			mySerialParser->txBuf[0] = 'V';
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = '0';
			mySerialParser->txBuf[3] = '8';
			mySerialParser->txBuf[4] = 'A';
			mySerialParser->txBuf[5] = 0x10;
			mySerialParser->txBuf[6] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'C') {
			mySerialParser->txBuf[0] = 'T';
			mySerialParser->txBuf[1] = '2';
			mySerialParser->txBuf[2] = '1';
			mySerialParser->txBuf[3] = 'E';
			mySerialParser->txBuf[4] = 0x10;
			mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 'G') {
			mySerialParser->state = PARSING_INCLINATION;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 0;
		}

		else if (myChar == 'S') {
			mySerialParser->state = PARSING_SPEED;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 0;
		}

	} else if (mySerialParser->state == PARSING_INCLINATION) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			if (mySerialParser->buf[4] == 0x10) {
				auxInt  = (((unsigned int)(mySerialParser->buf[0])) - '0')*100;
				auxInt += (((unsigned int)(mySerialParser->buf[1])) - '0')*10;
				auxInt += (((unsigned int)(mySerialParser->buf[2])) - '0');
				if (auxInt <= myTreadmill->maxInclination)
					myTreadmill->targetInclination = (auxInt*4)/10;
			}
		}
	} else if (mySerialParser->state == PARSING_SPEED) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			if (mySerialParser->buf[4] == 0x10) {
				auxInt  = (((unsigned int)(mySerialParser->buf[0])) - '0')*100;
				auxInt += (((unsigned int)(mySerialParser->buf[1])) - '0')*10;
				auxInt += (((unsigned int)(mySerialParser->buf[2])) - '0');
				if (mySerialParser->imperial && auxInt <= KMPH_TO_MPH(myTreadmill->maxSpeed_kmph_times10))
					myTreadmill->targetSpeed_kmph_times10 = MPH_TO_KMPH(auxInt);
				else if (auxInt <= myTreadmill->maxSpeed_kmph_times10)
					myTreadmill->targetSpeed_kmph_times10 = auxInt;
				myTreadmill->isRunning = true;
			}
		}
	}
}

/*==================================================*/
/* TYPE 10	                                        */
/*==================================================*/

void serialParser_type10(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	unsigned int auxInt;
	int i;

	mySerialParser->discTimeout_us = DISCONN_TIMEOUT_us;
	if (mySerialParser->state == IDLE) {

		if (myChar == 0xA2 || myChar == 0xAA) {
			treadmill_stop(myTreadmill);
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA0) {
			myTreadmill->isRunning = true;
			mySerialParser->commDiscStop = true;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA1) {
			myTreadmill->isRunning = true;
			mySerialParser->commDiscStop = false;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA8) {
			if (!myTreadmill->isRunning) {
				myTreadmill->partialMicros = 0;
				myTreadmill->totalSeconds = 0;
			}
			myTreadmill->totalDistance_km = 0;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xAB) {
			myTreadmill->targetSpeed_kmph_times10 = 20;
			myTreadmill->targetInclination = 0;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xAC) {
			mySerialParser->xAckDataFlag = (mySerialParser->xAckDataFlag) ? false : true;
			mySerialParser->txBuf[0] = myChar+0x10;
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[1] = myChar;
				mySerialParser->txBuf[2] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC0) {
			mySerialParser->txBuf[0] = myChar+0x10;
			if (!(myTreadmill->isRunning)) mySerialParser->txBuf[1] = '1';
			else if (mySerialParser->commDiscStop) mySerialParser->txBuf[1] = '2';
			else mySerialParser->txBuf[1] = '3';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[2] = myChar;
				mySerialParser->txBuf[3] = 0;
			} else mySerialParser->txBuf[2] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC1) {
			if (myTreadmill->isRunning && auxInt < 5) auxInt = 5;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = (myTreadmill->speed_kmph_times10)/100      + '0';
			mySerialParser->txBuf[3] = ((myTreadmill->speed_kmph_times10)%100)/10 + '0';
			mySerialParser->txBuf[4] = (myTreadmill->speed_kmph_times10)%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC2) {
			auxInt = ((myTreadmill->inclination)*25)/10;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC3) {
			if (myTreadmill->isRunning && auxInt < 5) auxInt = 5;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = (myTreadmill->targetSpeed_kmph_times10)/100      + '0';
			mySerialParser->txBuf[3] = ((myTreadmill->targetSpeed_kmph_times10)%100)/10 + '0';
			mySerialParser->txBuf[4] = (myTreadmill->targetSpeed_kmph_times10)%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC4) {
			auxInt = ((myTreadmill->targetInclination)*25)/10;
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = auxInt/100      + '0';
			mySerialParser->txBuf[3] = (auxInt%100)/10 + '0';
			mySerialParser->txBuf[4] = auxInt%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC6) {
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = (myTreadmill->totalSeconds)/1000       + '0';
			mySerialParser->txBuf[2] = ((myTreadmill->totalSeconds)%1000)/100 + '0';
			mySerialParser->txBuf[3] = ((myTreadmill->totalSeconds)%100)/10   + '0';
			mySerialParser->txBuf[4] = (myTreadmill->totalSeconds)%10         + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC7) {
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = (myTreadmill->totalDistance_km)/100      + '0';
			mySerialParser->txBuf[3] = ((myTreadmill->totalDistance_km)%100)/10 + '0';
			mySerialParser->txBuf[4] = (myTreadmill->totalDistance_km)%10       + '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC8 || myChar == 0xC9) {
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[3] = myChar;
				mySerialParser->txBuf[4] = 0;
			} else mySerialParser->txBuf[3] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xC5 || (myChar >= 0xCA && myChar <= 0xCD)) {
			mySerialParser->txBuf[0] = myChar+0x10;
			mySerialParser->txBuf[1] = '0';
			mySerialParser->txBuf[2] = '0';
			mySerialParser->txBuf[3] = '0';
			mySerialParser->txBuf[4] = '0';
			if (mySerialParser->xAckDataFlag) {
				mySerialParser->txBuf[5] = myChar;
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[5] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}

		else if (myChar == 0xA6) {
			mySerialParser->state = PARSING_PROTOCOL;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA7) {
			mySerialParser->state = PARSING_STAGE;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA5) {
			mySerialParser->state = PARSING_TIME;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA9) {
			mySerialParser->state = PARSING_WEIGHT;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA3) {
			mySerialParser->state = PARSING_SPEED;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else if (myChar == 0xA4) {
			mySerialParser->state = PARSING_INCLINATION;
			mySerialParser->timeout_us = SERIAL_TIMEOUT_us;
			mySerialParser->bufIndex = 1;
			mySerialParser->buf[0] = myChar;
		}

		else {
			mySerialParser->txBuf[0] = 0xBF;
			mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		} // illegal command

	} else if (mySerialParser->state == PARSING_PROTOCOL ||
							mySerialParser->state == PARSING_STAGE) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 2) {
			mySerialParser->state = IDLE;
			// NOT IMPLEMENTED, DO NOTHING
			mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
			if (mySerialParser->xAckDataFlag) {
				for (i = 0; i < 3; i++)
					mySerialParser->txBuf[i+1] = mySerialParser->buf[i];
				mySerialParser->txBuf[4] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	} else if (mySerialParser->state == PARSING_TIME ||
							mySerialParser->state == PARSING_WEIGHT) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			// NOT IMPLEMENTED, DO NOTHING
			mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
			if (mySerialParser->xAckDataFlag) {
				for (i = 0; i < 5; i++)
					mySerialParser->txBuf[i+1] = mySerialParser->buf[i];
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	} else if (mySerialParser->state == PARSING_SPEED) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			auxInt  = ((unsigned int)(mySerialParser->buf[1])-'0')*1000;
			auxInt += ((unsigned int)(mySerialParser->buf[2])-'0')*100;
			auxInt += ((unsigned int)(mySerialParser->buf[3])-'0')*10;
			auxInt += ((unsigned int)(mySerialParser->buf[4])-'0');
			if (auxInt <= myTreadmill->maxSpeed_kmph_times10) {
				mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
				myTreadmill->targetSpeed_kmph_times10 = auxInt;
			} else mySerialParser->txBuf[0] = 0xBE;
			if (mySerialParser->xAckDataFlag) {
				for (i = 0; i < 5; i++)
					mySerialParser->txBuf[i+1] = mySerialParser->buf[i];
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	} else if (mySerialParser->state == PARSING_INCLINATION) {
		mySerialParser->buf[mySerialParser->bufIndex] = myChar;
		mySerialParser->bufIndex += 1;
		if (mySerialParser->bufIndex > 4) {
			mySerialParser->state = IDLE;
			auxInt  = ((unsigned int)(mySerialParser->buf[1])-'0')*1000;
			auxInt += ((unsigned int)(mySerialParser->buf[2])-'0')*100;
			auxInt += ((unsigned int)(mySerialParser->buf[3])-'0')*10;
			auxInt += ((unsigned int)(mySerialParser->buf[4])-'0');
			if (auxInt <= myTreadmill->maxInclination) {
				mySerialParser->txBuf[0] = (mySerialParser->buf[0])+0x10;
				myTreadmill->targetInclination = (auxInt*4)/10;
			} else mySerialParser->txBuf[0] = 0xBE;
			if (mySerialParser->xAckDataFlag) {
				for (i = 0; i < 5; i++)
					mySerialParser->txBuf[i+1] = mySerialParser->buf[i];
				mySerialParser->txBuf[6] = 0;
			} else mySerialParser->txBuf[1] = 0;
			serial_sendBuf(mySerialParser->txBuf);
		}
	}
}

void serialParser_parse(
	serialParser_t* mySerialParser,
	treadmill_t* myTreadmill,
	unsigned char myChar)
{
	switch (mySerialParser->protocol) {
		case INBRAMED: serialParser_inbramed(mySerialParser, myTreadmill, myChar); break;
		case TYPE2: serialParser_type2(mySerialParser, myTreadmill, myChar); break;
		case TYPE3: serialParser_type3(mySerialParser, myTreadmill, myChar); break;
		case TYPE4: serialParser_type4(mySerialParser, myTreadmill, myChar); break;
		case TYPE5: serialParser_type5(mySerialParser, myTreadmill, myChar); break;
		case TYPE6: serialParser_type6(mySerialParser, myTreadmill, myChar); break;
		case TYPE7: serialParser_type7(mySerialParser, myTreadmill, myChar); break;
		case TYPE8: serialParser_type8(mySerialParser, myTreadmill, myChar); break;
		case TYPE9: serialParser_type9(mySerialParser, myTreadmill, myChar); break;
		case TYPE10: serialParser_type10(mySerialParser, myTreadmill, myChar); break;
	}
}

/*
  interface-rev2: serial.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _serial_h_
#define _serial_h_

#include "utils.h"
#include "timer1.h"

/*==================================================*/
/* TYPEDEFS                                         */
/*==================================================*/

typedef enum serial_serialMode_e {
	SHIFT_REGISTER,
	_8b_UART,
	_9b_UART,
	_39b_UART
} serial_serialMode_e;

typedef struct serial_t {
	serial_serialMode_e serialMode;
	bool multiprocComm;
	timer1_t baudrateGenerator;
} serial_t;

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

serial_t serial_init(
	serial_serialMode_e serialMode,
	bool multiprocComm
);

void serial_enableRX();
void serial_sendChar(char arg);
void serial_sendBuf(char*);
void serial_sendN(char*, int);

/*==================================================*/
/* EXTERNABLE VARIABLES                             */
/*==================================================*/

extern volatile bool serialFlag;

#endif

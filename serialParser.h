/*
  interface-rev2: serialParser.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _serialParser_h_
#define _serialParser_h_

#include "switches.h"
#include "treadmill.h"
#include "utils.h"

#define SERIAL_TIMEOUT_us 2000000
#define DISCONN_TIMEOUT_us 500000
#define RX_BUF_LEN 16
#define TX_BUF_LEN 16

/*==================================================*/
/* TYPEDEFS                                         */
/*==================================================*/

typedef enum serialParser_states_e {
	IDLE,
	PARSING,
	PARSING_SPEED,
	PARSING_INCLINATION,
	PARSING_PROTOCOL,
	PARSING_STAGE,
	PARSING_TIME,
	PARSING_WEIGHT
} serialParser_states_e;

typedef struct serialParser_t {
	protocol_e protocol;
	serialParser_states_e state;
	unsigned char buf[RX_BUF_LEN];
	unsigned char txBuf[TX_BUF_LEN];
	unsigned int bufIndex;
	long timeout_us;
	long discTimeout_us;
	bool xAckDataFlag;
	bool commDiscStop;
	bool imperial;
} serialParser_t;

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

serialParser_t serialParser_init(protocol_e);
void serialParser_parse(serialParser_t*, treadmill_t*, unsigned char);

#endif

/*
  interface-rev2: switches.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _switches_h_
#define _switches_h_

/*==================================================*/
/* TYPEDEFS                                         */
/*==================================================*/

typedef enum treadmill_e {
	BALDOR_12kmph                         = 0x00,
	BALDOR_10kmph5                        = 0x01,
	WEG_16kmph1_COM_FORRACAO              = 0x02,
	WEG_16kmph1_SEM_FORRACAO              = 0x03,
	WEG_ATL_24kmph                        = 0x04,
	WEG_SUPER_ATL_30kmph                  = 0x05,
	ESTEIRA_COM_INVERSOR_12kmph           = 0x06,
	ESTEIRA_COM_INVERSOR_16kmph_OU_24kmph = 0x07,
	ESTEIRA_COM_INVERSOR_18kmph           = 0x08
} treadmill_e;

typedef enum protocol_e {
	INBRAMED = 0x01,
	TYPE2 = 0x02,
	TYPE3 = 0x03,
	TYPE4 = 0x04,
	TYPE5 = 0x05,
	TYPE6 = 0x06,
	TYPE7 = 0x07,
	TYPE8 = 0x08,
	TYPE9 = 0x09,
	TYPE10 = 0x0A
} protocol_e;

typedef struct pulseConstants_t {
	unsigned char numberOfPulses_kmph;
	unsigned char numberOfPulses_mph;
	unsigned int numberOfPulses_km;
	unsigned int numberOfPulses_mile;
} pulseConstants_t;

typedef struct switches_t {
	treadmill_e treadmill;
	protocol_e protocol;
	pulseConstants_t pulseConstants;
	unsigned int maxSpeed_kmph_times10;
} switches_t;

/*==================================================*/
/* PUBLIC FUNCTIONS                                 */
/*==================================================*/

switches_t switches_init();

#endif

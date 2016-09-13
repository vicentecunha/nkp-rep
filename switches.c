/*
  interface-rev2: switches.c
  Author: Vicente Cunha
  Date: August 2016
*/

#include "switches.h"

xdata	unsigned char	FIRST_COLUMN  _at_ 0x1800;
xdata	unsigned char	SECOND_COLUMN _at_ 0x3800;

switches_t switches_init()
{
	switches_t mySwitches;

	mySwitches.treadmill = ~FIRST_COLUMN  & 0x0F;
	mySwitches.protocol  = ~SECOND_COLUMN & 0x0F;

	switch (mySwitches.treadmill) {
		case BALDOR_12kmph:
			mySwitches.pulseConstants.numberOfPulses_kmph = 126;
			mySwitches.pulseConstants.numberOfPulses_mph  = 203;
			mySwitches.pulseConstants.numberOfPulses_km   = 0x3b3d;
			mySwitches.pulseConstants.numberOfPulses_mile = 0x5f55;
			mySwitches.maxSpeed_kmph_times10              = 120;
			break;
		case BALDOR_10kmph5:
			mySwitches.pulseConstants.numberOfPulses_kmph  = 137;
			mySwitches.pulseConstants.numberOfPulses_mph   = 220;
			mySwitches.pulseConstants.numberOfPulses_km    = 0x402c;
			mySwitches.pulseConstants.numberOfPulses_mile  = 0x6746;
			mySwitches.maxSpeed_kmph_times10               = 105;
			break;
		case WEG_16kmph1_COM_FORRACAO:
			mySwitches.pulseConstants.numberOfPulses_kmph  = 103;
			mySwitches.pulseConstants.numberOfPulses_mph   = 165;
			mySwitches.pulseConstants.numberOfPulses_km    = 0x332b;
			mySwitches.pulseConstants.numberOfPulses_mile  = 0x5258;
			mySwitches.maxSpeed_kmph_times10               = 161;
			break;
		case WEG_16kmph1_SEM_FORRACAO:
			mySwitches.pulseConstants.numberOfPulses_kmph  = 108;
			mySwitches.pulseConstants.numberOfPulses_mph   = 174;
			mySwitches.pulseConstants.numberOfPulses_km    = 0x3053;
			mySwitches.pulseConstants.numberOfPulses_mile  = 0x4dc4;
			mySwitches.maxSpeed_kmph_times10               = 161;
			break;
		case WEG_ATL_24kmph:
			mySwitches.pulseConstants.numberOfPulses_kmph  = 73;
			mySwitches.pulseConstants.numberOfPulses_mph   = 117;
			mySwitches.pulseConstants.numberOfPulses_km    = 0x2318;
			mySwitches.pulseConstants.numberOfPulses_mile  = 0x3878;
			mySwitches.maxSpeed_kmph_times10               = 240;
			break;
		case WEG_SUPER_ATL_30kmph:
			mySwitches.pulseConstants.numberOfPulses_kmph  = 54;
			mySwitches.pulseConstants.numberOfPulses_mph   = 86;
			mySwitches.pulseConstants.numberOfPulses_km    = 0x1930;
			mySwitches.pulseConstants.numberOfPulses_mile  = 0x2888;
			mySwitches.maxSpeed_kmph_times10               = 300;
			break;
		case ESTEIRA_COM_INVERSOR_12kmph:
			mySwitches.pulseConstants.numberOfPulses_kmph  = 146;
			mySwitches.pulseConstants.numberOfPulses_mph   = 235;
			mySwitches.pulseConstants.numberOfPulses_km    = 0x413C;
			mySwitches.pulseConstants.numberOfPulses_mile  = 0x68FB;
			mySwitches.maxSpeed_kmph_times10               = 120;
			break;
		case ESTEIRA_COM_INVERSOR_16kmph_OU_24kmph:
			mySwitches.pulseConstants.numberOfPulses_kmph  = 108;
			mySwitches.pulseConstants.numberOfPulses_mph   = 174;
			mySwitches.pulseConstants.numberOfPulses_km    = 0x32DB;
			mySwitches.pulseConstants.numberOfPulses_mile  = 0x51D7;
			mySwitches.maxSpeed_kmph_times10               = 160;
			break;
		case ESTEIRA_COM_INVERSOR_18kmph:
			mySwitches.pulseConstants.numberOfPulses_kmph  = 96;
			mySwitches.pulseConstants.numberOfPulses_mph   = 154;
			mySwitches.pulseConstants.numberOfPulses_km    = 0x2CEC;
			mySwitches.pulseConstants.numberOfPulses_mile  = 0x4853;
			mySwitches.maxSpeed_kmph_times10               = 180;
			break;
		//default: while(1); // ERROR: unknown treadmill
	}

	return mySwitches;
}

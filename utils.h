/*
  interface-rev2: utils.h
  Author: Vicente Cunha
  Date: August 2016
*/

#ifndef _utils_h_
#define _utils_h_

#define XTAL_FREQ_MHz 7.3728
#define XTAL_FREQ_Hz 7372800

#define KMPH_TO_MPH(x) ((x*62)/100)
#define MPH_TO_KMPH(x) ((x*161)/100)

/*==================================================*/
/* TYPEDEFS                                         */
/*==================================================*/

typedef enum {false, true} bool;

#endif
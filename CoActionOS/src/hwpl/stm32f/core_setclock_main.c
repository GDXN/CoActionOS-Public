/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "core_flags.h"

int hwpl_core_cpu_freq HWPL_SYS_MEM;


#define PCLKSEL0_ALL_1 0x55515155
#define PCLKSEL1_ALL_1 0x54555455


void _hwpl_core_setclock(int fclk /*! The target clock frequency */,
		int fosc /*! The oscillator frequency (between 10 and 25MHz) */){


}


/*! @} */

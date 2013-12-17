/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "hwpl/core.h"

int hwpl_core_cpu_freq HWPL_SYS_MEM;

void _hwpl_core_setclockinternal(int fclk /*! The target clock frequency */){
	hwpl_core_cpu_freq = 8000000;
}


/*! @} */

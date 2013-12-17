/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <stdbool.h>
#include "hwpl/mpu.h"


int mpu_dev_init(void){
	int err;

	//Peripherals Regions 0 up to 2
	err = mpu_enable_region(
			0,
			(void*)0x2009C000,  //GPIO Access
			0x2009FFFF + 1 - 0x2009C000,
			MPU_ACCESS_PRW_URW,
			MPU_MEMORY_PERIPHERALS,
			false
	);

	if ( err < 0 ){
		return err;
	}

	err = mpu_enable_region(
			1,
			(void*)0x40000000,  //APB/AHB Peripherals
			0x60000000 - 0x40000000,
			MPU_ACCESS_PRW_UR,
			MPU_MEMORY_PERIPHERALS,
			false
	);

	if ( err < 0 ){
		return err;
	}

	/*
	err = mpu_enable_region(
			2,
			(void*)0x50000000,  //APB/AHB Peripherals
			0x501FFFFF + 1 - 0x50000000,
			MPU_ACCESS_PRW_URW,
			MPU_MEMORY_PERIPHERALS,
			false
	);

	if ( err < 0 ){
		return err;
	}
	*/

	return 0;
}

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"

int fault_dev_save(fault_t * fault){
	return 0;
}

int fault_dev_load(fault_t * fault){
	memset(fault, 0, sizeof(fault_t));
	return 0;
}


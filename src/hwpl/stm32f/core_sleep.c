/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "core_flags.h"


static void priv_set_sleep_mode(int * level);

int _hwpl_core_sleep(core_powerdown_t level){

	//Wait for an interrupts
	__WFI();
	return 0;
}

void priv_set_sleep_mode(int * level){

}



/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "hwpl/core.h"
#include "core_flags.h"


static void priv_set_sleep_mode(int * level);

int _hwpl_core_sleep(core_sleep_t level){

	hwpl_core_privcall((core_privcall_t)priv_set_sleep_mode, &level);
	if ( level < 0 ){
		return level;
	}

	//Wait for an interrupts
	__WFI();
	return 0;
}

void priv_set_sleep_mode(int * level){
	SCB->SCR &= ~(1<<SCB_SCR_SLEEPDEEP_Pos);
	LPC_PMU->PCON = 0;
	switch(*level){
	case CORE_DEEPSLEEP_STOP:
		LPC_PMU->PCON = 1; //turn off the flash as well
	case CORE_DEEPSLEEP:
		SCB->SCR |= (1<<SCB_SCR_SLEEPDEEP_Pos);
		break;
	case CORE_SLEEP:
		break;
	case CORE_DEEPSLEEP_STANDBY:
		SCB->SCR |= (1<<SCB_SCR_SLEEPDEEP_Pos);
		LPC_PMU->PCON = 3;
		break;
	default:
		*level = -1;
		return;
	}
	*level = 0;
}



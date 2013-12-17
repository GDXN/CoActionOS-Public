/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"

int fault_dev_save(fault_t * fault){
	LPC_RTC->GPREG0 = fault->num;
	LPC_RTC->GPREG1 = (uint32_t)fault->pc;
	LPC_RTC->GPREG2 = (uint32_t)fault->caller;
	LPC_RTC->GPREG3 = (uint32_t)fault->handler_pc;
	LPC_RTC->GPREG4 = (uint32_t)fault->handler_caller;
	return 0;
}

int fault_dev_load(fault_t * fault){
	fault->num = LPC_RTC->GPREG0;
	fault->pc = (void*)LPC_RTC->GPREG1;
	fault->caller = (void*)LPC_RTC->GPREG2;
	fault->handler_pc = (void*)LPC_RTC->GPREG3;
	fault->handler_caller = (void*)LPC_RTC->GPREG4;
	fault->addr = (void*)0xFFFFFFFF;
	LPC_RTC->GPREG0 = 0; //clear any existing faults since it has been read
	return 0;
}


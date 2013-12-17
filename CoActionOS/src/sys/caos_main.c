/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup CAOS
 * @{
 */

/*! \file */

#include "config.h"
#include "hwpl.h"
#include "hwpl/core.h"


#include "caos.h"
#include "sched/sched_flags.h"

extern const int cpu_init_freq;
extern const int microcomputer_osc_freq;

void gled_priv_error(void * args);
static void init_hw(void);
static void check_reset_source(void);

/*! \details This function runs the operating system.
 *
 */
int _main(void){
	init_hw();

	if ( sched_start(initial_thread, 10) < 0 ){
		hwpl_debug("Error:  Failed to start scheduler\n");
		_hwpl_core_priv_disable_interrupts(NULL);
		gled_priv_error(0);
	}

	hwpl_debug("Error: return to main\n");
	_hwpl_core_priv_disable_interrupts(NULL);
	gled_priv_error(0);
	while(1);
	return 0;
}

/*! \details This function initializes the hardware
 *
 */
void init_hw(void){
	_hwpl_core_setclock(cpu_init_freq, microcomputer_osc_freq); //Set the main clock
	hwpl_fault_init();
	_hwpl_core_priv_enable_interrupts(NULL); //Enable the interrupts

	if ( sched_init() < 0 ){ //Initialize the hardware used for the scheduler
		_hwpl_core_priv_disable_interrupts(NULL);
		gled_priv_error(0);
	}

	check_reset_source();
}

void check_reset_source(void){
	fault_t fault;
	core_attr_t attr;

	hwpl_core_getattr(0, &attr);

	switch(attr.reset_type){
	case CORE_RESET_SRC_WDT:
		//log the reset
		break;
	case CORE_RESET_SRC_POR:
	case CORE_RESET_SRC_BOR:
	case CORE_RESET_SRC_EXTERNAL:
		//read and discard the fault
		hwpl_fault_load(&fault);
		break;
	}
}

void gled_priv_error(void * args){
	while(1){
		gled_priv_on(0);
		_delay_ms(50);
		gled_priv_off(0);
		_delay_ms(50);
	}
}


/*! @} */

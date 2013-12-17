/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SCHED
 * @{
 *
 */

/*! \file */
#include "config.h"
#include "sched_flags.h"

#include "hwpl.h"

volatile sched_fault_t sched_fault HWPL_SYS_MEM;

#if SINGLE_TASK != 0
static void * (*start_function)(void*) HWPL_SYS_MEM;
static int start_single(void);
#endif

/*! \details This function initializes the peripheral hardware needed
 * by the scheduler specifically the microsecond timer.
 * \return Zero on success or an error code (see \ref caoslib_err_t)
 */
int sched_init(void){

#if SINGLE_TASK == 0
	task_total = CHILD_MAX;
	task_table = sched_task_table;

	sched_current_priority = SCHED_LOWEST_PRIORITY - 1;
	sched_status_changed = 0;

	memset((void*)task_table, 0, sizeof(task_t) * CHILD_MAX);
	memset((void*)sched_table, 0, sizeof(sched_task_t) * CHILD_MAX);

	//Do basic init of task 0 so that memory allocation can happen before the scheduler starts
	task_table[0].reent = _impure_ptr;
	task_table[0].global_reent = _global_impure_ptr;
#endif

	return 0;
}

/* \details This function initializes the scheduler.  It should be
 * called after all peripherals are initialized and interrupts are on.
 * \return Zero on success or an error code
 */
int sched_start(void * (*init)(void*), int priority){

#if SINGLE_TASK == 0

	sched_table[0].init = init;
	sched_table[0].priority = priority;
	sched_table[0].attr.stackaddr = &_data;
	sched_table[0].attr.stacksize = caoslib_system_memory_size;

	//Start the scheduler in a new thread
	if ( task_init(SCHED_RR_DURATION,
			scheduler, //run the scheduler
			NULL, //Let the task init function figure out where the stack needs to be and the heap size
			caoslib_system_memory_size)
	){
		return -1;
	}
#else

	start_function = init;
	//Enter thread mode
	task_init_single(start_single,
			NULL,
			caoslib_system_memory_size);
#endif
	//Program never gets to this point
	return -1;
}


int sched_prepare(void){

	if ( hwpl_debug_init() ){
		_hwpl_core_priv_disable_interrupts(NULL);
		gled_priv_error(0);
	}

#if CAOSLIB_USECOND_TMR_SLEEP_OC > -1
	if ( sched_timing_init() ){
		return -1;
	}
#endif
	//Load any possible faults from the last reset
	hwpl_fault_load((fault_t*)&sched_fault.fault);


#if USE_MEMORY_PROTECTION > 0
	if ( task_init_mpu(&_data, caoslib_system_memory_size) < 0 ){
		sched_debug("Failed to initialize memory protection\n");
		gled_priv_error(0);
	}
#endif

	_hwpl_core_unprivileged_mode(); //Enter unpriv mode
	return 0;
}

#if SINGLE_TASK != 0
int start_single(void){
	sched_prepare();
	start_function(NULL);
}
#endif




/*! @} */


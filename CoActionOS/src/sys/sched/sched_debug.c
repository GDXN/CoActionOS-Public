/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \ingroup SCHED
 * @{
 *
 */

/*! \file */


#include "sched_flags.h"

void sched_debug_print_tasks(void){
	int i;
	for(i=1; i < task_get_total(); i++){
		if ( task_enabled(i) ){
			hwpl_debug("Task %d Details (0x%X):\n", i, task_table[i].flags);
			hwpl_debug("\tMem Addr: 0x%X\n", (unsigned int)sched_table[i].attr.stackaddr);
			hwpl_debug("\tCurrent Stack Ptr:  0x%X\n", (unsigned int)task_table[i].sp);
			hwpl_debug("\tStack Size: %d\n", sched_table[i].attr.stacksize);
			hwpl_debug("\tSched Priority %d\n", sched_table[i].attr.schedparam.sched_priority);
		} else {
			hwpl_debug("Task %d is not enabled\n", i);
		}
	}
}

void sched_debug_print_active(void){
	int i;
	hwpl_debug("Active Tasks:\n");
	for(i=1; i < task_get_total(); i++){
		if ( sched_active_asserted(i) ){
			hwpl_debug("\t%d\n", i);
		}
	}
}

void sched_debug_print_executing(void){
	int i;
	hwpl_debug("Executing Tasks:\n");
	for(i=0; i < task_get_total(); i++){
		if ( task_exec_asserted(i) ){
			hwpl_debug("\t%d\n", i);
		}
	}
}


void sched_debug_print_timers(void){
	int i;
	uint32_t timer[2];
	hwpl_debug("Task Timers:\n");
	for(i=0; i < task_get_total(); i++){
		if ( task_enabled(i) ){
			task_get_timer(timer, i);
			hwpl_debug("\t%d 0x%08X 0x%08X\n", i, (unsigned int)timer[1], (unsigned int)timer[0]);
		}
	}
}



/*! @} */


/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SCHED
 * @{
 *
 */

/*! \file */

//#include "config.h"
#include <errno.h>
#include <signal.h>
#include "hwpl.h"

#include "sched_flags.h"
#include "../unistd/unistd_flags.h"
#include "../signal/sig_local.h"

#include "hwpl/debug.h"


void hwpl_fault_event_handler(fault_t * fault){
#if SINGLE_TASK == 0
	int i;
	int pid;

	pid = task_get_pid( task_get_current() );

	if ( sched_fault.logged == false ){
		sched_fault.tid = task_get_current();
		sched_fault.pid = pid;
		memcpy((void*)&sched_fault.fault, fault, sizeof(fault_t));
	}

	if ( pid == 0 ){
		hwpl_fault_save(fault); //save the fault in NV memory then log it to the filesystem on startup
		//The OS has experienced a fault

		sched_fault_build_string(hwpl_debug_buffer);
		hwpl_priv_write_debug_uart(NULL);

		gled_priv_error(0);

		//WDT will force a reset
	} else {

		//send a signal to kill the task

		for(i=1; i < task_get_total(); i++){
			if ( task_get_pid(i) == pid ){

				if( task_isthread_asserted(i) == 0 ){
					//reset the stack of the processes main task
					task_priv_resetstack(i);
					//send the kill signal
					if( signal_priv_send(0, i, SIGKILL, 0, 0, 0) < 0 ){
						//kill manually -- for example, if the target task doesn't have enough memory to accept SIGKILL
						task_priv_del(i);
					}
					break;
				}

			}
		}

		//sched_priv_update_on_sleep();
	}
#else


	while(1);
#endif
}

/*! @} */

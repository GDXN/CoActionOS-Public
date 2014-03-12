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
#include <stdbool.h>
#include <pthread.h>
#include <reent.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <malloc.h>

#include "hwpl/wdt.h"
#include "hwpl/debug.h"
#include "hwpl/rtc.h"

#include "sched_flags.h"
#include "../unistd/unistd_flags.h"

extern const char _stderr_dev[];

static int start_first_thread(void);

volatile int8_t sched_current_priority HWPL_SYS_MEM;
volatile int8_t sched_status_changed HWPL_SYS_MEM;
volatile sched_task_t sched_table[CHILD_MAX] HWPL_SYS_MEM;
task_t sched_task_table[CHILD_MAX] HWPL_SYS_MEM;


static void priv_check_sleep_mode(void * args);
static int check_faults(void);
static void priv_clear_faults(void * args);

static void * log_fault(void * args);



int sched_check_tid(int id){
	if( id < task_get_total() ){
		if ( task_enabled(id) ){
			return 0;
		}
	}
	return -1;
}

void set_uart_priority(void){
#ifndef NO_DEBUG
	core_irqprio_t req;
	req.port = HWPL_DEBUG_PORT;
	req.periph = CORE_PERIPH_UART;
	req.prio = 14;
	hwpl_core_setirqprio(0, &req);
#endif
}

/* \details This function should be called in main() after all hardware
 * and devices have been initialized.
 *
 * The scheduler executes as task 0.  It is run as part of the round robin with all
 * active tasks of the highest priority.
 *
 * When a task calls sleep() or usleep(), it goes inactive.  When a task does
 * a synchronous read() or write() using underlying asynchronous hardware,
 * the task goes inactive until the operation completes.
 *
 * It an active task has a lower priority of another active task, it stays
 * "active" but is not executed until all higher priority tasks go inactive.
 *
 * If a lower priority task is executing and a higher priority task goes "active" (for example,
 * after sleep() or usleep() timer is expired), the lower priority task is pre-empted.
 */
void scheduler(void){
	bool do_sleep;
	sched_priv_assert_status_change();

	//Elevate the priority of the UART so that it can debug in interrupts
	set_uart_priority();

	//This interval needs to be long enough to allow for flash writes
	hwpl_wdt_init(WDT_MODE_RESET|WDT_MODE_CLK_SRC_MAIN, SCHED_RR_DURATION * 10 * CHILD_MAX + 5);

	if ( sched_prepare() ){
		hwpl_core_privcall(gled_priv_error, 0);
	}

	if ( start_first_thread() ){
		sched_debug("Start first thread failed\n");
		hwpl_core_privcall(gled_priv_error, 0);
	}

	while(1){
		hwpl_core_privcall(hwpl_wdt_priv_reset, NULL);
		check_faults(); //check to see if a fault needs to be logged
		hwpl_core_privcall(priv_check_sleep_mode, &do_sleep);

		//Sleep when nothing else is going on
		if ( do_sleep ){
			_hwpl_core_sleep(CORE_SLEEP);
		} else {
			//Otherwise switch to the active task
			hwpl_core_privcall(task_priv_switch_context, NULL);
		}
	}
}

void priv_clear_faults(void * args){
	sched_fault_build_string(hwpl_debug_buffer);
	hwpl_priv_write_debug_uart(NULL);
	sched_fault.fault.num = 0;
	sched_fault.logged = false;
}

void * log_fault(void * args){
	int std_fd;
	int len;
	int err;
	char * p;
	std_fd = open(_stderr_dev, O_RDWR);

	hwpl_core_privcall(priv_clear_faults, NULL);

	if ( std_fd >= 0 ){
		len = strlen(hwpl_debug_buffer);
		p = hwpl_debug_buffer;
		do {
			err = write(std_fd, p, len);
			if ( err > 0 ){
				p += err;
				len -= err;
			}
		} while( err > 0 );

		close(std_fd);
	}

	return NULL;
}

void priv_fault_logged(void * args){
	sched_fault.logged = true;
}

int check_faults(void){
	pthread_attr_t attr;
	int err;

	if ( (sched_fault.fault.num != 0) && (sched_fault.logged == false) ){
		//Log the fault
		hwpl_core_privcall(priv_fault_logged, NULL);
		//log the fault in a new thread

		//manually set the pthread attr to prevent inclusion in the build
		attr.stacksize = 1024;
		attr.stackaddr = malloc(attr.stacksize + sizeof(struct _reent) + SCHED_DEFAULT_STACKGUARD_SIZE);
		if ( attr.stackaddr == NULL ){
			errno = ENOMEM;
			return -1;
		}
		PTHREAD_ATTR_SET_IS_INITIALIZED((&attr), 1);
		PTHREAD_ATTR_SET_CONTENTION_SCOPE((&attr), 0);
		PTHREAD_ATTR_SET_GUARDSIZE((&attr), SCHED_DEFAULT_STACKGUARD_SIZE);
		PTHREAD_ATTR_SET_INHERIT_SCHED((&attr), 0);
		PTHREAD_ATTR_SET_DETACH_STATE((&attr), PTHREAD_CREATE_DETACHED);
		PTHREAD_ATTR_SET_SCHED_POLICY((&attr), SCHED_OTHER);
		attr.schedparam.sched_priority = 4; //not the default priority

		err = sched_new_thread(log_fault,
				NULL,
				attr.stackaddr,
				attr.stacksize,
				&attr);

		if ( !err ){
			return err;
		}
	}

	return 0;
}

void priv_check_sleep_mode(void * args){
	bool * p = (bool*)args;
	int i;
	*p = true;
	for(i=1; i < task_get_total(); i++){
		if ( task_enabled(i) && sched_active_asserted(i) ){
			*p = false;
			return;
		}
	}
}

void sched_priv_update_on_stopped(void){
	int i;
	int new_priority;

	sched_current_priority = SCHED_LOWEST_PRIORITY - 1;
	new_priority = sched_current_priority;
	//start with process 1 -- 0 is the scheduler process
	for(i=1; i < task_get_total(); i++){
		//Find the highest priority of all active processes
		if ( task_enabled(i) &&
				sched_active_asserted(i) &&
				!sched_stopped_asserted(i) &&
				(sched_get_priority(i) > new_priority) ){
			new_priority = sched_get_priority(i);
		}
	}

	if ( new_priority >= SCHED_LOWEST_PRIORITY ){
		sched_priv_update_on_wake(new_priority);
	} else {
		task_priv_switch_context(NULL);
	}
}


void sched_priv_update_on_sleep(void){
	sched_priv_deassert_active( task_get_current() );
	sched_priv_update_on_stopped();
}

void sched_priv_update_on_wake(int new_priority){
	int i;
	bool switch_context;

	if (new_priority > sched_current_priority){
		switch_context = true;
		sched_current_priority = new_priority;
	} else if (new_priority == sched_current_priority){
		switch_context = false;
	} else {
		sched_priv_assert_status_change();
		return;
	}

	for(i=1; i < task_get_total(); i++){
		if ( task_enabled(i) && sched_active_asserted(i) && !sched_stopped_asserted(i) && ( sched_get_priority(i) == sched_current_priority ) ){
			//Enable process execution for highest active priority tasks
			task_assert_exec(i);
		} else {
			//Disable process execution for lower priority tasks
			task_deassert_exec(i);
		}
	}

	if ( switch_context == true ){
		task_priv_switch_context(NULL);
	}
}


int sched_get_highest_priority_blocked(void * block_object){
	int priority;
	int i;
	int new_thread;

	//check to see if another task is waiting for the mutex
	priority = SCHED_LOWEST_PRIORITY - 1;
	new_thread = -1;
	for(i=1; i < task_get_total(); i++){
		if ( task_enabled(i) ){
			if ( (sched_table[i].block_object == block_object) && ( !sched_active_asserted(i) ) ){
				//it's waiting for the block -- give the block to the highest priority and waiting longest
				if ( sched_table[i].attr.schedparam.sched_priority > priority ){

					//! \todo Find the task that has been waiting the longest time
					new_thread = i;
					priority = sched_table[i].attr.schedparam.sched_priority;
				}
			}
		}
	}
	return new_thread;
}

int sched_priv_unblock_all(void * block_object, int unblock_type){
	int i;
	int priority;
	priority = SCHED_LOWEST_PRIORITY - 1;
	for(i=1; i < task_get_total(); i++){
		if ( task_enabled(i) ){
			if ( (sched_table[i].block_object == block_object) && ( !sched_active_asserted(i) ) ){
				//it's waiting for the semaphore -- give the semaphore to the highest priority and waiting longest
				sched_priv_assert_active(i, unblock_type);
				if ( sched_table[i].attr.schedparam.sched_priority > priority ){
					priority = sched_table[i].attr.schedparam.sched_priority;
				}
			}
		}
	}
	return priority;
}

int start_first_thread(void){
	void * (*init)(void*);
	pthread_attr_t attr;
	int err;

	init = sched_table[0].init;

	attr.stacksize = SCHED_FIRST_THREAD_STACK_SIZE;
	attr.stackaddr = malloc(attr.stacksize + sizeof(struct _reent) + SCHED_DEFAULT_STACKGUARD_SIZE);
	if ( attr.stackaddr == NULL ){
		errno = ENOMEM;
		return -1;
	}
	PTHREAD_ATTR_SET_IS_INITIALIZED((&attr), 1);
	PTHREAD_ATTR_SET_CONTENTION_SCOPE((&attr), PTHREAD_SCOPE_SYSTEM);
	PTHREAD_ATTR_SET_GUARDSIZE((&attr), SCHED_DEFAULT_STACKGUARD_SIZE);
	PTHREAD_ATTR_SET_INHERIT_SCHED((&attr), PTHREAD_EXPLICIT_SCHED);
	PTHREAD_ATTR_SET_DETACH_STATE((&attr), PTHREAD_CREATE_DETACHED);
	PTHREAD_ATTR_SET_SCHED_POLICY((&attr), SCHED_RR);
	attr.schedparam.sched_priority = 21; //not the default priority

	err = sched_new_thread(init,
			NULL,
			attr.stackaddr,
			attr.stacksize,
			&attr);

	if ( !err ){
		return -1;
	}

	return 0;
}




/*! @} */


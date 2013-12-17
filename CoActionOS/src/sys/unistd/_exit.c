/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNISTD
 * @{
 */

/*! \file */

#include "config.h"
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/reent.h>
#include <errno.h>

#include "../sched/sched_flags.h"
#include "../signal/sig_local.h"
#include "sysfs.h"

static void priv_stop_threads(int * send_signal);
static void priv_zombie_process(int * signal_sent);

void exit(int status);

/*! \details This function causes the calling process
 * to exit with the specified exit code.
 *
 * \note In this version, named semaphores are not closed in this function.
 *
 * \return This function never returns
 */
void _exit(int __status){
#if SINGLE_PROCESS == 0
	int send_signal;
	int tmp;
	int i;
	int sent;

	tmp = (__status >> 8) & 0xff; //the signal number if terminated by a signal
	send_signal = __status & 0xff; //this is the 8-bit exit code
	__status = (send_signal << 8) | tmp;

	//Stop all the threads in the process
	send_signal = __status;
	hwpl_core_privcall((core_privcall_t)priv_stop_threads, &send_signal);

	// todo close named semaphores


	// todo cancel any async IO

	//Make sure all open file descriptors are closed
	for(i=0; i < OPEN_MAX; i++){
		close(i); //make sure all file descriptors are closed
	}

	//unlock any locks on the filesystems
	sysfs_unlock();

	//If this is a controlling process, it should send SIGHUP to each foreground process
	if ( send_signal == true ){
		sent = false;
		tmp = task_get_pid( task_get_parent( task_get_current() ) );
		for(i=0; i < task_get_total(); i++){
			//send SIGCHLD to each thread of parent
			if ( (tmp == task_get_pid(i)) && (task_enabled(i)) ){
				if( signal_send(i, SIGCHLD, SI_USER, __status) < 0 ){
					hwpl_debug("Parent failed to receive signal %d\n", errno);
				} else {
					sent = true;
				}
			}
		}

		if( sent == false ){
			send_signal = false; //don't be a zombie if the parent failed to receive the signal
		}

	} else {

	}

	while(1){ //if the process turns in to a zombie it should resume zombie mode if it receives a signal
		//This process will be a zombie process until it is disabled by the parent
		hwpl_core_privcall((core_privcall_t)priv_zombie_process, &send_signal);
	}
#else
	while(1);
#endif
}

#if SINGLE_PROCESS == 0

void priv_stop_threads(int * send_signal){
	int i;
	int tmp;
	struct _reent * parent_reent;

	//set the exit status
	sched_table[task_get_current()].exit_status = *send_signal;

	//Kill all other threads in process
	tmp = task_get_pid( task_get_current() );
	//Terminate the threads in this process
	for(i=1; i < task_get_total(); i++){
		if ( task_get_pid(i) == tmp ){
			if ( i != task_get_current() ){
				sched_table[i].flags = 0;
				task_priv_del(i);
			}
		}
	}

	//now check for SA_NOCLDWAIT in parent process
	tmp = task_get_parent( task_get_current() );
	parent_reent = (struct _reent *)task_table[tmp].global_reent;

	if ( task_get_pid(tmp) == 0 ){
		//process 0 never waits
		*send_signal = false;
	} else {
		*send_signal = true;
		if( parent_reent->procmem_base->sigactions != NULL ){
			if ( parent_reent->procmem_base->sigactions[SIGCHLD] != NULL ) {
				if ( (parent_reent->procmem_base->sigactions[SIGCHLD]->sa_flags & (1<<SA_NOCLDWAIT)) ||
						parent_reent->procmem_base->sigactions[SIGCHLD]->sa_handler == SIG_IGN){
					//do not send SIGCHLD -- do not be a zombie process

					*send_signal = false;
				}
			}
		}
	}

	if ( sched_zombie_asserted(task_get_current()) ){
		*send_signal = false;
	} else {
		sched_table[task_get_current()].flags |= (1<< SCHED_TASK_FLAGS_ZOMBIE);
	}
}

void priv_zombie_process(int * signal_sent){

	if ( *signal_sent == false ){
		//discard this thread immediately
		hwpl_priv_debug("Self kill %d\n", task_get_current());
		sched_table[task_get_current()].flags = 0;
		task_priv_del(task_get_current());
	} else {
		hwpl_priv_debug("Parent kill %d\n", task_get_current());
		//the parent is waiting -- set this thread to a zombie thread
		sched_priv_deassert_inuse(task_get_current());
	}

	sched_priv_update_on_sleep();
}

#endif


/*! @} */

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SIGNAL
 * @{
 */

/*! \file */

#include "config.h"
#include "hwpl.h"
#include "hwpl/task.h"
#include "hwpl/debug.h"
#include "caos.h"
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "../sched/sched_flags.h"
#include "sig_local.h"

#include "hwpl/debug.h"

static int signal_priv_forward(int send_tid, int tid, int si_signo, int si_sigcode, int sig_value);
static void signal_forward_handler(int send_tid, int signo, int sigcode, int sigvalue);

//this checks to see if sending a signal will cause a stack/heap collision in the target thread
static void signal_priv_check_stack(void * args){
	int * arg = (int*)args;
	int tid = *arg;
	int ret = 0;
	uint32_t sp;

	//Check to see if stacking an interrupt handler will cause a stack heap collision
	if( tid != task_get_current() ){
		//check the target stack pointer
		sp = (uint32_t)task_table[tid].sp;
	} else {
		//read the current stack pointer
		_hwpl_core_priv_get_thread_stack_ptr(&sp);
	}

	if( (sp - task_interrupt_stacksize() - (4*SCHED_DEFAULT_STACKGUARD_SIZE)) < //stackguard * 4 gives the handler a little bit of memory
			(uint32_t)(task_table[tid].mem.stackguard.addr) ){
		ret = -1;
	}

	*arg = ret;
}

int signal_callback(void * context, const void * data){
	//This only works if the other parts of the interrupt handler have not modified the stack
	signal_callback_t * args = (signal_callback_t*)context;
	if( signal_priv_send(0, args->tid, args->si_signo, args->si_sigcode, args->sig_value, 1) < 0){
		return 0; //this will dis-regard the callback so additional events stop sending signals
	}
	return args->keep; //non-zero return means to leave callback in place
}

void signal_forward_handler(int send_tid, int signo, int sigcode, int sigvalue){
	//This is called in a non-privileged context on task 0
	signal_send(send_tid, signo, sigcode, sigvalue);
}

int signal_priv_forward(int send_tid, int tid, int si_signo, int si_sigcode, int sig_value){
	task_interrupt_t intr;
	int check_stack;

	//make sure the task id is valid
	if ( (uint32_t)tid < task_get_total() ){
		if ( si_signo != 0 ){
			if ( si_signo < SCHED_NUM_SIGNALS ){

				check_stack = tid;
				signal_priv_check_stack(&check_stack);
				if( check_stack < 0 ){
					errno = ENOMEM;
					return -1;
				}


				intr.tid = tid;
				intr.handler = (task_interrupt_handler_t)signal_forward_handler;
				intr.sync_callback = (core_privcall_t)signal_priv_activate;
				intr.sync_callback_arg = &tid;
				intr.arg[0] = send_tid;
				intr.arg[1] = si_signo;
				intr.arg[2] = si_sigcode;
				intr.arg[3] = sig_value;
				task_priv_interrupt(&intr);
			} else {
				return -1;
			}
		}
	} else {
		return -1;
	}
	return 0;
}


int signal_priv_send(int send_tid, int tid, int si_signo, int si_sigcode, int sig_value, int forward){
	task_interrupt_t intr;
	int check_stack;

	if( (tid == task_get_current()) && (forward != 0) ){
		//If the receiving tid is currently executing, sending the signal directly will corrupt the stack
		//So we stack a signal on task 0 and have it send the signal
		//This only happens when priv signals are sent because the stack is in an unknown state
		return signal_priv_forward(tid, 0, si_signo, si_sigcode, sig_value);
	}

	//make sure the task id is valid
	if ( (uint32_t)tid < task_get_total() ){
		if ( si_signo != 0 ){
			if ( si_signo < SCHED_NUM_SIGNALS ){

				check_stack = tid;
				signal_priv_check_stack(&check_stack);
				if( check_stack < 0 ){
					errno = ENOMEM;
					return -1;
				}


				intr.tid = tid;
				intr.handler = (task_interrupt_handler_t)signal_handler;
				intr.sync_callback = (core_privcall_t)signal_priv_activate;
				intr.sync_callback_arg = &tid;
				intr.arg[0] = send_tid;
				intr.arg[1] = si_signo;
				intr.arg[2] = si_sigcode;
				intr.arg[3] = sig_value;
				task_priv_interrupt(&intr);
			} else {
				return -1;
			}
		}
	} else {
		return -1;
	}
	return 0;
}


int signal_send(int tid, int si_signo, int si_sigcode, int sig_value){
	task_interrupt_t intr;
	pthread_mutex_t * delay_mutex;
	int check_stack;

	//make sure the task id is valid
	if ( sched_check_tid(tid) ){
		errno = ESRCH;
		return -1;
	}

	if ( si_signo != 0 ){

		delay_mutex = sched_table[tid].signal_delay_mutex;
		//check for the signal delay mutex
		if( delay_mutex != NULL ){
			//this means the target task is doing critical work (like modifying the filesystem)
			if ( pthread_mutex_lock(delay_mutex) == 0 ){ //wait until the task releases the mutex
				pthread_mutex_unlock(delay_mutex);  //unlock the mutex then continue
			}
		}

		if ( si_signo < SCHED_NUM_SIGNALS ){

			check_stack = tid;
			hwpl_core_privcall(signal_priv_check_stack, &check_stack);
			if( check_stack < 0 ){
				errno = ENOMEM;
				return -1;
			}

			intr.tid = tid;
			intr.handler = (task_interrupt_handler_t)signal_handler;
			intr.sync_callback = (core_privcall_t)signal_priv_activate;
			intr.sync_callback_arg = &tid;
			intr.arg[0] = task_get_current();
			intr.arg[1] = si_signo;
			intr.arg[2] = si_sigcode;
			intr.arg[3] = sig_value;
			task_interrupt(&intr);
		} else {
			errno = EINVAL;
			return -1;
		}
	}
	return 0;
}

/*! \details This function sends the signal \a signo to \a thread.
 * The handler is executed in the context of \a thread, but the
 * signal effects the entire process.  For example,
 * \code
 * pthread_kill(8, SIGKILL);
 * \endcode
 * will kill the process that holds thread 8.  The exit handler
 * will be executed on thread 8's stack.
 *
 *
 * \return Zero or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL: \a signo is not a valid signal number
 * - ESRCH: \a pid is not a valid process id
 *
 */
int pthread_kill(pthread_t thread, int signo){
	return signal_send(thread, signo, SI_USER, 0);
}

void signal_priv_activate(int * thread){
	int id = *thread;
	sched_priv_deassert_stopped(id);
	sched_priv_assert_active(id, SCHED_UNBLOCK_SIGNAL);
	sched_priv_update_on_wake( sched_get_priority(id) );
}



/*! @} */

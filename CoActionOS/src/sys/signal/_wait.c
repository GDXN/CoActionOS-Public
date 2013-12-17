/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SIGNAL
 * @{
 */

/*! \file */

#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include "../sched/sched_flags.h"
#include "sig_local.h"

static void priv_wait_child(void * args);

typedef struct {
	int pid;
	int status;
	int tid;
} priv_check_for_zombie_child_t;

static void priv_check_for_zombie_child(void * args);

pid_t waitpid(pid_t pid, int *stat_loc, int options){
	priv_check_for_zombie_child_t args;

	if ( (pid < -1) || (pid == 0) ){
		errno = ENOTSUP;
		return -1;
	}

	args.pid = pid;
	hwpl_core_privcall(priv_check_for_zombie_child, &args);
	if ( args.tid > 0 ){
		if ( stat_loc != NULL ){
			*stat_loc = args.status;
		}
		return task_get_pid( args.tid );
	}

	if ( options & WNOHANG ){
		return 0;
	}


	//wait for SIGCHLD or another caught signal
	do {
		hwpl_core_privcall(priv_wait_child, &args);
	} while( !SIGCAUGHT_ASSERTED() && (args.tid == 0) );


	if ( args.tid == 0 ){
		//another signal interrupted the call
		errno = EINTR;
		return -1;
	}

	//no more children
	if ( args.tid < 0 ){
		errno = ECHILD;
		return -1;
	}

	//Read the status information from the child
	if ( stat_loc != NULL ){
		*stat_loc = args.status;
	}

	//return the pid of the child process
	return task_get_pid( args.tid );
}

pid_t _wait(int *stat_loc){
	return waitpid(-1, stat_loc, 0);
}

void priv_check_for_zombie_child(void * args){
	int num_children;
	priv_check_for_zombie_child_t * p;
	p = (priv_check_for_zombie_child_t*)args;
	int i;
	num_children = 0;
	for(i=1; i < task_get_total(); i++){
		if( task_get_pid( task_get_parent(i) ) == task_get_pid( task_get_current() ) ){ //is the task a child
			num_children++;
			if ( sched_zombie_asserted(i) ){
				//this zombie process is ready
				if ( (task_get_pid(i) == p->pid) || (p->pid == -1) ){ //matching pid or any pid?
					p->tid = i;
					p->status = sched_table[i].exit_status;
					sched_table[i].flags = 0;
					task_priv_del(i);
					return;
				}
			}
		}
	}
	if ( num_children > 0 ){
		p->tid = 0;
	} else {
		p->tid = -1;
	}
}

void priv_wait_child(void * args){
	//see if SIGCHLD is blocked and the status is available now
	priv_check_for_zombie_child_t * p;
	p = (priv_check_for_zombie_child_t*)args;

	priv_check_for_zombie_child(args);
	if ( p->tid > 0 ){
		SIGCHLD_ASSERT();
	} else {
		SIGCAUGHT_DEASSERT();
		SIGCHLD_DEASSERT();
		sched_priv_update_on_sleep(); //Sleep the current thread
	}
}

/*! @} */

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
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>

#include "sched_flags.h"

typedef struct {
	task_memories_t * mem;
	int tid;
} init_sched_task_t;

static void priv_init_sched_task(init_sched_task_t * task);
static void cleanup_process(void * status);

/*! \details This function creates a new process.
 * \return The thread id or zero if the thread could not be created.
 */
int sched_new_process(void (*p)(int, char* const*)  /*! The startup function (crt()) */,
		int argc/*! The first argument to main() */,
		char *const argv[] /*! The second argument to main() */,
		task_memories_t * mem,
		void * reent /*! The location of the reent structure */){
	int tid;
	init_sched_task_t args;

	//Start a new process
	tid = task_new_process(
			p,
			cleanup_process,
			argc,
			argv,
			mem,
			reent
	);

	if ( tid > 0 ){
		//update the scheduler table using a privileged call
		args.tid = tid;
		args.mem = mem;
		hwpl_core_privcall((core_privcall_t)priv_init_sched_task, &args);
	} else {
		return -1;
	}

	return task_get_pid( tid );
}

void priv_init_sched_task(init_sched_task_t * task){
	uint32_t stackguard;
	struct _reent * reent;
	int id = task->tid;
	memset((void*)&sched_table[id], 0, sizeof(sched_task_t));

	PTHREAD_ATTR_SET_IS_INITIALIZED((&(sched_table[id].attr)), 1);
	PTHREAD_ATTR_SET_SCHED_POLICY((&(sched_table[id].attr)), SCHED_OTHER);
	PTHREAD_ATTR_SET_GUARDSIZE((&(sched_table[id].attr)), SCHED_DEFAULT_STACKGUARD_SIZE);
	PTHREAD_ATTR_SET_CONTENTION_SCOPE((&(sched_table[id].attr)), PTHREAD_SCOPE_SYSTEM);
	PTHREAD_ATTR_SET_INHERIT_SCHED((&(sched_table[id].attr)), PTHREAD_EXPLICIT_SCHED);
	PTHREAD_ATTR_SET_DETACH_STATE((&(sched_table[id].attr)), PTHREAD_CREATE_DETACHED);

	sched_table[id].attr.stackaddr = task->mem->data.addr; //Beginning of process data memory
	sched_table[id].attr.stacksize = task->mem->data.size; //Size of the memory (not just the stack)
	sched_table[id].priority = 0; //This is the default starting priority priority
	sched_table[id].attr.schedparam.sched_priority = 0; //This is the priority to revert to after being escalated

	sched_table[id].wake.tv_sec = SCHED_TIMEVAL_SEC_INVALID;
	sched_table[id].wake.tv_usec = 0;
	sched_priv_assert_active(id, 0);
	sched_priv_assert_inuse(id);
	sched_priv_update_on_wake( sched_table[id].priority );
	stackguard = (uint32_t)task->mem->data.addr + task->mem->data.size - 128;
#if USE_MEMORY_PROTECTION > 0
	task_priv_set_stackguard(id, (void*)stackguard, SCHED_DEFAULT_STACKGUARD_SIZE);
#endif

	//Items inherited from parent process

	//Signal mask
	reent = (struct _reent *)task_table[id].reent;
	reent->sigmask = _REENT->sigmask;
}

static void cleanup_process(void * status){
	//Processes should ALWAYS use exit -- this should never get called but is here just in case
	kill(task_get_pid(task_get_current()), SIGKILL);
}


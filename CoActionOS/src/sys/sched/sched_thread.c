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
#include <pthread.h>
#include <reent.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>

#include "hwpl/debug.h"

#include "sched_flags.h"
#include "../syscalls/malloc_local.h"


static void cleanup_thread(void * status);

typedef struct {
	int joined;
	int status;
} priv_wait_joined_t;
static void priv_wait_joined(void * args);
static void priv_cleanup(void * args);

typedef struct {
	int id;
	pthread_attr_t * attr;
	void * stackguard;
} priv_activate_thread_t;
static void priv_activate_thread(priv_activate_thread_t * args);


/*! \details This function creates a new thread.
 * \return The thread id or zero if the thread could not be created.
 */
int sched_new_thread(void *(*p)(void*)  /*! The function to execute for the task */,
		void * arg /*! The thread's single argument */,
		void * mem_addr /*! The address for the thread memory (bottom of the stack) */,
		int mem_size /*! The stack size in bytes */,
		pthread_attr_t * attr){
	int id;
	struct _reent * reent;
	priv_activate_thread_t args;

	//start a new thread
	id = task_new_thread(p, cleanup_thread, arg, mem_addr, mem_size, task_get_pid( task_get_current() ) );

	if ( id > 0 ){
		//Initialize the reent structure
		reent = (struct _reent *)mem_addr; //The bottom of the stack
		_REENT_INIT_PTR(reent);
		reent->procmem_base = _GLOBAL_REENT->procmem_base; //malloc use the same base as the process

		//Now activate the thread
		args.id = id;
		args.attr = attr;
		args.stackguard = (void*)((uint32_t)mem_addr + sizeof(struct _reent));
		hwpl_core_privcall((core_privcall_t)priv_activate_thread, &args);
	}
	return id;
}

/*! \details This function activates a thread.  It needs to be called
 * when a new thread is created.
 *
 */
void priv_activate_thread(priv_activate_thread_t * args){
	int id;
	id = args->id;
	struct _reent * reent;
	memset( (void*)&sched_table[id], 0, sizeof(sched_task_t));
	memcpy( (void*)&(sched_table[id].attr), args->attr, sizeof(pthread_attr_t));
	sched_table[args->id].priority = args->attr->schedparam.sched_priority;
	if ( PTHREAD_ATTR_GET_SCHED_POLICY( (&(sched_table[id].attr)) ) == SCHED_FIFO ){
		task_assert_isfifo(id);
	} else {
		task_deassert_isfifo(id);
	}
	sched_table[id].wake.tv_sec = SCHED_TIMEVAL_SEC_INVALID;
	sched_table[id].wake.tv_usec = 0;
	sched_priv_assert_active(id, 0);
	sched_priv_assert_inuse(id);
#if USE_MEMORY_PROTECTION > 0
	task_priv_set_stackguard(id, args->stackguard, SCHED_DEFAULT_STACKGUARD_SIZE);
#endif
	sched_priv_update_on_wake(sched_table[id].priority);

	//Items inherited from parent thread

	//Signal mask
	reent = (struct _reent *)task_table[id].reent;
	reent->sigmask = _REENT->sigmask;
}

void cleanup_thread(void * status){
	int detach_state;
	priv_wait_joined_t args;

#if USE_STDIO != 0


	//Needs to free any buffers created for stdio
	if ( (stdin != NULL) && (stdin != (FILE*)&__sf_fake_stdin) ){
		//check for allocated buffers
		fclose(stdin);
	}

	if ( (stdout != NULL) && (stdout != (FILE*)&__sf_fake_stdout) ){
		fclose(stdout);
	}

	if ( (stderr != NULL) && (stderr != (FILE*)&__sf_fake_stderr) ){
		fclose(stderr);
	}


	//This will close any other open files
	if ( _REENT->__cleanup ){
		_REENT->__cleanup(_REENT);
	}


#endif

	detach_state = PTHREAD_ATTR_GET_DETACH_STATE( (&(sched_table[task_get_current()].attr)) );
	args.joined = 0;
	if ( detach_state == PTHREAD_CREATE_JOINABLE ){
		args.status = (int)status;
		do {
			hwpl_core_privcall(priv_wait_joined, &args);
		} while(args.joined == 0);
	}

	//Free all memory associated with this thread
	malloc_free_task_r(_REENT, task_get_current() );
	free( sched_table[task_get_current()].attr.stackaddr );
	hwpl_core_privcall(priv_cleanup, &args.joined);
}

void priv_wait_joined(void * args){
	int joined;
	priv_wait_joined_t * p = (priv_wait_joined_t*)args;

	//wait until the thread has been joined to free the resources
	for(joined=1; joined < task_get_total(); joined++){
		//check to see if any threads are blocked on this thread
		if ( sched_table[joined].block_object == &sched_table[task_get_current()] ){
			//This thread is joined to the current thread
			p->joined = joined;
			//the thread can continue when one thread has been joined
			break;
		}
	}

	if ( p->joined == 0 ){
		sched_table[task_get_current()].block_object = (void*)&sched_table[task_get_current()].block_object; //block on self
		sched_priv_update_on_sleep();
	} else {
		sched_table[task_get_current()].exit_status = p->status;
	}
}

void priv_cleanup(void * args){
	int joined;
	//notify all joined threads of termination
	for(joined=1; joined < task_get_total(); joined++){
		//check to see if any threads are blocked on this thread
		if ( sched_table[joined].block_object == &sched_table[task_get_current()] ){
			//This thread is joined to the current thread
			sched_table[joined].exit_status = sched_table[task_get_current()].exit_status;
			sched_priv_assert_active(joined, SCHED_UNBLOCK_PTHREAD_JOINED_THREAD_COMPLETE);
		}
	}

	sched_table[task_get_current()].flags = 0;
	task_priv_del(task_get_current());
	sched_priv_update_on_sleep();
}

/*! @} */

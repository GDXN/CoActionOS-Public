/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PTHREAD
 * @{
 *
 * \details This is the interface for POSIX threads.  Here is an example of creating a new thread:
 *
 * \code
 * #include <pthread.h>
 *
 * void * thread_function(void * args);
 *
 * pthread_t t;
 * pthread_attr_t attr;
 *
 * if ( pthread_attr_init(&attr) < 0 ){
 * 	perror("failed to initialize attr");
 * 	return -1;
 * }
 *
 * if ( pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) < 0 ){
 * 	perror("failed to set detach state");
 * 	return -1;
 * }
 *
 * if ( pthread_attr_setstacksize(&attr, 4096) < 0 ){
 * 	perror("failed to set stack size");
 * 	return -1;
 * }
 *
 * if ( pthread_create(&t, &attr, thread_function, NULL) ){
 * 	perror("failed to create thread");
 * 	return -1;
 * }
 *
 * \endcode
 *
 */

/*! \file */

#include "config.h"

#include <pthread.h>
#include <errno.h>


#include "../sched/sched_flags.h"


static void priv_join_thread(void * args);

/*! \details This function creates a new thread.
 * \return Zero on success or -1 with \a errno (see \ref ERRNO) set to:
 * - ENOMEM: error allocating memory for the thread
 * - EAGAIN: insufficient system resources to create a new thread
 *
 *
 */
int pthread_create(pthread_t * thread /*! If not null, the thread id is written here */,
		const pthread_attr_t * attr /*! Sets the thread attributes (defaults are used if this is NULL) */,
		void *(*start_routine)(void *) /*! A pointer to the start routine */,
		void *arg /*! A pointer to the start routine's single argument */){

	int id;
	pthread_attr_t attrs;

	if ( attr == NULL ){

		if ( pthread_attr_init(&attrs) < 0 ){
			//Errno is set by pthread_attr_init()
			return -1;
		}
	} else {
		memcpy(&attrs, attr, sizeof(pthread_attr_t));
	}

	id = sched_new_thread(start_routine,
			arg,
			attrs.stackaddr,
			attrs.stacksize,
			&attrs);

	if ( id ){
		if ( thread ){
			*thread = id;
		}
		return 0;
	} else {
		if ( attr == NULL ){
			pthread_attr_destroy(&attrs);
		}
		errno = EAGAIN;
		return -1;
	}
}


/*! \details This function blocks the calling thread until \a thread terminates.
 * \return Zero on success or -1 with \a errno (see \ref ERRNO) set to:
 * - ESRCH: \a thread does not exist
 * - EDEADLK: a deadlock has been detected or \a thread refers to the calling thread
 * - EINVAL: \a thread does not refer to a joinable thread.
 */
int pthread_join(pthread_t thread, void ** value_ptr){

	if( (thread < task_get_total()) && (thread >= 0) ){
		if ( task_enabled(thread) ){
			//now see if the thread is joinable
			if ( PTHREAD_ATTR_GET_DETACH_STATE( (&(sched_table[thread].attr))) != PTHREAD_CREATE_JOINABLE ){
				errno = EINVAL;
				return -1;
			}


			//See if the thread is joined to this thread
			if ( (sched_table[thread].block_object == (void*)&sched_table[task_get_current()]) ||
					(thread == task_get_current()) ){
				errno = EDEADLK;
				return -1;
			}


			do {
				hwpl_core_privcall(priv_join_thread, &thread);
				if ( thread < 0 ){
					errno = ESRCH;
					return -1;
				}
			} while( sched_get_unblock_type(task_get_current()) != SCHED_UNBLOCK_PTHREAD_JOINED_THREAD_COMPLETE);

			if ( value_ptr != NULL ){
				//When the thread terminates, it puts the exit value in this threads scheduler table entry
				*value_ptr = (void*)(sched_table[ task_get_current() ].exit_status);
			}

			return 0;
		}
	}
	errno = ESRCH;
	return -1;
}

void priv_join_thread(void * args){
	int * p = (int*)args;
	int id = *p;

	if ( task_enabled(id) ){
		sched_table[task_get_current()].block_object = (void*)&sched_table[id]; //block on the thread to be joined
		//If the thread is waiting to be joined, it needs to be activated
		if ( sched_table[id].block_object == (void*)&sched_table[id].block_object ){
			sched_priv_assert_active(id, SCHED_UNBLOCK_PTHREAD_JOINED);
		}
		sched_priv_update_on_sleep();
	} else {
		*p = -1;
	}
}



/*! @} */

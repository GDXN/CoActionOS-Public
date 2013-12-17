/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


/*! \addtogroup PTHREAD_COND Conditional Variables
 * @{
 *
 * \ingroup PTHREAD
 *
 */

/*! \file */

#include "config.h"

#include <pthread.h>
#include <errno.h>

#include "../sched/sched_flags.h"

#define PSHARED_FLAG 31
#define INIT_FLAG 30
#define PID_MASK 0x3FFFFFFF

static void priv_cond_signal(void * args);

typedef struct {
	pthread_cond_t *cond;
	pthread_mutex_t *mutex;
	int new_thread;
	struct sched_timeval interval;
	int ret;
} priv_cond_wait_t;
static void priv_cond_wait(void  * args);
static void priv_cond_broadcast(void * args);

/*! \details This function initializes a pthread block condition.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL: cond or attr is NULL
 */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr){
	if ( (cond == NULL) || (attr == NULL) ){
		errno = EINVAL;
		return -1;
	}

	*cond = getpid() | (1<<INIT_FLAG);
	if ( attr->process_shared != 0 ){
		*cond |= (1<<PSHARED_FLAG);
	}
	return 0;
}

/*! \details This function destroys a pthread block condition.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL: cond is NULL
 */
int pthread_cond_destroy(pthread_cond_t *cond){
	if ( cond == NULL ){
		errno = EINVAL;
		return -1;
	}

	if ( (*cond & (1<<INIT_FLAG)) == 0 ){
		return EINVAL;
		return -1;
	}

	*cond = 0;
	return 0;
}

void priv_cond_broadcast(void * args){
	int prio;
	prio = sched_priv_unblock_all(args, SCHED_UNBLOCK_COND);
	sched_priv_update_on_wake(prio);
}

/*! \details This function wakes all threads that are blocked on \a cond.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL: cond is NULL or not initialized
 */
int pthread_cond_broadcast(pthread_cond_t *cond){
	if ( cond == NULL ){
		errno = EINVAL;
		return -1;
	}

	if ( (*cond & (1<<INIT_FLAG)) == 0 ){
		errno = EINVAL;
		return -1;
	}

	//wake all tasks blocking on cond
	hwpl_core_privcall(priv_cond_broadcast, cond);
	return 0;
}

void priv_cond_signal(void * args){
	int id = *((int*)args);
	sched_priv_assert_active(id, SCHED_UNBLOCK_COND);
	sched_priv_update_on_wake( sched_table[id].priority );
}

/*! \details This function wakes the highest priority thread
 * that is blocked on \a cond.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL: cond is NULL or not initialized
 */
int pthread_cond_signal(pthread_cond_t *cond){
	int new_thread;

	if ( cond == NULL ){
		errno = EINVAL;
		return -1;
	}

	if ( (*cond & (1<<INIT_FLAG)) == 0 ){
		return EINVAL;
		return -1;
	}

	new_thread = sched_get_highest_priority_blocked(cond);

	if ( new_thread != -1 ){
		hwpl_core_privcall(priv_cond_signal, &new_thread);
	}

	return 0;
}

void priv_cond_wait(void  * args){
	priv_cond_wait_t * argsp = (priv_cond_wait_t*)args;
	int new_thread = argsp->new_thread;


	if ( argsp->mutex->pthread == task_get_current() ){
		//First unlock the mutex
		//Restore the priority to the task that is unlocking the mutex
		sched_table[task_get_current()].priority = sched_table[task_get_current()].attr.schedparam.sched_priority;

		if ( new_thread != -1 ){
			argsp->mutex->pthread = new_thread;
			argsp->mutex->pid = task_get_pid(new_thread);
			argsp->mutex->lock = 1;
			sched_table[new_thread].priority = argsp->mutex->prio_ceiling;
			sched_priv_assert_active(new_thread, SCHED_UNBLOCK_MUTEX);
		} else {
			argsp->mutex->lock = 0;
			argsp->mutex->pthread = -1; //The mutex is up for grabs
		}

		sched_priv_timedblock(argsp->cond, &argsp->interval);
		argsp->ret = 0;
	} else {
		argsp->ret = -1;
	}

}


/*! \details This function causes the calling thread to block
 * on \a cond. When called, \a mutex must be locked by the caller.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL:  cond is NULL or not initialized
 * - EACCES:  cond is from a different process and not shared
 * - EPERM:  the caller does not have a lock on \a mutex
 */
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
	int pid;
	priv_cond_wait_t args;

	if ( cond == NULL ){
		errno = EINVAL;
		return -1;
	}

	if ( (*cond & (1<<INIT_FLAG)) == 0 ){
		return EINVAL;
		return -1;
	}

	pid = *cond & PID_MASK;

	if ( (*cond & (1<<PSHARED_FLAG)) == 0 ){
		if ( pid != getpid() ){ //This is a different process with a not pshared cond
			errno = EACCES;
			return -1;
		}
	}

	args.cond = cond;
	args.mutex = mutex;
	args.interval.tv_sec = SCHED_TIMEVAL_SEC_INVALID;
	args.interval.tv_usec = 0;

	//release the mutex and block on the cond
	args.new_thread = sched_get_highest_priority_blocked(mutex);
	hwpl_core_privcall(priv_cond_wait, &args);

	if ( args.ret == -1 ){
		errno = EPERM;
		return -1;
	}

	return 0;
}

/*! \details This function causes the calling thread to block
 * on \a cond. When called, \a mutex must be locked by the caller.  If \a cond does
 * not wake the process by \a abstime, the thread resumes.
 *
 * Example:
 * \code
 * struct timespec abstime;
 * clock_gettime(CLOCK_REALTIME, &abstime);
 * abstime.tv_sec += 5; //time out after five seconds
 * if ( pthread_cond_timedwait(cond, mutex, &abstime) == -1 ){
 * 	if ( errno == ETIMEDOUT ){
 * 		//Timedout
 * 	} else {
 * 		//Failed
 * 	}
 * }
 * \endcode
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL:  cond is NULL or not initialized
 * - EACCES:  cond is from a different process and not shared
 * - EPERM:  the caller does not have a lock on \a mutex
 * - ETIMEDOUT:  \a abstime passed before \a cond arrived
 *
 */
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime){
	int pid;
	priv_cond_wait_t args;

	if ( cond == NULL ){
		errno = EINVAL;
		return -1;
	}

	if ( (*cond & (1<<INIT_FLAG)) == 0 ){
		return EINVAL;
		return -1;
	}

	pid = *cond & PID_MASK;

	if ( (*cond & (1<<PSHARED_FLAG)) == 0 ){
		if ( pid != getpid() ){ //This is a different process with a not pshared cond
			errno = EACCES;
			return -1;
		}
	}

	args.cond = cond;
	args.mutex = mutex;
	sched_convert_timespec(&args.interval, abstime);


	//release the mutex and block on the cond
	args.new_thread = sched_get_highest_priority_blocked(mutex);
	hwpl_core_privcall(priv_cond_wait, &args);

	if ( args.ret == -1 ){
		errno = EPERM;
		return -1;
	}

	if ( sched_get_unblock_type( task_get_current() ) == SCHED_UNBLOCK_SLEEP ){
		errno = ETIMEDOUT;
		return -1;
	}

	return 0;
}

/*! @} */


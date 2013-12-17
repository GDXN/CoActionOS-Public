/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


/*! \addtogroup PTHREAD_MUTEX Mutexes
 * @{
 *
 * \ingroup PTHREAD
 *
 */

/*! \file */


#include "config.h"

#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>

#include "hwpl/debug.h"
#include "../sched/sched_flags.h"

static int check_initialized(const pthread_mutex_t * mutex);
static int mutex_trylock(pthread_mutex_t *mutex, bool trylock, const struct timespec * abs_timeout);
typedef struct {
	int id;
	pthread_mutex_t *mutex;
	bool trylock;
	struct sched_timeval abs_timeout;
	int ret;
} priv_mutex_trylock_t;
static void priv_mutex_trylock(priv_mutex_trylock_t *args);

typedef struct {
	int id;
	pthread_mutex_t *mutex;
} priv_mutex_unlock_t;
static void priv_mutex_unlock(priv_mutex_unlock_t * args);


/*! \details This function initializes \a mutex with \a attr.
 * \return Zero on success or -1 with \a errno (see \ref ERRNO) set to:
 * - EINVAL: mutex is NULL
 */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr){
#if SINGLE_TASK == 0
	if ( mutex == NULL ){
		errno = EINVAL;
		return -1;
	}

	if ( attr == NULL ){
		mutex->flags = PTHREAD_MUTEX_FLAGS_INITIALIZED;
		mutex->prio_ceiling = PTHREAD_MUTEX_PRIO_CEILING;
		if ( task_get_current() == 0 ){
			mutex->pid = 0;
		} else {
			mutex->pid = task_get_pid( task_get_current() );
		}
		mutex->lock = 0;
		mutex->pthread = -1;
		return 0;
	}

	mutex->flags = PTHREAD_MUTEX_FLAGS_INITIALIZED;
	if ( attr->process_shared == true ){
		//Enter priv mode to modify a shared object
		mutex->flags |= (PTHREAD_MUTEX_FLAGS_PSHARED);
	}

	if ( attr->recursive ){
		mutex->flags |= (PTHREAD_MUTEX_FLAGS_RECURSIVE);
	}
	mutex->prio_ceiling = attr->prio_ceiling;
	mutex->pid = task_get_pid( task_get_current() );
	mutex->lock = 0;
	mutex->pthread = -1;
	return 0;
#else
	return 0;
#endif
}

/*! \details This function locks \a mutex.  If \a mutex cannot be locked immediately,
 * the thread is blocked until \a mutex is available.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL:  mutex is NULL
 * - EDEADLK:  the caller already holds the mutex
 * - ETIMEDOUT:  \a abstime passed before \a cond arrived
 *
 */
int pthread_mutex_lock(pthread_mutex_t * mutex){
#if SINGLE_TASK == 0
	int ret;

	if ( task_get_current() == 0 ){
		return 0;
	}


	if ( check_initialized(mutex) ){
		return -1;
	}

	ret = mutex_trylock(mutex, false, NULL);
	switch(ret){
	case 1:
		errno = EDEADLK;
		return -1;
	case -1:
		return -1;
	default:
		break;
	}
	return 0;
#else
	return 0;
#endif
}

/*! \details This function tries to lock \a mutex.  If \a mutex cannot be locked immediately,
 * the function returns without the lock.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL:  \a mutex is NULL
 * - EBUSY: \a mutex is locked by another thread
 *
 */
int pthread_mutex_trylock(pthread_mutex_t *mutex){
#if SINGLE_TASK == 0
	int ret;

	if ( task_get_current() == 0 ){
		return 0;
	}

	if ( check_initialized(mutex) ){
		return -1;
	}

	ret = mutex_trylock(mutex, true, NULL);
	switch(ret){
	case 0:
		//just acquired the lock
	case 1:
		//already owns the lock
		return 0;
	case -2:
		errno = EBUSY;
		return -1;
	default:
		//Doesn't own the lock
		return -1;
	}
#else
	return 0;
#endif
}

/*! \details This function unlocks \a mutex.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL:  \a mutex is NULL
 * - EACCES: the caller does not have a lock on \a mutex
 *
 */
int pthread_mutex_unlock(pthread_mutex_t *mutex){
#if SINGLE_TASK == 0
	priv_mutex_unlock_t args;

	if ( task_get_current() == 0 ){
		return 0;
	}

	if ( check_initialized(mutex) ){
		return -1;
	}

	args.id = task_get_current();
	if ( mutex->pthread == args.id ){ //Does this thread have a lock?
		if ( mutex->flags & PTHREAD_MUTEX_FLAGS_RECURSIVE ){
			mutex->lock--;
			if( mutex->lock != 0 ){
				return 0;
			}
		}
	} else {
		errno = EACCES;
		return -1;
	}

	args.mutex = mutex;  //The Mutex
	hwpl_core_privcall((core_privcall_t)priv_mutex_unlock, &args);
#endif
	return 0;
}

/*! \details This function destroys \a mutex.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL:  \a mutex is NULL
 *
 */
int pthread_mutex_destroy(pthread_mutex_t *mutex){
#if SINGLE_TASK == 0
	if ( check_initialized(mutex) ){
		return -1;
	}

	mutex->flags = 0;
	mutex->prio_ceiling = 0;
	mutex->pid = 0;
	mutex->pthread = -1;
	mutex->lock = 0;
#endif
	return 0;
}


#if SINGLE_TASK == 0

int mutex_trylock(pthread_mutex_t *mutex, bool trylock, const struct timespec * abs_timeout){
	int id;
	priv_mutex_trylock_t args;
	if ( mutex == NULL ){
		errno = EINVAL;
		return -1;
	}

	if ( !(mutex->flags & PTHREAD_MUTEX_FLAGS_INITIALIZED) ){
		//Mutex is not initialized
		errno = EINVAL;
		return -1;
	}

	if ( !(mutex->flags & PTHREAD_MUTEX_FLAGS_PSHARED) ){
		//check if pid is equal to the PID of the mutex
		if ( mutex->pid != getpid() ){
			//this mutex belongs to a different process and is not shared
			errno = EACCES;
			return -1;
		}
	}

	id = task_get_current();

	//Does this thread already have a lock?
	if ( mutex->pthread == id ){
		//If the mutex is recursive, simply update the count
		if ( mutex->flags & PTHREAD_MUTEX_FLAGS_RECURSIVE ){
			//Check the maximum number of locks allowed
			if ( mutex->lock < PTHREAD_MAX_LOCKS ){
				mutex->lock++;
				return 0;
			} else {
				errno = EAGAIN;
				return -1;
			}
		} else {
			//Already an owner of the mutex -- trylock returns 0, lock returns an error
			return 1;
		}
	}

	//Lock the mutex if it is free
	args.id = id;
	args.mutex = mutex;
	args.trylock = trylock;
	sched_convert_timespec(&args.abs_timeout, abs_timeout);
	hwpl_core_privcall((core_privcall_t)priv_mutex_trylock, &args);
	return args.ret;
}

int pthread_mutex_force_unlock(pthread_mutex_t *mutex){
	priv_mutex_unlock_t args;

	if ( mutex->flags & PTHREAD_MUTEX_FLAGS_PSHARED ){ //All pshared objects must be in shared memory space
		if ( mutex->pid == getpid() && (mutex->lock != 0) ){
			args.id = mutex->pthread; //Current owner of the mutex
			args.mutex = mutex;  //The Mutex
			hwpl_core_privcall((core_privcall_t)priv_mutex_unlock, &args);
		}
	}

	return 0;
}

void priv_mutex_trylock(priv_mutex_trylock_t *args){
	if ( args->mutex->pthread == -1 ){  //Is the mutex free
		//The mutex is free -- lock it up
		args->mutex->pthread = args->id; //This is to hold the mutex in case another task tries to grab it
		args->mutex->pid = task_get_pid(args->id);
		if ( args->mutex->prio_ceiling > sched_table[ args->id ].priority ){
			sched_table[ args->id ].priority = args->mutex->prio_ceiling; //Elevate the priority of the task based on prio_ceiling
		}
		args->mutex->lock = 1; //This is the lock count
		args->ret = 0;
	} else {
		//Mutex is not free
		if ( args->trylock == false ){
			//block the calling mutex
			sched_table[ args->id ].block_object = args->mutex; //Elevate the priority of the task based on prio_ceiling
			sched_priv_timedblock(args->mutex, &args->abs_timeout);
		}
		args->ret = -2;
	}
}

void priv_mutex_unlock(priv_mutex_unlock_t * args){
	int new_thread;
	int last_prio;
	//Restore the priority to the task that is unlocking the mutex
	sched_table[args->id].priority = sched_table[args->id].attr.schedparam.sched_priority;
	last_prio = sched_current_priority;
	sched_current_priority = sched_table[args->id].priority;
	sched_table[args->id].block_object = NULL;

	//check to see if another task is waiting for the mutex
	new_thread = sched_get_highest_priority_blocked(args->mutex);

	if ( new_thread != -1 ){
		args->mutex->pthread = new_thread;
		args->mutex->pid = task_get_pid(new_thread);
		args->mutex->lock = 1;
		sched_table[new_thread].priority = args->mutex->prio_ceiling;
		sched_priv_assert_active(new_thread, SCHED_UNBLOCK_MUTEX);
		sched_priv_update_on_wake(sched_table[new_thread].priority);
	} else {
		args->mutex->lock = 0;
		args->mutex->pthread = -1; //The mutex is up for grabs
		if ( last_prio > sched_current_priority ){
			sched_priv_update_on_wake(sched_table[args->id].priority); //The priority was downgraded
		}
	}
}

/*! \details This function causes the calling thread to lock \a mutex.  It \a mutex
 * cannot be locked, the thread is block until either the mutex is locked or \a abs_timeout
 * is greater than \a CLOCK_REALTIME.
 *
 * Example:
 * \code
 * struct timespec abstime;
 * clock_gettime(CLOCK_REALTIME, &abstime);
 * abstime.tv_sec += 5; //time out after five seconds
 * if ( pthread_mutex_timedlock(mutex, &abstime) == -1 ){
 * 	if ( errno == ETIMEDOUT ){
 * 		//Timedout
 * 	} else {
 * 		//Failed
 * 	}
 * }
 * \endcode
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL:  mutex or abs_timeout is NULL
 * - EDEADLK:  the caller already holds the mutex
 * - ETIMEDOUT:  \a abstime passed before \a cond arrived
 *
 */
int pthread_mutex_timedlock(pthread_mutex_t * mutex, const struct timespec * abs_timeout){
	int ret;
	if ( task_get_current() == 0 ){
		return 0;
	}

	if ( check_initialized(mutex) ){
		return -1;
	}

	ret = mutex_trylock(mutex, false, abs_timeout);
	switch(ret){
	case 1:
		errno = EDEADLK;
		return -1;
		break;
	case -2:
		//Either the lock was acquired or the timeout occurred
		if ( mutex->pthread == task_get_current() ){
			errno = 0;
			//Lock was acquired
			return 0;
		} else {
			//Timeout occurred
			errno = ETIMEDOUT;
			return -1;
		}

	case -1:
		return -1;
	default:
		return 0;
	}
}

/*! \details This function gets the mutex priority ceiling from \a mutex and stores
 * it in \a prioceiling.
 * \return Zero on success or -1 with \a errno (see \ref ERRNO) set to:
 * - EINVAL: mutex or prioceiling is NULL
 */
int pthread_mutex_getprioceiling(pthread_mutex_t * mutex, int *prioceiling){
	if ( check_initialized(mutex) ){
		return -1;
	}

	*prioceiling = mutex->prio_ceiling;
	return 0;
}

/*! \details This function sets \a mutex priority ceiling to prioceiling.  If \a old_ceiling
 * is not NULL, the old ceiling value is stored there.
 *
 * \return Zero on success or -1 with \a errno (see \ref ERRNO) set to:
 * - EINVAL: mutex is NULL
 */
int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling, int *old_ceiling){
	if ( check_initialized(mutex) ){
		return -1;
	}

	if ( old_ceiling != NULL ){
		*old_ceiling = mutex->prio_ceiling;
	}


	mutex->prio_ceiling = prioceiling;
	return 0;
}

int check_initialized(const pthread_mutex_t * mutex){
	if ( mutex == NULL ){
		errno = EINVAL;
		return -1;
	}

	if ( (mutex->flags & PTHREAD_MUTEX_FLAGS_INITIALIZED) == 0 ){
		errno = EINVAL;
		return -1;
	}

	return 0;
}
#endif

/*! @} */


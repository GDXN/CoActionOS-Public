/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


/*! \addtogroup SIGNAL
 *
 * @{
 *
 * \ingroup POSIX
 */

/*! \file */

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include "hwpl.h"
#include "hwpl/task.h"

#include "../sched/sched_flags.h"

#include "sig_local.h"

static int check_pending_set(const sigset_t * set);

/*! \details This function sends the signal \a signo to the process \a pid.
 * The signal value is specified by \a value.
 *
 *
 * \return Zero or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL: \a signo is not a valid signal number
 * - ESRCH: \a pid is not a valid process id
 *
 */
int sigqueue(pid_t pid, int signo, const union sigval value){
	int tid;

	//get the tid from the pid
	for(tid = 1; tid < task_get_total(); tid++){
		if ( pid == task_get_pid(tid) ){
			break;
		}
	}

	return signal_send(tid, signo, SI_QUEUE, value.sival_int);
}

/*! \details This function checks to see if any signals
 * in set are pending.  If a signal is pending, it is cleared
 * and the signal number is written to \a *sig, and the thread is not blocked.
 *
 * If no signals in set are pending, the thread is blocked until a signal
 * becomes pending.
 *
 * The signals defined by \a set should be blocked when this function is called
 * (see \ref pthread_sigmask() and \ref sigprocmask()).
 *
 * \return Zero or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL: \a set contains an invalid or unsupported signal
 *
 *
 */
int sigwait(const sigset_t * set, int * sig){
	int tmp;
	do {
		if ( (tmp = check_pending_set(set)) ){
			*sig = tmp;
			return 0;
		}

		//Block until a signal arrives
		hwpl_core_privcall(signal_priv_wait, NULL);

	} while(1);

	return 0;
}


/*! \details This function checks to see if any signals
 * in the set are pending.  If a signal is pending, it is cleared
 * and the signal information is written to info, and the function returns without blocking.
 *
 * If no signals in set are pending, the task is blocked until a signal
 * becomes pending or until the timeout expires.
 *
 * \return Zero or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL: \a set contains an invalid or unsupported signal
 * - EAGAIN: \a timeout expired before any signals arrived
 * - EINTR: a signal, not in \a set, was caught
 *
 *
 */
int sigtimedwait(const sigset_t * set,
		siginfo_t * info,
		const struct timespec * timeout){
	struct sched_timeval abs_timeout;
	int sig;

	sched_convert_timespec(&abs_timeout, timeout);

	do {
		if ( (sig = check_pending_set(set)) ){
			if ( GLOBAL_SIGINFOS != NULL ){
				*info = GLOBAL_SIGINFO(sig);
				return 0;
			}
		}
		//block the thread until a signal arrives
		hwpl_core_privcall(signal_priv_wait, &abs_timeout);

		//Check to see if a non-blocked signal was caught --executed by user function
		if ( sched_sigcaught_asserted(task_get_current()) ){ //! \todo the sigcault flag should not be in a protected area of memory
			errno = EINTR;
			return -1;
		}

		//Check to see if the thread woke up because the timeout expired
		if ( sched_get_unblock_type( task_get_current() ) == SCHED_UNBLOCK_SLEEP ){
			errno = EAGAIN;
			return -1;
		}

	} while(1);

	return 0;
}

/*! \details This function checks to see if any signals
 * in set are pending.  If a signal is pending, it is cleared
 * and the signal info is written to info, and the function returns without blocking.
 *
 * If no signals in set are pending, the task is blocked until a signal
 * becomes pending.
 *
 * \todo Add error checking
 *
 *
 * \return Zero or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL: \a set contains an invalid or unsupported signal
 * - EINTR: a signal, not in \a set, was caught
 *
 */
int sigwaitinfo(const sigset_t *set, siginfo_t *info){
	int sig;
	do {
		if ( (sig = check_pending_set(set)) ){
			if ( GLOBAL_SIGINFOS != NULL ){
				*info = GLOBAL_SIGINFO(sig);
				return 0;
			}
		}
		//block the thread until a signal arrives
		hwpl_core_privcall(signal_priv_wait, NULL);

		//Check to see if a non-blocked signal was caught --executed by user function
		if ( sched_sigcaught_asserted(task_get_current()) ){
			errno = EINTR;
			return -1;
		}

	} while(1);

	return 0;
}

int check_pending_set(const sigset_t * set){
	sigset_t mask;
	int i;
	mask = THREAD_SIGPENDING & *set;
	if ( mask ){
		for(i = 0; i < SCHED_NUM_SIGNALS; i++){
			if ( mask & (1<<i) ){
				THREAD_SIGPENDING &= ~(1<<i); //clear the pending signal
				//execute the handler


				return i;
			}
		}
	}
	return 0;
}

void signal_priv_wait(void * args){
	if ( args != NULL ){
		sched_priv_timedblock(NULL, (struct sched_timeval *)args);
	} else {
		sched_priv_update_on_sleep();
	}
}

/*! @} */

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SIGNAL
 * @{
 */

/*! \file */

#include <signal.h>
#include <errno.h>

#include "sig_local.h"
#include "../sched/sched_flags.h"

/*! \details This function sends the signal \a signo to the process \a pid.
 *
 * \return Zero or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL: \a signo is not a valid signal number
 * - ESRCH: \a pid is not a valid process id
 *
 */
int kill(pid_t pid, int signo);

int _kill(pid_t pid, int signo){
	int tid;

	for(tid = 1; tid < task_get_total(); tid++){
		if ( pid == task_get_pid(tid) ){
			break;
		}
	}

	//! \todo Add permission error checking
	return signal_send(tid, signo, SI_USER, 0);
}


/*! @} */

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef SYS_SELECT_H_
#define SYS_SELECT_H_

#include <sys/types.h>
#include <signal.h>
#include <time.h>

int pselect(int nfds,
		fd_set * readfds,
		fd_set * writefds,
		fd_set * errorfds,
		const struct timespec * timeout,
		const sigset_t * sigmask);

int select(int nfds,
		fd_set * readfds,
		fd_set * writefds,
		fd_set * errorfds,
		struct timeval * timeout);



#endif /* SYS_SELECT_H_ */

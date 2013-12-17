/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup TIME
 * @{
 */

/*! \file */

#include <sys/time.h>
#include <time.h>
#include <errno.h>

//struct itimerval itimers[SYSCALLS_NUM_ITIMERS];

int _getitimer(int which, struct itimerval *value){
	//Update the current value (time of day minus the start time)
	errno = ENOTSUP;
	return -1;
}

int _setitimer(int which, const struct itimerval * value,
	       struct itimerval * ovalue) {
	errno = ENOTSUP;
	return -1;
}

/*! @} */

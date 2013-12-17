/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup TIME
 * @{
 */

/*! \file */

#include <stdlib.h>
#include <sys/times.h>
#include <errno.h>

#include "hwpl/arch.h"
#include "hwpl/task.h"


/*! \details This function populates \a buffer with the:
 * - process user time
 * - system time on behalf of the process
 * - children process user time
 * - children system time on behalf of the process
 *
 * \return Zero or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL: buffer is NULL
 *
 */
clock_t times(struct tms *buffer);

clock_t _times(struct tms *buffer) {
	int i;
	clock_t value;
	int pid;

	value = 0;
	pid = task_get_pid( task_get_current() );
	for(i=0; i < task_get_total(); i++){
		if ( task_get_pid(i) == pid ){
			value += task_gettime(i);
		}
	}

	buffer->tms_utime = value; //user time
	buffer->tms_stime = 0; //system time on behalf of process

	value = 0;
	for(i=0; i < task_get_total(); i++){
		if ( task_get_pid( task_get_parent(i) ) == pid ){
			value += task_gettime(i);
		}
	}

	buffer->tms_cutime = value; //children user time
	buffer->tms_cstime = 0; //children system time on behalf of process
	return buffer->tms_cutime + buffer->tms_utime;
}

/*! @} */

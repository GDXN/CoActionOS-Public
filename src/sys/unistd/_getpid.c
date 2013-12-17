/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNISTD
 * @{
 */

/*! \file */

#include "config.h"
#include <sys/types.h>
#include "hwpl.h"
#include "hwpl/task.h"

/*! \details This function returns the process ID of the calling process.
 * \return The process ID of the caller.
 */
pid_t getpid(void);

pid_t _getpid(void){
#if SINGLE_TASK == 0
	return (pid_t)task_get_pid( task_get_current() );
#else
	return 0;
#endif
}

/*! @} */


/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNISTD
 * @{
 */

/*! \file */

#include <sys/types.h>
#include "hwpl.h"
#include "hwpl/task.h"

/*! \details This function returns the process ID of the parent process.
 * \return The process ID of the caller's parent process.
 */
pid_t getppid(void){
	return (pid_t)task_get_pid( task_get_parent( task_get_current() ) );
}

/*! @} */


/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PTHREAD
 * @{
 */

/*! \file */



#include <pthread.h>
#include "hwpl.h"
#include "hwpl/task.h"

/*! \details This function returns the thread ID of the calling process.
 * \return The thread ID of the caller.
 */
pthread_t pthread_self(void){
	return (pthread_t)task_get_current();
}

/*! @} */


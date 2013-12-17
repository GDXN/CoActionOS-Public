/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PTHREAD
 * @{
 *
 */

/*! \file */

#include "config.h"

#include <pthread.h>
#include <errno.h>


/*! \details This function is not supported.
 * \return -1 with errno equal to ENOTSUP
 */
int pthread_cancel(pthread_t thread){
	errno = ENOTSUP;
	return -1;
}

/*! @} */


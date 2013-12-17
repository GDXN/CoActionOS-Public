/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PTHREAD_MUTEX
 * @{
 *
 * \ingroup PTHREAD
 *
 */

/*! \file */

#include "config.h"

#include <pthread.h>
#include <errno.h>

int pthread_mutexattr_check_initialized(const pthread_mutexattr_t * attr);


/*! \details This function initializes \a attr with default values.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL:  \a attr is NULL
 *
 */
int pthread_mutexattr_init(pthread_mutexattr_t *attr){
#if PTHREAD_SINGLE_MODE == 1
	return 0;
#else
	attr->is_initialized = 1;
	attr->process_shared = 0;
	attr->prio_ceiling = 0;  //! \todo This should be a compile time define
	attr->protocol = 0;
	attr->recursive = 0;
	return 0;
#endif
}

/*! \details This function destroys \a attr.
 *
 * \return Zero on success or -1 with errno set to:
 * - EINVAL:  \a attr is not an initialized mutex attribute object
 *
 */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr){
#if PTHREAD_SINGLE_MODE == 1
	return 0;
#else
	if ( pthread_mutexattr_check_initialized(attr) ){
		return -1;
	}
	attr->is_initialized = 0;
	return 0;
#endif
}

int pthread_mutexattr_check_initialized(const pthread_mutexattr_t * attr){
	if ( attr == NULL ){
		return EINVAL;
	}

	if ( attr->is_initialized != 1 ){
		errno = EINVAL;
		return -1;
	}
	return 0;
}

/*! @} */


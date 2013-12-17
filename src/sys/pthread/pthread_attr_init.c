/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PTHREAD_ATTR
 * @{
 *
 * \ingroup PTHREAD
 *
 */

/*! \file */

#include "config.h"
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>


/*! \details This function initializes \a attr to the
 * default values.
 * \return 0 on success or -1 and errno set to:
 *  - ENOMEM:  Could not allocate memory for the thread attribute
 */
int pthread_attr_init(pthread_attr_t * attr /*! a pointer to the attributes structure */){
	attr->stacksize = PTHREAD_DEFAULT_STACK_SIZE;
	attr->stackaddr = malloc(attr->stacksize + sizeof(struct _reent) + SCHED_DEFAULT_STACKGUARD_SIZE);
	if ( attr->stackaddr == NULL ){
		errno = ENOMEM;
		return -1;
	}

	PTHREAD_ATTR_SET_IS_INITIALIZED((attr), 1);
	PTHREAD_ATTR_SET_CONTENTION_SCOPE((attr), PTHREAD_SCOPE_SYSTEM);
	PTHREAD_ATTR_SET_GUARDSIZE((attr), SCHED_DEFAULT_STACKGUARD_SIZE);
	PTHREAD_ATTR_SET_INHERIT_SCHED((attr), PTHREAD_EXPLICIT_SCHED);
	PTHREAD_ATTR_SET_DETACH_STATE((attr), PTHREAD_CREATE_JOINABLE);
	PTHREAD_ATTR_SET_SCHED_POLICY((attr), SCHED_OTHER);

	attr->schedparam.sched_priority = SCHED_DEFAULT_PRIORITY;
	return 0;
}

/*! \details This function destroys \a attr.
 * \return 0
 */
int pthread_attr_destroy(pthread_attr_t * attr /*! a pointer to the attributes structure */){
	if ( attr == NULL ){
		return EINVAL;
	}

	if ( PTHREAD_ATTR_GET_IS_INITIALIZED(attr) != 1 ){
		errno = EINVAL;
		return -1;
	}

	if ( attr->stackaddr ){
		free(attr->stackaddr);
		attr->stackaddr = NULL;
	}
	PTHREAD_ATTR_SET_IS_INITIALIZED((attr), 0);
	return 0;
}

/*! @} */


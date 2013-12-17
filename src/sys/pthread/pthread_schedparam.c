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
#include "sched.h"

#include "../sched/sched_flags.h"

typedef struct {
	int tid;
	int policy;
	const struct sched_param * param;
} priv_set_pthread_scheduling_param_t;
static void priv_set_scheduling_param(void * args);


/*! \details This function gets \a thread's scheduling policy and scheduling parameters and
 * stores them in \a policy and \a param respectively.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ESRCH:  thread is not a valid
 * - EINVAL:  policy or param is NULL
 *
 */
int pthread_getschedparam(pthread_t thread, int *policy,
    struct sched_param *param){

	if ( sched_check_tid(thread) ){
		errno = ESRCH;
		return -1;
	}

	if ( (policy == NULL) || (param == NULL) ){
		errno = EINVAL;
		return -1;
	}

	*policy = PTHREAD_ATTR_GET_SCHED_POLICY( (&(sched_table[thread].attr)) );
	memcpy(param, (const void *)&(sched_table[thread].attr.schedparam), sizeof(struct sched_param));
	return 0;
}

/*! \details This function sets \a thread's scheduling policy and scheduling parameters to
 * \a policy and \a param respectively.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ESRCH:  thread is not a valid
 * - EINVAL:  param is NULL or the priority is invalid
 *
 */
int pthread_setschedparam(pthread_t thread,
		int policy,
		struct sched_param *param){

	int min_prio;
	int max_prio;
	priv_set_pthread_scheduling_param_t args;

	if ( param == NULL ){
		errno = EINVAL;
		return -1;
	}

	if ( sched_check_tid(thread) < 0 ){
		errno = ESRCH;
		return -1;
	}

	max_prio = sched_get_priority_max(policy);
	min_prio = sched_get_priority_min(policy);

	if ( ((uint8_t)param->sched_priority >= min_prio) &&
			((uint8_t)param->sched_priority <= max_prio) ){
		args.tid = thread;
		args.policy = policy;
		args.param = param;
		hwpl_core_privcall(priv_set_scheduling_param, &args);
		return 0;
	}

	//The scheduling priority is invalid
	errno = EINVAL;
	return -1;
}


void priv_set_scheduling_param(void * args){
	priv_set_pthread_scheduling_param_t * p = (priv_set_pthread_scheduling_param_t*)args;
	int id;
	id = p->tid;

	PTHREAD_ATTR_SET_SCHED_POLICY( (&(sched_table[id].attr)), p->policy);
	memcpy((void*)&sched_table[id].attr.schedparam, p->param, sizeof(struct sched_param));

	if ( p->policy == SCHED_FIFO ){
		task_assert_isfifo(id);
	} else {
		task_deassert_isfifo(id);
	}

}

/*! @} */




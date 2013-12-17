/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYSCALLS
 * @{
 */

/*! \file */

#include "config.h"

#include <unistd.h>
#include <stdlib.h>
#include <reent.h>

#include "hwpl.h"
#include "hwpl/task.h"
#include "hwpl/core.h"


static void priv_update_guard(void * args);

void * _sbrk_r(struct _reent * reent_ptr, ptrdiff_t incr){
	char * stack;
	char * base;
	ptrdiff_t size;

	if ( reent_ptr == NULL ){
		return NULL;
	} else {
		if ( reent_ptr->procmem_base == NULL ){
			return NULL;
		}
		size = reent_ptr->procmem_base->size;
		base = (char*)&(reent_ptr->procmem_base->base);
	}

#if SINGLE_TASK == 0
	stack = (void*)task_get_sbrk_stack_ptr(reent_ptr);
#else
	stack = core_get_thread_stack_ptr();
#endif

	if ( (stack != NULL) &&  ((base + size + incr) > (stack - MALLOC_SBRK_JUMP_SIZE)) ){
		return NULL;
	}

	//adjust the location of the stack guard -- always 32 bytes for processes
#if USE_MEMORY_PROTECTION > 0
	hwpl_core_privcall(priv_update_guard, base + size + incr);
#endif

	reent_ptr->procmem_base->size += incr;
	return (caddr_t) (base + size);
}

#if USE_MEMORY_PROTECTION > 0
void priv_update_guard(void * args){
	int tid;
	tid = task_get_thread_zero( task_get_pid(task_get_current() ) );
	task_priv_set_stackguard(tid, args, SCHED_DEFAULT_STACKGUARD_SIZE);
}
#endif


void * _sbrk(ptrdiff_t incr) {
	return _sbrk_r(_REENT, incr);
}

/*! @} */

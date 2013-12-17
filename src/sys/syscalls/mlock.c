/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <pthread.h>
#include <malloc.h>
#include <sys/lock.h>

void __malloc_lock(struct _reent *ptr){
	pthread_mutex_lock(&(ptr->procmem_base->__malloc_lock_object));
}

void __malloc_unlock(struct _reent *ptr){
	pthread_mutex_unlock(&(ptr->procmem_base->__malloc_lock_object));
}

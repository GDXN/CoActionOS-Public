/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <reent.h>
#include <malloc.h>
#include <sys/types.h>
#include <string.h>

void * _calloc_r(struct _reent * reent_ptr, size_t s1, size_t s2){
	int size;
	void * alloc;
	size = s1*s2;
	alloc = _malloc_r(reent_ptr, size);
	if ( alloc != NULL ){
		memset(alloc, 0, size);
	}
	return alloc;
}

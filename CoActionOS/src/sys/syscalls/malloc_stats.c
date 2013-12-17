/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "config.h"
#include <stdio.h>
#include <malloc.h>

void malloc_stats(void){
	_malloc_stats_r(_REENT);
}

void _malloc_stats_r(struct _reent * reent_ptr){
	struct mallinfo mi;
	mi = _mallinfo_r(reent_ptr);
	iprintf("Total Malloc Memory %d bytes\n", mi.arena);
	iprintf("Total Free Chunks %d\n", mi.ordblks);
	iprintf("Total Free Memory %d bytes\n", mi.fordblks);
	iprintf("Total Used Memory %d bytes\n", mi.uordblks);
}


/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "crt_common.h"

#include <signal.h>
#include <reent.h>
#include <sys/lock.h>
#include <sys/reent.h>
#include <pthread.h>

#include "hwpl/core.h"
#include "hwpl/task.h"

extern void pre_exit(void);

#define SCHED_DEFAULT_STACKGUARD_SIZE 32

extern void (*_ctors)(void);
extern int _ctors_size;

extern void (*_dtors)(void);
extern int _dtors_size;

extern int main(int argc, char * const argv[]);

char ** const import_argv(int argc, char * const argv[]);


static void constructors(void);
static void destructors(void);

int crt_common(int argc, char * const argv[], int * ret, const char * name){

	//hwpl_core_privcall(priv_init_stackguard, NULL);
	task_setstackguard(pthread_self(), &_ebss, SCHED_DEFAULT_STACKGUARD_SIZE);

	//Zero out the BSS section
	memset(&_bss,
			0,
			(uint32_t)((char*)&_ebss - (char*)&_bss)
	);

	_REENT->procmem_base = (proc_mem_t*)&_ebss;
	_REENT->procmem_base->proc_name = name;
	_REENT->procmem_base->size = 0;
	_REENT->procmem_base->sigactions = NULL;
	memset(_REENT->procmem_base->open_file, 0, sizeof(open_file_t)*OPEN_MAX);

	//Initialize the global mutexes
	__lock_init_recursive_global(__malloc_lock_object);
	__lock_init_global(__tz_lock_object);
	__lock_init_recursive_global(__atexit_lock);
	__lock_init_recursive_global(__sfp_lock);
	__lock_init_recursive_global(__sinit_lock);
	__lock_init_recursive_global(__env_lock_object);


	//import argv in to the process memory
	argv = crt_import_argv(argc, argv);

	_REENT->procmem_base->siginfos = malloc(sizeof(siginfo_t)*32);

	//Initialize STDIO
    __sinit (_GLOBAL_REENT);
    write(stdout->_file, 0, 0); //forces stdin, stdout, and stderr to open

	//Execute main
	constructors();
	*ret = main(argc, argv);
	destructors();

	return 0;
}

void constructors(void){
	int i;
	for(i=0; i < (int)&_ctors_size; i++){
		(&_ctors)[i]();
	}
}
void destructors(void){
	int i;
	for(i=0; i < (int)&_dtors_size; i++){
		(&_dtors)[i]();
	}
}




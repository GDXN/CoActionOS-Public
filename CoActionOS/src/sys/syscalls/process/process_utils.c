/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYSCALLS_PROCESS
 * @{
 */

/*! \file */

#include "config.h"

#include "sysfs.h"
#include "device.h"
#include "hwpl/task.h"

#include <fcntl.h>
#include <errno.h>

#include "syscalls_process.h"
#include "../../sched/sched_flags.h"
#include "../../sysfs/appfs.h"

static int reent_is_free(struct _reent * reent);

int process_start(const char *path,
		char *const argv[],
		char *const envp[]
){
#if SINGLE_PROCESS == 0
	int fd;
	int err;
	int argc;
	appfs_file_t startup;
	task_memories_t mem;

	if ( access(path, X_OK) < 0 ){
		return -1;
	}

	//Open the program
	fd = open(path, O_RDONLY);
	if ( fd < 0 ){
		//The open() call set the errno already
		return -1;
	}

	//Count the arguments
	argc = 0;
	if ( argv != NULL ){
		while( argv[argc] != NULL ){
			argc++;
		}
	}

	//Read the program header
	err = read(fd, &startup, sizeof(appfs_file_t));
	if ( err != sizeof(appfs_file_t) ){
		//The read() function sets the errno already
		close(fd);
		return -1;
	}

	//verify the signature
	if( appfs_util_isexecutable(&startup) == false ){
		errno = ENOEXEC;
		return -1;
	}

	mem.code.addr = startup.exec.code_start;
	mem.code.size = startup.exec.code_size;
	mem.data.addr = startup.exec.ram_start;
	mem.data.size = startup.exec.ram_size;

	//check to see if the process is already running
	if( !reent_is_free(startup.exec.ram_start) ){
		errno = ENOTSUP;
		close(fd);
		return -1;
	}

	//The program is loaded and ready to execute
	close(fd);

	/*
	hwpl_debug("new process: 0x%X 0x%X %d 0x%X 0x%X\n",
			(unsigned int)startup.exec.startup,
			(unsigned int)startup.exec.ram_start,
			(int)mem.data.size,
			(unsigned int)startup.exec.code_start,
			(unsigned int)startup.exec.code_size);
			*/


	err = sched_new_process(startup.exec.startup,
			argc,
			argv,
			&mem,
			startup.exec.ram_start);

	//hwpl_debug("Err is %d\n", err);

	return err;
#else
	errno = ENOTSUP;
	return -1;
#endif
}



#if SINGLE_PROCESS == 0
int reent_is_free(struct _reent * reent){
	int i;
	for(i=0; i < task_get_total(); i++){
		if ( (reent == task_table[i].reent) && task_enabled(i) ){
			return 0;
		}
	}
	return 1;
}
#endif


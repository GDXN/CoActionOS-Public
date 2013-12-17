/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYSCALLS_PROCESS
 * @{
 */

/*! \file */

#include "config.h"

#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

#include "hwpl.h"
#include "syscalls_process.h"
#include "../../sched/sched_flags.h"

static char ** new_arguments(char * arg_buffer);
static void del_arguments(char ** argv);


/*! \details This function creates a new process.
 * The calling process blocks until the new process completes.
 *
 * \return The return value of the child process.
 */
int _system(const char * s){
	int err;
	int status;
	int len;
	char ** argv;
	char * arg_buffer;

	//Check for a NULL argument
	if ( s == NULL ){
		return 1; //command processor is available
	}

	//Check for an empty argument
	len = strlen(s);
	if ( len == 0 ){
		return 0;
	}

	//Create a buffer for the arguments
	arg_buffer = (char*)malloc(len + 1);
	if ( arg_buffer == NULL ){
		errno = ENOMEM;
		return -1;
	}
	memset(arg_buffer, 0, len + 1);
	memcpy(arg_buffer, s, len);

	//Create the argument array
	argv = new_arguments(arg_buffer);
	if ( argv == NULL ){
		free(arg_buffer);
		errno = ENOMEM;
		return -1;
	}

	//Start the new process
	err = process_start(argv[0], argv, NULL);
	if ( err < 0 ){
		//Process failed to start
		status = -1;
	} else {
		//Wait for the process to complete before returning
		if ( waitpid(err, &status, 0) < 0 ){
			status = -1;
		}
	}

	//free the argument memory
	free(arg_buffer);
	del_arguments(argv);
	return status;
}


static char ** new_arguments(char * arg_buffer){
	int argc;
	char ** argv;
	char * tmp;

	//Count the arguments
	tmp = (char*)arg_buffer;
	argc = 1;
	while( *tmp != 0 ){
		if ( *tmp == ' ' ){
			argc++;
		}
		tmp++;
	}

	//Allocate the argument array
	argv = (char**)malloc(sizeof(char**)*(argc + 1));
	if ( argv == NULL ){
		return NULL;
	}

	//Populate the argument vector
	tmp = (char*)arg_buffer;
	argv[0] = arg_buffer;
	argc = 1;
	while( (tmp = (char*)memchr(tmp, ' ', strlen( tmp ) )) ){
		tmp[0] = 0;
		tmp++;
		argv[argc] = tmp;
		argc++;
	}

	argv[argc] = NULL;

	return argv;
}

static void del_arguments(char ** argv){
	free(argv);
}


/*! @} */





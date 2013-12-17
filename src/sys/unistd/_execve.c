/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_PROCESS
 * @{
 */

#include <fcntl.h>
#include <errno.h>


int _execve(const char *path, char *const argv[], char *const envp[]){
	//This needs to manipulate the task table so that the current process is replaced by a new image
	errno = ENOTSUP;
	return -1;

}


/*! @} */





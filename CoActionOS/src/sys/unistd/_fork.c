/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_PROCESS
 * @{
 */

/*! \file */

#include <errno.h>
#include <sys/types.h>

/*! \details This function is not supported.
 *
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - ENOTSUP:  function is not supported
 *
 */
pid_t fork(void);

pid_t _fork(void){
	//This function can not be supported unless the CPU has an MMU
	errno = ENOTSUP;
	return -1;
}

/*! \details This function is not supported.
 *
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - ENOTSUP:  function is not supported
 *
 */
pid_t vfork(void);

pid_t _vfork(void){

	//create a new process copying the characteristic of the current process (has new process ID -- all else identical)

	//manipulate the stack of the current task so that the child process ID is immediately returned


	//Force a switch to the child process -- block the parent process execution


	//return 0 (as the child)


	//when exec() or _exit() is run -- the parent process needs to be woken up


	//Use the following label as the PC for the parent
	//parent_process_resume:

	errno = ENOTSUP;
	return -1;
}

/*! @} */


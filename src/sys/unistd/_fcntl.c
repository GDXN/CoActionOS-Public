/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FILDES
 * @{
 */

/*! \file */

#include <fcntl.h>
#include <stdarg.h>
#include "unistd_fs.h"
#include "unistd_flags.h"

#include "hwpl/debug.h"

/*! \details This function performs various operations on open files such as:
 * - F_DUPFD: duplicate a file descriptor
 * - F_GETFD:  get the file descriptor flags
 * - F_SETFD:  set the file descriptor flags
 * - F_GETOWN: get the file descriptor owner process ID.
 *
 * \param fildes The file descriptor
 * \param cmd The operation to perform
 *
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - EBADF:  invalid file descriptor
 *  - ENOTSUP:  \a cmd is not supported for the file descriptor
 *
 */
int fcntl(int fildes, int cmd, ...);

int _fcntl(int fildes, int cmd, ...){
	int tmp;
	int flags;
	va_list ap;

	fildes = unistd_fildes_is_bad(fildes);
	if ( fildes < 0 ){
		return -1;
	}

	flags = get_flags(fildes);

	va_start(ap, cmd);
	tmp = va_arg(ap, int);
	va_end(ap);

	switch(cmd){
	/*
	case F_DUPFD:

		new_fildes = unistd_new_open_file(tmp);
		if ( new_fildes == -1 ){
			return -1;
		}

		dup_open_file(new_fildes, fildes);
		return new_fildes;
		*/



	case F_GETFL:
		return flags;

	case F_SETFL:
		set_flags(fildes, tmp);
		break;

	case F_GETFD:
	case F_SETFD:
	case F_DUPFD:
	case F_GETOWN:
	case F_SETOWN:
	case F_GETLK:
	case F_SETLK:
	case F_SETLKW:
		errno = ENOTSUP;
		break;
	default:
		errno = EINVAL;
		break;
	}

	return -1;
}

/*! @} */

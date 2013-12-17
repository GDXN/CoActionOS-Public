/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


/*! \addtogroup UNI_FILDES
 * @{
 */

/*! \file */

#include  "unistd_fs.h"
#include  "unistd_flags.h"


/*! \details This function gets various file statistics for
 * the specified file descriptor.
 *
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - EBADF:  \a fildes is invalid
 *  - EINVAL: \a buf is NULL
 *
 */
int fstat(int fildes, struct stat *buf);

int _fstat(int fildes, struct stat *buf){
	void * handle;
	const sysfs_t * fs;

	fildes = unistd_fildes_is_bad(fildes);
	if ( fildes < 0 ){
		return -1;
	}

	handle = get_handle(fildes);
	memset (buf, 0, sizeof (* buf));
	fs = get_fs(fildes);
	if ( fs != NULL ){
		return fs->fstat(fs->cfg, handle, buf);
	} else {
		errno = EBADF;
		return -1;
	}
}

/*! @} */


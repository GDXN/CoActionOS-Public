/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FILDES
 * @{
 */

/*! \file */

#include "config.h"
#include  "unistd_fs.h"
#include  "unistd_flags.h"

static int get_size(int fildes);

/*! \details This function sets the file offset for \a fildes using the following
 * values of \a whence:
 * - SEEK_SET: set the offset to \a offset
 * - SEEK_CUR: set the offset to current location plus \a offset
 * - SEEK_END:  set the offset to the size of the file plus \a offset
 *
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - EBADF:  \a fildes is invalid
 *  - EINVAL:  \a whence is invalid
 *
 */
off_t lseek(int fildes, off_t offset, int whence);


off_t _lseek(int fildes, off_t offset, int whence){
	int loc;
	int32_t tmp;

	fildes = unistd_fildes_is_bad(fildes);
	if ( fildes < 0 ){
		return -1;
	}

	loc = get_loc(fildes);

	switch(whence){
	case SEEK_SET:
		tmp = (int32_t)offset;
		break;
	case SEEK_CUR:
		tmp = loc + (int32_t)offset;
		break;
	case SEEK_END:
		tmp = get_size(fildes) + (int32_t)offset;
		break;
	default:
		errno = EINVAL;
		return -1;
	}
	set_loc(fildes, tmp);
	return tmp;
}

int get_size(int fildes){
	int tmp;
	const sysfs_t * fs;
	struct stat st;
	fs = get_fs(fildes);
	st.st_size = 0;
	tmp = errno;
	fs->fstat(fs->cfg, get_handle(fildes), &st);
	errno = tmp;
	return st.st_size;
}

/*! @} */


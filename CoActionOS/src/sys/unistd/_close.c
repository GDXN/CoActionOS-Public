/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FILE_ACCESS
 * @{
 */

/*! \file
 */

#include "hwpl.h"
#include "sysfs.h"

#include "unistd_fs.h"
#include "unistd_flags.h"
#include "hwpl/debug.h"

/*! \details This function closes the file associated
 * with the specified descriptor.
 * \param fildes The File descriptor \a fildes.
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - EBADF:  Invalid file descriptor
 */
int close(int fildes);

int _close(int fildes) {
	//Close the file if it's open
	int ret;
	void ** handle_ptr;
	const sysfs_t * fs;

	fildes = unistd_fildes_is_bad(fildes);
	if ( fildes < 0 ){
		return -1;
	}

	handle_ptr = get_handle_ptr(fildes);
	fs = get_fs(fildes);
	ret = fs->close(fs->cfg, handle_ptr);
	unistd_reset_fildes(fildes);
	return ret;
}


/*! @} */


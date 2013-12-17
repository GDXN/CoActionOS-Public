/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FS
 * @{
 */

/*! \file */

#include "unistd_fs.h"
#include "unistd_flags.h"

/*! \details This function creates a new directory.
 *
 * \param path Path to the new directory
 * \param mode Ignored
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ENOENT:  \a path is an empty string or the parent directory cannot be found
 * - EEXIST:  \a path already exists
 * - ENOSPC:  Not enough space on the disk to add a new directory
 *
 */
int mkdir(const char *path, mode_t mode){
	const sysfs_t * fs;

	if ( sysfs_ispathinvalid(path) == true ){
		return -1;
	}

	fs = sysfs_find(path, true);
	if ( fs != NULL ){
		return fs->mkdir(fs->cfg,
				sysfs_stripmountpath(fs, path),
				mode);
	}
	errno = ENOENT;
	return -1;
}

/*! @} */

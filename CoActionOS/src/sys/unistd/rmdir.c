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

/*! \details This function removes the directory specified by \a path.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - ENOENT:  \a path is an empty string or the parent directory cannot be found
 * - EEXIST:  \a path already exists
 * - ENOTDIR:  \a path is not a directory
 * - ENOTEMPTY:  \a path is not an empty directory
 *
 */
int rmdir(const char *path){
	const sysfs_t * fs;

	if ( sysfs_ispathinvalid(path) == true ){
		return -1;
	}

	fs = sysfs_find(path, true);
	if ( fs != NULL ){
		return fs->rmdir(fs->cfg,
				sysfs_stripmountpath(fs, path)
				);
	}
	errno = ENOENT;
	return -1;
}

/*! @} */

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FILDES
 * @{
 */

/*! \file */

#include  "unistd_fs.h"

/*! \details This function is equivalent to \ref stat() except
 * path refers to a symbolic link.
 *
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 * - ENAMETOOLONG: \a path exceeds PATH_MAX or a component of \a path exceeds NAME_MAX
 * - ENOENT: \a path does not exist
 * - EACCES: search permission is denied for a component of \a path
 *
 */
int lstat(const char * path /*! The path the to symbolic link */,
		struct stat *buf /*! The destination buffer */);

int lstat(const char * path, struct stat *buf){
	const sysfs_t * fs;

	if ( sysfs_ispathinvalid(path) == true ){
		return -1;
	}

	fs = sysfs_find(path, true);
	if ( fs != NULL ){
		return fs->lstat(fs->cfg, sysfs_stripmountpath(fs, path), buf);
	}
	errno = ENOENT;
	return -1;
}

/*! @} */



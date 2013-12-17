/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_PERMS
 * @{
 */

/*! \file */

#include <string.h>
#include  "unistd_fs.h"

/*! \details This function changes the mode of the specified file
 * or directory.
 *
 * \return Zero on success or -1 with errno (see \ref ERRNO) set to:
 * - EIO:  IO Error
 * - ENAMETOOLONG: \a path exceeds PATH_MAX or a component of \a path exceeds NAME_MAX
 * - ENOENT: \a path does not exist
 * - EACCES: search permission is denied for a component of \a path
 *
 *
 */
int chown(const char *path, uid_t uid, gid_t gid){
	const sysfs_t * fs;

	if ( sysfs_ispathinvalid(path) == true ){
		return -1;
	}

	fs = sysfs_find(path, true);
	if ( fs != NULL ){
		return fs->chown(fs->cfg, sysfs_stripmountpath(fs, path), uid, gid);
	}
	errno = ENOENT;
	return -1;
}

/*! @} */


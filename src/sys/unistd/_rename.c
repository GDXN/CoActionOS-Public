/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FS
 * @{
 */

/*! \file */

#include "unistd_fs.h"

/*! \details This functions renames \a old to \a new.
 *
 * \return Zero on success or -1 with errno set to:
 * - EEXIST:  \a new already exists
 * - EIO:  IO error
 * - ENOENT:  \a old does not exist
 * - EACCESS:  \a old or \a new cannot be written
 */
int rename(const char *old, const char *new);

int _rename(const char *old, const char *new){
	const sysfs_t * fs_old;
	const sysfs_t * fs_new;

	if ( sysfs_ispathinvalid(old) == true ){
		return -1;
	}

	if ( sysfs_ispathinvalid(new) == true ){
		return -1;
	}

	fs_old = sysfs_find(old, true);
	fs_new = sysfs_find(new, true);

	if ( (fs_old == fs_new) && (fs_new != NULL) ){
		return fs_new->rename(fs_new->cfg, sysfs_stripmountpath(fs_old, old), sysfs_stripmountpath(fs_new, new) );
	}

	errno = ENOTSUP;
	return -1;
}

/*! @} */

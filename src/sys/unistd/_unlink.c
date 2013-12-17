/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FS
 * @{
 */

/*! \file */

#include  "unistd_fs.h"

int _unlink(const char * name){
	const sysfs_t * fs;

	if ( sysfs_ispathinvalid(name) == true ){
		return -1;
	}

	fs = sysfs_find(name, true);
	if ( fs != NULL ){
		return fs->unlink(fs->cfg,
				sysfs_stripmountpath(fs, name)
				);
	}
	errno = ENOENT;
	return -1;
}

/*! @} */


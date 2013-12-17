/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_PERMS
 * @{
 */

/*! \file
 */

#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/syslimits.h>
#include <string.h>
#include <stdio.h>

#include "sysfs.h"

static int check_permissions(int file_mode, int file_uid, int file_gid, int amode);

/*! \details This function checks to see if the specified access (\a amode)
 * is allowed for \a path.
 *
 * \return Zero on success (ie \a amode is allowed) or -1 with errno (see \ref ERRNO) set to:
 * - ENAMETOOLONG: \a path exceeds PATH_MAX or a component of \a path exceeds NAME_MAX
 * - ENOENT: \a path does not exist
 * - EACCES: \a amode is not allowed for \a path or search permission is denied for a component of \a path
 *
 */
int access(const char * path, int amode){
	const sysfs_t * fs;
	struct stat st;

	//Do a safe check of the path string length
	if ( sysfs_ispathinvalid(path) == true ){
		return -1;
	}

	fs = sysfs_find(path, true);
	if ( fs == NULL ){
		errno = ENOENT;
		return-1;
	}

	if ( fs->stat(fs->cfg, sysfs_stripmountpath(fs, path), &st) < 0 ){
		return -1;
	}

	return check_permissions(st.st_mode, st.st_uid, st.st_gid, amode);
}

static uint8_t euid = 0;
static uint8_t egid = 0;

void cl_sys_seteuid(int uid){
	euid = uid;
}

void cafs_sys_setegid(int gid){
	egid = gid;
}

int cl_sys_geteuid(void){
	return euid;
}

int cl_sys_getegid(void){
	return egid;
}

int check_permissions(int file_mode, int file_uid, int file_gid, int amode){
	int is_ok;

	is_ok = 0;
	if ( amode & R_OK ){
		if ( file_mode & S_IROTH ){
			is_ok |= R_OK;
		} else if ( (file_mode & S_IRUSR)  && ( file_uid == cl_sys_geteuid()) ){
			//Check to see if s.st_uid matches current user id
			is_ok |= R_OK;
		} else if ( (file_mode & S_IRGRP) && ( file_gid == cl_sys_getegid()) ){
			//Check to see if gid matches current group id
			is_ok |= R_OK;
		}
	}

	if ( amode & W_OK ){
		if ( file_mode & S_IWOTH ){
			is_ok |= W_OK;
		} else if ( (file_mode & S_IWUSR) && ( file_uid == cl_sys_geteuid()) ){
			//Check to see if user id matches file_uid
			is_ok |= W_OK;
		} else if ( (file_mode & S_IWGRP) && ( file_gid == cl_sys_getegid()) ){
			//Check to see if gid matches current group id
			is_ok |= W_OK;
		}
	}

	if ( amode & X_OK){
		if ( file_mode & S_IXOTH ){
			is_ok |= X_OK;
		} else if ( (file_mode & S_IXUSR) && ( file_uid == cl_sys_geteuid())  ){
			//Check to see if s.st_uid matches current user id
			is_ok |= X_OK;
		} else if ( (file_mode & S_IXGRP) && ( file_gid == cl_sys_getegid()) ){
			//Check to see if gid matches current group id
			is_ok |= X_OK;
		}
	}

	if ( is_ok == amode ){
		return 0;
	}
	errno = EACCES;
	return -1;
}





/*! @} */


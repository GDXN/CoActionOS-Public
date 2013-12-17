/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYSFS
 * @{
 */

/*! \file */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "sysfs.h"

const char sysfs_validset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_./";

int mkfs(const char * path){
	const sysfs_t * fs;

	fs = sysfs_find(path, false);
	if( fs == NULL ){
		errno = ENOENT;
		return -1;
	}

	return fs->mkfs(fs->cfg);
}


//get the filename from the end of the path
const char * sysfs_getfilename(const char * path, int * elements){
	const char * name;
	int tmp;
	name = path;
	tmp = 1;
	while( *path != 0 ){
		if ( *path == '/' ){
			tmp++;
			name = path+1;
		}
		path++;
	}

	if( elements != NULL ){
		*elements = tmp;
	}
	return name;
}

const char * sysfs_get_filename(const char * path){
	const char * name;
	name = path;
	while( *path != 0 ){
		if ( *path == '/' ){
			name = path+1;
		}
		path++;
	}
	return name;
}


/*! \details This finds the filesystem associated with a path.
 *
 */
const sysfs_t * sysfs_find(const char * path, bool needs_parent){
	int i;
	int pathlen;
	int mountlen;
	pathlen = strlen(path);

	i = 0;
	while( sysfs_isterminator(&(sysfs_list[i])) == false ){
		mountlen = strlen(sysfs_list[i].mount_path);
		if( strncmp(path, sysfs_list[i].mount_path, mountlen) == 0 ){
			if ( needs_parent == true ){
				if ( (pathlen > (mountlen+1)) || (pathlen == 1) ){
					return &sysfs_list[i];
				}
			} else {
				return &sysfs_list[i];
			}
		}
		i++;
	}
	return NULL;
}

const char * sysfs_stripmountpath(const sysfs_t * fs, const char * path){
	path = path + strlen(fs->mount_path);
	if (path[0] == '/' ){
		path++;
	}
	return path;
}

static bool isinvalid(const char * path, int max){
	int len;
	int tmp;
	const char * p;
	len = strnlen(path, max);
	if ( len == max ){
		errno = ENAMETOOLONG;
		return true;
	}

	p = path;
	tmp = 0;
	do {
		p++;
		if ( (*p == '/') || (*p == 0) ){
			if ( tmp > NAME_MAX ){
				errno = ENAMETOOLONG;
				return true;
			}
			tmp = 0;
		}
		tmp++;
	} while( *p != 0 );


	if ( len != strspn(path, sysfs_validset) ){
		errno = EINVAL;
		return true;
	}

	return false;
}

bool sysfs_ispathinvalid(const char * path){
	return isinvalid(path, PATH_MAX);
}

bool sysfs_isvalidset(const char * path){
	if ( strlen(path) == strspn(path, sysfs_validset)){
		return true;
	}
	return false;
}

int sysfs_getamode(int flags){
	int accmode;
	int amode;
	amode = 0;
	accmode = flags & O_ACCMODE;
	switch(accmode){
	case O_RDWR:
		amode = R_OK | W_OK;
		break;
	case O_WRONLY:
		amode = W_OK;
		break;
	case O_RDONLY:
		amode = R_OK;
		break;
	}
	if ( (flags & O_CREAT) || (flags & O_TRUNC) ){
		amode |= W_OK;
	}
	return amode;
}

int sysfs_access(int file_mode, int file_uid, int file_gid, int amode){
	int is_ok;
	uid_t euid;
	gid_t egid;

	euid = geteuid();
	egid = getegid();

	is_ok = 0;
	if ( amode & R_OK ){
		if ( file_mode & S_IROTH ){
			is_ok |= R_OK;
		} else if ( (file_mode & S_IRUSR)  && ( file_uid == euid) ){
			//Check to see if s.st_uid matches current user id
			is_ok |= R_OK;
		} else if ( (file_mode & S_IRGRP) && ( file_gid == egid) ){
			//Check to see if gid matches current group id
			is_ok |= R_OK;
		}
	}

	if ( amode & W_OK ){
		if ( file_mode & S_IWOTH ){
			is_ok |= W_OK;
		} else if ( (file_mode & S_IWUSR) && ( file_uid == euid) ){
			//Check to see if user id matches file_uid
			is_ok |= W_OK;
		} else if ( (file_mode & S_IWGRP) && ( file_gid == egid) ){
			//Check to see if gid matches current group id
			is_ok |= W_OK;
		}
	}

	if ( amode & X_OK){
		if ( file_mode & S_IXOTH ){
			is_ok |= X_OK;
		} else if ( (file_mode & S_IXUSR) && ( file_uid == euid)  ){
			//Check to see if s.st_uid matches current user id
			is_ok |= X_OK;
		} else if ( (file_mode & S_IXGRP) && ( file_gid == egid) ){
			//Check to see if gid matches current group id
			is_ok |= X_OK;
		}
	}

	if ( is_ok == amode ){
		return 0;
	}
	return -1;
}

void sysfs_unlock(void){
	int i;
	i = 0;
	while( sysfs_isterminator(&(sysfs_list[i])) == false ){
		sysfs_list[i].unlock( sysfs_list[i].cfg );
		i++;
	}
}



int sysfs_notsup(void){
	errno = ENOTSUP;
	return -1;
}

void * sysfs_notsup_null(void){
	errno = ENOTSUP;
	return NULL;
}

void sysfs_notsup_void(void){}

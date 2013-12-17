/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#ifndef CAFS_LITE_H_
#define CAFS_LITE_H_


#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "sysfs.h"


typedef struct {
	int list_block;
	int serialno_killed;
	int serialno;
} cafs_lite_state_t;

typedef struct {
	const sysfs_t * devfs;
	const void * dev_cfg;
	open_file_t * open_file;
	const char name[NAME_MAX];
	cafs_lite_state_t * state;
} cafs_lite_cfg_t;




int cafs_lite_init(const void * cfg); //initialize the filesystem
int cafs_lite_mkfs(const void * cfg);

int cafs_lite_opendir(const void * cfg, void ** handle, const char * path);
int cafs_lite_readdir_r(const void * cfg, void * handle, int loc, struct dirent * entry);
int cafs_lite_closedir(const void * cfg, void * handle);

int cafs_lite_fstat(const void * cfg, void * handle, struct stat * stat);
int cafs_lite_open(const void * cfg, void ** handle, const char * path, int flags, int mode);
int cafs_lite_read(const void * cfg, void * handle, int flags, int loc, void * buf, int nbyte);
int cafs_lite_write(const void * cfg, void * handle, int flags, int loc, const void * buf, int nbyte);
int cafs_lite_close(const void * cfg, void ** handle);
int cafs_lite_remove(const void * cfg, const char * path);
int cafs_lite_unlink(const void * cfg, const char * path);

void cafs_lite_unlock(const void * cfg);

int cafs_lite_stat(const void * cfg, const char * path, struct stat * stat);

#define CAFS_LITE(mount_loc_name, cfgp, access_mode) { \
		.mount_path = mount_loc_name, \
		.access = access_mode, \
		.init = cafs_lite_init, \
		.startup = SYSFS_NOTSUP, \
		.mkfs = cafs_lite_mkfs, \
		.open = cafs_lite_open, \
		.priv_read = NULL, \
		.priv_write = NULL, \
		.read = cafs_lite_read, \
		.write = cafs_lite_write, \
		.close = cafs_lite_close, \
		.priv_ioctl = SYSFS_NOTSUP, \
		.rename = SYSFS_NOTSUP, \
		.unlink = cafs_lite_unlink, \
		.mkdir = SYSFS_NOTSUP, \
		.rmdir = SYSFS_NOTSUP, \
		.remove = cafs_lite_remove, \
		.opendir = cafs_lite_opendir, \
		.closedir = cafs_lite_closedir, \
		.readdir_r = cafs_lite_readdir_r, \
		.link = SYSFS_NOTSUP, \
		.symlink = SYSFS_NOTSUP, \
		.stat = cafs_lite_stat, \
		.lstat = SYSFS_NOTSUP, \
		.fstat = cafs_lite_fstat, \
		.chmod = SYSFS_NOTSUP, \
		.chown = SYSFS_NOTSUP, \
		.unlock = cafs_lite_unlock, \
		.cfg = cfgp, \
}


#endif /* CAFS_LITE_H_ */

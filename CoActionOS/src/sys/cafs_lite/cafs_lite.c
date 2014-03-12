/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <sys/stat.h>

#ifndef __SIM__
#include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "cafs_lite_local.h"
#include "cafs_lite_serialno.h"
#include "cafs_lite_dir.h"
#include "cafs_lite_file.h"
#include "cafs_lite_block.h"
#include "cafs_lite_scratch.h"
#include "cafs_lite.h"
#include "sysfs.h"

extern int pthread_mutex_force_unlock(pthread_mutex_t *mutex);

#define OPENDIR_HANDLE ((void*)0x1234567)


#define DEBUG_LEVEL 10

#ifndef __SIM__
static pthread_mutex_t __cl_lock_object;
#endif


void cafs_lite_unlock(const void * cfg){ //force unlock when a process exits
#ifndef __SIM__
	pthread_mutex_force_unlock(&__cl_lock_object);
#endif
}

static void lock_cafs_lite(void){
#ifndef __SIM__
	if ( pthread_mutex_lock(&__cl_lock_object) < 0 ){
		cl_error("Failed to lock caosfs %d\n", errno);
	}
	cl_dev_setdelay_mutex(&__cl_lock_object);
#endif
	//hwpl_debug("0x%X locked\n", (int)&__cl_lock_object);
}

static void unlock_cafs_lite(void){
	cl_dev_setdelay_mutex(NULL);
#ifndef __SIM__
	if ( pthread_mutex_unlock(&__cl_lock_object) < 0 ){
		cl_error("Failed to unlock caosfs %d\n", errno);
	}
#endif
}

int cafs_lite_init(const void * cfg){
	cl_snlist_item_t bad_serialno;
	const cafs_lite_cfg_t * cfgp = cfg;
	int err;
	int tmp;
	int bad_files;
	bool clean_open_blocks;
#ifndef __SIM__
	pthread_mutexattr_t mutexattr;

	if( pthread_mutexattr_init(&mutexattr) < 0 ){
		return -1;
	}

	pthread_mutexattr_setpshared(&mutexattr, true);
	pthread_mutexattr_setprioceiling(&mutexattr, 19);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);

	if ( pthread_mutex_init(&__cl_lock_object, &mutexattr) ){
		return -1;
	}
#endif

	if ( cl_dev_open(cfg) < 0 ){
		cl_error("failed to open dev\n");
		return -1;
	}

	bad_files = 0;
	clean_open_blocks = false;
	while( (err = cl_serialno_init(cfg, &bad_serialno)) == 1 ){
		if( (tmp = cl_file_clean(cfg, bad_serialno.serialno, bad_serialno.block, bad_serialno.status)) < 0 ){
			cl_error("failed to clean file\n");
			return -1;
		}

		if ( tmp == 1 ){
			clean_open_blocks = true;
		}

		bad_files++;
		if ( bad_files > 4 ){
			err = -1;
			break;
		}
	}

	if ( err == -1 ){
		//failed to find initial serial numbers so no other access is allowed
		cfgp->open_file->fs = NULL;
		return -1;
	}

	if ( cl_scratch_init(cfg) < 0 ){
		cl_error("failed to restore scratch area\n");
		return -1;
	}

	if ( clean_open_blocks == true ){
		//scan all blocks and discard "OPEN" blocks
		if ( cl_block_discardopen(cfg) < 0 ){
			cl_error("failed to discard open blocks\n");
			return -1;
		}
	}

	cl_debug(DEBUG_LEVEL, "Found %d bad files\n", bad_files);

	//start a new thread to handle reads/writes if asynchronous IO will be supported
	return 0;
}


int cafs_lite_mkfs(const void * cfg){
	int ret;
	const cafs_lite_cfg_t * cfgp = cfg;
	lock_cafs_lite();
	ret = 0;
	cfgp->open_file->fs = cfgp->devfs;
	cl_debug(DEBUG_LEVEL, "Erase device\n");
	if ( (ret = cl_dev_erase(cfg)) < 0 ){
		cfgp->open_file->fs = NULL;
		cl_error("failed to erase\n");
		errno = ENOSPC;
	} else {
		cl_debug(DEBUG_LEVEL, "Init serial number\n");
		if ( (ret = cl_serialno_mkfs(cfg)) < 0 ){
			//failed to format so no other access is allowed
			cfgp->open_file->fs = NULL;
			errno = EIO;
		}
	}
	unlock_cafs_lite();
	return ret;
}

int cafs_lite_fstat(const void * cfg, void * handle, struct stat * stat){
	cl_handle_t * h = (cl_handle_t*)handle;
	cl_block_data_t tmp;
	int ret;
	ret = 0;

	lock_cafs_lite();
	if ( (ret = cl_block_load(cfg, h->hdr_block, &tmp)) < 0 ){
		cl_error("failed to load header block\n");
		errno = EIO;
	} else {
		stat->st_ino = h->segment_data.hdr.serialno;
		stat->st_size = h->size;
		stat->st_blocks = (h->size + BLOCK_DATA_SIZE - 1) / BLOCK_DATA_SIZE;
		stat->st_blksize = BLOCK_DATA_SIZE;
		stat->st_mode = 0666 | S_IFREG;
		//stat->st_atime = 0;
		stat->st_ctime = 0;
		stat->st_mtime = 0;
	}

	unlock_cafs_lite();
	return ret;
}

int cafs_lite_stat(const void * cfg, const char * path, struct stat * stat){
	cl_handle_t handle;
	cl_dir_lookup_t entry;
	int ret;

	CL_TP(CL_PROB_RARE);

	lock_cafs_lite();
	cl_debug(DEBUG_LEVEL, "path:%s\n", path);
	ret = cl_dir_exists(cfg, path, &entry, R_OK);
	if ( ret < 0 ){
		cl_error("failed to check existence\n");
	} else if ( ret != CAFS_DIR_PATH_EXISTS ){
		//path does not exist
		cl_error("path does not exist\n");
		errno = ENOENT;
		ret = -1;
	} else {
		//open the file
		cl_debug(DEBUG_LEVEL, "open file\n");
		if ( cl_file_open(cfg, &handle, entry.serialno, R_OK, false) < 0 ){
			cl_error("failed to open file\n");
			ret = -1;
		} else {
			//call fstat
			cl_debug(DEBUG_LEVEL, "run stat\n");
			if( cafs_lite_fstat(cfg, &handle, stat) < 0 ){
				//cl_fstat() will set the error number
				cl_error("failed to run fstat\n");
				ret = -1;
			}
			ret = 0;
		}
	}
	unlock_cafs_lite();
	return ret;
}

int cafs_lite_unlink(const void * cfg, const char * path){
	cl_dir_lookup_t entry;
	int ret;

	lock_cafs_lite();

	ret = cl_dir_exists(cfg, path, &entry, W_OK);
	if ( ret < 0 ){
		ret = -1;
		goto cafs_lite_unlink_unlock;
	}

	if ( ret != CAFS_DIR_PATH_EXISTS ){
		//path does not exist
		errno = ENOENT;
		ret = -1;
		goto cafs_lite_unlink_unlock;
	}

	ret = 0;

	cl_debug(DEBUG_LEVEL, "unlink serialno %d\n", entry.serialno);

	if ( cl_serialno_get(cfg, entry.serialno, CAFS_SNLIST_ITEM_STATUS_OPEN, NULL) != BLOCK_INVALID ){
		//the file is open -- cannot delete
		errno = EACCES;
		ret = -1;
		goto cafs_lite_unlink_unlock;
	}

	//clean up the file system
	if ( cl_file_remove(cfg, entry.serialno) < 0 ){
		ret = -1;
		goto cafs_lite_unlink_unlock;
	}

	cafs_lite_unlink_unlock:
	unlock_cafs_lite();
	return ret;
}

int cafs_lite_remove(const void * cfg, const char * path){
	CL_TP(CL_PROB_IMPROBABLE);
	return cafs_lite_unlink(cfg, path);
}

int cafs_lite_open(const void * cfg, void ** handle, const char * path, int flags, int mode){
	int err;
	int ret;
	cl_handle_t * h;
	cl_dir_lookup_t entry;
	const char * name;
	int amode;

	amode = sysfs_getamode(flags);

	lock_cafs_lite();

	//lock
	if ( (err = cl_dir_exists(cfg, path, &entry, amode)) < 0 ){
		//unlock
		errno = ENOTDIR;
		ret = -1;
		h = NULL;
		goto cafs_lite_open_unlock;
	}

	cl_debug(DEBUG_LEVEL, "Open serialno %d (%d)\n", entry.serialno, err);

	//See if there is a slot for an open file
	h = malloc(sizeof(cl_handle_t));
	if ( h == NULL ){
		//errno should be set by malloc
		ret = -1;
		goto cafs_lite_open_unlock;
	}

	ret = 0;
	name = sysfs_getfilename(path, NULL);
	if ( err == CAFS_DIR_PATH_EXISTS ){
		//The file already exists
		if ( (flags & O_EXCL) && (flags & O_CREAT) ){
			//it is an error to try to exclusively create a file that already exists
			errno = EEXIST;
			ret = -1;
		} else if ( flags & O_TRUNC ){

			//truncate file (use the same serialno)
			cl_debug(DEBUG_LEVEL, "create new file (old file truncated on close)\n");
			if ( cl_file_open(cfg, h, entry.serialno, amode, true) < 0 ){
				errno = ENOSPC;
				ret = -1;
			}

		} else {
			//open the existing file
			if ( cl_file_open(cfg, h, entry.serialno, amode, false) < 0 ){
				errno = ENOSPC;
				ret = -1;
			} else  if ( h->size < 0 ){
				//There is an error with this file -- it is likely corrupt
				errno = EFBIG;
				ret = -1;
			}
		}
	} else if ( err == CAFS_DIR_PARENT_EXISTS ){ //does the path exist?
		if ( flags & (O_CREAT) ){
			//create a new file
			//printf("Create brand new file\n");
			entry.serialno = cl_serialno_new(cfg);
			if ( cl_file_new(cfg, h, name, mode, &entry, BLOCK_TYPE_FILE_HDR, amode) < 0 ){
				errno = ENOSPC;
				ret = -1;
			}

		} else {
			cl_debug(DEBUG_LEVEL, "file %s does not exist\n", path);
			errno = ENOENT;
			ret = -1;
		}
	} else {
		//Path does not exist
		cl_debug(DEBUG_LEVEL, "file %s does not exist here\n", path);
		errno = ENOENT;
		ret = -1;
	}

	//unlock()
	if ( ret == -1 ){
		free(h);
		h = NULL;
	}

	CL_TP_DESC(CL_PROB_IMPROBABLE, "file has been opened");
	cafs_lite_open_unlock:
	*handle = h;
	unlock_cafs_lite();
	return ret;
}

int cafs_lite_read(const void * cfg, void * handle, int flags, int loc, void * buf, int nbyte){
	cl_handle_t * h = (cl_handle_t*)handle;
	device_transfer_t op;
	int ret;

	h->op = &op;

	op.loc = loc;
	op.buf = buf;
	op.nbyte = nbyte;
	op.callback = NULL;
	op.context = NULL;

	CL_TP(CL_PROB_IMPROBABLE);

	cl_debug(DEBUG_LEVEL + 5, "read %d bytes\n", nbyte);

	if ( (h->amode & R_OK) == 0 ){
		cl_error("no read access\n");
		errno = EACCES;
		return -1;
	}

	if ( cl_file_startread(handle) == nbyte ){
		return op.nbyte;
	}

	lock_cafs_lite();

	if ( cl_file_finishread(cfg, handle) < 0 ){
		ret = -1;
	} else {
		ret = op.nbyte;
	}

	unlock_cafs_lite();
	return ret;
}


int cafs_lite_write(const void * cfg, void * handle, int flags, int loc, const void * buf, int nbyte){
	cl_handle_t * h = (cl_handle_t*)handle;
	device_transfer_t op;
	int ret;

	h->op = &op;

	if ( flags & O_APPEND ){
		loc = h->size;
	}

	op.loc = loc;
	op.cbuf = buf;
	op.nbyte = nbyte;
	op.callback = NULL;
	op.context = NULL;

	CL_TP(CL_PROB_IMPROBABLE);

	cl_debug(DEBUG_LEVEL + 5, "write %d bytes\n", nbyte);

	if ( (h->amode & W_OK) == 0 ){
		cl_error("no write access\n");
		errno = EACCES;
		return -1;
	}

	if ( cl_file_startwrite(handle) == nbyte ){
		return op.nbyte;
	}

	lock_cafs_lite();
	if ( cl_file_finishwrite(cfg, handle) < 0 ){
		ret = -1;
	} else {
		ret = op.nbyte;
	}
	unlock_cafs_lite();
	return ret;
}

int cafs_lite_close(const void * cfg, void ** handle){
	int ret;
	void * h;
	CL_TP(CL_PROB_COMMON);
	h = *handle;
	lock_cafs_lite();
	ret = cl_file_close(cfg, h);
	*handle = NULL;
	free(h);
	unlock_cafs_lite();
	return ret;
}

int cafs_lite_opendir(const void * cfg, void ** handle, const char * path){

	if( path[0] != 0 ){
		errno = ENOTDIR;
		return -1;
	}

	*handle = OPENDIR_HANDLE;
	return 0;
}

int cafs_lite_readdir_r(const void * cfg, void * handle, int loc, struct dirent * entry){
	cl_block_data_t hdr_cl_block_data;
	cl_hdr_t * hdr;
	cl_list_t sn_list;
	cl_snlist_item_t item;
	int count;
	int ret;

	if ( handle != OPENDIR_HANDLE ){
		errno = EBADF;
		return -1;
	}

	lock_cafs_lite();

	cl_debug(DEBUG_LEVEL, "initialize list\n");
	if ( cl_snlist_init(cfg, &sn_list, cl_serialno_getlistblock(cfg) ) < 0 ){
		ret = -1;
		goto cafs_lite_readdir_unlock;
	}

	count = 0;
	hdr = (cl_hdr_t *)hdr_cl_block_data.data;
	ret = -1;
	while( cl_snlist_getnext(cfg, &sn_list, &item) == 0 ){
		if( (item.status == CAFS_SNLIST_ITEM_STATUS_CLOSED) && (item.serialno != CL_SERIALNO_LIST) ){
			if ( count == loc ){
				if ( cl_block_load(cfg, item.block, &hdr_cl_block_data) ){
					cl_error("failed to load block %d for serialno:%d\n", item.block, item.serialno);
					ret = -1;
					goto cafs_lite_readdir_unlock;
				}

				strcpy(entry->d_name, hdr->open.name);
				entry->d_ino = item.serialno;
				ret = 0;
				goto cafs_lite_readdir_unlock;
			}
			count++;
		}
	}

	cafs_lite_readdir_unlock:
	unlock_cafs_lite();
	//errno is not changed
	return ret;
}

int cafs_lite_closedir(const void * cfg, void * handle){
	if ( handle != OPENDIR_HANDLE ){
		errno = EBADF;
		return -1;
	}
	return 0;
}

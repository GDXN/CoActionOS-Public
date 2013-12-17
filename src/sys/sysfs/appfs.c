/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <stddef.h>
#include <reent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "hwpl/task.h"
#include "hwpl/core.h"
#include "hwpl/mem.h"
#include "appfs_local.h"
#include "../sched/sched_flags.h"

#define ANALYZE_PATH_NOENT -1
#define ANALYZE_PATH_ROOT 0
#define ANALYZE_PATH_INSTALL 1
#define ANALYZE_PATH_FLASH 2
#define ANALYZE_PATH_FLASH_DIR 3
#define ANALYZE_PATH_RAM 4
#define ANALYZE_PATH_RAM_DIR 5

static int analyze_path(const char * path, const char ** name, int * mem_type){
	int elements;

	if ( sysfs_ispathinvalid(path) == true ){
		return -1;
	}

	*name = sysfs_getfilename(path, &elements);
	if ( strcmp(*name, "") == 0 ){
		return ANALYZE_PATH_ROOT;
	}

	if (elements == 1){
		if ( strcmp(*name, ".install") == 0 ){
			return ANALYZE_PATH_INSTALL;
		}

		if ( strcmp(*name, "flash") == 0 ){
			return ANALYZE_PATH_FLASH_DIR;
		}

		if ( strcmp(*name, "ram") == 0 ){
			return ANALYZE_PATH_RAM_DIR;
		}

		errno = ENOENT;
		return ANALYZE_PATH_NOENT;

	} else if ( elements == 2 ){
		if ( strncmp(path, "flash/", 6) == 0 ){
			*mem_type = MEM_PAGEINFO_TYPE_FLASH;
			return ANALYZE_PATH_FLASH;
		}

		if ( strncmp(path, "ram/", 4) == 0 ){
			*mem_type = MEM_PAGEINFO_TYPE_RAM;
			return ANALYZE_PATH_RAM;
		}

	} else if ( elements > 2 ){
		errno = ENOENT;
		return ANALYZE_PATH_NOENT;
	}

	return -1;

}

static bool is_free(const char * name){
	if ( strncmp(name, ".free", 5) == 0 ){
		return true;
	}
	return false;
}

static bool is_sys(const char * name){
	if ( strncmp(name, ".sys", 4) == 0 ){
		return true;
	}
	return false;
}

int appfs_init(const void * cfg){
	int i;
	mem_attr_t attr;
	priv_load_fileinfo_t info;
	uint32_t buf[APPFS_RAM_USAGE_WORDS];
	const device_t * dev;
	dev = cfg;

	//the RAM usage table needs to be initialized
	memset(buf, 0, APPFS_RAM_USAGE_BYTES);

	//first mark the RAM used by the OS using _data and _edata, etc
	appfs_ram_setrange(buf, _hwpl_mem_getsyspage(), caoslib_system_memory_size, APPFS_MEMPAGETYPE_SYS);

	//now scan each flash page to see what RAM is used
	dev->driver.ioctl(&(dev->cfg), I_MEM_GETATTR, &attr);
	for(i=0; i < attr.flash_pages; i++){
		if ( (appfs_util_getfileinfo(&info, dev, i, MEM_PAGEINFO_TYPE_FLASH, NULL) == APPFS_MEMPAGETYPE_USER) && (appfs_util_isexecutable(&info.fileinfo) == true) ){

			//get the RAM page associated with the data start
			info.pageinfo.type = MEM_PAGEINFO_TYPE_QUERY;
			info.pageinfo.addr = (int)info.fileinfo.exec.ram_start;
			if ( dev->driver.ioctl(&(dev->cfg), I_MEM_GET_PAGEINFO, &info.pageinfo) < 0 ){
				continue;
			}

			appfs_ram_setrange(buf,
					(int)info.pageinfo.num,
					info.fileinfo.exec.ram_size,
					APPFS_MEMPAGETYPE_SYS);
		}
	}
	hwpl_core_privcall(appfs_ram_priv_saveusage, buf);
	return 0;
}

int appfs_startup(const void * cfg){
	int i;
	mem_attr_t attr;
	priv_load_fileinfo_t info;
	task_memories_t mem;
	int started;
	const device_t * dev;

	dev = cfg;

	//go through each flash page and look for programs that should be run on startup
	started = 0;
	dev->driver.ioctl(&(dev->cfg), I_MEM_GETATTR, &attr);
	for(i=0; i < attr.flash_pages; i++){
		if (appfs_util_getfileinfo(&info, dev, i, MEM_PAGEINFO_TYPE_FLASH, NULL) == APPFS_MEMPAGETYPE_USER ){
			if ( (appfs_util_isexecutable(&info.fileinfo) == true) &&
					(info.fileinfo.exec.options & APPFS_EXEC_OPTIONS_STARTUP) ){

				//start the process
				mem.code.addr = info.fileinfo.exec.code_start;
				mem.code.size = info.fileinfo.exec.code_size;
				mem.data.addr = info.fileinfo.exec.ram_start;
				mem.data.size = info.fileinfo.exec.ram_size;

				if ( sched_new_process(info.fileinfo.exec.startup,
						0,
						NULL,
						&mem,
						info.fileinfo.exec.ram_start) >= 0 ){
					started++;
				}

			}
		}
	}
	return started;
}

int appfs_mkfs(const void* cfg){
	//erase all user files in flash and ram
	return 0;
}

int appfs_open(const void * cfg, void ** handle, const char * path, int flags, int mode){
	int err;
	int ret;
	appfs_handle_t * h;
	priv_load_fileinfo_t args;
	int path_type;
	int mem_type;
	int size;
	const char * name;

	if ( (path_type = analyze_path(path, &name, &mem_type)) < 0 ){
		return -1;
	}

	switch(path_type){
	case ANALYZE_PATH_INSTALL:
	case ANALYZE_PATH_FLASH:
	case ANALYZE_PATH_RAM:
		break;
	default:
		errno = ENOENT;
		return -1;
	}

	//this function cannot create an executable file
	if ( mode & (S_IXUSR | S_IXGRP | S_IXOTH) ){
		errno = ENOTSUP;
		return -1;
	}

	//check the access to see if the operation is OK
	ret = 0;
	//allocate memory for the handle
	h = malloc(sizeof(appfs_handle_t));
	if ( h == NULL ){
		//errno should be set by malloc
		return -1;
	}

	if ( path_type == ANALYZE_PATH_INSTALL ){
		if( (flags & O_ACCMODE) != O_WRONLY ){
			errno = EINVAL;
			return -1;
		}
		h->is_install = true;
	} else {
		h->is_install = false;
		//check to see if the file exists
		err = appfs_util_lookupname(cfg, name, &args, mem_type, &size);
		if ( err == 0 ){ //the file already exists
			if ( (flags & O_EXCL) && (flags & O_CREAT) ){
				errno = EEXIST;
				ret = -1;
			} else if ( (flags & O_TRUNC) && ((flags & O_ACCMODE) != O_RDONLY) ){
				errno = ENOTSUP;
				return -1;
			} else {
				//open the existing file
				h->type.reg.beg_addr = args.pageinfo.addr;
				h->type.reg.page = args.pageinfo.num;
				h->type.reg.type = args.pageinfo.type;
				h->type.reg.size = size;

			}
		} else { //the file does not already exist
			if( flags & O_CREAT ){
				errno = ENOTSUP;
				ret = -1;
			} else {
				errno = ENOENT;
				ret = -1;
			}
		}
	}

	if ( ret == -1 ){
		free(h);
		h = NULL;
	}
	*handle = h;
	return ret;
}

int appfs_unlink(const void* cfg, const char * path){
	//this will erase the page associated with the filename -- system files are read-only
	priv_load_fileinfo_t args;
	int start_page;
	int end_page;
	int size_deleted;
	const char * name;
	int mem_type;

	if( analyze_path(path, &name, &mem_type) < 0 ){
		return -1;
	}

	//sys and free files cannot be deleted
	if ( is_sys(name) || is_free(name) ){
		errno = ENOTSUP;
		return -1;
	}

	//see if path exists
	if ( appfs_util_lookupname(cfg, name, &args, mem_type, NULL) < 0 ){
		errno = ENOENT;
		return -1;
	}


	//executable files are deleted based on the header file
	if ( mem_type == MEM_PAGEINFO_TYPE_FLASH ){
		start_page = args.pageinfo.num;
		size_deleted = args.pageinfo.size;  //start with the first page


		//need to read the size of each consecutive page until the size is met
		while(size_deleted < (args.fileinfo.exec.code_size + args.fileinfo.exec.data_size) ){
			if ( appfs_util_getpageinfo(cfg, &args.pageinfo) ){
				return -1;
			}
			size_deleted += args.pageinfo.size;
			args.pageinfo.num++;
		}

		end_page = args.pageinfo.num;

		appfs_util_erasepages(cfg, start_page, end_page);


	} else {
		if ( appfs_ram_setusage(args.pageinfo.num, args.fileinfo.exec.code_size, APPFS_MEMPAGETYPE_FREE) < 0 ){
			return -1;
		}
	}

	//check to see if the file is a data file (in this case no RAM is used)
	if( args.fileinfo.exec.signature != APPFS_CREATE_SIGNATURE ){
		//remove the application from the RAM usage table
		args.pageinfo.addr = (int)args.fileinfo.exec.ram_start;
		args.pageinfo.type = MEM_PAGEINFO_TYPE_QUERY;

		if ( appfs_util_getpageinfo(cfg, &args.pageinfo) ){
			return -1;
		}

		if ( appfs_ram_setusage(args.pageinfo.num, args.fileinfo.exec.ram_size, APPFS_MEMPAGETYPE_FREE) < 0 ){
			return -1;
		}
	}

	return 0;
}

int appfs_fstat(const void* cfg, void * handle, struct stat * st){
	appfs_handle_t * h;
	priv_load_fileinfo_t args;
	int size;

	h = handle;

	memset(st, 0, sizeof(struct stat));

	if ( h->is_install == true ){
		st->st_mode = S_IFBLK | 0222;
		return 0;
	}

	if ( appfs_util_getfileinfo(&args, cfg, h->type.reg.page, h->type.reg.type, &size) < 0 ){
		return -1;
	}

	st->st_ino = args.pageinfo.num;
	st->st_blksize = args.pageinfo.size;
	st->st_size = size;
	st->st_blocks = 1;
	//st->st_uid = args.fileinfo.hdr.uid;
	st->st_uid = 0;
	//st->st_gid = args.fileinfo.hdr.gid;
	st->st_gid = 0;
	st->st_mode = args.fileinfo.hdr.mode | S_IFREG;


	return 0;
}

int appfs_stat(const void* cfg, const char * path, struct stat * st){
	priv_load_fileinfo_t args;
	appfs_handle_t handle;
	int path_type;
	int mem_type;
	int size;
	const char * name;

	if ( (path_type = analyze_path(path, &name, &mem_type)) < 0 ){
		return -1;
	}

	switch(path_type){
	case ANALYZE_PATH_ROOT:
		return 0;
	case ANALYZE_PATH_INSTALL:
		handle.is_install = true;
		break;
	case ANALYZE_PATH_FLASH_DIR:
		st->st_mode = S_IFDIR | 0666;
		return 0;
	case ANALYZE_PATH_RAM_DIR:
		st->st_mode = S_IFDIR | 0666;
		return 0;

	case ANALYZE_PATH_RAM:
	case ANALYZE_PATH_FLASH:
		handle.is_install = false;
		//see if the path matches the name of any pages
		if ( appfs_util_lookupname(cfg, name, &args, mem_type, &size) < 0 ){
			errno = ENOENT;
			return -1;
		}
		handle.type.reg.size = size;
		handle.type.reg.beg_addr = args.pageinfo.addr;
		handle.type.reg.page = args.pageinfo.num;
		handle.type.reg.type = args.pageinfo.type;
	}

	return appfs_fstat(cfg, &handle, st);
}

int appfs_priv_read(const void* cfg, void * handle, device_transfer_t * op){
	const device_t * dev;
	appfs_handle_t * h;

	h = handle;
	dev = cfg;

	if ( op->loc >= h->type.reg.size ){
		op->nbyte = 0;
		return -1; //return EOF
	}

	//read should not go past the end of the file
	if ( (op->loc + op->nbyte) >= (h->type.reg.size) ){
		op->nbyte = h->type.reg.size - op->loc;
	}

	op->loc = (int)h->type.reg.beg_addr + op->loc;

	return dev->driver.read(&(dev->cfg), op);
}

int appfs_priv_write(const void* cfg, void * handle, device_transfer_t * op){
	errno = ENOTSUP;
	return -1;
}

int appfs_close(const void* cfg, void ** handle){
	//close a file
	appfs_handle_t * h = (appfs_handle_t*)*handle;
	free(h);
	h = NULL;
	return 0;
}

int appfs_opendir(const void* cfg, void ** handle, const char * path){
	if ( strcmp(path, "") == 0 ){
		*handle = (void*)0;
	} else if ( strcmp(path, "flash") == 0 ){
		*handle = (void*)1;
	} else if ( strcmp(path, "ram") == 0 ){
		*handle = (void*)2;
	} else {
		errno = ENOENT;
		return -1;
	}
	return 0;
}


int appfs_priv_ioctl(const void * cfg, void * handle, int request, void * ctl){
	appfs_handle_t * h = handle;
	appfs_installattr_t * attr;
	int ret;

	if ( h->is_install == false ){
		errno = EBADF;
		return -1;
	}

	attr = ctl;
	if( request == I_APPFS_INSTALL ){
		ret = appfs_util_priv_writeinstall(cfg, h, attr);
		return ret;
	} else if( request == I_APPFS_CREATE ){
		ret = appfs_util_priv_create(cfg, h, attr);
		return ret;
	}

	errno = EINVAL;
	return -1;
}

static int readdir_mem(const void* cfg, int loc, struct dirent * entry, int type){
	//this needs to load page number loc and see what the file is
	priv_load_fileinfo_t args;

	if ( appfs_util_getfileinfo(&args, cfg, loc, type, NULL) < 0 ){
		return -1;
	}

	strcpy(entry->d_name, args.fileinfo.hdr.name);
	entry->d_ino = loc;
	return 0;
}


static int readdir_root(const void * cfg, int loc, struct dirent * entry){
	switch(loc){
	case 0:
		strcpy(entry->d_name, ".install");
		break;
	case 1:
		strcpy(entry->d_name, "flash");
		break;
	case 2:
		strcpy(entry->d_name, "ram");
		break;
	default:
		return -1;
	}
	entry->d_ino = loc;
	return 0;
}


int appfs_readdir_r(const void* cfg, void * handle, int loc, struct dirent * entry){
	//this needs to load page number loc and see what the file is
	int h;
	h = (int)handle;
	switch(h){
	case 0:
		return readdir_root(cfg, loc, entry);
	case 1:
		return readdir_mem(cfg, loc, entry, MEM_PAGEINFO_TYPE_FLASH);
	case 2:
		return readdir_mem(cfg, loc, entry, MEM_PAGEINFO_TYPE_RAM);
	default:
		return -1;
	}
}



int appfs_closedir(const void* cfg, void * handle){
	//this doesn't need to do anything at all
	return 0;
}


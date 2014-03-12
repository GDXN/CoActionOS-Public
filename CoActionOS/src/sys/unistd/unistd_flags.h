/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef UNISTD_FLAGS_H_
#define UNISTD_FLAGS_H_

#include <stdint.h>
#include <sys/stat.h>
#include "dirent.h"
#include "sysfs.h"
#include "hwpl.h"

#define FILDES_STDIO_FLAG 0x0200


int unistd_new_open_file(int start);
void * unistd_device_open(const char * name, int flags);
int unistd_device_ioctl(open_file_t * open_file, int request, void * ctl);
int unistd_device_read(open_file_t * open_file, void * buf, int nbyte);
int unistd_device_write(open_file_t * open_file, const void * buf, int nbyte);

int unistd_init_stdio(int fildes);
int unistd_get_open_file(int fildes);
int unistd_fildes_is_bad(int fildes);
void unistd_reset_fildes(int fildes);

typedef struct {
	int err;
	const sysfs_t * fs;
	void * handle;
	int request;
	void * ctl;
} unistd_priv_attr_t;
void unistd_priv_ioctl(void * args);


static inline void * get_handle(int fildes) HWPL_ALWAYS_INLINE;
void * get_handle(int fildes){
	return _global_impure_ptr->procmem_base->open_file[fildes].handle;
}

static inline void ** get_handle_ptr(int fildes) HWPL_ALWAYS_INLINE;
void ** get_handle_ptr(int fildes){
	return &(_global_impure_ptr->procmem_base->open_file[fildes].handle);
}

static inline const sysfs_t * get_fs(int fildes) HWPL_ALWAYS_INLINE;
const sysfs_t * get_fs(int fildes){
	return (const sysfs_t*)_global_impure_ptr->procmem_base->open_file[fildes].fs;
}

static inline int get_flags(int fildes) HWPL_ALWAYS_INLINE;
int get_flags(int fildes){
	return _global_impure_ptr->procmem_base->open_file[fildes].flags;
}

static inline int get_loc(int fildes) HWPL_ALWAYS_INLINE;
int get_loc(int fildes){
	return _global_impure_ptr->procmem_base->open_file[fildes].loc;
}

static inline void * get_open_file(int fildes) HWPL_ALWAYS_INLINE;
void * get_open_file(int fildes){
	return (open_file_t*)&(_global_impure_ptr->procmem_base->open_file[fildes]);
}

static inline void set_loc(int fildes, int loc) HWPL_ALWAYS_INLINE;
void set_loc(int fildes, int loc){
	_global_impure_ptr->procmem_base->open_file[fildes].loc = loc;
}

static inline void set_handle(int fildes, void * handle) HWPL_ALWAYS_INLINE;
void set_handle(int fildes, void * handle){
	_global_impure_ptr->procmem_base->open_file[fildes].handle = handle;
}

static inline void set_fs(int fildes, const sysfs_t * fs) HWPL_ALWAYS_INLINE;
void set_fs(int fildes, const sysfs_t * fs){
	_global_impure_ptr->procmem_base->open_file[fildes].fs = (void*)fs;
}

static inline void set_flags(int fildes, int flags) HWPL_ALWAYS_INLINE;
void set_flags(int fildes, int flags){
	_global_impure_ptr->procmem_base->open_file[fildes].flags = flags;
}

static inline void dup_open_file(int new_fd, int old_fd) HWPL_ALWAYS_INLINE;
void dup_open_file(int new_fd, int old_fd){
	memcpy( &(_global_impure_ptr->procmem_base->open_file[new_fd]),
			&(_global_impure_ptr->procmem_base->open_file[old_fd]),
			sizeof(open_file_t));
}

static inline void update_loc(int fildes, int loc) HWPL_ALWAYS_INLINE;
void update_loc(int fildes, int loc){
	set_loc(fildes, loc);
}

#endif /* UNISTD_FLAGS_H_ */

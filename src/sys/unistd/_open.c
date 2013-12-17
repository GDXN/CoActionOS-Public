/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FILE_ACCESS
 * @{
 */

/*! \file */


#include <reent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/unistd.h>

#include "hwpl.h"
#include "unistd_fs.h"
#include "unistd_flags.h"
#include "../sched/sched_flags.h"

#if USE_STDIO != 0
extern const char _stdin_dev[];
extern const char _stdout_dev[];
extern const char _stderr_dev[];
#endif

#if SINGLE_TASK > 0
volatile bool waiting = false;
void * my_callback;
#endif


int unistd_new_open_file(int start){
	int i;
	for(i = start; i < OPEN_MAX; i++){
		if ( get_handle(i) == NULL ){
			set_handle(i, (void*)1);
			return i;
		}
	}
	errno = EMFILE;
	return -1;
}

void unistd_reset_fildes(int fildes){
	set_handle(fildes, NULL);
}

void set_open_file(int fildes, const sysfs_t * fs, void * handle, uint16_t flags){
	set_fs(fildes, fs);
	set_handle(fildes, handle);
	set_loc(fildes, 0);
	set_flags(fildes, flags);
}

/*! \details This function opens a file with flags being the OR'd combination of:
 * - O_RDONLY, O_WRONLY or O_RDWR
 * - O_NONBLOCK, O_CREAT, O_EXCL, O_TRUNC
 *
 * If the O_CREAT flag is set, the third argument should specify the mode as a mode_t.
 * The bits used with the mode are:
 * - S_IRWXU:  User read/write/execute
 * - S_IRUSR:  User read
 * - S_IWUSR:  User write
 * - S_IXUSR:  User execute
 * - S_IRWXG:  Group read/write/execute
 * - S_IRGRP:  Group read (groups not implemented)
 * - S_IWGRP:  Group write (groups not implemented)
 * - S_IXGRP:  Group execute (groups not implemented)
 * - S_IRWXO:  Other read/write/execute
 * - S_IROTH:  Other read
 * - S_IWOTH:  Other write
 * - S_IXOTH:  Other execute
 *
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - ENAMETOOLONG:  \a name exceeds PATH_MAX or a component of \a name exceeds NAME_MAX
 *  - ENOENT: \a name could not be found
 *  - EIO: IO error
 *  - EEXIST: \a name already exists and flags is not set to overwrite
 *  - ENOSPC: no more space left on the device
 *  - ENOMEM: not enough memory to open another file
 *	- ENOTDIR: the path to \a name does not exist
 *	- EFBIG: size error with the file (file is likely corrupt)
 *
 *
 */
int open(const char * name, int flags, ... );

int _open(const char * name, int flags, ...) {
	void * handle;
	int tmp;
	int fildes;
	va_list ap;
	int mode;
	const sysfs_t * fs;


	//Check the length of the filename
	if ( sysfs_ispathinvalid(name) == true ){
		//hwpl_debug("invalid path\n");
		return -1;
	}

	fs = sysfs_find(name, true); //see which filesystem we are working with first
	if ( fs == NULL ){ //if no filesystem is found for the path, return no entity
		errno = ENOENT;
		return -1;
	}

	fildes = unistd_new_open_file(0); //get a new file descriptor
	if ( fildes < 0 ){
		return -1;
	}

	tmp = 0;
	//check the access mode
	switch( (flags & O_ACCMODE) ){
	case O_RDONLY:
		tmp = R_OK;
		break;
	case O_WRONLY:
		tmp = W_OK;
		break;
	case O_RDWR:
		tmp = R_OK|W_OK;
		break;
	}

	if ( flags & O_CREAT ){
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
		tmp = mode & S_IFMT;
		switch(tmp){
		case S_IFDIR:
			//This is not the correct way to create a directory (must use mkdir)
			errno = EINVAL;
			return -1;
		case 0:
			//If no format is specified then create a regular file
			mode = mode | S_IFREG;
			break;
		}
	} else {
		mode = 0;
	}

	if ( fs->open(fs->cfg, &handle, sysfs_stripmountpath(fs, name), flags, mode) < 0 ){
		unistd_reset_fildes(fildes);
		return -1;
	}

	set_open_file(fildes, fs, handle, flags);
	if ( flags & O_APPEND ){
		lseek(fildes, 0, SEEK_END);
	}
	return fildes;
}

int unistd_fildes_is_bad(int fildes){
	if ( fildes < 0 ){
		errno = EBADF;
		return -1;
	}

	if ( (fildes) < OPEN_MAX ){
		//since _open() assigns handle to 1, and handle of 0 or 1 is invalid
		if ( ((int)get_handle(fildes) & ~(0x01)) != 0 ){
			return fildes;
		} else {
			errno = EBADF;
			return -1;
		}
	}

	if ( fildes & FILDES_STDIO_FLAG ){
		fildes = unistd_init_stdio(fildes);
	} else {
		errno = EBADF;
		return -1;
	}
	return fildes;
}

/*
typedef struct {
	int err;
	const device_t * handle;
} priv_open_device_t;
static void priv_open_device(void * args);
static int open_device(const char * name, int flags, int fildes);
static void set_open_file(int fildes, void * handle, uint16_t flags);

 */

int unistd_init_stdio(int fildes){
#if USE_STDIO != 0
	FILE * ptr;
	switch(fildes){
	case STDIN_FILENO:
		ptr = _REENT->_stdin;
		break;
	case STDOUT_FILENO:
		ptr = _REENT->_stdin;
		break;
	case STDERR_FILENO:
		ptr = _REENT->_stdin;
		break;
	default:
		errno = EBADF;
		return -1;
	}

	//! \todo Inherit the parent process stdio descriptors
	_REENT->_stdin->_file = _open(_stdin_dev, O_RDWR);
	_REENT->_stdout->_file = _open(_stdout_dev, O_RDWR);
	_REENT->_stderr->_file = _open(_stderr_dev, O_RDWR);

	return ptr->_file;
#else
	return 0;
#endif
}




/*! @} */


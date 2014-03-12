/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FILE_ACCESS
 * @{
 */

/*! \file */

#include  "unistd_fs.h"
#include  "unistd_flags.h"

/*! \details This function reads \a nbyte bytes from \a fildes to the memory
 * location pointed to by \a buf.
 *
 * read() is always a synchronous call which is either blocking or non-blocking
 * depending on the value of O_NONBLOCK for \a fildes.
 *
 * \param fildes The file descriptor returned by \ref open()
 * \param buf A pointer to the destination memory (process must have write access)
 * \param nbyte The number of bytes to read
 *
 * \return The number of bytes actually read of -1 with errno (see \ref ERRNO) set to:
 * - EBADF:  \a fildes is bad
 * - EACCESS:  \a fildes is on in O_WRONLY mode
 * - EIO:  IO error
 * - EAGAIN:  O_NONBLOCK is set for \a fildes and no new data is available
 *
 *
 */
int read(int fildes, void *buf, size_t nbyte);

int _read(int fildes, void *buf, size_t nbyte){
	void * handle;
	int flags;
	char * bufp;
	int loc;
	int bytes_read;
	const sysfs_t * fs;

	fildes = unistd_fildes_is_bad(fildes);
	if ( fildes < 0 ){
		return -1;
	}

	//This code will ensure that the process has permissions to access the specified memory
	//because reading is done in ISR (privileged mode) it will override the MPU
	if ( nbyte > 0 ){
		bufp = (char*)buf;
		loc = bufp[0];
		loc = bufp[nbyte-1];
	}


	if ( (get_flags(fildes) & O_ACCMODE) == O_WRONLY ){
		errno = EACCES;
		return -1;
	}

	fs = get_fs(fildes);
	if ( fs->priv_read != NULL ){  //This means the handle is not a regular file -- must be a device
		return unistd_device_read(get_open_file(fildes), buf, nbyte);
	}


	//initialize the file offset location
	loc = get_loc(fildes);
	flags = get_flags(fildes);
	handle = get_handle(fildes);
	bytes_read = fs->read(fs->cfg, handle, flags, loc, buf, nbyte);

	if ( bytes_read < 0 ){
		errno = bytes_read;
	} else {
		set_loc(fildes, loc + bytes_read);
	}

	return bytes_read;

}

/*! @} */

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UNI_FILE_ACCESS
 * @{
 */

/*! \file */

#include "config.h"

#include "dev/ioctl.h"
#include "hwpl.h"
#include <errno.h>
#include <stdarg.h>
#include "unistd_flags.h"
#include "hwpl/core.h"
#include "hwdl/sys.h"


/*! \details This function performs a control request on the device
 * associated with \a fildes. \a request is specific to the device.
 * The value of \a request determines what value should be passed
 * as the \a ctl argument.
 *
 * There are three types of devices which ioctl() can access.  The \a request
 * and \a ctl documentation for each device can be found in the respective
 * device type documentation:
 * - <a href="http://www.coactionos.com/swdoc/hwpl/html/group___p_e_r_i_p_h_i_o.html">HWPL IO</a>
 * - <a href="http://www.coactionos.com/swdoc/hwdl/html/index.html">HWDL</a>
 * - Custom (see driver specific documentation)
 *
 * \param fildes The file descriptor returned by \ref open()
 * \param request The request to the device.
 *
 * \return The number of bytes actually read of -1 with errno (see \ref ERRNO) set to:
 * - EBADF:  \a fildes is bad
 * - EIO:  IO error
 * - EAGAIN:  O_NONBLOCK is set for \a fildes and the device is busy
 *
 */
int ioctl(int fildes, int request, ...) {
	const sysfs_t * fs;
	fildes = unistd_fildes_is_bad(fildes);
	if ( fildes < 0 ){
		return -1;
	}

	fs = get_fs(fildes);
	if ( fs->priv_ioctl != NULL ){
		void * ctl;
		va_list ap;

		va_start(ap, request);
		ctl = va_arg(ap, void*);
		va_end(ap);

		//Character and device drivers both have the same interface to ioctl
		return unistd_device_ioctl(get_open_file(fildes), request, ctl);
	}

	errno = ENOTSUP;
	return -1;
}


int unistd_device_ioctl(open_file_t * open_file, int request, void * ctl){
	unistd_priv_attr_t args;
	args.fs = open_file->fs;
	args.handle = open_file->handle;
	args.request = request;
	args.ctl = ctl;

	hwpl_core_privcall(unistd_priv_ioctl, &args);
	return args.err;
}

void unistd_priv_ioctl(void * args){
	unistd_priv_attr_t * p = (unistd_priv_attr_t*)args;
	p->err = p->fs->priv_ioctl(
			p->fs->cfg,
			p->handle,
			p->request,
			p->ctl );
}

/*! @} */

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


/*! \addtogroup UNI_FILDES
 * @{
 */

/*! \file */

#include "unistd_fs.h"
#include "unistd_flags.h"

/*! \details This function checks to see if \a fildes is associated
 * with a terminal device.
 *
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - ENOTTY:  \a fildes is not associated with a terminal device
 *  - EBADF:  \a fildes is invalid
 *
 */
int isatty(int fildes);

int _isatty(int fildes){

	fildes = unistd_fildes_is_bad(fildes);
	if ( fildes < 0 ){
		return -1;
	}

	return 0;
}

/*! @} */


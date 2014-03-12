/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


/*! \addtogroup DIRENT
 * @{
 *
 * \details This interface accesses directory entries (both files and folders).  Here is an example of how to use this
 * interface.
 *
 * \code
 * #include <dirent.h>
 * #include <stdio.h>
 *
 * void show_directory(void){
 * 	DIR * dirp;
 * 	struct dirent entry;
 * 	struct dirent * result;
 *
 * 	dirp = opendir("/path/to/dir");
 * 	if ( dirp == NULL ){
 * 		perror("failed to open directory");
 * 		return;
 * 	}
 *
 * 	while( readdir_r(dirp, &entry, &result) == 0 ){
 * 		printf("Directory name (number): %s (%d)\n", entry.d_name, entry.d_ino);
 * 	}
 *
 * 	if ( closedir(dirp) < 0 ){
 * 		perror("failed to close directory");
 * 	}
 *
 * }
 *
 * \endcode
 *
 *
 */

/*! \file */

#include "config.h"
#include <unistd.h>
#include <malloc.h>
#include <errno.h>

#include "dirent.h"
#include "sysfs.h"
#include "hwpl/debug.h"


static int check_ebadf(DIR * dirp){
	if ( dirp == NULL ){
		errno = EBADF;
		return -1;
	}

	if ( dirp->loc > 65535 ){
		errno = EBADF;
		return -1;
	}
	return 0;
}

/*! \details This function closes the directory stream specified by \a dirp.
 *
 * \return Zero or -1 with errno (see \ref ERRNO) set to:
 * - EINVAL: \a dirp does not refere to an open directory stream
 *
 */
int closedir(DIR * dirp /*! A pointer to the open directory */){
	if (check_ebadf(dirp) < 0 ){
		return -1;
	}
	free(dirp);
	return 0;
}

/*! \details This function opens a directory.
 *
 * \return a pointer to the directory or NULL with errno (see \ref ERRNO) set to:
 * - ENOMEM: not enough memory
 * - ENOENT: \a dirname was not found
 * - EACCES: read access to \a dirname is not allowed
 * - ENAMETOOLONG: \a dirname exceeds \a PATH_MAX or an element of \a dirname exceeds \a NAME_MAX
 *
 */
DIR * opendir(const char * dirname){
	int err;
	DIR * dirp;
	const sysfs_t * fs;

	if ( sysfs_ispathinvalid(dirname) == true ){
		return NULL;
	}

	fs = sysfs_find(dirname, false); //see which filesystem we are working with first
	if ( fs == NULL ){ //if no filesystem is found for the path, return no entity
		errno = ENOENT;
		return NULL;
	}

	dirp = malloc(sizeof(DIR));
	if ( dirp == NULL ){
		errno = ENOMEM;
		return NULL;
	}

	//Open the directory and check for errors
	err = fs->opendir(fs->cfg, &(dirp->handle), sysfs_stripmountpath(fs, dirname));
	if ( err < 0 ){
		free(dirp);
		return NULL;
	}

	//Initialize the DIR structure
	dirp->fs = fs;
	dirp->loc = 0;

	//Return the pointer to the table
	return dirp;
}

/*! \details This function reads the next directory entry in the open directory.
 * \note This function is not thread-safe nor re-entrant;  use \ref readdir_r()
 * as a thread-safe, re-entrant alternative.
 *
 * \return a pointer to a dirent or NULL with errno (see \ref ERRNO) set to:
 * - EBADF: \a dirp is invalid
 * - ENOENT: the current position of the directory stream is invalid
 *
 */
struct dirent *readdir(DIR * dirp /*! a pointer to the directory structure */){
	static struct dirent result;
	struct dirent * tmp;
	readdir_r(dirp, &result, &tmp);
	return tmp;
}

/*! \details This function reads the next directory entry in the open directory (reentrant version).
 *
 * \return a pointer to a dirent or NULL with errno (see \ref ERRNO) set to:
 * - EBADF: \a dirp is invalid
 * - ENOENT: the current position of the directory stream is invalid
 *
 */
int readdir_r(DIR * dirp /*! a pointer to the directory structure */,
		struct dirent * entry /*! a pointer to the destination memory */,
		struct dirent ** result /*! this value is assigned to \a entry on success and NULL on failure */){
	int err;
	const sysfs_t * fs;
	if (check_ebadf(dirp) < 0 ){
		if ( result ){
			*result = NULL;
		}
		return -1;
	}

	fs = dirp->fs;
	err = fs->readdir_r(fs->cfg, dirp->handle, dirp->loc, entry);
	if ( err < 0 ){
		//errno is set by fs->readdir_r
		if ( result ){
			*result = NULL;
		}
		return -1;
	}

	dirp->loc++;

	if ( result ){
		*result = entry;
	}
	return 0;
}

/*! \details This function rewinds \a dirp.
 *
 */
void rewinddir(DIR * dirp /*! a pointer to the directory structure */){
	dirp->loc = 0;
}

/*! \details This function seeks to the specified location in
 * the directory.
 *
 */
void seekdir(DIR * dirp /*! a pointer to the directory structure */,
		long loc /*! the target location */){
	dirp->loc = loc;
}

/*! \details This function gets the current location in the
 * directory.
 *
 * \return The current directory location
 */
long telldir(DIR * dirp /*! a pointer to the directory structure */){
	return dirp->loc;
}

/*! @} */


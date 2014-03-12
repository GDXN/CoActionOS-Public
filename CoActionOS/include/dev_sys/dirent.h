/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup DIRENT
 * @{
 */

/*! \file */

#ifndef DIRENT_H_
#define DIRENT_H_

#include <sys/syslimits.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Directory Entry
 * \details This defines the data structure for a directory entry.
 *
 */
struct dirent {
	ino_t d_ino /*! \brief File Serial number */;
	char d_name[NAME_MAX+1] /*! \brief Name of entry */;
};

typedef struct {
	const void * fs;
	void * handle;
	long loc;
} DIR;

int closedir(DIR * dirp);
DIR *opendir(const char * dirname);
struct dirent *readdir(DIR * dirp);
int readdir_r(DIR * dirp, struct dirent * entry, struct dirent ** result);
void rewinddir(DIR * dirp);
void seekdir(DIR * dirp, long loc);
long telldir(DIR * dirp);

#ifdef __cplusplus
}
#endif

#endif /* DIRENT_H_ */

/*! @} */

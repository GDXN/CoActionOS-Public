/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#ifndef CAFS_DIR_H_
#define CAFS_DIR_H_

#include "cafs_lite_local.h"

typedef struct {
	serial_t serialno;
} cl_dir_lookup_t;

#define CAFS_DIR_PARENT_EXISTS 1
#define CAFS_DIR_PATH_EXISTS 2

int cl_dir_exists(const void * cfg, const char * path, cl_dir_lookup_t * dest, int amode);
int cl_dir_lookup(const void * cfg, const char * path, cl_dir_lookup_t * dest, int amode);


#endif /* CAFS_DIR_H_ */

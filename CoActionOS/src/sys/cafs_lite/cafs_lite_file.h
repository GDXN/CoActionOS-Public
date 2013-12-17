/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#ifndef CAFS_LITE_FILE_H_
#define CAFS_LITE_FILE_H_

#include <stdbool.h>

#include "cafs_lite_block.h"
#include "cafs_lite_dir.h"
#include "cafs_lite.h"
#include "cafs_lite_local.h"

typedef struct {
	int segment;
	int offset;
} cl_file_segment_t;


int cl_file_new(const void * cfg, cl_handle_t * handle, const char * name, int mode, cl_dir_lookup_t * entry, uint8_t type, int amode);
int cl_file_open(const void * cfg, cl_handle_t * handle, serial_t serialno, int amode, bool trunc);
int cl_file_close(const void * cfg, cl_handle_t * handle);
int cl_file_write(const void * cfg, cl_handle_t * handle, int start_segment, int nsegments);

int cl_file_remove(const void * cfg, serial_t serialno);

int cl_file_startread(cl_handle_t * handle);
int cl_file_finishread(const void * cfg, cl_handle_t * handle);

int cl_file_startwrite(cl_handle_t * handle);
int cl_file_finishwrite(const void * cfg, cl_handle_t * handle);

int cl_file_read(const void * cfg, cl_handle_t * handle, int start_segment,  int nsegments);
void cl_file_getsegment(const void * cfg, cl_file_segment_t * segment, int loc);

int cl_file_swapsegment(const void * cfg, cl_handle_t * handle, int new_segment);

int cl_file_loadsegment(const void * cfg, cl_handle_t * handle, int segment);
int cl_file_savesegment(const void * cfg, cl_handle_t * handle);

int cl_file_clean(const void * cfg, serial_t serialno, block_t hdr_block, uint8_t status);




#endif /* CAFS_LITE_FILE_H_ */

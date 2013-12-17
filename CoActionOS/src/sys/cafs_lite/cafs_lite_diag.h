/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#ifndef CAFS_DIAG_H_
#define CAFS_DIAG_H_


#include "cafs_lite_local.h"

typedef struct {
	int free_blocks;
	int allocated_blocks;
	int used_blocks;
	int dirty_blocks;
	int eraseable_blocks;
	int total_blocks;
	int dir_list_blocks;
	int file_data_blocks;
	int serialno_list_blocks;
	int del_serialno_list_blocks;
	int file_list_blocks;
} cl_diag_t;



typedef struct {
	int list_blocks;
	int dirty_blocks;
	int used_blocks;
	int allocated_blocks;
	int total_blocks;
} cl_diag_file_t;


int cl_diag_scan(const void * cfg);
int cl_diag_get(const void * cfg, cl_diag_t * dest);
void cl_diag_show(cl_diag_t * data);

int cl_diag_getfile(const void * cfg, const char * path, cl_diag_file_t * dest);
void cl_diag_showfile(cl_diag_file_t * data);

int cl_diag_show_eraseable(const void * cfg);

int cl_diag_scandir(const void * cfg, serial_t serialno);
int cl_diag_scanfile(const void * cfg, serial_t serialno);



#endif /* CAFS_DIAG_H_ */

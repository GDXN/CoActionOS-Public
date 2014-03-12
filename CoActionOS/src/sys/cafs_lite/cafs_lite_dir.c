/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */




#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "sysfs.h"

#include "cafs_lite_dir.h"
#include "cafs_lite_serialno.h"
#include "cafs_lite_list.h"

#define DEBUG_LEVEL 10
#define PROB_FAILURE 0.0

int cl_dir_lookup(const void * cfg, const char * path, cl_dir_lookup_t * dest, int amode){
	//just go through the serial number list and find the name

	cl_block_data_t hdr_cl_block_data;
	cl_hdr_t * hdr;
	cl_list_t sn_list;
	cl_snlist_item_t item;

	cl_debug(DEBUG_LEVEL, "initialize list\n");
	if ( cl_snlist_init(cfg, &sn_list, cl_serialno_getlistblock(cfg) ) < 0 ){
		errno = 1;
		return -1;
	}

	dest->serialno = SERIALNO_INVALID;
	hdr = (cl_hdr_t *)hdr_cl_block_data.data;
	while( cl_snlist_getnext(cfg, &sn_list, &item) == 0 ){

		if ( item.serialno == CL_SERIALNO_LIST ){
			continue;
		}

		if ( item.status == CAFS_SNLIST_ITEM_STATUS_CLOSED ){

			if ( cl_block_load(cfg, item.block, &hdr_cl_block_data) ){
				cl_error("failed to load block %d for serialno:%d\n", item.block, item.serialno);
				errno = 2;
				return -1;
			}

			cl_debug(DEBUG_LEVEL, "Checking %s to %s\n", path, hdr->open.name);
			if ( strncmp(path, hdr->open.name, NAME_MAX) == 0 ){
				dest->serialno = item.serialno;
				return 0;
			}
		}
	}

	return 0;
}


/*! \details This functions checks to see if a file and/or it's parent directory
 * exists.
 *
 * \return
 * - 0: neither exist
 * - 1: parent only exists
 * - 2: both file and parent exist
 */
int cl_dir_exists(const void * cfg, const char * path, cl_dir_lookup_t * dest, int amode){

	cl_debug(DEBUG_LEVEL, "lookup %s\n", path);
	if ( cl_dir_lookup(cfg, path, dest, amode) < 0 ){
		cl_error("failed to lookup\n");
		return -1;
	}

	if ( dest->serialno != SERIALNO_INVALID ){
		//parent and target exist
		return CAFS_DIR_PATH_EXISTS;
	}

	return CAFS_DIR_PARENT_EXISTS; //parent only exists
}




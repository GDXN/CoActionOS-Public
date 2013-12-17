/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#ifndef CAFS_BLOCK_H_
#define CAFS_BLOCK_H_

#include "cafs_lite_local.h"

/*! \details This function finds a free data block.  The data block will
 * be marked as "allocated" when this function returns.  The function will try to allocate
 * the data in an erasable block that is held by other blocks with the same file serial number.
 */
block_t cl_block_alloc(const void * cfg, serial_t serialno, block_t hint, uint8_t type);
int cl_block_save(const void * cfg, block_t cl_block_num, cl_block_data_t * data);
int cl_block_saveraw(const void * cfg, block_t cl_block_num, cl_block_data_t * data);
int cl_block_load(const void * cfg, block_t cl_block_num, cl_block_data_t * data);
int cl_block_loadhdr(const void * cfg, cl_block_hdr_t * dest, block_t src);
int cl_block_setstatus(const void * cfg, block_t cl_block_num, uint8_t status);

int cl_block_discardopen(const void * cfg);

serial_t cl_block_get_serialno(const void * cfg, block_t block);

block_t cl_block_geteraseable(const void * cfg);
int cl_block_gettotal(const void * cfg);
block_t cl_block_getfirst(const void * cfg);

static inline int cl_block_discard(const void * cfg, block_t cl_block_num) HWPL_ALWAYS_INLINE;
int cl_block_discard(const void * cfg, block_t cl_block_num){
	return cl_block_setstatus(cfg, cl_block_num, BLOCK_STATUS_DIRTY);
}

static inline int cl_block_close(const void * cfg, block_t cl_block_num) HWPL_ALWAYS_INLINE;
int cl_block_close(const void * cfg, block_t cl_block_num){
	return cl_block_setstatus(cfg, cl_block_num, BLOCK_STATUS_CLOSED);
}

#endif /* CAFS_BLOCK_H_ */

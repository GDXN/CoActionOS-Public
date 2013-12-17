/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#ifndef CAFS_SCRATCH_H_
#define CAFS_SCRATCH_H_

#include "cafs_lite_local.h"


enum {
	CAFS_SCRATCH_STATUS_UNUSED = 0xFF,
	CAFS_SCRATCH_STATUS_ALLOCATED = 0xFE,
	CAFS_SCRATCH_STATUS_COPIED = 0xFC,
	CAFS_SCRATCH_STATUS_ERASED = 0xF8,
	CAFS_SCRATCH_STATUS_RESTORED = 0x00
};

typedef struct HWPL_PACK {
	uint8_t status;
	block_t original_block; //0xFF for unused, X for in use, and 0x00 for dirty
	uint8_t checksum;
} cl_scratch_entry_t;


//return the number of free blocks in the scratchpad
int cl_scratch_init(const void * cfg);
int cl_scratch_erase(const void * cfg);
int cl_scratch_capacity(const void * cfg);
int cl_scratch_saveblock(const void * cfg, block_t cl_block_num);
int cl_scratch_restore(const void * cfg);


//on restart -- any entries in the scratch area must be restored to original locations


#endif /* CAFS_SCRATCH_H_ */

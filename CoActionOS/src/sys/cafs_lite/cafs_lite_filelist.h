/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#ifndef CAFS_FILELIST_H_
#define CAFS_FILELIST_H_

#include "cafs_lite_block.h"
#include "cafs_lite_list.h"

#define CAFS_FILELIST_BLOCK_ISFREE 0xFFFF
#define CAFS_FILELIST_BLOCK_ISDIRTY 0x0000


enum {
	CAFS_FILELIST_STATUS_FREE = 0xFF,
	CAFS_FILELIST_STATUS_CURRENT = 0xFE,
	CAFS_FILELIST_STATUS_OBSOLETE = 0xFC,
	CAFS_FILELIST_STATUS_DIRTY = 0x00
};

/*! \details This structure defines an entry in the file list.  The file
 * list keeps tracks of which blocks in the filesystem correspond to which
 * segment in the file.
 */
typedef struct {
	uint8_t status /*! the status of the entry */;
	uint8_t resd;
	block_t block /*! file block number */;
	serial_t segment /*! the segment number in the file */;
} cl_filelist_item_t;

block_t cl_filelist_get(const void * cfg, block_t list_block, int segment, uint8_t status, int * addr);
int cl_filelist_update(const void * cfg, block_t list_block, int segment, block_t new_block);
int cl_filelist_setstatus(const void * cfg, uint8_t status, int addr);
block_t cl_filelist_consolidate(const void * cfg, serial_t serialno, block_t list_block);

int cl_filelist_makeobsolete(const void * cfg, block_t list_block);

int cl_filelist_isfree(void * data);

static inline int cl_filelist_init(const void * cfg, cl_list_t * list, block_t list_block){
	return cl_list_init(cfg, list, list_block, sizeof(cl_filelist_item_t), cl_filelist_isfree);
}

static inline int cl_filelist_getnext(const void * cfg, cl_list_t * list,  cl_filelist_item_t * item){
	return cl_list_getnext(cfg, list, item, NULL);
}


#endif /* CAFS_FILELIST_H_ */

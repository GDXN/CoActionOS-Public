/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#ifndef CAFS_SERIALNO_H_
#define CAFS_SERIALNO_H_

#include "cafs_lite_list.h"
#include "cafs_lite_block.h"
#include "cafs_lite_local.h"

enum {
	CAFS_SNLIST_ITEM_STATUS_FREE = 0xFF,
	CAFS_SNLIST_ITEM_STATUS_OPEN = 0xFE,
	CAFS_SNLIST_ITEM_STATUS_CLOSING = 0xFC,
	CAFS_SNLIST_ITEM_STATUS_CLOSED = 0xF8,
	CAFS_SNLIST_ITEM_STATUS_DISCARDING_HDR = 0xF0,
	CAFS_SNLIST_ITEM_STATUS_DISCARDING_HDR_LIST = 0xE0,
	CAFS_SNLIST_ITEM_STATUS_DISCARDING = 0xC0,
	CAFS_SNLIST_ITEM_STATUS_DIRTY = 0x00
};


typedef struct HWPL_PACK {
	uint8_t status;
	serial_t serialno;
	block_t block;
	uint8_t checksum;
} cl_snlist_item_t;

#define CL_BLOCK_LIST 0x1234

int cl_serialno_init(const void * cfg, cl_snlist_item_t * bad_serialno_item);
serial_t cl_serialno_new(const void * cfg); //gets a new number (does not write the list)
block_t cl_serialno_get(const void * cfg, serial_t serialno, uint8_t status, int * addr);
int cl_serialno_append(const void * cfg, serial_t serialno, block_t new_block, int * addr, int status); //appends an entry as "open"
int cl_serialno_setstatus(const void * cfg, int addr, uint8_t status);
int cl_serialno_consolidate(const void * cfg);
int cl_serialno_mkfs(const void * cfg);
block_t cl_serialno_getlistblock(const void * cfg);
int cl_serialno_isfree(void * data);
int cl_serialno_scan(serial_t * serialno);

static inline int cl_snlist_init(const void * cfg, cl_list_t * list, block_t list_block){
	return cl_list_init(cfg, list, list_block, sizeof(cl_snlist_item_t), cl_serialno_isfree);
}

static inline int cl_snlist_getnext(const void * cfg, cl_list_t * status,  cl_snlist_item_t * item){
	return cl_list_getnext(cfg, status, item, NULL);
}


#endif /* CAFS_SERIALNO_H_ */

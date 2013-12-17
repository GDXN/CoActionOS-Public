/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#ifndef CAFS_LIST_H_
#define CAFS_LIST_H_

#include <stdint.h>
#include "cafs_lite_block.h"


typedef struct HWPL_PACK {
	cl_block_data_t block_data;
	block_t current_block;
	int8_t current_item;
	uint8_t total_in_block;
	uint8_t item_size;
	int (*is_free)(void*);
} cl_list_t;


typedef struct HWPL_PACK {
	block_t next;
	block_t prev;
} cl_list_hdr_t;


#define CAFS_LIST_STATUS_FREE (0xFFFF)
#define CAFS_LIST_STATUS_DIRTY (0x0000)
#define CAFS_LIST_NEXT_INVALID (0xFFFF)

#define CAFS_LIST_DATA_SIZE (BLOCK_DATA_SIZE - sizeof(cl_list_hdr_t))

typedef struct {
	cl_list_hdr_t hdr;
	char data[CAFS_LIST_DATA_SIZE];
} cl_list_block_t;

#define CAFS_LIST_NO_ANALYSIS 0
#define CAFS_LIST_DO_ANALYSIS 1

int cl_list_init(const void * cfg, cl_list_t * list, block_t list_block, int item_size, int (*is_free)(void*));
int cl_list_getnext(const void * cfg, cl_list_t * list,  void * item, int * addr);
int cl_list_append(const void * cfg, cl_list_t * list, uint8_t type, void * item, int * addr);
int cl_list_discard(const void * cfg, block_t list_block);
int cl_list_close(const void * cfg, block_t list_block);
block_t cl_list_consolidate(const void * cfg,
		serial_t serialno,
		block_t list_block,
		uint8_t type,
		int item_size,
		int (*is_dirty)(void*),
		int (*is_free)(void*));


#endif /* CAFS_LIST_H_ */

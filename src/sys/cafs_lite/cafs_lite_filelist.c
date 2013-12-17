/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <stddef.h>

#include "cafs_lite_filelist.h"
#include "cafs_lite_list.h"


#define DEBUG_LEVEL 10


int cl_filelist_isfree(void * data){
	cl_filelist_item_t * item;
	item = (cl_filelist_item_t*)data;

	if ( item->status == CAFS_FILELIST_STATUS_FREE ){
		return 1;
	} else {
		return 0;
	}
}

static int is_dirty(void * data){
	cl_filelist_item_t * item;
	item = (cl_filelist_item_t*)data;
	if ( item->status == CAFS_FILELIST_STATUS_DIRTY ){
		return 1;
	} else {
		return 0;
	}
}


block_t cl_filelist_get(const void * cfg, block_t list_block, int segment, uint8_t status, int * addr){
	cl_list_t list;
	cl_filelist_item_t item;

	if( cl_filelist_init(cfg, &list, list_block) < 0 ){
		cl_debug(DEBUG_LEVEL, "list block is invalid\n");
		return BLOCK_INVALID;
	}

	while( cl_list_getnext(cfg, &list, &item, addr) == 0 ){
		cl_debug(DEBUG_LEVEL + 3, "block:%d segment:%d = %d?\n", item.block, item.segment, segment);
		if ( (item.status == status) && (item.segment == segment) ){
			return item.block;
		}
	}

	return BLOCK_INVALID;
}

int cl_filelist_makeobsolete(const void * cfg, block_t list_block){
	cl_list_t list;
	cl_filelist_item_t item;
	int addr;

	if( cl_filelist_init(cfg, &list, list_block) < 0 ){
		cl_debug(DEBUG_LEVEL, "list block is invalid\n");
		return BLOCK_INVALID;
	}

	while( cl_list_getnext(cfg, &list, &item, &addr) == 0 ){
		if ( item.status == CAFS_FILELIST_STATUS_CURRENT){
			if (cl_filelist_setstatus(cfg, CAFS_FILELIST_STATUS_OBSOLETE, addr) < 0 ){
				cl_error("failed to mark entry as obsolete\n");
				return -1;
			}
		}
	}
	return 0;
}

int cl_filelist_setstatus(const void * cfg, uint8_t status, int addr){
	if ( cl_dev_write(cfg, addr + offsetof(cl_filelist_item_t, status), &status, sizeof(status)) != sizeof(status) ){
		return -1;
	}
	return 0;
}

int cl_filelist_update(const void * cfg, block_t list_block, int segment, block_t new_block){
	cl_filelist_item_t item;
	cl_list_t list;
	int addr;
	int strike_addr;
	int entry_num;

	if ( cl_filelist_init(cfg, &list, list_block) < 0 ){
		return -1;
	}

	strike_addr = -1;
	entry_num = 0;
	while( cl_list_getnext(cfg, &list, &item, &addr) == 0 ){
		//is this the one to strike??
		if( (item.segment == segment) && (item.status == CAFS_FILELIST_STATUS_CURRENT) ){
			strike_addr = addr;
			cl_debug(DEBUG_LEVEL, "striking segment %d block %d entry %d\n", item.segment, item.block, entry_num);
			break;
		}
		entry_num++;
	}


	cl_debug(DEBUG_LEVEL, "Appending entry %d segment %d\n", entry_num, segment);
	//The serial number is at the head of every block
	item.status = CAFS_FILELIST_STATUS_CURRENT;
	item.block = new_block; //The block location of the segment
	item.segment = segment; //The segment number in the file (unique)
	if( cl_list_append(cfg, &list, BLOCK_TYPE_FILE_LIST, &item, NULL) < 0 ){
		return -1;
	}

	//mark the old entry as obsolete
	if ( strike_addr != -1 ){
		if( cl_filelist_setstatus(cfg, CAFS_FILELIST_STATUS_OBSOLETE, strike_addr) < 0 ){
			return -1;
		}
	}

	return 0;
}

block_t cl_filelist_consolidate(const void * cfg, serial_t serialno, block_t list_block){
	return cl_list_consolidate(cfg, serialno, list_block, BLOCK_TYPE_FILE_LIST, sizeof(cl_filelist_item_t), is_dirty, cl_filelist_isfree);
}

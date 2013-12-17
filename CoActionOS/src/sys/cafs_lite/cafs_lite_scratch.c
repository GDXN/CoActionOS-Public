/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cafs_lite_block.h"
#include "cafs_lite_dev.h"
#include "cafs_lite_scratch.h"
#include "cafs_lite_tp.h"


#define DEBUG_LEVEL 4

static int get_scratch_entries(void){
	return SCRATCH_ENTRIES;
}

static int get_scratch_addr(const void * cfg){
	return cl_block_gettotal(cfg) * BLOCK_SIZE;
}

static int get_scratch_entry_addr(const void * cfg, int n){
	return get_scratch_addr(cfg) + n*sizeof(cl_scratch_entry_t);
}

static int get_scratch_block_addr(const void * cfg, int n){
	return get_scratch_addr(cfg) + sizeof(cl_scratch_entry_t) * get_scratch_entries() + BLOCK_SIZE*n;
}

static int save_scratch_entry(const void * cfg, int n, cl_scratch_entry_t * entry){
	int addr;
	addr = get_scratch_entry_addr(cfg, n); //get the address of the header entry
	if ( cl_dev_write(cfg, addr, entry, sizeof(*entry)) != sizeof(*entry) ){
		return -1;
	}
	return 0;
}


static void set_checksum(cl_scratch_entry_t * entry){
	uint8_t * p;
	int i;
	p = (uint8_t*)entry;
	entry->checksum = 0;
	//checksum of list and block
	for(i=0; i < sizeof(cl_scratch_entry_t) - 1; i++){
		entry->checksum += ~p[i];
	}
}

static int validate_checksum(cl_scratch_entry_t * entry){
	uint8_t * p;
	int i;
	p = (uint8_t*)entry;
	uint8_t checksum = 0;
	//checksum of list and block
	for(i=0; i < sizeof(cl_scratch_entry_t) - 1; i++){
		checksum += ~p[i];
	}
	cl_debug(DEBUG_LEVEL+4, "checksum 0x%X - 0x%X\n", checksum, entry->checksum);
	return checksum - entry->checksum;
}


static int load_scratch_entry(const void * cfg, int n, cl_scratch_entry_t * entry){
	int addr;
	addr = get_scratch_entry_addr(cfg, n); //get the address of the header entry
	if ( cl_dev_read(cfg, addr, entry, sizeof(*entry)) != sizeof(*entry) ){
		return -1;
	}
	return 0;
}

static int strike_scratch_entry(const void * cfg, int n){
	int addr;
	cl_scratch_entry_t entry;
	memset(&entry, 0, sizeof(entry));
	addr = get_scratch_entry_addr(cfg, n); //get the address of the header entry
	if ( cl_dev_write(cfg, addr, &entry, sizeof(entry)) != sizeof(entry) ){
		return -1;
	}
	return 0;
}

static int scratch_cl_block_is_ready(const void * cfg, int n){
	int addr;
	cl_block_data_t tmp;
	int i;
	addr = get_scratch_block_addr(cfg, n);
	//ensure the target block is all 0xFF -- this is done in case the header table is wrong because of a reset/power failure
	if ( cl_dev_read(cfg, addr, &tmp, sizeof(cl_block_data_t)) != sizeof(cl_block_data_t) ){
		return -1;
	}

	for(i=0; i < BLOCK_SIZE; i++){
		if ( ((unsigned char*)(&tmp))[i] != 0xFF ){
			return 0; //not writable
		}
	}
	return 1;
}

static int restore_block(const void * cfg, block_t original_block, int n){
	int addr;
	cl_block_data_t tmp;

	addr = get_scratch_block_addr(cfg, n);
	//ensure the target block is all 0xFF -- this is done in case the header table is wrong because of a reset/power failure
	if ( cl_dev_read(cfg, addr, &tmp, sizeof(cl_block_data_t)) != sizeof(cl_block_data_t) ){
		cl_error("could not read scratch block at 0x%X\n", addr);
		return -1;
	}

	cl_debug(DEBUG_LEVEL, "Restore block %d from scratch 0x%X (%d bytes) sn:%d\n", original_block, addr, (int)sizeof(cl_block_data_t), tmp.hdr.serialno );


	if ( cl_block_saveraw(cfg, original_block, &tmp) < 0 ){
		cl_error("Could not save block %d\n", original_block);
		return -1;
	}

	return 0;

}

static int copy_block_to_scratch(const void * cfg, block_t cl_block_num, int n){
	int addr;
	cl_block_data_t tmp;
	addr = get_scratch_block_addr(cfg, n);
	if ( cl_block_load(cfg, cl_block_num, &tmp) < 0 ){
		cl_error("FAILED TO LOAD BLOCK\n");
		return -1;
	}

	if ( cl_dev_write(cfg, addr, &tmp, sizeof(tmp)) != sizeof(tmp) ){
		cl_error("FAILED TO SAVE TO SCRATCH\n");
		return -1;
	}

	cl_debug(DEBUG_LEVEL, "Save block %d in scratch at 0x%X (%d bytes) sn:%d\n", cl_block_num, addr, (int)(sizeof(tmp)), tmp.hdr.serialno);
	return 0;
}

int cl_scratch_init(const void * cfg){
	//go through the scratch pad and restore any entries that are pending
	return cl_scratch_restore(cfg);
}

int cl_scratch_erase(const void * cfg){
	return cl_dev_erasesection(cfg, get_scratch_addr(cfg) );
}

int cl_scratch_capacity(const void * cfg){
	int i;
	int total;
	cl_scratch_entry_t entry;

	total = get_scratch_entries();

	for(i=0; i < total; i++){

		if ( load_scratch_entry(cfg, i, &entry) < 0 ){
			cl_error("Error loading scratch entry\n");
			return -1;
		}

		if ( entry.status == CAFS_SCRATCH_STATUS_UNUSED ){
			break;
		}
	}
	return total - i;
}

int cl_scratch_saveblock(const void * cfg, block_t cl_block_num){
	int i;
	int total;
	cl_scratch_entry_t entry;
	total = get_scratch_entries();

	//find a free entry in the table
	for(i=0; i < total; i++){

		if ( load_scratch_entry(cfg, i, &entry) < 0 ){
			cl_error("failed to load entry\n");
			return -1;
		}

		if ( entry.status == CAFS_SCRATCH_STATUS_UNUSED ){
			break;
		}
	}

	if ( i == total ){
		//The scratch pad is full
		cl_error("The scratch area is full\n");
		return -1;
	}

	//check to see if the scratch block is available
	if ( scratch_cl_block_is_ready(cfg, i) == 0 ){
		//block is not ready -- strike the entry
		if ( strike_scratch_entry(cfg, i) < 0 ){
			cl_error("failed to strike entry\n");
			return -1;
		}
		cl_error("block is not ready\n");
		return -1;
	}


	//copy the data to the scratch pad
	if ( copy_block_to_scratch(cfg, cl_block_num, i) < 0 ){
		cl_error("couldn't copy block %d to scratch %d\n", cl_block_num, i);
		return -1;
	}

	//update the table with the entry
	entry.original_block = cl_block_num;
	entry.status = CAFS_SCRATCH_STATUS_COPIED;
	set_checksum(&entry);
	if ( save_scratch_entry(cfg, i, &entry) < 0 ){
		return -1;
	}

	return 0;
}


int cl_scratch_restore(const void * cfg){
	//restore all the valid blocks in the scratch area
	int i;
	cl_scratch_entry_t entry;

	for(i=0; i < get_scratch_entries(); i++){
		if ( load_scratch_entry(cfg, i, &entry) < 0 ){
			cl_error("failed to load scratch entry %d\n", i);
			return -1;
		}

		if ( entry.status == CAFS_SCRATCH_STATUS_UNUSED ){
			break;
		}

		if ( entry.status == CAFS_SCRATCH_STATUS_COPIED ){

			if( validate_checksum(&entry) == 0 ){ //only restore blocks with valid checksums
				if ( restore_block(cfg, entry.original_block, i) < 0 ){
					cl_error("restore failed\n");
					return -1;
				}
			}

			if ( strike_scratch_entry(cfg, i) < 0 ){
				cl_error("Strike failed\n");
				return -1;
			}

		}
	}

	return 0;
}

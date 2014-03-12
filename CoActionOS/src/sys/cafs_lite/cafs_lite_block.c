/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

#include "cafs_lite_scratch.h"
#include "cafs_lite_serialno.h"
#include "cafs_lite_block.h"

#define DEBUG_LEVEL 10

static int get_cl_block_addr(block_t block){
	return BLOCK_SIZE * block;
}

static block_t alloc_block(const void * cfg, serial_t serialno, block_t hint, uint8_t type);
static int erase_dirty_blocks(const void * cfg, int max_written);
static int erase_dirty_block(const void * cfg, block_t cl_block_num);

block_t cl_block_geteraseable(const void * cfg){
	return cl_dev_geterasesize(cfg) / BLOCK_SIZE;
}

int cl_block_gettotal(const void * cfg){
	return (int)(cl_dev_getsize(cfg) - cl_dev_geterasesize(cfg)) / BLOCK_SIZE; //don't include the scratch area
}

static int mark_allocated(const void * cfg, block_t block, serial_t serialno, uint8_t type){
	cl_block_hdr_t hdr;
	hdr.type = type;
	hdr.serialno = serialno;
	hdr.status = BLOCK_STATUS_OPEN;
	return cl_dev_write(cfg, get_cl_block_addr(block), &hdr, sizeof(hdr));
}

/*! \details This function reads the serial number associated with the block.
 * \return The serial number for the specified block.
 */
serial_t cl_block_get_serialno(const void * cfg, block_t block){
	cl_block_hdr_t hdr;
	if ( cl_dev_read(cfg, get_cl_block_addr(block), &hdr, sizeof(hdr)) ){
		return -1;
	}
	return hdr.serialno;
}

/*! \details This function allocates a new block of data for the specified serial number.
 * The block is preferably allocated in
 * - 1.  an erasable section of blocks all associated with the same serial number
 * - 2.  a new eraseable block or
 * - 3.  an erasable block shared with other serial numbers
 *
 * \return The index of the new block
 */
block_t cl_block_alloc(const void * cfg, serial_t serialno, block_t hint, uint8_t type){
	block_t ret;
	int i;
	ret = alloc_block(cfg, serialno, hint, type);
	if ( ret == BLOCK_INVALID ){ //all blocks have been written

		//erasing blocks that are mostly dirty helps to wear the flash evenly
		if ( erase_dirty_blocks(cfg,  cl_block_geteraseable(cfg) >> 4) < 0 ){
			cl_error("failed to erase dirty blocks\n");
			return BLOCK_INVALID;
		}

		ret = alloc_block(cfg, serialno, hint, type);
		if ( ret == BLOCK_INVALID ){
			for(i = 3; i >= 1; i--){
				//This first looks for blocks that are mostly dirty
				if ( erase_dirty_blocks(cfg, cl_block_geteraseable(cfg) >> i) < 0 ){
					cl_error("failed to erase dirty blocks (%d)\n", i);
					return BLOCK_INVALID;
				}
				ret = alloc_block(cfg, serialno, hint, type);
				if ( ret != BLOCK_INVALID ){
					break;
				}
			}

			if ( ret == BLOCK_INVALID ){
				//This will rearrange blocks that are up to 75% clean
				if ( erase_dirty_blocks(cfg,  (cl_block_geteraseable(cfg) * 3) >> 2) < 0 ){
					cl_error("failed to erase dirty blocks here\n");
					return BLOCK_INVALID;
				}
				ret = alloc_block(cfg, serialno, hint, type);
			}
		}
	}
	return ret;
}


/*! \details This function saves the block data pointed to by \a src to the
 * location on disk associated with block \a dest.
 *
 * \return Zero on success
 */
int cl_block_save(const void * cfg, block_t cl_block_num, cl_block_data_t * data){

	if( cl_block_num == BLOCK_INVALID ){
		cl_error("invalid block\n");
		return -1;
	}

	cl_debug(DEBUG_LEVEL + 3, "save block %d\n", cl_block_num);
	data->hdr.status = BLOCK_STATUS_OPEN; //the block must be closed at a later time using cl_block_close()
	if ( cl_dev_write(cfg, get_cl_block_addr(cl_block_num) + offsetof(cl_block_hdr_t, status),
			&(data->hdr.status),
			sizeof(*data) - offsetof(cl_block_hdr_t, status)) !=
					sizeof(*data) - offsetof(cl_block_hdr_t, status) ){
		cl_error("failed to save block\n");
		return -1;
	}

	return 0;
}

int cl_block_saveraw(const void * cfg, block_t cl_block_num, cl_block_data_t * data){
	if ( cl_dev_write(cfg, get_cl_block_addr(cl_block_num), data, sizeof(*data) ) != sizeof(*data)  ){
		return -1;
	}
	return 0;
}

/*! \details This function loads the data associated with block \a src to the location
 * pointed to by \a dest.
 *
 * \return Zero on success
 */
int cl_block_load(const void * cfg, block_t cl_block_num, cl_block_data_t * data){
	//load the data from the device
	cl_debug(DEBUG_LEVEL + 2, "load block %d\n", cl_block_num);

	if ( cl_block_num == BLOCK_INVALID ){
		return -1;
	}

	if ( cl_dev_read(cfg, get_cl_block_addr(cl_block_num), data, BLOCK_SIZE) != BLOCK_SIZE ){
		cl_error("failed to read\n");
		return -1;
	}
	return 0;
}

int cl_block_loadhdr(const void * cfg, cl_block_hdr_t * dest, block_t src){
	//load the data from the device
	cl_debug(DEBUG_LEVEL, "load block %d\n", src);

	if ( src == BLOCK_INVALID ){
		return -1;
	}

	if ( cl_dev_read(cfg, get_cl_block_addr(src), dest, sizeof(cl_block_hdr_t)) != sizeof(cl_block_hdr_t) ){
		cl_error("failed to read\n");
		return -1;
	}
	return 0;
}

int cl_block_setstatus(const void * cfg, block_t block, uint8_t status){
	if ( block == BLOCK_INVALID ){
		return -1;
	}
	return cl_dev_write(cfg, get_cl_block_addr(block) + offsetof(cl_block_hdr_t, status), &status, sizeof(status));
}

int cl_block_discardopen(const void * cfg){
	cl_block_hdr_t hdr;
	int i;
	int total_blocks;

	total_blocks = cl_block_gettotal(cfg); //total number of blocks on the device

	for(i = FIRST_BLOCK; i < total_blocks; i++){
		//read the header
		if ( cl_block_loadhdr(cfg, &hdr, i) < 0 ){
			cl_error("failed to load header\n");
			return -1;
		}

		//check status for OPEN
		if (hdr.status == BLOCK_STATUS_OPEN){
			//discard any OPEN blocks
			if ( cl_block_discard(cfg, i) < 0 ){
				cl_error("failed to discard\n");
				return -1;
			}
		}
	}
	return 0;
}


block_t alloc_block(const void * cfg, serial_t serialno, block_t hint, uint8_t type){
	cl_block_hdr_t hdr;
	int i;
	int j;
	int first_loop;
	int total_blocks;
	int eraseable_blocks;
	int first;

	eraseable_blocks = cl_block_geteraseable(cfg);  //number of blocks that are eraseable contiguously
	total_blocks = cl_block_gettotal(cfg); //total number of blocks on the device
	first = FIRST_BLOCK;

	if ( hint < first ){
		hint = first;
	}

	if ( hint != BLOCK_INVALID ){
		first_loop = eraseable_blocks - ( hint % eraseable_blocks) + hint;

		//starting at hint -- find a free block within the erasable block

		for(i = hint+1; i < first_loop; i++){
			if ( cl_dev_read(cfg, get_cl_block_addr(i), &hdr, sizeof(hdr)) != sizeof(hdr) ){
				cl_error("failed to read device\n");
				return BLOCK_INVALID;
			}

			if ( hdr.status == BLOCK_STATUS_FREE ){
				if ( mark_allocated(cfg, i, serialno, type) < 0 ){
					cl_error("failed to mark block allocated\n");
					return BLOCK_INVALID;
				}
				return i;
			}
		}

	} else {
		i = first;
	}


	//now try to find a free erasable block
	for( ; i < total_blocks; i += eraseable_blocks){

		for(j = 0; j < eraseable_blocks; j++){

			if ( cl_dev_read(cfg, get_cl_block_addr(i+j), &hdr, sizeof(hdr)) != sizeof(hdr) ){
				cl_error("failed to read device here\n");
				return BLOCK_INVALID;
			}

			//See if this eraseable block is used by another serial number
			if ((hdr.status == BLOCK_STATUS_CLOSED) || (hdr.status == BLOCK_STATUS_OPEN)){
				if ( hdr.serialno != serialno ){
					break;
				} else {
					continue;
				}
			}

			if ( hdr.status == BLOCK_STATUS_FREE ){
				if ( mark_allocated(cfg, i+j, serialno, type) < 0 ){
					cl_error("failed to mark block allocated here\n");
					return BLOCK_INVALID;
				}
				return (i + j);
			}
		}
	}


	//now just find a block anywhere
	for(i = first; i < total_blocks; i++){
		if ( cl_dev_read(cfg, get_cl_block_addr(i), &hdr, sizeof(hdr)) != sizeof(hdr) ){
			cl_error("failed to read device there\n");
			return BLOCK_INVALID;
		}

		if ( hdr.status == BLOCK_STATUS_FREE ){
			if ( mark_allocated(cfg, i, serialno, type) < 0 ){
				cl_error("failed to mark block allocated there\n");
				return BLOCK_INVALID;
			}
			return i;
		}
	}

	cl_debug(DEBUG_LEVEL, "never found a block\n");
	return BLOCK_INVALID;
}

int erase_dirty_blocks(const void * cfg, int max_written){
	int i;
	int j;
	int total_blocks;
	int eraseable_blocks;
	cl_block_hdr_t hdr;

	int written;
	bool do_erase;

	eraseable_blocks = cl_block_geteraseable(cfg);  //number of blocks that are eraseable contiguously
	total_blocks = cl_block_gettotal(cfg); //total number of blocks on the device

	//now try to find a free erasable block
	for(i = 0; i < total_blocks; i += eraseable_blocks){
		written = 0;
		do_erase = true;
		for(j = 0; j < eraseable_blocks; j++){

			if ( cl_dev_read(cfg, get_cl_block_addr(i+j), &hdr, sizeof(hdr)) != sizeof(hdr) ){
				return -1;
			}

			//See if this eraseable block is used by another serial number
			if ( hdr.status == BLOCK_STATUS_CLOSED ){
				if ( hdr.serialno == CL_SERIALNO_LIST ){
					do_erase = false;
					break;
				}
				written++; //count how many blocks are finalized
			} else if ( hdr.status == BLOCK_STATUS_OPEN ){
				do_erase = false;
				break;
			} else if ( (hdr.status == BLOCK_STATUS_FREE) && ((i+j)!=0) ){
				do_erase = false;
				break;
			}
		}

		if ( (do_erase == true) && (written < max_written) ){
			if ( written > cl_scratch_capacity(cfg) ){
				if ( cl_scratch_erase(cfg) < 0 ){
					cl_error("failed to erase scratch area\n");
					return -1;
				}
			}

			if ( erase_dirty_block(cfg, i) < 0 ){
				cl_error("failed to erase dirty blocks\n");
				return -1;
			}
		}

	}
	return 0;
}

int erase_dirty_block(const void * cfg, block_t cl_block_num){
	//cl_block_num should be the start of an eraseable block
	int i;
	int eraseable_blocks;
	cl_block_hdr_t hdr;


	eraseable_blocks = cl_block_geteraseable(cfg);  //number of blocks that are eraseable contiguously
	for(i = cl_block_num; i < (cl_block_num + eraseable_blocks); i++){

		if ( cl_dev_read(cfg, get_cl_block_addr(i), &hdr, sizeof(hdr)) != sizeof(hdr) ){
			cl_error("failed to read device\n");
			return -1;
		}

		if ( (hdr.status == BLOCK_STATUS_CLOSED) ||
				(hdr.status == BLOCK_STATUS_OPEN) ){
			//Save this block in the scratch area
			if ( cl_scratch_saveblock(cfg, i) < 0 ){
				cl_error("failed to save block %d\n", i);
				return -1;
			}
		}
	}

	CL_TP_DESC(CL_PROB_RARE, "saved to scratch");

	//erase the eraseable block
	if ( cl_dev_erasesection(cfg, get_cl_block_addr(cl_block_num)) < 0 ){
		cl_error("failed to erase section\n");
		return -1;
	}

	CL_TP_DESC(CL_PROB_RARE, "section erased");

	//restore the scratch area
	if ( cl_scratch_restore(cfg) < 0 ){
		cl_error("failed to restore scratch area\n");
		return -1;
	}

	return 0;

}

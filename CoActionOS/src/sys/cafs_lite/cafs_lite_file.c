/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef __SIM__
#include "hwpl/core.h"
#endif


#include "cafs_lite_block.h"
#include "cafs_lite_local.h"

#include "cafs_lite_filelist.h"
#include "cafs_lite_serialno.h"
#include "cafs_lite_dir.h"
#include "cafs_lite_file.h"


#define DEBUG_LEVEL 10

static int cleanup_file(const void * cfg, block_t hdr_block, int addr, uint8_t status);
int mark_file_closed(const void * cfg, block_t hdr_block);


static int get_cl_block_data_addr(block_t block){
	return block * BLOCK_SIZE + offsetof(cl_block_data_t, data);
}

static void execute_callback(cl_handle_t * handle){
	handle->op->callback(handle->op->context, NULL);
}

static int calc_page_size(cl_handle_t * handle, int * offset){
	*offset = (handle->op->loc % BLOCK_DATA_SIZE);
	return (handle->bytes_left > ( BLOCK_DATA_SIZE - *offset )) ? (BLOCK_DATA_SIZE - *offset) : handle->bytes_left;
}

static void update_current_segment(cl_handle_t * handle, int page_size){
	handle->op->loc += page_size;
	handle->bytes_left -= page_size;
	handle->op->buf += page_size;
}

static int read_current_segment(cl_handle_t * handle){
	int page_size;
	int offset;
	page_size = calc_page_size(handle, &offset);
	if ( page_size > 0 ){
		memcpy(handle->op->buf, &(handle->segment_data.data[offset]), page_size);
		update_current_segment(handle, page_size);
	}
	return page_size;
}

static int write_current_segment(cl_handle_t * handle){
	int page_size;
	int offset;
	page_size = calc_page_size(handle, &offset);
	if ( page_size > 0 ){
		memcpy(&(handle->segment_data.data[offset]), handle->op->buf, page_size);
		handle->segment_data.hdr.status = BLOCK_STATUS_CLOSED;
		update_current_segment(handle, page_size);
		if( handle->op->loc > handle->size ){
			handle->size = handle->op->loc;
		}
	}
	return page_size;
}

int cl_file_startread(cl_handle_t * handle){
	int segment;
	segment = handle->op->loc / BLOCK_DATA_SIZE;

	//first check to see if the current segment is the segment being read
	handle->bytes_left = handle->op->nbyte;
	if ( handle->bytes_left + handle->op->loc > handle->size ){
		handle->bytes_left = handle->size - handle->op->loc;
		handle->op->nbyte = handle->bytes_left;
	}

	if ( segment == handle->segment ){
		read_current_segment(handle);
		if ( handle->bytes_left == 0 ){
			return handle->op->nbyte;
		}
	}
	return 0;
}

int cl_file_finishread(const void * cfg, cl_handle_t * handle){
	int nsegments;

	if ( cl_file_savesegment(cfg, handle) < 0 ){
		cl_error("could not save segment\n");
		return -1;
	}

	//load the first segment
	if ( cl_file_loadsegment(cfg, handle, handle->op->loc / BLOCK_DATA_SIZE) < 0){
		cl_error("could not load segment\n");
		return -1;
	}

	read_current_segment(handle);

	if( handle->bytes_left > 0 ){
		//read remaining full segments
		nsegments = handle->bytes_left / BLOCK_DATA_SIZE;
		if ( cl_file_read(cfg, handle, handle->segment + 1, nsegments) < 0 ){
			cl_error("failed to read file\n");
			return -1;
		}

		//read the final segment
		if ( cl_file_loadsegment(cfg, handle, handle->segment + 1) < 0){
			cl_error("failed to load final segment\n");
			return -1;
		}

		read_current_segment(handle);
	}

	if ( handle->op->callback != NULL ){
		hwpl_core_privcall((core_privcall_t)execute_callback, handle);
	}

	return 0;

}

int cl_file_startwrite(cl_handle_t * handle){
	int segment;
	segment = handle->op->loc / BLOCK_DATA_SIZE;
	if ( handle->op->loc > handle->size ){
		cl_error("loc is > size\n");
		return -1;
	}

	//first check to see if the current segment is the segment being written
	handle->bytes_left = handle->op->nbyte;
	if ( segment == handle->segment ){
		write_current_segment(handle);
		if ( handle->bytes_left == 0 ){
			return handle->op->nbyte; //The operation completed just using the RAM buffer
		}
	}

	//This means the operation must finish using cl_file_finishwrite()
	return 0;
}

int cl_file_finishwrite(const void * cfg, cl_handle_t * handle){
	int nsegments;

	//save the first segment or the previous segment
	if ( cl_file_savesegment(cfg, handle) < 0 ){
		cl_error("could not save segment\n");
		return -1;
	}

	//load the first segment
	if ( cl_file_loadsegment(cfg, handle, handle->op->loc / BLOCK_DATA_SIZE) < 0){
		return -1;
	}

	write_current_segment(handle);
	if ( handle->bytes_left > 0 ){
		if ( cl_file_savesegment(cfg, handle) < 0 ){
			cl_error("could not save next segment\n");
			return -1;
		}

		//write remaining full segments
		handle->segment++;
		nsegments = handle->bytes_left / BLOCK_DATA_SIZE;
		if ( cl_file_write(cfg, handle, handle->segment, nsegments) < 0 ){
			cl_error("could not write file\n");
			return -1;
		}

		//load the final segment
		if ( cl_file_loadsegment(cfg, handle, handle->segment) < 0){
			cl_error("could not load final segment\n");
			return -1;
		}

		write_current_segment(handle);
	}

	if ( handle->op->callback != NULL ){
		hwpl_core_privcall((core_privcall_t)execute_callback, handle);
	}
	return 0;
}

int cl_file_remove(const void * cfg, serial_t serialno){
	int addr;
	block_t cl_block_num;
	//lookup the block number
	cl_block_num = cl_serialno_get(cfg, serialno, CAFS_SNLIST_ITEM_STATUS_CLOSED, &addr);
	cl_debug(DEBUG_LEVEL, "remove serialno:%d at block:%d\n", serialno, cl_block_num);
	if ( cl_block_num == BLOCK_INVALID ){
		return -1;
	}

	//set serial number status to discarding header and list
	if ( cl_serialno_setstatus(cfg, addr, CAFS_SNLIST_ITEM_STATUS_DISCARDING) < 0 ){
		cl_error("failed to set status at addr 0x%X\n", addr);
		return -1;
	}

	return cleanup_file(cfg, cl_block_num, addr, CAFS_SNLIST_ITEM_STATUS_DISCARDING);
}


int cl_file_savesegment(const void * cfg, cl_handle_t * handle){
	block_t block;
	//save this to a new block in the file
	if ( handle->segment_data.hdr.status == BLOCK_STATUS_CLOSED){
		handle->mtime = 0;
		block = cl_block_alloc(cfg, handle->segment_data.hdr.serialno, handle->segment_list_block, BLOCK_TYPE_FILE_DATA);

		cl_debug(DEBUG_LEVEL + 2, "saving segment %d to block %d\n", handle->segment, block);

		if ( block == BLOCK_INVALID ){
			cl_error("could not alloc block\n");
			return -1;
		}

		//save the new block to the disk -- this will be closed when the file is closed
		if ( cl_block_save(cfg, block, &(handle->segment_data)) < 0 ){
			cl_error("could not save block %d\n", block);
			return -1;
		}

		//Enter the new block in the list
		if ( cl_filelist_update(cfg,
				handle->segment_list_block,
				handle->segment,
				block) < 0){
			cl_error("could not update file list\n");
			return -1;
		}

	} else {
		cl_debug(DEBUG_LEVEL + 2, "segment not dirty\n");
	}
	return 0;
}

int cl_file_loadsegment(const void * cfg, cl_handle_t * handle, int segment){
	block_t block;
	//save this to a new block in the file
	block = cl_filelist_get(cfg, handle->segment_list_block, segment, CAFS_FILELIST_STATUS_CURRENT, NULL);
	if ( block == BLOCK_INVALID ){ //the segment doesn't exist in the file; create it
		cl_debug(DEBUG_LEVEL + 2, "segment %d doesn't exist %d\n", segment, handle->segment_list_block);
		memset(handle->segment_data.data, 0, BLOCK_DATA_SIZE);
	} else {
		cl_debug(DEBUG_LEVEL + 2, "loading segment %d from block %d\n", segment, block);
		if( cl_block_load(cfg, block, &(handle->segment_data)) < 0 ){
			cl_error("failed to load segment (%d) data block (%d)\n", segment, block);
			return -1;
		}
	}
	handle->segment = segment;
	handle->segment_data.hdr.status = BLOCK_STATUS_OPEN;
	return 0;
}


int cl_file_swapsegment(const void * cfg, cl_handle_t * handle, int new_segment){
	block_t block;

	if ( new_segment == handle->segment ){
		//The new segment is equal to the current segment
		return 0;
	}

	//check if current buffer needs to be written to disk
	if ( cl_file_savesegment(cfg, handle) ){
		return -1;
	}

	//now load the new segment -- mark it as allocated
	handle->segment = new_segment;
	block = cl_filelist_get(cfg, handle->segment_list_block, new_segment, CAFS_FILELIST_STATUS_CURRENT, NULL);

	if ( block == BLOCK_INVALID ){ //the segment doesn't exist in the file; create it
		memset(handle->segment_data.data, 0, BLOCK_DATA_SIZE);
	} else {
		cl_block_load(cfg, block, &(handle->segment_data));
	}

	handle->segment_data.hdr.status = BLOCK_STATUS_OPEN;
	return 0;
}

int cl_file_write(const void * cfg, cl_handle_t * handle, int start_segment, int nsegments){
	int i;

	if ( nsegments > 0 ){
		handle->segment = start_segment;
		for(i=0; i < nsegments; i++){
			memcpy(handle->segment_data.data, handle->op->buf, BLOCK_DATA_SIZE);
			handle->segment_data.hdr.status = BLOCK_STATUS_CLOSED;
			if ( cl_file_savesegment(cfg, handle) < 0){
				return -1;
			}
			handle->segment++;
			handle->op->buf += BLOCK_DATA_SIZE;
		}

		handle->op->loc += (BLOCK_DATA_SIZE * nsegments);
		handle->bytes_left -= (BLOCK_DATA_SIZE * nsegments);

		if( handle->op->loc > handle->size ){
			handle->size = handle->op->loc;
		}
	}

	return 0;
}

int cl_file_read(const void * cfg, cl_handle_t * handle, int start_segment, int nsegments){
	int i;
	if ( nsegments > 0 ){
		for(i=0; i < nsegments; i++){
			if ( cl_file_loadsegment(cfg, handle, start_segment + i) < 0 ){
				return -1;
			}
			memcpy(handle->op->buf, handle->segment_data.data, BLOCK_DATA_SIZE);
			handle->op->buf += BLOCK_DATA_SIZE;
		}
		handle->op->loc += (BLOCK_DATA_SIZE * nsegments);
		handle->bytes_left -= (BLOCK_DATA_SIZE * nsegments);
	}
	return 0;
}

int cl_file_open(const void * cfg, cl_handle_t * handle, serial_t serialno, int amode, bool trunc){
	block_t block;
	cl_hdr_t * hdr;

	block = cl_serialno_get(cfg, serialno, CAFS_SNLIST_ITEM_STATUS_CLOSED, &(handle->serialno_addr));
	if ( block == BLOCK_INVALID ){
		cl_error("serialno does not exist\n");
		return -1;
	}

	cl_debug(DEBUG_LEVEL, "file exists at block %d\n", block);

	if ( cl_block_load(cfg, block, &(handle->segment_data)) < 0 ){
		cl_error("could not load block\n");
		return -1;
	}

	hdr = (cl_hdr_t*)(handle->segment_data.data);

	if ( handle->segment_data.hdr.type == BLOCK_TYPE_FILE_HDR ){

		if ( amode & W_OK  ){ //create a new entry
			cl_debug(DEBUG_LEVEL, "Creating a new entry for file writing\n");
			block = cl_block_alloc(cfg, serialno, block, BLOCK_TYPE_FILE_HDR);
			if ( block == BLOCK_INVALID ){
				cl_error("failed to alloc block for new header entry\n");
				return -1;
			}

			//add an entry in the serial number list -- this entry will be marked as open
			if ( cl_serialno_append(cfg, serialno, block, &(handle->serialno_addr), CAFS_SNLIST_ITEM_STATUS_OPEN) < 0 ){
				cl_error("failed to append new serialno %d %d\n", serialno, block);
				return -1;
			}

			if ( (hdr->open.content_block = cl_filelist_consolidate(cfg, serialno, hdr->open.content_block)) == BLOCK_INVALID ){
				cl_error("failed to consolidate list\n");
				return -1;
			}

			if ( trunc == true ){
				//consolidate the file list to a new block
				if ( cl_filelist_makeobsolete(cfg, hdr->open.content_block) < 0 ){
					cl_error("failed to make the list obsolete\n");
				}
				cl_debug(DEBUG_LEVEL, "file is being truncated to new list block %d\n", hdr->open.content_block);
				hdr->close.size = 0;
			} else {
				cl_debug(DEBUG_LEVEL, "file is being modified to new list block %d\n", hdr->open.content_block);
			}


			cl_debug(DEBUG_LEVEL, "New header block is open as serialno %d at block %d\n", serialno, block);
			//write the constant part of the file header
			if ( cl_dev_write(cfg, get_cl_block_data_addr(block) + offsetof(cl_hdr_t, open),
					&(hdr->open),
					sizeof(cl_hdr_open_t)) != sizeof(cl_hdr_open_t) ){
				cl_error("failed to write the open portion of header\n");
				return -1;
			}
		}

		handle->amode = amode;
		handle->hdr_block = block;
		handle->segment_list_block = hdr->open.content_block;
		handle->op = NULL;
		if ( hdr->close.size < 0 ){
			//In this case the file was never created properly and will be truncated
			handle->size = 0; //! \todo set the size to the number of segments * BLOCK_DATA_SIZE
			handle->mtime = 0;
		} else {
			handle->size = hdr->close.size;
		}

		cl_debug(DEBUG_LEVEL, "Load first segment\n");
		if ( cl_file_loadsegment(cfg, handle, 0) < 0 ){
			cl_error("failed to load segment 0\n");
			return -1;
		}

	}

	cl_debug(DEBUG_LEVEL, "File opened successfully\n");
	return 0;
}



int cl_file_new(const void * cfg, cl_handle_t * handle, const char * name, int mode, cl_dir_lookup_t * entry, uint8_t type, int amode){
	cl_hdr_t hdr;
	block_t block;
	block_t list_block;

	//Allocate the block for the file header
	if ( (block = cl_block_alloc(cfg, entry->serialno, BLOCK_INVALID, type)) == BLOCK_INVALID ){
		cl_error("failed to alloc header block\n");
		return -1;
	}

	cl_debug(DEBUG_LEVEL, "new file serialno %d block %d\n", entry->serialno, block);
	//mark the serial number as "open" in the serial number list
	if ( cl_serialno_append(cfg, entry->serialno, block, &(handle->serialno_addr), CAFS_SNLIST_ITEM_STATUS_OPEN) < 0 ){
		cl_error("failed to append serialno\n");
		return -1;
	}

	//Allocate the block for the file data list
	if ( (list_block = cl_block_alloc(cfg, entry->serialno, block, BLOCK_TYPE_FILE_LIST)) == BLOCK_INVALID ){
		cl_error("failed to alloc list block\n");
		return -1;
	}

	handle->hdr_block = block;
	handle->segment_list_block = list_block;
	handle->size = 0;
	handle->segment = SEGMENT_INVALID;
	handle->segment_data.hdr.status = BLOCK_STATUS_FREE;
	handle->segment_data.hdr.serialno = entry->serialno;
	handle->segment_data.hdr.type = BLOCK_TYPE_FILE_DATA;
	handle->mtime = 0;
	handle->amode = amode;
	handle->op = NULL;

	cl_debug(DEBUG_LEVEL, "new file: list block:%d\n", list_block);

	//populate the part of the header that is written on open
	strcpy(hdr.open.name, name);
	hdr.open.content_block = list_block;

	//write the constant part of the file header
	if ( cl_dev_write(cfg, get_cl_block_data_addr(block) + offsetof(cl_hdr_t, open),
			&(hdr.open),
			sizeof(cl_hdr_open_t)) != sizeof(cl_hdr_open_t) ){
		return -1;
	}

	return 0;
}

static int finish_close(const void * cfg,
		uint8_t discard_status,
		block_t new_block,
		block_t old_block,
		int new_addr,
		int old_addr,
		bool already_closed){
	bool close_failed;

	cl_debug(DEBUG_LEVEL, "need to discard old file list/header\n");
	//The old entry needs to be updated
	close_failed = false;

	if ( already_closed == false ){
		//update the status of the old and new serial number entries
		//Mark the new entry as closing
		cl_debug(DEBUG_LEVEL, "writing CLOSING to 0x%X\n", new_addr);
		if ( cl_serialno_setstatus(cfg, new_addr, CAFS_SNLIST_ITEM_STATUS_CLOSING) < 0 ){
			return -1;
		}

		CL_TP_DESC(CL_PROB_COMMON, "New file is CLOSING and old file is CLOSED");

		if ( mark_file_closed(cfg, new_block) < 0 ){
			cl_error("failed to close file blocks\n");
			close_failed = true;
		} else {

			CL_TP_DESC(CL_PROB_COMMON, "new file has been closed but not marked CLOSED");

			cl_debug(DEBUG_LEVEL, "writing 0x%X to 0x%X\n", discard_status, old_addr);
			if ( cl_serialno_setstatus(cfg, old_addr, discard_status) < 0 ){
				return -1;
			}

			CL_TP_DESC(CL_PROB_COMMON, "New file is CLOSING and old file is DISCARDING...");


			//Mark the new entry as closing
			if ( cl_serialno_setstatus(cfg, new_addr, CAFS_SNLIST_ITEM_STATUS_CLOSED) < 0 ){
				return -1;
			}
		}
	}

	if ( close_failed == false ){
		//test point
		CL_TP_DESC(CL_PROB_COMMON, "About to complete discard");

		//clean up the old file data
		if ( cleanup_file(cfg, old_block, old_addr, discard_status) < 0 ){
			cl_error("failed to clean\n");
			return -1;
		}
	}

	return 0;
}


int mark_file_closed(const void * cfg, block_t hdr_block){

	//this is done in a bottom up manner -- data, then list, then header
	cl_block_data_t tmp;
	cl_hdr_t * hdr;
	cl_list_t list;
	cl_filelist_item_t file_item;
	int filelist_addr;

	cl_debug(DEBUG_LEVEL, "close hdr block %d\n", hdr_block);
	//load the header which is located at cl_block_num
	if ( cl_block_load(cfg, hdr_block, &tmp) ){
		cl_error("failed to load block %d\n", hdr_block);
		return -1;
	}

	if ( tmp.hdr.status == BLOCK_STATUS_CLOSED ){
		//this file is already closed (up to the header)
		return 0;
	}

	if ( (tmp.hdr.type != BLOCK_TYPE_FILE_HDR) ){
		cl_error("header block is wrong type 0x%X\n", tmp.hdr.type);
		return -1;
	}

	hdr = (cl_hdr_t*)tmp.data;

	cl_debug(DEBUG_LEVEL, "discarding file data serialno: %d hdr %d list %d\n",
			tmp.hdr.serialno,
			hdr_block,
			hdr->open.content_block);
	//Discard all the segments in the file
	if ( cl_filelist_init(cfg, &list, hdr->open.content_block) < 0 ){
		cl_error("failed to init file list\n");
		return -1;
	}

	while( cl_list_getnext(cfg, &list, &file_item, &filelist_addr) == 0 ){

		if ( (file_item.status == CAFS_FILELIST_STATUS_CURRENT) ){
			cl_debug(DEBUG_LEVEL + 2, "closing current block %d\n", file_item.block);
			if ( cl_block_close(cfg, file_item.block) < 0 ){
				cl_error("failed to discard block\n");
				return -1;
			}
		} else if ( file_item.status == CAFS_FILELIST_STATUS_OBSOLETE ){
			//discard the associated data block
			cl_debug(DEBUG_LEVEL + 4, "discard this block %d 0x%X\n", file_item.block, file_item.status);
			if ( cl_block_discard(cfg, file_item.block) < 0 ){
				cl_error("failed to discard block\n");
				return -1;
			}

			//mark it dirty in the list
			if ( cl_filelist_setstatus(cfg, CAFS_FILELIST_STATUS_DIRTY, filelist_addr) < 0 ){
				cl_error("failed to set status to dirty\n");
				return -1;
			}
		}

		//close the list block if not already closed
		if ( list.block_data.hdr.status == BLOCK_STATUS_OPEN ){
			cl_block_close(cfg, list.current_block);
			list.block_data.hdr.status = BLOCK_STATUS_CLOSED;
		}

	}

	//close the list of file segments
	if ( cl_list_close(cfg, hdr->open.content_block) ){
		return -1;
	}

	//close the header
	if ( cl_block_close(cfg, hdr_block) < 0 ){
		return -1;
	}


	return 0;
}

int cl_file_close(const void * cfg, cl_handle_t * handle){
	cl_hdr_t hdr;
	block_t block;
	uint8_t discard_status;
	int old_addr;

	if ( !(handle->amode & W_OK) ){
		//if the file was not written (ie. opened read-only), nothing needs to be done here
		return 0;
	}

	//write the RAM segment to disk if it has been modified
	cl_debug(DEBUG_LEVEL, "Saving segment\n");
	if ( cl_file_savesegment(cfg, handle) ){
		cl_error("Failed to save file segment\n");
		return -1;
	}

	//write the header to the file -- with the size and modified/accessed date
	hdr.close.size = handle->size;

	cl_debug(DEBUG_LEVEL, "content block is %d\n", handle->segment_list_block);
	cl_debug(DEBUG_LEVEL, "write to header block %d\n", handle->hdr_block);

	CL_TP_DESC(CL_PROB_COMMON, "about to close (size not written)");


	//Write the information available on close (size is written on close)
	cl_debug(DEBUG_LEVEL, "close file (size:%d content block:%d)\n", hdr.close.size, handle->segment_list_block);
	if ( cl_dev_write(cfg, get_cl_block_data_addr(handle->hdr_block) + offsetof(cl_hdr_t, close),
			&hdr.close,
			sizeof(cl_hdr_close_t)) != sizeof(cl_hdr_close_t) ){
		cl_error("failed to write close data\n");
		return -1;
	}

	//this should be handled the same every time

	//get the address of the old serial number
	block = cl_serialno_get(cfg, handle->segment_data.hdr.serialno, CAFS_SNLIST_ITEM_STATUS_CLOSED, &old_addr);
	cl_debug(DEBUG_LEVEL, "Old block is %d\n", block);

	if ( block != BLOCK_INVALID ){

		CL_TP_DESC(CL_PROB_COMMON, "modifying but still open");

		//The old entry needs to be updated
		discard_status = CAFS_SNLIST_ITEM_STATUS_DISCARDING_HDR_LIST;

		cl_debug(DEBUG_LEVEL, "finish close status 0x%X hdr %d old hdr %d serialno %d\n",
				discard_status, handle->hdr_block, block, handle->segment_data.hdr.serialno);
		if ( finish_close(cfg, discard_status, handle->hdr_block, block, handle->serialno_addr, old_addr, false) < 0 ){
			return -1;
		}

	} else {

		cl_debug(DEBUG_LEVEL, "closing new file (file did not previously exist)\n");
		if ( cl_serialno_setstatus(cfg, handle->serialno_addr, CAFS_SNLIST_ITEM_STATUS_CLOSING) < 0 ){
			cl_error("failed to close out new serialno\n");
			return -1;
		}

		CL_TP_DESC(CL_PROB_COMMON, "CLOSING but not cleaned");

		//Close the file (includes discarding obsolete segments)
		if ( mark_file_closed(cfg, handle->hdr_block) < 0 ){
			cl_error("failed to clean file list block\n");
			return -1;
		}


		CL_TP_DESC(CL_PROB_COMMON, "cleaned but not CLOSED");


		if ( cl_serialno_setstatus(cfg, handle->serialno_addr, CAFS_SNLIST_ITEM_STATUS_CLOSED) < 0 ){
			cl_error("failed to close out new serialno\n");
			return -1;
		}

	}


	return 0;
}

int cl_file_clean(const void * cfg, serial_t serialno, block_t hdr_block, uint8_t status){
	int new_addr;
	int old_addr;
	block_t old_block;
	block_t new_block;
	cl_block_data_t cl_block_data;
	bool already_closed;


	cl_debug(DEBUG_LEVEL, "cleaning file %d\n", serialno);

	old_block = cl_serialno_get(cfg, serialno, status, &old_addr);
	if ( hdr_block != old_block ){
		cl_error("failed to clean file\n");
		return -1;
	}

	switch(status){
	case CAFS_SNLIST_ITEM_STATUS_DISCARDING:
		if ( cleanup_file(cfg, old_block, old_addr, status) < 0 ){
			cl_error("failed to complete discard\n");
			return -1;
		}
		break;
	case CAFS_SNLIST_ITEM_STATUS_DISCARDING_HDR_LIST:
	case CAFS_SNLIST_ITEM_STATUS_DISCARDING_HDR:
		//look for the new file
		if ( (new_block = cl_serialno_get(cfg, serialno, CAFS_SNLIST_ITEM_STATUS_CLOSING, &new_addr)) == BLOCK_INVALID ){
			//no "CLOSING" files exist -- check for "CLOSED"
			if ( (new_block = cl_serialno_get(cfg, serialno, CAFS_SNLIST_ITEM_STATUS_CLOSED, &new_addr)) == BLOCK_INVALID ){
				cl_error("new file is not found anywhere\n");
				return -1;
			} else {
				cl_debug(DEBUG_LEVEL, "new file is already closed\n");
				already_closed = true;
			}
		} else {
			already_closed = false;
		}

		if ( cl_block_load(cfg, new_block, &cl_block_data) < 0 ){
			cl_error("failed to load new block %d\n", new_block);
			return -1;
		}

		cl_debug(DEBUG_LEVEL, "finish closing and discarding file\n");
		if ( finish_close(cfg, status, new_block, old_block, new_addr, old_addr, already_closed) < 0 ){
			return -1;
		}
		break;

	case CAFS_SNLIST_ITEM_STATUS_CLOSING:

		//need to see if another file is "CLOSED"
		cl_debug(DEBUG_LEVEL, "cleaning CLOSING file\n");
		if ( (new_block = cl_serialno_get(cfg, serialno, CAFS_SNLIST_ITEM_STATUS_CLOSED, &new_addr)) == BLOCK_INVALID ){

			//the CLOSING file is the only one that exists -- it just needs to be clean and closed
			cl_debug(DEBUG_LEVEL, "clean CLOSING file (no other files)\n");

			if ( cl_block_load(cfg, old_block, &cl_block_data) < 0 ){
				cl_error("failed to load new block %d\n", new_block);
				return -1;
			}


			//Close the file (includes discarding obsolete segments)
			if ( mark_file_closed(cfg, hdr_block) < 0 ){
				cl_error("failed to clean file list block\n");
				return -1;
			}

			CL_TP_DESC(CL_PROB_COMMON, "cleaned but not CLOSED");

			if ( cl_serialno_setstatus(cfg, old_addr, CAFS_SNLIST_ITEM_STATUS_CLOSED) < 0 ){
				cl_error("failed to close file\n");
				return -1;
			}

		} else {

			cl_debug(DEBUG_LEVEL, "clean CLOSING file (other file is already closed)\n");

			//the file already exists as "CLOSED"
			cl_debug(DEBUG_LEVEL, "cleaning CLOSING file but original is still CLOSED\n");
			if ( cleanup_file(cfg, old_block, old_addr, CAFS_SNLIST_ITEM_STATUS_DISCARDING_HDR) < 0 ){
				cl_error("failed to complete discard\n");
				return -1;
			}

			return 1;
		}
		break;
	case CAFS_SNLIST_ITEM_STATUS_OPEN:

		cl_debug(DEBUG_LEVEL, "cleaning open file\n");
		if ( cl_serialno_setstatus(cfg, old_addr, CAFS_SNLIST_ITEM_STATUS_DIRTY) < 0 ){
			cl_error("failed to close file\n");
			return -1;
		}

		return 1;

		break;

	default:
		cl_error("unknown status %d\n", status);
		return -1;
	}

	return 0;
}


int cleanup_file(const void * cfg, block_t hdr_block, int addr, uint8_t status){
	cl_block_data_t tmp;
	cl_hdr_t * hdr;
	cl_list_t list;
	cl_filelist_item_t file_item;
	int filelist_addr;

	cl_debug(DEBUG_LEVEL, "clean block %d (0x%X)\n", hdr_block, status);
	//load the header which is located at cl_block_num
	if ( cl_block_load(cfg, hdr_block, &tmp) ){
		cl_error("failed to load block %d\n", hdr_block);
		return -1;
	}

	if ( (tmp.hdr.type != BLOCK_TYPE_FILE_HDR) ){
		cl_error("header block is wrong type 0x%X\n", tmp.hdr.type);
		return -1;
	}

	hdr = (cl_hdr_t*)tmp.data;


	switch(status){
	case CAFS_SNLIST_ITEM_STATUS_DISCARDING:
		cl_debug(DEBUG_LEVEL, "discarding file data serialno: %d hdr %d list %d\n",
				tmp.hdr.serialno,
				hdr_block,
				hdr->open.content_block);
		//Discard all the segments in the file
		if ( cl_filelist_init(cfg, &list, hdr->open.content_block) < 0 ){
			cl_error("failed to open list\n");
			return -1;
		}

		while( cl_list_getnext(cfg, &list, &file_item, &filelist_addr) == 0 ){

			cl_debug(DEBUG_LEVEL + 2, "discard this block 0x%X\n", file_item.status);
			if ( (file_item.status == CAFS_FILELIST_STATUS_CURRENT) || (file_item.status == CAFS_FILELIST_STATUS_OBSOLETE) ){
				//if the whole file is discarded, discard all blocks
				cl_debug(DEBUG_LEVEL + 2, "Discarding obsolete block %d\n", file_item.block);
				if ( cl_block_discard(cfg, file_item.block) < 0 ){
					cl_error("failed to discard block\n");
					return -1;
				}

				//mark it dirty in the list
				if ( cl_filelist_setstatus(cfg, CAFS_FILELIST_STATUS_DIRTY, filelist_addr) < 0 ){
					cl_error("failed to set status to dirty\n");
					return -1;
				}
			}
		}

	case CAFS_SNLIST_ITEM_STATUS_DISCARDING_HDR_LIST:
		//Discard the list of file segments
		if ( cl_list_discard(cfg, hdr->open.content_block) ){
			cl_error("failed to discard list\n");
			return -1;
		}

	case CAFS_SNLIST_ITEM_STATUS_DISCARDING_HDR:
		//Discard the header
		if ( cl_block_discard(cfg, hdr_block) < 0 ){
			cl_error("failed to discard header\n");
			return -1;
		}
	}


	cl_debug(DEBUG_LEVEL, "Marking file dirty at 0x%X\n", addr);
	//the old entry has been deleted
	if ( cl_serialno_setstatus(cfg, addr, CAFS_SNLIST_ITEM_STATUS_DIRTY) < 0 ){
		return -1;
	}

	return 0;
}

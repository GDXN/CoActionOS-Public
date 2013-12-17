/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */




#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "cafs_lite_local.h"
#include "cafs_lite_diag.h"
#include "cafs_lite_dir.h"
#include "cafs_lite_list.h"
#include "cafs_lite_filelist.h"
#include "cafs_lite_serialno.h"

void cl_diag_show(cl_diag_t * data){
	printf("Used Blocks: %d (SN%d DSN%d DL%d FL%d FD%d)\n",
			data->used_blocks,
			data->serialno_list_blocks,
			data->del_serialno_list_blocks,
			data->dir_list_blocks,
			data->file_list_blocks,
			data->file_data_blocks);
	printf("Open Blocks: %d\n", data->allocated_blocks);
	printf("Free Blocks: %d\n", data->free_blocks);
	printf("Dirty Blocks: %d\n", data->dirty_blocks);
	printf("Eraseable Blocks: %d\n", data->eraseable_blocks);
	printf("Total Blocks: %d\n", data->total_blocks);
}

void cl_diag_showfile(cl_diag_file_t * data){
	printf("Used File Blocks: %d\n", data->used_blocks);
	printf("Allocated File Blocks: %d\n", data->allocated_blocks);
	printf("Dirty File Blocks: %d\n", data->dirty_blocks);
	printf("Total File Blocks: %d\n", data->total_blocks);
}

int cl_diag_show_eraseable(const void * cfg){
	int i;
	int j;
	cl_block_data_t cl_block_data;
	serial_t serials[ cl_block_geteraseable(cfg) ];
	int eraseable;
	int total_eraseable;
	int uneraseable;

	total_eraseable = 0;
	for(i=0; i < cl_block_gettotal(cfg); i+= cl_block_geteraseable(cfg)){

		printf("Blocks %d to %d:  ", i, i+cl_block_geteraseable(cfg)-1);
		eraseable = 1;
		uneraseable = 0;
		memset(serials, 0, sizeof(serial_t) * cl_block_geteraseable(cfg) );
		for(j=0; j < cl_block_geteraseable(cfg); j++){

			if ( i+j != BLOCK_INVALID ){
				if ( cl_block_load(cfg, i+j, &cl_block_data) < 0 ){
					printf("failed to load block %d\n", i+j);
					return -1;
				}

				if (cl_block_data.hdr.status == BLOCK_STATUS_CLOSED) {
					eraseable = 0;
					uneraseable++;
					printf("W%d ", (int)cl_block_data.hdr.serialno);
				} else if (cl_block_data.hdr.status == BLOCK_STATUS_OPEN){
					eraseable = 0;
					uneraseable++;
					printf("%XA%d ", cl_block_data.hdr.type, (int)cl_block_data.hdr.serialno);
				} else if ( cl_block_data.hdr.status == BLOCK_STATUS_FREE ){
					eraseable = 0;
					printf("FXX ");
				} else if ( cl_block_data.hdr.status == BLOCK_STATUS_DISCARDING ){
					eraseable = 0;
					printf("D%d ", (int)cl_block_data.hdr.serialno);
				} else if ( cl_block_data.hdr.status == BLOCK_STATUS_DIRTY ){
					printf("X%d ", (int)cl_block_data.hdr.serialno);
				} else {
					printf("?0x%X ", cl_block_data.hdr.status);
				}
			}

		}
		if ( eraseable == 1 ){
			printf("Eraseable ");
			total_eraseable+=cl_block_geteraseable(cfg);
		}
		printf("In Use: %d", uneraseable);
		printf("\n");
	}

	printf("Total Eraseable: %d\n", total_eraseable);

	return 0;

}


int cl_diag_scan(const void * cfg){
	cl_block_data_t hdr_cl_block_data;
	cl_hdr_t * hdr;
	cl_list_t sn_list;
	cl_snlist_item_t item;
	int ret;
	int num_files;
	int num_dirs;
	int file_size;

	cl_debug(1, "initialize list\n");
	if ( cl_snlist_init(cfg, &sn_list, cl_serialno_getlistblock(cfg) ) < 0 ){
		return -1;
	}

	hdr = (cl_hdr_t*)(hdr_cl_block_data.data);

	file_size = 0;
	num_files = 0;
	num_dirs = 0;
	ret = 0;
	cl_debug(1, "go through serialno list\n");
	while( cl_snlist_getnext(cfg, &sn_list, &item) == 0 ){

		if ( item.status != CAFS_SNLIST_ITEM_STATUS_CLOSED ){
			//has been deleted
			continue;
		}

		if ( cl_block_load(cfg, item.block, &hdr_cl_block_data) < 0 ){
			printf("failed to load block %d\n", item.block);
			return -1;
		}

		if ( hdr_cl_block_data.hdr.type == BLOCK_TYPE_FILE_HDR ){
			num_files++;
			file_size += hdr->close.size;
			if ( cl_diag_scanfile(cfg, item.serialno) ){
				cl_error("Error with file %d\n", item.serialno);
				ret = -1;
			}
		}

	}

	printf("Total Bytes in files: %d\n", file_size);
	printf("Total Files: %d\n", num_files);
	printf("Total Dirs:  %d\n", num_dirs);

	return ret;
}


int cl_diag_scanfile(const void * cfg, serial_t serialno){
	int ret;
	cl_hdr_t * hdr;
	block_t hdr_block;
	cl_block_data_t hdr_cl_block_data;
	cl_block_data_t cl_block_data;
	cl_list_t file_data_list;
	cl_filelist_item_t list_item;

	if ( serialno == CL_SERIALNO_LIST ){
		return 0;
	}

	ret = 0;
	hdr = (cl_hdr_t*)hdr_cl_block_data.data;

	//first lookup the file's block number

	hdr_block = cl_serialno_get(cfg, serialno, CAFS_SNLIST_ITEM_STATUS_CLOSED, NULL);

	if ( cl_block_load(cfg, hdr_block, &hdr_cl_block_data) < 0 ){
		printf("failed to load block %d\n", hdr_block);
		return -1;
	}

	if( hdr_cl_block_data.hdr.type != BLOCK_TYPE_FILE_HDR ){
		printf("Error header is wrong type (0x%X != 0x%X)\n", hdr_cl_block_data.hdr.type, BLOCK_TYPE_FILE_HDR);
		ret = -1;
	}

	if ( hdr_cl_block_data.hdr.serialno != serialno ){
		printf("Error file header is wrong serialno (%d != %d)\n", (int)cl_block_data.hdr.serialno, (int)serialno );
		ret = -1;
	}

	if( cl_filelist_init(cfg, &file_data_list, hdr->open.content_block) < 0 ){
		printf("Failed to initialize file data list\n");
		return -1;
	}

	if( file_data_list.block_data.hdr.type != BLOCK_TYPE_FILE_LIST ){
		printf("Error file list is wrong type (0x%X != 0x%X)\n", hdr_cl_block_data.hdr.type, BLOCK_TYPE_FILE_HDR);
		ret = -1;
	}

	if ( file_data_list.block_data.hdr.serialno != serialno ){
		printf("Error file list is wrong serialno (%d != %d)\n", (int)cl_block_data.hdr.serialno, (int)serialno );
		ret = -1;
	}


	while( cl_filelist_getnext(cfg, &file_data_list, &list_item) == 0 ){

		if ( list_item.status == CAFS_FILELIST_STATUS_CURRENT ){ //if status is 0, the entry has been discarded

			if ( cl_block_load(cfg, list_item.block, &cl_block_data) < 0 ){
				printf("failed to load block %d\n", list_item.block);
				return -1;
			}

			if( cl_block_data.hdr.type != BLOCK_TYPE_FILE_DATA ){
				printf("Error on segment %d invalid block data type 0x%X\n", (int)list_item.segment, (unsigned int)cl_block_data.hdr.type);
				ret = -1;
			}

			if ( cl_block_data.hdr.serialno != serialno ){
				printf("Error file data is wrong serialno (%d != %d)\n", (int)cl_block_data.hdr.serialno, (int)serialno );
				ret = -1;
			}
		}
	}

	return ret;
}


/*! \details This function gets the diagnostic data structure from
 * the file system.
 */
int cl_diag_get(const void * cfg, cl_diag_t * dest){
	int i;
	int j;
	int addr;
	int size;
	int erase_size;
	int eraseable;
	cl_block_hdr_t hdr;
	int free_blocks;
	int dirty_blocks;
	int written_blocks;

	memset(dest, 0, sizeof(cl_diag_t));
	erase_size = cl_dev_geterasesize(cfg);
	size = cl_dev_getsize(cfg) - erase_size*2;

	for(j=0*BLOCK_SIZE; j < size; j+=erase_size){
		eraseable = 1;

		free_blocks = 0;
		dirty_blocks = 0;
		written_blocks = 0;

		for(i=0; i < erase_size; i+= BLOCK_SIZE){
			addr = j + i;
			if ( cl_dev_read(cfg, addr, &hdr, sizeof(cl_block_hdr_t)) != sizeof(cl_block_hdr_t) ){
				return -1;
			}
			dest->total_blocks++;
			switch(hdr.status){
			case BLOCK_STATUS_FREE:
				dest->free_blocks++;
				free_blocks++;
				if( (i+j) != 0 ){
					eraseable = 0;
				}
				break;
			case BLOCK_STATUS_OPEN:
				dest->allocated_blocks++;
				break;
			case BLOCK_STATUS_CLOSED:
				dest->used_blocks++;
				written_blocks++;
				eraseable = 0;
				if ( 1 ){
					switch(hdr.type & ~BLOCK_TYPE_LINKED_LIST_FLAG){
					case BLOCK_TYPE_SERIALNO_LIST:
						dest->serialno_list_blocks++;
						break;
					case BLOCK_TYPE_SERIALNO_DEL_LIST:
						dest->del_serialno_list_blocks++;
						break;
					case BLOCK_TYPE_DIR_LIST:
						dest->dir_list_blocks++;
						break;
					case BLOCK_TYPE_FILE_LIST:
						dest->file_list_blocks++;
						break;
					case BLOCK_TYPE_FILE_DATA:
						dest->file_data_blocks++;
						break;
					}
				}

				break;
			case BLOCK_STATUS_DIRTY:
				dest->dirty_blocks++;
				dirty_blocks++;
				break;
			default:
				//printf("Unknown block status at 0x%X (0x%X)\n", addr, hdr.status);
				break;
			}
		}

		//printf("%d to %d: dirty: %d free: %d written: %d\n", j/BLOCK_SIZE, (j+erase_size)/BLOCK_SIZE-1, dirty_blocks, free_blocks, written_blocks);

		if ( eraseable == 1 ){
			dest->eraseable_blocks+=blocks_per_eraseable;
		}
	}

	return 0;
}

int cl_diag_getfile(const void * cfg, const char * path, cl_diag_file_t * dest){
	int i;
	int j;
	int addr;
	int size;
	int erase_size;
	int blocks_per_eraseable;
	cl_block_hdr_t hdr;
	serial_t serialno;
	cl_dir_lookup_t entry;
	int err;

	err = cl_dir_exists(cfg, path, &entry, R_OK);
	if ( err < 0 ){
		printf("exists failed\n");
		return -1;
	}

	if ( err != CAFS_DIR_PATH_EXISTS ){
		return -1;
	}

	serialno = entry.serialno;

	if ( cl_diag_scanfile(cfg, entry.serialno) < 0 ){
		printf("file is bad\n");
		exit(5);
	}

	memset(dest, 0, sizeof(cl_diag_t));
	size = cl_dev_getsize(cfg);
	erase_size = cl_dev_geterasesize(cfg);
	blocks_per_eraseable = erase_size / BLOCK_SIZE;

	for(j=0; j < size; j+=erase_size){

		for(i=0; i < erase_size; i+= BLOCK_SIZE){
			addr = j + i;
			if ( cl_dev_read(cfg, addr, &hdr, sizeof(cl_block_hdr_t)) != sizeof(cl_block_hdr_t) ){
				return -1;
			}

			if ( hdr.serialno == serialno ){
				dest->total_blocks++;
				switch(hdr.status){
				case BLOCK_STATUS_OPEN:
					dest->allocated_blocks++;
					break;
				case BLOCK_STATUS_CLOSED:
					dest->used_blocks++;
					break;
				case BLOCK_STATUS_DIRTY:
					dest->dirty_blocks++;
					break;
				default:
					//printf("Invalid block status at 0x%X (0x%X)\n", addr, hdr.status);
					break;
				}
			}
		}
	}

	return 0;
}

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#ifndef CAFS_LOCAL_H_
#define CAFS_LOCAL_H_

#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <stdint.h>

#ifdef __SIM__
#include "sim_device.h"
#else
#include "device.h"
#include "hwpl/core.h"
#endif
#include "cafs_lite_dev.h"

typedef uint32_t serial_t;
typedef uint16_t block_t;

#define SCRATCH_ENTRIES 30
#define BLOCK_SIZE 128
#define BLOCK_GET_ADDRESS(x) ( x * BLOCK_SIZE )

#define BLOCK_INVALID (0xFFFF)
#define SERIALNO_INVALID (0xFFFFFFFF)
#define SEGMENT_INVALID (0xFFFF)

#define FIRST_BLOCK 1

#define CL_SERIALNO_LIST (0)


enum {
	BLOCK_STATUS_FREE = 0xFF,
	BLOCK_STATUS_OPEN = 0xFE,
	BLOCK_STATUS_CLOSED = 0xFC,
	BLOCK_STATUS_DISCARDING = 0xF8,
	BLOCK_STATUS_DIRTY = 0x00
};

#define BLOCK_TYPE_LIST_FLAG 0x80
#define BLOCK_TYPE_LINKED_LIST_FLAG 0x40
#define BLOCK_TYPE_LIST_MASK (BLOCK_TYPE_LIST_FLAG|BLOCK_TYPE_LINKED_LIST_FLAG)


enum {
	BLOCK_TYPE_SERIALNO_LIST = 0x00 | BLOCK_TYPE_LIST_FLAG,
	BLOCK_TYPE_SERIALNO_DEL_LIST = 0x01 | BLOCK_TYPE_LIST_FLAG,
	BLOCK_TYPE_DIR_HDR = 0x02,
	BLOCK_TYPE_DIR_LIST = 0x03 | BLOCK_TYPE_LIST_FLAG,
	BLOCK_TYPE_FILE_HDR = 0x04,
	BLOCK_TYPE_FILE_LIST = 0x05 | BLOCK_TYPE_LIST_FLAG,
	BLOCK_TYPE_FILE_DATA = 0x06,
	BLOCK_TYPE_LINK_HDR = 0x07,
	BLOCK_TYPE_SYMLINK_HDR = 0x08
};

typedef struct HWPL_PACK {
	serial_t serialno;
	uint8_t type;
	uint8_t status;
} cl_block_hdr_t;

#define BLOCK_HEADER_SIZE (sizeof(cl_block_hdr_t))
#define BLOCK_DATA_SIZE (BLOCK_SIZE - BLOCK_HEADER_SIZE)

//This defines the physical layer
typedef struct HWPL_PACK {
	cl_block_hdr_t hdr;
	char data[BLOCK_DATA_SIZE];
} cl_block_data_t;


typedef struct {
	char name[NAME_MAX+1];
	block_t content_block;
} cl_hdr_open_t;

typedef struct {
	int32_t size;
} cl_hdr_close_t;

typedef struct HWPL_PACK {
	cl_hdr_open_t open;
	cl_hdr_close_t close;
} cl_hdr_t;


typedef struct HWPL_PACK {
	block_t hdr_block /*! the block for the file header */;
	block_t segment_list_block /*! The block containing the file's list of segments */;
	int serialno_addr /*! The address of the serial number entry */;
	device_transfer_t * op /*! a pointer to the current operation */;
	int bytes_left /*! the number of bytes read/written so far */;
	int size /*! The size of the file */;
	uint8_t amode /*! The open mode */;
	uint16_t segment /*! The segment of the file */;
	uint32_t mtime /*! The time of the last modification */;
	cl_block_data_t segment_data; /*! The RAM buffer for the segment */;
} cl_handle_t;

#ifdef __SIM__
#define CL_DEBUG 3
#define CL_ERROR
#define CL_TEST

#define HWPL_ALWAYS_INLINE

#define hwpl_debug(...) printf(__VA_ARGS__)

#else
#include "hwpl/debug.h"
#endif


#if (CL_DEBUG > 0)
#include <stdio.h>
#define cl_debug(LEVEL, ...) do { if ( LEVEL <= CL_DEBUG){ printf("%s:%d: ", __func__, __LINE__); printf(__VA_ARGS__); }} while(0)
#else
#define cl_debug(LEVEL, ...)
#endif

#ifdef CL_ERROR
#define cl_error(...) do { printf("ERROR:%d:%s: ", __LINE__, __func__); printf(__VA_ARGS__); } while(0)
#else
#define cl_error(...)
#endif

#include "cafs_lite_tp.h"


#endif /* CAFS_LOCAL_H_ */

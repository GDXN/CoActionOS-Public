/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef APPFS_LOCAL_H_
#define APPFS_LOCAL_H_

#include "config.h"
#include <stdbool.h>
#include "sysfs.h"
#include "appfs.h"
#include "dev/mem.h"
#include "hwpl/debug.h"
#include "link_protocol.h"


typedef struct {
	uint32_t code_start /*! the new value */;
	uint32_t code_size;
	uint32_t data_start /*! the new data start */;
	uint32_t data_size;
	uint32_t rewrite_mask;
	uint32_t kernel_symbols_total;
} appfs_util_handle_t;

#define APPFS_MEMPAGETYPE_FREE 0
#define APPFS_MEMPAGETYPE_SYS 1
#define APPFS_MEMPAGETYPE_USER 2
#define APPFS_MEMPAGETYPE_MASK 0x03

typedef struct {
	int beg_addr /*! the is the address of the beginning of the file */;
	int size /*! the current size of the file */;
	int page /*! the start page of the open file */;
	int type /*! the type of memory (flash or RAM) */;
} appfs_reg_handle_t;


typedef struct {
	bool is_install /*! boolean for the .install file */;
	union {
		appfs_util_handle_t install;
		appfs_reg_handle_t reg;
	} type;
} appfs_handle_t;

typedef struct {
	int ret;
	const device_t * dev;
	mem_pageinfo_t pageinfo;
	appfs_file_t fileinfo;
} priv_load_fileinfo_t;


//page utilities
int appfs_util_getpagetype(appfs_hdr_t * info, int page, int type);
int appfs_util_getflashpagetype(appfs_hdr_t * info);

//file utilities
int appfs_util_lookupname(const void * cfg, const char * path, priv_load_fileinfo_t * args, int type, int * size);

//memory access utilities
void appfs_util_priv_closeinstall(void * args);
int appfs_util_priv_writeinstall(const device_t * dev, appfs_handle_t * h, appfs_installattr_t * attr);
int appfs_util_priv_create(const device_t * dev, appfs_handle_t * h, appfs_installattr_t * attr);
int appfs_util_getfileinfo(priv_load_fileinfo_t * info, const device_t * dev, int page, int type, int * size);
int appfs_util_erasepages(const device_t * dev, int start_page, int end_page);
int appfs_util_getpageinfo(const device_t * dev, mem_pageinfo_t * pageinfo);

//ram access
extern const int caoslib_system_memory_size;
#define APPFS_RAM_PAGES ((DEVICE_MAX_RAM-DEVICE_SYS_RAM)/DEVICE_RAM_PAGE_SIZE)
#define APPFS_RAM_USAGE_BITS_SIZE (APPFS_RAM_PAGES*2) //size of usage in bits (2 bits per page)
#define APPFS_RAM_USAGE_WORDS (APPFS_RAM_USAGE_BITS_SIZE/16)
#define APPFS_RAM_USAGE_BYTES (APPFS_RAM_USAGE_WORDS*4)

int appfs_ram_getusage(int page);
int appfs_ram_setusage(int page, int size, int type);
void appfs_ram_priv_saveusage(void * args);
void appfs_ram_setrange(uint32_t * buf, int page, int size, int usage);
extern uint32_t appfs_ram_usagetable[APPFS_RAM_USAGE_WORDS];






#endif /* APPFS_LOCAL_H_ */

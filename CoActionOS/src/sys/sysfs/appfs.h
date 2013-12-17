/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef APPFS_H_
#define APPFS_H_

#include <stdbool.h>

#define APPFS_EXEC_OPTIONS_FLASH (1<<0)
#define APPFS_EXEC_OPTIONS_RAM (0<<0)
#define APPFS_EXEC_OPTIONS_STARTUP (1<<1) //if set executes on boot

typedef struct {
	void (*startup)(int argc, char * const argv[]) /*! The startup routine */;
	void * code_start /*! The start of memory */;
	uint32_t code_size /*! The size of the code section */;
	void * ram_start /*! The start of memory */;
	int32_t ram_size /*! The size of  memory (total RAM for process)*/;
	int32_t data_size /*! Number of initialized bytes */;
	uint32_t options /*! must be valid to execute the file */;
	uint32_t signature /*! The kernel signature used for the build */;
} appfs_exec_t; //32 bytes

typedef struct {
	char name[NAME_MAX] /*! The name of the process or file (must be specially written to the binary) */;
	//uid_t uid;
	//gid_t gid;
	mode_t mode;
} appfs_hdr_t;  //30 bytes

typedef struct {
	appfs_hdr_t hdr;
	appfs_exec_t exec;
} appfs_file_t;

bool appfs_util_isexecutable(const appfs_file_t * info);


#endif /* APPFS_H_ */

/*
 * main.c
 *
 *  Created on: Feb 3, 2012
 *      Author: tgil
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "cafs_lite_local.h"
#include "cafs_lite.h"
#include "cafs_lite_block.h"
#include "cafs_lite_dir.h"
#include "cafs_lite_file.h"
#include "cafs_lite_serialno.h"
#include "cafs_lite_diag.h"
#include "cafs_lite_tp.h"
#include "tests.h"

volatile int finished;
volatile int failed;

#define FS_ERROR_RET 1
#define TP_RET 2
#define SUCCESS_RET 0

//just for testing
extern void show_mem(int addr, int nbyte);
extern int save_filesystem(const char * path);
extern int load_filesystem(const char * path);


static void fail_routine(void){
	failed = 1;
	save_filesystem("myfilesystem.fs");
	exit(TP_RET);
}


void diags(void){
	const void * cfg;
	cl_diag_t diag;
	cfg = NULL;
	//cl_diag_show_eraseable(cfg);
	cl_diag_get(cfg, &diag);
	cl_diag_show(&diag);

	if ( finished == 1 ){
		printf("Filesystem saved:  no errors\n");
	} else if ( failed == 1 ){
		printf("Filesystem saved:  test point failure\n");
	} else {
		printf("Failed to complete\n");
	}
}


#define BUFFER_SIZE 16
#define NUM_TESTS 5

open_file_t open_file;
const cafs_lite_cfg_t ccfg = {
		.open_file = &open_file,
		.dev_cfg = NULL,
		.devfs = (void*)1,
		.name = "disk"
};

int main(void) {
	cl_diag_t diag;
	const void * cfg = NULL;
	pid_t child;
	int ret;

	cfg = &ccfg;

	do {
		child = fork();
		if ( child == 0 ){

			cl_dev_open(cfg);
			finished = 0;
			failed = 0;

			atexit(diags);

			cl_tp_setfailroutine(fail_routine);

			if ( load_filesystem("myfilesystem.fs") < 0 ){
				if ( cafs_lite_mkfs(cfg) < 0 ){
					printf("failed to mkfs\n");
					exit(FS_ERROR_RET);
				}
			}

			if ( cafs_lite_init(cfg) < 0 ){
				exit(FS_ERROR_RET);
			}


			if ( cl_diag_scan(cfg) < 0 ){
				printf("Errors with filesystem\n");
				exit(FS_ERROR_RET);
			}


			if ( 1 ){
				if ( test_run(true, false) < 0 ){
					exit(FS_ERROR_RET);
				}
			}

			cl_diag_get(cfg, &diag);
			cl_diag_show(&diag);

			if ( cl_diag_scan(cfg) < 0 ){
				printf("Errors with filesystem\n");
				exit(FS_ERROR_RET);
			}

			if ( save_filesystem("myfilesystem.fs") < 0 ){
				printf("failed to save filesystem\n");
			}

			finished = 1;

			cl_tp_createreport("failreport.txt");

			exit(SUCCESS_RET);
		}

		wait(&ret);

		ret = WEXITSTATUS(ret);

		//exit(0);

	} while( (ret == SUCCESS_RET) || (ret == TP_RET) );

	return 0;
}

void * test_open(const char * path, int flags, int mode){
	void * handle;
	if ( cafs_lite_open(NULL, &handle, path, flags, mode) < 0 ){
		return NULL;
	}
	return handle;
}

int test_close(void * handle){
	int ret;
	ret = cafs_lite_close(NULL, &handle);
	return ret;
}

int test_read(void * handle, int loc, void * buf, int nbyte){
	return cafs_lite_read(NULL, handle, 0, loc, buf, nbyte);
}

int test_write(void * handle, int loc, const void * buf, int nbyte){
	return cafs_lite_write(NULL, handle, 0, loc, buf, nbyte);
}

void * test_opendir(const char * path){
	void * handle;
	if ( cafs_lite_opendir(NULL, &handle, path) < 0 ){
		return NULL;
	}
	return handle;
}

int test_readdir_r(void * handle, int loc, struct dirent * entry){
	return cafs_lite_readdir_r(NULL, handle, loc, entry);
}

int test_closedir(void * handle){
	return cafs_lite_closedir(NULL, handle);
}
int test_fstat(void * handle, struct stat * stat){
	return cafs_lite_fstat(NULL, handle, stat);
}

int test_remove(const char * path){
	return cafs_lite_remove(NULL, path);
}

int test_unlink(const char * path){
	return cafs_lite_unlink(NULL, path);
}

int test_stat(const char * path, struct stat * stat){
	return cafs_lite_stat(NULL, path, stat);
}

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "crt_common.h"

extern uint32_t _signature;

void crt(int argc, char * const argv[]);

extern struct _reent global_impure_data __attribute__((section(".global_reent")));
struct _reent global_impure_data = _REENT_INIT(global_impure_data);

const appfs_file_t startup_data __attribute__ ((section(".startup"))) = {
		.hdr.name = "",
		//.hdr.uid = 0,
		//.hdr.gid = 0,
		.hdr.mode = 0777,
		.exec.startup = crt,
		.exec.code_start = &_text,
		.exec.code_size = (uint32_t)&_text_size,
		.exec.ram_start = &_data,
		//.exec.ram_size = (int)&_app_ram_size,
		.exec.ram_size = 2048,
		.exec.data_size = (int)&_data_size,
		.exec.options = APPFS_EXEC_OPTIONS_FLASH,
		.exec.signature = (uint32_t)&_signature
};


void crt(int argc, char * const argv[]){
	int ret;

	//Copy the heap to RAM -- since the heap copy is in no man's land this needs to be privileged
	//hwpl_core_privcall(priv_load_data, NULL);
	crt_load_data(&global_impure_data, startup_data.exec.code_start, startup_data.exec.code_size, startup_data.exec.data_size);

	if ( crt_common(argc, argv, &ret, startup_data.hdr.name) != 0 ){
		ret = -1;
	}

	exit(ret);
}


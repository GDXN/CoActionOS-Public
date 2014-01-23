/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef CRT_COMMON_H_
#define CRT_COMMON_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>

#include "../sys/sysfs/appfs.h"


extern uint32_t _text;
extern uint32_t _text_size;
extern uint32_t _data;
extern uint32_t _data_size;
extern uint32_t _bss;
extern uint32_t _ebss;

extern uint32_t _app_ram_size;

int crt_common(int argc, char * const argv[], int * ret, const char * name);
void crt_exit(int exit_code);

void crt_load_data(void * global_impure, void * code_start, int code_size, int data_size);
char ** const crt_import_argv(int argc, char * const argv[]);

#endif /* CRT_COMMON_H_ */

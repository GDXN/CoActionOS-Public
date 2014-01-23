/*
 * crt_sys.c
 *
 *  Created on: Jan 23, 2014
 *      Author: tgil
 */


#include <stdlib.h>
#include <string.h>

#include "hwpl/core.h"

typedef struct {
	char ** argv;
	char ** new_argv;
	int total;
	int argc;
	char * arg_buffer;
} priv_import_argv_t;

typedef struct {

} priv_load_;

static void priv_get_total_len(void * args);
static void priv_import_args(void * args);

typedef struct {
	void * global_impure;
	void * data;
	int size;
} priv_load_data_t;

void priv_load_data(void * args){
	//don't use &_etext -- it has a bug when the binary is 32768 or 65536
	priv_load_data_t * p = (priv_load_data_t*)args;
	memcpy(p->global_impure,
			p->data,
			p->size);
}

void priv_get_total_len(void * args){
	int i;
	priv_import_argv_t * argsp = (priv_import_argv_t*)args;
	argsp->total = 0;
	for(i=0; i < argsp->argc; i++){
		argsp->total += strlen(argsp->argv[i]);
	}
}

void priv_import_args(void * args){
	priv_import_argv_t * argsp = (priv_import_argv_t*)args;
	char * arg_bufp;
	int i;
	int len;
	arg_bufp = argsp->arg_buffer;
	for(i=0; i < argsp->argc; i++){
		len = strlen(argsp->argv[i]) + 1; //The string length plus the terminator
		memcpy(arg_bufp, argsp->argv[i], len); //copy the length plus the
		argsp->new_argv[i] = arg_bufp;
		arg_bufp += len;
	}

	argsp->new_argv[i] = NULL; //add the NULL terminator

}

void crt_load_data(void * global_impure, void * code_start, int code_size, int data_size){
	priv_load_data_t args;

	//If the caller doesn't have the ability to write this memory location, a fault will occur
	memset(global_impure, 0, data_size);

	//Now copy the data in priv mode
	args.global_impure = global_impure;
	args.data = code_start + code_size;
	args.size = data_size;
	hwpl_core_privcall(priv_load_data, &args);
}

char ** const crt_import_argv(int argc, char * const argv[]){
	priv_import_argv_t args;

	//get the total length of the argv
	args.argv = (char**)argv;
	args.argc = argc;
	hwpl_core_privcall(priv_get_total_len, &args);

	//Allocate memory for the new args
	args.arg_buffer = malloc(args.total + args.argc);
	if ( args.arg_buffer == NULL ){
		return NULL;
	}

	//Allocate memory for the pointers to the args
	args.new_argv = malloc( (args.argc+1) * sizeof(char * const));
	if ( args.new_argv == NULL ){
		free(args.arg_buffer);
		return NULL;
	}

	//Copy the contents of the argv to the local process memory
	hwpl_core_privcall(priv_import_args, &args);

	return (char ** const)args.new_argv;
}

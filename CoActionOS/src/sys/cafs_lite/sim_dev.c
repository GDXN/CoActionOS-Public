/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#include "cafs_lite_dev.h"


#define ERASE_SIZE 4096
#define MEM_SIZE (1*1024*1024)
char mem[MEM_SIZE];

static uint8_t euid = 0;
static uint8_t egid = 0;

static int dev_list_block;
static int dev_serialno;


int cl_dev_getlist_block(const void * cfg){
	return dev_list_block;
}

void cl_dev_setlist_block(const void * cfg, int list_block){
	dev_list_block = list_block;
}

int cl_dev_getserialno(const void * cfg){
	return dev_serialno;
}

void cl_dev_setserialno(const void * cfg, int serialno){
	dev_serialno = serialno;
}

void cl_dev_setdelay_mutex(pthread_mutex_t * mutex){
	//hwpl_core_privcall(set_delay_mutex, mutex);
}

void cl_sys_seteuid(int uid){
	euid = uid;
}

void cafs_sys_setegid(int gid){
	egid = gid;
}

int cl_sys_geteuid(void){
	return euid;
}

int cl_sys_getegid(void){
	return egid;
}

int save_filesystem(const char * path){
	FILE * f;
	f = fopen(path, "wb");
	if ( f == NULL ){
		return -1;
	}

	if ( fwrite(mem, MEM_SIZE, 1, f) != 1 ){
		fclose(f);
		return -1;
	}

	fclose(f);
	return 0;
}

int load_filesystem(const char * path){
	FILE * f;
	f = fopen(path, "rb");
	if ( f == NULL ){
		return -1;
	}

	if ( fread(mem, MEM_SIZE, 1, f) != 1 ){
		fclose(f);
		memset(mem, 0xFF, MEM_SIZE);
		return -1;
	}

	fclose(f);
	return 0;
}

int cl_dev_open(const void * cfg){
	return 0;
}

int cl_dev_write(const void * cfg, int loc, const void * buf, int nbyte){
	int i;
	unsigned char dest;
	unsigned char src;
	const unsigned char * chbuf;

	if ( loc >= MEM_SIZE ){
		printf("Loc does not fit in memory\n");
		return -1;
	}

	if ( loc + nbyte > MEM_SIZE ){
		printf("truncate the size (%d + %d > %d)\n", loc, nbyte, MEM_SIZE);
		nbyte = MEM_SIZE - loc;
	}

	//make sure the memory is writeable
	chbuf = (const unsigned char *)buf;
	for(i=0; i < nbyte; i++){

		dest = mem[loc + i];
		src = chbuf[i];

		if ( (dest | src) != dest ){
			printf("Bad write 0x%X cannot be written over 0x%X at 0x%X\n", src, dest, loc + i);
			exit(1);
		}

		mem[loc + i] = src;
		//printf("0x%X ", src);
	}

	//printf("\n");
	//printf("wrote %d bytes to 0x%X\n", nbyte, loc);

	return nbyte;
}

int cl_dev_erase(const void * cfg){
	//printf("DEVICE ERASED!!!!!!!\n");
	memset(mem, 0xFF, MEM_SIZE);
	return 0;
}

int cl_dev_erasesection(const void * cfg, int loc){
	int addr;
	addr = loc & ~(ERASE_SIZE-1);
	memset(&(mem[addr]), 0xFF, ERASE_SIZE);
	//printf("-------ERASING addr from 0x%X to 0x%X\n", addr, addr + ERASE_SIZE-1);
	return 0;
}


int cl_dev_read(const void * cfg, int loc, void * buf, int nbyte){
	if ( loc >= MEM_SIZE ){
		return -1;
	}

	if ( loc + nbyte > MEM_SIZE ){
		nbyte = MEM_SIZE - loc;
	}

	memcpy(buf, &(mem[loc]), nbyte);
	//printf("read %d bytes from 0x%X\n", nbyte, loc);
	return nbyte;
}

int cl_dev_close(const void * cfg){
	return 0;
}

int cl_dev_getsize(const void * cfg){
	return MEM_SIZE;
}

int cl_dev_geterasesize(const void * cfg){
	return ERASE_SIZE;
}

void show_mem(int addr, int nbyte){
	int i;
	for(i = 0; i < nbyte; i++){
		printf("0x%X = 0x%X %d\n", addr + i, mem[addr+i], mem[addr+i]);
	}
}

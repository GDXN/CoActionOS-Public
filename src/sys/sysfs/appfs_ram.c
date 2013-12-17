/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl/mpu.h"
#include "appfs_local.h"

uint32_t appfs_ram_usagetable[APPFS_RAM_USAGE_WORDS] HWPL_SYS_MEM;

void appfs_ram_priv_saveusage(void * args){
	memcpy(appfs_ram_usagetable, args, APPFS_RAM_USAGE_BYTES);
}

//this is a privileged call
static void set_ram_usage(uint32_t * buf, int page, int usage){
	int block;
	int shift;
	block = page / 16;
	shift = (page % 16) * 2;
	buf[block] &= ~(APPFS_MEMPAGETYPE_MASK << (shift)); //clear the bits
	buf[block] |= ((usage & APPFS_MEMPAGETYPE_MASK) << (shift));  //set the bits
}

void appfs_ram_setrange(uint32_t * buf, int page, int size, int usage){
	int i;
	int pages;
	size = mpu_getnextpowerof2(size);
	pages = (size + DEVICE_RAM_PAGE_SIZE - 1) / DEVICE_RAM_PAGE_SIZE;
	for(i=page; i < (pages+page); i++){
		set_ram_usage(buf, i, usage);
	}
}

int appfs_ram_getusage(int page){
	int block;
	int shift;
	if ( (uint32_t)page < APPFS_RAM_PAGES){
		block = page / 16;
		shift = (page % 16) * 2;
		return ( (appfs_ram_usagetable[block] >> (shift)) & APPFS_MEMPAGETYPE_MASK );
	} else {
		return -1;
	}
}




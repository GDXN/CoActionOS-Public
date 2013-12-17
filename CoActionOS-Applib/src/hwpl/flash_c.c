/*
 * flash.c
 *
 *  Created on: Oct 18, 2012
 *      Author: tgil
 */
#include <stdlib.h>
#include <hwpl/flash.h>

#include "flash.h"
#include "util.h"

int flash_open(port_t port){
	return hwpl_flash_open((const device_cfg_t*)&(port));
}
int flash_close(port_t port){
	return hwpl_flash_close((const device_cfg_t*)&(port));
}
int flash_getattr(port_t port, flash_attr_t * attr){
	return hwpl_flash_getattr(port, attr);
}
int flash_setattr(port_t port, flash_attr_t * attr){
	return hwpl_flash_setattr(port, attr);
}
int flash_eraseaddr(port_t port, uint32_t addr){
	return hwpl_flash_eraseaddr(port, (void*)addr);
}
int flash_erasepage(port_t port, int page){
	return hwpl_flash_erasepage(port, (void*)page);
}
int flash_getpage(port_t port, uint32_t addr){
	return hwpl_flash_getpage(port, (void*)addr);
}
int flash_getsize(port_t port){
	return hwpl_flash_getsize(port, NULL);
}
int flash_getpageinfo(port_t port, flash_pageinfo_t * info){
	return hwpl_flash_getpageinfo(port, info);
}

int flash_writepage(port_t port, flash_writepage_t * wattr){
	return hwpl_flash_writepage(port, wattr);
}

int flash_read(port_t port, int loc, void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_flash_read, loc, buf, nbyte, true);
}
int flash_write(port_t port, int loc, const void * buf, int nbyte){
	return applib_util_transfer((const device_cfg_t*)&port, hwpl_flash_write, loc, buf, nbyte, false);
}

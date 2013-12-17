/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "link_flags.h"

#include <stdbool.h>
#include <stdarg.h>

static int reset_device(link_phy_t handle, bool invoke_bootloader);

int link_isbootloader(link_phy_t handle){
	bootloader_attr_t attr;

	if( link_ioctl(handle, LINK_BOOTLOADER_FILDES, I_BOOTLOADER_GETATTR, &attr) < 0 ){
		//If this fails, no bootloader
		link_error("Failed to respond to ioctl\n");
		return 0;
	}

	link_debug(3, "Bootloader Version is 0x%X\n", attr.version);

	//If the above succeeds, the bootloader is present
	return 1;
}

int link_reset(link_phy_t handle){
	link_op_t op;
	link_debug(3, "try to reset--check bootloader\n");
	if( link_isbootloader(handle) ){
		//execute the request
		op.ioctl.cmd = LINK_CMD_IOCTL;
		op.ioctl.fildes = LINK_BOOTLOADER_FILDES;
		op.ioctl.request = I_BOOTLOADER_RESET;
		op.ioctl.arg = 0;
		link_protocol_masterwrite(handle, &op, sizeof(link_ioctl_t));
		link_phy_close(handle);
	} else {
		link_debug(3, "reset device with /dev/core\n");
		return reset_device(handle, false);
	}
	return 0;
}

int reset_device(link_phy_t handle, bool invoke_bootloader){
	//use "/dev/core" to reset
	int fd;
	link_op_t op;

	fd = link_open(handle, "/dev/core", LINK_O_RDWR);
	if ( fd < 0 ){
		return -1;
	}

	op.ioctl.cmd = LINK_CMD_IOCTL;
	op.ioctl.fildes = fd;
	op.ioctl.arg = (size_t)NULL;

	if( invoke_bootloader == false ){
		link_debug(3, "Try to reset\n");
		op.ioctl.request = I_CORE_RESET;

	} else {
		link_debug(3, "Try to invoke bootloader\n");
		op.ioctl.request = I_CORE_INVOKEBOOTLOADER;
	}

	link_protocol_masterwrite(handle, &op, sizeof(link_ioctl_t));
	link_phy_close(handle);
	return 0;
}

int link_resetbootloader(link_phy_t handle){
	return reset_device(handle, true);
}

int link_eraseflash(link_phy_t handle){
	if( link_ioctl_delay(handle, LINK_BOOTLOADER_FILDES, I_BOOTLOADER_ERASE, NULL, 0, 500) < 0 ){
		return -1;
	}

	link_phy_wait(500);
	link_phy_wait(500);
	return 0;
}

int link_readflash(link_phy_t handle, int addr, void * buf, int nbyte){
	link_op_t op;
	link_reply_t reply;
	int err;
	int len;

	op.read.cmd = LINK_CMD_READ;
	op.read.addr = addr;
	op.read.nbyte = nbyte;


	link_debug(4, "write read flash op\n");
	err = link_protocol_masterwrite(handle, &op, sizeof(link_read_t));
	if ( err < 0 ){
		return err;
	}

	link_debug(4, "read flash data\n");
	len = link_protocol_masterread(handle, buf, nbyte);
	if ( len < 0 ){
		return LINK_TRANSFER_ERR;
	}

	link_debug(4, "read reply\n");
	err = link_protocol_masterread(handle, &reply, sizeof(reply));
	if ( err < 0 ){
		return err;
	}

	if ( reply.err < 0 ){
		link_errno = reply.err_number;
	}

	link_debug(4, "Read %d bytes from device\n", reply.err);

	return reply.err;
}

int link_writeflash(link_phy_t handle, int addr, const void * buf, int nbyte){
	bootloader_writepage_t wattr;
	int page_size;
	int bytes_written;
	int err;

	bytes_written = 0;
	wattr.addr = addr;
	page_size = BOOTLOADER_WRITEPAGESIZE;
	if( page_size > nbyte ){
		page_size = nbyte;
	}
	wattr.nbyte = page_size;

	link_debug(4, "Page size is %d (%d)\n", page_size, nbyte);

	do {
		memset(wattr.buf, 0xFF, BOOTLOADER_WRITEPAGESIZE);
		memcpy(wattr.buf, buf, page_size);


		err = link_ioctl_delay(handle, LINK_BOOTLOADER_FILDES, I_BOOTLOADER_WRITEPAGE, &wattr, 0, 0);
		if( err < 0 ){
			return err;
		}

		wattr.addr += page_size;
		buf += page_size;
		bytes_written += page_size;

	} while(bytes_written < nbyte);
	return nbyte;
}

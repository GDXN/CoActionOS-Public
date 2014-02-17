/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup LINK
 * @{
 *
 */

#include "config.h"

#include <errno.h>
#include <stdbool.h>
#include <sys/fcntl.h>
#include <hwpl/core.h>
#include <applib/flash.h>
#include <applib/debug.h>
#include <applib/core.h>
#include <dev/bootloader.h>

#define FLASH_PORT 0

#include <hwpl/debug.h>

#include "link.h"
#include "link_local.h"

static bool is_erased = false;

#define LINK_DEBUG 0
#define link_debug(...) do { if ( LINK_DEBUG == 1 ){ hwpl_debug("%s:", __func__); hwpl_debug(__VA_ARGS__); } } while(0)

void gled_on(void);
void gled_off(void);
void gled_open(void);
void gled_close(void);

static int read_flash(int loc, int nbyte);
static int read_flash_callback(void * context, void * buf, int nbyte);

typedef struct {
	int err;
	int nbyte;
	//char data[LINK_PACKET_DATA_SIZE];
	link_op_t op;
	link_reply_t reply;
} link_data_t;


static link_phy_t phy_handle;
static void link_cmd_none(link_data_t * args);
static void link_cmd_readserialno(link_data_t * args);
static void link_cmd_ioctl(link_data_t * args);
static void link_cmd_read(link_data_t * args);

static void link_cmd_reset_bootloader(link_data_t * args);
static void erase_flash(void);
static void link_cmd_reset(link_data_t * args);


void (* const link_cmd_func_table[LINK_BOOTLOADER_CMD_TOTAL])(link_data_t*) = {
		link_cmd_none,
		link_cmd_readserialno,
		link_cmd_ioctl,
		link_cmd_read
};



int link_init(void){
	if( (phy_handle = link_phy_open(NULL, 0)) == LINK_PHY_ERROR){
		return -1;
	}
	return 0;
}

void * link_update(void * arg){
	int err;

	link_data_t data;
	data.op.cmd = 0;
	err = 0;

	dstr("Enter update loop\n");
	while(1){
		//Wait for data to arrive on the USB
		while( 1 ){

			if ( (err = link_protocol_slaveread(phy_handle, &data.op, sizeof(link_op_t), NULL, NULL)) < 0 ){
				continue;
				core_reset(0);
			}

			if( err > 0 ){
				break;
			}

		}

		dstr("EXEC CMD: "); dint(data.op.cmd); dstr("\n");
		if ( data.op.cmd < LINK_BOOTLOADER_CMD_TOTAL ){
			link_cmd_func_table[data.op.cmd](&data);
		} else {
			data.reply.err = -1;
			data.reply.err_number = EINVAL;
		}

		//send the reply
		if( data.op.cmd != 0 ){
			link_protocol_slavewrite(phy_handle, &data.reply, sizeof(data.reply), NULL, NULL);
			data.op.cmd = 0;
		}

	}
	return NULL;
}

void link_cmd_none(link_data_t * args){
	return;
}

void link_cmd_readserialno(link_data_t * args){
	char serialno[LINK_PACKET_DATA_SIZE];
	uint32_t tmp[4];
	int i, j;
	char * p = serialno;
	memset(serialno, 0, LINK_PACKET_DATA_SIZE);


	_hwpl_core_getserialno(tmp);
	for(j=3; j >= 0; j--){
		for(i=0; i < 8; i++){
			*p++ = htoc((tmp[j] >> 28) & 0x0F);
			tmp[j] <<= 4;
		}
	}

	args->reply.err = strlen(serialno);
	args->reply.err_number = 0;

	if( link_protocol_slavewrite(phy_handle, &args->reply, sizeof(args->reply), NULL, NULL) < 0 ){
		return;
	}

	if( link_protocol_slavewrite(phy_handle, serialno, args->reply.err, NULL, NULL) < 0 ){
		return;
	}

	args->op.cmd = 0; //reply was already sent
}

void link_cmd_ioctl(link_data_t * args){
	int err;
	int size;
	size = _IOCTL_SIZE(args->op.ioctl.request);
	bootloader_attr_t attr;
	bootloader_writepage_t wattr;

	dstr("IOCTL REQ: "); dhex(args->op.ioctl.request); dstr("\n");

	switch(args->op.ioctl.request){
	case I_BOOTLOADER_ERASE:
		//the erase takes awhile -- so send the reply a little early
		link_protocol_slavewrite(phy_handle, &args->reply, sizeof(args->reply), NULL, NULL);
		args->op.cmd = 0;

		erase_flash();
		is_erased = true;
		return;
	case I_BOOTLOADER_GETATTR:
		//write data to io_buf
		attr.version = BCDVERSION;
		_hwpl_core_getserialno(attr.serialno);

		err = link_protocol_slavewrite(phy_handle, &attr, size, NULL, NULL);
		if ( err == -1 ){
			args->op.cmd = 0;
			args->reply.err = -1;
		}

		attr.startaddr = PROGRAM_START_ADDR;
		break;
	case I_BOOTLOADER_RESET:
		if( args->op.ioctl.arg == 0 ){
			link_cmd_reset(args);
		} else {
			link_cmd_reset_bootloader(args);
		}
		break;
	case I_BOOTLOADER_WRITEPAGE:

#ifdef __SECURE
		//decrypt incoming data

#endif

		err = link_protocol_slaveread(phy_handle, &wattr, size, NULL, NULL);
		if( err < 0 ){
			dstr("failed to read data\n");
			break;
		}

		args->reply.err = flash_writepage(FLASH_PORT, (flash_writepage_t*)&wattr);
		if( args->reply.err < 0 ){
			dstr("Failed to write flash\n");
		}

		break;
	default:
		args->reply.err_number = EINVAL;
		args->reply.err = -1;
		break;
	}

	if ( args->reply.err < 0 ){
		args->reply.err_number = errno;
	}

}

void link_cmd_read(link_data_t * args){
	args->reply.err = read_flash(args->op.read.addr, args->op.read.nbyte);
	return;
}

void link_cmd_write(link_data_t * args){
	args->reply.err = -1;
	return;
}

void erase_flash(void){
	int page = 0;
	while(flash_erasepage(FLASH_PORT, page++) != 0 ){
		//these are the bootloader pages and won't be erased
		gled_on();
		link_phy_wait(1);
		gled_off();
	}

	//erase the flash pages -- ends when an erase on an invalid page is attempted
	while( flash_erasepage(FLASH_PORT, page++) == 0 ){
		gled_on();
		link_phy_wait(1);
		gled_off();
	}
	gled_on();
}

void link_cmd_reset(link_data_t * args){
	gled_off();
	uint32_t * dfu_sw_req;
	link_phy_wait(500);
	link_phy_close(phy_handle);
	dfu_sw_req = (uint32_t*)SW_REQ_LOC;
	*dfu_sw_req = 0;
	_hwpl_core_priv_reset(NULL);
	//the program never arrives here
}

void link_cmd_reset_bootloader(link_data_t * args){
	uint32_t * dfu_sw_req;
	link_phy_wait(500);
	link_phy_close(phy_handle);
	dfu_sw_req = (uint32_t*)SW_REQ_LOC;
	*dfu_sw_req = SW_REQ_VALUE;
	_hwpl_core_priv_reset(NULL);
}

int read_flash_callback(void * context, void * buf, int nbyte){
	int * loc;
	int ret;
	loc = context;
	ret = flash_read(FLASH_PORT, *loc, buf, nbyte);
	if( ret > 0 ){
		*loc += ret;
	}
	return ret;
}

int read_flash(int loc, int nbyte){
	return link_protocol_slavewrite(phy_handle, NULL, nbyte, read_flash_callback, &loc);
}



/*! @} */


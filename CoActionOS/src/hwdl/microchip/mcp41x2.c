/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MCP41X2
 * @{
 *
 */

/*! \file  */

#include <errno.h>
#include "hwdl.h"
#include "hwpl/pio.h"
#include "hwpl/spi.h"

#include "dev/microchip/mcp41x2.h"

#define mcp41x2_rd() (mcp41x2_cmd(MCP41X2_CMD_RD, 0) & 0x03FF)
#define mcp41x2_wr(x) (mcp41x2_cmd(MCP41X2_CMD_WR, x))
#define mcp41x2_inc() (mcp41x2_cmd(MCP41X2_CMD_INC, -1))
#define mcp41x2_dec() (mcp41x2_cmd(MCP41X2_CMD_DEC, -1))


static void assert_cs(const device_cfg_t * cfg){
	hwpl_pio_clrmask(cfg->pcfg.spi.cs.port, (void*)(size_t)(1<<cfg->pcfg.spi.cs.pin));
}

static void deassert_cs(const device_cfg_t * cfg){
	hwpl_pio_setmask(cfg->pcfg.spi.cs.port, (void*)(size_t)(1<<cfg->pcfg.spi.cs.pin));
}

uint16_t mcp41x2_cmd(const device_cfg_t * cfg ,
		uint8_t cmd /* The command (use a mcp41x2_cmd_t value) */,
		int16_t data /* The data to be written or 0 */){

	uint8_t tmp0 = 0;
	uint8_t tmp1 = 0;
	int tmp;
	assert_cs(cfg);
	if ( data < 0 ){
		//This is an 8-bit command
		tmp = cmd;
		hwpl_spi_swap(cfg->periph.port, (void*)tmp);
	} else {
		//This is a 16-bit command -- the top 2 data bits need to be OR'd with the command
		tmp0 = (data >> 8) & 0x03;
		cmd |= tmp0;
		hwpl_spi_swap(cfg->periph.port, (void*)(size_t)cmd);
		hwpl_spi_swap(cfg->periph.port, (void*)(size_t)data);
	}
	deassert_cs(cfg);
	return ( (uint16_t)(tmp1 << 8) + tmp0);
}


int mcp41x2_open(const device_cfg_t * cfg){
	int err;
	pio_attr_t attr;

	//Check to see if the SPI port is properly configured
	err = hwdl_check_spi_port(cfg);
	if ( err < 0 ){
		return err;
	}

	deassert_cs(cfg);
	attr.mask = (1<<cfg->pcfg.spi.cs.pin);
	attr.mode = PIO_MODE_OUTPUT | PIO_MODE_DIRONLY;
	hwpl_pio_setattr(cfg->pcfg.spi.cs.port, &attr);
	//! \todo Ping the device to see if it is responding

	return 0;
}

int mcp41x2_ioctl(const device_cfg_t * cfg, int request, void * ctl){

	switch(request){
	default:
		return hwpl_spi_ioctl(cfg, request, ctl);
	}

}

int mcp41x2_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}

int mcp41x2_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int mcp41x2_close(const device_cfg_t * cfg){
	return hwpl_spi_close(cfg);
}


/*! @} */


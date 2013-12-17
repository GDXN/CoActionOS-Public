/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup AD73XX
 * @{
 */

/*! \file */
#include <errno.h>
#include "dev/analog/ad73xx.h"
#include "hwdl/analog/ad73xx.h"
#include "hwpl/spi.h"
#include "hwpl/pio.h"

#include "hwpl/debug.h"

static void assert_cs(const device_cfg_t * cfg){
	hwpl_pio_clrmask(cfg->pcfg.spi.cs.port, (void*)(size_t)(1<<cfg->pcfg.spi.cs.pin));

}

static void deassert_cs(const device_cfg_t * cfg){
	hwpl_pio_setmask(cfg->pcfg.spi.cs.port, (void*)(size_t)(1<<cfg->pcfg.spi.cs.pin));

}

int ad73xx_open(const device_cfg_t * cfg){
	pio_attr_t attr;

	if( hwdl_check_spi_port(cfg) < 0 ){
		//SPI port could not be opened with the proper configuration
		return -1;
	}

	deassert_cs(cfg);

	//Configure the chip select as an output
	attr.mask = (1<<cfg->pcfg.spi.cs.pin);
	attr.mode = PIO_MODE_OUTPUT | PIO_MODE_DIRONLY;
	hwpl_pio_setattr(cfg->pcfg.spi.cs.port, &attr);
	return 0;
}

static int complete_spi_operation(const device_cfg_t * cfg, uint32_t ignore){
	ad73xx_state_t * state = (ad73xx_state_t*)cfg->state;
	deassert_cs(cfg);
	state->bytes_read += 4;
	if ( state->bytes_read == state->rop_orig->nbyte ){
		state->rop_orig->callback(state->rop_orig->context, 0);
		return 0;
	} else {
		state->rop.nbyte = 4;
		state->rop.buf += 4;
		assert_cs(cfg);
		state->rop.callback = (hwpl_callback_t)complete_spi_operation;
		state->rop.context = (void*)cfg;
		hwpl_spi_read(cfg, &(state->rop));
		return 1; //don't clear the newly set SPI callback
	}
}

int ad73xx_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_spi_ioctl(cfg, request, ctl);
}

int ad73xx_read(const device_cfg_t * cfg, device_transfer_t * rop){
	ad73xx_state_t * state = (ad73xx_state_t*)cfg->state;
	assert_cs(cfg);
	state->rop_orig = rop;
	rop->nbyte &= ~0x03; //the read needs to be align to 4 bytes
	state->rop.callback = (hwpl_callback_t)complete_spi_operation;
	state->rop.context = (void*)cfg;
	state->rop.nbyte = 4;
	state->rop.buf = rop->buf;
	state->bytes_read = 0;

	return hwpl_spi_read(cfg, &(state->rop) );
}


int ad73xx_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int ad73xx_close(const device_cfg_t * cfg){
	//! \todo Power down the device on exit
	return hwpl_spi_close(cfg);
}

/*! @} */

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwpl/spi.h"
#include "hwpl/pio.h"
#include "dev/bosch/bma150.h"
#include "hwdl/bosch/bma150.h"

static void assert_cs(const device_cfg_t * cfg){
	hwpl_pio_clrmask(cfg->pcfg.spi.cs.port, (void*)(ssize_t)(1<<cfg->pcfg.spi.cs.pin));
}

static void deassert_cs(const device_cfg_t * cfg){
	hwpl_pio_setmask(cfg->pcfg.spi.cs.port, (void*)(ssize_t)(1<<cfg->pcfg.spi.cs.pin));
}


int bma150_open(const device_cfg_t * cfg){
	int err;
	uint8_t status;
	pio_attr_t attr;

	//make sure the SPI has a compatible configuration
	err = hwdl_check_spi_port(cfg);
	if ( err < 0 ){
		return err;
	}

	deassert_cs(cfg);
	attr.mask = (1<<cfg->pcfg.spi.cs.pin);
	attr.mode = PIO_MODE_OUTPUT | PIO_MODE_DIRONLY;
	hwpl_pio_setattr(cfg->pcfg.spi.cs.port, &attr);


	//Ping the Device to see if the ID is correct
	assert_cs(cfg);
	hwpl_spi_swap(cfg->periph.port, (void*)0x80);  //read the Chip ID
	status = hwpl_spi_swap(cfg->periph.port, (void*)0x00);
	deassert_cs(cfg);

	if ( status != 0x02 ){  //The Chip ID should always be 0x02
		errno = EIO;
		return -1;
	}

	return 0;
}

int bma150_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_spi_ioctl(cfg, request, ctl);
}

static void complete_spi_operation(const device_cfg_t * cfg, const void * data){
	bma150_state_t * state = (bma150_state_t*)cfg->state;
	deassert_cs(cfg);
	state->callback(state->context, 0);
	state->callback = NULL;
}

int bma150_read(const device_cfg_t * cfg, device_transfer_t * rop){
	bma150_state_t * state = (bma150_state_t*)cfg->state;
	state->callback = rop->callback;
	state->context = rop->context;
	rop->context = (void*)cfg;
	rop->callback = (hwpl_callback_t)complete_spi_operation;
	assert_cs(cfg);
	hwpl_spi_swap(DEVICE_GET_PORT(cfg), (void*)(0x80 | rop->loc));
	return hwpl_spi_read(cfg, rop);
}



int bma150_write(const device_cfg_t * cfg, device_transfer_t * wop){
	bma150_state_t * state = (bma150_state_t*)cfg->state;
	state->callback = wop->callback;
	state->context = wop->context;
	wop->context = (void*)cfg;
	wop->callback = (hwpl_callback_t)complete_spi_operation;
	assert_cs(cfg);
	hwpl_spi_swap(DEVICE_GET_PORT(cfg), (void*)wop->loc);
	return hwpl_spi_write(cfg, wop);
}

int bma150_close(const device_cfg_t * cfg){
	return 0;
}


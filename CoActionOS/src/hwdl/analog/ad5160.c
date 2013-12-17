/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup AD5160
 * @{
 *
 *
 */

/*! \file  */

#include "hwpl.h"
#include "hwdl.h"
#include "dev/analog/ad5160.h"
#include "hwpl/spi.h"
#include "hwpl/pio.h"

static void assert_cs(const device_cfg_t * cfg){
	hwpl_pio_clrmask(cfg->pcfg.spi.cs.port, (void*)(size_t)(1<<cfg->pcfg.spi.cs.pin));

}

static void deassert_cs(const device_cfg_t * cfg){
	hwpl_pio_setmask(cfg->pcfg.spi.cs.port, (void*)(size_t)(1<<cfg->pcfg.spi.cs.pin));
}


int ad5160_open(const device_cfg_t * cfg){
	int err;
	pio_attr_t attr;
	err = hwdl_check_spi_port((device_cfg_t*)cfg);
	if ( err ){
		//SPI port could not be opened with the proper configuration
		return -1;
	}

	attr.mask = (1<<cfg->pcfg.spi.cs.pin);
	attr.mode = PIO_MODE_OUTPUT | PIO_MODE_DIRONLY;
	hwpl_pio_setattr(cfg->pcfg.spi.cs.port, &attr);
	//This device is an input only device.  There is no way to tell if it is working
	return 0;
}

int ad5160_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return 0;
}

int ad5160_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return 0;
}

int ad5160_write(const device_cfg_t * cfg, device_transfer_t * wop){
	const uint8_t * bufp;
	bufp = (const uint8_t *)wop->buf;
	device_cfg_t* cfgp = (device_cfg_t*)cfg;
	assert_cs(cfg);
	hwpl_spi_swap(cfgp->periph.port, (void*)(size_t)(*bufp));
	deassert_cs(cfg);
	return 1;
}

int ad5160_close(const device_cfg_t * cfg){
	return hwpl_spi_close(cfg);
}



/*! @} */


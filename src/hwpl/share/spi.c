/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/spi.h"

//These functions are device specific
extern void _hwpl_spi_dev_power_on(int port);
extern void _hwpl_spi_dev_power_off(int port);
extern int _hwpl_spi_dev_powered_on(int port);
extern int _hwpl_spi_dev_read(const device_cfg_t * cfg, device_transfer_t * rop);
extern int _hwpl_spi_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const spi_ioctl_func_table[I_GLOBAL_TOTAL + I_SPI_TOTAL])(int, void*) = {
		hwpl_spi_getattr,
		hwpl_spi_setattr,
		hwpl_spi_setaction,
		hwpl_spi_swap,
		hwpl_spi_setduplex,
};

int hwpl_spi_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_SPI_PORTS,
			_hwpl_spi_dev_powered_on,
			_hwpl_spi_dev_power_on);
}

int hwpl_spi_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_SPI_PORTS,
			_hwpl_spi_dev_powered_on,
			spi_ioctl_func_table,
			I_GLOBAL_TOTAL + I_SPI_TOTAL);
}



int hwpl_spi_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_read(cfg, rop,
			HWPL_SPI_PORTS,
			_hwpl_spi_dev_powered_on,
			_hwpl_spi_dev_read);

}


int hwpl_spi_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg, wop,
			HWPL_SPI_PORTS,
			_hwpl_spi_dev_powered_on,
			_hwpl_spi_dev_write);

}

int hwpl_spi_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_SPI_PORTS, _hwpl_spi_dev_powered_on, _hwpl_spi_dev_power_off);
}



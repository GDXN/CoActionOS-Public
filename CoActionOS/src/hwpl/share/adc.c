/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/adc.h"


//These functions are device specific
extern void _hwpl_adc_dev_power_on(int port);
extern void _hwpl_adc_dev_power_off(int port);
extern int _hwpl_adc_dev_powered_on(int port);
extern int _hwpl_adc_dev_read(const device_cfg_t * cfg, device_transfer_t * rop);

int (* const adc_ioctl_func_table[I_GLOBAL_TOTAL])(int, void*) = {
		hwpl_adc_getattr,
		hwpl_adc_setattr,
		hwpl_adc_setaction
};

int hwpl_adc_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_ADC_PORTS,
			_hwpl_adc_dev_powered_on,
			_hwpl_adc_dev_power_on);
}

int hwpl_adc_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_ADC_PORTS,
			_hwpl_adc_dev_powered_on,
			adc_ioctl_func_table,
			I_GLOBAL_TOTAL);
}

int hwpl_adc_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_read(cfg,
			rop,
			HWPL_ADC_PORTS,
			_hwpl_adc_dev_powered_on,
			_hwpl_adc_dev_read);
}

int hwpl_adc_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_adc_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_ADC_PORTS, _hwpl_adc_dev_powered_on, _hwpl_adc_dev_power_off);
}



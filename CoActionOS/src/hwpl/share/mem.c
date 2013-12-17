/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/mem.h"


//These functions are device specific
extern void _hwpl_mem_dev_power_on(int port);
extern void _hwpl_mem_dev_power_off(int port);
extern int _hwpl_mem_dev_powered_on(int port);
extern int _hwpl_mem_dev_read(const device_cfg_t * cfg, device_transfer_t * rop);
extern int _hwpl_mem_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const mem_ioctl_func_table[I_GLOBAL_TOTAL + I_MEM_TOTAL])(int, void*) = {
		hwpl_mem_getattr,
		hwpl_mem_setattr,
		hwpl_mem_setaction,
		hwpl_mem_erasepage,
		hwpl_mem_getpageinfo,
		hwpl_mem_writepage
};

int hwpl_mem_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_MEM_PORTS,
			_hwpl_mem_dev_powered_on,
			_hwpl_mem_dev_power_on);
}

int hwpl_mem_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_MEM_PORTS,
			_hwpl_mem_dev_powered_on,
			mem_ioctl_func_table,
			I_GLOBAL_TOTAL + I_MEM_TOTAL);
}

int hwpl_mem_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_read(cfg, rop,
			HWPL_MEM_PORTS,
			_hwpl_mem_dev_powered_on,
			_hwpl_mem_dev_read);
}


int hwpl_mem_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg, wop,
			HWPL_MEM_PORTS,
			_hwpl_mem_dev_powered_on,
			_hwpl_mem_dev_write);

}

int hwpl_mem_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_MEM_PORTS, _hwpl_mem_dev_powered_on, _hwpl_mem_dev_power_off);
}



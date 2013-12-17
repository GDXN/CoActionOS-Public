/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/uart.h"

//These functions are device specific
extern void _hwpl_uart_dev_power_on(int port);
extern void _hwpl_uart_dev_power_off(int port);
extern int _hwpl_uart_dev_powered_on(int port);
extern int _hwpl_uart_dev_read(const device_cfg_t * cfg, device_transfer_t * rop);
extern int _hwpl_uart_dev_write(const device_cfg_t * cfg, device_transfer_t * wop);

int (* const uart_ioctl_func_table[I_GLOBAL_TOTAL + I_UART_TOTAL])(int, void*) = {
		hwpl_uart_getattr,
		hwpl_uart_setattr,
		hwpl_uart_setaction,
		hwpl_uart_clear,
		hwpl_uart_getbyte,
		hwpl_uart_flush
};

int hwpl_uart_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_UART_PORTS,
			_hwpl_uart_dev_powered_on,
			_hwpl_uart_dev_power_on);
}

int hwpl_uart_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_UART_PORTS,
			_hwpl_uart_dev_powered_on,
			uart_ioctl_func_table,
			I_GLOBAL_TOTAL + I_UART_TOTAL);
}



int hwpl_uart_read(const device_cfg_t * cfg, device_transfer_t * rop){
	return hwpl_read(cfg, rop,
			HWPL_UART_PORTS,
			_hwpl_uart_dev_powered_on,
			_hwpl_uart_dev_read);
}

int hwpl_uart_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_write(cfg, wop,
			HWPL_UART_PORTS,
			_hwpl_uart_dev_powered_on,
			_hwpl_uart_dev_write);

}

int hwpl_uart_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_UART_PORTS, _hwpl_uart_dev_powered_on, _hwpl_uart_dev_power_off);
}



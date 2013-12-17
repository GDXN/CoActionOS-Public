/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "local.h"
#include "hwpl/rtc.h"

//These functions are device specific
extern void _hwpl_rtc_dev_power_on(int port);
extern void _hwpl_rtc_dev_power_off(int port);
extern int _hwpl_rtc_dev_powered_on(int port);

int (* const rtc_ioctl_func_table[I_GLOBAL_TOTAL + I_RTC_TOTAL])(int, void*) = {
		hwpl_rtc_getattr,
		hwpl_rtc_setattr,
		hwpl_rtc_setaction,
		hwpl_rtc_setalarm,
		hwpl_rtc_getalarm,
		hwpl_rtc_disablealarm,
		hwpl_rtc_set,
		hwpl_rtc_get,
		hwpl_rtc_setcountevent
};

int hwpl_rtc_open(const device_cfg_t * cfg){
	return hwpl_open(cfg,
			HWPL_RTC_PORTS,
			_hwpl_rtc_dev_powered_on,
			_hwpl_rtc_dev_power_on);
}

int hwpl_rtc_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	return hwpl_ioctl(cfg,
			request,
			ctl,
			HWPL_RTC_PORTS,
			_hwpl_rtc_dev_powered_on,
			rtc_ioctl_func_table,
			I_GLOBAL_TOTAL + I_RTC_TOTAL);
}

int hwpl_rtc_read(const device_cfg_t * cfg, device_transfer_t * rop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_rtc_write(const device_cfg_t * cfg, device_transfer_t * wop){
	errno = ENOTSUP;
	return -1;
}

int hwpl_rtc_close(const device_cfg_t * cfg){
	return hwpl_close(cfg, HWPL_RTC_PORTS, _hwpl_rtc_dev_powered_on, _hwpl_rtc_dev_power_off);
}



/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwpl/adc.h"
#include "hwdl.h"
#include "dev/st/stlm20.h"

int stlm20_open(const device_cfg_t * cfg){
	//Check the ADC port configuration
	return hwdl_check_adc_port(cfg);
}

int stlm20_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	stlm20_attr_t * attr = ctl;
	switch(request){
	case I_STLM20_GETATTR:
		attr->adc_ref = cfg->pcfg.adc.reference;
		return 0;
	default:
		return hwpl_adc_ioctl(cfg, request, ctl);
	}
}

int stlm20_read(const device_cfg_t * cfg, device_transfer_t * rop){
	rop->loc = cfg->pcfg.adc.channels[0];
	return hwpl_adc_read(cfg, rop);
}

int stlm20_write(const device_cfg_t * cfg, device_transfer_t * wop){
	return hwpl_adc_write(cfg, wop);
}

int stlm20_close(const device_cfg_t * cfg){
	return hwpl_adc_close(cfg);
}

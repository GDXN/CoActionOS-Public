/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwdl.h"
#include "hwpl/adc.h"

#include "hwpl/debug.h"

/* \details This function checks to see if the ADC
 * port is configured to use the channels specified
 * by the configuration.  If the ADC port is closed,
 * it is opened and configured using this function.
 * \return Zero if the port is ready to use; an
 * error code if it could not be configured.
 */
int hwdl_check_adc_port(const device_cfg_t * cfgp){
	int err;
	int i;
	uint16_t enabled_channels;
	adc_attr_t adc_cfg;

	enabled_channels = 0;
	for(i=0; i < DEVICE_MAX_ADC_CHANNELS; i++){
		if ( cfgp->pcfg.adc.channels[i] != -1 ){
			if ( cfgp->pcfg.adc.channels[i] < DEVICE_MAX_ADC_CHANNELS ){
				enabled_channels |= (1<<cfgp->pcfg.adc.channels[i]);
			}
		}
	}

	err = hwpl_adc_ioctl(cfgp, I_ADC_GETATTR, &adc_cfg);
	if ( err == 0 ){ //port is already open
		//Check the port configuration

		if ( adc_cfg.pin_assign != cfgp->pin_assign ){
			errno = EINVAL;
			return -1;
		}

		//Now enable the channels
		adc_cfg.enabled_channels |= enabled_channels;
		if ( (err = hwpl_adc_ioctl(cfgp, I_ADC_SETATTR, &adc_cfg) ) < 0 ){
			return err;
		}

	} else {
		//Open the ADC port
		adc_cfg.pin_assign = cfgp->pin_assign;
		adc_cfg.enabled_channels = enabled_channels;
		adc_cfg.freq = ADC_MAX_FREQ;

		err = hwpl_adc_open(cfgp);
		if ( err ){
			return err;
		}

		return hwpl_adc_ioctl(cfgp, I_ADC_SETATTR, &adc_cfg);
	}

	return 0;
}

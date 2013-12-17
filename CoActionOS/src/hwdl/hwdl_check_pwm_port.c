/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwdl.h"
#include "hwpl/pwm.h"


/* \details This function checks to see if the PWM
 * port is configured to use the channels specified
 * by the configuration.  If the PWM port is closed,
 * it is opened and configured using this function.
 * \return Zero if the port is ready to use; an
 * error code if it could not be configured.
 */
int hwdl_check_pwm_port(const device_cfg_t * cfgp){
	int err;
	int i;
	uint16_t enabled_channels;
	pwm_attr_t pwm_cfg;

	enabled_channels = 0;
	for(i=0; i < DEVICE_MAX_PWM_CHANNELS; i++){
		if ( cfgp->pcfg.pwm.channels[i] != -1 ){
			if ( cfgp->pcfg.pwm.channels[i] < DEVICE_MAX_PWM_CHANNELS ){
				enabled_channels |= (1<<cfgp->pcfg.pwm.channels[i]);
			}
		}
	}

	err = hwpl_pwm_ioctl(cfgp, I_PWM_GETATTR, &pwm_cfg);
	if ( err == 0 ){ //port is already open
		//Check the port configuration

		if ( pwm_cfg.pin_assign != cfgp->pin_assign ){
			errno = EINVAL;
			return -1;
		}

		if ( pwm_cfg.freq != cfgp->pcfg.pwm.freq ){
			errno = EINVAL;
			return -1;
		}

		if ( pwm_cfg.top != cfgp->pcfg.pwm.top ){
			errno = EINVAL;
			return -1;
		}

		//Now enable the channels
		pwm_cfg.enabled_channels |= enabled_channels;
		if ( (err = hwpl_pwm_ioctl(cfgp, I_PWM_SETATTR, &pwm_cfg) ) ){
			return err;
		}

	} else {
		//Open the PWM port
		pwm_cfg.pin_assign = cfgp->pin_assign;
		pwm_cfg.enabled_channels = enabled_channels;
		pwm_cfg.freq = cfgp->pcfg.pwm.freq;
		pwm_cfg.top = cfgp->pcfg.pwm.top;

		err = hwpl_pwm_open(cfgp);
		if ( err ){
			return err;
		}
		return hwpl_pwm_ioctl(cfgp, I_PWM_SETATTR, &pwm_cfg);
	}

	return 0;
}

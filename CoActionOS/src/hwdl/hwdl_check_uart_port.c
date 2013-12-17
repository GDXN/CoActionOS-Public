/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include "hwdl.h"
#include "hwpl/uart.h"

/* \details This function checks to see if a UART port is opened
 * and configured in a way that is compatible with the
 * configuration specified.  If the UART port is not
 * already open, this function will open it with
 * the configuration specified.
 * \return 0 if the port is ready to use or an error
 * code if the port is already open with
 * incompatible settings.
 */
int hwdl_check_uart_port(const device_cfg_t * cfgp){
	int err;
	uart_attr_t attr;

	//check to see if the UART port is already configured
	err = hwpl_uart_ioctl(cfgp, I_UART_GETATTR, &attr);
	if ( err == 0 ){

		if ( attr.pin_assign != cfgp->pin_assign ){
			errno = EINVAL;
			return -1;
		}

		if ( attr.width != cfgp->pcfg.uart.width ){
			errno = EINVAL;
			return -1;
		}

		if ( attr.baudrate != cfgp->bitrate ){
			errno = EINVAL;
			return -1;
		}

		if ( attr.parity != cfgp->pcfg.uart.parity ){
			errno = EINVAL;
			return -1;
		}

		if ( attr.stop != cfgp->pcfg.uart.stop_bits ){
			errno = EINVAL;
			return -1;
		}


	} else {
		//Open the port with the desired configuration
		attr.pin_assign = cfgp->pin_assign;
		attr.width = cfgp->pcfg.spi.width;
		attr.baudrate = cfgp->bitrate;
		attr.stop = cfgp->pcfg.uart.stop_bits;
		attr.parity = cfgp->pcfg.uart.parity;
		err = hwpl_uart_open(cfgp);
		if ( err < 0 ){
			return err;
		}

		return hwpl_uart_ioctl(cfgp, I_UART_SETATTR, &attr);
	}

	return 0;
}

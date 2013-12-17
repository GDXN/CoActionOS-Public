/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \file */


/*! \addtogroup HWDL Hardware Device Library
 * @{
 *
 * \details The HWDL is a collection of device drivers organized by device
 * manufacturer.  Additional system drivers are available such as /dev/null and
 * a FIFO (which can be used to share data between processes).
 *
 */

/*! \addtogroup ANALOG Analog Devices
 *
 */

/*! \addtogroup ATMEL Atmel
 *
 */

/*! \addtogroup BOSCH Bosch Sensortec
 *
 */

/*! \addtogroup DIGI Digi International
 *
 */

/*! \addtogroup FREESCALE Freescale
 *
 */

/*! \addtogroup GARMIN Garmin
 *
 */

/*! \addtogroup MAXIM Maxim
 *
 */

/*! \addtogroup MICROCHIP Microchip
 *
 */

/*! \addtogroup NXP NXP
 *
 */

/*! \addtogroup SYS System
 *
 */

/*! \addtogroup ST STMicroelectronics
 *
 */

/*! \addtogroup TI Texas Instruments
 *
 */

/*! \addtogroup VISHAY Vishay
 *
 */

#ifndef HWDL_H_
#define HWDL_H_

#include <stdint.h>
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

int hwdl_check_adc_port(const device_cfg_t * cfgp);
int hwdl_check_spi_port(const device_cfg_t * cfgp);
int hwdl_check_i2c_port(const device_cfg_t * cfgp);
int hwdl_check_pwm_port(const device_cfg_t * cfgp);
int hwdl_check_uart_port(const device_cfg_t * cfgp);


#ifdef __cplusplus
}
#endif


#endif /* HWDL_H_ */

/*! @} */

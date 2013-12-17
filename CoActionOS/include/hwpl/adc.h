/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup ADC_DEV ADC
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_ADC_H_
#define _HWPL_ADC_H_

#include "../device.h"
#include "../dev/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_adc_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_adc_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_adc_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_adc_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_adc_close(const device_cfg_t * cfg);


int hwpl_adc_getattr(int port, void * ctl);
int hwpl_adc_setattr(int port, void * ctl);
int hwpl_adc_setaction(int port, void * ctl);

#ifdef __cplusplus
}
#endif


#endif // ADC_H_

/*! @} */


/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SPI_DEV SPI
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_SPI_H_
#define _HWPL_SPI_H_


#include "../device.h"
#include "../dev/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_spi_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_spi_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_spi_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_spi_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_spi_close(const device_cfg_t * cfg);

int hwpl_spi_getattr(int port, void * ctl);
int hwpl_spi_setattr(int port, void * ctl);
int hwpl_spi_setaction(int port, void * ctl);
int hwpl_spi_swap(int port, void * ctl);
int hwpl_spi_setduplex(int port, void * ctl);

int hwpl_ssp_open(const device_cfg_t * cfg);
int hwpl_ssp_read(const device_cfg_t * cfg, device_transfer_t * rop);
int hwpl_ssp_write(const device_cfg_t * cfg, device_transfer_t * wop);
int hwpl_ssp_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int hwpl_ssp_close(const device_cfg_t * cfg);
int hwpl_ssp_getattr(int port, void * ctl);
int hwpl_ssp_setattr(int port, void * ctl);
int hwpl_ssp_setaction(int port, void * ctl);
int hwpl_ssp_swap(int port, void * ctl);
int hwpl_ssp_setduplex(int port, void * ctl);


#ifdef __cplusplus
}
#endif


#endif /* _HWPL_SPI_H_ */

/*! @} */

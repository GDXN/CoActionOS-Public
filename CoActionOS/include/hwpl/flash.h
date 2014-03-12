/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup FLASH_DEV Flash
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_FLASH_H_
#define _HWPL_FLASH_H_


#include "../device.h"
#include "../dev/flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 *
 * The following is an example of how to open the EINT:
 * \code
 * flash_attr_t flash_cfg;
 * if ( flash_open(0, &flash_cfg) ){
 *  //Failed
 * }
 * \endcode
 *
 * \sa periph_open()
 *
 */
int hwpl_flash_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_flash_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_flash_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_flash_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_flash_close(const device_cfg_t * cfg);


int hwpl_flash_getattr(int port, void * ctl);
int hwpl_flash_setattr(int port, void * ctl);
int hwpl_flash_setaction(int port, void * ctl);

int hwpl_flash_eraseaddr(int port, void * ctl);
int hwpl_flash_erasepage(int port, void * ctl);
int hwpl_flash_getpage(int port, void * ctl);
int hwpl_flash_getsize(int port, void * ctl);
int hwpl_flash_getpageinfo(int port, void * ctl);
int hwpl_flash_writepage(int port, void * ctl);

#ifdef __cplusplus
}
#endif


#endif // _HWPL_FLASH_H_

/*! @} */



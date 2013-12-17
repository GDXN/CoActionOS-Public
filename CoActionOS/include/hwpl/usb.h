/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup USB_DEV USB
 * @{
 * \ingroup DEV
 *
 */

#ifndef _HWPL_USB_H_
#define _HWPL_USB_H_

#include "../device.h"
#include "../dev/usb.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_usb_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_usb_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_usb_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_usb_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_usb_close(const device_cfg_t * cfg);


int hwpl_usb_getattr(int port, void * ctl);
int hwpl_usb_setattr(int port, void * ctl);
int hwpl_usb_setaction(int port, void * ctl);

int hwpl_usb_reset(int port, void * ctl);
int hwpl_usb_attach(int port, void * ctl);
int hwpl_usb_detach(int port, void * ctl);
int hwpl_usb_configure(int port, void * ctl);
int hwpl_usb_setaddr(int port, void * ctl);
int hwpl_usb_resetep(int port, void * ctl);
int hwpl_usb_enableep(int port, void * ctl);
int hwpl_usb_disableep(int port, void * ctl);
int hwpl_usb_stallep(int port, void * ctl);
int hwpl_usb_unstallep(int port, void * ctl);
int hwpl_usb_cfgep(int port, void * ctl);
int hwpl_usb_seteventhandler(int port, void * ctl);
int hwpl_usb_isconnected(int port, void * ctl);


//Endpoint functions
int hwpl_usb_rd_ep(int port,
		uint32_t endpoint_num ,
		void * dest);
int hwpl_usb_wr_ep(int port,
		uint32_t endpoint_num,
		const void * src,
		uint32_t size);


#ifdef __cplusplus
}
#endif

#endif /* _HWPL_USB_H_ */

/*! @} */

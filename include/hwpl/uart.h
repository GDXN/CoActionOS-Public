/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup UART_DEV UART
 * @{
 *
 * \ingroup DEV
 *
 */

#ifndef _HWPL_UART_H_
#define _HWPL_UART_H_

#include "../device.h"
#include "../dev/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_uart_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_uart_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_uart_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_uart_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_uart_close(const device_cfg_t * cfg);


int hwpl_uart_getattr(int port, void * ctl);
int hwpl_uart_setattr(int port, void * ctl);
int hwpl_uart_setaction(int port, void * ctl);

int hwpl_uart_clear(int port, void * ctl);
int hwpl_uart_getbyte(int port, void * ctl);
int hwpl_uart_flush(int port, void * ctl);

#ifdef __cplusplus
}
#endif


#endif // _HWPL_UART_H_

/*! @} */


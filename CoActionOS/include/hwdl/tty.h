/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup TTY
 * @{
 */

/*! \file  */
#ifndef HWDL_TTY_H_
#define HWDL_TTY_H_

#include "hwdl.h"

int tty_usbbulk_open(const device_cfg_t * cfg);
int tty_usbbulk_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int tty_usbbulk_write(const device_cfg_t * cfg, device_transfer_t * wop);
int tty_usbbulk_read(const device_cfg_t * cfg, device_transfer_t * rop);
int tty_usbbulk_close(const device_cfg_t * cfg);

int tty_uart_open(const device_cfg_t * cfg);
int tty_uart_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int tty_uart_write(const device_cfg_t * cfg, device_transfer_t * wop);
int tty_uart_read(const device_cfg_t * cfg, device_transfer_t * rop);
int tty_uart_close(const device_cfg_t * cfg);


#endif /* HWDL_TTY_H_ */

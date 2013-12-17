/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYS_DEV System Devices
 * @{
 *
 *
 * \ingroup SYS
 *
 * \details This is a system driver that provides the following devices:
 * - null
 * - zero
 * - full
 * - random
 * - urandom
 *
 *
 */

/*! \file  */

#ifndef HWDL_SYS_H_
#define HWDL_SYS_H_

#include "dev/sys.h"
#include "hwdl.h"


int led_open(const device_cfg_t * cfg);
int led_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int led_read(const device_cfg_t * cfg, device_transfer_t * rop);
int led_write(const device_cfg_t * cfg, device_transfer_t * wop);
int led_close(const device_cfg_t * cfg);

int null_open(const device_cfg_t * cfg);
int null_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int null_read(const device_cfg_t * cfg, device_transfer_t * rop);
int null_write(const device_cfg_t * cfg, device_transfer_t * wop);
int null_close(const device_cfg_t * cfg);

int zero_open(const device_cfg_t * cfg);
int zero_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int zero_read(const device_cfg_t * cfg, device_transfer_t * rop);
int zero_write(const device_cfg_t * cfg, device_transfer_t * wop);
int zero_close(const device_cfg_t * cfg);

int full_open(const device_cfg_t * cfg);
int full_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int full_read(const device_cfg_t * cfg, device_transfer_t * rop);
int full_write(const device_cfg_t * cfg, device_transfer_t * wop);
int full_close(const device_cfg_t * cfg);

int random_open(const device_cfg_t * cfg);
int random_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int random_read(const device_cfg_t * cfg, device_transfer_t * rop);
int random_write(const device_cfg_t * cfg, device_transfer_t * wop);
int random_close(const device_cfg_t * cfg);

int urandom_open(const device_cfg_t * cfg);
int urandom_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int urandom_read(const device_cfg_t * cfg, device_transfer_t * rop);
int urandom_write(const device_cfg_t * cfg, device_transfer_t * wop);
int urandom_close(const device_cfg_t * cfg);

int sys_open(const device_cfg_t * cfg);
int sys_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int sys_read(const device_cfg_t * cfg, device_transfer_t * rop);
int sys_write(const device_cfg_t * cfg, device_transfer_t * wop);
int sys_close(const device_cfg_t * cfg);


#endif /* HWDL_FIFO_H_ */


/*! @} */

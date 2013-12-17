/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SST25VF SST25VF Serial Flash Storage
 *
 * @{
 *
 */

#ifndef HWDL_S29GL_H_
#define HWDL_S29GL_H_

#include "hwdl.h"
#include "dev/spansion/s29gl.h"

typedef struct {
	const char * buf;
	int nbyte;
	uint8_t cmd[8];
	device_transfer_t op;
	hwpl_callback_t callback;
	void * context;
	int prot;
} s29gl_state_t;

int s29gl_open(const device_cfg_t * cfg);
int s29gl_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int s29gl_read(const device_cfg_t * cfg, device_transfer_t * rop);
int s29gl_write(const device_cfg_t * cfg, device_transfer_t * wop);
int s29gl_close(const device_cfg_t * cfg);

int s29gl_tmr_open(const device_cfg_t * cfg);
int s29gl_tmr_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int s29gl_tmr_read(const device_cfg_t * cfg, device_transfer_t * rop);
int s29gl_tmr_write(const device_cfg_t * cfg, device_transfer_t * wop);
int s29gl_tmr_close(const device_cfg_t * cfg);


#endif /* HWDL_S29GL_H_ */

/*! @} */

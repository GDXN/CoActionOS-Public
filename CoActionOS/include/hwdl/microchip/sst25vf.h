/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SST25VF SST25VF Serial Flash Storage
 *
 * @{
 *
 */

#ifndef HWDL_SST25VF_H_
#define HWDL_SST25VF_H_

#include "hwdl.h"
#include "dev/microchip/sst25vf.h"

typedef struct {
	const char * buf;
	int nbyte;
	uint8_t cmd[8];
	device_transfer_t op;
	hwpl_callback_t callback;
	void * context;
	int prot;
} sst25vf_state_t;

int sst25vf_open(const device_cfg_t * cfg);
int sst25vf_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int sst25vf_read(const device_cfg_t * cfg, device_transfer_t * rop);
int sst25vf_write(const device_cfg_t * cfg, device_transfer_t * wop);
int sst25vf_close(const device_cfg_t * cfg);

int sst25vf_tmr_open(const device_cfg_t * cfg);
int sst25vf_tmr_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int sst25vf_tmr_read(const device_cfg_t * cfg, device_transfer_t * rop);
int sst25vf_tmr_write(const device_cfg_t * cfg, device_transfer_t * wop);
int sst25vf_tmr_close(const device_cfg_t * cfg);


#endif /* HWDL_SST25VF_H_ */

/*! @} */

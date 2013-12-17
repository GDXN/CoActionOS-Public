/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MCP4725
 * @{
 *
 *
 */

/*! \file  */
#ifndef HWDL_MCP4725_H_
#define HWDL_MCP4725_H_

#include "hwdl.h"
#include "dev/mcp4725.h"

int mcp4725_open(const device_cfg_t * cfg);
int mcp4725_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int mcp4725_read(const device_cfg_t * cfg, device_transfer_t * rop);
int mcp4725_write(const device_cfg_t * cfg, device_transfer_t * wop);
int mcp4725_close(const device_cfg_t * cfg);

#endif /* HWDL_MCP4725_H_ */


/*! @} */

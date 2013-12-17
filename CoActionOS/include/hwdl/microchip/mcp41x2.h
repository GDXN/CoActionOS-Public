/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MCP41X2
 * @{
 *
 */

/*! \file  */

#ifndef HWDL_MCP41X2_H_
#define HWDL_MCP41X2_H_


#include "hwdl.h"

int mcp41x2_open(const device_cfg_t * cfg);
int mcp41x2_ioctl(const device_cfg_t * cfg, int request, void * ctl);
int mcp41x2_read(const device_cfg_t * cfg, device_transfer_t * rop);
int mcp41x2_write(const device_cfg_t * cfg, device_transfer_t * wop);
int mcp41x2_close(const device_cfg_t * cfg);

#endif /* HWDL_MCP41X2_H_ */

/*! @} */


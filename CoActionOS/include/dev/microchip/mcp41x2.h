/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MCP41X2 MCP41X2 Digital Potentiometer (alpha)
 * @{
 *
 * \ingroup MICROCHIP
 *
 * \details This device driver interfaces with Microchips MCP41X2 digital potentiometers
 * using the SPI bus and a IO line attached to the chip select line.
 *
 *
 */

/*! \file  */

#ifndef DEV_MCP41X2_H_
#define DEV_MCP41X2_H_

#include <stdint.h>
#include "dev/ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief defines a type for the valid mcp41x2 commands.
 * \details This enumeration defines a type for the
 * valid mcp41x2 commands.
 */
typedef enum {
	MCP41X2_CMD_WR = (0x00<<2),
	MCP41X2_CMD_INC = (0x01<<2),
	MCP41X2_CMD_DEC = (0x02<<2),
	MCP41X2_CMD_RD = (0x03<<2)
} mcp41x2_cmd_t;




typedef struct {
	uint8_t power_mode;
	uint16_t dac_value;
} mcp41x2_attr_t;


#define I_MCP41X2_POWERMODE (HWDL_DEVICE_IOCTL_REQ|0x00)
#define I_MCP41X2_WRITEDAC (HWDL_DEVICE_IOCTL_REQ|0x01)
#define I_MCP41X2_WRITEDACEEPROM (HWDL_DEVICE_IOCTL_REQ|0x02)

#ifdef __cplusplus
}
#endif



#endif /* DEV_MCP41X2_H_ */

/*! @} */


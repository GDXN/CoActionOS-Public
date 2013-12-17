/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MCP4725 MCP4725 Digital to Analog Converter
 * @{
 *
 * \details This is a driver for the MCP4725 analog to digital converter.
 *
 * Here is an example of how to use the driver
 * \code
 * #include <fcntl.h>
 * #include <unistd.h>
 * #include <dev/mcp4725.h>
 *
 * int fd;
 * mcp4725_t data;
 * uint16_t value;
 * fd = open("/dev/mcp4725-0", O_RDWR);
 * if ( fd < 0 ){
 * 	perror("failed to open /dev/mcp4725-0");
 * }
 *
 * value = 786; //This is a twelve bit DAC
 * data.cmd = MCP4725_POWERDOWN_NORMAL|MCP4725_LOC_WRITE_DAC;
 * data.high = (value & 0x0FF0) >> 4; //the top 8 bits are in this byte
 * data.low = (value & 0x0F) << 4; //the bottom 4 bits are left-justified in this byte
 * if ( write(fd, &data, sizeof(data)) != sizeof(data) ){
 * 	perror("failed to write");
 * }
 * close(fd);
 *
 * \endcode
 *
 * \ingroup MICROCHIP
 *
 *
 */

/*! \file  */
#ifndef DEV_MCP4725_H_
#define DEV_MCP4725_H_

#include <stdint.h>
#include "hwpl/types.h"


/*! \details This defines the configuration values for
 * a MCP4725 device in the device table.
 *
 * \param device_name The name of the device (e.g "mcp4725-0")
 * \param port_number The I2C peripheral port number
 * \param pin_assign_value The GPIO configuration to use with the I2C bus
 * \param i2c_slave_addr The slave address of the MCP4725
 * \param bitrate_value The maximum allowable bitrate for the MCP4725
 * \param mode_value The access mode (usually 0666)
 * \param uid_value The User ID
 * \param gid_value The Group ID
 *
 * Example:
 * \code
 * const device_t devices[DEVICES_TOTAL + 1] = {
 * 	DEVICE_PERIPH("i2c0", i2c, 0, 0666, USER_ROOT, GROUP_ROOT, S_IFCHR),
 * 	... //other peripherals if desired
 * 	MMA7660_DEVICE("mcp4725-0", 0, 0, 0x4C, 100000, 0666, USER_ROOT, GROUP_ROOT),
 * 	... //other devices if desired
 * 	DEVICE_TERMINATOR
 *  }
 * \endcode
 * \hideinitializer
 *
 */
#define MCP4725_DEVICE(device_name, port_number, pin_assign_value, i2c_slave_addr, bitrate_value, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFBLK), \
		DEVICE_DRIVER(mcp4725), \
		.cfg.periph.port = port_number, \
		.cfg.pin_assign = pin_assign_value, \
		.cfg.pcfg.i2c.slave_addr = i2c_slave_addr, \
		.cfg.bitrate = bitrate_value \
}

#ifdef __cplusplus
extern "C" {
#endif

#define MCP4725_LOC_WRITE_DAC (2<<5)
/*! \brief EERPOM and DAC write.
 * \hideinitializer
 */
#define MCP4725_LOC_WRITE_DAC_EERPOM (3<<5)

/*! \brief Normal power (OR with mcp4725_t.cmd)
 * \hideinitializer
 */
#define MCP4725_LOC_POWERDOWN_NORMAL (0)
/*! \brief Power down with 1K pulldown (OR with mcp4725_t.cmd)
 * \hideinitializer
 */
#define MCP4725_LOC_POWERDOWN_1KGROUND (1<<1)
/*! \brief Power down with 100K pulldown (OR with mcp4725_t.cmd)
 * \hideinitializer
 */
#define MCP4725_LOC_POWERDOWN_100KGROUND (2<<1)
/*! \brief Power down with 500K pulldown (OR with mcp4725_t.cmd)
 * \hideinitializer
 */
#define MCP4725_LOC_POWERDOWN_500KGROUND (3<<1)



/*! \details This is the data structure used to access the MCP4725
 * registers.
 */
typedef struct HWPL_PACK {
	uint8_t high /*! D11 to D4 */;
	uint8_t low /*! \brief Bits: D3 D2 D1 D0 X X X X */;
} mcp4725_t;

/*! \brief This is the maximum value that the DAC can have.
 */
#define MCP4725_MAX 4095


#ifdef __cplusplus
}
#endif

#endif /* DEV_MCP4725_H_ */


/*! @} */

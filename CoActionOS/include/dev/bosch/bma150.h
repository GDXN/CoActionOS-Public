/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup BMA150 BMA150 Accelerometer
 * @{
 *
 *
 * \ingroup BOSCH
 *
 * \details This device driver reads the Bosch BMA150 accelerometer.
 *
 *
 * Required Peripheral Hardware:
 * - 1 SPI Bus (MODE3)
 * - 1 GPIO Pin(for chip select)
 *
 * Example:
 *
 * \code
 *
 * #include <stddef.h>
 * #include <stdio.h>
 * #include <unistd.h>
 * #include <fcntl.h>
 * #include <dev/bma150.h>
 *
 * void read_accel(void){
 * 	int fd;
 * 	char mode;
 * 	bma150_sample_t sample;
 * 	fd = open("/dev/bma150-0", O_RDWR);
 * 	if ( fd < 0 ){
 * 		perror("failed to open");
 * 		return;
 * 	}
 *
 *	lseek(fd, offsetof(bma150_t, x), SEEK_SET);
 *	if ( read(fd, &sample, sizeof(sample)) != sizeof(sample) ){
 *		perror("read failed");
 *		return;
 *	}
 *
 *	printf("%d, %d, %d\n", sample.x, sample.y, sample.z);
 *	close(fd);
 *
 * }
 *
 * \endcode
 *
 *
 */

/*! \file  */

#ifndef DEV_BMA150_H_
#define DEV_BMA150_H_

#include <stdint.h>
#include "hwpl/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief This is used to declare a value in the device list.
 *
 * \details This defines the configuration values for
 * a BMA150 device in the device table.
 *
 *
 * \param device_name The name of the device (e.g "bma150-0")
 * \param port_number The SPI peripheral port number
 * \param pin_assign_value The GPIO configuration to use with the SPI bus
 * \param cs_port_value The GPIO port used for the chip select
 * \param cs_pin_value The GPIO pin used for the chip select
 * \param bitrate_value The maximum allowable bitrate for the BMA150
 * \param state_var A pointer to a ad73xx_state_t variable
 * \param mode_value The access mode (usually 0666)
 * \param uid_value The User ID
 * \param gid_value The Group ID
 *
 * \hideinitializer
 *
 */
#define BMA150_DEVICE(device_name, port_number, pin_assign_value, cs_port_value, cs_pin_value, bitrate_value, state_var, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFBLK), \
		DEVICE_DRIVER(bma150), \
		.cfg.periph.port = port_number, \
		.cfg.pin_assign = pin_assign_value, \
		.cfg.pcfg.spi.mode = SPI_ATTR_MODE3, \
		.cfg.pcfg.spi.cs.port = cs_port_value, \
		.cfg.pcfg.spi.cs.pin = cs_pin_value, \
		.cfg.pcfg.spi.width = 8, \
		.cfg.pcfg.spi.format = SPI_ATTR_FORMAT_SPI, \
		.cfg.bitrate = bitrate_value, \
		.cfg.state = state_var, \
		.cfg.dcfg = NULL \
}

/*! \brief This holds the x, y, z data read from the device.
 *
 * \details This defines the data for taking a sample
 * from the accelerometer.
 *
 */
typedef struct HWPL_PACK {
	int16_t x /*! \brief The X value */;
	int16_t y /*! \brief The Y value */;
	int16_t z /*! \brief The Z value */;
} bma150_sample_t;


/*! \brief BMA150 Register Map Data Structure
 * \details This defines the register map for the
 * BMA150.  See the datasheet for more details.
 */
typedef struct HWPL_PACK {
	uint8_t chipid /*! \brief The device's Chip ID (0x00)*/;
	uint8_t version /*! \brief 0x01 The version number */;
	int16_t x /*! \brief 0x02 The X value */;
	int16_t y /*! \brief 0x04 The Y value */;
	int16_t z /*! \brief 0x06 The Z value */;
	uint8_t status /*! \brief 0x08 The status byte */;
	uint8_t control0 /*! \brief 0x09 Control word */;
	uint8_t control1 /*! \brief 0x0A Control word */;
	uint8_t lg_thres /*! \brief 0x0B See datasheet */;
	uint8_t lg_dur /*! \brief 0x0C See datasheet */;
	uint8_t hg_thres /*! \brief 0x0D See datasheet */;
	uint8_t hg_dur /*! \brief 0x0E See datasheet */;
	uint8_t any_motion_thres /*! \brief 0x0F See datasheet */;
	uint8_t hyst /*! \brief 0x10 See datasheet */;
	uint16_t reserved /*! \brief 0x11 See datasheet */;
	uint8_t range_bandwidth /*! \brief 0x13 See datasheet */;
	uint8_t control2 /*! \brief 0x14 Control word */;
	uint8_t control3 /*! \brief 0x15 Control word */;
	uint8_t offset_gain_x /*! \brief 0x16 X offset and gain */;
	uint8_t offset_gain_y /*! \brief 0x17 Y offset and gain */;
	uint8_t offset_gain_z /*! \brief 0x18 Z offset and gain */;
	uint8_t offset_gain_t /*! \brief 0x16 T offset and gain */;
	uint8_t offset_x /*! \brief 0x16 X offset */;
	uint8_t offset_y /*! \brief 0x17 Y offset */;
	uint8_t offset_z /*! \brief 0x18 Z offset */;
	uint8_t offset_t /*! \brief 0x16 T offset */;
} bma150_t;


#ifdef __cplusplus
}
#endif


#endif /* DEV_BMA150_H_ */


/*! @} */

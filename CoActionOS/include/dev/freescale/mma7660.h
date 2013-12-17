/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MMA7660 MMA7660FC 3-Axis Accelerometer
 *

 *
 * @{
 *
 * \ingroup FREESCALE
 *
 * \details This module is a firmware driver for Freescale Semiconductor's MMA7660 accelerometer.
 * The accelerometer features a 3-axis sensing mode and a sleep mode.
 *
 * Required Peripheral Hardware:
 * - 1 I2C Bus
 *
 * Example:
 *
 * \code
 *
 * #include <stdio.h>
 * #include <unistd.h>
 * #include <fcntl.h>
 * #include <dev/mma7660.h>
 *
 * void read_accel(void){
 * 	int fd;
 * 	char mode;
 * 	mma7660_sample_t sample;
 * 	fd = open("/dev/mma7660-0", O_RDWR);
 * 	if ( fd < 0 ){
 * 		perror("failed to open");
 * 		return;
 * 	}
 *
 * 	lseek(fd, offsetof(mma7660_t, mode), SEEK_SET);
 *	mode = MMA7660_MODE_ACTIVE; //see the datasheet for more info
 *	if ( write(fd, &mode, 1) != 1 ){
 *		perror("failed to set mode");
 *		close(fd);
 *		return;
 *	}
 *
 *	lseek(fd, offsetof(mma7660_t, x), SEEK_SET);
 *	if ( read(fd, &sample, sizeof(sample)) != sizeof(sample) ){
 *		perror("read failed");
 *		return;
 *	}
 *
 *	ioctl(fd, I_MMA7660_CONVERT, &sample); //adjust the sample to signed values
 *	printf("%d, %d, %d\n", sample.x, sample.y, sample.z);
 *	close(fd);
 *
 * }
 *
 * \endcode
 *
 *
 *
 */

/*! \file */
#ifndef DEV_MMA7660_H_
#define DEV_MMA7660_H_

#include <stdint.h>
#include "dev/ioctl.h"


/*! \details This defines the configuration values for
 * a MMA7660 device in the device table.
 *
 * \param device_name The name of the device (e.g "mma7660-0")
 * \param port_number The I2C peripheral port number
 * \param pin_assign_value The GPIO configuration to use with the I2C bus
 * \param i2c_slave_addr The slave address of the MMA7660
 * \param bitrate_value The maximum allowable bitrate for the MMA7660
 * \param mode_value The access mode (usually 0666)
 * \param uid_value The User ID
 * \param gid_value The Group ID
 *
 * Example:
 * \code
 * const device_t devices[DEVICES_TOTAL + 1] = {
 * 	DEVICE_PERIPH("i2c0", i2c, 0, 0666, USER_ROOT, GROUP_ROOT, S_IFCHR),
 * 	... //other peripherals if desired
 * 	MMA7660_DEVICE("mma7660-0", 0, 0, 0x4C, 100000, 0666, USER_ROOT, GROUP_ROOT),
 * 	... //other devices if desired
 * 	DEVICE_TERMINATOR
 *  }
 * \endcode
 * \hideinitializer
 *
 */
#define MMA7660_DEVICE(device_name, port_number, pin_assign_value, i2c_slave_addr, bitrate_value, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFBLK), \
		DEVICE_DRIVER(mma7660), \
		.cfg.periph.port = port_number, \
		.cfg.pin_assign = pin_assign_value, \
		.cfg.pcfg.i2c.slave_addr = i2c_slave_addr, \
		.cfg.bitrate = bitrate_value \
}


#ifdef __cplusplus
extern "C" {
#endif



/*! \brief This represents the register map on the MMA7660.
 *
 * \details This lists the register locations
 * of the MMA7660 when reading and writing the device.
 * See the datasheet for more details.
 */
typedef struct HWPL_PACK {
	uint8_t x /*! \brief X output data */;
	uint8_t y /*! \brief Y output data */;
	uint8_t z /*! \brief Z output data */;
	uint8_t tilt /*! \brief Tilt value */;
	uint8_t srst /*! \brief SRST Register */;
	uint8_t spcnt /*! \brief SPCNT Register */;
	uint8_t intsu /*! \brief INTSU Register */;
	uint8_t mode /*! \brief Mode (active and standby) */;
	uint8_t sr /*! \brief SR register */;
	uint8_t pdet /*! \brief PDET register */;
	uint8_t pd /*! \brief PD register */;
} mma7660_t;

/*! \brief defines a structure containing the output of the device.
 * \details This structure contains the data for the output of
 * the 3 axes: x, y, and z.
 *
 * \code
 * mma7660_sample_t sample;
 * lseek(mma7660_fd, MMA7660_LOC_XOUT, SEEK_SET);
 * read(mma7660_fd, &sample, sizeof(mma7660_sample_t));
 * \endcode
 */
typedef struct HWPL_PACK {
	int8_t x /*! \brief The x acceleration */;
	int8_t y /*! \brief The y acceleration */;
	int8_t z /*! \brief The z acceleration */;
	int8_t tilt /*! \brief The tilt value */;
} mma7660_sample_t;


/*! \details Bit definitions for the MMA7660_LOC_MODE values.
 *
 */
typedef enum {
	MMA7660_MODE_ACTIVE = (1<<0) /*! \brief See datasheet */,
	MMA7660_MODE_TON = (1<<2) /*! \brief See datasheet */,
	MMA7660_MODE_AUTO_WAKE_EN = (1<<3) /*! \brief See datasheet */,
	MMA7660_MODE_AUTO_SLEEP_EN = (1<<4) /*! \brief See datasheet */,
	MMA7660_MODE_PRESCALAR16 = (1<<5) /*! \brief See datasheet */,
	MMA7660_MODE_INT_PUSH_PULL = (1<<6) /*! \brief See datasheet */,
	MMA7660_MODE_INT_ACTIVE_HIGH = (1<<7) /*! \brief See datasheet */
} mma7660_mode_t;

/*! \details This enumerates the valid values for the SR AM settings.
 */
typedef enum {
	MMA7660_SR_AMPD /*! See datasheet */,
	MMA7660_SR_AM64 /*! See datasheet */,
	MMA7660_SR_AM32 /*! See datasheet */,
	MMA7660_SR_AM16 /*! See datasheet */,
	MMA7660_SR_AM8 /*! See datasheet */,
	MMA7660_SR_AM4 /*! See datasheet */,
	MMA7660_SR_AM2 /*! See datasheet */,
	MMA7660_SR_AM1 /*! See datasheet */
} mma7660_sr_am_t;

/*! \details This lists the values for the SR AW settings.
 *
 */
typedef enum{
	MMA7660_SR_AW32 /*! See datasheet */ = (0x00 << 3),
	MMA7660_SR_AW16 /*! \brief See datasheet */ = (0x01 << 3),
	MMA7660_SR_AW8 /*! \brief See datasheet */ = (0x02 << 3),
	MMA7660_SR_AW1 /*! \brief See datasheet */ = (0x03 << 3)
} mma7660_sr_aw_t;

/*! \details This lists the values for the sample rate filter settings.
 *
 */
typedef enum {
	MMA7600_SR_FILT_DISABLE /*! \brief See datasheet */ = (0x00 << 5),
	MMA7600_SR_FILT_2 /*! \brief See datasheet */ = (0x01 << 5),
	MMA7600_SR_FILT_3 /*! \brief See datasheet */ = (0x02 << 5),
	MMA7600_SR_FILT_4 /*! \brief See datasheet */ = (0x03 << 5),
	MMA7600_SR_FILT_5 /*! \brief See datasheet */ = (0x04 << 5),
	MMA7600_SR_FILT_6 /*! \brief See datasheet */ = (0x05 << 5),
	MMA7600_SR_FILT_7 /*! \brief See datasheet */ = (0x06 << 5),
	MMA7600_SR_FILT_8 /*! \brief See datasheet */ = (0x07 << 5),
} mma7660_sr_filt_t;

/*! \cond */
static inline void mma7660_convert(mma7660_sample_t * data) HWPL_ALWAYS_INLINE;
/*! \endcond */

/*! \details This function converts the raw data to signed values.
 * It does not access the hardware in any way.
 *
 */
void mma7660_convert(mma7660_sample_t * data /*! a pointer to the data to convert */){
	data->x <<= 2;
	data->y <<= 2;
	data->z <<= 2;
}


#ifdef __cplusplus
}
#endif

#endif /* DEV_MMA7660_H_ */

/*! @} */

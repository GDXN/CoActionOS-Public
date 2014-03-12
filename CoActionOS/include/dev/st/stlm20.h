/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup STLM20 STLM20, Temperature Sensor
 * @{
 *
 *
 * \ingroup ST
 *
 * \details This is a device driver for the STLM20 temperature sensor.
 *
 * Required Peripheral Hardware:
 * - 1 ADC Channel
 *
 * Example:
 * \code
 * #include <stdio.h>
 * #include <unistd.h>
 * #include <fcntl.h>
 * #include <dev/stlm20.h>
 *
 * void read_accel(void){
 * 	int fd;
 * 	char mode;
 * 	stlm20_attr_t attr;
 * 	stlm20_sample_t sample;
 * 	fd = open("/dev/stlm20-0", O_RDWR);
 * 	if ( fd < 0 ){
 * 		perror("failed to open");
 * 		return;
 * 	}
 *
 * 	//get the attributes
 * 	ioctl(fd, I_STLM20_GETATTR, &attr);
 *
 *	if ( read(fd, &sample, sizeof(sample)) != sizeof(sample) ){
 *		perror("read failed");
 *		return;
 *	}
 *
 *	printf("ADC Sample Value: %d\n", sample.raw);
 *	stlm20_convert(&sample, attr.adc_ref);
 *	printf("Temperature is %0.2f\n", sample.temperature);
 *
 *	close(fd);
 * }
 *
 * \endcode
 *
 *
 */

/*! \file  */

#ifndef DEV_STLM20_H_
#define DEV_STLM20_H_

#include <stdint.h>
#include "dev/ioctl.h"
#include "hwpl/types.h"

#include "dev/adc.h"


#ifdef __cplusplus
extern "C" {
#endif


#define STLM20_IOC_CHAR 's'

/*! \details This defines the configuration values for
 * a STLM20 device in the device table.
 *
 * \param device_name The name of the device (e.g "stlm20-0")
 * \param port_number The SPI peripheral port number
 * \param pin_assign_value The GPIO configuration to use with the I2C bus
 * \param adc_ref_value The ADC reference voltage in millivolts
 * \param adc_channel The ADC channel used to read the device
 * \param mode_value The access mode (usually 0666)
 * \param uid_value The User ID
 * \param gid_value The Group ID
 *
 * \hideinitializer
 *
 */
#define STLM20_DEVICE(device_name, port_number, pin_assign_value, adc_ref_value, adc_channel, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFCHR), \
		DEVICE_DRIVER(stlm20), \
		.cfg.periph.port = port_number, \
		.cfg.pin_assign = pin_assign_value, \
		.cfg.pcfg.adc.reference = adc_ref_value, \
		.cfg.pcfg.adc.channels[0] = adc_channel, \
		.cfg.pcfg.adc.channels[1] = -1, \
		.cfg.pcfg.adc.channels[2] = -1, \
		.cfg.pcfg.adc.channels[3] = -1, \
		.cfg.pcfg.adc.channels[4] = -1, \
		.cfg.pcfg.adc.channels[5] = -1, \
		.cfg.pcfg.adc.channels[6] = -1, \
		.cfg.pcfg.adc.channels[7] = -1 \
}

/*! \brief Device attributes.
 *
 */
typedef struct HWPL_PACK {
	uint32_t adc_ref /*! \brief ADC refernce voltage in millivolts */;
} stlm20_attr_t;

/*! \brief Union for converting ADC to temperature
 * \details This union defines the data needed
 * to convert from an ADC sample to a temperature using
 * \ref stlm20_convert().
 */
typedef union HWPL_PACK {
	adc_sample_t raw /*! \brief The ADC sample value obtained with read() */;
	float temperature /*! \brief The temperature obtained with ioctl() and I_STLM20_CONVERT */;
} stlm20_sample_t;

/*! \details This request gets the device attributes
 *
 */
#define I_STLM20_GETATTR _IOCTLR(STLM20_IOC_CHAR, 0, stlm20_attr_t)

/*! \cond */
static inline int stlm20_convert(stlm20_sample_t * sample, int adc_ref) HWPL_ALWAYS_INLINE;
/*! \endcond */

/*! \details This function converts the raw sample
 * to a temperature.
 *
 */
int stlm20_convert(stlm20_sample_t * sample /*! the raw sample (and the result) */, int adc_ref /*! ADC reference voltage in millivolts */){
	float voltage;
	voltage = ( ( sample->raw * adc_ref * 1.0 ) / ADC_MAX );
	//Convert the voltage to temperature
	sample->temperature = (1866.3 - voltage) / ( 11.69 );
	return 0;
}

#ifdef __cplusplus
}
#endif


#endif /* DEV_STLM20_H_ */


/*! @} */

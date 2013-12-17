/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup AD73XX AD73XX Dual Simultaneous Analog to Digital Converter (beta)
 * @{
 *
 * \ingroup ANALOG
 *
 * \details The AD73XX from Analog Devices is a dual analog to digital converter which simultaneously
 * samples and provides the data through a SPI bus.  Here is a small code example:
 * \code
 * #include <unistd.h>
 * #include <fcntl.h>
 * #include <dev/ad73xx.h>
 * ...
 * int fd;
 * ad73xx_sample_t samples[16];
 * int i;
 * fd = open("/dev/ad73xx-0.h", O_RDWR);
 * if ( fd < 0 ){
 * 	perror("failed to open");
 * }
 *
 * if ( read(fd, samples, 16*sizeof(ad73xx_sample_t)) < 0 ){
 * 	perror("failed to read");
 * }
 *
 * for(i=0; i<16; i++){
 * 	ad73xx_convert(&(samples[i]));
 * }
 *
 *	//valid data is now at samples.chans[0] and samples.chans[1]
 *
 * close(fd);
 *
 * \endcode
 *
 */

/*! \file */

#ifndef DEV_AD73XX_H_
#define DEV_AD73XX_H_

#include <stdint.h>
#include "dev/spi.h"
#include "hwpl/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*! \brief This is used to declare a value in the device list.
 *
 * \details This defines the configuration values for
 * a AD73XX device in the device table.
 *
 * The SPI device is set to mode 3.
 *
 *
 * \param device_name The name of the device (e.g "ad73xx-0")
 * \param port_number The SPI peripheral port number
 * \param pin_assign_value The GPIO configuration to use with the SPI bus
 * \param cs_port_value The GPIO port used for the chip select
 * \param cs_pin_value The GPIO pin used for the chip select
 * \param bitrate_value The maximum allowable bitrate for the AD73XX
 * \param state_var A pointer to a ad73xx_state_t variable
 * \param mode_value The access mode (usually 0666)
 * \param uid_value The User ID
 * \param gid_value The Group ID
 *
 * \hideinitializer
 *
 */
#define AD73XX_DEVICE(device_name, port_number, pin_assign_value, cs_port_value, cs_pin_value, bitrate_value, state_var, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFBLK), \
		DEVICE_DRIVER(ad73xx), \
		.cfg.periph.port = port_number, \
		.cfg.pin_assign = pin_assign_value, \
		.cfg.pcfg.spi.mode = SPI_ATTR_MODE2, \
		.cfg.pcfg.spi.cs.port = cs_port_value, \
		.cfg.pcfg.spi.cs.pin = cs_pin_value, \
		.cfg.pcfg.spi.width = 8, \
		.cfg.pcfg.spi.format = SPI_ATTR_FORMAT_SPI, \
		.cfg.bitrate = bitrate_value, \
		.cfg.state = state_var, \
		.cfg.dcfg = NULL \
}

/* \brief The maximum value the ADC can be
 */
#define AD73XX_MAX (16384)

/*! \brief The resolution value; one step is this value.
 */
#define AD73XX_RES (4)

/*! \details This lists the channels for the ADC converter.
 *
 */
typedef enum {
	AD73XX_CHAN_A /*! \brief Channel A when indexing ad73xx_sample_t.chans */,
	AD73XX_CHAN_B /*! \brief Channel B when indexing ad73xx_sample_t.chans */
} ad73xx_chan_t;


/*! \brief Defines a union for reading both channels
 * of the device.
 * \details This union defines a memory structure used
 * to read both channels of the device.
 */
typedef union {
	uint32_t raw /*! \brief The raw value */;
	int16_t chans[2] /*! \brief Index 0 is A and Index 1 is B */;
	uint8_t craw[4] /*! \brief raw in char format */;
} ad73xx_sample_t;



/*! \details This function converts a raw sample to 16-bit values that
 * are stored in the \a chans member.
 */
static inline void ad73xx_convert(ad73xx_sample_t * value) HWPL_ALWAYS_INLINE;

/*! \cond */
static inline void ad73xx_convert(ad73xx_sample_t * value){
	uint8_t tmp;
	tmp = value->craw[0];
	value->craw[0] = value->craw[1];
	value->craw[1] = tmp;

	tmp = value->craw[2];
	value->craw[2] = value->craw[3];
	value->craw[3] = tmp;
}
/*! \endcond */

#ifdef __cplusplus
}
#endif

#endif //DEV_AD73XX_H_

/*! @} */

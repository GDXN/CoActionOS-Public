/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup AT25DF AT25DF Serial Flash (alpha)
 *
 *
 * @{
 *
 * \ingroup ATMEL
 *
 * \details The AT25DF data flash driver reads and writes data to Atmel's data flash chips
 * using the SPI interface.
 *
 * This driver is not fully implemented in version this version.
 *
 *

 */

/*! \file */

#ifndef DEV_AT25DF_H_
#define DEV_AT25DF_H_

#include "disk.h"


/*! \details This defines the configuration values for
 * a AT25DF device in the device table.
 *
 *
 * \param device_name The name of the device (e.g "disk0")
 * \param port_number The SPI peripheral port number
 * \param pin_assign_value The GPIO configuration to use with the SPI bus
 * \param cs_port_value The GPIO port used for the chip select
 * \param cs_pin_value The GPIO pin used for the chip select
 * \param bitrate_value The maximum allowable bitrate for the SST25VF
 * \param cfg_ptr A pointer to the configuration structure (const sst25vf_cfg_t *)
 * \param state_ptr A pointer to the state structure (sst25vf_state_t *)
 * \param mode_value The access mode (usually 0666)
 * \param uid_value The User ID
 * \param gid_value The Group ID
 *
 * \hideinitializer
 *
 */
#define AT25DF_DEVICE(device_name, port_number, pin_assign_value, cs_port_value, cs_pin_value, bitrate_value, cfg_ptr, state_ptr, mode_value, uid_value, gid_value) { \
		.name = device_name, \
		DEVICE_MODE(mode_value, uid_value, gid_value, S_IFBLK), \
		DEVICE_DRIVER(sst25vf), \
		.cfg.periph.port = port_number, \
		.cfg.pin_assign = pin_assign_value, \
		.cfg.pcfg.spi.mode = SPI_ATTR_MODE0, \
		.cfg.pcfg.spi.cs.port = cs_port_value, \
		.cfg.pcfg.spi.cs.pin = cs_pin_value, \
		.cfg.pcfg.spi.width = 8, \
		.cfg.pcfg.spi.format = SPI_ATTR_FORMAT_SPI, \
		.cfg.bitrate = bitrate_value, \
		.cfg.state = state_ptr, \
		.cfg.dcfg = cfg_ptr \
}

/*! \details This defines the configuration values for the at25df_cfg_t
 * configuration data.
 *
 * \param hold_port The GPIO port used for the hold (-1 to ignore)
 * \param hold_pin The GPIO pin used for the hold
 * \param wp_port The GPIO port used for the write protect (-1 to ignore)
 * \param wp_pin The GPIO pin used for the write protect
 * \param miso_port The GPIO port used for the MISO
 * \param miso_pin The GPIO pin used for the MISO
 * \param device_size The number of bytes on the device
 * \hideinitializer
 */
#define AT25DF_DEVICE_CFG(hold_port, hold_pin, wp_port, wp_pin, miso_port, miso_pin, device_size) { \
		.hold.port = hold_port, \
		.hold.pin = hold_pin, \
		.wp.port = wp_port, \
		.wp.pin = wp_pin, \
		.miso.port = miso_port, \
		.miso.pin = miso_pin, \
		.size = (device_size) \
}


/*! \details This defines the device specific static configuration.
 *
 */
typedef struct {
	device_gpio_t hold /*! \brief Hold Pin */;
	device_gpio_t wp /*! \brief Write protect pin */;
	device_gpio_t miso /*! \brief The write complete pin */;
	uint32_t size /*! \brief The size of the memory on the device */;
} at25df_cfg_t;



#ifdef __cplusplus
}
#endif



#endif /* DEV_AT25DF_H_ */


/*! @} */

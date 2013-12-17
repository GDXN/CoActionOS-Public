/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup PIO_DEV PIO
 *
 * \ingroup DEV
 *
 */

#ifndef _HWPL_PIO_H_
#define _HWPL_PIO_H_

#include "core.h"
#include "../device.h"
#include "../dev/pio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_pio_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_pio_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_pio_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_pio_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_pio_close(const device_cfg_t * cfg);

int hwpl_pio_getattr(int port, void * ctl);
int hwpl_pio_setattr(int port, void * ctl);
int hwpl_pio_setaction(int port, void * ctl);

int hwpl_pio_setmask(int port, void * ctl);
int hwpl_pio_clrmask(int port, void * ctl);
int hwpl_pio_get(int port, void * ctl);
int hwpl_pio_set(int port, void * ctl);


#ifndef __link

static inline int _hwpl_i2c_cfg_pio(int port, int pio_port, int scl_pin, int sda_pin) HWPL_ALWAYS_INLINE;
int _hwpl_i2c_cfg_pio(int port, int pio_port, int scl_pin, int sda_pin){
	if ( _hwpl_core_set_pinsel_func(pio_port, sda_pin, CORE_PERIPH_I2C, port) ) return -1;  //Use pins for I2C
	if ( _hwpl_core_set_pinsel_func(pio_port, scl_pin, CORE_PERIPH_I2C, port) ) return -1;  //Use pins for I2C
	return 0;
}


static inline int _hwpl_uart_cfg_pio(int port, int pio_port, int rx_pin, int tx_pin) HWPL_ALWAYS_INLINE;
int _hwpl_uart_cfg_pio(int port, int pio_port, int rx_pin, int tx_pin){
	if ( _hwpl_core_set_pinsel_func(pio_port, rx_pin, CORE_PERIPH_UART, port) ) return -1;  //Use pin for UART
	if ( _hwpl_core_set_pinsel_func(pio_port, tx_pin, CORE_PERIPH_UART, port) ) return -1;  //Use pin for UART
	return 0;
}

static inline int _hwpl_spi_cfg_pio(int port, int pio_port, int mosi_pin, int miso_pin, int sck_pin) HWPL_ALWAYS_INLINE;
int _hwpl_spi_cfg_pio(int port, int pio_port, int mosi_pin, int miso_pin, int sck_pin){
	if ( _hwpl_core_set_pinsel_func(pio_port, mosi_pin, CORE_PERIPH_SPI, port) ) return -1;  //Use pin for SPI
	if ( _hwpl_core_set_pinsel_func(pio_port, miso_pin, CORE_PERIPH_SPI, port) ) return -1;  //Use pin for SPI
	if ( _hwpl_core_set_pinsel_func(pio_port, sck_pin, CORE_PERIPH_SPI, port) ) return -1;  //Use pin for SPI
	return 0;
}

static inline int _hwpl_usb_cfg_pio(int usb_port,
		int data_pio_port,
		int datap_pin,
		int datam_pin,
		int vbus_pio_port,
		int vbus_pio_pin) HWPL_ALWAYS_INLINE;

int _hwpl_usb_cfg_pio(int port,
		int data_pio_port,
		int datap_pin,
		int datam_pin,
		int vbus_pio_port,
		int vbus_pio_pin){
	if ( _hwpl_core_set_pinsel_func(data_pio_port, datap_pin, CORE_PERIPH_USB, port) ) return -1;  //Use pin for USB
	if ( _hwpl_core_set_pinsel_func(data_pio_port, datam_pin, CORE_PERIPH_USB, port) ) return -1;  //Use pin for USB
	if ( _hwpl_core_set_pinsel_func(vbus_pio_port, vbus_pio_pin, CORE_PERIPH_USB, port) ) return -1;  //Use pin for USB
	return 0;
}

#endif

#ifdef __cplusplus
}
#endif



#endif /* _HWPL_PIO_H_ */

/*! @} */

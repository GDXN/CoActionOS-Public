/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup GPIO_DEV GPIO
 *
 * \ingroup DEV
 *
 */

#ifndef _HWPL_GPIO_H_
#define _HWPL_GPIO_H_

#include "hwpl/core.h"
#include "../device.h"
#include "../dev/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 * \sa periph_open()
 *
 */
int hwpl_gpio_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_gpio_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_gpio_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_gpio_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_gpio_close(const device_cfg_t * cfg);

int hwpl_gpio_getattr(int port, void * ctl);
int hwpl_gpio_setattr(int port, void * ctl);
int hwpl_gpio_setaction(int port, void * ctl);

int hwpl_gpio_setmask(int port, void * ctl);
int hwpl_gpio_clrmask(int port, void * ctl);
int hwpl_gpio_get(int port, void * ctl);
int hwpl_gpio_set(int port, void * ctl);
int hwpl_gpio_setpinmode(int port, void * ctl);


#ifndef __link

void hwpl_gpio_set_pinmode(int port, int pin, int mode, int open_drain);
void hwpl_gpio_poweron(int port);

static inline int i2c_cfg_gpio(int port, int gpio_port, int scl_pin, int sda_pin) HWPL_ALWAYS_INLINE;
int i2c_cfg_gpio(int port, int gpio_port, int scl_pin, int sda_pin){
	if ( _hwpl_core_set_pinsel_func(gpio_port, sda_pin, CORE_PERIPH_I2C, port) ) return -1;  //Use pins for I2C
	if ( _hwpl_core_set_pinsel_func(gpio_port, scl_pin, CORE_PERIPH_I2C, port) ) return -1;  //Use pins for I2C
	return 0;
}


static inline int uart_cfg_gpio(int port, int gpio_port, int rx_pin, int tx_pin) HWPL_ALWAYS_INLINE;
int uart_cfg_gpio(int port, int gpio_port, int rx_pin, int tx_pin){
	if ( _hwpl_core_set_pinsel_func(gpio_port, rx_pin, CORE_PERIPH_UART, port) ) return -1;  //Use pin for UART
	if ( _hwpl_core_set_pinsel_func(gpio_port, tx_pin, CORE_PERIPH_UART, port) ) return -1;  //Use pin for UART
	return 0;
}

static inline int spi_cfg_gpio(int port, int gpio_port, int mosi_pin, int miso_pin, int sck_pin) HWPL_ALWAYS_INLINE;
int spi_cfg_gpio(int port, int gpio_port, int mosi_pin, int miso_pin, int sck_pin){
	if ( _hwpl_core_set_pinsel_func(gpio_port, mosi_pin, CORE_PERIPH_SPI, port) ) return -1;  //Use pin for SPI
	if ( _hwpl_core_set_pinsel_func(gpio_port, miso_pin, CORE_PERIPH_SPI, port) ) return -1;  //Use pin for SPI
	if ( _hwpl_core_set_pinsel_func(gpio_port, sck_pin, CORE_PERIPH_SPI, port) ) return -1;  //Use pin for SPI
	return 0;
}

static inline int usb_cfg_gpio(int usb_port,
		int data_gpio_port,
		int datap_pin,
		int datam_pin,
		int vbus_gpio_port,
		int vbus_gpio_pin) HWPL_ALWAYS_INLINE;

int usb_cfg_gpio(int port,
		int data_gpio_port,
		int datap_pin,
		int datam_pin,
		int vbus_gpio_port,
		int vbus_gpio_pin){
	if ( _hwpl_core_set_pinsel_func(data_gpio_port, datap_pin, CORE_PERIPH_USB, port) ) return -1;  //Use pin for USB
	if ( _hwpl_core_set_pinsel_func(data_gpio_port, datam_pin, CORE_PERIPH_USB, port) ) return -1;  //Use pin for USB
	if ( _hwpl_core_set_pinsel_func(vbus_gpio_port, vbus_gpio_pin, CORE_PERIPH_USB, port) ) return -1;  //Use pin for USB
	return 0;
}

#endif

#ifdef __cplusplus
}
#endif



#endif /* _HWPL_GPIO_H_ */

/*! @} */

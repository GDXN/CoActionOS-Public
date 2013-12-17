/*
 * gpio.h
 *
 *  Created on: Sep 26, 2012
 *      Author: tgil
 */

#ifndef APPLIB_GPIO_H_
#define APPLIB_GPIO_H_

#include <dev/gpio.h>
#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

int gpio_open(port_t port);
int gpio_close(port_t port);
int gpio_getattr(port_t port, gpio_attr_t * attr);
int gpio_setattr(port_t port, gpio_attr_t * attr);
int gpio_setmask(port_t port, gpio_sample_t mask);
int gpio_clrmask(port_t port, gpio_sample_t mask);
gpio_sample_t gpio_get(port_t port);
int gpio_set(port_t port, gpio_sample_t value);

#ifdef __cplusplus
}
#endif

#endif /* APPLIB_GPIO_H_ */

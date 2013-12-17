/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef GPIO_FLAGS_H_
#define GPIO_FLAGS_H_

#include <stdint.h>
#include "hwpl/gpio.h"

#define DECLARE_REGS(regs, port) GPIO_TypeDef * regs = hwpl_gpio_regs[port]

extern GPIO_TypeDef * const hwpl_gpio_regs[HWPL_GPIO_PORTS];

void hwpl_gpio_poweron(int port);

#define MODE_CNF_OUTPUT_PUSHPULL_50MHZ (0x3)
#define MODE_CNF_OUTPUT_PUSHPULL_10MHZ (0x1)
#define MODE_CNF_OUTPUT_PUSHPULL_2MHZ (0x2)
#define MODE_CNF_ALTFUNCTION_PUSHPULL_50MHZ (0xB)
#define MODE_CNF_ALTFUNCTION_PUSHPULL_10MHZ (0x9)
#define MODE_CNF_ALTFUNCTION_PUSHPULL_2MHZ (0xA)
#define MODE_CNF_INPUT_FLOATING (0x4)
#define MODE_CNF_INPUT_PULLUP_DOWN (0x8)
#define MODE_CNF_INPUT_ANALOG (0x0)

static void configure_gpio_low(GPIO_TypeDef * reg, uint8_t pin, uint8_t mode_cnf) HWPL_ALWAYS_INLINE;
void configure_gpio_low(GPIO_TypeDef * reg, uint8_t pin, uint8_t mode_cnf){
	reg->CRL &= ~(0x0F<<(pin*4));
	reg->CRL |= (mode_cnf<<(pin*4));
}

static void configure_gpio_high(GPIO_TypeDef * reg, uint8_t pin, uint8_t mode_cnf) HWPL_ALWAYS_INLINE;
void configure_gpio_high(GPIO_TypeDef * reg, uint8_t pin, uint8_t mode_cnf){
	reg->CRH &= ~(0x0F<<((pin-8)*4));
	reg->CRH |= (mode_cnf<<((pin-8)*4));
}

static void configure_gpio_low_pair(GPIO_TypeDef * reg, uint8_t pin0, uint8_t mode_cnf0, uint8_t pin1, uint8_t mode_cnf1) HWPL_ALWAYS_INLINE;
void configure_gpio_low_pair(GPIO_TypeDef * reg, uint8_t pin0, uint8_t mode_cnf0, uint8_t pin1, uint8_t mode_cnf1){
	reg->CRL &= ~((0x0F<<(pin0*4))|((0x0F<<(pin1*4))));
	reg->CRL |= ((mode_cnf0<<(pin0*4))|(mode_cnf1<<(pin1*4)));
}

static void configure_gpio_high_pair(GPIO_TypeDef * reg, uint8_t pin0, uint8_t mode_cnf0, uint8_t pin1, uint8_t mode_cnf1) HWPL_ALWAYS_INLINE;
void configure_gpio_high_pair(GPIO_TypeDef * reg, uint8_t pin0, uint8_t mode_cnf0, uint8_t pin1, uint8_t mode_cnf1){
	reg->CRH &=  ~((0x0F<<((pin0-8)*4))|((0x0F<<((pin1-8)*4))));
	reg->CRH |= ((mode_cnf0<<((pin0-8)*4))|(mode_cnf1<<((pin1-8)*4)));
}

static void configure_gpio_low_trio(GPIO_TypeDef * reg, uint8_t pin0, uint8_t mode_cnf0, uint8_t pin1, uint8_t mode_cnf1, uint8_t pin2, uint8_t mode_cnf2) HWPL_ALWAYS_INLINE;
void configure_gpio_low_trio(GPIO_TypeDef * reg, uint8_t pin0, uint8_t mode_cnf0, uint8_t pin1, uint8_t mode_cnf1, uint8_t pin2, uint8_t mode_cnf2){
	reg->CRL &= ~((0x0F<<(pin0*4))|((0x0F<<(pin1*4)))|((0x0F<<(pin2*4))));
	reg->CRL |= ((mode_cnf0<<(pin0*4))|(mode_cnf1<<(pin1*4))|(mode_cnf2<<(pin2*4)));
}

static void configure_gpio_high_trio(GPIO_TypeDef * reg, uint8_t pin0, uint8_t mode_cnf0, uint8_t pin1, uint8_t mode_cnf1, uint8_t pin2, uint8_t mode_cnf2) HWPL_ALWAYS_INLINE;
void configure_gpio_high_trio(GPIO_TypeDef * reg, uint8_t pin0, uint8_t mode_cnf0, uint8_t pin1, uint8_t mode_cnf1, uint8_t pin2, uint8_t mode_cnf2){
	reg->CRH &= ~((0x0F<<(pin0*4))|((0x0F<<(pin1*4)))|((0x0F<<(pin2*4))));
	reg->CRH |= ((mode_cnf0<<(pin0*4))|(mode_cnf1<<(pin1*4))|(mode_cnf2<<(pin2*4)));
}




#endif /* GPIO_FLAGS_H_ */

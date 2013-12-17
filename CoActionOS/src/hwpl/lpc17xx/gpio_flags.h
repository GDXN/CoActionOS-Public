/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef GPIO_FLAGS_H_
#define GPIO_FLAGS_H_

static inline void gpio_set(int port, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_clr(int port, int mask) HWPL_ALWAYS_INLINE;
static inline int gpio_tst(int port, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_toggle(int port, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_wr(int port, int value) HWPL_ALWAYS_INLINE;
static inline int gpio_rd(int port) HWPL_ALWAYS_INLINE;
static inline void gpio_set_dir(int port, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_clr_dir(int port, int mask) HWPL_ALWAYS_INLINE;
static inline uint32_t gpio_get_dir(int port) HWPL_ALWAYS_INLINE;
static inline void gpio_wr_dir(int port, int value) HWPL_ALWAYS_INLINE;
static inline void gpio_toggle_dir(int port, int mask) HWPL_ALWAYS_INLINE;

void gpio_set(int port, int mask){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].SET=mask;
}

void gpio_clr(int port, int mask){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].CLR=mask;
}

int gpio_tst(int port, int mask){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	return (gpio_regs[port].PIN) & mask;
}

void gpio_toggle(int port, int mask){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].PIN ^= mask;
}

void gpio_wr(int port, int value){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].PIN=value;
}

int gpio_rd(int port){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	return gpio_regs[port].PIN;
}

uint32_t gpio_get_dir(int port){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	return gpio_regs[port].DIR;
}

void gpio_set_dir(int port, int mask){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].DIR|=mask;
}

void gpio_clr_dir(int port, int mask){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].DIR&=(~mask);
}

void gpio_wr_dir(int port, int value){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].DIR=value;
}

void gpio_toggle_dir(int port, int mask){
	LPC_GPIO_TypeDef * gpio_regs = (LPC_GPIO_TypeDef *)LPC_GPIO0_BASE;
	gpio_regs[port].DIR ^= mask;
}


#endif /* GPIO_FLAGS_H_ */

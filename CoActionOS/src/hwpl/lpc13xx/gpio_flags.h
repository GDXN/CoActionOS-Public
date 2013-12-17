/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef GPIO_FLAGS_H_
#define GPIO_FLAGS_H_

static inline void gpio_init(void) HWPL_ALWAYS_INLINE;
static inline void gpio_set(int p, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_clr(int p, int mask) HWPL_ALWAYS_INLINE;
static inline int gpio_tst(int p, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_toggle(int p, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_set_dir(int p, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_clr_dir(int p, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_toggle_dir(int p, int mask) HWPL_ALWAYS_INLINE;
static inline int gpio_tst_dir(int p, int mask) HWPL_ALWAYS_INLINE;
static inline void gpio_wr(int p, int value) HWPL_ALWAYS_INLINE;
static inline int gpio_rd(int p) HWPL_ALWAYS_INLINE;

void gpio_init(void){
	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_GPIO;
}

void gpio_set(int p, int mask){
	switch(p){
	case 0:
		LPC_GPIO0->MASKED_ACCESS[mask] = (mask);
		break;
	case 1:
		LPC_GPIO1->MASKED_ACCESS[mask] = (mask);
		break;
	case 2:
		LPC_GPIO2->MASKED_ACCESS[mask] = (mask);
		break;
	case 3:
		LPC_GPIO3->MASKED_ACCESS[mask] = (mask);
		break;
	}
}

void gpio_clr(int p, int mask){
	switch(p){
	case 0:
		LPC_GPIO0->MASKED_ACCESS[mask] = 0;
		break;
	case 1:
		LPC_GPIO1->MASKED_ACCESS[mask] = 0;
		break;
	case 2:
		LPC_GPIO2->MASKED_ACCESS[mask] = 0;
		break;
	case 3:
		LPC_GPIO3->MASKED_ACCESS[mask] = 0;
		break;
	}
}

int gpio_tst(int p, int mask){
	switch(p){
	case 0:
		return LPC_GPIO0->MASKED_ACCESS[mask];
		break;
	case 1:
		return LPC_GPIO1->MASKED_ACCESS[mask];
		break;
	case 2:
		return LPC_GPIO2->MASKED_ACCESS[mask];
		break;
	case 3:
		return LPC_GPIO3->MASKED_ACCESS[mask];
		break;
	}
	return 0;
}

void gpio_toggle(int p, int mask){
	switch(p){
	case 0:
		LPC_GPIO0->MASKED_ACCESS[mask] ^= (mask);
		break;
	case 1:
		LPC_GPIO1->MASKED_ACCESS[mask] ^= (mask);
		break;
	case 2:
		LPC_GPIO2->MASKED_ACCESS[mask] ^= (mask);
		break;
	case 3:
		LPC_GPIO3->MASKED_ACCESS[mask] ^= (mask);
		break;
	}
}

void gpio_set_dir(int p, int mask){
	switch(p){
	case 0:
		LPC_GPIO0->DIR |= (mask);
		break;
	case 1:
		LPC_GPIO1->DIR |= (mask);
		break;
	case 2:
		LPC_GPIO2->DIR |= (mask);
		break;
	case 3:
		LPC_GPIO3->DIR |= (mask);
		break;
	}
}


void gpio_clr_dir(int p, int mask){
	switch(p){
	case 0:
		LPC_GPIO0->DIR &= ~(mask);
		break;
	case 1:
		LPC_GPIO1->DIR &= ~(mask);
		break;
	case 2:
		LPC_GPIO2->DIR &= ~(mask);
		break;
	case 3:
		LPC_GPIO3->DIR &= ~(mask);
		break;
	}
}

void gpio_toggle_dir(int p, int mask){
	switch(p){
	case 0:
		LPC_GPIO0->DIR ^= (mask);
		break;
	case 1:
		LPC_GPIO1->DIR ^= (mask);
		break;
	case 2:
		LPC_GPIO2->DIR ^= (mask);
		break;
	case 3:
		LPC_GPIO3->DIR ^= (mask);
		break;
	}
}


int gpio_tst_dir(int p, int mask){
	switch(p){
	case 0:
		return LPC_GPIO0->DIR & (mask);
	case 1:
		return LPC_GPIO1->DIR & (mask);
	case 2:
		return LPC_GPIO2->DIR & (mask);
	case 3:
		return LPC_GPIO3->DIR & (mask);
	}
}

void gpio_wr(int p, int value){
	switch(p){
	case 0:
		LPC_GPIO0->DATA = (value);
		break;
	case 1:
		LPC_GPIO1->DATA = (value);
		break;
	case 2:
		LPC_GPIO2->DATA = (value);
		break;
	case 3:
		LPC_GPIO3->DATA = (value);
		break;
	}
}

int gpio_rd(int p){
	switch(p){
	case 0:
		return LPC_GPIO0->DATA;
	case 1:
		return LPC_GPIO1->DATA;
	case 2:
		return LPC_GPIO2->DATA;
	case 3:
		return LPC_GPIO3->DATA;
	}
}
/*! \endcond */



#endif /* GPIO_FLAGS_H_ */

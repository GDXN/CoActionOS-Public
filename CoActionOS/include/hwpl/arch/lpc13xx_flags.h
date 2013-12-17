/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup LPC13XXDEV
 * @{
 *
 *
 */

/*! \file */

#ifndef LPC13XX_FLAGS_H_
#define LPC13XX_FLAGS_H_

#include "hwpl/types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	PCSYS = 0,
	PCROM = 1,
	PCRAM = 2,
	PCFLASHREG = 3,
	PCFLASHARRAY = 4,
	PCI2C = 5,
	PCGPIO = 6,
	PCCT16B0 = 7,
	PCCT16B1 = 8,
	PCCT32B0 = 9,
	PCCT32B1 = 10,
	PCSSP = 11,
	PCSSP0 = 11,
	PCUART = 12,
	PCADC = 13,
	PCUSB_REG = 14,
	PCWDT = 15,
	PCIOCON = 16,
	PCSSP1 = 18,
	PC_TOTAL
};

static inline void _hwpl_lpc_core_enable_pwr(int port) HWPL_ALWAYS_INLINE;
static inline void _hwpl_lpc_core_enable_pwr_mask(int mask) HWPL_ALWAYS_INLINE;
static inline void _hwpl_lpc_core_disable_pwr(int port) HWPL_ALWAYS_INLINE;
static inline void _hwpl_lpc_core_disable_pwr_mask(int mask) HWPL_ALWAYS_INLINE;
static inline int _hwpl_lpc_core_pwr_enabled(int periph) HWPL_ALWAYS_INLINE;


void _hwpl_lpc_core_enable_pwr(int port){ HWPL_SET_BIT(LPC_SYSCON->SYSAHBCLKCTRL, port); }
void _hwpl_lpc_core_enable_pwr_mask(int mask){ HWPL_SET_MASK(LPC_SYSCON->SYSAHBCLKCTRL, mask); }
void _hwpl_lpc_core_disable_pwr(int port){ HWPL_CLR_BIT(LPC_SYSCON->SYSAHBCLKCTRL, port); }
void _hwpl_lpc_core_disable_pwr_mask(int mask){ HWPL_CLR_MASK(LPC_SYSCON->SYSAHBCLKCTRL, mask); }
int _hwpl_lpc_core_pwr_enabled(int periph){ return (HWPL_TEST_BIT(LPC_SYSCON->SYSAHBCLKCTRL, periph)); }


#define CORE_PINMODE_MASK ( (1<<4)|(1<<3)|(1<<5) )

void core_wr_pinmode(int port, int pin, int value);

#ifdef __cplusplus
}
#endif


#endif /* LPC13XX_FLAGS_H_ */

/*! @} */

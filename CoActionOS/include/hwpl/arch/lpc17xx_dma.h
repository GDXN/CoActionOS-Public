/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup DMA LPC17XX Direct Memory Access
 * @{
 *
 * \ingroup LPC17XXDEV
 *
 * \details The Direct memory access allows for direct memory transfers from memory to memory or memory to/from
 * peripherals.
 *
 *
 */

/*! \file */

#ifndef _HWPL_ARCH_LPC17XX_DMA_H_
#define _HWPL_ARCH_LPC17XX_DMA_H_



#define DMA_STATIC

#include "hwpl/dma.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DMA_MAX_CHANNEL 8
#define DMA_MAX_TRANSFER_SIZE 4095


enum {
	DMA_CTRL_BURST_SIZE1,
	DMA_CTRL_BURST_SIZE4,
	DMA_CTRL_BURST_SIZE8,
	DMA_CTRL_BURST_SIZE16,
	DMA_CTRL_BURST_SIZE32,
	DMA_CTRL_BURST_SIZE64,
	DMA_CTRL_BURST_SIZE128,
	DMA_CTRL_BURST_SIZE256
};

#define DMA_CTRL_TRANSFER_SIZE(x) (x & 0x0FFF)
#define DMA_CTRL_SRC_BURST_SIZE(x) ( (x & 0x07) << 12)
#define DMA_CTRL_DEST_BURST_SIZE(x) ( (x & 0x07) << 15)


#define DMA_CTRL_SRC_WIDTH_MASK (3<<18)
#define DMA_CTRL_SRC_WIDTH8 (0 << 18)
#define DMA_CTRL_SRC_WIDTH16 (1 << 18)
#define DMA_CTRL_SRC_WIDTH32 (2 << 18)
#define DMA_CTRL_DEST_WIDTH8 (0 << 21)
#define DMA_CTRL_DEST_WIDTH16 (1 << 21)
#define DMA_CTRL_DEST_WIDTH32 (2 << 21)
#define DMA_CTRL_SRC_INC (1<<26)
#define DMA_CTRL_DEST_INC (1<<27)
#define DMA_CTRL_ENABLE_TERMINAL_COUNT_INT (1<<31)

#define DMA_CFG_ENABLE_CHAN (1<<0)
#define DMA_CFG_SRC_PERIPH(x) ( (x & 0x0F) << 1)
#define DMA_CFG_DEST_PERIPH(x) ( (x & 0x0F) << 6)
#define DMA_CFG_TRANSFER_TYPE(x) (x<<11)
#define DMA_CFG_ENABLE_IE (1<<14)
#define DMA_CFG_ENABLE_ITC (1<<15)
#define DMA_CFG_HALT (1<<18)

#define DMA_CFG_ENABLE 0
#define DMA_CFG_BIT_ENDIAN 1

//Read only
#define DMA_CFG_ACTIVE (1<<17)


#define DMA_HIGH_PRIORITY 7
#define DMA_LOW_PRIORITY 3


#define DMA_REQ_SSP0_TX	0
#define DMA_REQ_SSP0_RX	1
#define DMA_REQ_SSP1_TX	2
#define DMA_REQ_SSP1_RX	3
#define DMA_REQ_ADC 4
#define DMA_REQ_I2S0 5
#define DMA_REQ_I2S1 6
#define DMA_REQ_DAC 7
#define DMA_REQ_UART0_TX 8
#define DMA_REQ_MAT00 8
#define DMA_REQ_UART0_RX 9
#define DMA_REQ_MAT01 9
#define DMA_REQ_UART1_TX 10
#define DMA_REQ_MAT10 10
#define DMA_REQ_UART1_RX 11
#define DMA_REQ_MAT11 11
#define DMA_REQ_UART2_TX 12
#define DMA_REQ_MAT20 12
#define DMA_REQ_UART2_RX 13
#define DMA_REQ_MAT21 13
#define DMA_REQ_UART3_TX 14
#define DMA_REQ_MAT30 14
#define DMA_REQ_UART3_RX 15
#define DMA_REQ_MAT31 15


#ifdef __cplusplus
}
#endif


#endif /* _HWPL_ARCH_LPC17XX_DMA_H_ */

/*! @} */

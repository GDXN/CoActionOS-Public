/*
 * ssp_dev.c
 *
 *  Created on: Nov 2, 2013
 *      Author: tgil
 */


/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */
#include <errno.h>
#include <fcntl.h>
#include "hwpl/spi.h"
#include "hwpl/pio.h"
#include "ssp_flags.h"
#include "ssp_flags.h"
#include "hwpl/core.h"

#include "hwpl/debug.h"

typedef struct {
	char * volatile rx_buf;
	char * volatile tx_buf;
	volatile int size;
	void * duplex_mem;
	int ret;
	uint8_t pin_assign;
	uint8_t width;
	uint8_t ref_count;
	hwpl_callback_t callback;
	void * context;
} ssp_local_t;

static ssp_local_t ssp_local[HWPL_SSP_PORTS] HWPL_SYS_MEM;

static int ssp_port_transfer(int port, int is_read, device_transfer_t * dop);
static void ssp_fill_tx_fifo(int port);
static void ssp_empty_rx_fifo(int port);
static int byte_swap(int port, int byte);

LPC_SSP_TypeDef * const ssp_regs_table[HWPL_SSP_PORTS] = HWPL_SSP_REGS;

void _hwpl_ssp_dev_power_on(int port){
	if ( ssp_local[port].ref_count == 0 ){
		switch(port){
		case 0:
			_hwpl_lpc_core_enable_pwr(PCSSP0);
			_hwpl_core_priv_enable_irq((void*)SSP0_IRQn);
			break;
		case 1:
			_hwpl_lpc_core_enable_pwr(PCSSP1);
			_hwpl_core_priv_enable_irq((void*)SSP1_IRQn);
			break;
#ifdef __lpc177x_8x
		case 2:
			_hwpl_lpc_core_enable_pwr(PCSSP2);
			_hwpl_core_priv_enable_irq((void*)SSP2_IRQn);
			break;
#endif
		}
		ssp_local[port].duplex_mem = NULL;
		ssp_local[port].callback = NULL;
	}
	ssp_local[port].ref_count++;

}

void _hwpl_ssp_dev_power_off(int port){
	if ( ssp_local[port].ref_count > 0 ){
		if ( ssp_local[port].ref_count == 1 ){
			switch(port){

			case 0:
				_hwpl_core_priv_disable_irq((void*)SSP0_IRQn);
				_hwpl_lpc_core_disable_pwr(PCSSP0);
				break;
			case 1:
				_hwpl_core_priv_disable_irq((void*)SSP1_IRQn);
				_hwpl_lpc_core_disable_pwr(PCSSP1);
				break;
#ifdef __lpc177x_8x
			case 2:
				_hwpl_core_priv_disable_irq((void*)SSP2_IRQn);
				_hwpl_lpc_core_disable_pwr(PCSSP2);
				break;
#endif
			}
		}
		ssp_local[port].ref_count--;
	}
}

int _hwpl_ssp_dev_powered_on(int port){
	return ( ssp_local[port].ref_count != 0 );
}


int hwpl_ssp_getattr(int port, void * ctl){
	LPC_SSP_TypeDef * regs;
	spi_attr_t * ctlp = (spi_attr_t*)ctl;

	uint8_t tmp;

	ctlp->pin_assign = ssp_local[port].pin_assign;

	regs = ssp_regs_table[port];

	//Master
	if ( regs->CR1 & (1<<2) ){
		//Slave
		ctlp->master = SPI_ATTR_SLAVE;
	} else {
		ctlp->master = SPI_ATTR_MASTER;
	}

	//width
	ctlp->width = (regs->CR0 & 0x0F) + 1;

	//format
	tmp = (regs->CR0 >> 4) & 0x03;
	if ( tmp == 0 ){
		ctlp->format = SPI_ATTR_FORMAT_SPI;
	} else if ( tmp == 1 ){
		ctlp->format = SPI_ATTR_FORMAT_TI;
	} else if ( tmp == 2 ){
		ctlp->format = SPI_ATTR_FORMAT_MICROWIRE;
	}

	//Mode
	ctlp->mode = 0;
	if ( regs->CR0 & (1<<6) ){
		ctlp->mode |= 0x02;
	}
	if ( regs->CR0 & (1<<7) ){
		ctlp->mode |= 0x01;
	}

	//bitrate
	if ( regs->CPSR != 0){
		ctlp->bitrate = _hwpl_core_getclock() / (regs->CPSR);
	} else {
		ctlp->bitrate = -1;
	}


	return 0;
}

int hwpl_ssp_setattr(int port, void * ctl){
	LPC_SSP_TypeDef * regs;
	uint32_t cr0, cr1, cpsr;
	uint32_t tmp;
	spi_attr_t * ctlp = (spi_attr_t*)ctl;


	regs = ssp_regs_table[port];

	if ( ctlp->bitrate == 0 ) {
		errno = EINVAL;
		return -1 - offsetof(spi_attr_t, bitrate);
	}

	if ( ctlp->mode >= 4 ) {
		errno = EINVAL;
		return -1 - offsetof(spi_attr_t, mode);
	}

	if ( ctlp->master != SPI_ATTR_MASTER ){
		errno = EINVAL;
		return -1 - offsetof(spi_attr_t, master);
	}

	if( ctlp->width > 8 ){
		errno = EINVAL;
		return -1 - offsetof(spi_attr_t, width);
	}


	cr0 = 0;
	cr1 = (1<<1); //set the enable

	tmp = _hwpl_core_getclock() / ctlp->bitrate;
	tmp = ( tmp > 255 ) ? 254 : tmp;
	tmp = ( tmp < 2 ) ? 2 : tmp;
	if ( tmp & 0x01 ){
		tmp++; //round the divisor up so that actual is less than the target
	}
	cpsr = tmp;

	if ( ctlp->mode & 0x01 ){
		cr0 |= (1<<7);
	}
	if ( ctlp->mode & 0x02 ){
		cr0 |= (1<<6);
	}

	if ( ctlp->width >= 4 && ctlp->width <= 16 ){
		cr0 |= ( ctlp->width - 1);
	} else {
		errno = EINVAL;
		return -1;
	}

	if ( ctlp->format == SPI_ATTR_FORMAT_SPI ){

	} else if ( ctlp->format == SPI_ATTR_FORMAT_TI ){
		cr0 |= (1<<4);
	} else if ( ctlp->format == SPI_ATTR_FORMAT_MICROWIRE ){
		cr0 |= (1<<5);
	} else {
		errno = EINVAL;
		return -1;
	}


	if ( ctlp->pin_assign != HWPL_GPIO_CFG_USER ){

		switch(port){
		case 0:
			if (1){
				switch(ctlp->pin_assign){
				case 0:
					if ( _hwpl_spi_cfg_pio(0, 0, 18, 17, 15) < 0 ){
						errno = ENODEV;
						return -1;
					}
					break;
				case 1:
					if( _hwpl_spi_cfg_pio(0, 1, 24, 23, 20) < 0 ){
						errno = ENODEV;
						return -1;
					}
					break;
#ifdef __lpc177x_8x
				case 2:
					if( _hwpl_spi_cfg_pio(0, 2, 27, 26, 22) < 0 ){
						errno = ENODEV;
						return -1;
					}
					break;
#endif
				default:
					errno = EINVAL;
					return -1;
				}
			}
			break;
		case 1:
			if (1){
				switch(ctlp->pin_assign){
				case 0:
					if ( _hwpl_spi_cfg_pio(1, 0, 9, 8, 7) < 0 ){
						errno = ENODEV;
						return -1;
					}
					break;
#ifdef __lpc177x_8x
				case 1:
					if ( _hwpl_spi_cfg_pio(1, 4, 23, 22, 20) < 0 ){
						errno = ENODEV;
						return -1;
					}
					break;
#endif
				default:
					errno = EINVAL;
					return -1;
				}
			}
			break;
#ifdef __lpc177x_8x
		case 2:
			if (1){
				switch(ctlp->pin_assign){
				case 0:
					if ( _hwpl_spi_cfg_pio(2, 1, 4, 1, 0) < 0 ){
						errno = ENODEV;
						return -1;
					}
					break;
				default:
					errno = EINVAL;
					return -1;
				}
			}
#endif
		}
	}

	regs->CR0 = cr0;
	regs->CR1 = cr1;
	regs->CPSR = cpsr;
	regs->IMSC &= ~(SSPIMSC_RXIM|SSPIMSC_RTIM);

	ssp_local[port].pin_assign = ctlp->pin_assign;

	return 0;
}

int hwpl_ssp_swap(int port, void * ctl){
	return byte_swap(port, (int)ctl);
}

int hwpl_ssp_setduplex(int port, void * ctl){
	ssp_local[port].duplex_mem = (void * volatile)ctl;
	return 0;
}

static void exec_callback(int port, LPC_SSP_TypeDef * regs, void * data){
	regs->IMSC &= ~(SSPIMSC_RXIM|SSPIMSC_RTIM); //Kill the interrupts
	if ( (ssp_local[port].size == 0) && (ssp_local[port].callback != NULL) ){
		if ( ssp_local[port].callback(ssp_local[port].context, 0) == 0 ){
			ssp_local[port].callback = NULL;
		}
	}
}


int hwpl_ssp_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	LPC_SSP_TypeDef * regs;
	regs = ssp_regs_table[port];

	if( action->callback == 0 ){
		if ( regs->IMSC & (SSPIMSC_RXIM|SSPIMSC_RTIM) ){
			exec_callback(port, regs, DEVICE_OP_CANCELLED);
		}

		ssp_local[port].callback = 0;

		return 0;
	}


	ssp_local[port].callback = action->callback;
	ssp_local[port].context = action->context;
	return 0;
}


int byte_swap(int port, int byte){
	LPC_SSP_TypeDef * regs;
	regs = ssp_regs_table[port];
	regs->DR = byte;
	while ( regs->SR & SSPSR_BSY ){
		;
	}
	byte = regs->DR; //read the byte to empty the RX FIFO
	return byte;

}

int _hwpl_ssp_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int port = DEVICE_GET_PORT(cfg);
	return ssp_port_transfer(port, 0, wop);
}

int _hwpl_ssp_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	int port = DEVICE_GET_PORT(cfg);
	return ssp_port_transfer(port, 1, rop);
}

void ssp_fill_tx_fifo(int port){
	LPC_SSP_TypeDef * regs;
	regs = ssp_regs_table[port];
	int size = 0;
	while ( (regs->SR & SSPSR_TNF) && ssp_local[port].size && (size < 8) ){
		if ( ssp_local[port].tx_buf != NULL ){
			//! \todo This won't handle spi widths other than 8 bits -- need to read the transmit width
			regs->DR = *ssp_local[port].tx_buf++;
		} else {
			regs->DR = 0xFF;
		}
		ssp_local[port].size--;
		size++;
	}
}

void ssp_empty_rx_fifo(int port){
	LPC_SSP_TypeDef * regs;
	regs = ssp_regs_table[port];
	while ( regs->SR & SSPSR_RNE ){
		if ( ssp_local[port].rx_buf != NULL ){
			*ssp_local[port].rx_buf++ = regs->DR;
		} else {
			regs->DR;
		}
	}
}

void _hwpl_core_ssp_isr(int port){
	LPC_SSP_TypeDef * regs;
	regs = ssp_regs_table[port];
	ssp_empty_rx_fifo(port);
	ssp_fill_tx_fifo(port);
	if ( (regs->SR & (SSPSR_TFE)) && !(regs->SR & (SSPSR_RNE)) ){
		exec_callback(port, regs, 0);
	}
}

void _hwpl_core_ssp0_isr(void){
	_hwpl_core_ssp_isr(0);
}

void _hwpl_core_ssp1_isr(void){
	_hwpl_core_ssp_isr(1);
}

int ssp_port_transfer(int port, int is_read, device_transfer_t * dop){
	int size;
	LPC_SSP_TypeDef * regs;
	size = dop->nbyte;
	if ( size == 0 ){
		return 0;
	}

	regs = ssp_regs_table[port];

	//Check to see if SSP port is busy
	if ( regs->IMSC & (SSPIMSC_RXIM|SSPIMSC_RTIM) ){
		errno = EAGAIN;
		return -1;
	}

	if ( is_read ){
		ssp_local[port].rx_buf = dop->buf;
		ssp_local[port].tx_buf = ssp_local[port].duplex_mem;
	} else {
		ssp_local[port].tx_buf = dop->buf;
		ssp_local[port].rx_buf = ssp_local[port].duplex_mem;
	}
	ssp_local[port].size = size;
	ssp_local[port].callback = dop->callback;
	ssp_local[port].context = dop->context;
	ssp_fill_tx_fifo(port);

	//! \todo Use DMA for SSP ssp_local??
	regs->IMSC |= (SSPIMSC_RXIM|SSPIMSC_RTIM); //when RX is half full or a timeout, get the bytes
	ssp_local[port].ret = size;

	return 0;
}


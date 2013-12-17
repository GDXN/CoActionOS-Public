/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */
#include <errno.h>
#include <fcntl.h>
#include "hwpl/spi.h"
#include "pio_flags.h"
#include "hwpl/pio.h"
#include "hwpl/core.h"

#include "hwpl/debug.h"

typedef struct {
	char * volatile rx_buf;
	char * volatile tx_buf;
	volatile size;
	void * duplex_mem;
	int ret;
	uint8_t pin_assign;
	uint8_t width;
	uint8_t ref_count;
	hwpl_callback_t callback;
	void * context;
} spi_local_t;

static spi_local_t spi_local[HWPL_SPI_PORTS] HWPL_SYS_MEM;

#define DECLARE_SPI_REGS(regs, port) SPI_TypeDef * regs = spi_regs_table[port]
static int spi_port_transfer(int is_read, int port, device_transfer_t * dop);

static SPI_TypeDef * const spi_regs_table[HWPL_SPI_PORTS] = HWPL_SPI_REGS;

void _hwpl_spi_dev_power_on(int port){
	if ( spi_local[port].ref_count == 0 ){
		switch(port){
		case 0:
			RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
			NVIC_EnableIRQ(SPI1_IRQn);
			break;
#if HWPL_SPI_PORTS > 1
		case 1:
			RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
			NVIC_EnableIRQ(SPI2_IRQn);
			break;
#endif
		}
		spi_local[port].duplex_mem = NULL;
		spi_local[port].callback = NULL;
	}
	spi_local[port].ref_count++;
}

void _hwpl_spi_dev_power_off(int port){
	if ( spi_local[port].ref_count > 0 ){
		if ( spi_local[port].ref_count == 1 ){
			switch(port){
			case 0:
				NVIC_DisableIRQ(SPI1_IRQn);
				RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
				break;
#if HWPL_SPI_PORTS > 1
			case 1:
				NVIC_DisableIRQ(SPI2_IRQn);
				RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
				break;
#endif
			}
		}
		spi_local[port].ref_count--;
	}
}

int _hwpl_spi_dev_powered_on(int port){
	return ( spi_local[port].ref_count != 0 );
}


int hwpl_spi_getattr(int port, void * ctl){
	spi_attr_t * attr = ctl;
	return 0;
}



int hwpl_spi_setattr(int port, void * ctl){
	spi_attr_t * attr = ctl;
	uint16_t cr1;
	uint16_t cr2;
	uint8_t fdiv;
	uint32_t tmp;
	uint8_t pin_assign;
	DECLARE_SPI_REGS(regs, port);

	cr1 = SPI_CR1_SPE; //SPI enable
	cr2 = 0;

	if( (attr->bitrate == 0) ||
			(attr->format != SPI_ATTR_FORMAT_SPI) ||
			(attr->width != 8 )
			){
		errno = EINVAL;
		return -1;
	}

	if( attr->mode & 0x01 ){
		cr1 |= SPI_CR1_CPHA;
	}

	if( attr->mode & 0x02 ){
		cr1 |= SPI_CR1_CPOL;
	}

	if( attr->master == SPI_ATTR_MASTER ){
		cr1 |= SPI_CR1_MSTR;
	}

	fdiv = 0;
	tmp = _hwpl_core_getclock() >> 1;
	while( tmp > attr->bitrate ){
		tmp >>= 1;
		fdiv++;
		if( fdiv == 7 ){
			break;
		}
	}
	//fdiv is limited to factor of 2 (2, 4, 8, 16, 32)
	cr1 |= (fdiv << 3);

	//enable the pins
	pin_assign = attr->pin_assign;
	switch(port){
	case 0:
		if( pin_assign == 0 ){
			//PA4, PA5, PA6, PA7 (PA4 is slave select)
			//configure IO pins
			RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //make sure the GPIO is clocked
			AFIO->MAPR &= ~(AFIO_MAPR_SPI1_REMAP); //clear the remapping bit

			configure_pio_low_pair(GPIOA,
					4, MODE_CNF_INPUT_PULLUP_DOWN,  //PULL NSS high
					5, MODE_CNF_ALTFUNCTION_PUSHPULL_50MHZ);

			GPIOA->BSRR = (1<<4); //NSS is a pullup
			configure_pio_low_pair(GPIOA,
					6, MODE_CNF_INPUT_FLOATING,
					7, MODE_CNF_ALTFUNCTION_PUSHPULL_50MHZ);


		} else if ( pin_assign == 1 ){
			//PB3, PB4, and PB5
			//configure IO pins
			RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //make sure the GPIO is clocked
			AFIO->MAPR |= (AFIO_MAPR_SPI1_REMAP); //set the remapping bit
			configure_pio_low_trio(GPIOB,
					3, MODE_CNF_ALTFUNCTION_PUSHPULL_50MHZ,
					4, MODE_CNF_INPUT_FLOATING,
					5, MODE_CNF_ALTFUNCTION_PUSHPULL_50MHZ);

		} else {
			errno = EINVAL;
			return -1;
		}
		break;
#if HWPL_SPI_PORTS > 1
	case 1:
		if( pin_assign == 0 ){
			//PB12, PB13, PB14, PB15 (PB12 is NSS -- not yet implemented --slave only)
			//configure IO pins
			RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //make sure the GPIO is clocked
			AFIO->MAPR &= ~(AFIO_MAPR_SPI2_REMAP); //clear the remapping bit

			configure_pio_high_pair(GPIOB,
					12, MODE_CNF_INPUT_PULLUP_DOWN,
					13, MODE_CNF_ALTFUNCTION_PUSHPULL_50MHZ);

			GPIOB->BSRR = (1<<12); //NSS is a pullup
			configure_pio_high_pair(GPIOB,
					14, MODE_CNF_INPUT_FLOATING,
					15, MODE_CNF_ALTFUNCTION_PUSHPULL_50MHZ);
		} else {
			errno = EINVAL;
			return -1;
		}
		break;
#endif
	default:
		errno = EINVAL;
		return -1;
	}

	regs->CR1 = cr1;
	regs->CR2 = 0;

	return 0;
}

int hwpl_spi_swap(int port, void * ctl){
	return byte_swap(port, (int)ctl);
}

int hwpl_spi_setduplex(int port, void * ctl){
	spi_local[port].duplex_mem = (void * volatile)ctl;
	return 0;
}

int hwpl_spi_setaction(int port, void * ctl){
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	spi_local[port].callback = action->callback;
	spi_local[port].context = action->context;
	return 0;
}


int byte_swap(int port, int byte){
	DECLARE_SPI_REGS(regs, port);
	while( !(regs->SR & SPI_SR_TXE) ){
		;
	}
	regs->DR = byte;

	while( !(regs->SR & SPI_SR_TXE) ){
		;
	}
	return regs->DR;
}

int _hwpl_spi_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int port = DEVICE_GET_PORT(cfg);
	return spi_port_transfer(0, port, wop);

}

int _hwpl_spi_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	int port = DEVICE_GET_PORT(cfg);
	return spi_port_transfer(1, port, rop);
}

void _hwpl_core_spi_isr(int port){
	int tmp;
	int status;
	DECLARE_SPI_REGS(regs, port);

	status = regs->SR;
	//! \todo Check for errors

	tmp = regs->DR;
	if ( spi_local[port].rx_buf != NULL ){
		*spi_local[port].rx_buf++ = tmp;
	}

	if ( spi_local[port].size ){
		spi_local[port].size--;
	}

	if ( spi_local[port].size ){
		if ( spi_local[port].tx_buf != NULL ){
			tmp = *spi_local[port].tx_buf++;
		} else {
			tmp = 0xFF;
		}

		regs->DR = tmp;
	} else {
		regs->CR2 &= ~(SPI_CR2_TXEIE|SPI_CR2_RXNEIE); //disable the transmit interrupt
	}

	if ( spi_local[port].size == 0 ){
		//execute the callback
		if ( spi_local[port].callback != NULL ){
			if( spi_local[port].callback(spi_local[port].context, 0) == 0 ){
				spi_local[port].callback = NULL;
			}
		}
	}
}

void _hwpl_core_spi1_isr(void){
	_hwpl_core_spi_isr(0);
}

void _hwpl_core_spi2_isr(void){
	_hwpl_core_spi_isr(1);
}

int spi_port_transfer(int is_read, int port, device_transfer_t * dop){
	int size;
	DECLARE_SPI_REGS(regs, port);
	size = dop->nbyte;
	if ( size == 0 ){
		return 0;
	}

	if ( regs->CR2 & (SPI_CR2_RXNEIE) ){
		errno = EAGAIN;
		return -1;
	}

	if ( is_read ){
		spi_local[port].rx_buf = dop->buf;
		spi_local[port].tx_buf = spi_local[port].duplex_mem;
	} else {
		spi_local[port].tx_buf = dop->buf;
		spi_local[port].rx_buf = spi_local[port].duplex_mem;
	}
	spi_local[port].size = size;
	spi_local[port].callback = dop->callback;
	spi_local[port].context = dop->context;
	regs->CR2 |= (SPI_CR2_RXNEIE); //enable the interrupt
	if ( spi_local[port].tx_buf ){
		//! \todo This won't handle spi widths other than 8 bits
		regs->DR = *spi_local[port].tx_buf++;
	} else {
		regs->DR = 0xFF;
	}

	spi_local[port].ret = size;

	return 0;
}

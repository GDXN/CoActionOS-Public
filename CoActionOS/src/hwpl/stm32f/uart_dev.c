/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <errno.h>
#include <fcntl.h>
#include "hwpl/core.h"
#include "hwpl/uart.h"
#include "hwpl/gpio.h"
#include "hwpl/debug.h"

#include "gpio_flags.h"

typedef struct {
	hwpl_callback_t write_callback;
	void * write_context;
	hwpl_callback_t read_callback;
	void * read_context;
	uart_attr_t attr;
	char * rx_bufp;
	int * rx_nbyte;
	volatile int rx_len;
	const char * tx_bufp;
	volatile int tx_len;
	uint8_t ref_count;
} uart_local_t;

static uart_local_t uart_local[HWPL_UART_PORTS] HWPL_SYS_MEM;

static void read_rx_data(int port);
static void write_tx_data(int port);
static int do_transfer(int port);

#define DECLARE_UART_REGS(regs, port) USART_TypeDef * regs = uart_regs_table[port]

USART_TypeDef * const uart_regs_table[HWPL_UART_PORTS] = HWPL_UART_REGS;

int _hwpl_uart_dev_port_is_invalid(int port){
	if ( (uint32_t)port > (HWPL_UART_PORTS-1) ){
		return 1;
	}
	return 0;
}

void _hwpl_uart_dev_power_on(int port){
	if ( uart_local[port].ref_count == 0 ){
		switch(port){
		case 0:
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
			NVIC_EnableIRQ(USART1_IRQn);
			break;
		case 1:
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
			NVIC_EnableIRQ(USART2_IRQn);
			break;
#if HWPL_UART_PORTS > 2
		case 2:
			RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
			NVIC_EnableIRQ(USART3_IRQn);
			break;
#endif

#if HWPL_UART_PORTS > 3
		case 3:
			RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
			NVIC_EnableIRQ(UART4_IRQn);
			break;
#endif

#if HWPL_UART_PORTS > 4
		case 4:
			RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
			NVIC_EnableIRQ(UART5_IRQn);
			break;
#endif
		}
		uart_local[port].tx_bufp = NULL;
		uart_local[port].rx_bufp = NULL;
	}
	uart_local[port].ref_count++;
}

void _hwpl_uart_dev_power_off(int port){

	if ( uart_local[port].ref_count > 0 ){
		if ( uart_local[port].ref_count == 1 ){
			switch(port){
			case 0:
				NVIC_DisableIRQ(USART1_IRQn);
				RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
				break;
			case 1:
				NVIC_DisableIRQ(USART2_IRQn);
				RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
				break;
#if HWPL_UART_PORTS > 2
			case 2:
				NVIC_DisableIRQ(USART3_IRQn);
				RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN;
				break;
#endif

#if HWPL_UART_PORTS > 3
			case 3:
				NVIC_DisableIRQ(UART4_IRQn);
				RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN;
				break;
#endif

#if HWPL_UART_PORTS > 4
			case 4:
				NVIC_DisableIRQ(UART5_IRQn);
				RCC->APB1ENR &= ~RCC_APB1ENR_UART5EN;
				break;
#endif
			}
			uart_local[port].tx_bufp = NULL;
			uart_local[port].rx_bufp = NULL;
		}
		uart_local[port].ref_count--;
	}
}

int _hwpl_uart_dev_powered_on(int port){
	return ( uart_local[port].ref_count != 0 );
}

int hwpl_uart_getattr(int port, void * ctl){
	memcpy(ctl, &(uart_local[port].attr), sizeof(uart_attr_t));
	return 0;
}

int hwpl_uart_setattr(int port, void * ctl){
	uint32_t baud_rate;
	uint16_t cr1;
	uint16_t cr2;
	uint16_t cr3;
	uint32_t f_div;
	uint32_t tmp;
	uint16_t brr;
	uint8_t pin_assign;
	uart_attr_t * ctl_ptr = (uart_attr_t*)ctl;
	DECLARE_UART_REGS(uart_regs, port);

	if ( ctl_ptr->baudrate != 0 ){
		baud_rate = ctl_ptr->baudrate;
	} else {
		errno = EINVAL;
		return -1;
	}

	cr1 = USART_CR1_UE|USART_CR1_TE|USART_CR1_RE;
	cr2 = 0;
	cr3 = 0;

	switch(ctl_ptr->stop){
	case UART_ATTR_STOP_BITS_1:
		break;
	case UART_ATTR_STOP_BITS_0_5:
		cr2 |= USART_CR2_STOP_0;
		break;
	case UART_ATTR_STOP_BITS_2:
		cr2 |= USART_CR2_STOP_1;
		break;
	case UART_ATTR_STOP_BITS_1_5:
		cr2 |= (USART_CR2_STOP_1|USART_CR2_STOP_0);
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	if ( ctl_ptr->width != 8 ){
		errno = EINVAL;
		return -1;
	}


	switch(ctl_ptr->parity){
	case UART_PARITY_NONE:
		break;
	case UART_PARITY_ODD:
		cr1 |= (USART_CR1_PCE|USART_CR1_PS);
		break;
	case UART_PARITY_EVEN:
		cr1 |= (USART_CR1_PCE);
		break;
	default:
		errno = EINVAL;
		return -1;
	}


	pin_assign = ctl_ptr->pin_assign;
	switch(port){
	case 0:
		if( pin_assign == 0 ){
			//PA9 and PA10
			//configure IO pins
			RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //make sure the GPIO is clocked
			AFIO->MAPR &= ~(AFIO_MAPR_USART1_REMAP); //clear the remapping bit

			configure_gpio_high_pair(GPIOA,
					9, MODE_CNF_ALTFUNCTION_PUSHPULL_2MHZ,
					10, MODE_CNF_INPUT_FLOATING);


		} else if ( pin_assign == 1 ){
			//PB6 and PB7
			//configure IO pins
			RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //make sure the GPIO is clocked
			AFIO->MAPR |= (AFIO_MAPR_USART1_REMAP); //set the remapping bit
			configure_gpio_low_pair(GPIOB,
					6, MODE_CNF_ALTFUNCTION_PUSHPULL_2MHZ,
					7, MODE_CNF_INPUT_FLOATING);
		} else {
			errno = EINVAL;
			return -1;
		}
		break;
	case 1:
		if( pin_assign == 0 ){
			//PA2 and PA3
			//configure IO pins
			RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //make sure the GPIO is clocked
			AFIO->MAPR &= ~(AFIO_MAPR_USART1_REMAP); //clear the remapping bit
			configure_gpio_low_pair(GPIOA, 2, MODE_CNF_ALTFUNCTION_PUSHPULL_2MHZ,
					3, MODE_CNF_INPUT_FLOATING);

		} else if ( pin_assign == 1 ){
			//PD5 and PD6
			//configure IO pins
			configure_gpio_low_pair(GPIOD, 5, MODE_CNF_ALTFUNCTION_PUSHPULL_2MHZ,
					6, MODE_CNF_INPUT_FLOATING);
			AFIO->MAPR |= (AFIO_MAPR_USART2_REMAP); //set the remapping bit
		} else {
			errno = EINVAL;
			return -1;
		}
		break;
#if HWPL_UART_PORTS > 2
	case 2:
		if( pin_assign == 0 ){
			//PB10 and PB11
			//configure IO pins
			RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //make sure the GPIO is clocked
			AFIO->MAPR &= ~(AFIO_MAPR_USART3_REMAP); //clear the remapping bit
			configure_gpio_high_pair(GPIOB,
					10, MODE_CNF_ALTFUNCTION_PUSHPULL_2MHZ,
					11, MODE_CNF_INPUT_FLOATING);

		} else if ( pin_assign == 1 ){
			//PC10 and PC11
			//configure IO pins
			configure_gpio_high_pair(GPIOC,
					10, MODE_CNF_ALTFUNCTION_PUSHPULL_2MHZ,
					11, MODE_CNF_INPUT_FLOATING);
			AFIO->MAPR |= (AFIO_MAPR_USART3_REMAP_0); //partial remap
		} else if ( pin_assign == 2 ){
			//PD8 and PD9
			//configure IO pins
			configure_gpio_high_pair(GPIOD, 8, MODE_CNF_ALTFUNCTION_PUSHPULL_2MHZ,
					9, MODE_CNF_INPUT_FLOATING);
			AFIO->MAPR |= (AFIO_MAPR_USART3_REMAP); //full
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



	//calculate the set the baud rate
	brr = (_hwpl_core_getclock() + (baud_rate>>1))/baud_rate;
	uart_regs->BRR = brr;

	uart_regs->CR1 = cr1;
	uart_regs->CR2 = cr2;
	uart_regs->CR3 = cr3;

	memcpy(&(uart_local[port].attr), ctl, sizeof(uart_attr_t));
	//need to store actual baud rate not target baud rate
	uart_local[port].attr.baudrate = (_hwpl_core_getclock() + (brr>>1))/(brr);
	return 0;
}

int hwpl_uart_setaction(int port, void * ctl){
	DECLARE_UART_REGS(uart_regs, port);
	hwpl_action_t * action = (hwpl_action_t*)ctl;
	if( action->event == UART_EVENT_DATA_READY ){
		uart_local[port].read_callback = action->callback;
		uart_local[port].read_context = action->context;
		if ( uart_local[port].read_callback != NULL ){
			uart_regs->CR1 |= USART_CR1_RXNEIE;
			uart_local[port].rx_bufp = NULL;
		} else {
			uart_regs->CR1 &= ~USART_CR1_RXNEIE;
		}
	} else if ( action->event == UART_EVENT_WRITE_COMPLETE ){
		uart_local[port].write_callback = action->callback;
		uart_local[port].write_context = action->context;
	} else {
		errno = EINVAL;
		return -1;
	}
	return 0;
}

int hwpl_uart_clear(int port, void * ctl){
	return 0;
}

int hwpl_uart_flush(int port, void * ctl){

	return 0;
}


int hwpl_uart_getbyte(int port, void * ctl){

	return -1;
}


int _hwpl_uart_dev_read(const device_cfg_t * cfg, device_transfer_t * rop){
	int len;
	int port;
	USART_TypeDef * uart_regs;

	//grab the port and registers
	port = DEVICE_GET_PORT(cfg);
	uart_regs = uart_regs_table[port];

	if ( uart_regs->CR1 & USART_CR1_RXNEIE ){
		errno = EAGAIN;
		return -1;
	}

	//initialize the transfer
	uart_local[port].rx_bufp = rop->buf;
	uart_local[port].rx_len = rop->nbyte;
	uart_local[port].rx_nbyte = &rop->nbyte;

	//Check the local buffer for bytes that are immediately available
	read_rx_data(port);
	len = rop->nbyte - uart_local[port].rx_len;
	if ( len == 0 ){  //nothing available to read
		if ( rop->flags & O_NONBLOCK ){
			uart_local[port].read_callback = NULL;
			uart_local[port].rx_bufp = NULL;
			rop->nbyte = 0;
		} else {
			uart_local[port].read_callback = rop->callback;
			uart_local[port].read_context = rop->context;
			uart_regs->CR1 |= USART_CR1_RXNEIE;
		}
	}
	return len;
}

int _hwpl_uart_dev_write(const device_cfg_t * cfg, device_transfer_t * wop){
	USART_TypeDef * uart_regs;
	int port;
	port = DEVICE_GET_PORT(cfg);

	if ( wop->nbyte == 0 ){
		return 0;
	}

	//Grab the registers
	uart_regs = uart_regs_table[port];

	//Check to see if the port is busy

	if ( uart_regs->CR1 & USART_CR1_TXEIE ){
		errno = EAGAIN;
		return -1;
	}

	//Initialize variables
	uart_local[port].tx_bufp = wop->cchbuf;
	uart_local[port].tx_len = wop->nbyte;

	//Check the local buffer for bytes that are immediately available
	uart_local[port].write_callback = wop->callback;
	uart_local[port].write_context = wop->context;
	uart_regs->CR1 |= (USART_CR1_TXEIE|USART_CR1_TCIE);
	write_tx_data(port);
	return 0;
}

void read_rx_data(int port){
	USART_TypeDef * uart_regs = uart_regs_table[port];
	if( uart_regs->SR & USART_SR_RXNE ){
		*(uart_local[port].rx_bufp)++ = uart_regs->DR;
		uart_local[port].rx_len--;
	}
}

void write_tx_data(int port){
	USART_TypeDef * uart_regs = uart_regs_table[port];
	if( uart_regs->SR & USART_SR_TXE ){
		uart_regs->DR = *(uart_local[port].tx_bufp)++;
		uart_local[port].tx_len--;
	}
}

void _hwpl_uart_isr(int port){

	//first determine if this is a UART0 interrupt
	USART_TypeDef * uart_regs = uart_regs_table[port];
	uint32_t status;
	status = uart_regs->SR;

	//Check for incoming bytes
	if( status & USART_SR_RXNE ){ //this is the RX FIFO ready flag -- read all the bytes in the FIFO before exiting
		//todo check for errors with this byte

		if ( uart_local[port].rx_bufp != NULL ){
			read_rx_data(port);
			*(uart_local[port].rx_nbyte) = *(uart_local[port].rx_nbyte) - uart_local[port].rx_len;
			uart_local[port].rx_bufp = NULL;
		}

		if ( uart_local[port].read_callback != NULL ){
			if( uart_local[port].read_callback(uart_local[port].read_context, NULL) == 0 ){
				uart_local[port].read_callback = NULL;
			}
		}

		//if the callback is NULL now, disable the interrupt
		if( uart_local[port].read_callback == NULL ){
			uart_regs->CR1 &= ~USART_CR1_RXNEIE;
		}
	}

	if( uart_regs->CR1 & USART_CR1_TXEIE ){

		if( status & USART_SR_TXE ){ //DR is empty
			//check for outgoing bytes
			if ( uart_local[port].tx_len > 0 ){
				write_tx_data(port);
			} else if( status & USART_SR_TC ){
				//transmission complete
				uart_local[port].tx_bufp = NULL;
				//call the write callback
				if ( uart_local[port].write_callback != NULL ){
					if( uart_local[port].write_callback(uart_local[port].write_context, NULL) == 0 ){
						uart_local[port].write_callback = NULL;
					}
				}

				//if the callback is NULL now, disable the interrupt
				if( uart_local[port].write_callback == NULL ){
					uart_regs->CR1 &= ~(USART_CR1_TXEIE|USART_CR1_TCIE);
				}
			}
		}
	}
}

void _hwpl_core_usart1_isr(void){
	_hwpl_uart_isr(0);
}

void _hwpl_core_usart2_isr(void){
	_hwpl_uart_isr(1);
}

#if HPWL_UART_PORTS > 2
void core_usart3_isr(void){
	_hwpl_uart_isr(2);
}
#endif

#if HPWL_UART_PORTS > 3
void core_uart4_isr(void){
	_hwpl_uart_isr(3);
}
#endif

#if HPWL_UART_PORTS > 4
void core_uart5_isr(void){
	_hwpl_uart_isr(3);
}
#endif


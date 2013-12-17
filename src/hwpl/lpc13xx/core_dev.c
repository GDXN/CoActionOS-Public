/*
 * core_dev.c
 *
 *  Created on: Nov 1, 2012
 *      Author: tgil
 */


#include "config.h"
#include <errno.h>
#include <fcntl.h>
#include "hwpl/core.h"

#include "hwpl/debug.h"

static core_reset_src_t _hwpl_core_get_reset_src(void);
static int _hwpl_core_enable_clkout(int clk_source, int div);
void _hwpl_set_sleep_mode(int * level);


//static const uint8_t _hwpl_irq_map[CORE_PERIPH_TOTAL];
//static const uint8_t _hwpl_irq_map_ports[CORE_PERIPH_TOTAL];


int hwpl_core_setpinfunc(int port, void * arg){
	core_pinfunc_t * argp = arg;
	return _hwpl_core_set_pinsel_func(argp->io.port,
			argp->io.pin,
			argp->periph_func,
			argp->periph_port);
}


void _hwpl_core_dev_power_on(int port){}
void _hwpl_core_dev_power_off(int port){}
int _hwpl_core_dev_powered_on(int port){ return 1; }


int hwpl_core_getattr(int port, void * arg){
	core_attr_t * attrp = arg;
	attrp->clock = hwpl_core_cpu_freq;
	attrp->reset_type = _hwpl_core_get_reset_src();
	//attrp->signature = _hwpl_core_getsignature(port, arg);
	attrp->signature = 0;
	return _hwpl_lpc_flash_get_serialno(attrp->serial_number);
}

int hwpl_core_setattr(int port, void * arg){
	errno = ENOTSUP;
	return -1;
}

int hwpl_core_setaction(int port, void * arg){
	errno = ENOTSUP;
	return -1;
}

int hwpl_core_sleep(int port, void * arg){
	int tmp;
	int level;
	level = (int)arg;

	_hwpl_set_sleep_mode(&level);
	if ( level < 0 ){
		return tmp;
	}


	//Wait for an interrupts
	__WFI();
	return 0;
}

int hwpl_core_reset(int port, void * arg){
	//delay first
	_delay_us(20*1000);
	_hwpl_core_priv_reset(NULL);
	//doesn't arrive here
	return 0;
}

int hwpl_core_invokebootloader(int port, void * arg){
	/*
	uint32_t * dfu_sw_req;
	_delay_us(20*1000);
	dfu_sw_req = (uint32_t*)HWPL_CORE_BOOTLOADER_LOC;
	*dfu_sw_req = HWPL_CORE_BOOTLOADER_VALUE;
	_hwpl_core_priv_reset(NULL);
	//doesn't arrive here
	*/
	return -1;
}


void _hwpl_set_sleep_mode(int * level){

	SCB->SCR &= ~(1<<SCB_SCR_SLEEPDEEP_Pos);
	LPC_SYSCON->PDSLEEPCFG = 0xFFF;
	LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;

	LPC_PMU->PCON = 0;
	switch(*level){
	case CORE_DEEPSLEEP_STOP:

	case CORE_DEEPSLEEP:
		SCB->SCR |= (1<<SCB_SCR_SLEEPDEEP_Pos);
		break;
	case CORE_SLEEP:
		break;
	case CORE_DEEPSLEEP_STANDBY:
		SCB->SCR |= (1<<SCB_SCR_SLEEPDEEP_Pos);
		LPC_PMU->PCON |= (1<<1); //deep power down mode
		break;
	default:
		*level = -1;
		return;
	}

	*level = 0;
}

core_reset_src_t _hwpl_core_get_reset_src(void){
	uint32_t src_reg;
	src_reg = LPC_SYSCON->SYSRESSTAT;
	LPC_SYSCON->SYSRESSTAT = 0x1F; //reset the status
	if ( src_reg & (1<<3) ){
		return CORE_RESET_SRC_BOR;
	}

	if ( src_reg & (1<<2) ){
		return CORE_RESET_SRC_WDT;
	}

	if ( src_reg & (1<<1) ){
		return CORE_RESET_SRC_EXTERNAL;
	}

	if ( src_reg & (1<<0) ){
		return CORE_RESET_SRC_POR;
	}

	if ( src_reg & (1<<4) ){
		return CORE_RESET_SRC_SYSTEM;
	}

	return 0;
}

int _hwpl_core_enable_clkout(int clk_source, int div){
	/*
	div = (div & 0xF);
	if ( div != 0 ){
		div = div - 1;
	}
	switch(clk_source){
	case CORE_CLKOUT_CPU:
		LPC_SC->CLKOUTCFG = (1<<8)|(div<<4)|(0<<0);
		break;
	case CORE_CLKOUT_MAIN_OSC:
		LPC_SC->CLKOUTCFG = (1<<8)|(div<<4)|(1<<0);
		break;
	case CORE_CLKOUT_INTERNAL_OSC:
		LPC_SC->CLKOUTCFG = (1<<8)|(div<<4)|(2<<0);
		break;
	case CORE_CLKOUT_USB:
		LPC_SC->CLKOUTCFG = (1<<8)|(div<<4)|(3<<0);
		break;
	case CORE_CLKOUT_RTC:
		LPC_SC->CLKOUTCFG = (1<<8)|(div<<4)|(4<<0);
		break;
	default:
		return -1;
	}

	hwpl_core_set_pinsel_func(1, 27, CORE_PERIPH_CORE, 0);
	*/
	return 0;

}


int hwpl_core_setirqprio(int port, void * arg){
	/*
	IRQn_Type irq;
	core_irqprio_t * p = arg;
	irq = (IRQn_Type)_hwpl_irq_map[p->periph];
	if( irq == 0xFF ){
		errno = EINVAL;
		return -1;
	}
	if( irq != SPI_IRQn ){
		if( p->port < _hwpl_irq_map_ports[p->periph]){
			irq += p->port;
		} else {
			errno = EINVAL;
			return -1;
		}
	} else {
		switch(p->port){
		case 0:
			irq = SSP0_IRQn;
			break;
		case 1:
			irq = SSP1_IRQn;
			break;
		case 2:
			irq = SPI_IRQn;
			break;
		default:
			errno = EINVAL;
			return -1;
		}
	}
	NVIC_SetPriority( irq, p->prio );
	return 0;
	*/
	return -1;
}

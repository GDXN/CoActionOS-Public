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
	attrp->signature = _hwpl_core_getsignature(port, arg);
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

	//Wait for an interrupt
	__WFI();
	return 0;
}

int hwpl_core_reset(int port, void * arg){
	_hwpl_core_priv_reset(NULL);
	//doesn't arrive here
	return 0;
}

int hwpl_core_invokebootloader(int port, void * arg){
	uint32_t * dfu_sw_req;
	dfu_sw_req = (uint32_t*)HWPL_CORE_BOOTLOADER_LOC;
	*dfu_sw_req = HWPL_CORE_BOOTLOADER_VALUE;
	_hwpl_core_priv_reset(NULL);
	//doesn't arrive here
	return 0;
}


void _hwpl_set_sleep_mode(int * level){
	SCB->SCR &= ~(1<<SCB_SCR_SLEEPDEEP_Pos);
	PWR->CR &= ~(PWR_CR_LPDS|PWR_CR_PDDS);
	switch(*level){
	case CORE_DEEPSLEEP_STANDBY:
		PWR->CR |= PWR_CR_PDDS;
	case CORE_DEEPSLEEP_STOP:
		PWR->CR |= PWR_CR_LPDS;
	case CORE_DEEPSLEEP:
		SCB->SCR |= (1<<SCB_SCR_SLEEPDEEP_Pos);
	case CORE_SLEEP:
		break;
	default:
		*level = -1;
		return;
	}
	*level = 0;
}

core_reset_src_t _hwpl_core_get_reset_src(void){
	return 0;
}

int _hwpl_core_enable_clkout(int clk_source, int div){
	errno = ENOTSUP;
	return -1;
}


int hwpl_core_setirqprio(int port, void * arg){
	errno = ENOTSUP;
	return -1;
}

/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <errno.h>
#include <unistd.h>
#include "hwpl/pio.h"
#include "hwpl/spi.h"
#include "hwpl/tmr.h"
#include "hwpl/debug.h"
#include "hwdl.h"
#include "sst25vf_local.h"

static void complete_spi_write(const device_cfg_t * cfg, uint32_t ignore);
static void continue_spi_write(const device_cfg_t * cfg, uint32_t ignore);

int sst25vf_tmr_open(const device_cfg_t * cfg){
	int err;
	uint8_t status;
	pio_attr_t attr;

	err = hwdl_check_spi_port(cfg);
	if ( err < 0 ){
		return err;
	}

	sst25vf_share_deassert_cs(cfg);
	attr.mask = (1<<cfg->pcfg.spi.cs.pin);
	attr.mode = PIO_MODE_OUTPUT | PIO_MODE_DIRONLY;
	hwpl_pio_setattr(cfg->pcfg.spi.cs.port, &attr);

	sst25vf_share_write_disable(cfg);

	//Now ping the device to see if it responds
	sst25vf_share_power_up(cfg);
	sst25vf_share_global_protect(cfg);
	status = sst25vf_share_read_status(cfg);

	if ( status != 0x9C ){
		//Global protect command failed
		errno = EIO;
		return -1;
	}


	if ( sst25vf_share_global_unprotect(cfg) ){
		errno = EIO;
		return -1;
	}
	status = sst25vf_share_read_status(cfg);

	if ( status != 0x80 ){
		//global unprotect failed
		errno = EIO;
		return -1;
	}

	//The device is ready to use
	return 0;
}

static void complete_spi_read(const device_cfg_t * cfg, uint32_t ignore){
	sst25vf_state_t * state = (sst25vf_state_t*)cfg->state;
	sst25vf_share_deassert_cs(cfg);
	if( state->callback != NULL ){
		state->callback(state->context, NULL);
		state->callback = NULL;
	}
}

int sst25vf_tmr_read(const device_cfg_t * cfg, device_transfer_t * rop){
	sst25vf_state_t * state = (sst25vf_state_t*)cfg->state;
	const sst25vf_cfg_t * dcfg = (const sst25vf_cfg_t *)(cfg->dcfg);
	state->callback = rop->callback;
	state->context = rop->context;
	rop->context = (void*)cfg;
	rop->callback = (hwpl_callback_t)complete_spi_read;

	if ( rop->loc >= dcfg->size ){
		return EOF;
	}

	if ( rop->loc + rop->nbyte > dcfg->size ){
		rop->nbyte = dcfg->size - rop->loc; //update the bytes read to not go past the end of the disk
	}

	sst25vf_share_assert_cs(cfg);
	sst25vf_share_write_opcode_addr(cfg, SST25VF_INS_RD_HS, rop->loc);
	hwpl_spi_swap(cfg->periph.port, NULL); //dummy byte output
	return hwpl_spi_read(cfg, rop);
}


void continue_spi_write(const device_cfg_t * cfg, uint32_t ignore){
	sst25vf_state_t * state = (sst25vf_state_t *)cfg->state;
	const sst25vf_cfg_t * sst_cfg = (sst25vf_cfg_t*)cfg->dcfg;
	hwpl_action_t action;
	uint8_t * addrp;
	//should be called 10 us after complete_spi_write() executes

	//Disable the TMR interrupt
	action.callback = NULL;
	action.context = NULL;
	action.event = TMR_ACTION_EVENT_NONE;
	action.channel = sst_cfg->miso.pin;
	hwpl_tmr_off(sst_cfg->miso.port, NULL);
	hwpl_tmr_setaction(sst_cfg->miso.port, &action);
	hwpl_tmr_on(sst_cfg->miso.port, NULL);


	if( state->nbyte > 0 ){
		sst25vf_share_assert_cs(cfg);
		addrp = (uint8_t*)&(state->op.loc);
		state->cmd[0] = SST25VF_INS_PROGRAM;
		state->cmd[1] = addrp[2];
		state->cmd[2] = addrp[1];
		state->cmd[3] = addrp[0];
		state->cmd[4] = state->buf[0];

		state->op.nbyte = 5;
		hwpl_spi_write(cfg, &(state->op));
		state->op.loc++;
		state->buf += 1;
		state->nbyte -= 1;
	} else {

		sst25vf_share_write_disable(cfg);

		sst25vf_share_read_status(cfg);

		//Set the buffer to NULL to indicate the device is not busy
		state->buf = NULL;

		//call the event handler to show the operation is complete
		if ( state->callback != NULL ){
			state->callback(state->context, NULL);
			state->callback = NULL;
		}
	}
}

void complete_spi_write(const device_cfg_t * cfg, uint32_t ignore){
	uint32_t tval;
	hwpl_action_t action;
	tmr_reqattr_t attr;
	sst25vf_cfg_t * sst_cfg = (sst25vf_cfg_t*)cfg->dcfg;


	sst25vf_share_deassert_cs(cfg);

	//configure the TMR to interrupt in 10 microseconds
	action.context = (void*)cfg;
	action.callback = (hwpl_callback_t)continue_spi_write;
	action.channel = sst_cfg->miso.pin;
	action.event = TMR_ACTION_EVENT_INTERRUPT;

	attr.channel = sst_cfg->miso.pin;

	//turn the timer off
	hwpl_tmr_off(sst_cfg->miso.port, NULL);
	hwpl_tmr_setaction(sst_cfg->miso.port, &action);
	tval = hwpl_tmr_get(sst_cfg->miso.port, NULL);
	attr.value = tval + 20;
	if( attr.value > (1000000*2048) ){
		attr.value -= (1000000*2048);
	}
	hwpl_tmr_setoc(sst_cfg->miso.port, &attr);

	//everything is set; turn the timer back on
	hwpl_tmr_on(sst_cfg->miso.port, NULL);
}

int sst25vf_tmr_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int err;
	uint8_t *addrp;
	sst25vf_state_t * state = (sst25vf_state_t*)cfg->state;

	if ( state->buf != NULL ){
		errno = EBUSY;
		return -1;
	}

	//This is the final callback and context when all the writing is done
	state->callback = wop->callback;
	state->context = wop->context;
	state->buf = wop->buf;
	state->nbyte = wop->nbyte;

	sst25vf_share_write_enable(cfg);

	//Byte program
	addrp = (uint8_t*)&(wop->loc);
	state->cmd[0] = SST25VF_INS_PROGRAM;
	state->cmd[1] = addrp[2];
	state->cmd[2] = addrp[1];
	state->cmd[3] = addrp[0];
	state->cmd[4] = state->buf[0];

	sst25vf_share_assert_cs(cfg);
	state->op.flags = wop->flags;
	state->op.callback = (hwpl_callback_t)complete_spi_write;
	state->op.context = (void*)cfg;
	state->op.cbuf = state->cmd;
	state->op.nbyte = 5;
	state->op.loc = wop->loc;

	err = hwpl_spi_write(cfg, &state->op);
	state->op.loc++;
	state->buf = state->buf + 1;
	state->nbyte = state->nbyte - 1;
	return err;
}

int sst25vf_tmr_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	sst25vf_cfg_t * sst_cfg = (sst25vf_cfg_t*)cfg->dcfg;
	switch(request){
	case I_DISK_PROTECT:
		sst25vf_share_global_protect(cfg);
		break;
	case I_DISK_UNPROTECT:
		sst25vf_share_global_unprotect(cfg);
		break;
	case I_DISK_ERASE_BLOCK:
		sst25vf_share_block_erase_4kb(cfg, (ssize_t)ctl);
		break;
	case I_DISK_ERASE_DEVICE:
		sst25vf_share_chip_erase(cfg);
		break;
	case I_DISK_POWER_DOWN:
		sst25vf_share_power_down(cfg);
		break;
	case I_DISK_POWER_UP:
		sst25vf_share_power_up(cfg);
		break;
	case I_DISK_GET_BLOCKSIZE:
		return SST25VF_BLOCK_ERASE_SIZE;
	case I_DISK_GET_DEVICE_ERASETIME:
		return SST25VF_CHIP_ERASE_TIME;
	case I_DISK_GET_BLOCK_ERASETIME:
		return SST25VF_BLOCK_ERASE_TIME;
	case I_DISK_GET_SIZE:
		return sst_cfg->size;
	default:
		return hwpl_spi_ioctl(cfg, request, ctl);
	}
	return 0;
}

int sst25vf_tmr_close(const device_cfg_t * cfg){
	sst25vf_share_power_down(cfg);
	return hwpl_spi_close(cfg);
}



/*! @} */


/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <errno.h>
#include <unistd.h>
#include "hwpl/pio.h"
#include "hwpl/spi.h"
#include "hwpl/debug.h"
#include "hwdl.h"
#include "sst25vf_local.h"

static int complete_spi_write(const device_cfg_t * cfg, uint32_t ignore);
static int continue_spi_write(const device_cfg_t * cfg, uint32_t ignore);

int sst25vf_open(const device_cfg_t * cfg){
	int err;
	uint8_t status;
	pio_attr_t attr;
	sst25vf_state_t * state = (sst25vf_state_t*)cfg->state;

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

	state->prot = 0;

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

int sst25vf_read(const device_cfg_t * cfg, device_transfer_t * rop){
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

static void assert_delay(void){
	int loop_max;
	int tmp;

	if ( _hwpl_core_getclock() <= 25000000 ){
		return;
	} else if ( _hwpl_core_getclock() > 100000000 ){
		loop_max = 75;
	} else if ( _hwpl_core_getclock() > 50000000 ){
		loop_max = 60;
	} else if ( _hwpl_core_getclock() > 25000000 ){
		loop_max = 30;
	}

	//Must be high for either 100ns (25MHz model) or 50ns (50MHz and 80MHz) models
	//This loop is 4 instructions --
	for(tmp = 0; tmp < loop_max; tmp++){
		asm volatile("nop\n");
	}
}

int continue_spi_write(const device_cfg_t * cfg, uint32_t ignore){
	sst25vf_state_t * state = (sst25vf_state_t *)cfg->state;
	int tmp;
	//should be called 10 us after complete_spi_write() executes

	sst25vf_share_deassert_cs(cfg);

	if( state->nbyte > 0 ){
		state->cmd[0] = SST25VF_INS_SEQ_PROGRAM;
		state->cmd[1] = state->buf[0];
		if ( state->nbyte > 1 ){
			state->cmd[2] = state->buf[1];
			tmp = 2;
		} else {
			state->cmd[2] = 0xFF;
			tmp = 1;
		}

		assert_delay();
		sst25vf_share_assert_cs(cfg);
		state->op.nbyte = 3;
		hwpl_spi_write(cfg, &(state->op));
		state->buf += tmp;
		state->nbyte -= tmp;
	} else {

		sst25vf_share_write_disable(cfg);
		sst25vf_share_write_dbsy(cfg);

		sst25vf_share_read_status(cfg);

		//Set the buffer to NULL to indicate the device is not busy
		state->buf = NULL;

		//call the event handler to show the operation is complete
		if ( state->callback != NULL ){
			state->callback(state->context, NULL);
			state->callback = NULL;
		}
	}
	return 0;
}

int complete_spi_write(const device_cfg_t * cfg, uint32_t ignore){
	hwpl_action_t action;
	sst25vf_cfg_t * sst_cfg = (sst25vf_cfg_t*)cfg->dcfg;
	sst25vf_state_t * state = (sst25vf_state_t*)cfg->state;

	//configure the GPIO to interrupt on a rising edge
	action.context = (void*)cfg;
	action.callback = (hwpl_callback_t)continue_spi_write;
	action.channel = sst_cfg->miso.pin;
	action.event = PIO_ACTION_EVENT_RISING;
	hwpl_pio_setaction(sst_cfg->miso.port, &action);

	sst25vf_share_deassert_cs(cfg);
	assert_delay();
	sst25vf_share_assert_cs(cfg);
	//continue_spi_write() will be called when the SPI flash is done writing and the GPIO is triggered
	if( state->nbyte > 0 ){
		return 1;
	} else {
		return 0;
	}
}

int sst25vf_write(const device_cfg_t * cfg, device_transfer_t * wop){
	int tmp;
	int err;
	uint8_t *addrp;
	sst25vf_state_t * state = (sst25vf_state_t*)cfg->state;
	//sst25vf_cfg_t * sst_cfg = (sst25vf_cfg_t*)cfg->dcfg;

	if( state->prot == 1 ){
		errno = EROFS;
		return -1;
	}

	if ( state->buf != NULL ){
		errno = EBUSY;
		return -1;
	}

	//This is the final callback and context when all the writing is done
	state->callback = wop->callback;
	state->context = wop->context;
	state->buf = wop->buf;
	state->nbyte = wop->nbyte;

	sst25vf_share_write_ebsy(cfg);
	sst25vf_share_write_enable(cfg);

	//set Auto increment
	addrp = (uint8_t*)&(wop->loc);
	state->cmd[0] = SST25VF_INS_SEQ_PROGRAM;
	state->cmd[1] = addrp[2];
	state->cmd[2] = addrp[1];
	state->cmd[3] = addrp[0];

	if ( wop->loc & 0x01 ){
		//starts on an odd address
		state->cmd[4] = 0xFF;
		state->cmd[5] = state->buf[0];
		tmp = 1;
	} else {
		state->cmd[4] = state->buf[0];
		if ( state->nbyte > 1 ){
			state->cmd[5] = state->buf[1];
			tmp = 2;
		} else {
			state->cmd[5] = 0xFF;
			tmp = 1;
		}
	}

	state->op.flags = wop->flags;
	state->op.callback = (hwpl_callback_t)complete_spi_write;
	state->op.context = (void*)cfg;
	state->op.cbuf = state->cmd;
	state->op.nbyte = 6;
	state->op.loc = 0;

	sst25vf_share_assert_cs(cfg);
	err = hwpl_spi_write(cfg, &state->op);
	state->buf = state->buf + tmp;
	state->nbyte = state->nbyte - tmp;
	return err;
}

int sst25vf_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	sst25vf_cfg_t * sst_cfg = (sst25vf_cfg_t*)cfg->dcfg;
	sst25vf_state_t * state = (sst25vf_state_t*)cfg->state;

	switch(request){
	case I_DISK_ERASE_BLOCK:
	case I_DISK_ERASE_DEVICE:
		if( state->prot == 1 ){
			errno = EROFS;
			return -1;
		}
		break;
	}

	switch(request){
	case I_DISK_PROTECT:
		sst25vf_share_global_protect(cfg);
		state->prot = 1;
		break;
	case I_DISK_UNPROTECT:
		sst25vf_share_global_unprotect(cfg);
		state->prot = 0;
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

int sst25vf_close(const device_cfg_t * cfg){
	return 0;
}



/*! @} */


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


void sst25vf_share_assert_cs(const device_cfg_t * cfg){
	hwpl_pio_clrmask(cfg->pcfg.spi.cs.port, (void*)(ssize_t)(1<<cfg->pcfg.spi.cs.pin));
}

void sst25vf_share_deassert_cs(const device_cfg_t * cfg){
	hwpl_pio_setmask(cfg->pcfg.spi.cs.port, (void*)(ssize_t)(1<<cfg->pcfg.spi.cs.pin));
}


void sst25vf_share_write_enable(const device_cfg_t * cfg){
	sst25vf_share_write_quick_opcode(cfg, SST25VF_INS_WRITE_ENABLE);
}

void sst25vf_share_write_disable(const device_cfg_t * cfg){
	sst25vf_share_write_quick_opcode(cfg, SST25VF_INS_WRITE_DISABLE);
}

void sst25vf_share_write_ebsy(const device_cfg_t * cfg){
	sst25vf_share_write_quick_opcode(cfg, SST25VF_INS_EBSY);
}

void sst25vf_share_write_dbsy(const device_cfg_t * cfg){
	sst25vf_share_write_quick_opcode(cfg, SST25VF_INS_DBSY);
}

void sst25vf_share_write_opcode_addr(const device_cfg_t * cfg, uint8_t opcode, uint32_t addr){
	uint8_t * addrp;
	addrp = (uint8_t*)&addr;
	//send the opcode
	hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)opcode);
	//send the 3-byte address MSB to LSB (assumes little-endian arch)
	hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)addrp[2]);
	hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)addrp[1]);
	hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)addrp[0]);
}

void sst25vf_share_write_quick_opcode(const device_cfg_t * cfg, uint8_t opcode){
	sst25vf_share_assert_cs(cfg);
	hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)opcode);
	sst25vf_share_deassert_cs(cfg);
}

void sst25vf_share_block_erase_4kb(const device_cfg_t * cfg, uint32_t addr /*! Any address in the 4KB block */){
	sst25vf_share_write_enable(cfg);
	sst25vf_share_assert_cs(cfg);
	sst25vf_share_write_opcode_addr(cfg, SST25VF_INS_BLOCK_ERASE_4KB, addr);
	sst25vf_share_deassert_cs(cfg);
}

void sst25vf_share_block_erase_32kb(const device_cfg_t * cfg, uint32_t addr /*! Any address in the 32KB block */){
	sst25vf_share_write_enable(cfg);
	sst25vf_share_assert_cs(cfg);
	sst25vf_share_write_opcode_addr(cfg, SST25VF_INS_BLOCK_ERASE_32KB, addr);
	sst25vf_share_deassert_cs(cfg);
}

void sst25vf_share_block_erase_64kb(const device_cfg_t * cfg, uint32_t addr /*! Any address in the 64KB block */){
	sst25vf_share_write_enable(cfg);
	sst25vf_share_assert_cs(cfg);
	sst25vf_share_write_opcode_addr(cfg, SST25VF_INS_BLOCK_ERASE_64KB, addr);
	sst25vf_share_deassert_cs(cfg);
}

void sst25vf_share_chip_erase(const device_cfg_t * cfg){
	sst25vf_share_write_enable(cfg);
	sst25vf_share_write_quick_opcode(cfg, SST25VF_INS_CHIP_ERASE);
	sst25vf_share_read_status(cfg);
}

void sst25vf_share_write_byte(const device_cfg_t * cfg, uint32_t addr, char byte){
	sst25vf_share_write_enable(cfg);
	sst25vf_share_assert_cs(cfg);
	sst25vf_share_write_opcode_addr(cfg, SST25VF_INS_PROGRAM, addr);
	hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)byte);
	sst25vf_share_deassert_cs(cfg);
}

char sst25vf_share_read_byte(const device_cfg_t * cfg, uint32_t addr){
	char byte;
	sst25vf_share_assert_cs(cfg);
	sst25vf_share_write_opcode_addr(cfg, SST25VF_INS_RD_HS, addr);
	hwpl_spi_swap(cfg->periph.port, 0); //dummy byte output
	byte = hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)0xFF);
	sst25vf_share_deassert_cs(cfg);
	return byte;
}

uint8_t sst25vf_share_read_status(const device_cfg_t * cfg){
	uint8_t status;
	sst25vf_share_assert_cs(cfg);
	hwpl_spi_swap(cfg->periph.port, (void*)SST25VF_INS_RD_STATUS);
	status = hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)0xFF);
	sst25vf_share_deassert_cs(cfg);
	return status;
}

void sst25vf_share_write_status(const device_cfg_t * cfg, uint8_t status){
	sst25vf_share_write_enable(cfg);
	sst25vf_share_assert_cs(cfg);
	hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)SST25VF_INS_WR_STATUS);
	hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)status);
	sst25vf_share_deassert_cs(cfg);
}

void sst25vf_share_power_down(const device_cfg_t * cfg){
	sst25vf_share_write_quick_opcode(cfg, SST25VF_INS_POWER_DOWN);
}

void sst25vf_share_power_up(const device_cfg_t * cfg){
	sst25vf_share_write_quick_opcode(cfg, SST25VF_INS_POWER_UP);
}

void sst25vf_share_global_protect(const device_cfg_t * cfg){
	//maninpulate the status registers
	uint8_t status;
	status = (1<<SST25VF_STATUS_SWPL_BIT) |
			(1<<SST25VF_STATUS_SWP_BIT0) |
			(1<<SST25VF_STATUS_SWP_BIT1) |
			(1<<SST25VF_STATUS_SWP_BIT2) |
			(1<<SST25VF_STATUS_SWP_BIT3);
	sst25vf_share_write_quick_opcode(cfg, SST25VF_INS_WR_STATUS_ENABLE);
	sst25vf_share_write_status(cfg, status); //global protect and set SWPL
}

int sst25vf_share_global_unprotect(const device_cfg_t * cfg){
	//manipulate the status registers
	sst25vf_share_write_quick_opcode(cfg, SST25VF_INS_WR_STATUS_ENABLE);
	sst25vf_share_write_status(cfg, (1<<SST25VF_STATUS_SWPL_BIT)); //global unprotect and set SPRL
	return 0;
}

void sst25vf_share_read_id(const device_cfg_t * cfg, char * dest){
	int i;
	sst25vf_share_assert_cs(cfg);
	hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)SST25VF_INS_RD_ID);
	for(i=0; i < 4; i++){
		dest[i] = hwpl_spi_swap(cfg->periph.port, (void*)(ssize_t)0xFF);
	}
	sst25vf_share_deassert_cs(cfg);
}




/*! @} */


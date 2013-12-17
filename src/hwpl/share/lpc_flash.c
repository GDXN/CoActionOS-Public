/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include "hwpl.h"
#include "lpc_flash_flags.h"
#include "hwpl/core.h"

int _hwpl_lpc_flash_write_page(int page, void * addr, const void * src, int size){
	iap_result_t result;
	iap_op_t cmd;

	//prepare the sector
	if ( (int)src & 0x03){
		return -1; //needs to be on a word (4 byte) boundary
	}
	switch(size){
	case 256:
	case 512:
	case 1024:
		break;
	default:
		return -1;
	}

	//! \todo If this goes over a page boundary it won't work
	cmd.sector.cmd = IAP_CMD_PREPARE_SECTOR;
	cmd.sector.start = (unsigned long)page;
	cmd.sector.end = (unsigned long)page;
	//iap_entry_wrapper, &args);
	_hwpl_iap_entry((unsigned*)&cmd, (unsigned*)&result);

	if (result.status != IAP_RESULT_CMD_SUCCESS){
		return -1;
	}

	//copy from ram to flash the sector (or page)
	cmd.mem.cmd = IAP_CMD_CPY_RAM_TO_FLASH;
	cmd.mem.src = (unsigned long)src;
	cmd.mem.dest = (unsigned long)addr;
	cmd.mem.size = (unsigned long)size;
	cmd.mem.cpu_freq = _hwpl_core_getclock() / 1000;
	_hwpl_iap_entry((unsigned*)&cmd, (unsigned*)&result);

	if (result.status != IAP_RESULT_CMD_SUCCESS){
		return -1;
	}
	return 0;
}

int _hwpl_lpc_flash_erase_page(int page){
	iap_result_t result;
	iap_op_t cmd;
	//prepare the sector
	cmd.sector.cmd = IAP_CMD_PREPARE_SECTOR;
	cmd.sector.start = page;
	cmd.sector.end = page;
	cmd.sector.cpu_freq = _hwpl_core_getclock() / 1000;
	_hwpl_iap_entry((unsigned*)&cmd, (unsigned*)&result);
	if (result.status != IAP_RESULT_CMD_SUCCESS){
		return -1;
	}

	//Erase the sector (or page)
	cmd.sector.cmd = IAP_CMD_ERASE_SECTOR;
	_hwpl_iap_entry((unsigned*)&cmd, (unsigned*)&result);
	if (result.status != IAP_RESULT_CMD_SUCCESS){
		return -1;
	}
	return 0;
}

int _hwpl_lpc_flash_get_serialno(uint32_t * dest){
	iap_result_t result;
	iap_op_t cmd;
	//prepare the sector
	cmd.sector.cmd = IAP_CMD_RD_SERIAL_NUMBER;
	cmd.sector.start = 0;
	cmd.sector.end = 0;
	cmd.sector.cpu_freq = 0;
	memset(result.dummy, 0, 4*sizeof(int));
	_hwpl_iap_entry((unsigned*)&cmd, (unsigned*)&result);
	if (result.status != IAP_RESULT_CMD_SUCCESS){
		return -1;
	}
	memcpy(dest, result.dummy, 4*sizeof(int));
	return 0;
}

void _hwpl_iap_entry(unsigned param_tab[], unsigned result_tab[]){
    void (*iap)(unsigned [],unsigned []);
    iap = (void (*)(unsigned [],unsigned []))IAP_ADDRESS;
    iap(param_tab,result_tab);
}


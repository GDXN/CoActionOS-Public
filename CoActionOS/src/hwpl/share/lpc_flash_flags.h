/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef IAP_H_
#define IAP_H_



#define IAP_LOCATION 0x1FFF1FF1


typedef union {
	struct __attribute__((packed)){
		unsigned cmd;
		unsigned start;
		unsigned end;
		unsigned cpu_freq /*! Cpu freq in KHz */;
		unsigned dummy;
	} sector;
	struct __attribute__((packed)){
		unsigned cmd;
		unsigned dest;
		unsigned src;
		unsigned size;
		unsigned cpu_freq /*! Clock in Khz */;
	} mem;
} iap_op_t;


typedef struct __attribute__((packed)){
	unsigned int status;
	unsigned int dummy[4];
} iap_result_t;

enum {
	IAP_CMD_PREPARE_SECTOR = 50,
	IAP_CMD_CPY_RAM_TO_FLASH,
	IAP_CMD_ERASE_SECTOR,
	IAP_CMD_BLANK_CHECK_SECTOR,
	IAP_CMD_RD_PART_ID,
	IAP_CMD_RD_BOOT_CODE_VERSION,
	IAP_CMD_CMP_ADDR,
	IAP_CMD_INVOKE_ISP,
	IAP_CMD_RD_SERIAL_NUMBER
};

enum {
	IAP_RESULT_CMD_SUCCESS,
	IAP_RESULT_INVALID_COMMAND,
	IAP_RESULT_SRC_ADDR_ERROR,
	IAP_RESULT_DST_ADDR_ERROR,
	IAP_RESULT_SRC_ADDR_NOT_MAPPED,
	IAP_RESULT_DST_ADDR_NOT_MAPPED,
	IAP_RESULT_COUNT_ERROR,
	IAP_RESULT_INVALID_SECTOR,
	IAP_RESULT_SECTOR_NOT_BLANK,
	IAP_RESULT_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,
	IAP_RESULT_COMPARE_ERROR,
	IAP_RESULT_BUSY
};


//Function is provided in ROM by linker
void _hwpl_iap_entry(unsigned param_tab[],unsigned result_tab[]);


#endif /* IAP_H_ */

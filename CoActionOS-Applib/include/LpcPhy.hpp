/*
 * LpcPhy.hpp
 *
 *  Created on: Jun 1, 2013
 *      Author: tgil
 */

#ifndef LPCPHY_HPP_
#define LPCPHY_HPP_

#include "Uart.hpp"
#include "Pin.hpp"
#include <stdint.h>

#define LPC_ISP_UNLOCK_CODE "23130"

#define LPCPHY_RAM_BUFFER_SIZE 512

class LpcPhy {
public:
	LpcPhy(Uart * uart, Pin * reset, Pin * ispreq){
		this->uart = uart;
		this->rst = reset;
		this->ispreq = ispreq;
		echo = 0;
		ram_buffer = 0x40000200;
	}


	int init(int pinassign);
	int exit(void);
	int open(int crystal);
	int close(void);
	int writemem(uint32_t loc, const void * buf, int nbyte, uint32_t sector);
	void set_ram_buffer(uint32_t addr);
	int readmem(uint32_t loc, void * buf, int nbyte);
	int reset(void);
	int start_bootloader(void);
	int sync_bootloader(uint32_t crystal);
	int sync_bootloader_lpc177x_8x(uint32_t crystal);
	int unlock(const char * unlock_code);
	int echo_off(void);
	int echo_on(void);
	int wr_ram(uint32_t ram_dest /*! The RAM destination address--must be a word boundary */,
			void * src /*! A pointer to the source data */,
			uint32_t size /*! The number of bytes to write--must be a multiple of 4 */);
	uint32_t rd_mem(void * dest /*! A pointer to the destination memory */,
			uint32_t src_addr /*! The source address to read--must be a word boundary */,
			uint32_t size /*! The number of bytes to read--must be a multiple of 4 */);
	int prep_sector(uint32_t start /*! The first sector to prepare */,
			uint32_t end /*! The last sector to prepare--must be >= start */);
	int cpy_ram_to_flash(uint32_t flash_addr /*! The flash address (destination memory) */,
			uint32_t ram_addr /*! The RAM address (source memory) */,
			uint32_t size /*! The number of bytes to copy */);
	int go(uint32_t addr /*! Where to start code execution */,
			char mode /*! 'T' for thumb mode and 'A' for arm mode--default is 'A' */);
	int erase_sector(uint32_t start /*! The first sector to erase */,
			uint32_t end /*! The last sector to erase--must be >= start */);
	int blank_check_sector(uint32_t start /*! The first sector to blank check */,
			uint32_t end /*! The last sector to blank check--must be >= start */);
	uint32_t rd_part_id(void);
	uint32_t rd_boot_version(void);
	int memcmp(uint32_t addr0 /*! The beginning of the first block */,
			uint32_t addr1 /*! The beginning of the second block */,
			uint32_t size /*! The number of bytes to compare */);

private:
	char echo;
	Uart * uart;
	Pin * ispreq;
	Pin * rst;
	uint32_t ram_buffer;
	int32_t lpc_write_data(void * src, uint32_t size);
	int32_t lpc_read_data(void * dest, uint32_t size);
	int get_line(void * buf, int nbyte, int max_wait);
	int lpc_rd_return_code(uint16_t timeout);
	int lpc_wait_response(const char * response, uint16_t timeout);
	int lpc_wait_ok(uint16_t timeout);

	bool islpc177x_8x_;
	bool islpc177x_8x(){ return islpc177x_8x_; }
	void setlpc177x_8x(bool enabled = true){ islpc177x_8x_ = enabled; }

	int sendcommand(const char * cmd, int timeout, int wait_ms = 0);

	int flush(void);

};

#endif /* LPCISP_HPP_ */

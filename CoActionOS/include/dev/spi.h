/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SPI Serial Peripheral Interface (SPI) Master
 * @{
 *
 * \ingroup PERIPHIO
 *
 * \details This module implements a hardware SPI master.  The chip select functionality
 * is not part of this module and must be implemented separately.  This allows the application
 * to use the SPI bus to access multiple devices.  More information about accessing peripheral IO
 * is in the \ref PERIPHIO section.
 *
 *
 * The following is an example of how to read/write the SPI in an OS environment:
 *
 * \code
 *
 * #include <unistd.h>
 * #include <fcntl.h>
 * #include <errno.h>
 * #include <stdio.h>
 * #include "hwpl.h"
 *
 * int read_write_spi(void){
 * 	int fd;
 * 	spi_attr_t attr;
 * 	char buffer[16];
 * 	char duplex_buffer[16];
 *
 * 	fd = open("/dev/spi0", O_RDWR); //Here O_RDWR could be OR'd with O_NONBLOCK for non blocking operation
 * 	if ( fd < 0 ){
 * 		printf("Error opening peripheral (%d)\n", errno);
 * 	} else {
 *		attr.bitrate = 1000000; //1MHz bitrate
 * 		attr.format = SPI_ATTR_FORMAT_SPI;
 * 		attr.pin_assign = 0; //Use GPIO configuration zero (see device specific documentation for details)
 * 		attr.master = SPI_ATTR_MASTER;
 * 		attr.mode = SPI_ATTR_MODE0;
 * 		attr.width = 8;
 * 		if( ioctl(fd, I_SETATTR, &attr) < 0 ){
 * 			printf("Failed to set peripheral configuration (%d)\n", errno);
 * 			return -1;
 *		}
 *
 *		//Now read or write the SPI in half duplex mode
 *		strcpy(buffer, "1/2 Duplex Test\n");
 *		if ( write(fd, buffer, strlen(buffer) < 0 ){ //returns after all bytes have been written
 *			printf("Failed to write peripheral (%d)\n", errno);
 *		}
 *
 *		if ( read(fd, buffer, 16) < 0 ){ //Return after all bytes have been read
 *			printf("Failed to read peripheral (%d)\n", errno);
 *		}
 *
 *		//Now read or write the SPI in full duplex mode
 *
 *		if ( ioctl(fd, I_SPI_SETDUPLEX, duplex_buffer) < 0 ){
 *			printf("Failed to configure duplex buffer (%d)\n", errno);
 *		}
 *
 *		strcpy(buffer, "Full Duplex Test\n");
 *		if ( write(fd, buffer, strlen(buffer) < 0 ){ //returns after all bytes have been written
 *			printf("Failed to write peripheral (%d)\n", errno);
 *		}
 *
 *		if ( read(fd, buffer, 16) < 0 ){ //doesn't return until 16 bytes arrive (use O_NONBLOCK to return with whatever is available)
 *			printf("Failed to read peripheral (%d)\n", errno);
 *		}
 *
 *		//This puts the SPI back in half-duplex mode
 *		if ( ioctl(fd, I_SPI_SETDUPLEX, NULL) < 0 ){
 *			printf("Failed to configure duplex buffer (%d)\n", errno);
 *		}
 *
 * 	}
 * 	close(fd);
 * 	return 0;
 * }
 *
 * \endcode
 *
 */

/*! \file
 * \brief Serial Peripheral Interface Header File
 *
 */

#ifndef DEV_SPI_H_
#define DEV_SPI_H_

#include <stdint.h>
#include "ioctl.h"
#include "hwpl/arch.h"

#ifdef __cplusplus
extern "C" {
#endif


#define SPI_IOC_IDENT_CHAR 's'

/*! \details This lists the valid SPI mode types.
 *
 */
enum {
	SPI_ATTR_MODE0 /*! SPI Mode 0 */,
	SPI_ATTR_MODE1 /*! SPI Mode 1 */,
	SPI_ATTR_MODE2 /*! SPI Mode 2 */,
	SPI_ATTR_MODE3 /*! SPI Mode 3 */
};

/*! \brief This specifies the attribute master value for SPI master.
 *
 */
#define SPI_ATTR_MASTER 1

/*! \brief This specifies the attribute master value for SPI slave.
 * This value can't be used with spi_ioctl().  It must
 * be used with spi_slave_ioctl().
 *
 */
#define SPI_ATTR_SLAVE 0

enum {
	SPI_ATTR_FORMAT_SPI /*! SPI Format */,
	SPI_ATTR_FORMAT_TI  /*! TI Format */,
	SPI_ATTR_FORMAT_MICROWIRE /*! Microwire Format */
};

/*! \brief SPI Attribute Data Structure
 * \details This structure defines how the control structure
 * for opening or reconfiguring the SPI port.
 */
typedef struct HWPL_PACK {
	uint8_t pin_assign /*! \brief The GPIO configuration to use (see \ref LPC17XXDEV) */;
	uint32_t bitrate /*! \brief The SPI bitrate */;
	uint8_t mode /*! \brief The SPI mode from 0 to 3 inclusive (CPOL:CPHA or SPI_ATTR_MODE0 to SPI_ATTR_MODE3) */;
	uint8_t master /*! \brief Must be set to master (SPI_ATTR_MASTER) */;
	uint8_t width /*! \brief The transfer width */;
	uint8_t format /*! \brief The SPI output format (SPI_ATTR_FORMAT_SPI, SPI_ATTR_FORMAT_TI or SPI_ATTR_FORMAT_MICROWIRE) */;
} spi_attr_t;


typedef hwpl_action_t spi_action_t;

/*! \brief This request gets the SPI attributes.
 * \hideinitializer
 */
#define I_SPI_GETATTR _IOCTLR(SPI_IOC_IDENT_CHAR, I_GLOBAL_GETATTR, spi_attr_t)
/*! \brief This request sets the SPI attributes.
 * \hideinitializer
 */
#define I_SPI_SETATTR _IOCTLW(SPI_IOC_IDENT_CHAR, I_GLOBAL_SETATTR, spi_attr_t)
#define I_SPI_SETACTION _IOCTLW(SPI_IOC_IDENT_CHAR, I_GLOBAL_SETACTION, spi_action_t)


/*! \brief See details below.
 * \details This tells the SPI bus to
 * get a swap a byte from the bus.  spi_ioctl() returns the
 * word read from the bus;
 * The ctl argument to spi_ioctl() is the
 * word to put (not a pointer to spi_attr_t).
 *
 * The following example puts 0xAC on the bus and stores
 * whatever is read from the bus in swapped_word.
 * \code
 * int swapped_word;
 * swapped_word = ioctl(spi_fd, I_SPI_SWAP, 0xAC);
 * \endcode
 * \hideinitializer
 */
#define I_SPI_SWAP _IOCTL(SPI_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 0)

/*! \brief See details below.
 * \details This tells the SPI bus to perform full or half duplex
 * operations.
 * The ctl argument to spi_ioctl() is the pointer
 * to the buffer where the duplex memory is stored.  If the ctl argument is NULL,
 * half duplex operation is enabled.  Otherwise, reads and writes are full duplex.  For example,
 * on read() operations, the SPI bus will be written with
 * the contents of the duplex pointer.
 * \hideinitializer
 */
#define I_SPI_SETDUPLEX _IOCTL(SPI_IOC_IDENT_CHAR, I_GLOBAL_TOTAL + 1)
#define I_SPI_SET_DUPLEX I_SPI_SETDUPLEX

#define I_SPI_TOTAL 2

#ifdef __cplusplus
}
#endif


#endif /* DEV_SPI_H_ */

/*! @} */

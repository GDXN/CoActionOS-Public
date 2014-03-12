/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup BOOTLOADER_DEV Bootloader Access
 * @{
 *
 * \ingroup PERIPHIO
 *
 * \details The Bootloader provides the IOCTL definitions for accessing the bootloader.  This interface
 * is not available on any device when CoActionOS is running.  It is only available
 * to the bootloader.
 *
 */

/*! \file
 * \brief Analog to Digital Converter Header File
 *
 */

#ifndef DEV_BOOTLOADER_H_
#define DEV_BOOTLOADER_H_

#include <stdint.h>
#include "ioctl.h"
#include "hwpl/arch.h"
#include "hwpl/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOOTLOADER_IOC_IDENT_CHAR 'b'

/*! \brief Bootloader attributes.
 * \details This structure contains the attributes for the bootloader.
 */
typedef struct HWPL_PACK {
	uint32_t version /*! \brief The bootloader version */;
	uint32_t serialno[4] /*! \brief The device serial number */;
	uint32_t startaddr /*! \brief The start address of the target program (read-only) */;
} bootloader_attr_t;


/*! \brief This is the size of a bootloader flash page.
 */
#define BOOTLOADER_WRITEPAGESIZE 1024

/*! \brief See details below.
 * \details This structure is used for writing
 * data to the bootloader flash using \ref I_BOOTLOADER_WRITEPAGE.
 */
typedef struct HWPL_PACK {
	uint32_t addr /*! \brief The address to write to */;
	uint32_t nbyte /*! \brief The number of bytes to write */;
	uint8_t buf[BOOTLOADER_WRITEPAGESIZE] /*! \brief A buffer for writing to the flash */;
} bootloader_writepage_t;


/*! \brief See below for details.
 * \details This request sets the functionality of the specified pin.
 *
 */
#define I_BOOTLOADER_ERASE _IOCTL(BOOTLOADER_IOC_IDENT_CHAR, 0)

/*! \brief See below for details.
 * \details This request loads the bootloader attributes.
 *
 * \code
 * bootloader_attr_t attr;
 * link_ioctl(LINK_BOOTLOADER_FILDES, I_BOOTLOADER_GETATTR, &attr);
 * \endcode
 */
#define I_BOOTLOADER_GETATTR _IOCTLR(BOOTLOADER_IOC_IDENT_CHAR, 1, bootloader_attr_t)

/*! \brief See below for details.
 * \details This request invokes a reset.  The third IOCTL argument is a
 * boolean value saying whether or not to start the bootloader.  If this value
 * is false, the application is loaded.
 *
 * \code
 * bool invoke_bootloader = false;
 * link_ioctl(LINK_BOOTLOADER_FILDES, I_BOOTLOADER_RESET, NULL);
 * \endcode
 */
#define I_BOOTLOADER_RESET _IOCTL(BOOTLOADER_IOC_IDENT_CHAR, 2)

/*! \brief See below for details.
 * \details This request writes a page to the flash memory.
 *
 * \code
 * bootloader_writepage_t attr;
 * attr.loc = 0x1000;
 * attr.nbyte = 1024;
 * memset(attr.buf, 0xFF, FLASH_MAX_WRITE_SIZE);
 *  //copy data in to attr.buf up to FLASH_MAX_WRITE_SIZE bytes
 * ioctl(fd, I_BOOTLOADER_WRITEPAGE, &attr);
 * \endcode
 */
#define I_BOOTLOADER_WRITEPAGE _IOCTLW(BOOTLOADER_IOC_IDENT_CHAR, 3, bootloader_writepage_t)

#define I_BOOTLOADER_TOTAL 4

#ifdef __cplusplus
}
#endif


#endif // DEV_BOOTLOADER_H_

/*! @} */


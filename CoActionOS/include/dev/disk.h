/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup DISK Disk
 * @{
 *
 *
 * \ingroup SYS
 *
 * \details This is the interface for accessing disks.  The disk can be any external storage device such
 * as an SD card, serial flash chip, etc.  Disks are always read/write block devices.
 *
 *
 */

/*! \file  */

#ifndef DEV_DISK_H_
#define DEV_DISK_H_

#include <stdint.h>
#include "dev/ioctl.h"

#define DISK_IOC_CHAR 'd'


/*! \details This request applies the software write protect
 * to the entire device.
 *
 * Example:
 * \code
 * ioctl(fildes, I_DISK_PROTECT);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_PROTECT _IOCTL(DISK_IOC_CHAR, 0)

/*! \details This request removes the software write protect
 * from the entire device.
 *
 * Example:
 * \code
 * ioctl(fildes, I_DISK_UNPROTECT);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_UNPROTECT _IOCTL(DISK_IOC_CHAR, 1)

/*! \details This request erases a block of memory.  The ctl argurment
 * is an address that is contained in the block to erase.
 *
 * Example:
 * \code
 * ioctl(fildes, I_DISK_ERASEBLOCK, 0x1000);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_ERASEBLOCK _IOCTL(DISK_IOC_CHAR, 2)
#define I_DISK_ERASE_BLOCK I_DISK_ERASEBLOCK

/*! \details This request erases the device.
 *
 * Example:
 * \code
 * ioctl(fildes, I_DISK_ERASEDEVICE);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_ERASEDEVICE _IOCTL(DISK_IOC_CHAR, 3)
#define I_DISK_ERASE_DEVICE I_DISK_ERASEDEVICE

/*! \details This request powers down the device.
 *
 * Example:
 * \code
 * ioctl(fildes, I_DISK_POWERDOWN);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_POWERDOWN _IOCTL(DISK_IOC_CHAR, 4)
#define I_DISK_POWER_DOWN I_DISK_POWERDOWN

/*! \details This request powers up the device.
 *
 * Example:
 * \code
 * ioctl(fildes, I_DISK_POWER_UP);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_POWERUP _IOCTL(DISK_IOC_CHAR, 5)
#define I_DISK_POWER_UP I_DISK_POWERUP

/*! \details This request gets the size of the memory on the device.
 *
 * Example:
 * \code
 * uint32_t size;
 * size = ioctl(fildes, I_DISK_GETSIZE);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_GETSIZE _IOCTL(DISK_IOC_CHAR, 6)
#define I_DISK_GET_SIZE I_DISK_GETSIZE

/*! \details This request gets the size of the smallest eraseable block.
 *
 * Example:
 * \code
 * uint32_t size;
 * size = ioctl(fildes, I_DISK_GETBLOCKSIZE);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_GETBLOCKSIZE _IOCTL(DISK_IOC_CHAR, 7)
#define I_DISK_GET_BLOCKSIZE I_DISK_GETBLOCKSIZE

/*! \details This request gets the amount of time to erase a block.
 *
 * Example:
 * \code
 * uint32_t useconds;
 * useconds = ioctl(fildes, I_DISK_GETBLOCKERASETIME);
 * ioctl(fildes, I_DISK_ERASEBLOCK, 0x1000);
 * usleep(useconds);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_GETBLOCKERASETIME _IOCTL(DISK_IOC_CHAR, 8)
#define I_DISK_GET_BLOCK_ERASETIME I_DISK_GETBLOCKERASETIME

/*! \details This request gets the amount of time required to erase the device.
 *
 * Example:
 * \code
 * uint32_t useconds;
 * useconds = ioctl(fildes, I_DISK_GETDEVICEERASETIME);
 * ioctl(fildes, I_DISK_ERASEDEVICE, 0x1000);
 * usleep(useconds);
 * \endcode
 * \hideinitializer
 */
#define I_DISK_GETDEVICEERASETIME _IOCTL(DISK_IOC_CHAR, 9)
#define I_DISK_GET_DEVICE_ERASETIME I_DISK_GETDEVICEERASETIME

#define I_DISK_TOTAL 10

#endif /* DEV_DISK_H_ */

/*! @} */

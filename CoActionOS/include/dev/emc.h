/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup EMC External Memory Controller (EMC)
 * @{
 *
 * \ingroup PERIPHIO
 *
 * \details The EMC API allows configuration of the external memory controller.
 *
 * The EMC is usually initialized by the OS at startup using the following sample code:
 *
 * \code
 *
 *
 * \endcode
 *
 */

/*! \file
 * \brief External Interrupt Header File
 *
 */


#ifndef DEV_EMC_H_
#define DEV_EMC_H_

#include <stdint.h>
#include "ioctl.h"
#include "hwpl/arch.h"


#define EMC_IOC_IDENT_CHAR 'E'

/*! \brief See below for details */
/*! \details These are the flags for mode in emc_attr_t */
typedef enum {
	EMC_MODE_BUS8 /*! 8-bit bus width */ = (1<<0),
	EMC_MODE_BUS16 /*! 16-bit bus width */ = (1<<1),
	EMC_MODE_BUS32 /*! 32-bit bus width (default) */ = 0,
	EMC_MODE_STATIC /*! Static memory (default) */ = 0,
	EMC_MODE_DYNAMIC /*! Dynamic memory */ = (1<<2),
	EMC_MODE_PAGEMODE /*! Page mode */ = (1<<3),
	EMC_MODE_CSPOL_HIGH /*! Active high chip select */ = (1<<4),
	EMC_MODE_CSPOL_LOW /*! Active high chip select (default) */ = 0,
	EMC_MODE_BYTELANE_HIGH /*! Byte lane bytes are high for reads and low for writes (default) */ = 0,
	EMC_MODE_BYTELANE_LOW /*! Byte lane bytes are low for reads and writes */ = (1<<5),
	EMC_MODE_EXTWAIT /*! Enable Extended wait */ = (1<<6),
	EMC_MODE_BUFFER /*! Enable buffer */ = (1<<7),
	EMC_MODE_WP /*! Enable write protect */ = (1<<8),
	EMC_MODE_LITTLE_ENDIAN /*! Little Endian mode (default) */ = 0,
	EMC_MODE_BIT_ENDIAN /*! Little Endian mode (default) */ = (1<<9),
	EMC_MODE_ADDR_MIRROR /*! Address mirror */ = (1<<10),
	EMC_MODE_USEBLS /*! Enable BLS signal */ = (1<<11)
} emc_mode_t;

typedef struct HWPL_PACK {
	uint8_t port /*! EMC port */;
	uint8_t channel /*! EMC Channel (chip select line) */;
	uint8_t pin_assign /*! EMC Pin assignment */;
	uint8_t wait_write_enable /*! Clock delay from chip select to write enable */;
	uint8_t wait_output_enable /*! Clock delay from chip select to output enable */;
	uint8_t wait_rd /*! Read mode delay */;
	uint8_t wait_page /*! Number of wait states for async page mode read */;
	uint8_t wait_wr /*! Write mode delay (SRAM) */;
	uint8_t wait_turn /*! Turn around cycles */;
	uint8_t addr_width /*! The width of the address bus */;
	uint32_t mode /*! Mode flags */;
	uint32_t size /*! Size of the external memory */;
	uint32_t clock /*! Maximum clock rate */;
} emc_attr_t;



#define I_EMC_GETATTR _IOCTLR(EMC_IOC_IDENT_CHAR, I_GLOBAL_GETATTR, eint_attr_t)

/*! \brief IOCTL request to set the attributes
 */
#define I_EMC_SETATTR _IOCTLW(EMC_IOC_IDENT_CHAR, I_GLOBAL_SETATTR, eint_attr_t)

#define I_EMC_SETACTION _IOCTLW(EMC_IOC_IDENT_CHAR, I_GLOBAL_SETACTION, eint_action_t)

#define I_EMC_TOTAL 0


#endif /* DEV_EMC_H_ */

/*! @} */

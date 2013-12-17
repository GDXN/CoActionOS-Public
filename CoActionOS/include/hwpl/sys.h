/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYS_DEV SYS
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_SYS_H_
#define _HWPL_SYS_H_


#include "../device.h"
#include "../dev/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t _hwpl_sys_getsecurity(void);
void _hwpl_sys_setsecuritylock(bool enabled);

#ifdef __cplusplus
}
#endif

#endif /* _HWPL_SYS_H_ */

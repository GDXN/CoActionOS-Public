/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup MEM_DEV Memory
 * @{
 *
 * \ingroup DEV
 */

#ifndef _HWPL_MEM_H_
#define _HWPL_MEM_H_


#include "../device.h"
#include "../dev/mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details
 *
 * The following is an example of how to open the EINT:
 * \code
 * mem_attr_t mem_cfg;
 * if ( mem_open(0, &mem_cfg) ){
 *  //Failed
 * }
 * \endcode
 *
 * \sa periph_open()
 *
 */
int hwpl_mem_open(const device_cfg_t * cfg);
/*! \details
 * \sa periph_read()
 *
 */
int hwpl_mem_read(const device_cfg_t * cfg, device_transfer_t * rop);
/*! \details
 * \sa periph_write()
 */
int hwpl_mem_write(const device_cfg_t * cfg, device_transfer_t * wop);
/*! \details
 * \sa periph_ioctl()
 *
 */
int hwpl_mem_ioctl(const device_cfg_t * cfg, int request, void * ctl);
/*! \details
 * \sa periph_close()
 */
int hwpl_mem_close(const device_cfg_t * cfg);


int hwpl_mem_getattr(int port, void * ctl);
int hwpl_mem_setattr(int port, void * ctl);
int hwpl_mem_setaction(int port, void * ctl);

int hwpl_mem_eraseaddr(int port, void * ctl);
int hwpl_mem_erasepage(int port, void * ctl);
int hwpl_mem_getpage(int port, void * ctl);
int hwpl_mem_getsize(int port, void * ctl);
int hwpl_mem_getpageinfo(int port, void * ctl);
int hwpl_mem_writepage(int port, void * ctl);

int _hwpl_mem_getsyspage(void);


#ifdef __cplusplus
}
#endif


#endif // _HWPL_MEM_H_

/*! @} */




#ifndef APPLIB_FLASH_H_
#define APPLIB_FLASH_H_

#include <stdint.h>
#include <dev/flash.h>

#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

int flash_open(port_t port);
int flash_close(port_t port);
int flash_getattr(port_t port, flash_attr_t * attr);
int flash_setattr(port_t port, flash_attr_t * attr);
int flash_eraseaddr(port_t port, uint32_t addr);
int flash_erasepage(port_t port, int page);
int flash_getpage(port_t port, uint32_t addr);
int flash_getsize(port_t port);
int flash_getpageinfo(port_t port, flash_pageinfo_t * info);
int flash_writepage(port_t port, flash_writepage_t * wattr);
int flash_read(port_t port, int loc, void * buf, int nbyte);
int flash_write(port_t port, int loc, const void * buf, int nbyte);

#ifdef __cplusplus
}
#endif

#endif /* APPLIB_FLASH_H_ */

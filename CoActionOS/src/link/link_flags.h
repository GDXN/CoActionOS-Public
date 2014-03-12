/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#ifndef LINK_FLAGS_H_
#define LINK_FLAGS_H_

#include "hwpl.h"
#include "link_protocol.h"


extern link_dev_t link_dev;

#define LINK_PHY_NAME_MAX 1024


int link_handle_err(link_phy_t handle, int err);

int link_ioctl_delay(link_phy_t handle, int fildes, int request, void * argp, int arg, int delay);


//Functions
int link_rd_err(link_dev_t dev);

#define SET_FD_PORT(fd,port) (fd|(port<<8))
#define GET_FD(fd_port) (fd_port & 0xFF)
#define GET_PORT(fd_port) (fd_port >> 8)

extern int link_debug_level;
#define link_debug(x, ...) do { if ( link_debug_level >= x ) { if ( link_debug_level > 1 ) printf("%s():", __func__ ); printf(__VA_ARGS__); fflush(stdout); } } while(0)
#define link_error(...) do { if ( link_debug_level >= 1 ) { if ( link_debug_level > 1 ) printf("%s():", __func__ ); printf(__VA_ARGS__); fflush(stdout); } } while(0)



#endif /* LINK_FLAGS_H_ */

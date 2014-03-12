/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <stdbool.h>

#include "hwpl.h"
#include "link_flags.h"



int link_open_stdio(link_phy_t handle /*, const char * name */){
	return 0;
}


int link_close_stdio(link_phy_t handle){
	return 0;
}

int link_read_stdout(link_phy_t handle, void * buf, int nbyte){
	return link_phy_read(handle, buf, nbyte);
}


int link_write_stdin(link_phy_t handle, const void * buf, int nbyte){
	return link_phy_write(handle, buf, nbyte);
}

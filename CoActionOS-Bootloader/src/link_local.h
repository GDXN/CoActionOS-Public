/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#ifndef LINK_LOCAL_H_
#define LINK_LOCAL_H_

//#include "config.h"
#include <stddef.h>

#include "usb_dev.h"
#include "usb_dev_std_inline.h"

int link_phyopen(void);
int link_phyread(void *buf, int nbyte);
int link_phywrite(void *buf, int nbyte);
int link_phyclose(void);


#endif /* LINK_LOCAL_H_ */

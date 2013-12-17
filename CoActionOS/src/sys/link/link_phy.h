/*
 * link_phy.h
 *
 *  Created on: Aug 2, 2013
 *      Author: tgil
 */

#ifndef LINK_PHY_H_
#define LINK_PHY_H_

#include "link_local.h"

link_phy_t link_phy_open(const char * name, int baudrate);
int link_phy_write(link_phy_t handle, const void * buf, int nbyte);
int link_phy_read(link_phy_t handle, void * buf, int nbyte);
int link_phy_close(link_phy_t handle);
void link_phy_wait(int msec);
void link_phy_flush(link_phy_t handle);

#endif /* LINK_PHY_H_ */

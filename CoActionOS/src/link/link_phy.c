/*
 * link_phy_posix.c
 *
 *  Created on: Oct 6, 2012
 *      Author: tgil
 */


//#include "config.h"

#ifdef __win32
#include "win32/link_phy_win32.c"
#endif

#ifdef __win64
#include "win32/link_phy_win32.c"
#endif


#ifdef __macosx
#include "posix/link_phy_posix.c"
#endif

#ifdef __linux
#include "posix/link_phy_posix.c"
#endif




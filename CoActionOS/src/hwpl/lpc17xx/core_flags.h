/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef CORE_FLAGS_H_
#define CORE_FLAGS_H_



#define OSCRANGE 4
#define OSCEN 5
#define OSCSTAT 6
#define PLLE0 0
#define PLLC0 1
#define PLLE0_STAT 24
#define PLLC0_STAT 25
#define PLOCK0 26



#define PLLE1 0
#define PLLC1 1
#define PLLE1_STAT 8
#define PLLC1_STAT 9
#define PLOCK1 10

#define IRC_OSC 0
#define MAIN_OSC 1


#define PM1 (1<<1)
#define PM0 (1<<0)
#define PM_MASK (0x03)




#endif /* CORE_FLAGS_H_ */

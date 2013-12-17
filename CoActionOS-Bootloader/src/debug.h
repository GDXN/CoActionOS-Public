/*
 * debug.h
 *
 *  Created on: Sep 28, 2013
 *      Author: tgil
 */

#ifndef DEBUG_BOOTLOADER_H_
#define DEBUG_BOOTLOADER_H_

#include "config.h"

#ifdef DEBUG_BOOTLOADER
#include <hwpl/debug.h>
#include <applib/debug.h>
#else
#define dstr(a)
#define dint(a)
#define dhex(a)
#endif




#endif /* DEBUG_BOOTLOADER_H_ */

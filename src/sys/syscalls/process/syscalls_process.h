/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup SYSCALLS_PROCESS
 * @{
 */

/*! \file */

#ifndef SYSCALLS_PROCESS_H_
#define SYSCALLS_PROCESS_H_

#include "config.h"

#include <stdint.h>
#include <unistd.h>

int process_start(const char *path, char *const argv[], char *const envp[]);

#endif /* SYSCALLS_PROCESS_H_ */


/*! @} */

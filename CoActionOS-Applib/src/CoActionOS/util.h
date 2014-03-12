/*
 * utili.h
 *
 *  Created on: Oct 13, 2012
 *      Author: tgil
 */

#ifndef UTILI_H_
#define UTILI_H_

#include <stdbool.h>
#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NO_PORT_NUMBER 0xAAEE5566

int phylib_util_open(const char * name, port_t port, int * fd, int total, bool * init);
int phylib_util_close(port_t port, int * fd);

#ifdef __cplusplus
}
#endif

#endif /* UTILI_H_ */

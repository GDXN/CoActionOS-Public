/*
 * Pchar.cpp
 *
 *  Created on: Oct 14, 2012
 *      Author: tgil
 */

#include <unistd.h>
#include "Pchar.hpp"
#include "Timer.hpp"

Pchar::Pchar(port_t port, int numports, int * fd, bool * isinitialized) : Periph(port, numports, fd, isinitialized) {}

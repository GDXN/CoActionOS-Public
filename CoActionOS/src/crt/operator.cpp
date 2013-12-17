/*
 * operator.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: tgil
 */

#include <cstdlib>

extern "C" void * operator new(size_t size){
    return malloc(size);
}

extern "C" void operator delete(void * p){
    free(p);
}


/*
 * Signal.cpp
 *
 *  Created on: Dec 6, 2013
 *      Author: tgil
 */

#include "Signal.hpp"


using namespace Signal;

int Event::set_handler(Handler * handler) const {
	if( handler->sigaction()->sa_flags & SA_SIGINFO ){
		return ::sigaction(signo_, handler->sigaction(), 0);
	} else {
		_sig_func_ptr ptr = handler->sigaction()->sa_handler;
		::signal(signo_, ptr);
		return 0;
	}
}


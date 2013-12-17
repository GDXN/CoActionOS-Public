/*
 * State.cpp
 *
 *  Created on: Jul 5, 2013
 *      Author: tgil
 */

#include "StringTypes.hpp"
#include "State.hpp"

State::State() {
	// TODO Auto-generated constructor stub

}


State * State::state(State * list[], const char * name){
	StringSmall sname;
	State * ptr;
	int i;

	if( list == 0 ){
		return 0;
	}

	for(i=0; list[i] != 0; i++){
		ptr = list[i];
		sname = ptr->name();
		if( sname == name ){
			return ptr;
		}
		ptr++;
	}
	return invalid();
}

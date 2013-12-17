/*
 * SMach.cpp
 *
 *  Created on: Jul 5, 2013
 *      Author: tgil
 */

#include <cstdio>
#include "SMach.hpp"

bool SMach::action(void){
	State * next;
	//Execute the state action
	if( current_->action() == false ){
		return false;
	}

	//Check for a transition to another state
	next = current_->exit_condition(list);
	if( (next != 0) && (next != State::invalid()) ){
		if( next->entry_action() == false ){
			return false;
		}
		current_ = next;
	}

	if( next == State::invalid() ){
		return false;
	}

	return true;
}

bool SMach::exec(State * list[], SMach * current){
	bool ret;
	State * next;
	State * state;

	state = current;

	ret = state->entry_action();
	if( ret == false ){
		return false;
	}

	do {
		ret = state->action();
		if( ret == false ){
			return false;
		}
		next = state->exit_condition(list);
		if( (next != 0) && (next != State::invalid()) ){
			state = next;
			ret = state->entry_action();
			if( ret == false ){
				return false;
			}
		}
	} while( State::valid(next) );
	return false;
}

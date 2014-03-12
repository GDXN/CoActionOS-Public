/*! \file */

#ifndef SMACH_HPP_
#define SMACH_HPP_

#include "State.hpp"

/*! \brief State Machine class */
/*! \details The State Machine class is a lightweight, powerful
 * mechanism for implementing state machines.  SMach inherits
 * State so that state machines can execute within state machines.
 *
 */
class SMach : public State {
public:
	SMach(State * list[], State * current) : list(list), current_(current){
		if( this->current_ != 0 ){
			this->current_->entry_action();
		}
	}

	/*! \brief Execute the top level state machine (blocking) */
	static bool exec(State * list[], SMach * current);

	/*! \brief Action to run this state machine (non-blocking) */
	virtual bool action(void);

	/*! \brief Return a pointer to the current state */
	State * current(){ return current_; }

private:
	State ** list;
	State * current_;

};

#endif /* SMACH_HPP_ */

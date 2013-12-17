/*! \file */

#ifndef STATE_HPP_
#define STATE_HPP_

/*! \brief State class (part of a state machine -- SMach) */
class State {
public:
	State();

	//This will be deprecated -- use find
	static State * state(State * list[], const char * name);

	/*! \brief Searches a state \a list for \a name */
	static inline State * find(State * list[], const char * name){
		return state(list, name);
	}

	/*! \brief Checks to see if \a ptr is a valid state */
	static bool valid(State * ptr){
		if( ptr == (State*)-1 ){
			return false;
		}
		return true;
	}

	/*! \brief Returns a pointer to an invalid state */
	static State * invalid(){ return (State*)-1; }

	/*! \brief Returns the name of a state */
	virtual const char * name(){ return 0; }

	/*! \brief Action called when entering state */
	virtual bool entry_action(void){ return true; }

	/*! \brief Action called when state is active */
	virtual bool action(void){ return true; }

	/*! \brief Checks to see if it is time to exit the state (optionally executes an exit action)*/
	virtual State * exit_condition(State * list[]){ return 0; }

};

#endif /* STATE_HPP_ */

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

	/*! \brief Called when exiting to the state specified by \a name */
	/*! \details This is called when the state is about to exit.
	 *
	 * @param name Name of target state
	 * @return \a true if it is OK to proceed with transition and \a false to abort transition
	 */
	virtual bool transition(const char * name){ return true; }

	/*! \brief Exit condition list entry */
	/*! \details This details the structure of an exit condition.
	 *
	 */
	typedef struct {
		const char * state /*! \brief The next state (if condition is true) */;
		const char * desc /*! \brief A description of the condition (e.g. button press) */;
		bool (*condition)(void) /*! \brief a function to execute to see if the condition is true */;
	} transition_t;

	virtual const transition_t * transition_list(void) const { return 0; }
	virtual int transition_size(void) const { return 0; }

	static bool istrue(){ return true; }

};

#endif /* STATE_HPP_ */

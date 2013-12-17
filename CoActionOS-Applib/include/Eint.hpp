/*! \file */

#ifndef APPLIB_EINT_HPP_
#define APPLIB_EINT_HPP_

#include <dev/eint.h>
#include "Pblock.hpp"

/*! \brief External Interrupt Class
 * \details This class gives access to external interrupt circuitry.  You can use this class
 * to have the interrupt trigger a function or block a thread until the interrupt arrives.
 *
 * Here is an example:
 *
 * \code
 *
 * #include <applib/Eint.hpp>
 * #include <applib/Timer.hpp>
 *
 *  //This will be executed when the event is triggered
 * int event_callback(void * args, const void * data){
 * 	volatile int * done = (int*)args;
 * 	*done = 1;
 * 	return 0; //return 0 to clear the callback and 1 to keep it
 * }
 *
 * void eint_example(){
 * 	volatile int done;
 * 	Eint eint(0); //use eint port 0 (pin 2.10 on lpc17xx devices)
 *
 * 	eint.init(); //open and set attributes
 *
 * 	eint.setaction(0, //use channel 0
 * 		EINT_ACTION_EVENT_FALLING, //trigger on a falling edge
 * 		event_callback, //execute this function (in privileged mode when complete)
 * 		&done //pass this to event_callback as the first argumen
 * 		);
 *
 * 	while( done == 0 ){
 * 		Timer::wait_msec(1); //wait for the signal to arrive
 * 	}
 *
 * }
 *
 *  //alternatively you can configure (setaction() and block by writing hwpl_action_t to the device)
 *
 *  hwpl_action_t action;
 *  action.channel = 0;
 *  action.event = EINT_ACTION_EVENT_FALLING;
 *  action.callback = 0; //this is ignored and doesn't need to be set
 *  action.context = 0; //this is ignored and doesn't need to be set
 *  eint.write(&action, sizeof(action)); //this will block until a falling edge arrives
 *
 * \endcode
 *
 */
class Eint : public Periph {
public:
	Eint(port_t port);
	static int getports();
	int open(int flags = 0);
	/*! \brief Get the attributes for the external interrupt. */
	int getattr(eint_attr_t * attr);
	/*! \brief Set the attributes for the external interrupt. */
	int setattr(const eint_attr_t * attr);
	/*! \brief Set the action associated with the external interrupt. */
	int setaction(const eint_action_t * action);

	bool value(void){
		eint_attr_t attr;
		getattr(&attr);
		return attr.value != 0;
	}

	inline bool get(void){ return value(); }

	/*! \brief Set the action using the specified parameters */
	int setaction(int channel, int event, hwpl_callback_t callback, void * context){
		eint_action_t action;
		action.channel = channel;
		action.event = event;
		action.callback = callback;
		action.context = context;
		return setaction(&action);
	}

	/*! \brief Set the attributes using the specified pin assignment. */
	int setattr(uint8_t pin_assign = 0){
		eint_attr_t attr;
		attr.pin_assign = pin_assign;
		return setattr(&attr);
	}

	/*! \brief Open and set attributes using specified pin assignment. */
	int init(uint8_t pin_assign = 0){
		if( open() < 0 ){
			return -1;
		}
		return setattr(pin_assign);
	}

#ifdef __HWPL_ONLY__
	int close();
#endif

private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_EINT_PORTS];
	static bool isinitialized;
#endif
};

#endif /* APPLIB_EINT_HPP_ */

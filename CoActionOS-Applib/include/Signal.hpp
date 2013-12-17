/*
 * Signal.hpp
 *
 *  Created on: Nov 5, 2013
 *      Author: tgil
 */

#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#include <pthread.h>
#include <signal.h>
#include <caos.h>
#include <hwpl.h>

/*! \brief Namespace for POSIX Signal events and handlers */
namespace Signal {

/*! \brief Class for handling Signal events */
class Handler {
public:
	/*! \brief Construct a signal handler */
	Handler(void (*handler)(int) ){
		sig_action_.sa_handler = (_sig_func_ptr)handler;
		sig_action_.sa_flags = 0;
		sig_action_.sa_mask = 0;
	}

	/*! \brief Construct a sigaction handler */
	Handler(void (*sigaction)(int, siginfo_t*, void*), int flags = 0, sigset_t mask = 0){
		sig_action_.sa_sigaction = sigaction;
		sig_action_.sa_flags = flags;
		sig_action_.sa_mask = mask;
	}

	const struct sigaction * sigaction() const { return &sig_action_; }

private:
	struct sigaction sig_action_;
};

/*! \brief Class for sending Signal events */
class Event {
public:

	/*! \brief Construct an event based on a signal number */
	Event(int signo){ signo_ = signo; }

	/*! \brief Send a signal to a process */
	/*! \details This method sends a signal to a process
	 *
	 * @param pid  The process ID of the receiving signal
	 * @param signo The signal number (e.g. SIGTERM)
	 * @return Zero on success
	 */
	int trigger(pid_t pid) const { return ::kill(pid, signo_); }

	/*! \brief Send a signal to a thread within a process */
	/*! \details This method sends a signal to a thread within a process.
	 *
	 * @param t The thread ID
	 * @param signo The signal number
	 * @return Zero on success
	 */
	int trigger(pthread_t t) const { return ::pthread_kill(t, signo_); }

	/*! \brief Trigger the event on the current thread */
	int trigger(void) const { return ::pthread_kill(pthread_self(), signo_); }

	/*! \brief Set the event handler */
	int set_handler(Handler * handler) const;

private:
	int signo_;

};

/*! \brief Class for bridging physical events with Signal events */
/*! \details This class allows you to configure physical events
 * to trigger signals.  For example, a rising edge on a pin
 * can cause the system to send a signal to a thread.  Here is an example:
 * \code
 *
 * #include <applib/Signal.hpp>
 * #include <applib/Timer.hpp>
 *
 * volatile bool my_var;
 * void my_handler(int a){
 * 	//this executes on the rising edge of EINT0
 * 	my_var = true;
 * 	//you shouldn't use any non re-entrant functions here (such as printf() and malloc())
 *
 * }
 *
 * int main(int argc, char * argv[]){
 * 	Eint eint(0);
 * 	eint_action_t action;
 *
 * 	Signal::EventPhy phy_event(true, SIGUSR1);  //send SIGUSR1 to this thread on every rising edge
 * 	Signal::Handler handler(my_handler);
 *
 * 	eint.init(0);
 * 	phy_event.set_handler(&handler);
 *
 * 	//this configures the hardware to send the signal to this thread (my_handler() will execute)
 *  //phy_event MUST exist as long as this action is set
 *  action = phy_event.action(EINT_ACTION_EVENT_RISING);
 * 	eint.setaction( &action );
 *
 * 	while(my_var == false){
 * 		Timer::wait_sec(1);
 * 	}
 *
 * 	return 0;
 * }
 *
 *
 * \endcode
 */
class EventPhy: public Event {
public:

	/*! \brief Construct a physical event for the current thread */
	/*! \details This constructs a physical event.
	 *
	 * @param persistant If false, the signal will be sent only on the first hardware event
	 * @param signo The signal number
	 * @param sigcode The signal code
	 * @param sigvalue The signal value
	 */
	EventPhy(bool persistant, int signo, int sigcode = 0, int sigvalue = 0) : Event(signo){
		context.tid = pthread_self();
		context.si_sigcode = sigcode;
		context.si_signo = signo;
		context.keep = persistant;
		context.sig_value = sigvalue;
	}


	/*! \brief Construct a physical event. */
	/*! \details Construct a physical event using a signal_callback_t data structure.
	 *
	 * @param context A copy of the signal_callback_t data to use to handle the event.
	 */
	EventPhy(signal_callback_t context) : Event(context.si_signo){
		this->context = context;
	}

	/*! \brief Return a hwpl_action_t data structure for a hardware event */
	/*! \details This returns a hwpl_action_t data structure that can
	 * be used to set the action associated with a hardware event.
	 *
	 * @param event The hardware event
	 * @param channel The hardware channel
	 * @return a copy of a hwpl_action_t data structure
	 *
	 * \
	 */
	hwpl_action_t action(int event, int channel = 0){
		hwpl_action_t a;
		a.callback = signal_callback;
		a.context = &context;
		a.channel = channel;
		a.event = event;
		return a;
	}

private:
	signal_callback_t context;
};

}

#endif /* SIGNAL_HPP_ */

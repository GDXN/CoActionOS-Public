/*! \file */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <time.h>
#include "Tmr.hpp"

/*! \brief Timer Class
 * \details This class implements a logical timer.  Using CoActionOS it uses
 * the system timer.  If using HWPL only, you must specify which timer using init().
 *
 * Physical timers are controlled using the Tmr class.
 *
 * Here is an example of using the timer to time events.
 * \code
 * #include <applib/Timer.hpp>
 *
 * Timer t;
 * t.start(); //start
 * Timer::wait_usec(500);
 * t.stop();
 * printf("Timer value after 500usec is %d\n", t.usec());
 * \endcode
 *
 * The output of the above code varies depending on the MCU clock cycles and the
 * scheduler.  The following is a sample output.
 *
 * \code
 * Timer value after 500usec is 502
 * \endcode
 *
 *
 */
class Timer {
public:
	Timer();
#ifdef __HWPL_ONLY__
	static int init(Tmr::port_t port);
#endif

	/*! \brief Wait for a number of seconds.
	 *
	 * @param timeout Time to wait in seconds
	 */
	static void wait_sec(uint32_t timeout);

	/*! \brief Wait for a number of milliseconds.
	 *
	 * @param timeout Time to wait in milliseconds
	 */
	static void wait_msec(uint32_t timeout);
	/*! \brief Wait for a number of microseconds.
	 *
	 * @param timeout Time to wait in microseconds
	 */
	static void wait_usec(uint32_t timeout);

	/*! \brief The the usec's on the real-time clock */
	static int clock_usec(void);
	/*! \brief The the msec's on the real-time clock */
	static int clock_msec(void);
	/*! \brief The the sec's on the real-time clock */
	static int clock_sec(void);

	/*! \brief Set the timeout value.
	 * @param timeout In microseconds
	 *
	 * \details This sets the timeout based on the current timer value.  Once
	 * the timeout has been reached, isexpired() returns true.
	 */
	void settimeout(uint32_t timeout);

	/*! \brief Set the timeout value.
	 * @param timeout In microseconds
	 *
	 * \details This sets the timeout based on the current timer value.  Once
	 * the timeout has been reached, isexpired() returns true.
	 */
	void settimeout_usec(uint32_t timeout);

	/*! \brief Set the timeout value.
	 * @param timeout In milliseconds
	 *
	 * \details This sets the timeout based on the current timer value.  Once
	 * the timeout has been reached, isexpired() returns true.
	 */
	void settimeout_msec(uint32_t timeout);

	/*! \brief Set the timeout value.
	 * @param timeout In seconds
	 *
	 * \details This sets the timeout based on the current timer value.  Once
	 * the timeout has been reached, isexpired() returns true.
	 */
	void settimeout_sec(uint32_t timeout);

	/*! \brief Check if timeout has expired
	 * \details This checks to see if the
	 * @return
	 */
	bool isexpired(void); //check to see if timeout has expired

	/*! \brief Start the timer.
	 * \details This starts the timer.  It is used with value() and stop().
	 *
	 */
	void start(void); //start counting

	/*! \brief Check if timer is running */
	bool isrunning();

	/*! \brief Reset the value of the timer */
	void reset(void);

	/*! \brief Read the current timer value.
	 *
	 * @return The number of microseconds that have elapsed since start()
	 */
	uint32_t value(void); //current value of timer

	/*! \brief Timer value in milliseconds
	 * @return The number of milliseconds that have elapsed since start.
	 */
	uint32_t msec(void);

	/*! \brief Timer value in microseconds
	 * @return The number of microseconds that have elapsed since start.
	 */
	uint32_t usec(void);

	/*! \brief Timer value in seconds
	 * @return The number of seconds that have elapsed since start.
	 */
	uint32_t sec(void);

	/*! \brief Stop the current timer.
	 * \details This stops the timer from counting.  Subsequent calls to value()
	 * will return the same number.
	 */
	void stop(void); //stop counting

private:
#ifdef __HWPL_ONLY__
	static Tmr::port_t port;
	uint32_t start_;
	uint32_t stop_;
	uint32_t timeout_stop_;
#else
	struct timespec start_;
	struct timespec stop_;
	struct timespec timeout_stop_;
#endif


};

#endif /* TIMER_HPP_ */

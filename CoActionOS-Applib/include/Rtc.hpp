/*! \file */

#ifndef APPLIB_RTC_HPP_
#define APPLIB_RTC_HPP_

#include <dev/rtc.h>
#include "Pchar.hpp"

/*! \brief RTC Class
 * \details This class implements an RTC.
 */
class Rtc : public Periph {
public:
	Rtc(port_t port);
	static int getports(){ return HWPL_RTC_PORTS; }
	int open(int flags = 0);
	/*! \brief Get RTC attributes */
	int getattr(rtc_attr_t * attr);
	/*! \brief Set RTC attributes */
	int setattr(rtc_attr_t * attr);
	/*! \brief Set RTC action */
	int setaction(rtc_action_t * action);
	/*! \brief Set RTC alarm */
	int setalarm(rtc_alarm_t * alarm);
	/*! \brief Get RTC alarm */
	int getalarm(rtc_alarm_t * alarm);
	/*! \brief Disable RTC alarm */
	int disablealarm(void);
	/*! \brief Set RTC time */
	int set(rtc_time_t * time);
	/*! \brief Get RTC time */
	int get(rtc_time_t * time);

#ifdef __HWPL_ONLY__
	int close();
#endif


private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_RTC_PORTS];
	static bool isinitialized;
#endif
};

#endif /* APPLIB_RTC_HPP_ */

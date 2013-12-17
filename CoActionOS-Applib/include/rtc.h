/*! \addtogroup RTC
 *
 * @{
 *
 * \brief Real Time Clock
 */


#ifndef APPLIB_RTC_H_
#define APPLIB_RTC_H_

#include <dev/rtc.h>

#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details This function opens the specified RTC port (e.g. /dev/rtc0).
 */
int rtc_open(port_t port);

/*! \details This functions closes the specified RTC port.
 *
 */
int rtc_close(port_t port);

/*! \details This function gets the RTC attributes from the specified RTC port.
 *
 */
int rtc_getattr(port_t port, rtc_attr_t * attr);

/*! \details This function sets the RTC attributes of the specified RTC port.
 *
 */
int rtc_setattr(port_t port, rtc_attr_t * attr);

/*! \details This function sets an action associated with the RTC.
 *
 */
int rtc_setaction(port_t port, rtc_action_t * action);

/*! \details This function sets the RTC alarm.
 *
 */
int rtc_setalarm(port_t port, rtc_alarm_t * alarm);

/*! \details This function gets the RTC alarm.
 *
 */
int rtc_getalarm(port_t port, rtc_alarm_t * alarm);

/*! \details This function disables the RTC alarm.
 *
 */
int rtc_disablealarm(port_t port);

/*! \details This function sets the RTC time.
 *
 */
int rtc_set(port_t port, rtc_time_t * time);

/*! \details This function gets the RTC time.
 *
 */
int rtc_get(port_t port, rtc_time_t * time);

#ifdef __cplusplus
}
#endif


#endif /* APPLIB_RTC_H_ */

/*! @} */

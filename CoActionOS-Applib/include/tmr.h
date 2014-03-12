/*! \addtogroup TMR Timer (TMR)
 * @{
 * \brief Timer
 *
 */
#ifndef APPLIB_TMR_H_
#define APPLIB_TMR_H_

#include <dev/tmr.h>
#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Opens the timer */
int tmr_open(port_t port);

/*! \brief Closes the timer */
int tmr_close(port_t port);

/*! \brief Gets the timer attributes */
int tmr_getattr(port_t port, tmr_attr_t * attr);

/*! \brief Sets the timer action */
int tmr_setattr(port_t port, const tmr_attr_t * attr);

/*! \brief Sets the timer action */
int tmr_setaction(port_t port, const tmr_action_t * action);

/*! \brief Turns the timer on (starts counting) */
int tmr_on(port_t port);

/*! \brief Turns the timer off (stops counting) */
int tmr_off(port_t port);

/*! \brief Sets the output compare characteristics */
int tmr_setoc(port_t port, tmr_reqattr_t * attr);

/*! \brief Sets the input capture characteristics */
int tmr_setic(port_t port, tmr_reqattr_t * attr);

/*! \brief Gets the output compare attributes */
int tmr_getoc(port_t port, tmr_reqattr_t * attr);

/*! \brief Gets the input capture attributes */
int tmr_getic(port_t port, tmr_reqattr_t * attr);

/*! \brief Gets the value of the timer */
int tmr_get(port_t port);

/*! \brief Sets the value of the timer */
int tmr_set(port_t port, int value);

#ifdef __cplusplus
}
#endif

#endif /* APPLIB_TMR_H_ */

/*! @} */

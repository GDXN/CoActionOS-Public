/*! \addtogroup PWM
 * @{
 *
 * \brief Pulse Width Modulation
 *
 */


#ifndef APPLIB_PWM_H_
#define APPLIB_PWM_H_

#include <dev/pwm.h>
#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Open \a port */
int pwm_open(port_t port);
/*! \brief Close \a port */
int pwm_close(port_t port);
/*! \brief Get the port attributes */
int pwm_getattr(port_t port, pwm_attr_t * attr);
/*! \brief Set the port attributes */
int pwm_setattr(port_t port, const pwm_attr_t * attr);
/*! \brief Set the port attributes */
int pwm_setaction(port_t port, const pwm_action_t * action);
/*! \brief Set the value of \a port */
int pwm_set(port_t port, const pwm_reqattr_t * req);
/*! \brief Write data to the port (updated once per PWM period) */
int pwm_write(port_t port, int loc, const void * buf, int nbyte);

#ifdef __cplusplus
}
#endif

#endif /* APPLIB_PWM_H_ */

/*! @} */

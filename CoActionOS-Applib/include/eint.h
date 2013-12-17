/*! \addtogroup EINT
 * @{
 *
 * \brief External Interrupts
 *
 */

/*! \file */

#ifndef APPLIB_EINT_H_
#define APPLIB_EINT_H_

#include <stdint.h>
#include <dev/eint.h>

#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Open the external interrupt. */
int eint_open(port_t port);
/*! \brief Close the external interrupt. */
int eint_close(port_t port);
/*! \brief Get the attributes for the external interrupt. */
int eint_getattr(port_t port, eint_attr_t * attr);
/*! \brief Set the attributes for the external interrupt. */
int eint_setattr(port_t port, const eint_attr_t * attr);
/*! \brief Set the action for the external interrupt. */
int eint_setaction(port_t port, const eint_action_t * action);


#ifdef __cplusplus
}
#endif

#endif /* APPLIB_EINT_H_ */

/*! @} */

/*! \addtogroup QEI
 * @{
 *
 * \brief Quadrature Encoder Interface
 *
 */

/*! \file */

#ifndef APPLIB_QEI_H_
#define APPLIB_QEI_H_

#include <stdint.h>
#include <dev/qei.h>

#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Open the external interrupt. */
int qei_open(port_t port);
/*! \brief Close the external interrupt. */
int qei_close(port_t port);
/*! \brief Get the attributes for the external interrupt. */
int qei_getattr(port_t port, qei_attr_t * attr);
/*! \brief Set the attributes for the external interrupt. */
int qei_setattr(port_t port, const qei_attr_t * attr);
/*! \brief Set the action for the external interrupt. */
int qei_setaction(port_t port, const qei_action_t * action);
/*! \brief Get the current position of the encoder. */
int qei_get(port_t port);
/*! \brief Get the current velocity of the encoder. */
int qei_getvelocity(port_t port);
/*! \brief Get the current index count. */
int qei_getindex(port_t port);
/*! \brief Reset the encoder. */
int qei_reset(port_t port, int flags);

#ifdef __cplusplus
}
#endif

#endif /* APPLIB_QEI_H_ */

/*! @} */

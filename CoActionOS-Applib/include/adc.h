
/*! \addtogroup ADC
 * @{
 *
 * \brief Analog to Digital Converter
 *
 */

/*! \file */

#ifndef APPLIB_ADC_H_
#define APPLIB_ADC_H_

#include <dev/adc.h>
#include "periph.h"
#ifdef __cplusplus
extern "C" {
#endif

/*! \details This function opens the specified ADC port (e.g. /dev/adc0).
 */
int adc_open(port_t port);

/*! \brief This functions closes the specified ADC port.
 */
int adc_close(port_t port);

/*! \brief This function gets the ADC attributes from the specified ADC port
 */
int adc_getattr(port_t port, adc_attr_t * attr);

/*! \brief This function sets the ADC attributes for \a port.
 */
int adc_setattr(port_t port, const adc_attr_t * attr);


/*! \brief Reads consecutive samples from the ADC port.
 */
int adc_read(port_t port, int chan, void * buf, int nbyte);

#ifdef __cplusplus
}
#endif


#endif /* APPLIB_ADC_H_ */

/*! @} */

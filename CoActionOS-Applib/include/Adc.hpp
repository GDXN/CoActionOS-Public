/*! \file */

#ifndef APPLIB_ADC_HPP_
#define APPLIB_ADC_HPP_

#include <dev/adc.h>
#include "Pblock.hpp"

/*! \brief ADC Peripheral Class
 * \details This class implements ADC device peripherals.
 *
 * \code
 * Adc adc(0);  //create an instance of ADC to access port 0
 * adc_sample_t samples[16];
 * adc.init((1<<0)|(1<<1)); //open() and setattr() enabling channels 0 and 1
 * adc.read(0, samples, 16*sizeof(adc_sample_t)); //read 16 samples from channel 0
 * adc.read(1, samples, 16*sizeof(adc_sample_t)); //read 16 samples from channel 1
 * adc.close(); //close the ADC
 * \endcode
 *
 */
class Adc : public Pblock {
public:
	/*! \brief Initialize the instance with \a port */
	Adc(port_t port);
	static int getports();
	int open(int flags = 0);

	/*! \brief Get the ADC attributes */
	int getattr(adc_attr_t * attr);
	/*! \brief Set the ADC attributes */
	int setattr(const adc_attr_t * attr);
	/*! \brief Set the ADC attributes */
	int setattr(uint16_t enabled_channels /*! Enabled Channels */,
			int freq = ADC_MAX_FREQ /*! ADC clock frequency (use ADC_MAX_FREQ for maximum speed) */,
			uint8_t pin_assign = 0 /*! Pin assignment value */){
		adc_attr_t attr;
		attr.enabled_channels = enabled_channels;
		attr.freq = freq;
		attr.pin_assign = pin_assign;
		return setattr(&attr);
	}

	/*! \brief Open ADC and set the attributes as specified.
	 * \details This method opens the ADC then sets the ADC
	 * attributes as specified.
	 */
	int init(uint16_t enabled_channels /*! Enabled Channels */,
			int freq = ADC_MAX_FREQ /*! ADC clock frequency (use ADC_MAX_FREQ for maximum speed)  */,
			uint8_t pin_assign = 0 /*! Pin assignment value */){
		if( open() < 0 ){
			return -1;
		}
		return setattr(enabled_channels, freq, pin_assign);
	}


#ifdef __HWPL_ONLY__
	using Pblock::read;
	int read(void * buf, int nbyte);
	int close();
#endif


private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_ADC_PORTS];
	static bool isinitialized;
#endif
};

#endif /* APPLIB_ADC_HPP_ */

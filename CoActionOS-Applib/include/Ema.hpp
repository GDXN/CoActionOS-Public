/*! \file */

#ifndef EMA_HPP_
#define EMA_HPP_

#include <stdint.h>
#include <cstdio>

/*! \brief Digital Signal Processing namespace */
namespace Dsp {

/*! \brief Macro for creating the alpha value for 32-bit calculations */
/*! \details This value can be set between 0.0 and 1.0.  More weight is
 * given to older samples when the valeu is close to 0.0.  When the value is set
 * to 1.0, no averaging occurs.
 */
#define DSP_EMA_I32_ALPHA(x) ( (uint16_t)(x * 65535) )

/*! \brief Macro for creating the alpha avelu for 16-bit calculations */
/*! \details See \ref DSP_EMA_I32_ALPHA for details */
#define DSP_EMA_I16_ALPHA(x) ( (uint8_t)(x * 255) )

/*! \brief Exponential Moving Average template class */
/*! \details This class is a template for an exponential moving average (EMA)
 * calculation.  The EMA is a type of low pass filter and is helpful when trying
 * to smooth out data that is sampled on the ADC.
 *
 * The following is an example of using the Ema class using 32-bit integers.
 * \code
 * #include <applib/Adc.hpp>
 * #include <applib/Ema.hpp>
 * #include <cstdio>
 *
 *  //first initialize a filter with a value of 0 and with an averaging constant of 0.1
 * Ema_i32 filter(0, DSP_EMA_I32_ALPHA(0.1));
 *  //Or do:  Ema<uint16_t, int32_t, int64_t> filter(0, DSP_EMA_I32_ALPHA(0.1));
 *
 *  //now use the ADC to get some data
 * Adc adc(0);
 * adc_sample_t sample;
 * adc.init(1<<0); //initialize with channel 0 enabled
 *
 * for(i=0; i < 100; i++){  //take 100 samples and filter as we go
 * 	adc.read(0, &sample, sizeof(sample));
 * 	filter.calc(sample);
 * 	printf("%d %d %d;\n", i, sample, filter.avg());
 * }
 * \endcode
 *
 * The filter is easy-to-use and uses minimal resources.  For more information on
 * EMA filtering see <a href="/projects/coactionos/wiki/An_Easy-to-Use_Digital_Filter">this wiki article</a>.
 *
 */
template<typename intsmall, typename intmedium, typename intlarge>class Ema {
public:
	Ema(intmedium start, intsmall alpha){ average = start;  this->alpha = alpha; }
	intmedium small_max(){ return 1<<(8*sizeof(intsmall)); }
	intmedium calc(intmedium in){
		intlarge tmp0;
		tmp0 = (intlarge)in * (alpha) + (intlarge)average * (small_max() - alpha);
		average = (intmedium)(((intlarge)tmp0 + (intlarge)small_max()) >> (sizeof(intsmall)*8));
		return average;
	}
	intmedium avg(){ return average; }
private:
	intmedium average;
	intsmall alpha;
};

/*! \brief Exponential Moving Average class (int32_t) */
/*! \details See \ref Ema for details */
class Ema_i32 : public Ema<uint16_t, int32_t, int64_t> {
public:
	Ema_i32(int32_t start, uint16_t alpha) : Ema(start, alpha){}
};

/*! \brief Exponential Moving Average class (int16_t) */
/*! \details See \ref Ema for details */
class Ema_i16 : public Ema<uint8_t, int16_t, int32_t> {
public:
	Ema_i16(int16_t start, uint8_t alpha) : Ema(start, alpha){}
};

/*! \brief Exponential Moving Average class (uint32_t) */
/*! \details See \ref Ema for details */
class Ema_u32 : public Ema<uint16_t, uint32_t, uint64_t> {
public:
	Ema_u32(uint32_t start, uint16_t alpha) : Ema(start, alpha){}
};

/*! \brief Exponential Moving Average class (uint16_t) */
/*! \details See \ref Ema for details */
class Ema_u16 : public Ema<uint8_t, uint16_t, uint32_t> {
public:
	Ema_u16(uint16_t start, uint8_t alpha) : Ema(start, alpha){}
};

/*! \brief Exponential Moving Average class (uint16_t) */
/*! \details See \ref Ema for details */
class Ema_float {
public:
	Ema_float(float start, float alpha){ this->alpha = alpha; this->average = average; }
	float small_max(){ return 1.0; }
	float calc(float in){
		float tmp;
		average = in * (alpha) + average * (1.0 - alpha);
		return average;
	}
	float avg(){ return average; }
private:
	float average;
	float alpha;
};


}

/* namespace Dsp */
#endif /* EMA_HPP_ */

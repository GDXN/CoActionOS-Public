/*! \file */

#ifndef EXPLCDDEVICE_HPP_
#define EXPLCDDEVICE_HPP_

#include "coactionhero.h"
#include "Nhdc12832.hpp"

/*! \brief CoAction Hero LCD Expansion board class */
/*! \details This is the implementation of the Nhdc12832 when using
 * the CoAction Hero's LCD device board.
 *
 * Here is an example of drawing on the LCD:
 *
 * \code
 * ExpLcdDevice lcd;
 *
 * lcd.clear();
 * lcd.setline(0,0, lcd.xmax(), lcd.ymax());
 * lcd.refresh();
 *
 * \endcode
 *
 */
class ExpLcdDevice : public Nhdc12832 {
public:
	ExpLcdDevice() : Nhdc12832(COACTION_EXP_SPI_PORT, 8000000, COACTION_EXP_SPI_PINASSIGN,
			COACTION_EXP_PIO0_PORT, COACTION_EXP_PIO0_PINMASK,
			COACTION_EXP_PIO2_PORT, COACTION_EXP_PIO2_PINMASK,
			COACTION_EXP_PIO3_PORT, COACTION_EXP_PIO3_PINMASK,
			COACTION_EXP_PWM0_PORT,
			COACTION_EXP_PWM0_CHANNEL,
			COACTION_EXP_PWM0_PINASSIGN){}
};

#endif /* EXPLCDDEVICE_HPP_ */

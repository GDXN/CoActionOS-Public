/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup HWPL
 * @{
 */

/*! \addtogroup PERIPHIO Peripheral Input/Output
 * @{
 *
 */

/*! @} */

/*! \addtogroup ARCHITECTURES Architectures
 * @{
 *
 */

/*! @} */

/*! @} */


/*! \file
 * \brief HWPL Header file (includes all other header files)
 */

/*! \addtogroup HWPL Hardware Peripheral Library
 *
 * @{
 *
 *
 */


#ifndef HWPL_H_
#define HWPL_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


/*! @} */

/*! \addtogroup PERIPHIO
 * @{
 */


/*! @} */

/*! \addtogroup HWPL
 * @{
 */

#include "hwpl/fault.h"
#include "hwpl/arch.h"
#include "hwpl/types.h"

#ifdef __cplusplus
extern "C" {
#endif


//These values are defined in the linker script
extern uint32_t _top_of_stack;
extern uint32_t _text;
extern uint32_t _etext;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;
extern uint32_t _sys;
extern uint32_t _esys;

#ifdef __cplusplus
}
#endif



#endif /* HWPL_H_ */

/*! @} */



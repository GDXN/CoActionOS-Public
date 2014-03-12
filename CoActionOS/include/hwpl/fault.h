/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup FAULT Fault Handling
 * @{
 *
 * \ingroup CORE
 *
 * \details The fault handling module handles processor faults.  When a fault occurs, the fault
 * handler decodes the fault and calls hwpl_fault_event_handler() which must be provided
 * externally by the application.
 *
 */


/*! \file
 * \brief Fault Handling Header file (Defines Fault Codes)
 */

#ifndef FAULT_H_
#define FAULT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*! \details This lists the possible values of a HWPL fault.
 *
 */
typedef enum {
	HWPL_FAULT_NONE,
	HWPL_FAULT_MEMORY_STACKING /*! 1 A fault occurred while stacking */,
	HWPL_FAULT_MEMORY_UNSTACKING /*! 2 A fault occured while unstacking */,
	HWPL_FAULT_MEMORY_ACCESS_VIOLATION /*! 3 A read or write was attempted on a location where this is not allowed */,
	HWPL_FAULT_MEMORY_EXEC_VIOLATION /*! 4 An instruction fetch was attempted on memory that is marked as non-executable */,
	HWPL_FAULT_BUS_STACKING /*! 5 A bus fault occurred while stacking */,
	HWPL_FAULT_BUS_UNSTACKING /*! 6 A bus fault occurred whild unstacking */,
	HWPL_FAULT_BUS_IMPRECISE /*! 7 An imprecise bus fault occurred */,
	HWPL_FAULT_BUS_PRECISE /*! 8 A precision bus fault occurred */,
	HWPL_FAULT_BUS_INSTRUCTION /*! 9 A bus fault occurred when fetching an instruction */,
	HWPL_FAULT_USAGE_DIVBYZERO /*! 10 A divide by zero was attempted */,
	HWPL_FAULT_USAGE_UNALIGNED /*! 11 An unaligned memory access */,
	HWPL_FAULT_USAGE_NO_COPROCESSOR /*! 12 An instruction attempted to access a coprocessor that is not supported */,
	HWPL_FAULT_USAGE_INVALID_PC /*! 13 In invalid PC was loaded on return */,
	HWPL_FAULT_USAGE_INVALID_STATE /*! 14 An instruction has made invalid use of the EPSR register */,
	HWPL_FAULT_USAGE_UNDEFINED_INSTRUCTION /*! 15 The processor has attempted to execute an undefined instruction */,
	HWPL_FAULT_HARD_VECTOR_TABLE /*! 16 This is a bus fault on a vector table read */,
	HWPL_FAULT_UNHANDLED_INTERRUPT /*! 17 This is executed if a interrupt request routine is not defined */,
	HWPL_FAULT_HARD_UNKNOWN /*! 18 This is an unknown fault */,
	HWPL_FAULT_BUS_UNKNOWN /*! 19 An unknown bus fault */,
	HWPL_FAULT_MEM_UNKNOWN /*! 20 An unknown memory fault */,
	HWPL_FAULT_USAGE_UNKNOWN /*! 21 An unknown usage fault */,
	HWPL_FAULT_TOTAL
} fault_num_t;


typedef struct {
	int num /*! The Fault number (see \ref fault_num_t) */;
	void * addr /*! The faulting address if applicable */;
	void * pc /*! The thread program counter */;
	void * caller /*! The thread caller */;
	void * handler_pc /*! The handler program counter */;
	void * handler_caller /*! The handler caller */;
} fault_t;

int hwpl_fault_init(void);

/*! \details This function must be provided by the application or
 * OS to handle faults.
 */
extern void hwpl_fault_event_handler(fault_t * fault);

int hwpl_fault_save(fault_t * fault);
int hwpl_fault_load(fault_t * fault);

#ifdef __cplusplus
}
#endif

#endif /* FAULT_H_ */


/*! @} */

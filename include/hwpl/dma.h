/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup DMA Direct Memory Access (DMA)
 * @{
 *
 * \ingroup CORE
 *
 * \details The Direct memory access allows for direct memory transfers from memory to memory or
 * memory to/from peripherals.  Various peripheral APIs use the DMA to asynchronously read/write
 * peripheral data.  The DMA should not be used directly by the application because it
 * may conflict with peripheral IO's use of the DMA.
 *
 *
 *
 */

/*! \file
 * \brief Direct Memory Access Header File
 *
 */

#ifndef DMA_H_
#define DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef dma_no_sleep_ram
/* \details This attribute can be applied to RAM so
 * that it is stored in RAM that is not shut off
 * when the processor sleeps.  This allows DMA operations
 * to continue during sleep.
 *
 * Example:
 * \code
 * dma_no_sleep_ram static char buffer[512];
 * \endcode
 */
#define dma_no_sleep_ram
#endif

typedef struct dma_lli_t{
	const void * src;
	void * dest;
	struct dma_lli_t * next;
	uint32_t ctrl;
} dma_lli_t;


/*! \details This lists the valid DMA operations.
 */
enum {
	DMA_MEM_TO_MEM /*! A memory to memory DMA operation */,
	DMA_MEM_TO_PERIPH /*! A memory to peripheral DMA operation */,
	DMA_PERIPH_TO_MEM /*! A peripheral to memory DMA operation */,
	DMA_PERIPH_TO_PERIPH /*! A peripheral to peripheral DMA operation */
};


/*! \details This function opens the DMA peripheral.
 */
void _hwpl_dma_init(int mode /*! Flags which specify how the DMA is configured */);



/*! \details This function halts the specified DMA channel.
 */
int _hwpl_dma_halttransfer(int chan /*! The channel to halt */);

/*! \details This function closes and disables the DMA peripheral.
 */
void _hwpl_dma_exit(void);

int _hwpl_dma_transferlist(int operation,
		int chan,
		dma_lli_t * linked_list,
		hwpl_callback_t callback,
		void * context,
		uint32_t dest_periph,
		uint32_t src_periph
		);

int _hwpl_dma_transfer(int operation,
		int chan,
		void * dest,
		const void * src,
		int ctrl,
		hwpl_callback_t cb,
		void * context,
		uint32_t dest_periph,
		uint32_t src_periph);


#ifdef __cplusplus
}
#endif

#endif /* DMA_H_ */

/*! @} */

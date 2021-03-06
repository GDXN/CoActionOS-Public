/*
 * Rle.hpp
 *
 *  Created on: Nov 15, 2013
 *      Author: tgil
 */

#ifndef RLE_HPP_
#define RLE_HPP_

#include <stdint.h>
#include "File.hpp"
#include "Appfs.hpp"

/*! \brief Run Length Encoding Class */
/*! \details This class implements Run length encoding and
 * decoding algorithms.
 *
 */
class Rle {
public:
	Rle();

	/*! \brief Encode a chunk of data */
	/*! \details This method encodes a block of data.
	 *
	 * @param dest A pointer to the destination data
	 * @param dest_size Pass the max size of dest, this will hold the number of encoded bytes upon return
	 * @param src A pointer to the source data
	 * @param src_size The number of bytes to encode
	 * @return Number of un-encoded bytes that were processed
	 */
	static int encode(void * dest, int & dest_size, const void * src, int src_size);

	/*! \brief Decode a chunk of data */
	/*! \details This method decodes a block of data.
	 *
	 * @param dest A pointer to the destination data
	 * @param dest_size Pass the max size of dest, this will hold the number of decoded bytes upon return
	 * @param src A pointer to the encoded data
	 * @param src_size The number of encoded bytes to process
	 * @return Number of encoded bytes that were processed
	 */
	static int decode(void * dest, int & dest_size, const void * src, int src_size);

	/*! \brief Calculate the size of the encoded data */
	/*! \details This methods calculate the number of bytes that will be used by the
	 * compressed data.
	 *
	 * @param src A pointer to the source memory
	 * @param size The number of bytes to process
	 * @return The number of bytes the data would occupy after compression
	 */
	static int size(const void * src, int nbyte);

private:
	typedef struct HWPL_PACK {
		uint8_t size;
		uint8_t data;
	} element_t;

	typedef struct HWPL_PACK {
		uint32_t size;
		uint32_t data;
	} element32_t;

};

/*! \brief Class for reading/writing compressed file data */
class RleFile : public Rle, public File {
public:

	/*! \brief Encode and write data to a file */
	/*! \details This method encodes then writes the data to a file.
	 *
	 * @param buf The source data
	 * @param nbyte The number of bytes to encode and write
	 * @return The number of un-encoded bytes that were written
	 */
	int write(const void * buf, int nbyte);


	/*! brief Read and decode data from a file */
	/*! \details This method reads and decodes data from a file.
	 *
	 * @param buf A pointer to the destination memory
	 * @param nbyte The maximum number of bytes to read
	 * @return The number of bytes read after decoding
	 */
	int read(void * buf, int nbyte);

private:
	char buf[64];
};

/*! \brief Class for reading/writing compressed file data using Appfs */
class RleAppfs : public Rle, public Appfs {
public:
	RleAppfs();

	/*! \brief Encode and write data to a file */
	/*! \details This method encodes then writes the data to a file.
	 *
	 * @param buf The source data
	 * @param nbyte The number of bytes to encode and write
	 * @return The number of un-encoded bytes that were written
	 */
	int write(const void * buf, int nbyte);


	/*! brief Read and decode data from a file */
	/*! \details This method reads and decodes data from a file.
	 *
	 * @param buf A pointer to the destination memory
	 * @param nbyte The number of bytes to read
	 * @return The number of bytes read after decoding
	 */
	int read(void * buf, int nbyte);

};

#endif /* RLE_HPP_ */

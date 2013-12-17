
#ifndef BASE64_HPP_
#define BASE64_HPP_

/*! \file */

/*! \brief Base64 Encode/Decode Class
 * \details This class includes methods to encode and decode data
 * using the base64 algorithm.  Base64 is useful for representing binary
 * data using only valid text characters.  This can be useful when
 * transmitting data over certain serial links that do not support binary
 * transfers.
 */
class Base64 {
public:
	Base64();

	/*! \brief See below for details. */
	/*! \details This method encodes data to the base64 format. */
	static int encode(char * dest /*! Pointer to destination memory */,
			const void * src /*! Pointer to source data (binary) */,
			int nbyte /*! Number of bytes to encode */);
	/*! \brief Calculate encoded size of \a nbyte */
	static int esize(int nbyte /*! Number of bytes to encode */);
	/*! \brief See below for details. */
	/*! \details This methods decodes base64 encoded data.
	 *
	 * @param dest Pointer to destination memory (binary format)
	 * @param src Pointer to source data (base64 encoded)
	 * @param nbyte Pointer the number of bytes to decode (src size)
	 * @return Zero
	 */
	static int decode(void * dest, const char * src, int nbyte);
	/*! \brief Calculate max decoded size of \a nbyte */
	static int dsize(int nbyte);

};

#endif /* BASE64_HPP_ */

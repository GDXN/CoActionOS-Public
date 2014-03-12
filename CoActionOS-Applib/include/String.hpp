#ifndef STRING_HPP_
#define STRING_HPP_

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "StringUtil.hpp"

/*! \file */


/*! \brief String class
 * \details This is an embedded friendly string class.  It is
 * based on fixed capacity c-strings and uses many of the cstring
 * functions for implementation.  To keep things familiar, the API
 * is similar to std::string.
 *
 * It also uses lightweight number formatting code in cases where
 * sprintf() is not available or is too large.
 *
 * \code
 * #include <applib/StringTypes.hpp>
 *
 * StringMedium s1; //allocate 64 bytes for string characters
 * StringSmall s2;
 * s1 = "This is my string";
 * s1 << " " << s1.capacity() << "\n";
 * printf("%s", s1.c_str());
 *  //Strings can be compared
 * s2 = "This is another string";
 * if( s1 == s2 ){
 * 	printf("The strings are the same!\n");
 * } else {
 * 	printf("The strings are different\n");
 * }
 * \endcode
 *
 * The above code outputs:
 * \code
 * This is my string 64
 * The strings are different
 * \endcode
 *
 */
class String {
public:
	/*! \brief Declare an empty string */
	String();
	/*! \brief Declare a string and initialize to \a s */
	String(const char * s);
	/*! \brief Declare a string and initialize to \a s */
	String(const String & s);
	/*! \brief Set number format (DECIMAL, BINARY, etc) */
	void setformat(enum StringUtil::mode m, int width = 0);
	operator const char * ();

	/*! \brief Append a string */
	String& operator<<(const String & a);
	/*! \brief Append a character */
	String& operator<<(char a);
	/*! \brief Append a c-string */
	String& operator<<(const char * a);
	/*! \brief Append an integer */
	String& operator<<(int a);
	/*! \brief Append an unsigned integer */
	String& operator<<(unsigned int a);
	/*! \brief Assign a c-string */
	String& operator=(const char * a);
	/*! \brief Assign a String */
	String& operator=(const String & a);
	/*! \brief Compare to another String */
	bool operator==(const String & cmp) const;
	/*! \brief Compare to a c-string */
	bool operator==(const char * cmp) const;
	/*! \brief Compare (not equal) to another String */
	bool operator!=(const String & cmp) const;
	/*! \brief Compare to a c-string */
	bool operator!=(const char * cmp) const;
	/*! \brief Convert to an integer */
	int atoi() const;

#ifndef __link
	String& operator<<(uint32_t a) { return this->operator<<((unsigned int)a); }
	String& operator<<(int32_t a) { return this->operator<<((int)a); }
#endif
	String& operator<<(uint16_t a) { return this->operator<<((unsigned int)a); }
	String& operator<<(int16_t a) { return this->operator<<((int)a); }

	/*! \brief Insert \a s (zero terminated) into string at \a pos */
	String& insert(unsigned int pos, const char * s);
	/*! \brief Insert \a str into string at \a pos */
	String& insert(unsigned int pos, const String & str);

	/*! \brief Erase a portion of the string starting with the character at \a pos */
	String& erase(unsigned int pos, unsigned int len = -1);

	/*! \brief Return character at \a pos */
	char at(unsigned int pos) const;

	/*! \brief Return the length of the string */
	unsigned int size() const;
	/*! \brief Return the length of the string */
	inline unsigned int length() const { return size(); }

	/*! \brief Test if string is empty */
	bool empty() const;
	/*! \brief Assign \a a to string */
	String& assign(const String& a);
	/*! \brief Assign a substring of \a a to string */
	String& assign(const String& a, unsigned int subpos, unsigned int sublen);
	/*! \brief Assign a maximum of \a n characters of \a a to string */
	String& assign(const char * a, unsigned int n);
	/*! \brief Assign \a a (zero terminated) to string  */
	String& assign(const char * a);
	/*! \brief Append \a a (zero terminated) to string  */
	void append(const char * a);
	/*! \brief Append \a c to string  */
	void append(char c);
	/*! \brief Append \a c to string  */
	inline void push_back(char c) { append(c); }
	/*! \brief Clear string */
	void clear(void);

	/*! \brief Copy the \a nth element (separated by \a sep) of the string to to \a dest */
	bool csv(String & dest, int n, char sep = ',', char term = '\n');
	/*! \brief Return the number of elements in the String */
	int csv_size(char sep = ',', char term = '\n');

	/*! \brief Copy a portion of the string to \a s */
	void copy(char * s, unsigned int n, unsigned int pos = 0) const;
	inline void copy(String & s, unsigned int n, unsigned int pos = 0) const {
		return copy(s.data(), n, pos);
	}

	/*! \brief Return the c string (zero terminated) */
	virtual const char * c_str() const = 0;
	/*! \brief Return the total number of characters the string is capable of holding. */
	virtual unsigned int capacity() const = 0;
	/*! \brief Return a pointer to the string data */
	virtual char * data() = 0;

private:
	StringUtil::mode m;
	int width;
};

/*! \brief Static String Template Class
 * \details This template is used for declaring fixed length strings.
 */
template <unsigned int arraysize>
class StringStatic : public String {
public:
	StringStatic() : String(){ clear(); }
	StringStatic(const char * s) : String(){ clear(); assign(s); }
	StringStatic(const String & s) : String(){ clear(); assign(s.c_str()); }

	const char * c_str() const { return s; }
	char * data() { return s; }
	unsigned int capacity() const{ return arraysize; }

private:
	char s[arraysize+1];
};

class StringUser : public String {
public:
	StringUser(char * data, unsigned int size){ s = data; capacity_ = size; clear(); }

	const char * c_str() const { return s; }
	char * data() { return s; }
	unsigned int capacity() const{ return capacity_; }

private:
	char * s;
	unsigned int capacity_;
};

/*! \brief Tiny String */
class StringTiny : public StringStatic<16> {
public:
	StringTiny() : StringStatic(){}
	StringTiny(const char * s) : StringStatic(s){}
	StringTiny(const String & s) : StringStatic(s){}
};

/*! \brief Small String */
class StringSmall : public StringStatic<32> {
public:
	StringSmall() : StringStatic(){}
	StringSmall(const char * s) : StringStatic(s){}
	StringSmall(const String & s) : StringStatic(s){}
};

/*! \brief Medium String */
class StringMedium : public StringStatic<64> {
public:
	StringMedium() : StringStatic(){}
	StringMedium(const char * s) : StringStatic(s){}
	StringMedium(const String & s) : StringStatic(s){}
};

/*! \brief Large String */
class StringLarge : public StringStatic<128> {
public:
	StringLarge() : StringStatic(){}
	StringLarge(const char * s) : StringStatic(s){}
	StringLarge(const String & s) : StringStatic(s){}
};

#endif /* STRING_HPP_ */

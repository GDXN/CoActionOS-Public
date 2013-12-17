/*
 * Token.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: tgil
 */

#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include "StringTypes.hpp"

class Token : public String {
public:
	Token(const char * src, const char * delim) : String(src){ num_tokens = 0; string_size = 0; }
	Token(const String & src, const char * delim) : String(src){ num_tokens = 0; string_size = 0; }

	/*! \brief Return the total number of tokens */
	unsigned int size() const { return num_tokens; }

	/*! \brief Return a pointer to the token specified by offset */
	const char * at(unsigned int n) const;

	virtual const char * c_str() const = 0;
	virtual unsigned int capacity() const = 0;
	virtual char * data() = 0;

protected:
	void construct(const char * delim);

private:
	unsigned int num_tokens;
	unsigned int string_size;


};

template <unsigned int arraysize>
class TokenStatic : public Token {
public:
	TokenStatic(const char * s, const char * delim) : Token(s,delim){ clear(); assign(s); construct(delim); }
	TokenStatic(const String & s, const char * delim) : Token(s,delim){ clear(); assign(s); construct(delim); }

	const char * c_str() const { return s; }
	char * data() { return s; }
	unsigned int capacity() const{ return arraysize; }

private:
	char s[arraysize+1];
};

/*! \brief Tiny Token */
class TokenTiny : public TokenStatic<16> {
public:
	TokenTiny(const char * s, const char * delim) : TokenStatic(s,delim){}
	TokenTiny(const String & s, const char * delim) : TokenStatic(s,delim){}
};

/*! \brief Small Token */
class TokenSmall : public TokenStatic<32> {
public:
	TokenSmall(const char * s, const char * delim) : TokenStatic(s,delim){}
	TokenSmall(const String & s, const char * delim) : TokenStatic(s,delim){}
};

/*! \brief Medium Token */
class TokenMedium : public TokenStatic<64> {
public:
	TokenMedium(const char * s, const char * delim) : TokenStatic(s,delim){}
	TokenMedium(const String & s, const char * delim) : TokenStatic(s,delim){}
};

/*! \brief Large Token */
class TokenLarge : public TokenStatic<128> {
public:
	TokenLarge(const char * s, const char * delim) : TokenStatic(s,delim){}
	TokenLarge(const String & s, const char * delim) : TokenStatic(s,delim){}
};


#endif /* TOKEN_HPP_ */

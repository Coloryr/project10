/*
  WString.h - String1 library for Wiring & Arduino
  ...mostly rewritten by Paul Stoffregen...
  Copyright (c) 2009-10 Hernando Barragan.  All right reserved.
  Copyright 2011, Paul Stoffregen, paul@pjrc.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef String_class_h
#define String_class_h
#ifdef __cplusplus

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "avr/pgmspace.h"

// When compiling programs with this class, the following gcc parameters
// dramatically increase performance and memory (RAM) efficiency, typically
// with little or no increase in code size.
//     -felide-constructors
//     -std=c++0x

class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))

// An inherited class for holding the result of a concatenation.  These
// result objects are assumed to be writable by subsequent concatenations.
class StringSumHelper;

// The string class
class String1
{
	// use a function pointer to allow for "if (s)" without the
	// complications of an operator bool(). for more information, see:
	// http://www.artima.com/cppsource/safebool.html
	typedef void (String1::*StringIfHelperType)() const;
	void StringIfHelper() const {}

public:
	// constructors
	// creates a copy of the initial value.
	// if the initial value is null or invalid, or if memory allocation
	// fails, the string will be marked as invalid (i.e. "if (s)" will
	// be false).
	String1(const char *cstr = "");
	String1(const String1 &str);
	String1(const __FlashStringHelper *str);
	#ifdef __GXX_EXPERIMENTAL_CXX0X__
	String1(String1 &&rval);
	String1(StringSumHelper &&rval);
	#endif
	explicit String1(char c);
	explicit String1(unsigned char, unsigned char base=10);
	explicit String1(int, unsigned char base=10);
	explicit String1(unsigned int, unsigned char base=10);
	explicit String1(long, unsigned char base=10);
	explicit String1(unsigned long, unsigned char base=10);
	explicit String1(float, unsigned char decimalPlaces=2);
	explicit String1(double, unsigned char decimalPlaces=2);
	~String1(void);

	// memory management
	// return true on success, false on failure (in which case, the string
	// is left unchanged).  reserve(0), if successful, will validate an
	// invalid string (i.e., "if (s)" will be true afterwards)
	unsigned char reserve(unsigned int size);
	inline unsigned int length(void) const {return len;}

	// creates a copy of the assigned value.  if the value is null or
	// invalid, or if the memory allocation fails, the string will be
	// marked as invalid ("if (s)" will be false).
	String1 & operator = (const String1 &rhs);
	String1 & operator = (const char *cstr);
	String1 & operator = (const __FlashStringHelper *str);
	#ifdef __GXX_EXPERIMENTAL_CXX0X__
	String1 & operator = (String1 &&rval);
	String1 & operator = (StringSumHelper &&rval);
	#endif

	// concatenate (works w/ built-in types)

	// returns true on success, false on failure (in which case, the string
	// is left unchanged).  if the argument is null or invalid, the
	// concatenation is considered unsucessful.
	unsigned char concat(const String1 &str);
	unsigned char concat(const char *cstr);
	unsigned char concat(char c);
	unsigned char concat(unsigned char c);
	unsigned char concat(int num);
	unsigned char concat(unsigned int num);
	unsigned char concat(long num);
	unsigned char concat(unsigned long num);
	unsigned char concat(float num);
	unsigned char concat(double num);
	unsigned char concat(const __FlashStringHelper * str);

	// if there's not enough memory for the concatenated value, the string
	// will be left unchanged (but this isn't signalled in any way)
	String1 & operator += (const String1 &rhs)	{concat(rhs); return (*this);}
	String1 & operator += (const char *cstr)		{concat(cstr); return (*this);}
	String1 & operator += (char c)			{concat(c); return (*this);}
	String1 & operator += (unsigned char num)		{concat(num); return (*this);}
	String1 & operator += (int num)			{concat(num); return (*this);}
	String1 & operator += (unsigned int num)		{concat(num); return (*this);}
	String1 & operator += (long num)			{concat(num); return (*this);}
	String1 & operator += (unsigned long num)	{concat(num); return (*this);}
	String1 & operator += (float num)		{concat(num); return (*this);}
	String1 & operator += (double num)		{concat(num); return (*this);}
	String1 & operator += (const __FlashStringHelper *str){concat(str); return (*this);}

	friend StringSumHelper & operator + (const StringSumHelper &lhs, const String1 &rhs);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, const char *cstr);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, char c);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned char num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, int num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned int num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, long num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned long num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, float num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, double num);
	friend StringSumHelper & operator + (const StringSumHelper &lhs, const __FlashStringHelper *rhs);

	// comparison (only works w/ Strings and "strings")
	operator StringIfHelperType() const { return buffer ? &String1::StringIfHelper : 0; }
	int compareTo(const String1 &s) const;
	unsigned char equals(const String1 &s) const;
	unsigned char equals(const char *cstr) const;
	unsigned char operator == (const String1 &rhs) const {return equals(rhs);}
	unsigned char operator == (const char *cstr) const {return equals(cstr);}
	unsigned char operator != (const String1 &rhs) const {return !equals(rhs);}
	unsigned char operator != (const char *cstr) const {return !equals(cstr);}
	unsigned char operator <  (const String1 &rhs) const;
	unsigned char operator >  (const String1 &rhs) const;
	unsigned char operator <= (const String1 &rhs) const;
	unsigned char operator >= (const String1 &rhs) const;
	unsigned char equalsIgnoreCase(const String1 &s) const;
	unsigned char startsWith( const String1 &prefix) const;
	unsigned char startsWith(const String1 &prefix, unsigned int offset) const;
	unsigned char endsWith(const String1 &suffix) const;

	// character acccess
	char charAt(unsigned int index) const;
	void setCharAt(unsigned int index, char c);
	char operator [] (unsigned int index) const;
	char& operator [] (unsigned int index);
	void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index=0) const;
	void toCharArray(char *buf, unsigned int bufsize, unsigned int index=0) const
		{getBytes((unsigned char *)buf, bufsize, index);}
	const char * c_str() const { return buffer; }

	// search
	int indexOf( char ch ) const;
	int indexOf( char ch, unsigned int fromIndex ) const;
	int indexOf( const String1 &str ) const;
	int indexOf( const String1 &str, unsigned int fromIndex ) const;
	int lastIndexOf( char ch ) const;
	int lastIndexOf( char ch, unsigned int fromIndex ) const;
	int lastIndexOf( const String1 &str ) const;
	int lastIndexOf( const String1 &str, unsigned int fromIndex ) const;
	String1 substring( unsigned int beginIndex ) const { return substring(beginIndex, len); };
	String1 substring( unsigned int beginIndex, unsigned int endIndex ) const;

	// modification
	void replace(char find, char replace);
	void replace(const String1& find, const String1& replace);
	void remove(unsigned int index);
	void remove(unsigned int index, unsigned int count);
	void toLowerCase(void);
	void toUpperCase(void);
	void trim(void);

	// parsing/conversion
	long toInt(void) const;
	float toFloat(void) const;

protected:
	char *buffer;	        // the actual char array
	unsigned int capacity;  // the array length minus one (for the '\0')
	unsigned int len;       // the String1 length (not counting the '\0')
protected:
	inline void init(void) {
            buffer = NULL;
	    capacity = 0;
	    len = 0;
        }

	void invalidate(void);
	unsigned char changeBuffer(unsigned int maxStrLen);
	unsigned char concat(const char *cstr, unsigned int length);

	// copy and move
	String1 & copy(const char *cstr, unsigned int length);
	String1 & copy(const __FlashStringHelper *pstr, unsigned int length);
	#ifdef __GXX_EXPERIMENTAL_CXX0X__
	void move(String1 &rhs);
	#endif
};

class StringSumHelper : public String1
{
public:
	StringSumHelper(const String1 &s) : String1(s) {}
	StringSumHelper(const char *p) : String1(p) {}
	StringSumHelper(char c) : String1(c) {}
	StringSumHelper(unsigned char num) : String1(num) {}
	StringSumHelper(int num) : String1(num) {}
	StringSumHelper(unsigned int num) : String1(num) {}
	StringSumHelper(long num) : String1(num) {}
	StringSumHelper(unsigned long num) : String1(num) {}
	StringSumHelper(float num) : String1(num) {}
	StringSumHelper(double num) : String1(num) {}
};

#endif  // __cplusplus
#endif  // String_class_h

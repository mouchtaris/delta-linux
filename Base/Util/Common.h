/**
 *	Common.h
 *
 *	November 2006
 */
#ifndef UTIL_COMMON_H
#define UTIL_COMMON_H

#pragma warning (disable:4786)
#pragma warning (disable:4996)

#include <functional>
#include <algorithm>
#include <utility>
#include <list>
#include <vector>
#include <map>

#include <boost/bind.hpp>
#include <boost/call_traits.hpp>

#include <assert.h>

#include "StringDefs.h"

////////////////////////////////////////////////////////////////////////
// Primitive Types. (though 32-bit ix86 specific).
//
typedef	unsigned char		uint8;
typedef	char				int8;

typedef	unsigned short		uint16;
typedef short				int16;

typedef unsigned int		uint32;
typedef int					int32;

typedef unsigned long long	uint64;
typedef long long			int64;

typedef unsigned long		ulong;
typedef unsigned int		uint;
typedef unsigned short		ushort;
typedef unsigned char		uchar;
typedef unsigned char		byte;

typedef util::char_type		tchar;

////////////////////////////////////////////////////////////////////////
// Common typedefs
//
typedef std::list<std::string>		StdStringList;
typedef std::list<String>			StringList;
typedef std::vector<std::string>	StdStringVec;
typedef std::vector<String>			StringVec;
typedef std::list<int>				IntList;
typedef std::list<unsigned int>		UIntList;
typedef std::string					Buffer;
typedef std::pair<String, bool>		StringBoolPair;

////////////////////////////////////////////////////////////////////////
// Useful in-class macros
//
#define DISABLE_CONSTRUCTION(CLASSNAME)		\
	CLASSNAME (void);						\
	~CLASSNAME (void);						\
	CLASSNAME (const CLASSNAME&);			\
	CLASSNAME& operator= (const CLASSNAME&)

#define DISABLE_COPY(CLASSNAME)				\
	CLASSNAME (const CLASSNAME&);			\
	CLASSNAME& operator= (const CLASSNAME&)

#define CLASS_TYPE_ID(NUM)					\
	enum { Type = NUM };					\
	virtual int GetType (void) const { return Type; }

////////////////////////////////////////////////////////////////////////
// Call traits defines
//
#define VALUE_T(TYPE)	boost::call_traits<TYPE>::value_type
#define PARAM_T(TYPE)	boost::call_traits<TYPE>::param_type
#define REF_T(TYPE)		boost::call_traits<TYPE>::reference
#define CREF_T(TYPE)	boost::call_traits<TYPE>::const_reference

#define VALUE(TYPE) typename boost::call_traits<TYPE>::value_type
#define PARAM(TYPE) typename boost::call_traits<TYPE>::param_type
#define REF(TYPE)	typename boost::call_traits<TYPE>::reference
#define CREF(TYPE)	typename boost::call_traits<TYPE>::const_reference

////////////////////////////////////////////////////////////////////////
// Useful macros
//
#define SIZEOF_ARRAY(ARR) (sizeof (ARR) / sizeof (ARR[0]))

//**********************************************************************

#define STRINGIFY_1(X)	#X
#define STRINGIFY(X)	STRINGIFY_1(X)

////////////////////////////////////////////////////////////////////////

namespace util {

////////////////////////////////////////////////////////////////////////
// Small template functions & functors
//
template <class Pair>
typename Pair::first_type select1st (const Pair& pair) { return pair.first; }

template <class Pair>
typename Pair::second_type select2nd (const Pair& pair) { return pair.second; }

//**********************************************************************

template <class Type>
struct identity : public std::unary_function <const Type&, const Type&> {
	result_type operator() (argument_type element) const { return element; }
};

//**********************************************************************

template <class TObject>
struct deleter : public std::unary_function<TObject*, void> {
	void operator() (TObject* p) const { delete p; }
};

//**********************************************************************

template<class T>
inline T clamp (const T x, const T low, const T high)
	{ return (x > high) ? high : (x < low) ? low : x; }

template<class T>
inline T abs (const T x) { return (x < 0) ? -x : x; }

////////////////////////////////////////////////////////////////////////

} // namespace util

#endif // UTIL_COMMON_H

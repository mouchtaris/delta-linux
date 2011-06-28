/**
 *	ExportedFunctionStubCommon.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_EXPORTEDFUNCTIONSTUBCOMMON_H
#define IDE_EXPORTEDFUNCTIONSTUBCOMMON_H

#include "Common.h"
#include "ExportedFunctionSetup.h"

#include "ComponentMemberCallback.h"
#include "Encoding.h"
#include "MessageUtils.h"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#include <boost/mpl/if.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// Extension to boost::preprocessor (enum stream operations)
//
#define JPP_ENUM_ARG_LIST(count, p1, p2, typeDecoratorMacro) \
	BOOST_PP_REPEAT(count, JPP_ENUM_ARG_LIST_M, (p1, p2, typeDecoratorMacro))

#define JPP_ENUM_ARG_LIST_M(z, n, pp)							\
	JPP_ENUM_ARG_LIST_M_I(z, n, BOOST_PP_TUPLE_ELEM(3, 0, pp),	\
		BOOST_PP_TUPLE_ELEM(3, 1, pp), BOOST_PP_TUPLE_ELEM(3, 2, pp))

#define JPP_ENUM_ARG_LIST_M_I(z, n, p1, p2, typeDecoratorMacro) \
	BOOST_PP_COMMA_IF(n) typeDecoratorMacro(BOOST_PP_CAT(p1, n)) BOOST_PP_CAT(p2, n)

////////////////////////////////////////////////////////////////////////

#define JPP_ENUM_LSHIFT_PARAMS(count, param)	BOOST_PP_REPEAT(count, JPP_ENUM_LSHIFT_PARAMS_M, param)
#define JPP_ENUM_LSHIFT_PARAMS_M(z, n, param)	JPP_LSHIFT_IF(n) param ## n
#define JPP_LSHIFT_IF(cond)						BOOST_PP_IF(cond, JPP_LSHIFT, BOOST_PP_EMPTY)()
#define JPP_LSHIFT()							<<

////////////////////////////////////////////////////////////////////////

template<bool vect_, typename R, class TDerived>
class ExportedFunctionStubBase {
public:
	template<typename T> struct Vectorize { typedef typename std::vector<T> type; };
	template<> struct Vectorize<void> { typedef void type; };

	typedef TDerived			DerivedType;
	typedef IDE_VALUE_TYPE(R)	ReturnValueType;

	typedef typename boost::mpl::if_c<
		vect_,
		typename Vectorize<ReturnValueType>::type,
		ReturnValueType
	>::type ReturnType;

	const DerivedType* This (void) const { return static_cast<const DerivedType*>(this); }
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_EXPORTEDFUNCTIONSTUBCOMMON_H

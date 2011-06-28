/**
 *	ExportedFunctionSkeleton.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_EXPORTEDFUNCTIONSKELETON_H
#define IDE_EXPORTEDFUNCTIONSKELETON_H

#include "Common.h"
#include "ExportedFunctionSetup.h"

#include "ComponentMemberCallback.h"
#include "Encoding.h"
#include "MessageUtils.h"

#include <boost/type_traits/add_const.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/iteration/local.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// Extension to boost::preprocessor (enum variable definitions)
//
#define JPP_ENUM_VARS(count, p1, p2) \
	BOOST_PP_REPEAT(count, JPP_ENUM_VARS_M, (p1, p2))

#define JPP_ENUM_VARS_M(z, n, pp) \
	JPP_ENUM_VARS_M_I(z, n, BOOST_PP_TUPLE_ELEM(2, 0, pp), BOOST_PP_TUPLE_ELEM(2, 1, pp))

#define JPP_ENUM_VARS_M_I(z, n, p1, p2) \
	JPP_SEMICOLON_IF(n) IDE_VALUE_TYPE(BOOST_PP_CAT(p1, n)) BOOST_PP_CAT(p2, n)

#define JPP_SEMICOLON_IF(cond) \
	BOOST_PP_IF(cond, JPP_SEMICOLON, BOOST_PP_EMPTY)()

#define JPP_SEMICOLON()	;

////////////////////////////////////////////////////////////////////////
// Extension to boost::preprocessor (enum stream operations)
//
#define JPP_ENUM_RSHIFT_PARAMS(count, param)	BOOST_PP_REPEAT(count, JPP_ENUM_RSHIFT_PARAMS_M, param)
#define JPP_ENUM_RSHIFT_PARAMS_M(z, n, param)	JPP_RSHIFT_IF(n) param ## n
#define JPP_RSHIFT_IF(cond)						BOOST_PP_IF(cond, JPP_RSHIFT, BOOST_PP_EMPTY)()
#define JPP_RSHIFT()							>>

////////////////////////////////////////////////////////////////////////

template <typename Signature>
class ExportedFunctionSkeleton;

//**********************************************************************
// Implement ExportedFunctionSkeleton with (void) argument-list for efficiency
//
template<>
class ExportedFunctionSkeleton<void (void)> {
public:
	template<class T>
	void operator() (T* comp, void (T::*func) (void), EXPORTED_FUNCTION_ARGS) const {
		MessageSetDefaultEndpoints(EXPORTED_FUNCTION_ARG_NAMES, comp);
		(comp->*func)();
	}
	void operator() (void (*func) (void), EXPORTED_FUNCTION_ARGS) const {
		MessageSetDefaultEndpoints(EXPORTED_FUNCTION_ARG_NAMES);
		(*func)();
	}
};

template<typename R>
class ExportedFunctionSkeleton<R (void)> {
public:
	template<class T>
	void operator() (T* comp, R (T::*func) (void), EXPORTED_FUNCTION_ARGS) const {
		MessageSetDefaultEndpoints(EXPORTED_FUNCTION_ARG_NAMES, comp);
		typename boost::add_const<R>::type retVal = (comp->*func)();
		result.SetData(comm::encoder() << retVal);
	}
	void operator() (R (*func) (void), EXPORTED_FUNCTION_ARGS) const {
		MessageSetDefaultEndpoints(EXPORTED_FUNCTION_ARG_NAMES);
		typename boost::add_const<R>::type retVal = (*func)();
		result.SetData(comm::encoder() << retVal);
	}
};

//**********************************************************************

#define ARG_CLASS_TYPE_LIST(count)	BOOST_PP_ENUM_PARAMS(count, class Param)
#define ARG_TYPE_LIST(count)		BOOST_PP_ENUM_PARAMS(count, Param)
#define ARG_LIST(count)				BOOST_PP_ENUM_PARAMS(count, param)
#define ARG_DEFINITION(count)		JPP_ENUM_VARS(count, Param, param)
#define ARG_DECODING(count)			JPP_ENUM_RSHIFT_PARAMS(count, param)

//**********************************************************************

#define CREATE_EXPORTED_FUNCTION_SKELETON(nArgs)												\
	template<ARG_CLASS_TYPE_LIST(nArgs)>														\
	class ExportedFunctionSkeleton<void (ARG_TYPE_LIST(nArgs))> {								\
	public:																						\
		template<class T>																		\
		void operator() (																		\
			T* comp, void (T::*func) (ARG_TYPE_LIST(nArgs)), EXPORTED_FUNCTION_ARGS				\
		) const {																				\
			MessageSetDefaultEndpoints(EXPORTED_FUNCTION_ARG_NAMES, comp);						\
																								\
			comm::decoder dec(msg.GetData());													\
			ARG_DEFINITION(nArgs);																\
			dec >> ARG_DECODING(nArgs);															\
			(comp->*func)(ARG_LIST(nArgs));														\
		}																						\
		void operator() (void (*func) (ARG_TYPE_LIST(nArgs)), EXPORTED_FUNCTION_ARGS) const {	\
			MessageSetDefaultEndpoints(EXPORTED_FUNCTION_ARG_NAMES);							\
																								\
			comm::decoder dec(msg.GetData());													\
			ARG_DEFINITION(nArgs);																\
			dec >> ARG_DECODING(nArgs);															\
			(*func)(ARG_LIST(nArgs));															\
		}																						\
	};																							\
																								\
	template<typename R, ARG_CLASS_TYPE_LIST(nArgs)>											\
	class ExportedFunctionSkeleton<R (ARG_TYPE_LIST(nArgs))> {									\
	public:																						\
		template<class T>																		\
		void operator() (																		\
			T* comp, R (T::*func) (ARG_TYPE_LIST(nArgs)), EXPORTED_FUNCTION_ARGS				\
		) const {																				\
			MessageSetDefaultEndpoints(EXPORTED_FUNCTION_ARG_NAMES, comp);						\
																								\
			comm::decoder dec(msg.GetData());													\
			ARG_DEFINITION(nArgs);																\
			dec >> ARG_DECODING(nArgs);															\
			typename boost::add_const<R>::type retVal = (comp->*func)(ARG_LIST(nArgs));			\
			result.SetData(comm::encoder() << retVal);											\
		}																						\
		void operator() (R (*func) (ARG_TYPE_LIST(nArgs)), EXPORTED_FUNCTION_ARGS) const {		\
			MessageSetDefaultEndpoints(EXPORTED_FUNCTION_ARG_NAMES);							\
																								\
			comm::decoder dec(msg.GetData());													\
			ARG_DEFINITION(nArgs);																\
			dec >> ARG_DECODING(nArgs);															\
			typename boost::add_const<R>::type retVal = (*func)(ARG_LIST(nArgs));				\
			result.SetData(comm::encoder() << retVal);											\
		}																						\
	};

//**********************************************************************

#define BOOST_PP_LOCAL_MACRO(n)   CREATE_EXPORTED_FUNCTION_SKELETON(n)
#define BOOST_PP_LOCAL_LIMITS     (1, IDE_EXPORTEDFUNCTION_MAX_ARGS)
#include BOOST_PP_LOCAL_ITERATE()

//**********************************************************************

#undef CREATE_EXPORTED_FUNCTION_SKELETON

#undef ARG_DECODING
#undef ARG_DEFINITION
#undef ARG_LIST
#undef ARG_TYPE_LIST
#undef ARG_CLASS_TYPE_LIST

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_EXPORTEDFUNCTIONSKELETON_H

/**
 *	ExportedFunctionStub.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_EXPORTEDFUNCTIONSTUB_H
#define IDE_EXPORTEDFUNCTIONSTUB_H

#include "Common.h"
#include "ExportedFunctionSetup.h"
#include "ExportedFunctionStubCommon.h"

#include "ComponentMemberCallback.h"
#include "Encoding.h"
#include "MessageUtils.h"

#include <boost/type_traits/add_const.hpp>

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/iteration/local.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

template<bool vect_, class TRetMsg, class TDerived, typename Signature>
class ExportedFunctionStub;

//**********************************************************************
// Implement ExportedFunctionStub with (void) argument-list
//
template<bool vect_, class TRetMsg, class TDerived>
class ExportedFunctionStub<vect_, TRetMsg, TDerived, void (void)> :
	public ExportedFunctionStubBase<vect_, void, TDerived> {
public:
	void operator() (void) const { This()->Invoke(TRetMsg(), Buffer()); }
};

template<bool vect_, class TRetMsg, class TDerived, typename R>
class ExportedFunctionStub<vect_, TRetMsg, TDerived, R (void)> :
	public ExportedFunctionStubBase<vect_, R, TDerived> {
public:
	ReturnType operator() (void) const {
		TRetMsg result;
		if (This()->Invoke(result, Buffer()))
			return DerivedType::ExtractReturnValue(result);
		else
			return ReturnType();
	}
};

//**********************************************************************

#define ARG_LIST(count)				JPP_ENUM_ARG_LIST(count, Param, param, PARAM)
#define ARG_CLASS_TYPE_LIST(count)	BOOST_PP_ENUM_PARAMS(count, class Param)
#define ARG_TYPE_LIST(count)		BOOST_PP_ENUM_PARAMS(count, Param)
#define ARG_ENCODING(count)			JPP_ENUM_LSHIFT_PARAMS(count, param)

//**********************************************************************

#define CREATE_EXPORTED_FUNCTION_STUB(nArgs)													\
	template<bool vect_, class TRetMsg, class TDerived, ARG_CLASS_TYPE_LIST(nArgs)>				\
	class ExportedFunctionStub<vect_, TRetMsg, TDerived, void (ARG_TYPE_LIST(nArgs))> :			\
		public ExportedFunctionStubBase<vect_, void, TDerived> {								\
	public:																						\
		void operator() (ARG_LIST(nArgs)) const													\
			{ This()->Invoke(TRetMsg(), comm::encoder() << ARG_ENCODING(nArgs)); }				\
	};																							\
																								\
	template<bool vect_, class TRetMsg, class TDerived, typename R, ARG_CLASS_TYPE_LIST(nArgs)>	\
	class ExportedFunctionStub<vect_, TRetMsg, TDerived, R (ARG_TYPE_LIST(nArgs))> :			\
		public ExportedFunctionStubBase<vect_, R, TDerived> {									\
	public:																						\
		ReturnType operator() (ARG_LIST(nArgs)) const {											\
			TRetMsg result;																		\
			if (This()->Invoke(result, comm::encoder() << ARG_ENCODING(nArgs)))					\
				return DerivedType::ExtractReturnValue(result);									\
			else																				\
				return ReturnType();															\
		}																						\
	};

//**********************************************************************

#define BOOST_PP_LOCAL_MACRO(n)   CREATE_EXPORTED_FUNCTION_STUB(n)
#define BOOST_PP_LOCAL_LIMITS     (1, IDE_EXPORTEDFUNCTION_MAX_ARGS)
#include BOOST_PP_LOCAL_ITERATE()

//**********************************************************************

#undef CREATE_EXPORTED_FUNCTION_STUB

#undef ARG_ENCODING
#undef ARG_TYPE_LIST
#undef ARG_CLASS_TYPE_LIST
#undef ARG_LIST

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_EXPORTEDFUNCTIONSTUB_H

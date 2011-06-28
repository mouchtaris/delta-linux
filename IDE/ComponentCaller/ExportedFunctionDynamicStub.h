/**
 *	ComponentFunctionDynamicStub.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTFUNCTIONDYNAMICSTUB_H
#define IDE_COMPONENTFUNCTIONDYNAMICSTUB_H

#include "Common.h"
#include "ExportedFunctionSetup.h"
#include "ExportedFunctionStubCommon.h"

#include "Encoding.h"
#include "MessageUtils.h"

#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/iteration/local.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

#define ARG_CREF_TYPE(type)			const type&
#define ARG_LIST(count)				JPP_ENUM_ARG_LIST(count, Param, param, ARG_CREF_TYPE)
#define ARG_CLASS_TYPE_LIST(count)	BOOST_PP_ENUM_PARAMS(count, class Param)
#define ARG_ENCODING(count)			JPP_ENUM_LSHIFT_PARAMS(count, param)

////////////////////////////////////////////////////////////////////////

template<bool vect_, class R, class TRetMsg, class TDerived>
class ExportedFunctionDynamicStub;

////////////////////////////////////////////////////////////////////////

template<bool vect_, class R, class TRetMsg, class TDerived>
class ExportedFunctionDynamicStub :
	public ExportedFunctionStubBase<vect_, R, TDerived> {
public:
	ReturnType operator() (void) const {
		TRetMsg result;
		This()->Call(result, Buffer());
		return DerivedType::ExtractReturnValue(result);
	}

#define CREATE_FUNCTION(nArgs)												\
	template<ARG_CLASS_TYPE_LIST(nArgs)>									\
	ReturnType operator() (ARG_LIST(nArgs)) const {							\
		TRetMsg result;														\
		if (This()->Invoke(result, comm::encoder() << ARG_ENCODING(nArgs)))	\
			return DerivedType::ExtractReturnValue(result);					\
		else																\
			return ReturnType();											\
	}

#define BOOST_PP_LOCAL_MACRO(n)   CREATE_FUNCTION(n)
#define BOOST_PP_LOCAL_LIMITS     (1, IDE_EXPORTEDFUNCTION_MAX_ARGS)
#include BOOST_PP_LOCAL_ITERATE()

#undef CREATE_FUNCTION

};

////////////////////////////////////////////////////////////////////////

template<bool vect_, class TRetMsg, class TDerived>
class ExportedFunctionDynamicStub<vect_, void, TRetMsg, TDerived> :
	public ExportedFunctionStubBase<vect_, void, TDerived> {
public:
	void operator() (void) const { This()->Invoke(TRetMsg(), Buffer()); }

#define CREATE_FUNCTION(nArgs)												\
	template<ARG_CLASS_TYPE_LIST(nArgs)>									\
	void operator() (ARG_LIST(nArgs)) const {								\
		This()->Invoke(TRetMsg(), comm::encoder() << ARG_ENCODING(nArgs));	\
	}

#define BOOST_PP_LOCAL_MACRO(n)   CREATE_FUNCTION(n)
#define BOOST_PP_LOCAL_LIMITS     (1, IDE_EXPORTEDFUNCTION_MAX_ARGS)
#include BOOST_PP_LOCAL_ITERATE()

#undef CREATE_FUNCTION

};

//**********************************************************************

#undef ARG_ENCODING
#undef ARG_CLASS_TYPE_LIST
#undef ARG_LIST
#undef ARG_CREF_TYPE

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTFUNCTIONDYNAMICSTUB_H

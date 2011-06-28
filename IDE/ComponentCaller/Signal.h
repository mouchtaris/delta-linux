/**
 *	Signal.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_SIGNAL_H
#define IDE_SIGNAL_H

#include "Common.h"
#include "IDECore.h"
#include "ComponentHandle.h"

#include "ExportedFunctionSetup.h"
#include "ExportedFunctionStubCommon.h"
#include "ComponentSignalEmitter.h"

#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/iteration/local.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

template<typename Signature>
class Signal;

//**********************************************************************

#define ARG_LIST(count)				JPP_ENUM_ARG_LIST(count, Param, param, PARAM)
#define ARG_CLASS_TYPE_LIST(count)	BOOST_PP_ENUM_PARAMS(count, class Param)
#define ARG_TYPE_LIST(count)		BOOST_PP_ENUM_PARAMS(count, Param)
#define ARG_ENCODING(count)			JPP_ENUM_LSHIFT_PARAMS(count, param)

//**********************************************************************

#define CREATE_EXPORTED_SIGNAL(nArgs)																\
	template<ARG_CLASS_TYPE_LIST(nArgs)>															\
	class Signal<void (ARG_TYPE_LIST(nArgs))> : ComponentSignalEmitter {							\
	public:																							\
		Signal (const std::string& classId, const std::string& signalId) :							\
			ComponentSignalEmitter(classId, signalId) {}											\
		void operator() (Component* comp BOOST_PP_COMMA_IF(nArgs) ARG_LIST(nArgs)) const {			\
			Handle handle(comp);																	\
			this->Invoke(comp, comm::encoder() << handle JPP_LSHIFT_IF(nArgs) ARG_ENCODING(nArgs));	\
		}																							\
		void operator() (ARG_LIST(nArgs)) const {													\
			const std::string& classId = this->GetClassId();										\
			this->Invoke(comm::encoder() << classId JPP_LSHIFT_IF(nArgs) ARG_ENCODING(nArgs));		\
		}																							\
	};

//**********************************************************************

#define BOOST_PP_LOCAL_MACRO(n)   CREATE_EXPORTED_SIGNAL(n)
#define BOOST_PP_LOCAL_LIMITS     (0, IDE_EXPORTEDFUNCTION_MAX_ARGS)
#include BOOST_PP_LOCAL_ITERATE()

//**********************************************************************

#undef CREATE_EXPORTED_SIGNAL

#undef ARG_ENCODING
#undef ARG_TYPE_LIST
#undef ARG_CLASS_TYPE_LIST
#undef ARG_LIST

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_SIGNAL_H

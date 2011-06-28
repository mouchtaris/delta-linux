/**
 *	RemoteValue.cpp
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	October 2010
 */
#include "RemoteValue.h"
#include "ExportedFunctionSetup.h"
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/iteration/local.hpp>

namespace ext {

////////////////////////////////////////////////////////////////////////

#define ARG_LIST(count)				BOOST_PP_ENUM_PARAMS(count, const RemoteValue& t)
#define ARG_USAGE(count)			BOOST_PP_REPEAT(count, ARG_USAGE_M, param)
#define ARG_USAGE_M(z, n, param)	l.push_back(t##n##);

#define CREATE_FUNCTION(nArgs)						\
	RemoteValueList ArgumentList(ARG_LIST(nArgs)) {	\
		RemoteValueList l;							\
		ARG_USAGE(nArgs)							\
		return l;									\
	}

#define BOOST_PP_LOCAL_MACRO(n)   CREATE_FUNCTION(n)
#define BOOST_PP_LOCAL_LIMITS     (0, IDE_EXPORTEDFUNCTION_MAX_ARGS)
#include BOOST_PP_LOCAL_ITERATE()

////////////////////////////////////////////////////////////////////////

} // namespace ext

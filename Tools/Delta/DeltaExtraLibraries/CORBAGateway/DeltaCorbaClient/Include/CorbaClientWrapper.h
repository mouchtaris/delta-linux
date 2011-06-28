#ifndef CORBA_CLIENT_WRAPPER
#define CORBA_CLIENT_WRAPPER

#include "utypes.h"
#include "uvalidatable.h"
#include "Corba.h"
#include "CorbaCommonLib.h"

#define CORBA_CLIENT_TYPE_STR	CORBA_NAMESPACE_PREFIX "client"

////////////////////////////////////////////////////////////

class DeltaValue;

class CorbaClientWrapper : public Validatable {
private:
	CORBA::Object_ptr object;
public:
	const char*				GetExtClassString	(void) const	{ return CORBA_CLIENT_TYPE_STR; }
	CORBA::Object_ptr		GetCorbaObject		(void)			{ return object; }

	DYNAMIC_CAST_NOBASE_INLINE_IMPL(CORBA_CLIENT_TYPE_STR)

	//**********************

	CorbaClientWrapper (CORBA::Object_ptr object) : object(object) {}
	virtual ~CorbaClientWrapper(){}
};

#define	CORBA_CLIENT_VAL(val)	\
		VGET_INST(CorbaClientWrapper, val, CORBA_CLIENT_TYPE_STR)

#define	DLIB_GET_CORBA_CLIENT \
	DLIB_GET_VALIDATABLE_ARG(corbaClientVal, corbaClientWrapper, CorbaClientWrapper, CORBA_CLIENT_TYPE_STR, CORBA_CLIENT_VAL)

////////////////////////////////////////////////////////////

#endif
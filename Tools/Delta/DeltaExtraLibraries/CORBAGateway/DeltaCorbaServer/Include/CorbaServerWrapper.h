#ifndef CORBA_SERVER_WRAPPER
#define CORBA_SERVER_WRAPPER

#include "utypes.h"
#include "uvalidatable.h"
#include "CorbaCommonLib.h"

#define CORBA_SERVER_TYPE_STR	CORBA_NAMESPACE_PREFIX "server"

////////////////////////////////////////////////////////////

class DeltaValue;
class CorbaServer;

class CorbaServerWrapper : public Validatable {
private:
	CorbaServer *server;
public:
	const char*		GetExtClassString	(void) const	{ return CORBA_SERVER_TYPE_STR; }
	CorbaServer *	GetCorbaObject		(void)			{ return server; }

	DYNAMIC_CAST_NOBASE_INLINE_IMPL(CORBA_SERVER_TYPE_STR)

	//**********************

	CorbaServerWrapper (CorbaServer *server) : server(server) {}
	virtual ~CorbaServerWrapper(){}
};

#define	CORBA_SERVER_VAL(val)	\
		VGET_INST(CorbaServerWrapper, val, CORBA_SERVER_TYPE_STR)

#define	DLIB_GET_CORBA_SERVER \
	DLIB_GET_VALIDATABLE_ARG(corbaServerVal, corbaServerWrapper, CorbaServerWrapper, CORBA_SERVER_TYPE_STR, CORBA_SERVER_VAL)

////////////////////////////////////////////////////////////

#endif
#ifndef CORBA_H
#define CORBA_H

#include "CorbaCommonLibDefs.h"
#include <list>

#include <tao/corba.h>
#include <tao/DynamicInterface/Request.h>
#include <tao/DynamicInterface/Server_Request.h>
#include <tao/DynamicInterface/ExceptionList.h>
#include <tao/DynamicInterface/Context.h>
#include <tao/DynamicInterface/Dynamic_Implementation.h>
#include <tao/DynamicInterface/Unknown_User_Exception.h>
#include <tao/IFR_Client/IFR_BasicC.h>
#include <tao/DynamicAny/DynamicAny.h>
#include <tao/DynamicAny/DynAnyFactory.h>
#include <tao/PortableServer/PortableServer.h>
#include "Naming_Client.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

CORBACOMMON_FUNC CORBA::Boolean operator >>= (const CORBA::Any& any, CORBA::Boolean& b);
CORBACOMMON_FUNC CORBA::Boolean operator >>= (const CORBA::Any& any, CORBA::Char& c);
CORBACOMMON_FUNC CORBA::Boolean operator >>= (const CORBA::Any& any, CORBA::WChar& wc);

CORBACOMMON_FUNC void operator <<= (CORBA::Any& any, CORBA::Boolean b);
CORBACOMMON_FUNC void operator <<= (CORBA::Any& any, CORBA::Char c);
CORBACOMMON_FUNC void operator <<= (CORBA::Any& any, CORBA::WChar wc);
CORBACOMMON_FUNC void operator <<= (CORBA::Any &any, const CORBA::Exception &exception);

/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace corba {
	CORBACOMMON_FUNC bool Initialize	(const std::list<std::string>& initRefs);
	CORBACOMMON_FUNC void Cleanup		(void);
	CORBACOMMON_FUNC bool IsInitialized	(void);

	CORBACOMMON_FUNC CORBA::ORB_ptr					GetORB			(void);
	CORBACOMMON_FUNC PortableServer::POA_ptr		GetPOA			(void);
	CORBACOMMON_FUNC CORBA::Repository_ptr			GetIFR			(void);
	CORBACOMMON_FUNC DynamicAny::DynAnyFactory_ptr	GetDynAnyFactory(void);

	CORBACOMMON_FUNC CORBA::Object_ptr			Resolve					(const char *name);
	CORBACOMMON_FUNC CORBA::Object_ptr			ResolveInitialReferences(const char *name);
	CORBACOMMON_FUNC bool						RegisterServer			(PortableServer::DynamicImplementation *server, const char *name);
	CORBACOMMON_FUNC bool						DeactivateServer		(PortableServer::DynamicImplementation *server);
	CORBACOMMON_FUNC CORBA::InterfaceDef_ptr	GetInterface			(const char *id);
};

#endif
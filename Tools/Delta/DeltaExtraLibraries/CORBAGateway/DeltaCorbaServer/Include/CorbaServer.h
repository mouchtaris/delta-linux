#ifndef CORBA_SERVER_H
#define CORBA_SERVER_H

#include "CorbaServerLibDefs.h"
#include "Corba.h"
#include "DDebug.h"
#include <map>
#include <list>

class DeltaTable;
class DeltaValue;
class CorbaServerWrapper;

class CORBASERVER_CLASS CorbaServer :
	public PortableServer::DynamicImplementation,
	public PortableServer::RefCountServantBase
{
private:
	typedef void (CorbaServer::*Handler)(CORBA::ServerRequest_ptr request);
	typedef std::map<std::string, Handler> StdServerFunctionMap;
	static StdServerFunctionMap stdFunctions;

	//**********************************************************************

	typedef std::list<CorbaServer*> CorbaServerList;
	static CorbaServerList servers;

	CORBA::InterfaceDef_ptr interfaceDef;
	DeltaTable *implementation;
	DeltaValue *externId;
	CorbaServerWrapper *wrapper;
	std::string _ior;

	//**********************************************************************

	CORBA::RepositoryId CorbaServer::repository_id(void);

	static void InitializeStdFunctionHandlers(void);

	void HandleIsARequest			(CORBA::ServerRequest_ptr request);
	void HandleNonExistentRequest	(CORBA::ServerRequest_ptr request);
	void HandleGetComponentRequest	(CORBA::ServerRequest_ptr request);
	void HandleGetInterfaceRequest	(CORBA::ServerRequest_ptr request);
	void HandleRepositoryIdRequest	(CORBA::ServerRequest_ptr request);

	//**********************************************************************

	typedef std::list<DeltaValue> DeltaArgumentList;
	DeltaArgumentList ExtractArguments(CORBA::NVList_ptr arglist);
	void HandleOutAndInoutParameters(DeltaArgumentList& arguments, CORBA::NVList_ptr arglist);

	CORBA::Any GenerateUserException(DeltaValue& exception, CORBA::ExceptionDefSeq_var exceptionList);
	void CorbaServer::ServerImplementationError(void);
public:
	CORBA::RepositoryId _primary_interface(const PortableServer::ObjectId& oid, PortableServer::POA_ptr poa);

	DeltaTable			* GetImplementation	(void) const { return implementation; }
	DeltaValue			* GetExternId		(void) const { return externId; }
	CorbaServerWrapper	* GetWrapper		(void) const { return wrapper; }

	void SetExternId	(DeltaValue *value)			{ externId = value; }
	void SetWrapper		(CorbaServerWrapper *value) { wrapper = value; }

	const std::string ior(void) const { return _ior; }

	virtual CORBA::Boolean _is_a(const char *name);
	virtual CORBA::InterfaceDef_ptr _get_interface();

	virtual void invoke(CORBA::ServerRequest_ptr request);

	//**********************************************************************

	static bool Initialize	(void);
	static void Cleanup		(void);

	//**********************************************************************

	CorbaServer(CORBA::InterfaceDef_ptr def, DeltaTable *implementation);
private:
	DFRIENDDESTRUCTOR();
	~CorbaServer();	//Instances of this class should be deleted only at exit through Cleanup.
};

#endif
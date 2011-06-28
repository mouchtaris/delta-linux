#include "Corba.h"
#include <assert.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////

CORBA::Boolean operator >>= (const CORBA::Any& any, CORBA::Boolean& b)
	{ return any >>= CORBA::Any::to_boolean(b); }

CORBA::Boolean operator >>= (const CORBA::Any& any, CORBA::Char& c)
	{ return any >>= CORBA::Any::to_char(c); }

CORBA::Boolean operator >>= (const CORBA::Any& any, CORBA::WChar& wc)
	{ return any >>= CORBA::Any::to_wchar(wc); }

void operator <<= (CORBA::Any& any, CORBA::Boolean b)
	{ any <<= CORBA::Any::from_boolean(b); }

void operator <<= (CORBA::Any& any, CORBA::Char c)
	{ any <<= CORBA::Any::from_char(c); }

void operator <<= (CORBA::Any& any, CORBA::WChar wc)
	{ any <<= CORBA::Any::from_wchar(wc); }

/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace corba {
	CORBA::ORB_ptr orb = (CORBA::ORB_ptr) 0;
	PortableServer::POA_ptr poa = (PortableServer::POA_ptr) 0;
	CORBA::Repository_ptr ifr = (CORBA::Repository_ptr) 0;
	DynamicAny::DynAnyFactory_ptr dynany_factory = (DynamicAny::DynAnyFactory_ptr) 0;

	static TAO_Naming_Client naming_client;
	static bool initialized = false;
	static std::list<std::string> initialReferences;
	static char **argv = (char **) 0;

	bool Initialize (const std::list<std::string>& initRefs) {
		assert(!IsInitialized());
		try {
			initialReferences = initRefs;
			int argc = 2 * (int) initialReferences.size() + 1;
			char **argv = new char *[argc + 1];
			argv[0] = "";	//executable name not available
			std::list<std::string>::const_iterator iter = initialReferences.begin();
			for (int i = 1; i < argc; i += 2) {
				argv[i] = "-ORBInitRef";
				assert(iter != initialReferences.end());
				argv[i + 1] = const_cast<char*>((*iter++).c_str());
			}
			assert(iter == initialReferences.end());
			argv[argc] = (char *) 0;
			if (!(orb = CORBA::ORB_init(argc, argv, (char *) 0)))
				return false;
			
			CORBA::Object_ptr poaObject = orb->resolve_initial_references("RootPOA");
			if(!poaObject || CORBA::is_nil(poa = PortableServer::POA::_narrow(poaObject)))
				return false;
			poa->the_POAManager()->activate();

			CORBA::Object_ptr repository = orb->resolve_initial_references("InterfaceRepository");
			if (!repository || CORBA::is_nil(ifr = CORBA::Repository::_narrow(repository)))
				return false;

			if(!(corba::dynany_factory = ::DynamicAny::DynAnyFactory::_narrow(
				corba::orb->resolve_initial_references("DynAnyFactory"))))
				return false;

			if (naming_client.init(orb) != 0)
				return false;
			
			return initialized = true;
		}
		catch(...) { return false; }
	}

	void Cleanup (void) {
		assert(IsInitialized());
		CORBA::release(dynany_factory);
		dynany_factory = (::DynamicAny::DynAnyFactory_ptr) 0;
		CORBA::release(ifr);
		ifr = (CORBA::Repository_ptr) 0;
		CORBA::release(poa);
		poa = (PortableServer::POA_ptr) 0;
		CORBA::release(orb);
		orb = (CORBA::ORB_ptr) 0;
		delete argv;
		argv = (char **) 0;
		initialReferences.clear();
		initialized = false;
	}

	bool IsInitialized (void) { return initialized; }

	//**********************************************************************

	CORBA::ORB_ptr					GetORB			(void) { return orb; }
	PortableServer::POA_ptr			GetPOA			(void) { return poa; }
	CORBA::Repository_ptr			GetIFR			(void) { return ifr; }
	DynamicAny::DynAnyFactory_ptr	GetDynAnyFactory(void) { return dynany_factory; }

	//**********************************************************************

	CORBA::Object_ptr Resolve(const char *name) {
		if (!initialized)
			return (CORBA::Object_ptr) 0;
		else {
			CosNaming::Name server_name(1);
			server_name.length(1);
			server_name[0].id = name;
			return naming_client->resolve(server_name);
		}
	}

	CORBA::Object_ptr ResolveInitialReferences(const char *name) {
		if (!initialized)
			return (CORBA::Object_ptr) 0;
		else
			return orb->resolve_initial_references(name);
	}

	bool RegisterServer(PortableServer::DynamicImplementation *server, const char *name) {
		if (!initialized)
			return false;
		else {
			CORBA::Object_var serverObject = poa->servant_to_reference(server);
			CosNaming::Name bindName;
			bindName.length (1);
			bindName[0]. id = CORBA::string_dup(name);
			naming_client->rebind(bindName, serverObject.in());
			return true;
		}
	}

	bool DeactivateServer(PortableServer::DynamicImplementation *server) {
		if (!initialized)
			return false;
		else {			
			poa->deactivate_object(*poa->servant_to_id(server));
			return true;
		}
	}

	CORBA::InterfaceDef_ptr GetInterface(const char *id) {
		if (!initialized)
			return (CORBA::InterfaceDef_ptr) 0;
		if (CORBA::Contained_ptr contained = ifr->lookup_id(id))
			return CORBA::InterfaceDef::_narrow(contained);
		else
			return (CORBA::InterfaceDef_ptr) 0;
	}
};

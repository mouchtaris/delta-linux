#include "CorbaServerLib.h"
#include "CorbaCommonLib.h"

#include "CorbaServer.h"
#include "CorbaServerWrapper.h"
#include "ToCorbaConverter.h"
#include "ToDeltaConverter.h"
#include "ConversionException.h"

#include "DeltaLibraryCreators.h"

//**********************************************************************
// FIELD GETTER
//
static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return CORBA_SERVER_VAL(val)? DPTR(getter)->GetAllKeys(val, at) : false; }

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys", &GetKeys,	DELTA_GETBYSTRING_NO_PRECOND }
};

//**********************************************************************
// API, CREATION, BINDERS.
//
static void corba_server_LibFunc (DeltaVirtualMachine*);
static void corba_run_LibFunc (DeltaVirtualMachine*);
static void corba_shutdown_LibFunc (DeltaVirtualMachine*);
static void corba_destroy_LibFunc (DeltaVirtualMachine*);
static void corba_register_LibFunc (DeltaVirtualMachine*);
static void corba_ior_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "server",		corba_server_LibFunc	},
	{ "run",		corba_run_LibFunc		},
	{ "shutdown",	corba_shutdown_LibFunc	},
	{ "destroy",	corba_destroy_LibFunc	},
	{ "register",	corba_register_LibFunc	},
	{ "ior",		corba_ior_LibFunc		}
};

//**********************************************************************

static bool FromObjectServerConversion(DeltaValue* output, CORBA::Object_ptr obj);
static bool ToObjectServerConversion(CORBA::Any* output, DeltaValue* input);

static void Initialize (void) {
	CorbaServer::Initialize();
	ToCorbaConverter::AddObjectConversion(ToObjectServerConversion);
	ToDeltaConverter::SetServerObjectConversion(FromObjectServerConversion);
}

static void Cleanup (void) {
	ToCorbaConverter::RemoveObjectConversion(ToObjectServerConversion);
	CorbaServer::Cleanup();
}

inline bool VAL_IS_CORBA_SERVER (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == CORBA_SERVER_TYPE_STR; }

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_CORBA_SERVER(val), CORBA_NAMESPACE_PREFIX CORBA_SERVER_TYPE_STR)

DELTALIBFUNC_DECLARECONSTS(3, uarraysize(funcs) - 3, "destroy", "ior")

DLIB_INITIALIZE_CLEANUP_FUNCS_EX(
	CorbaServer, 
	"server", 
	CORBA_NAMESPACE_PREFIX,
	Initialize(); DELTALIBFUNC_CREATE_METHODS_TABLE();,
	Cleanup(); DELTALIBFUNC_DESTROY_METHODS_TABLE();
)

DLIB_TOSTRING_FUNC(CorbaServerWrapper, CORBA_SERVER_TYPE_STR, CORBA_SERVER_VAL)

//**********************************************************************

static bool ToObjectServerConversion(CORBA::Any* output, DeltaValue* input) {
	std::string type;
	void *val;
	CorbaServerWrapper *corbaServer;
	if (input->Type() == DeltaValue_ExternId && (val = input->ToExternId(type)) &&
		type == CORBA_SERVER_TYPE_STR && (corbaServer = CORBA_SERVER_VAL(val))
	) {
		*output <<= corba::GetPOA()->servant_to_reference(corbaServer->GetCorbaObject());
		return true;
	}
	else
		return false;
}

static bool FromObjectServerConversion(DeltaValue* output, CORBA::Object_ptr obj) {
	PortableServer::Servant s;
	try { s = corba::GetPOA()->reference_to_servant(obj); }
	catch(CORBA::Exception&) { return false; }
	if (CorbaServer *server = dynamic_cast<CorbaServer*>(s)) {
		output->Assign(server->GetExternId());
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////////////////

DLIB_FUNC_START(corba_server, 2, Nil)
	DLIB_ARG(const char*, id);
	DLIB_ARG(DeltaTable*, serverObject);
	ERROR_IF_CORBA_NOT_INITIALIZED;
	try {
		if (CORBA::InterfaceDef_ptr def = corba::GetInterface(DLIB_ARGVAL(id))) {
			CorbaServer *server = DNEWCLASS(CorbaServer, (def, DLIB_ARGVAL(serverObject)));
			CorbaServerWrapper *wrapper = DNEWCLASS(CorbaServerWrapper, (server));
			DeltaValue *externId = DNEWCLASS(DeltaValue, (
				(void *) wrapper->GetSerialNo(),
				DeltaExternId_NonCollectable,
				ToString,
				CORBA_SERVER_TYPE_STR,
				getter
			));
			externId->GetExternIdUserData()->Set(CORBA_SERVER_NATIVE_INDEX, server->GetImplementation());
			DELTALIBFUNC_DELEGATE_METHODS(*externId);
			server->SetWrapper(wrapper);
			server->SetExternId(externId);
			DLIB_RETVAL_REF.Assign(*externId);
		}
		else
			{ DLIB_RESET_RETURN; }
	}
	catch(const CORBA::Exception& e) { DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(e._rep_id());	}
DLIB_FUNC_END

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(corba_run, 0, Empty)
	ERROR_IF_CORBA_NOT_INITIALIZED;
	try { corba::GetORB()->run(); }
	catch(const CORBA::Exception& e) { DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(e._rep_id());	}
DLIB_FUNC_END

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(corba_shutdown, 0, Empty)
	ERROR_IF_CORBA_NOT_INITIALIZED;
	try { corba::GetORB()->shutdown(); }
	catch(const CORBA::Exception& e) { DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(e._rep_id());	}
DLIB_FUNC_END

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(corba_destroy, 1, Empty)
	DLIB_GET_CORBA_SERVER;
	ERROR_IF_CORBA_NOT_INITIALIZED;
	try {
		CorbaServer *server = corbaServerWrapper->GetCorbaObject();
		DASSERT(server);
		corba::DeactivateServer(server);
		DDELETE(server);
	}
	catch(const CORBA::Exception& e) { DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(e._rep_id());	}
DLIB_FUNC_END

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(corba_register, 2, Empty)
	DLIB_GET_CORBA_SERVER;
	DLIB_ARG(const char*, name);
	ERROR_IF_CORBA_NOT_INITIALIZED;
	try {
		CorbaServer *server = corbaServerWrapper->GetCorbaObject();
		DASSERT(server);
		DLIB_RETVAL_REF.FromBool(corba::RegisterServer(server, DLIB_ARGVAL(name)));
	}
	catch(const CORBA::Exception& e) { DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(e._rep_id());	}
DLIB_FUNC_END

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(corba_ior, 1, Nil)
	DLIB_GET_CORBA_SERVER;
	ERROR_IF_CORBA_NOT_INITIALIZED;
	try {
		CorbaServer *server = corbaServerWrapper->GetCorbaObject();
		DASSERT(server);
		DLIB_RETVAL_REF.FromString(server->ior());
	}
	catch(const CORBA::Exception& e) { DELTA_EXCEPTIONS_THROW_INSIDE_LIBRARY_FUNC(e._rep_id());	}
DLIB_FUNC_END
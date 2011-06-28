#include "CorbaServer.h"
#include "ToCorbaConverter.h"
#include "ToDeltaConverter.h"
#include "ConversionException.h"
#include "CorbaServerWrapper.h"

#include "Delta.h"

/////////////////////////////////////////////////////////////////////

CorbaServer::CorbaServerList CorbaServer::servers;
CorbaServer::StdServerFunctionMap CorbaServer::stdFunctions;

/////////////////////////////////////////////////////////////////////

CORBA::Boolean CorbaServer::_is_a(const char *name) { return interfaceDef->is_a(name); }

CORBA::InterfaceDef_ptr CorbaServer::_get_interface() { return CORBA::InterfaceDef::_duplicate(interfaceDef); }

CORBA::RepositoryId CorbaServer::_primary_interface(const PortableServer::ObjectId& oid, PortableServer::POA_ptr poa)
	{ return repository_id(); }

CORBA::RepositoryId CorbaServer::repository_id(void)
	{ return CORBA::string_dup(interfaceDef->describe_interface()->id); }

/////////////////////////////////////////////////////////////////////

void CorbaServer::InitializeStdFunctionHandlers(void) {
	stdFunctions["_is_a"]			= &CorbaServer::HandleIsARequest;
	stdFunctions["_non_existent"]	= &CorbaServer::HandleNonExistentRequest;
	stdFunctions["_component"]		= &CorbaServer::HandleGetComponentRequest;
	stdFunctions["_interface"]		= &CorbaServer::HandleGetInterfaceRequest;
	stdFunctions["_repository_id"]	= &CorbaServer::HandleRepositoryIdRequest;
}

#define IMPLEMENT_ZERO_ARG_STD_FUNCTION(_name, _value)			\
	void CorbaServer::_name(CORBA::ServerRequest_ptr request) {	\
		CORBA::NVList_ptr arglist;								\
		corba::GetORB()->create_list(0, arglist);				\
		request->arguments(arglist);							\
		CORBA::Any any;											\
		any <<= _value;											\
		request->set_result(any);								\
	}

//This SHOULD take an arg and use it to invoke _is_a(name),
//but no arg is supplied so just always return true.
IMPLEMENT_ZERO_ARG_STD_FUNCTION(HandleIsARequest, true)

IMPLEMENT_ZERO_ARG_STD_FUNCTION(HandleNonExistentRequest, _non_existent())
IMPLEMENT_ZERO_ARG_STD_FUNCTION(HandleGetComponentRequest, _get_component())
IMPLEMENT_ZERO_ARG_STD_FUNCTION(HandleGetInterfaceRequest, _get_interface())
IMPLEMENT_ZERO_ARG_STD_FUNCTION(HandleRepositoryIdRequest, repository_id())

/////////////////////////////////////////////////////////////////////

CorbaServer::DeltaArgumentList CorbaServer::ExtractArguments(CORBA::NVList_ptr arglist) {
	const CORBA::ULong totalFormals = arglist->count();
	DeltaArgumentList arguments;
	for (CORBA::ULong i = 0; i < totalFormals; ++i) {
		CORBA::NamedValue_ptr nv = arglist->item(i);
		DeltaValue arg;
		if (nv->flags() != CORBA::ARG_OUT)
			ToDeltaConverter::convert(&arg, nv->value());
		if (nv->flags() == CORBA::ARG_OUT || nv->flags() == CORBA::ARG_INOUT) {
			DeltaTable* table = DeltaObject::NewObject();
			DeltaValue o(table);
			table->Set((DeltaNumberValueType) 0, arg);
			arg.Assign(o);
		}
		arguments.push_front(arg);	//arguments given in inverse order
	}
	return arguments;
}

//**********************************************************************

void CorbaServer::HandleOutAndInoutParameters(DeltaArgumentList& arguments, CORBA::NVList_ptr arglist) {
	const CORBA::ULong totalFormals = arglist->count();
	DeltaArgumentList::reverse_iterator iter = arguments.rbegin();
	for (CORBA::ULong i = 0; i < totalFormals; ++i, ++iter) {
		CORBA::NamedValue_ptr nv = arglist->item(i);
		const CORBA::Flags flags = nv->flags();
		if (flags == CORBA::ARG_OUT || flags == CORBA::ARG_INOUT) {
			CORBA::Any *out = nv->value();
			DASSERT(iter->Type() == DeltaValue_Table);
			DeltaValue value;
			DASSERT(iter != arguments.rend());
			iter->ToTable()->Get((DeltaNumberValueType) 0, &value);
			try { ToCorbaConverter::convert(out, &value, out->type()); }
			catch(const ConversionException& e) {
				const char *typeStr = flags == CORBA::ARG_OUT ? "out" : "inout";
				std::cout << "Server implementation error: cannot convert " << e.what() <<
							 " value for corba " << typeStr << " arg " << i << std::endl;
				throw CORBA::DATA_CONVERSION();
			}
		}
	}
}

//**********************************************************************

CORBA::Any CorbaServer::GenerateUserException(DeltaValue& exception, CORBA::ExceptionDefSeq_var exceptionList) {
	CORBA::Any any;
	any <<= CORBA::UNKNOWN(0, CORBA::COMPLETED_YES);
	if (exception.Type() == DeltaValue_Table && exceptionList->length()) {
		DeltaTable *table = exception.ToTable();
		DeltaValue id, value;
		if (table->Get("id", &id) && id.Type() == DeltaValue_String &&
			table->Get("value", &value) && !value.IsNil()
		) {
			for (CORBA::ULong i = 0; i < exceptionList->length(); ++i) {
				CORBA::TypeCode_var tc = exceptionList[i]->type();
				const std::string tcId = tc->id();
				if (tcId == id.ToString()) {
					try { ToCorbaConverter::convert(&any, &value, tc.in()); }
					catch(const ConversionException&) {}
					break;
				}
			}
		}
	}
	return any;
}

//**********************************************************************

void CorbaServer::ServerImplementationError(void) {
	const std::string error  = uerror::GetSingleton().getreport();
	std::cout << error << std::endl;
	uerror::GetSingleton().clear();
	DeltaVirtualMachine::ResetRunTimeErrors();
	throw CORBA::INTERNAL();
}

/////////////////////////////////////////////////////////////////////

void CorbaServer::invoke(CORBA::ServerRequest_ptr request) {
	static const std::string getPrefix = "_get_";
	static const std::string setPrefix = "_set_";
	CORBA::ORB_ptr orb = corba::GetORB();
	CORBA::NVList_ptr arglist;

	const char *operation = request->operation();
	StdServerFunctionMap::const_iterator i = stdFunctions.find(operation);
	if (i != stdFunctions.end()) {
		(this->*i->second)(request);
		return;
	}

	CORBA::InterfaceDef_var intf_def = _get_interface();
	DASSERT(intf_def);

	if (!strncmp(operation, getPrefix.c_str(), getPrefix.size())) {
		const std::string attribute = operation + getPrefix.size();
		CORBA::ContainedSeq_var ctd_seq = intf_def->lookup_name(attribute.c_str(), -1, CORBA::dk_Attribute, false);
		if (!ctd_seq || ctd_seq->length() != 1)
			throw CORBA::BAD_OPERATION();
		else {
			CORBA::AttributeDef_var attr_def = CORBA::AttributeDef::_narrow(ctd_seq.in()[0]);
			orb->create_list(0, arglist);
			request->arguments(arglist);

			DeltaValue value;
			if (!implementation->Get(attribute, &value)) {
				DASSERT(UERROR_ISRAISED());
				ServerImplementationError();
			}
			else if (value.IsNil())
				throw CORBA::NO_IMPLEMENT();
			else {
				CORBA::Any any;
				ToCorbaConverter::convert(&any, &value, attr_def->type());
				request->set_result(any);
			}
		}
	}
	else if (!strncmp(operation, setPrefix.c_str(), setPrefix.size())) {
		const std::string attribute = operation + setPrefix.size();
		CORBA::ContainedSeq_var ctd_seq = intf_def->lookup_name(attribute.c_str(), -1, CORBA::dk_Attribute, false);
		if (!ctd_seq || ctd_seq->length() != 1)
			throw CORBA::BAD_OPERATION();
		else {
			CORBA::AttributeDef_var attr_def = CORBA::AttributeDef::_narrow(ctd_seq.in()[0]);
			orb->create_list(0, arglist);
			CORBA::Any* any = arglist->add(CORBA::ARG_IN)->value();
			request->arguments(arglist);

			DeltaValue value;
			ToDeltaConverter::convert(&value, any);
			if (!implementation->Set(attribute, value)) {
				DASSERT(UERROR_ISRAISED());
				ServerImplementationError();
			}
		}
	}
	else {
		CORBA::ContainedSeq_var ctd_seq = intf_def->lookup_name(operation, -1, CORBA::dk_Operation, false);
		if (!ctd_seq || ctd_seq->length() != 1)
			throw CORBA::BAD_OPERATION();
		CORBA::OperationDef_var op_def = CORBA::OperationDef::_narrow(ctd_seq.in()[0]);
		DASSERT(op_def);

		DeltaValue function;
		implementation->Get(operation, &function);
		if (function.IsCallable()) {
			orb->create_operation_list(op_def.in(), arglist);
			request->arguments(arglist);
			DeltaArgumentList arguments;
			try { arguments = ExtractArguments(arglist); }
			catch(const ConversionException&) { throw CORBA::MARSHAL(); }

			DeltaValue retval;
			DELTA_EXCEPTIONS_NATIVE_TRY { function(arguments, &retval); }
			DELTA_EXCEPTIONS_NATIVE_TRAP(exception) {
				request->set_exception(GenerateUserException(exception, op_def->exceptions()));
				return;
			}

			if (UERROR_ISRAISED())
				ServerImplementationError();
			
			CORBA::TypeCode_var result_tc = op_def->result();
			if (result_tc->kind() != CORBA::tk_void) {
				CORBA::Any any;
				try { ToCorbaConverter::convert(&any, &retval, result_tc); }
				catch(const ConversionException& e) {
					std::cout << "Server implementation error: return value cannot be converted to " << e.what() << std::endl;
					std::string type;
					void *val;
					if (retval.Type() == DeltaValue_ExternId && (val = retval.ToExternId(type)) &&
						type == CORBA_SERVER_TYPE_STR && !CORBA_SERVER_VAL(val)
					)
						std::cout << "invalid " << CORBA_SERVER_TYPE_STR << " handle, probably destroyed!";

					throw CORBA::DATA_CONVERSION();
				}
				request->set_result(any);
			}

			HandleOutAndInoutParameters(arguments, arglist);
		}
		else
			throw CORBA::NO_IMPLEMENT();
	}
}

/////////////////////////////////////////////////////////////////////

bool CorbaServer::Initialize(void) {
	InitializeStdFunctionHandlers();
	return true;
}

void CorbaServer::Cleanup(void) {
	stdFunctions.clear();
	PortableServer::POA_ptr poa = corba::GetPOA();
	for(CorbaServerList::iterator i = servers.begin(); i != servers.end(); ++i) {
		poa->deactivate_object(*poa->servant_to_id(*i));
		DDELETE(*i);
	}
}

/////////////////////////////////////////////////////////////////////

CorbaServer::CorbaServer(CORBA::InterfaceDef_ptr def, DeltaTable *implementation) : 
	interfaceDef(CORBA::InterfaceDef::_duplicate(def)), implementation(implementation)
{
	CORBA::Object_var object = corba::GetPOA()->servant_to_reference(this);
	CORBA::String_var s = corba::GetORB()->object_to_string(object);
	_ior = s;
	implementation->IncRefCounter((DeltaValue *) 0);
	servers.push_back(this);
}

CorbaServer::~CorbaServer() {
	servers.remove(this);
	DASSERT(externId && wrapper);
	DDELETE(externId);
	DDELETE(wrapper);
	implementation->DecRefCounter((DeltaValue *) 0);
	CORBA::release(interfaceDef);
}

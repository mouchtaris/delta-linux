/**
 *	RemoteDeploymentAPI.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 *	Updated October 2010 by by JL (Lilis).
 */

#include "Common.h"
#include "RemoteDeploymentAPI.h"
#include "RemoteFunctionCaller.h"
#include "IOServiceThread.h"
#include "MessageRouterClient.h"

#include <boost/foreach.hpp>

namespace ext {

////////////////////////////////////////////////////////////////////////

void RemoteDeploymentAPI::Initialize (const std::string& host, unsigned short port)
{
	net::IOServiceThreadSingleton::Initialize();
	net::IOServiceThreadSingleton::Instance().Start();

	MessageRouterClient::Initialize();
	MessageRouterClient::Instance().Connect(util::std2str(host), port);
}

void RemoteDeploymentAPI::CleanUp (void)
{
	MessageRouterClient::CleanUp();
	net::IOServiceThreadSingleton::CleanUp();
}

//**********************************************************************

template<class T>
static void PerformCall (const T& target, const std::string& function, const RemoteValueList& arguments, RemoteValue *retval)
{
	Message result;
	comm::encoder args;
	BOOST_FOREACH(const RemoteValue& arg, arguments) {
		if (arg.IsUndefined())
			throw RemoteDeploymentAPI::InvalidArgumentsException();
		switch(arg.GetType()) {
			case ext::RemoteValueTypeVoid:	throw RemoteDeploymentAPI::InvalidArgumentsException();
#define CASE(_valuetype, _ctype, _func) case ext::_valuetype: { _ctype value = arg._func(); args << value; break; }
			CASE(RemoteValueTypeUChar,		unsigned char,	GetUChar)
			CASE(RemoteValueTypeChar,		char,			GetChar)
			CASE(RemoteValueTypeUShort,		unsigned short,	GetUShort)
			CASE(RemoteValueTypeShort,		short,			GetShort)
			CASE(RemoteValueTypeUInt,		unsigned int,	GetUInt)
			CASE(RemoteValueTypeInt,		int,			GetInt)
			CASE(RemoteValueTypeULong,		unsigned long,	GetULong)
			CASE(RemoteValueTypeLong,		long,			GetLong)
			CASE(RemoteValueTypeFloat,		float,			GetFloat)
			CASE(RemoteValueTypeDouble,		double,			GetDouble)
			CASE(RemoteValueTypeBool,		bool,			GetBool)
			CASE(RemoteValueTypeString,		std::string,	GetString)
			CASE(RemoteValueTypeWxString,	wxString,		GetWxString)
#undef CASE
			case ext::RemoteValueTypeHandle:
				{
					RemoteHandle remote = arg.GetHandle();
					ide::Handle handle(remote.classId, remote.serial);
					args << handle;
					break;
				}
			default: assert(false);
		}
	}

	try { RemoteFunctionCaller("Remote", target, function).Invoke(result, args); }
	catch(...) { throw RemoteDeploymentAPI::CallFailedException(); }

	try {
		comm::decoder decoder(result.GetData());
		if (retval)
			switch(retval->GetType()) {
				case ext::RemoteValueTypeVoid:	break;
#define CASE(_valuetype, _ctype, _func)	\
	case ext::_valuetype: { _ctype value; decoder >> value; retval->_func(value); break; }
				CASE(RemoteValueTypeUChar,		unsigned char,	SetUChar)
				CASE(RemoteValueTypeChar,		char,			SetChar)
				CASE(RemoteValueTypeUShort,		unsigned short,	SetUShort)
				CASE(RemoteValueTypeShort,		short,			SetShort)
				CASE(RemoteValueTypeUInt,		unsigned int,	SetUInt)
				CASE(RemoteValueTypeInt,		int,			SetInt)
				CASE(RemoteValueTypeULong,		unsigned long,	SetULong)
				CASE(RemoteValueTypeLong,		long,			SetLong)
				CASE(RemoteValueTypeFloat,		float,			SetFloat)
				CASE(RemoteValueTypeDouble,		double,			SetDouble)
				CASE(RemoteValueTypeBool,		bool,			SetBool)
				CASE(RemoteValueTypeString,		std::string,	SetString)
				CASE(RemoteValueTypeWxString,	wxString,		SetWxString)
#undef CASE
				case ext::RemoteValueTypeHandle:
					{
						ide::Handle handle;
						decoder >> handle;
						retval->SetHandle(RemoteHandle(handle.GetClassId(), handle.GetSerial()));
						break;
					}
				default: assert(false);
			}
	}
	catch(...) { throw RemoteDeploymentAPI::InvalidReturnValueException(); }
}

void RemoteDeploymentAPI::Call (const std::string& classId, const std::string& function, const RemoteValueList& arguments, RemoteValue *retval)
	{ PerformCall(classId, function, arguments, retval); }

void RemoteDeploymentAPI::Call (const RemoteHandle& handle, const std::string& function, const RemoteValueList& arguments, RemoteValue *retval)
	{ PerformCall(ide::Handle(handle.classId, handle.serial), function, arguments, retval); }

////////////////////////////////////////////////////////////////////////

} // namespace ext

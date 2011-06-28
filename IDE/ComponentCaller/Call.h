/**
 *	Call.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_CALL_H
#define IDE_CALL_H

#include "Common.h"
#include "IDECore.h"
#include "Message.h"
#include "MessageUtils.h"
#include "ExportedFunctionStub.h"
#include "ComponentFunctionCaller.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;

////////////////////////////////////////////////////////////////////////

template<class Signature, class TFunctionCaller = ComponentFunctionCaller>
class Call :
	public TFunctionCaller,
	public ExportedFunctionStub<false, Message, Call<Signature, TFunctionCaller>, Signature> {
public:
	typedef typename ExportedFunctionStub<
			false, Message, Call<Signature, TFunctionCaller>, Signature
		>::ReturnType ReturnType;

	Call (void) {}
	Call (Component* caller, const Handle& handle, const std::string& function) :
		TFunctionCaller(caller, handle, function) {}

	Call (Component* caller, const std::string& classId, const std::string& function) :
		TFunctionCaller(caller, classId, function) {}

	Call (const std::string& callerId, const Handle& handle, const std::string& function) :
		TFunctionCaller(callerId, handle, function) {}

	Call (const std::string& callerId, const std::string& classId, const std::string& function) :
		TFunctionCaller(callerId, classId, function) {}

	//******************************************************************

	static ReturnType ExtractReturnValue (const Message& result)
		{ return MessageExtractSingleValue<ReturnType>(result); }
};

////////////////////////////////////////////////////////////////////////

template<class Signature, class TFunctionCaller = ComponentFunctionCaller>
class MCall :
	public TFunctionCaller,
	public ExportedFunctionStub<true, MessageVec, MCall<Signature, TFunctionCaller>, Signature> {
public:
	typedef typename ExportedFunctionStub<
			true, MessageVec, MCall<Signature, TFunctionCaller>, Signature
		>::ReturnType ReturnType;

	MCall (void) {}
	MCall (Component* caller, const std::string& classId, const std::string& function) :
		TFunctionCaller(caller, classId, function) {}

	MCall (const std::string& callerId, const std::string& classId, const std::string& function) :
		TFunctionCaller(callerId, classId, function) {}

	//******************************************************************

	static ReturnType ExtractReturnValue (const MessageVec& results)
		{ return MessageExtractSingleValue<ReturnType>(results); }
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_CALL_H

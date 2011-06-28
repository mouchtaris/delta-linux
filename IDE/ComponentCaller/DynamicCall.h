/**
 *	DynamicCall.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_DYNAMICCALL_H
#define IDE_DYNAMICCALL_H

#include "Common.h"
#include "IDECore.h"

#include "Message.h"
#include "ExportedFunctionDynamicStub.h"
#include "ComponentFunctionCaller.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;
class Handle;

////////////////////////////////////////////////////////////////////////

template<class R, class TFunctionCaller = ComponentFunctionCaller>
class DCall :
	public TFunctionCaller,
	public ExportedFunctionDynamicStub< false, R, Message, DCall<R, TFunctionCaller> > {
public:
	typedef typename ExportedFunctionDynamicStub<
			false, R, Message, DCall<R, TFunctionCaller>
		>::ReturnType ReturnType;

	DCall (void) {}
	DCall (Component* caller, const Handle& handle, const std::string& function) :
		TFunctionCaller(caller, handle, function) {}

	DCall (Component* caller, const std::string& classId, const std::string& function) :
		TFunctionCaller(caller, classId, function) {}

	DCall (const std::string& callerId, const Handle& handle, const std::string& function) :
		TFunctionCaller(callerId, handle, function) {}

	DCall (const std::string& callerId, const std::string& classId, const std::string& function) :
		TFunctionCaller(callerId, classId, function) {}

	//******************************************************************

	static ReturnType ExtractReturnValue (const Message& result)
		{ return MessageExtractSingleValue<ReturnType>(result); }
};

//////////////////////////////////////////////////////////////////////

template<class R, class TFunctionCaller = ComponentFunctionCaller>
class MDCall :
	public TFunctionCaller,
	public ExportedFunctionDynamicStub< true, R, MessageVec, MDCall<R, TFunctionCaller> > {
public:
	typedef typename ExportedFunctionDynamicStub<
			true, R, MessageVec, MDCall<R, TFunctionCaller>
		>::ReturnType ReturnType;

	MDCall (void) {}
	MDCall (Component* caller, const std::string& classId, const std::string& function) :
		TFunctionCaller(caller, classId, function) {}

	MDCall (const std::string& callerId, const std::string& classId, const std::string& function) :
		TFunctionCaller(callerId, classId, function) {}

	//******************************************************************

	static ReturnType ExtractReturnValue (const MessageVec& results)
		{ return MessageExtractSingleValue<ReturnType>(results); }
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_DYNAMICCALL_H

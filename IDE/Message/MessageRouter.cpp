/**
 *	MessageRouter.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "MessageRouter.h"
#include "Component.h"
#include "ComponentSignalRegistry.h"
#include "ComponentRegistry.h"
#include "Component.h"
#include "Algorithms.h"

#include <boost/functional.hpp>
#include <boost/foreach.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

inline const ComponentEntry& _GetComponentEntry (const std::string& classId)
	{ return ComponentRegistry::Instance().GetComponentEntry(classId); }

inline const ComponentEntry& _GetComponentEntry (const Message& msg)
	{ return _GetComponentEntry(msg.GetDst().classId); }

//**********************************************************************

inline bool _InvokeMemberFuncEntry (
		Component*					component,
		const ComponentFuncEntry&	func,
		Message&					result,
		const Message&				args
	)
{
	if (func && component) {
		assert(func.IsMemberFunc());
		return func.GetMemberFunc()(component, result, args);
	}
	return false;
}

//**********************************************************************

inline bool _InvokeStaticFuncEntry (
		const ComponentFuncEntry&	func,
		Message&					result,
		const Message&				args
	)
{
	if (func) {
		assert(func.IsStaticFunc());
		return func.GetStaticFunc()(result, args);
	}
	return false;
}

//**********************************************************************

inline void _InvokeStaticSlot (const ComponentSlot& slot, const Message& msg)
{
	const ComponentFuncEntry& func = _GetComponentEntry(slot.classId).GetFuncEntry(slot.funcId);
	Message result;
	Message argMsg(Message::Endpoint(slot.classId, slot.funcId), msg.GetData());

	_InvokeStaticFuncEntry(func, result, argMsg);
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(MessageRouter);

//**********************************************************************

bool MessageRouter::DispatchMessage (
		const std::string&	callerId, 
		Message&			result, 
		const Message&		msg
	)
{
	const std::string			funcName	= msg.GetDst().function;
	const ComponentEntry&		entry		= _GetComponentEntry(msg);
	const ComponentFuncEntry*	func		= (const ComponentFuncEntry*) 0;

	Component* comp = msg.GetDst().serial != 0 ? entry.GetInstance(msg.GetDst().serial) : entry.GetFocusedInstance();
	if (comp) 	// Dynamic functions have higher lookup priiority than methods / static functions.
		if (const ComponentFuncEntry& entry = comp->GetDynamicFuncEntry(funcName)) {
			func = &entry;
			assert(func->IsMemberFunc());
		}

	if (!func)
		func = &entry.GetFuncEntry(funcName);

	if (*func && (comp || func->IsStaticFunc())) {	// Member funcs for destroyed comps should not be dispatched.
		sigPreDispatchMessage(callerId, msg, false, *func);

		bool returnValue = false;
		
		if (func->IsMemberFunc())
			returnValue = _InvokeMemberFuncEntry(comp, *func, result, msg);
		else {
			assert(func->IsStaticFunc());
			returnValue = _InvokeStaticFuncEntry(*func, result, msg);
		}
		sigPostDispatchMessage(callerId, msg, false, *func, returnValue);

		return returnValue;
	}
	else
		return false;
}

//**********************************************************************

bool MessageRouter::DispatchMessageToAll (
		const std::string& callerId, MessageVec& results, const Message& msg
	)
{
	const ComponentEntry& entry = _GetComponentEntry(msg);
	const ComponentFuncEntry& func = entry.GetFuncEntry(msg.GetDst().function);

	bool returnValue = false;
	sigPreDispatchMessage(callerId, msg, true, func);

	if (func.IsMemberFunc()) {

		bool retVal = true;
		BOOST_FOREACH(Component* inst, entry.GetInstances(true)) {
			if (entry.HasInstance(inst, true)) {
				results.push_back(Message());
				retVal = _InvokeMemberFuncEntry(inst, func, results.back(), msg) && retVal;
			}
		}
		returnValue = retVal;
	}
	else if (func.IsStaticFunc()) {
		results.push_back(Message());
		returnValue = _InvokeStaticFuncEntry(func, results.back(), msg);
	}

	sigPostDispatchMessage(callerId, msg, true, func, returnValue);
	return returnValue;
}

//**********************************************************************

void MessageRouter::DispatchSignal (const Message& msg)
{
	const ComponentSignal& signal = ComponentSignalRegistry::Instance().
		GetSignal(msg.GetSrc().function);

	util::for_all(signal.GetStaticSlotList(), boost::bind2nd(_InvokeStaticSlot, msg));

	////////////////////////////////////////////////////////////////////
	// Invoking member slots. Need to take inheritance into account.
	//
	const ComponentSignal::MemberSlotMap& slotMap = signal.GetMemberSlotMap();
	typedef std::map<Component*, const ComponentEntry*> ComponentMap;
	ComponentMap instances;		//First gather all instances related to the signal
	BOOST_FOREACH(const ComponentSlot& slot, slotMap) {
		const ComponentEntry& targetEntry = _GetComponentEntry(slot.classId);
		ComponentEntry::InstanceList entryInstances = targetEntry.GetInstances(true);
		BOOST_FOREACH(Component* comp, entryInstances)
			instances[comp] = (const ComponentEntry*) 0;
	}

	BOOST_FOREACH(ComponentMap::value_type& pair, instances)	//Then match them with their derived ComponentEntries
		pair.second = &_GetComponentEntry(pair.first->GetDerivedClassId());

	Message result;	//ignored
	BOOST_FOREACH(ComponentMap::value_type& pair, instances) {	//Finally apply the function to the correct entry in the hierarchy
		Component* comp = pair.first;
		const ComponentEntry* targetEntry = pair.second;
		if (targetEntry->HasInstance(comp)) {
			std::string classId = comp->GetClassId();
			ComponentSignal::MemberSlotMap::const_iterator i = slotMap.find(classId);
			while(i == slotMap.end()) {	//Find the most derived class with a slot entry
				classId = targetEntry->GetBaseClassId();
				assert(!classId.empty());
				targetEntry = &_GetComponentEntry(classId);
				i = slotMap.find(classId);
			}
			const ComponentSlot& slot = *i;
			const ComponentFuncEntry& targetFunc = targetEntry->GetFuncEntry(slot.funcId);
			Message argMsg(Message::Endpoint(slot.classId, slot.funcId), msg.GetData());
			if (targetEntry->HasInstance(comp, true)) {
				argMsg.GetDst().serial = comp->GetSerial();
				_InvokeMemberFuncEntry(comp, targetFunc, result, argMsg);
			}
		}
	}

	////////////////////////////////////////////////////////////////////
	// Invoking dynamic instance slots. No inheritance taken into account.
	//
	const ComponentSignal::InstanceSlotMap dynamicSlots = signal.GetInstanceSlotMap();
	//copy first as handlers my invalidate the slot map
	BOOST_FOREACH(const ComponentSignal::InstanceSlotMap::value_type& pair, dynamicSlots)
		if (Component* comp = pair.first.Resolve()) {
			const std::string funcName = pair.second;
			Message argMsg(Message::Endpoint(comp->GetClassId(), funcName), msg.GetData());
			argMsg.GetDst().serial = comp->GetSerial();

			if (const ComponentFuncEntry& entry = comp->GetDynamicFuncEntry(funcName))
				_InvokeMemberFuncEntry(comp, entry, result, argMsg);
		}
}

//**********************************************************************

void MessageRouter::BeginBatchUndoAction (const std::string& callerId)
	{ sigBatchUndoBegin(callerId); }

void MessageRouter::EndBatchUndoAction (const std::string& callerId)
	{ sigBatchUndoEnd(callerId); }

void MessageRouter::RegisterUndoMessage (const Message& msg)
	{ sigRegisteredUndoMessage(msg); }

////////////////////////////////////////////////////////////////////////

} // namespace ide

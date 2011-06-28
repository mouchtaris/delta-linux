/**
 *	ComponentSignalRegistry.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "Portability.h"

#include "ComponentSignalRegistry.h"
#include "ComponentEntry.h"
#include "Algorithms.h"
#include "Adaptors.h"
#include "Streams.h"

#include <boost/foreach.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// ComponentSlot
//
ComponentSlot::ComponentSlot (const std::string& classId, const std::string& funcId) :
	classId(classId), funcId(funcId)
{

}

//**********************************************************************

ComponentSlot::~ComponentSlot (void)
{

}

////////////////////////////////////////////////////////////////////////
// DynamicComponentSlot
//
DynamicComponentSlot::DynamicComponentSlot (const Handle& handle, const std::string& funcId) :
	handle(handle), funcId(funcId)
{

}

//**********************************************************************

DynamicComponentSlot::~DynamicComponentSlot (void)
{

}

////////////////////////////////////////////////////////////////////////
// ComponentSignal
//
ComponentSignal::ComponentSignal (const std::string& id) :
	m_id(id), m_undefined(true), m_isStatic(false)
{

}

//**********************************************************************

ComponentSignal::ComponentSignal (const std::string& id, const std::string& argList, bool isStatic) :
	m_id(id), m_undefined(true), m_isStatic(isStatic)
{
	this->SetArgList(argList, isStatic);
}

//**********************************************************************

const std::string ComponentSignal::GetSignature (void) const
{
	const std::string args = m_undefined ? "..." : FunctionGetArgList(m_argVec);
	return "void " + m_id + " (" + args + ")";
}

//**********************************************************************

void ComponentSignal::ConnectSlot (const ComponentSlot& slot, bool isStatic)
{
	if (isStatic)
		m_staticSlots.push_back(slot);
	else
		m_memberSlots.insert(slot);
}

//**********************************************************************

void ComponentSignal::ConnectSlot (const DynamicComponentSlot& slot)
{
	m_instanceSlots.insert(std::make_pair(slot.handle, slot.funcId));
}

//**********************************************************************

bool ComponentSignal::DisconnectSlot (const ComponentSlot& slot)
{
	MemberSlotMap::iterator sIter = m_memberSlots.find(slot.classId);
	if (sIter != m_memberSlots.end() && sIter->funcId == slot.funcId) {
		m_memberSlots.erase(sIter);
		return true;
	}
	StaticSlotList::iterator mIter = std::find(m_staticSlots.begin(), m_staticSlots.end(), slot);
	if (mIter != m_staticSlots.end()) {
		m_staticSlots.erase(mIter);
		return true;
	}
	return false;
}

//**********************************************************************

bool ComponentSignal::DisconnectSlot (const Handle& handle)
{
	InstanceSlotMap::iterator i = m_instanceSlots.find(handle);
	if (i != m_instanceSlots.end()) {
		m_instanceSlots.erase(i);
		return true;
	}
	else
		return false;
}

//**********************************************************************

bool ComponentSignal::DisconnectSlot (const std::string& classId)
{
	bool retval = false;
	for(InstanceSlotMap::iterator i = m_instanceSlots.begin(); i != m_instanceSlots.end(); ++i)
		if (i->first.GetClassId() == classId) {
			m_instanceSlots.erase(i);
			retval = true;
			break;
		}

	MemberSlotMap::iterator iter = m_memberSlots.find(classId);
	if (iter != m_memberSlots.end()) {
		m_memberSlots.erase(iter);
		return true;
	}

	StaticSlotList::iterator mIter = std::find(m_staticSlots.begin(), m_staticSlots.end(), classId);
	if (mIter != m_staticSlots.end()) {
		m_staticSlots.erase(mIter);
		return true;
	}
	return retval;
}

//**********************************************************************

bool ComponentSignal::HasSlot (const ComponentSlot& slot) const
{
	MemberSlotMap::iterator iter = m_memberSlots.find(slot.classId);
	if (iter != m_memberSlots.end())
		return iter->funcId == slot.funcId;
	return std::find(m_staticSlots.begin(), m_staticSlots.end(), slot) != m_staticSlots.end();
}

//**********************************************************************

bool ComponentSignal::HasSlot (const Handle& handle) const
{
	return m_instanceSlots.find(handle) != m_instanceSlots.end();
}

//**********************************************************************

void ComponentSignal::SetArgList (const std::string& argList, bool isStatic)
{
	if (m_undefined) {

		if (isStatic)
			m_argVec.push_back(ArgumentVec::value_type("std::string", "classId"));
		else
			m_argVec.push_back(ArgumentVec::value_type("Handle", "component"));

		FunctionDecomposeArgs(m_argVec, argList);
		m_undefined = false;
		m_isStatic = isStatic;
	}

#ifndef NDEBUG

	else {

		if (m_isStatic != isStatic)
			DBGOUT << "Signal: " << m_id << " differs in linkage" << std::endl;

		ArgumentVec newArgs;
		FunctionDecomposeArgs(newArgs, argList);
		bool mismatch = (newArgs.size() != m_argVec.size() - 1);
		if (!mismatch) {
			for (size_t i = 1; i < m_argVec.size(); ++i) {
				if (newArgs[i - 1].first != m_argVec[i].first) {
					mismatch = true;
					break;
				}
			}
		}
		if (mismatch)
			DBGOUT << "Signal: " << m_id << " differs in signature" << std::endl;
	}

#endif // NDEBUG
}

////////////////////////////////////////////////////////////////////////
// ComponentSignalRegistry class
//
IMPLEMENT_SINGLETON(ComponentSignalRegistry);

//**********************************************************************

ComponentSignalRegistry::~ComponentSignalRegistry (void)
{
	BOOST_MULTI_INDEX_CONTAINER_CLEAR(m_signalMap);
}

//**********************************************************************

void ComponentSignalRegistry::AddSignal (const std::string& signal, const std::string& argList, bool isStatic)
{
	SignalMap::iterator sigIter = m_signalMap.find(signal);
	if (sigIter == m_signalMap.end())
		m_signalMap.insert(ComponentSignal(signal, argList, isStatic));
	else
		const_cast<ComponentSignal&>(*sigIter).SetArgList(argList, isStatic);
}

//**********************************************************************

void ComponentSignalRegistry::ConnectSlot (const std::string& signal, const ComponentSlot& slot, bool isStatic)
{
	SignalMap::iterator sigIter = m_signalMap.find(signal);
	if (sigIter == m_signalMap.end())
		const_cast<ComponentSignal&>(*m_signalMap.insert(ComponentSignal(signal)).first).ConnectSlot(slot, isStatic);
	else
		const_cast<ComponentSignal&>(*sigIter).ConnectSlot(slot, isStatic);
}

//**********************************************************************

void ComponentSignalRegistry::ConnectDynamicSlot (const std::string& signal, const DynamicComponentSlot& slot)
{
	SignalMap::iterator sigIter = m_signalMap.find(signal);
	if (sigIter == m_signalMap.end())
		const_cast<ComponentSignal&>(*m_signalMap.insert(ComponentSignal(signal)).first).ConnectSlot(slot);
	else
		const_cast<ComponentSignal&>(*sigIter).ConnectSlot(slot);
}

//**********************************************************************

void ComponentSignalRegistry::DisconnectSlot (const std::string& signal, const std::string& classId)
{
	SignalMap::iterator sigIter = m_signalMap.find(signal);
	if (sigIter != m_signalMap.end())
		const_cast<ComponentSignal&>(*sigIter).DisconnectSlot(classId);
}

//**********************************************************************

void ComponentSignalRegistry::DisconnectSlot (const ComponentSlot& slot)
{
	BOOST_FOREACH(const ComponentSignal& entry, m_signalMap)
		if (const_cast<ComponentSignal&>(entry).DisconnectSlot(slot))
			break;
}

//**********************************************************************

void ComponentSignalRegistry::DisconnectDynamicSlot (const std::string& signal, const Handle& handle)
{
	BOOST_FOREACH(const ComponentSignal& entry, m_signalMap)
		if (const_cast<ComponentSignal&>(entry).DisconnectSlot(handle))
			break;
}

//**********************************************************************

void ComponentSignalRegistry::DisconnectAllSlots (const std::string& classId)
{
	BOOST_FOREACH(const ComponentSignal& entry, m_signalMap)
		const_cast<ComponentSignal&>(entry).DisconnectSlot(classId);
}

//**********************************************************************

const ComponentSignal& ComponentSignalRegistry::GetSignal (const std::string& signal) const
{
	static const ComponentSignal nullSignal("undefined");
	SignalMap::iterator iter = m_signalMap.find(signal);
	return iter != m_signalMap.end() ? *iter : nullSignal;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide

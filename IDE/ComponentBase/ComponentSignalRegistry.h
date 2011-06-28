/**
 *	ComponentSignalRegistry.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTSIGNALREGISTRY_H
#define IDE_COMPONENTSIGNALREGISTRY_H

#include "Common.h"
#include "IDECommon.h"
#include "Singleton.h"
#include "MultiIndex.h"
#include "FunctionUtils.h"
#include "ComponentHandle.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentSlot {
public:
	ComponentSlot (const std::string& classId, const std::string& funcId);
	~ComponentSlot (void);

	bool operator== (const ComponentSlot& csd) const
		{ return classId == csd.classId && funcId == csd.funcId; }

	bool operator== (const std::string& cId) const { return classId == cId; }

	const std::string	classId;
	const std::string	funcId;
};

////////////////////////////////////////////////////////////////////////

class _IDE_API DynamicComponentSlot {
public:
	DynamicComponentSlot (const Handle& handle, const std::string& funcId);
	~DynamicComponentSlot (void);

	bool operator== (const DynamicComponentSlot& slot) const
		{ return handle == slot.handle && funcId == slot.funcId; }

	const Handle		handle;
	const std::string	funcId;
};

////////////////////////////////////////////////////////////////////////

class ComponentSignalRegistry;

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentSignal {
public:
	typedef std::list<ComponentSlot> StaticSlotList;

	typedef multi_index_container<
		ComponentSlot,
		indexed_by<
			ordered_unique<
				member<ComponentSlot, const std::string, &ComponentSlot::classId>
			>
		>
	> MemberSlotMap;

	typedef std::map<Handle, std::string> InstanceSlotMap;

	//******************************************************************

	ComponentSignal (const std::string& id);
	ComponentSignal (const std::string& id, const std::string& argList, bool isStatic);
	~ComponentSignal (void) {}

	//******************************************************************

	const std::string&		GetId (void) const { return m_id; }
	const std::string		GetSignature (void) const;
	const ArgumentVec&		GetArgumentVec (void) const { return m_argVec; }

	const StaticSlotList&	GetStaticSlotList (void) const { return m_staticSlots; }
	const MemberSlotMap&	GetMemberSlotMap (void) const { return m_memberSlots; }
	const InstanceSlotMap&	GetInstanceSlotMap (void) const { return m_instanceSlots; }

	//******************************************************************

	void ConnectSlot (const ComponentSlot& slot, bool isStatic);
	void ConnectSlot (const DynamicComponentSlot& slot);
	bool DisconnectSlot (const ComponentSlot& slot);
	bool DisconnectSlot (const Handle& handle);
	bool DisconnectSlot (const std::string& classId);

	//******************************************************************

	operator bool (void) const { return !m_id.empty(); }

	bool IsUndefined (void) const { return m_undefined; }
	bool HasSlot (const ComponentSlot& slot) const;
	bool HasSlot (const Handle& handle) const;
	bool IsStatic (void) const { return m_isStatic; }

protected:
	friend class ComponentSignalRegistry;

	void SetArgList (const std::string& argList, bool isStatic);

private:
	const std::string	m_id;
	ArgumentVec			m_argVec;
	bool				m_undefined;
	bool				m_isStatic;

	StaticSlotList		m_staticSlots;
	MemberSlotMap		m_memberSlots;
	InstanceSlotMap		m_instanceSlots;
};

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentSignalRegistry : public util::Singleton {
	DECLARE_SINGLETON(ComponentSignalRegistry);

public:

	typedef multi_index_container<
		ComponentSignal,
		indexed_by<
			ordered_unique<
				member<ComponentSignal, const std::string, &ComponentSignal::m_id>
			>
		>
	> SignalMap;

	void AddSignal (const std::string& signal, const std::string& argList, bool isStatic);

	//******************************************************************

	void ConnectSlot (const std::string& signal, const ComponentSlot& slot, bool isStatic);
	void ConnectDynamicSlot (const std::string& signal, const DynamicComponentSlot& slot);	//Member slots

	//******************************************************************

	void DisconnectSlot (const std::string& signal, const std::string& classId);
	void DisconnectSlot (const ComponentSlot& slot);
	void DisconnectDynamicSlot (const std::string& signal, const Handle& handle);

	void DisconnectAllSlots (const std::string& classId);

	//******************************************************************

	const ComponentSignal&	GetSignal (const std::string& signal) const;
	const SignalMap&		GetSignalMap (void) const { return m_signalMap; }

private:
	ComponentSignalRegistry (void) {}
	~ComponentSignalRegistry (void);

	SignalMap m_signalMap;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTSIGNALREGISTRY_H

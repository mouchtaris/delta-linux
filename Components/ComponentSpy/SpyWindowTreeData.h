/**
 *	SpyWindowTreeData.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_SPYWINDOWTREEDATA_H
#define IDE_SPYWINDOWTREEDATA_H

#include "Common.h"

#include <boost/function.hpp>

#include <wx/treebase.h>
class wxMenu;

namespace ide {

class Component;

////////////////////////////////////////////////////////////////////////

enum SWCommandType {
	SWCMD_NONE,
	SWCMD_INVOKE_FUNC,
	SWCMD_REMOVE_FUNC,
	SWCMD_REMOVE_SIGNAL,
	SWCMD_DELETE_INSTANCE,
	SWCMD_UNLOAD_COMPONENT,
	SWCMD_CONFIGURE_COMPONENT,
};

////////////////////////////////////////////////////////////////////////

class TreeData : public wxTreeItemData {
public:
	typedef boost::function<void (bool)> ResultFunc;

	TreeData (void);
	virtual ~TreeData (void);

	virtual bool IsComponent (const std::string& id) const { return false; }
	virtual bool IsFunction (const std::string& id) const { return false; }
	virtual bool IsHandler (const std::string& id) const { return false; }
	virtual bool IsInstance (uint serial) const { return false; }
	virtual bool IsInstanceClass (const std::string& compId) const { return false; }
	virtual bool IsSignal (const std::string& id) const { return false; }

	// DoAction() returns true if this action changed current element.
	// However, when the element is deleted it happily returns false.
	//
	virtual wxMenu*			GetMenu (void) const { return 0; }
	virtual bool			DoAction (SWCommandType type) const { return false; }
	virtual const String	GetFormattedSummary (void) const = 0;

	void PostAction (SWCommandType type, const ResultFunc& func = ResultFunc());
};

////////////////////////////////////////////////////////////////////////

class ComponentTreeData : public TreeData {
public:
	ComponentTreeData (const std::string& id) : m_id(id) {}
	~ComponentTreeData (void);

	bool IsComponent (const std::string& id) const { return m_id == id; }

	wxMenu*			GetMenu (void) const;
	bool			DoAction (SWCommandType type) const;
	const String	GetFormattedSummary (void) const;

private:
	const std::string	m_id;
	static wxMenu*		s_menu;
};

////////////////////////////////////////////////////////////////////////

class AbstractFunctionTreeData : public TreeData {
public:
	AbstractFunctionTreeData (const std::string& compId, const std::string& id, const std::string& type) :
		m_compId(compId), m_id(id), m_type(type) {}
	virtual ~AbstractFunctionTreeData (void);

	wxMenu*			GetMenu (void) const;
	bool			DoAction (SWCommandType type) const;
	const String	GetFormattedSummary (void) const;

protected:
	const std::string	m_compId;
	const std::string	m_id;
	const std::string	m_type;
	static wxMenu*		s_menu;
};

class AbstractDynamicFunctionTreeData : public TreeData {
public:
	AbstractDynamicFunctionTreeData (Component* comp, const std::string& id, const std::string& type) :
		m_comp(comp), m_id(id), m_type(type) {}
	virtual ~AbstractDynamicFunctionTreeData (void);

	wxMenu*			GetMenu (void) const;
	bool			DoAction (SWCommandType type) const;
	const String	GetFormattedSummary (void) const;

protected:
	Component*			m_comp;
	const std::string	m_id;
	const std::string	m_type;
	static wxMenu*		s_menu;
};

////////////////////////////////////////////////////////////////////////

class FunctionTreeData : public AbstractFunctionTreeData {
public:
	FunctionTreeData (const std::string& compId, const std::string& id) :
		AbstractFunctionTreeData(compId, id, "Function") {}
	bool IsFunction (const std::string& id) const { return m_id == id; }
};

class DynamicFunctionTreeData : public AbstractDynamicFunctionTreeData {
public:
	DynamicFunctionTreeData (Component* comp, const std::string& id) :
		AbstractDynamicFunctionTreeData(comp, id, "Function") {}
	bool IsFunction (const std::string& id) const { return m_id == id; }
};

////////////////////////////////////////////////////////////////////////

class HandlerTreeData : public AbstractFunctionTreeData {
public:
	HandlerTreeData (const std::string& compId, const std::string& id) :
		AbstractFunctionTreeData(compId, id, "Handler") {}
	bool IsHandler (const std::string& id) const { return m_id == id; }
};

class DynamicHandlerTreeData : public AbstractDynamicFunctionTreeData {
public:
	DynamicHandlerTreeData (Component* comp, const std::string& id) :
		AbstractDynamicFunctionTreeData(comp, id, "Handler") {}
	bool IsHandler (const std::string& id) const { return m_id == id; }
};

////////////////////////////////////////////////////////////////////////

class InstanceTreeData : public TreeData {
public:
	InstanceTreeData (const std::string& compId, uint serial) :
		m_compId(compId), m_serial(serial) {}
	~InstanceTreeData (void);

	bool IsInstance (uint serial) const { return m_serial == serial; }
	bool IsInstanceClass (const std::string& compId) const { return m_compId == compId; }

	wxMenu*			GetMenu (void) const;
	bool			DoAction (SWCommandType type) const;
	const String	GetFormattedSummary (void) const;

private:
	const std::string	m_compId;
	uint				m_serial;
	static wxMenu*		s_menu;
};

////////////////////////////////////////////////////////////////////////

class SignalTreeData : public TreeData {
public:
	SignalTreeData (const std::string& compId, const std::string& id) :
		m_compId(compId), m_id(id) {}
	~SignalTreeData (void);

	bool IsSignal (const std::string& id) const { return m_id == id; }

	wxMenu*			GetMenu (void) const;
	bool			DoAction (SWCommandType type) const;
	const String	GetFormattedSummary (void) const;

private:
	const std::string	m_compId;
	const std::string	m_id;
	static wxMenu*		s_menu;
};

////////////////////////////////////////////////////////////////////////

#define CONTAINER_CLASS(_name)									\
	class _name : public TreeData {								\
	public:														\
		_name (const std::string& compId) : m_compId(compId) {}	\
		~_name (void) {}										\
		const String GetFormattedSummary (void) const;			\
	private:													\
		const std::string m_compId;								\
	}

CONTAINER_CLASS(AllFunctionsTreeData);
CONTAINER_CLASS(MemberFunctionsTreeData);
CONTAINER_CLASS(StaticFunctionsTreeData);
CONTAINER_CLASS(DynamicFunctionsTreeData);
CONTAINER_CLASS(AllHandlersTreeData);
CONTAINER_CLASS(MemberHandlersTreeData);
CONTAINER_CLASS(StaticHandlersTreeData);
CONTAINER_CLASS(DynamicHandlersTreeData);
CONTAINER_CLASS(AllInstancesTreeData);
CONTAINER_CLASS(MemberSignalsTreeData);
CONTAINER_CLASS(StaticSignalsTreeData);
CONTAINER_CLASS(AllSignalsTreeData);

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_SPYWINDOWTREEDATA_H

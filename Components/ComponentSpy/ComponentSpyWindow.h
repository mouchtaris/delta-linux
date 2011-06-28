/**
 *	ComponentSpyWindow.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTSPYWINDOW_H
#define IDE_COMPONENTSPYWINDOW_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "Component.h"
#include "ComponentEntry.h"

#include <wx/splitter.h>
#include <wx/treebase.h>
class wxTreeCtrl;

#include <boost/function.hpp>
#include <boost/signals/trackable.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

class HTMLWindowExt;
class TreeData;

////////////////////////////////////////////////////////////////////////

class ComponentSpyWindow :
	public wxSplitterWindow, public boost::signals::trackable {
public:
	ComponentSpyWindow (void);
	ComponentSpyWindow (wxWindow* parent);
	~ComponentSpyWindow (void);

	bool Create (wxWindow* parent);

	//******************************************************************

	void AddComponent (const ComponentEntry& entry);
	void RemoveComponent (const std::string& id);

	void AddFunctionOrHandler (const std::string& compId, const ComponentFuncEntry& entry);
	void RemoveFunctionOrHandler (const std::string& compId, const std::string& id, bool isSlot, bool isStatic);

	void AddDynamicFunctionOrHandler (Component* comp, const ComponentFuncEntry& entry);
	void RemoveDynamicFunctionOrHandler (Component* comp, const std::string& id, bool isSlot);

	void AddInstance (Component* instance);
	void RemoveInstance (Component* instance);
	void ChangeParentInstance (Component* instance, Component* oldParent);
	void ChangeFocusedInstance (Component* instance, Component* oldFocused);

	void AddSignal (const std::string& compId, const std::string& sigId);
	void RemoveSignal (const std::string& compId, const std::string& sigId);

	void UpdateHandler (const std::string& compId, const ComponentFuncEntry& entry);
	void UpdateDynamicHandler (Component* comp, const ComponentFuncEntry& entry);

	//******************************************************************

	void SetActiveColor (wxColour color);
	void SetFormattedSummary (const String& desc);

protected:
	void AddChildInstance (Component* instance);
	void RemoveChildInstance (Component* instance, Component* oldParent);

	void FocusInstance (Component* instance, bool focus);
	void FocusInstanceItem (wxTreeItemId treeId, bool focus);

	void OnDoubleClickSash (int x, int y);

private:
	typedef boost::function<bool (TreeData*)>	TreeItemPred;
	typedef std::list<wxTreeItemId>				TreeIdList;

	DECLARE_EVENT_TABLE();

	void onItemSelected (wxTreeEvent& event);
	void onContextMenu (wxTreeEvent& event);
	void onCommand (wxCommandEvent& event);

	wxTreeItemId getPositionedEntry (const std::string& id, uint pos);
	wxTreeItemId getFunctionEntry (const std::string& id);
	wxTreeItemId getHandlerEntry (const std::string& id);
	wxTreeItemId getInstanceEntry (const std::string& id);
	wxTreeItemId getSignalEntry (const std::string& id);

	wxTreeItemId findComponent (const std::string& id);
	wxTreeItemId findFunction (const std::string& compClassId, const std::string& id, bool isHandler, bool isStatic);
	wxTreeItemId findDynamicFunction (Component* comp, const std::string& id, bool isHandler);
	wxTreeItemId findInstance (const std::string& compClassId, uint serial);
	wxTreeItemId findDynamicFunctionOrHandlerInstance (Component* comp, bool isHandler);
	wxTreeItemId findSignal (const std::string& compClassId, const std::string& id);

	void findAllInstances (
			TreeIdList&			ids,
			const TreeItemPred& pred,
			bool				prune = true,
			bool				addTopLevel = true
		);
	void findAllInstancesHelper (
			wxTreeItemId		root,
			TreeIdList&			ids,
			const TreeItemPred& pred,
			bool				prune,
			bool				addTopLevel
		);

	void erase (wxTreeItemId item);
	void itemViewNeedsUpdate (bool value);
	void connectSignals (void);

	wxColour		m_activeColor;
	wxTreeCtrl*		m_hierarchy;
	HTMLWindowExt*	m_summary;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTSPYWINDOW_H

/**
 *	ComponentSpyWindow.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentSpyWindow.h"
#include "SpyWindowTreeData.h"
#include "ComponentSignalRegistry.h"
#include "ComponentRegistry.h"
#include "HTMLExt.h"

#include "CommonGUIUtils.h"
#include "Algorithms.h"
#include "Predicates.h"

#include <wx/treectrl.h>
#include <wx/imaglist.h>
#include <wx/menu.h>
#include <wx/sizer.h>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "Icons/component_16.xpm"
#include "Icons/function_16.xpm"
#include "Icons/instance_16.xpm"
#include "Icons/group_16.xpm"
#include "Icons/signal_16.xpm"

namespace ide {

////////////////////////////////////////////////////////////////////////
// Predicates
//
struct CompPred {
	CompPred (const std::string& id, uint serial) : m_id(id), m_serial(serial) {}
	bool operator() (TreeData* data) const
		{ return data->IsInstanceClass(m_id) && data->IsInstance(m_serial); }
private:
	const std::string&	m_id;
	uint				m_serial;
};

////////////////////////////////////////////////////////////////////////
// ComponentSpyWindow class
//
BEGIN_EVENT_TABLE(ComponentSpyWindow, wxSplitterWindow)
	EVT_TREE_SEL_CHANGED(wxID_ANY, ComponentSpyWindow::onItemSelected)
	EVT_TREE_ITEM_MENU(wxID_ANY, ComponentSpyWindow::onContextMenu)
	EVT_MENU(wxID_ANY, ComponentSpyWindow::onCommand)
END_EVENT_TABLE();

//**********************************************************************

ComponentSpyWindow::ComponentSpyWindow (void) :
	m_activeColor(0, 0, 160)
{
}

//**********************************************************************

ComponentSpyWindow::ComponentSpyWindow (wxWindow* parent)
{
	this->Create(parent);
}

//**********************************************************************

ComponentSpyWindow::~ComponentSpyWindow (void)
{
}

//**********************************************************************

bool ComponentSpyWindow::Create (wxWindow* parent)
{
	if (
		!this->wxSplitterWindow::Create(
			parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE)
	) {
		return false;
	}
	m_hierarchy = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT | wxTR_ROW_LINES | wxTR_SINGLE);
	m_hierarchy->AddRoot(String());

	m_summary = new HTMLWindowExt(this);

	this->SetMinimumPaneSize(100);
	this->SetSashGravity(0.9);

	GUI_SCOPED_FREEZE(m_hierarchy);

	util::for_all(ComponentRegistry::Instance().GetComponentMap(),
		boost::bind(&ComponentSpyWindow::AddComponent, this, _1));

	this->connectSignals();

	////////////////////////////////////////////////////////////////////
	// Create & assign image list
	//
	wxImageList* imageList = new wxImageList(16, 16);

	imageList->Add(component_16);
	imageList->Add(function_16);
	imageList->Add(instance_16);
	imageList->Add(group_16);
	imageList->Add(signal_16);

	m_hierarchy->AssignImageList(imageList);

	return true;
}

//**********************************************************************

void ComponentSpyWindow::AddComponent (const ComponentEntry& entry)
{
	GUI_SCOPED_FREEZE(m_hierarchy);

	const std::string& id = entry.GetClassId();

	wxTreeItemId treeId = m_hierarchy->AppendItem(
		m_hierarchy->GetRootItem(), util::std2str(id), 0, -1, new ComponentTreeData(id)
	);
	wxTreeItemId functions = m_hierarchy->AppendItem(treeId, _("Functions"), 3, -1, new AllFunctionsTreeData(id));
	m_hierarchy->AppendItem(functions, _("Member Functions"), 3, -1, new MemberFunctionsTreeData(id));
	m_hierarchy->AppendItem(functions, _("Static Functions"), 3, -1, new StaticFunctionsTreeData(id));
	m_hierarchy->AppendItem(functions, _("Dynamic Functions"), 3, -1, new DynamicFunctionsTreeData(id));
	wxTreeItemId handlers = m_hierarchy->AppendItem(treeId, _("Handlers"), 3, -1, new AllHandlersTreeData(id));
	m_hierarchy->AppendItem(handlers, _("Member Handlers"), 3, -1, new MemberHandlersTreeData(id));
	m_hierarchy->AppendItem(handlers, _("Static Handlers"), 3, -1, new StaticHandlersTreeData(id));
	m_hierarchy->AppendItem(handlers, _("Dynamic Handlers"), 3, -1, new DynamicHandlersTreeData(id));
	m_hierarchy->AppendItem(treeId, _("Instances"), 3, -1, new AllInstancesTreeData(id));
	wxTreeItemId signals = m_hierarchy->AppendItem(treeId, _("Signals"), 3, -1, new AllSignalsTreeData(id));
	m_hierarchy->AppendItem(signals, _("Member Signals"), 3, -1, new MemberSignalsTreeData(id));
	m_hierarchy->AppendItem(signals, _("Static Signals"), 3, -1, new StaticSignalsTreeData(id));

	util::for_all(entry.GetFunctionMap(),
		boost::bind(&ComponentSpyWindow::AddFunctionOrHandler, this, id, _1));

	util::for_all(entry.GetInstances(),
		boost::bind(&ComponentSpyWindow::AddInstance, this, _1));

	util::for_all(entry.GetSignalList(),
		boost::bind(&ComponentSpyWindow::AddSignal, this, id, _1));
}

//**********************************************************************

void ComponentSpyWindow::RemoveComponent (const std::string& id)
	{ this->erase(this->findComponent(id)); }

//**********************************************************************

void ComponentSpyWindow::AddFunctionOrHandler (const std::string& compId, const ComponentFuncEntry& entry)
{
	wxTreeItemId funcTreeId = entry.IsSlot() ? getHandlerEntry(compId) : getFunctionEntry(compId);
	const String label(util::std2str(entry.GetSignature()));
	TreeData *data = entry.IsSlot() ?	(TreeData *) (new HandlerTreeData(compId, entry.GetId())) :
										(TreeData *) (new FunctionTreeData(compId, entry.GetId()));
	wxTreeItemIdValue cookie;
	funcTreeId = m_hierarchy->GetFirstChild(funcTreeId, cookie);
	if (entry.IsStaticFunc())
		funcTreeId = m_hierarchy->GetNextChild(funcTreeId, cookie);
	if (funcTreeId.IsOk())
		m_hierarchy->AppendItem(funcTreeId, label, 1, -1, data);
}

//**********************************************************************

void ComponentSpyWindow::RemoveFunctionOrHandler (const std::string& compId, const std::string& id, bool isHandler, bool isStatic)
	{ this->erase(this->findFunction(compId, id, isHandler, isStatic)); }

//**********************************************************************

void ComponentSpyWindow::AddDynamicFunctionOrHandler (Component* comp, const ComponentFuncEntry& entry)
{
	const std::string compId = comp->GetClassId();
	uint serial = comp->GetSerial();

	wxTreeItemId funcTreeId = entry.IsSlot() ? getHandlerEntry(compId) : getFunctionEntry(compId);
	const String label(util::std2str(entry.GetSignature()));
	TreeData *data = entry.IsSlot() ?	(TreeData *) (new DynamicHandlerTreeData(comp, entry.GetId())) :
										(TreeData *) (new DynamicFunctionTreeData(comp, entry.GetId()));
	wxTreeItemIdValue cookie;
	funcTreeId = m_hierarchy->GetFirstChild(funcTreeId, cookie);
	funcTreeId = m_hierarchy->GetNextChild(funcTreeId, cookie);
	wxTreeItemId dynamic = m_hierarchy->GetNextChild(funcTreeId, cookie);

	wxTreeItemId item = m_hierarchy->GetFirstChild(dynamic, cookie);
	while (item.IsOk()) {
		TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
		if (data->IsInstance(serial))
			break;
		item = m_hierarchy->GetNextChild(dynamic, cookie);
	} 
	if (!item.IsOk()) {
		const String label(util::std2str(compId) + _T("#") + boost::lexical_cast<String>(serial));
		item = m_hierarchy->AppendItem(dynamic, label, 2, -1, new InstanceTreeData(compId, serial));
	}
	m_hierarchy->AppendItem(item, label, 1, -1, data);
}

//**********************************************************************

void ComponentSpyWindow::RemoveDynamicFunctionOrHandler (Component* comp, const std::string& id, bool isHandler)
	{ this->erase(this->findDynamicFunction(comp, id, isHandler)); }

//**********************************************************************

void ComponentSpyWindow::AddInstance (Component* instance)
{
	GUI_SCOPED_FREEZE(m_hierarchy);
	wxTreeItemId instTreeId = getInstanceEntry(instance->GetClassId());
	if (!instTreeId.IsOk())
		return;

	const String serial = boost::lexical_cast<String>(instance->GetSerial());
	const String label(util::std2str(instance->GetClassId()) + _T("#") + serial);

	wxTreeItemId treeId = m_hierarchy->AppendItem(instTreeId, label, 2, -1,
		new InstanceTreeData(instance->GetClassId(), instance->GetSerial()));

	if (instance->IsFocused())
		this->FocusInstanceItem(treeId, true);

	this->AddChildInstance(instance);
}

//**********************************************************************

void ComponentSpyWindow::RemoveInstance (Component* instance)
{
	GUI_SCOPED_FREEZE(m_hierarchy);

	this->erase(this->findInstance(instance->GetClassId(), instance->GetSerial()));
	this->RemoveChildInstance(instance, instance->GetParent());
	wxTreeItemId dynamicFunctionInstance = this->findDynamicFunctionOrHandlerInstance(instance, false);
	if (dynamicFunctionInstance.IsOk())
		this->erase(dynamicFunctionInstance);
	wxTreeItemId dynamicHandlerInstance = this->findDynamicFunctionOrHandlerInstance(instance, true);
	if (dynamicHandlerInstance.IsOk())
		this->erase(dynamicHandlerInstance);
}

//**********************************************************************

void ComponentSpyWindow::ChangeParentInstance (Component* instance, Component* oldParent)
{
	GUI_SCOPED_FREEZE(m_hierarchy);

	this->RemoveChildInstance(instance, oldParent);
	this->AddChildInstance(instance);
}

//**********************************************************************

void ComponentSpyWindow::ChangeFocusedInstance (Component* instance, Component* oldFocused)
{
	GUI_SCOPED_FREEZE(m_hierarchy);

	this->FocusInstance(oldFocused, false);
	this->FocusInstance(instance, true);
}

//**********************************************************************

void ComponentSpyWindow::AddSignal (const std::string& compId, const std::string& sigId)
{
	const ComponentSignal& sig = ComponentSignalRegistry::Instance().GetSignal(sigId);
	wxTreeItemId sigTreeId = getSignalEntry(compId);
	if (!sigTreeId.IsOk())
		return;

	wxTreeItemIdValue cookie;
	sigTreeId = m_hierarchy->GetFirstChild(sigTreeId, cookie);
	if (sig.IsStatic())
		sigTreeId = m_hierarchy->GetNextChild(sigTreeId, cookie);

	const String label(util::std2str(sig.GetSignature()));
	wxTreeItemId treeId = m_hierarchy->AppendItem(sigTreeId, label, 4, -1,
		new SignalTreeData(compId, sigId));
}

//**********************************************************************

void ComponentSpyWindow::RemoveSignal (const std::string& compId, const std::string& id)
	{ this->erase(this->findSignal(compId, id)); }

//**********************************************************************

void ComponentSpyWindow::UpdateHandler (const std::string& compId, const ComponentFuncEntry& entry)
{
	assert(entry.IsSlot());
	wxTreeItemId handler = findFunction(compId, entry.GetId(), true, entry.IsStaticFunc());
	if (handler.IsOk())
		m_hierarchy->SetItemText(handler, util::std2str(entry.GetSignature()));
}

//**********************************************************************

void ComponentSpyWindow::UpdateDynamicHandler (Component* comp, const ComponentFuncEntry& entry)
{
	assert(entry.IsSlot() && entry.IsMemberFunc());
	wxTreeItemId handler = findDynamicFunction(comp, entry.GetId(), true);
	if (handler.IsOk())
		m_hierarchy->SetItemText(handler, util::std2str(entry.GetSignature()));
}

//**********************************************************************

void ComponentSpyWindow::SetActiveColor (wxColour color)
{
	GUI_SCOPED_FREEZE(m_hierarchy);

	m_activeColor = color;
	TreeIdList ids;
	this->findAllInstances(ids, util::const_pred<TreeData*, true>(), false);
	for (TreeIdList::iterator i = ids.begin(); i != ids.end(); ++i) {
		if (m_hierarchy->GetItemTextColour(*i) != wxNullColour)
			m_hierarchy->SetItemTextColour(*i, color);
	}
}

//**********************************************************************

void ComponentSpyWindow::SetFormattedSummary (const String& desc)
{
	m_summary->SetPage(
		HTMLWindowExt::GetBodyStart(_("Summary")) + desc + HTMLWindowExt::GetBodyEnd()
	);
}

//**********************************************************************

void ComponentSpyWindow::AddChildInstance (Component* instance)
{
	if (Component* parent = instance->GetParent()) {

		const String serial = boost::lexical_cast<String>(instance->GetSerial());
		const String label(util::std2str(instance->GetClassId()) + _T("#") + serial);

		TreeIdList ids;
		this->findAllInstances(ids, CompPred(parent->GetClassId(), parent->GetSerial()));
		for (TreeIdList::iterator i = ids.begin(); i != ids.end(); ++i) {

			wxTreeItemId treeId = m_hierarchy->AppendItem(*i, label, 2, -1,
				new InstanceTreeData(instance->GetClassId(), instance->GetSerial()));

			if (instance->IsFocused())
				this->FocusInstanceItem(treeId, true);
		}
	}
	util::for_all(instance->GetChildren(),
		boost::bind(&ComponentSpyWindow::AddChildInstance, this, _1));
}

//**********************************************************************

void ComponentSpyWindow::RemoveChildInstance (Component* instance, Component* oldParent)
{
	if (oldParent) {
		TreeIdList ids;
		CompPred pred(instance->GetClassId(), instance->GetSerial());
		this->findAllInstances(ids, pred, true, false);
		util::for_all(ids, boost::bind(&ComponentSpyWindow::erase, this, _1));
	}
}

//**********************************************************************

void ComponentSpyWindow::FocusInstance (Component* instance, bool focus)
{
	if (instance) {
		TreeIdList ids;
		this->findAllInstances(ids, CompPred(instance->GetClassId(), instance->GetSerial()));
		util::for_all(ids, boost::bind(&ComponentSpyWindow::FocusInstanceItem, this, _1, focus));
	}
}

//**********************************************************************

void ComponentSpyWindow::FocusInstanceItem (wxTreeItemId treeId, bool focus)
{
	m_hierarchy->SetItemTextColour(treeId, focus ? m_activeColor : wxNullColour);
}

//**********************************************************************

void ComponentSpyWindow::OnDoubleClickSash (int x, int y)
{
	if (this->IsSplit()) {

		int mode = this->GetSplitMode();
		this->Unsplit();
		if (mode ==  wxSPLIT_VERTICAL)
			this->SplitHorizontally(m_hierarchy, m_summary);
		else
			this->SplitVertically(m_hierarchy, m_summary);
	}
}

//**********************************************************************

void ComponentSpyWindow::onItemSelected (wxTreeEvent& event)
{
	if (!this->IsSplit()) // eoeo
		this->SplitVertically(m_hierarchy, m_summary);

	TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(event.GetItem()));
	this->SetFormattedSummary(data->GetFormattedSummary());
}

//**********************************************************************

void ComponentSpyWindow::onContextMenu (wxTreeEvent& event)
{
	m_hierarchy->SelectItem(event.GetItem());
	wxTreeItemId item = m_hierarchy->GetSelection();

	TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
	if (wxMenu* menu = data->GetMenu())
		m_hierarchy->PopupMenu(menu);	
}

//**********************************************************************

void ComponentSpyWindow::onCommand (wxCommandEvent& event)
{
	wxTreeItemId item = m_hierarchy->GetSelection();
	if (item.IsOk()) {
		TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
		assert(data);

		//data->DoAction(static_cast<SWCommandType>(event.GetId()));

		data->PostAction(static_cast<SWCommandType>(event.GetId()),
			boost::bind(&ComponentSpyWindow::itemViewNeedsUpdate, this, _1));
	}
}

//**********************************************************************

wxTreeItemId ComponentSpyWindow::getPositionedEntry (const std::string& id, uint pos)
{
	wxTreeItemId compTreeId = this->findComponent(id);
	if (!compTreeId.IsOk())
		return wxTreeItemId();
	wxTreeItemIdValue cookie;
	wxTreeItemId treeId = m_hierarchy->GetFirstChild(compTreeId, cookie);
	for (uint i = 0; i < pos; ++i)
		treeId = m_hierarchy->GetNextChild(treeId, cookie);
	return treeId;
}

wxTreeItemId ComponentSpyWindow::getFunctionEntry (const std::string& id)
	{ return getPositionedEntry(id, 0); }

wxTreeItemId ComponentSpyWindow::getHandlerEntry (const std::string& id)
	{ return getPositionedEntry(id, 1); }

wxTreeItemId ComponentSpyWindow::getInstanceEntry (const std::string& id)
	{ return getPositionedEntry(id, 2); }

wxTreeItemId ComponentSpyWindow::getSignalEntry (const std::string& id)
	{ return getPositionedEntry(id, 3); }

//**********************************************************************

wxTreeItemId ComponentSpyWindow::findComponent (const std::string& id)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId root = m_hierarchy->GetRootItem();
	wxTreeItemId item = m_hierarchy->GetFirstChild(root, cookie); 

	while (item.IsOk()) {
		TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
		if (data->IsComponent(id))
			return item;
		item = m_hierarchy->GetNextChild(root, cookie);
	} 
	return wxTreeItemId();
}

//**********************************************************************

wxTreeItemId ComponentSpyWindow::findFunction (
		const std::string& compClassId, const std::string& id, bool isHandler, bool isStatic
	)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId root = isHandler ? getHandlerEntry(compClassId) : getFunctionEntry(compClassId);
	root = m_hierarchy->GetFirstChild(root, cookie);
	if (isStatic)
		root = m_hierarchy->GetNextChild(root, cookie);
	if (!root.IsOk())
		return wxTreeItemId();

	wxTreeItemId item = m_hierarchy->GetFirstChild(root, cookie);
	while (item.IsOk()) {
		TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
		if (isHandler && data->IsHandler(id) || !isHandler && data->IsFunction(id))
			return item;
		item = m_hierarchy->GetNextChild(root, cookie);
	} 
	return wxTreeItemId();
}

//**********************************************************************

wxTreeItemId ComponentSpyWindow::findDynamicFunction (Component* comp, const std::string& id, bool isHandler)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId funcTreeId = isHandler ? getHandlerEntry(comp->GetClassId()) : getFunctionEntry(comp->GetClassId());
	funcTreeId = m_hierarchy->GetFirstChild(funcTreeId, cookie);
	funcTreeId = m_hierarchy->GetNextChild(funcTreeId, cookie);
	wxTreeItemId dynamic = m_hierarchy->GetNextChild(funcTreeId, cookie);

	wxTreeItemId item = m_hierarchy->GetFirstChild(dynamic, cookie);
	while (item.IsOk()) {
		TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
		if (data->IsInstance(comp->GetSerial()))
			break;
		item = m_hierarchy->GetNextChild(dynamic, cookie);
	} 
	if (!item.IsOk())
		return wxTreeItemId();
	
	item = m_hierarchy->GetFirstChild(item, cookie);
	while (item.IsOk()) {
		TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
		if (isHandler && data->IsHandler(id) || !isHandler && data->IsFunction(id))
			return item;
		item = m_hierarchy->GetNextChild(dynamic, cookie);
	} 
	return wxTreeItemId();
}

//**********************************************************************

wxTreeItemId ComponentSpyWindow::findInstance (const std::string& compClassId, uint serial)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId root = getInstanceEntry(compClassId);
	if (!root.IsOk())
		return wxTreeItemId();

	wxTreeItemId item = m_hierarchy->GetFirstChild(root, cookie);
	while (item.IsOk()) {
		TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
		if (data->IsInstance(serial))
			return item;
		item = m_hierarchy->GetNextChild(root, cookie);
	} 
	return wxTreeItemId();
}

//**********************************************************************

wxTreeItemId ComponentSpyWindow::findDynamicFunctionOrHandlerInstance (Component* comp, bool isHandler)
{
	const std::string compId = comp->GetClassId();
	uint serial = comp->GetSerial();
	wxTreeItemIdValue cookie;
	wxTreeItemId funcTreeId = isHandler ? getHandlerEntry(compId) : getFunctionEntry(compId);
	funcTreeId = m_hierarchy->GetFirstChild(funcTreeId, cookie);
	funcTreeId = m_hierarchy->GetNextChild(funcTreeId, cookie);
	wxTreeItemId dynamic = m_hierarchy->GetNextChild(funcTreeId, cookie);

	wxTreeItemId item = m_hierarchy->GetFirstChild(dynamic, cookie);
	while (item.IsOk()) {
		TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
		if (data->IsInstance(serial))
			return item;
		item = m_hierarchy->GetNextChild(dynamic, cookie);
	}
	return wxTreeItemId();
}

//**********************************************************************

wxTreeItemId ComponentSpyWindow::findSignal (const std::string& compClassId, const std::string& id)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId root = getSignalEntry(compClassId);
	if (!root.IsOk())
		return wxTreeItemId();

	root = m_hierarchy->GetFirstChild(root, cookie);
	const ComponentSignal& sig = ComponentSignalRegistry::Instance().GetSignal(id);
	if (sig.IsStatic())	
		root = m_hierarchy->GetNextChild(root, cookie);
	wxTreeItemId item = m_hierarchy->GetFirstChild(root, cookie);

	while (item.IsOk()) {
		TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(item));
		if (data->IsSignal(id))
			return item;
		item = m_hierarchy->GetNextChild(root, cookie);
	} 
	return wxTreeItemId();
}

//**********************************************************************

void ComponentSpyWindow::findAllInstances (
		TreeIdList&			ids,
		const TreeItemPred& pred,
		bool				prune,
		bool				addTopLevel
	)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId root = m_hierarchy->GetRootItem();
	wxTreeItemId componentTreeId = m_hierarchy->GetFirstChild(root, cookie);

	while (componentTreeId.IsOk()) {

		wxTreeItemIdValue cookie2;
		m_hierarchy->GetFirstChild(componentTreeId, cookie2);	//Functions
		m_hierarchy->GetNextChild(componentTreeId, cookie2);	//Handlers
		wxTreeItemId instRootTreeId = m_hierarchy->GetNextChild(componentTreeId, cookie2);	//Instances
		wxTreeItemId instTreeId = m_hierarchy->GetFirstChild(instRootTreeId, cookie2);

		while (instTreeId.IsOk()) {
			this->findAllInstancesHelper(instTreeId, ids, pred, prune, addTopLevel);
			instTreeId = m_hierarchy->GetNextChild(instRootTreeId, cookie2);
		}
		componentTreeId = m_hierarchy->GetNextChild(root, cookie);
	}
}

//**********************************************************************

void ComponentSpyWindow::findAllInstancesHelper (
		wxTreeItemId		root,
		TreeIdList&			ids,
		const TreeItemPred& pred,
		bool				prune,
		bool				addTopLevel
	)
{
	assert(root.IsOk());

	if (pred(static_cast<TreeData*>(m_hierarchy->GetItemData(root)))) {
		if (addTopLevel)
			ids.push_back(root);
		if (prune)
			return;
	}
	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_hierarchy->GetFirstChild(root, cookie);

	while (item.IsOk()) {
		this->findAllInstancesHelper(item, ids, pred, prune, true);
		item = m_hierarchy->GetNextChild(root, cookie);
	}
}

//**********************************************************************

void ComponentSpyWindow::erase (wxTreeItemId item)
{
	if (item.IsOk())
		m_hierarchy->Delete(item);
}

//**********************************************************************

void ComponentSpyWindow::itemViewNeedsUpdate (bool value)
{
	if (value) {
		wxTreeItemId selection = m_hierarchy->GetSelection();
		if (selection.IsOk()) {
			TreeData* data = static_cast<TreeData*>(m_hierarchy->GetItemData(selection));
			this->SetFormattedSummary(data->GetFormattedSummary());
		}
	}
}

//**********************************************************************

void ComponentSpyWindow::connectSignals (void)
{
	ComponentRegistry::Instance().sigRegisteredComponent.connect(
		boost::bind(&ComponentSpyWindow::AddComponent, this, _1)
	);
	ComponentRegistry::Instance().sigUnregisteredComponent.connect(
		boost::bind(&ComponentSpyWindow::RemoveComponent, this, _1)
	);
	ComponentRegistry::Instance().sigAddedFunction.connect(
		boost::bind(&ComponentSpyWindow::AddFunctionOrHandler, this, _1, _2)
	);
	ComponentRegistry::Instance().sigRemovedFunction.connect(
		boost::bind(&ComponentSpyWindow::RemoveFunctionOrHandler, this, _1, _2, _3, _4)
	);
	ComponentRegistry::Instance().sigAddedDynamicFunction.connect(
		boost::bind(&ComponentSpyWindow::AddDynamicFunctionOrHandler, this, _1, _2)
	);
	ComponentRegistry::Instance().sigRemovedDynamicFunction.connect(
		boost::bind(&ComponentSpyWindow::RemoveDynamicFunctionOrHandler, this, _1, _2, _3)
	);
	ComponentRegistry::Instance().sigCreatedInstance.connect(
		boost::bind(&ComponentSpyWindow::AddInstance, this, _1)
	);
	ComponentRegistry::Instance().sigDestroyedInstance.connect(
		boost::bind(&ComponentSpyWindow::RemoveInstance, this, _1)
	);
	ComponentRegistry::Instance().sigChangedParentInstance.connect(
		boost::bind(&ComponentSpyWindow::ChangeParentInstance, this, _1, _2)
	);
	ComponentRegistry::Instance().sigChangedFocusedInstance.connect(
		boost::bind(&ComponentSpyWindow::ChangeFocusedInstance, this, _1, _2)
	);
	ComponentRegistry::Instance().sigAddedSignal.connect(
		boost::bind(&ComponentSpyWindow::AddSignal, this, _1, _2)
	);
	ComponentRegistry::Instance().sigRemovedSignal.connect(
		boost::bind(&ComponentSpyWindow::RemoveSignal, this, _1, _2)
	);
	ComponentRegistry::Instance().sigSlotUpdated.connect(
		boost::bind(&ComponentSpyWindow::UpdateHandler, this, _1, _2)
	);
	ComponentRegistry::Instance().sigDynamicSlotUpdated.connect(
		boost::bind(&ComponentSpyWindow::UpdateDynamicHandler, this, _1, _2)
	);
}

////////////////////////////////////////////////////////////////////////

} // namespace ide

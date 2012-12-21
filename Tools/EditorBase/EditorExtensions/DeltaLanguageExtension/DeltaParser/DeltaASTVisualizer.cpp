/**
 *	DeltaASTVisualizer.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaASTVisualizer.h"
#include "Streams.h"
#include "DeltaAST.h"

#include <wx/imaglist.h>
#include <wx/menu.h>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/functional.hpp>

namespace delta {

////////////////////////////////////////////////////////////////////////
// ItemData class
//
class ItemData : public wxTreeItemData {
public:
	ItemData (DeltaASTNode* node) : m_node(node) {}

	DeltaASTNode* GetNode (void) const { return m_node; }

	void Call (const ASTreeCtrl::NodeFunc& func, int command) {
		assert(func);
		func(m_node, command);
	}

	inline static ItemData* Get (ASTreeCtrl* tree, wxTreeItemId item)
		{ return static_cast<ItemData*>(tree->GetItemData(item)); }

	inline static ItemData* GetSafe (ASTreeCtrl* tree, wxTreeItemId id)
	{
		ItemData* result = Get(tree, id);
		assert(result);
		return result;
	}

private:
	DeltaASTNode* m_node;
};

////////////////////////////////////////////////////////////////////////
// ASTreeCtrl class
//
ASTreeCtrl::ASTreeCtrl (wxWindow* parent, ProgramDescription* descr, const NodeFunc& func) :
	wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_ROW_LINES | wxTR_SINGLE),
	ProgramDescription::View(descr),
	m_nodeFunc(func)
{
	this->AddRoot(_("Program"));
	this->Fill(pm_progDesc->GetAST());

	pm_progDesc->AddView(this);
}

//**********************************************************************

ASTreeCtrl::~ASTreeCtrl (void)
{
	pm_progDesc->RemoveView(this);
}

//**********************************************************************

BEGIN_EVENT_TABLE(ASTreeCtrl, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(wxID_ANY, ASTreeCtrl::onItemSelected)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, ASTreeCtrl::onItemActivated)
	EVT_TREE_ITEM_GETTOOLTIP(wxID_ANY, ASTreeCtrl::onHover)
	EVT_TREE_ITEM_MENU(wxID_ANY, ASTreeCtrl::onCtxMenu)
	EVT_MENU(wxID_ANY, ASTreeCtrl::onCommand)
END_EVENT_TABLE();

//**********************************************************************

void ASTreeCtrl::AddCommand (int id, const String& name, const String& helpString)
{
	if (!s_menu)
		s_menu = new wxMenu;
	s_menu->Append(id, name, helpString);
}

//**********************************************************************

void ASTreeCtrl::ClearCommands (void)
{
	delete s_menu;
	s_menu = 0;
}

//**********************************************************************

void ASTreeCtrl::ExpandRoot (void)
{
	this->Expand(this->GetRootItem());

	wxTreeItemId treeId = this->GetLastChild(this->GetRootItem());
	if (treeId.IsOk())
		this->Expand(treeId);
}

//**********************************************************************

void ASTreeCtrl::Focus (DeltaASTNode* node)
{
	if (node) {
		wxTreeItemId item = this->findNode(node, this->GetRootItem());
		this->SelectItem(item);
	}
}

//**********************************************************************

void ASTreeCtrl::OnSetAST (StmtsASTNode* ast)
{
	this->Clear();
	this->Fill(ast);
}

//**********************************************************************

void ASTreeCtrl::OnMerge (
		StmtsASTNode*			parent,
		StmtsASTNode::Iterator	end,
		StmtsASTNode*			src
	)
{
	wxTreeItemId root = this->findNode(parent, this->GetRootItem());

	if (!root.IsOk())
		return;

	wxTreeItemId item;
	size_t count = std::distance(parent->Begin(), end);

	if (count != 0) {

		TreeItemList removedNodes;
		wxTreeItemIdValue cookie;
		item = this->GetFirstChild(root, cookie);

		while (--count) {
			item = this->GetNextChild(root, cookie);
			assert(item.IsOk());
		}
	}
	DeltaASTVisualizer visualizer(this, root, item);
	std::for_each(src->Begin(), src->End(),
		std::bind2nd(std::mem_fun(&DeltaASTNode::Accept), &visualizer));
}

//**********************************************************************

void ASTreeCtrl::OnRemove (StmtsASTNode* parent, const Range& range)
{
	wxTreeItemId root = this->findNode(parent, this->GetRootItem());

	if (!root.IsOk())
		return;

	TreeItemList removedNodes;
	wxTreeItemIdValue cookie;
	wxTreeItemId item = this->GetFirstChild(root, cookie);

	while (item.IsOk()) {
		ItemData* pos = ItemData::GetSafe(this, item);
		if (range.Contains(pos->GetNode()->GetRange()))
			removedNodes.push_back(item);
		item = this->GetNextChild(root, cookie);
	}
	std::for_each(removedNodes.rbegin(), removedNodes.rend(),
		boost::bind1st(std::mem_fun(&ASTreeCtrl::Delete), this));
}

//**********************************************************************

void ASTreeCtrl::OnRemove (DeltaASTNode* node)
{
	wxTreeItemId nodeId = this->findNode(node, this->GetRootItem());
	if (nodeId.IsOk())
		this->Delete(nodeId);
}

//**********************************************************************

void ASTreeCtrl::OnRefreshChildren (DeltaASTNode* node)
{
	wxTreeItemId root = this->findNode(node, this->GetRootItem());

	if (!root.IsOk())
		return;

	TreeItemList removedNodes;
	wxTreeItemIdValue cookie;
	wxTreeItemId item = this->GetFirstChild(root, cookie);

	while (item.IsOk()) {
		ItemData* pos = ItemData::GetSafe(this, item);
		removedNodes.push_back(item);
		item = this->GetNextChild(root, cookie);
	}
	std::for_each(removedNodes.rbegin(), removedNodes.rend(),
		boost::bind1st(std::mem_fun(&ASTreeCtrl::Delete), this));

	DeltaASTNodeList children;
	node->GetChildren(children);
	BOOST_FOREACH(DeltaASTNode* n, children)
	{
		DeltaASTVisualizer visualizer(this, root, item);
		n->Accept(&visualizer);
		item = visualizer.m_previousItem;
	}
//	std::for_each(children.rbegin(), children.rend(),
//		std::bind2nd(std::mem_fun(&DeltaASTNode::Accept), &visualizer));
}

//**********************************************************************

void ASTreeCtrl::Fill (StmtsASTNode* root)
{
	if (root) {
		DeltaASTVisualizer visualizer(this);
		root->Accept(&visualizer);
	}
}

//**********************************************************************

void ASTreeCtrl::Clear (void)
{
	this->DeleteChildren(this->GetRootItem());
}

//**********************************************************************

void ASTreeCtrl::onItemSelected (wxTreeEvent& event)
{
	if (ItemData* pos = ItemData::Get(this, event.GetItem()))
		pos->Call(m_nodeFunc, -1);
}

//**********************************************************************

void ASTreeCtrl::onItemActivated (wxTreeEvent& event)
{
	if (ItemData* pos = ItemData::Get(this, event.GetItem()))
		pos->Call(m_nodeFunc, -2);
}

//**********************************************************************

void ASTreeCtrl::onHover (wxTreeEvent& event)
{
	if (ItemData* pos = ItemData::Get(this, event.GetItem()))
		pos->Call(m_nodeFunc, -3);
}

//**********************************************************************

void ASTreeCtrl::onCtxMenu (wxTreeEvent& event)
{
	this->SelectItem(event.GetItem());
	if (s_menu)
		this->PopupMenu(s_menu);
}

//**********************************************************************

void ASTreeCtrl::onCommand (wxCommandEvent& event)
{
	if (ItemData* pos = ItemData::Get(this, this->GetSelection()))
		pos->Call(m_nodeFunc, event.GetId());
}

//**********************************************************************

wxTreeItemId ASTreeCtrl::findNode (DeltaASTNode* node, wxTreeItemId root)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId item = this->GetFirstChild(root, cookie);

	while (item.IsOk()) {

		ItemData* pos = ItemData::GetSafe(this, item);
		if (pos->GetNode()->GetRange().Contains(node->GetRange())) {
			if (pos->GetNode() == node)
				return item;
			return findNode(node, item);
		}
		item = this->GetNextChild(root, cookie);
	}
	return item;
}

//**********************************************************************

wxMenu* ASTreeCtrl::s_menu = 0;

////////////////////////////////////////////////////////////////////////
// DeltaASTVisualizer class
//
DeltaASTVisualizer::DeltaASTVisualizer (ASTreeCtrl* visTree) :
	m_tree(visTree)
{
	m_itemList.push_back(m_tree->GetRootItem());
}

//**********************************************************************

DeltaASTVisualizer::DeltaASTVisualizer (ASTreeCtrl* visTree, wxTreeItemId root, wxTreeItemId previous) :
	m_tree(visTree), m_previousItem(previous)
{
	m_itemList.push_back(root);
}

//**********************************************************************

DeltaASTVisualizer::~DeltaASTVisualizer (void)
{

}

//**********************************************************************

#define IMPLEMENT_DEFAULT_VISIT_ENTER(type, str)			\
	bool DeltaASTVisualizer::VisitEnter (type* node)		\
		{ this->pushItem(node, _(str)); return true; }

#define IMPLEMENT_DEFAULT_VISIT_LEAVE(type)					\
	bool DeltaASTVisualizer::VisitLeave (type* node)		\
		{ this->popItem(); return true; }

#define IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(type, str)	\
	IMPLEMENT_DEFAULT_VISIT_ENTER(type, str)				\
	IMPLEMENT_DEFAULT_VISIT_LEAVE(type)

#define IMPLEMENT_VISIT_ENTER_WITH_VALUE(type, str)			\
	bool DeltaASTVisualizer::VisitEnter (type* node)		\
		{ this->pushItem(node, _(str), util::std2str(node->GetValue())); return true; }

#define IMPLEMENT_VISIT_ENTER_AND_LEAVE_WITH_VALUE(type, str)	\
	IMPLEMENT_VISIT_ENTER_WITH_VALUE(type, str)					\
	IMPLEMENT_DEFAULT_VISIT_LEAVE(type)

#define IMPLEMENT_DEFAULT_VISIT(type, str)					\
	bool DeltaASTVisualizer::Visit (type* node)				\
	{ this->addItem(node, _(str)); return true; }

#define IMPLEMENT_VISIT_WITH_VALUE(type, str)				\
	bool DeltaASTVisualizer::Visit (type* node)				\
		{ this->addItem(node, _(str), util::std2str(node->GetValue())); return true; }

//**********************************************************************

IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(StmtsASTNode, "Statements")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(ExpressionListASTNode, "Expression List")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(ArgListASTNode, "Argument List")

IMPLEMENT_VISIT_ENTER_AND_LEAVE_WITH_VALUE(UnaryKwdASTNode, "Unary Keyword")

IMPLEMENT_VISIT_WITH_VALUE(LeafKwdASTNode, "Leaf Keyword")

IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(WhileASTNode, "While Statement")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(ForASTNode, "For Statement")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(ForeachASTNode, "Foreach Statement")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(IfASTNode, "If Statement")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(ElseASTNode, "Else Statement")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(CompoundASTNode, "Compount Statement")

IMPLEMENT_VISIT_ENTER_AND_LEAVE_WITH_VALUE(FunctionASTNode, "Function")

IMPLEMENT_DEFAULT_VISIT(FunctionNameASTNode, "Function name")

IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(TernaryOpASTNode, "Ternary Statement")

IMPLEMENT_VISIT_ENTER_AND_LEAVE_WITH_VALUE(PrefixOpASTNode, "Prefix Operation")
IMPLEMENT_VISIT_ENTER_AND_LEAVE_WITH_VALUE(SuffixOpASTNode, "Suffix Operation")
IMPLEMENT_VISIT_ENTER_AND_LEAVE_WITH_VALUE(BinaryOpASTNode, "Binary Operation")
IMPLEMENT_VISIT_ENTER_AND_LEAVE_WITH_VALUE(UnaryOpASTNode, "Unary Operation")

IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(CallASTNode, "Call Statement")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(TryASTNode, "Try Statement")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(TrapASTNode, "Trap Statement")

IMPLEMENT_DEFAULT_VISIT(StringifiedIdASTNode, "Stringified id")
IMPLEMENT_DEFAULT_VISIT(StdLibfuncASTNode, "Std libfunc")
IMPLEMENT_DEFAULT_VISIT(LibfuncASTNode, "Libfunc")
IMPLEMENT_DEFAULT_VISIT(AttributeASTNode, "Attribute")
IMPLEMENT_DEFAULT_VISIT(VariableASTNode, "Variable")
IMPLEMENT_DEFAULT_VISIT(ConstASTNode, "Const Value")
IMPLEMENT_DEFAULT_VISIT(ArgASTNode, "Argument")

IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(NewAttributeASTNode, "Attribute Construct")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(TableElemASTNode, "Table Element")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(TableElemsASTNode, "Table Elements")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(TableIndexListASTNode, "Table Indexed List")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(TableConstructASTNode, "Table Construct")

IMPLEMENT_DEFAULT_VISIT(TableConstKeyASTNode, "Const key")

IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(OtherStmtASTNode, "Other statement")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(UsingASTNode, "Using")
IMPLEMENT_DEFAULT_VISIT_ENTER_AND_LEAVE(QuotedElementsASTNode, "Quoted Elements")

//**********************************************************************

inline const String _GetLabel (const String& type, const String& value)
	{ return type + (value.empty() ? String() : _T(": ") + value); }

inline ItemData* _CreateData (DeltaASTNode* node)
	{ return new ItemData(node); }

//**********************************************************************

void DeltaASTVisualizer::pushItem (DeltaASTNode* node, const String& type, const String& value)
{
	wxTreeItemId treeId = m_tree->InsertItem(m_itemList.back(), m_previousItem,
		_GetLabel(type, value), -1, -1, _CreateData(node));

	m_itemList.push_back(m_previousItem = treeId);
}

//**********************************************************************

void DeltaASTVisualizer::popItem (void)
{
	m_previousItem = m_itemList.back();
	m_itemList.pop_back();
}

//**********************************************************************

void DeltaASTVisualizer::addItem (DeltaASTNode* node, const String& type, const String& value)
{
	m_previousItem = m_tree->AppendItem(m_itemList.back(),
		_GetLabel(type, value), -1, -1, _CreateData(node));
}

////////////////////////////////////////////////////////////////////////

} // namespace delta

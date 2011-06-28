/**
 *	DeltaASTVisualizer.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTVISUALIZER_H
#define DELTA_DELTAASTVISUALIZER_H

#include "Common.h"
#include "DeltaASTVisitor.h"
#include "ProgramDescription.h"

#include <wx/treectrl.h>
class wxWindow;
class wxMenu;
class wxCommand;

#include <boost/function.hpp>

namespace delta {

////////////////////////////////////////////////////////////////////////

typedef std::list<wxTreeItemId> TreeItemList;
class ItemPosition;

////////////////////////////////////////////////////////////////////////
// ASTreeCtrl class
//
class ASTreeCtrl : public wxTreeCtrl, public ProgramDescription::View {
public:

	// Commands:
	// 0, 1, ... :	UserCommands (id of AddCommand)
	// -1 :			Selected item
	// -2 :			Activated item
	// -3 :			Hover on item
	//
	typedef boost::function<void (DeltaASTNode* node, int command)> NodeFunc;

	ASTreeCtrl (wxWindow* parent, ProgramDescription* descr, const NodeFunc& func);
	~ASTreeCtrl (void);

	static void AddCommand (int id, const String& name, const String& helpString = String());
	static void ClearCommands (void);

	void ExpandRoot (void);
	void Focus (DeltaASTNode* node);

protected:
	virtual void OnSetAST (StmtsASTNode* ast);

	virtual void OnMerge (
			StmtsASTNode*			parent,
			StmtsASTNode::Iterator	end,
			StmtsASTNode*			src
		);
	virtual void OnRemove (StmtsASTNode* parent, const Range& range);
	virtual void OnRemove (DeltaASTNode* node);
	virtual void OnRefreshChildren (DeltaASTNode* node);

	void Fill (StmtsASTNode* root);
	void Clear (void);

private:
	DECLARE_EVENT_TABLE();

	wxTreeItemId findNode (DeltaASTNode* node, wxTreeItemId root);

	void onItemSelected (wxTreeEvent& event);
	void onItemActivated (wxTreeEvent& event);
	void onHover (wxTreeEvent& event);
	void onCtxMenu (wxTreeEvent& event);
	void onCommand (wxCommandEvent& event);

	NodeFunc		m_nodeFunc;
	static wxMenu*	s_menu;
};

////////////////////////////////////////////////////////////////////////

class DeltaASTVisualizer : public DeltaASTVisitor {
protected:
	friend class ASTreeCtrl;

	DeltaASTVisualizer (ASTreeCtrl* visTree);
	DeltaASTVisualizer (ASTreeCtrl* visTree, wxTreeItemId root, wxTreeItemId previous);
	~DeltaASTVisualizer (void);

protected:
	virtual bool VisitEnter (StmtsASTNode* node);
	virtual bool VisitLeave (StmtsASTNode* node);

	virtual bool VisitEnter (ExpressionListASTNode* node);
	virtual bool VisitLeave (ExpressionListASTNode* node);

	virtual bool VisitEnter (ArgListASTNode* node);
	virtual bool VisitLeave (ArgListASTNode* node);

	virtual bool VisitEnter (UnaryKwdASTNode* node);
	virtual bool VisitLeave (UnaryKwdASTNode* node);

	virtual bool Visit (LeafKwdASTNode* node);

	virtual bool VisitEnter (WhileASTNode* node);
	virtual bool VisitLeave (WhileASTNode* node);

	virtual bool VisitEnter (ForASTNode* node);
	virtual bool VisitLeave (ForASTNode* node);

	virtual bool VisitEnter (ForeachASTNode* node);
	virtual bool VisitLeave (ForeachASTNode* node);

	virtual bool VisitEnter (IfASTNode* node);
	virtual bool VisitLeave (IfASTNode* node);

	virtual bool VisitEnter (ElseASTNode* node);
	virtual bool VisitLeave (ElseASTNode* node);

	virtual bool VisitEnter (CompoundASTNode* node);
	virtual bool VisitLeave (CompoundASTNode* node);

	virtual bool VisitEnter (FunctionASTNode* node);
	virtual bool VisitLeave (FunctionASTNode* node);

	virtual bool Visit (FunctionNameASTNode* node);

	virtual bool VisitEnter (TernaryOpASTNode* node);
	virtual bool VisitLeave (TernaryOpASTNode* node);

	virtual bool VisitEnter (PrefixOpASTNode* node);
	virtual bool VisitLeave (PrefixOpASTNode* node);

	virtual bool VisitEnter (SuffixOpASTNode* node);
	virtual bool VisitLeave (SuffixOpASTNode* node);

	virtual bool VisitEnter (BinaryOpASTNode* node);
	virtual bool VisitLeave (BinaryOpASTNode* node);

	virtual bool VisitEnter (UnaryOpASTNode* node);
	virtual bool VisitLeave (UnaryOpASTNode* node);

	virtual bool VisitEnter (CallASTNode* node);
	virtual bool VisitLeave (CallASTNode* node);

	virtual bool VisitEnter (TryASTNode* node);
	virtual bool VisitLeave (TryASTNode* node);

	virtual bool VisitEnter (TrapASTNode* node);
	virtual bool VisitLeave (TrapASTNode* node);

	virtual bool Visit (StringifiedIdASTNode* node);
	virtual bool Visit (StdLibfuncASTNode* node);
	virtual bool Visit (LibfuncASTNode* node);
	virtual bool Visit (AttributeASTNode* node);
	virtual bool Visit (VariableASTNode* node);
	virtual bool Visit (ConstASTNode* node);
	virtual bool Visit (ArgASTNode* node);

	virtual bool VisitEnter (NewAttributeASTNode* node);
	virtual bool VisitLeave (NewAttributeASTNode* node);

	virtual bool VisitEnter (TableElemASTNode* node);
	virtual bool VisitLeave (TableElemASTNode* node);

	virtual bool VisitEnter (TableElemsASTNode* node);
	virtual bool VisitLeave (TableElemsASTNode* node);

	virtual bool VisitEnter (TableIndexListASTNode* node);
	virtual bool VisitLeave (TableIndexListASTNode* node);

	virtual bool VisitEnter (TableConstructASTNode* node);
	virtual bool VisitLeave (TableConstructASTNode* node);

	virtual bool Visit (TableConstKeyASTNode* node);

	virtual bool VisitEnter (OtherStmtASTNode* node);
	virtual bool VisitLeave (OtherStmtASTNode* node);

	virtual bool VisitEnter (UsingASTNode* node);
	virtual bool VisitLeave (UsingASTNode* node);

private:
	void pushItem (DeltaASTNode* node, const String& type, const String& value = String());
	void popItem (void);
	void addItem (DeltaASTNode* node, const String& type, const String& value = String());

	ASTreeCtrl*		m_tree;
	TreeItemList	m_itemList;
	wxTreeItemId	m_previousItem;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTVISUALIZER_H

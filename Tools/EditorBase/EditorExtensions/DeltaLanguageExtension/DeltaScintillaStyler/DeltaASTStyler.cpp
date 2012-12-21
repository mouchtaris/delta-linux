/**
 *	DeltaASTStyler.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaASTStyler.h"

#include "EditorMain/EditorWindow.h"
#include "DeltaAST.h"

namespace delta {

////////////////////////////////////////////////////////////////////////
// DeltaASTStyler class
//
DeltaASTStyler::DeltaASTStyler (editor::EditorWindow* window, uint start, uint end) : m_window(window)
{
	int startLine = window->LineFromPosition(start);
	m_foldLevel = window->GetFoldLevel(startLine) & wxSCI_FOLDLEVELNUMBERMASK;
	int endLine = window->LineFromPosition(end);
	while (startLine <= endLine)
		window->SetFoldLevel(startLine++, m_foldLevel);
}

//**********************************************************************

DeltaASTStyler::~DeltaASTStyler (void)
{
}

//**********************************************************************

#define IMPLEMENT_EMPTY_VISIT_ENTER(type)	bool DeltaASTStyler::VisitEnter (type* node) { return true; }
#define IMPLEMENT_EMPTY_VISIT_LEAVE(type)	bool DeltaASTStyler::VisitLeave (type* node) { return true; }
#define IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(type)	\
	IMPLEMENT_EMPTY_VISIT_ENTER(type)				\
	IMPLEMENT_EMPTY_VISIT_LEAVE(type)

#define IMPLEMENT_CONTROL_STMT_VISIT_ENTER(type, len)		\
	bool DeltaASTStyler::VisitEnter (type* node) {			\
		this->highlight(node->GetRange().left, len, node);	\
		this->fold(node);									\
		++m_foldLevel;										\
		return true;										\
	}

#define IMPLEMENT_CONTROL_STMT_VISIT_LEAVE(type)	\
	bool DeltaASTStyler::VisitLeave (type* node) {	\
		--m_foldLevel;								\
		return true;								\
	}

#define IMPLEMENT_CONTROL_STMT_VISIT_ENTER_AND_LEAVE(type, len)	\
	IMPLEMENT_CONTROL_STMT_VISIT_ENTER(type, len)				\
	IMPLEMENT_CONTROL_STMT_VISIT_LEAVE(type)


#define IMPLEMENT_VISIT(type)	bool DeltaASTStyler::Visit (type* node) { highlight(node); return true; }

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(StmtsASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(ExpressionListASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(ArgListASTNode)

//**********************************************************************

bool DeltaASTStyler::VisitEnter (UnaryKwdASTNode* node)
{
	this->highlight(node->GetRange().left, (uint) node->GetValue().Size(), node);
	return true;
}

IMPLEMENT_EMPTY_VISIT_LEAVE(UnaryKwdASTNode)

//**********************************************************************

IMPLEMENT_VISIT(LeafKwdASTNode)

//**********************************************************************

IMPLEMENT_CONTROL_STMT_VISIT_ENTER_AND_LEAVE(WhileASTNode, 5)
IMPLEMENT_CONTROL_STMT_VISIT_ENTER_AND_LEAVE(ForASTNode, 3)
IMPLEMENT_CONTROL_STMT_VISIT_ENTER_AND_LEAVE(ForeachASTNode, 7)
IMPLEMENT_CONTROL_STMT_VISIT_ENTER_AND_LEAVE(IfASTNode, 2)

//**********************************************************************

bool DeltaASTStyler::VisitEnter (ElseASTNode* node)
{
	this->highlight(node->GetRange().left, 4, node);
	return true;
}

IMPLEMENT_EMPTY_VISIT_LEAVE(ElseASTNode)

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(CompoundASTNode)

//**********************************************************************

IMPLEMENT_CONTROL_STMT_VISIT_ENTER_AND_LEAVE(TryASTNode, 3)

//**********************************************************************

bool DeltaASTStyler::VisitEnter (TrapASTNode* node)
{
	this->highlight(node->GetRange().left, 4, node);
	return true;
}

IMPLEMENT_EMPTY_VISIT_LEAVE(TrapASTNode)

//**********************************************************************

bool DeltaASTStyler::VisitEnter (FunctionASTNode* node)
{
	this->highlight(node->GetRange().left, (uint) node->GetValue().Size(), node);
	this->fold(node);
	++m_foldLevel;
	return true;
}

bool DeltaASTStyler::VisitLeave (FunctionASTNode* node)
{
	--m_foldLevel;
	return true;
}

//**********************************************************************

IMPLEMENT_VISIT(FunctionNameASTNode)

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TernaryOpASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(SuffixOpASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(UnaryOpASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(CallASTNode)

//**********************************************************************

bool DeltaASTStyler::VisitEnter (PrefixOpASTNode* node) {
	Literal l = node->GetValue();
	if (l == "not")	//highlight prefix operator 'not' as a keyword
		this->highlight(node->GetRange().left, (uint) l.Size(), UnaryKwdASTNode::Type);
	return true;
}

IMPLEMENT_EMPTY_VISIT_LEAVE(PrefixOpASTNode)

//**********************************************************************

bool DeltaASTStyler::VisitEnter (BinaryOpASTNode* node) {
	Literal l = node->GetValue();
	if (l == "or" || l == "and") {	//highlight binary operators 'or' and 'and' as keywords
		uint start = node->GetLeftChild()->GetRange().right;
		uint end = node->GetRightChild()->GetRange().left;
		this->highlight(start, end - start, UnaryKwdASTNode::Type);
	}
	return true;
}

IMPLEMENT_EMPTY_VISIT_LEAVE(BinaryOpASTNode)

//**********************************************************************

IMPLEMENT_VISIT(StringifiedIdASTNode)
IMPLEMENT_VISIT(StdLibfuncASTNode)
IMPLEMENT_VISIT(LibfuncASTNode)
IMPLEMENT_VISIT(AttributeASTNode)
IMPLEMENT_VISIT(VariableASTNode)
IMPLEMENT_VISIT(ConstASTNode)
IMPLEMENT_VISIT(ArgASTNode)

//**********************************************************************

bool DeltaASTStyler::VisitEnter (NewAttributeASTNode* node)
{
	this->fold(node);
	++m_foldLevel;
	return true;
}

bool DeltaASTStyler::VisitLeave (NewAttributeASTNode* node)
{
	--m_foldLevel;
	return true;
}

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TableElemASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TableElemsASTNode)
IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(TableIndexListASTNode)

//**********************************************************************

bool DeltaASTStyler::VisitEnter (TableConstructASTNode* node)
{
	this->fold(node);
	++m_foldLevel;
	return true;
}

bool DeltaASTStyler::VisitLeave (TableConstructASTNode* node)
{
	--m_foldLevel;
	return true;
}

//**********************************************************************

IMPLEMENT_VISIT(TableConstKeyASTNode)

//**********************************************************************

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(OtherStmtASTNode)


bool DeltaASTStyler::VisitEnter (UsingASTNode* node)
{
	this->highlight(node->GetRange().left, (uint) node->GetValue().Size(), node);
	if (node->GetChild<0>())
		this->highlight(node->GetChild<0>());
	if (node->GetChild<1>())
		this->highlight(node->GetChild<1>());
	return true;
}

IMPLEMENT_EMPTY_VISIT_LEAVE(UsingASTNode)

IMPLEMENT_EMPTY_VISIT_ENTER_AND_LEAVE(QuotedElementsASTNode)

//**********************************************************************

void DeltaASTStyler::highlight (DeltaASTNode* node)
{
	const DeltaASTNode::Range& range = node->GetRange();
	this->highlight(range.left, range.Length(), node->GetType());
}

//**********************************************************************

void DeltaASTStyler::highlight (uint start, uint len, DeltaASTNode* node)
{
	this->highlight(start, len, node->GetType());
}

//**********************************************************************

void DeltaASTStyler::highlight (uint start, uint len, int style)
{
	m_window->StartStyling(start, DELTA_STYLING_MASK);
	m_window->SetStyling(len, style);
}

//**********************************************************************

void DeltaASTStyler::fold (DeltaASTNode* node)
{
	const DeltaASTNode::Range& range = node->GetRange();

	int firstLine = m_window->LineFromPosition(range.left);

	//if (m_window.LevelAt(firstLine) == (m_foldLevel | SC_FOLDLEVELHEADERFLAG))
	//	return;

	int lastLine = m_window->LineFromPosition(range.right);
	if (lastLine != firstLine) {
		m_window->SetFoldLevel(firstLine, m_foldLevel | wxSCI_FOLDLEVELHEADERFLAG);
		while (++firstLine <= lastLine)
			m_window->SetFoldLevel(firstLine, m_foldLevel + 1);
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace delta

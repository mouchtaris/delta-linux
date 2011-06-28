/**
 *	ProgramDescription.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 *  Updates to support selective partial parsing, Anthony Savidis, July 2010.
 */

#include "ProgramDescription.h"
#include "DeltaAST.h"
#include "DeltaASTPositionVisitor.h"
#include "DeltaASTAlgorithms.h"
#include "DeltaASTPrinter.h"
#include "Streams.h"

namespace delta {

////////////////////////////////////////////////////////////////////////
// Convenient macro for ProgramDescription::View calling
//
#define VIEWS(list)								\
	for (size_t i = 0; i < list.size(); ++i)	\
		list[i]

////////////////////////////////////////////////////////////////////////
// ProgramDescription class
//
ProgramDescription::ProgramDescription (void) : m_programAst	((StmtsASTNode*) 0),
												m_stmtAst		((DeltaASTNode*) 0),
												m_funcAst		((DeltaASTNode*) 0),
												m_tableAst		((DeltaASTNode*) 0),
												m_namespaceAst	((DeltaASTNode*) 0){}

//**********************************************************************

ProgramDescription::~ProgramDescription (void)
{
	delete m_programAst;
}

//**********************************************************************

void ProgramDescription::AddView (View* view) { m_views.push_back(view); }

void ProgramDescription::RemoveView (View* view)
{
	Views::iterator iter = std::find(m_views.begin(), m_views.end(), view);
	if (iter != m_views.end())
		m_views.erase(iter);
}

void ProgramDescription::ClearViews (void) { m_views.clear(); }

//**********************************************************************

const ProgramDescription::Slice ProgramDescription::GetAffectedSliceAfterAdd (const Range& range)
{
	StmtsASTNode* parent = m_programAst;
	DeltaASTNode* topLevelStmt;

	m_notParsed.AddRange(range);
	Range affectedRange = m_notParsed.RemoveWholeRange<Range>();

	if (topLevelStmt = this->getTopLevelStmtInRange(affectedRange)) {

		DeltaASTNodeFinderVisitor<
				Range, TypeOfParentNodePred<StmtsASTNode>
			> stmtFinder(affectedRange);

		topLevelStmt->Accept(&stmtFinder);

		if (DeltaASTNode* stmtNode = stmtFinder.GetNode()) {
			affectedRange.Expand(stmtNode->GetRange());
			parent = static_cast<StmtsASTNode*>(stmtNode->GetParent());
		}
		else if (topLevelStmt->GetType() == StmtsASTNode::Type)
			parent = static_cast<StmtsASTNode*>(topLevelStmt);
	}

	if (!topLevelStmt && parent) {
		if (DeltaASTNode* stmtNode = this->getCurrentStmt(parent, affectedRange.left))
			affectedRange.ExpandLeft(stmtNode->GetRange());
		else // For the comments at the beginning bug ...
			affectedRange.ExpandLeft(Range(0, parent->GetRange().left));
	}

	if (parent) {
		if (DeltaASTNode* stmtNode = this->getNextStmt(parent, affectedRange.right))
			affectedRange.ExpandRight(stmtNode->GetRange());
		if (DeltaASTNode* stmtNode = this->getPrevStmt(parent, affectedRange.left))
			affectedRange.ExpandLeft(stmtNode->GetRange());
	}
	return Slice(affectedRange, parent);
}

//**********************************************************************

const ProgramDescription::Slice ProgramDescription::GetAffectedSliceAfterRemove (const Range& range)
{
	Slice slice = this->GetAffectedSliceAfterAdd(Range(range.left, range.left));
	slice.first.Expand(m_comments.GetRange(range.left));
	slice.first.Expand(m_cpp.GetRange(range.left));
	return slice;
}

//**********************************************************************

void ProgramDescription::Remove (const Slice& slice)
{
	if (slice.second)
		this->removeStmts(slice.second, slice.first);

	m_parseErrors.RemoveElementsInRange(slice.first);
	m_semanticErrors.RemoveElementsInRange(slice.first);
	m_comments.RemoveElementsInRange(slice.first);
	m_cpp.RemoveElementsInRange(slice.first);
}

//**********************************************************************

void ProgramDescription::ReplaceNode (DeltaASTNode* node, DeltaASTNode* replacement)
{
	this->replaceNode(node, replacement);
	NotifyReplaced(node, replacement);	// i-sense call.
}

//**********************************************************************

void ProgramDescription::Merge (const Slice& slice, ProgramDescription& other)
{
	if (m_programAst) {

		assert(slice.second);

		if (other.m_programAst) {
			this->mergeStmts(other.m_programAst, slice.second);
			other.SetAST(0);
		}
		m_parseErrors.Merge(other.m_parseErrors);
		m_semanticErrors.Merge(other.m_semanticErrors);
		m_comments.Merge(other.m_comments);
		m_cpp.Merge(other.m_cpp);
		m_notParsed.Merge(other.m_notParsed);
	}
	else {
		this->SetAST(other.m_programAst);
		other.m_programAst = 0;
		other.SetAST(0); // To notify its views

		m_parseErrors.Swap(other.m_parseErrors);
		m_semanticErrors.Swap(other.m_semanticErrors);
		m_comments.Swap(other.m_comments);
		m_cpp.Swap(other.m_cpp);
		m_notParsed.Swap(other.m_notParsed);
	}
}

//**********************************************************************

void ProgramDescription::SetAST (StmtsASTNode* ast)
{
	VIEWS(m_views)->OnSetAST(ast);

	delete m_programAst;
	m_programAst = ast;
}

//**********************************************************************

void ProgramDescription::AddParseError (const Range& range, const String& msg)
	{ m_parseErrors.AddRange(ErrorRange(range.left, range.right, msg)); }

void ProgramDescription::AddSemanticError (const Range& range, const String& msg)
	{ m_semanticErrors.AddRange(ErrorRange(range.left, range.right, msg)); }

void ProgramDescription::AddComment (const Range& range)
	{ m_comments.AddRange(range); }

void ProgramDescription::AddCpp (const Range& range)
	{ m_cpp.AddRange(range); }

void ProgramDescription::AddNotParsed (const Range& range)
	{ m_notParsed.AddRange(range); }

//**********************************************************************

void ProgramDescription::ClearAST (void) { this->SetAST(0); }

void ProgramDescription::ClearParseErrors (void) { m_parseErrors.Clear(); }
void ProgramDescription::ClearSemanticErrors (void) { m_semanticErrors.Clear(); }
void ProgramDescription::ClearComments (void) { m_comments.Clear(); }
void ProgramDescription::ClearCpp (void) { m_cpp.Clear(); }
void ProgramDescription::ClearNotParsed (void) { m_notParsed.Clear(); }

void ProgramDescription::ClearAll (void)
{
	this->ClearAST();
	this->ClearParseErrors();
	this->ClearSemanticErrors();
	this->ClearComments();
	this->ClearCpp();
	this->ClearNotParsed();
}

//**********************************************************************

void ProgramDescription::UpdateASTPositions (uint atPos, int offset)
{
	if (m_programAst) {

		m_programAst->GetRange().Update(atPos, offset);
		if (m_programAst->GetRange().Empty()) {
			this->SetAST(0);
			return;
		}

		StmtsASTNode::Iterator iter = m_programAst->Begin(), end = m_programAst->End();
		for (; iter != end; ++iter)
			if ((*iter)->GetRange().right > atPos)
				break;

		DeltaASTPositionUpdateVisitor visitor(atPos, offset);
		while (iter != end)
			(*iter++)->Accept(&visitor);

		const DeltaASTNodeList& removedNodes = visitor.GetRemovedNodes();
		std::for_each(removedNodes.rbegin(), removedNodes.rend(),
			std::bind1st(std::mem_fun(&ProgramDescription::removeNode), this));
	}
}

void ProgramDescription::UpdateParseErrorPositions (uint atPos, int offset)
	{ m_parseErrors.UpdatePositionsWithoutRemove(atPos, offset); } // TODO: Temporary ... refactor

void ProgramDescription::UpdateSemanticErrorPositions (uint atPos, int offset)
	{ m_semanticErrors.UpdatePositionsWithoutRemove(atPos, offset); } // TODO: Temporary ... refactor

void ProgramDescription::UpdateCommentPositions (uint atPos, int offset)
	{ m_comments.UpdatePositions(atPos, offset); }

void ProgramDescription::UpdateCppPositions (uint atPos, int offset)
	{ m_cpp.UpdatePositions(atPos, offset); }

void ProgramDescription::UpdateNotParsedPositions (uint atPos, int offset)
	{ m_notParsed.UpdatePositions(atPos, offset); }

void ProgramDescription::UpdateAllPositions (uint atPos, int offset)
{
	this->UpdateASTPositions(atPos, offset);
	this->UpdateParseErrorPositions(atPos, offset);
	this->UpdateSemanticErrorPositions(atPos, offset);
	this->UpdateCommentPositions(atPos, offset);
	this->UpdateCppPositions(atPos, offset);
	this->UpdateNotParsedPositions(atPos, offset);
}

//**********************************************************************

DeltaASTNode* ProgramDescription::GetNode (uint atPos)
{
	if (m_programAst) {
		DeltaASTNodeFinderVisitor<uint> finder(atPos);
		m_programAst->Accept(&finder);
		return finder.GetNode();
	}
	return 0;
}

//**********************************************************************
// Added for i-sense (AS).

DeltaASTNode* ProgramDescription::GetCursorNode (uint atPos) {
	if (m_programAst) {
		DeltaASTNodeFocusFinderVisitor<uint> finder(atPos);
		m_programAst->Accept(&finder);
		return finder.GetNode();
	}
	else
		return (DeltaASTNode*) 0;
}

const std::string ProgramDescription::GetFocusNodeInformation (void) const {
	if (!m_focusNode)
		return "no focus item";
	else {
		DeltaASTPrinter printer;
		m_focusNode->AcceptNode(&printer);
		return util::str2std(printer.GetDescription());
	}
}

//**********************************************************************

const ErrorRange& ProgramDescription::GetParseError (uint atPos)
	{ return m_parseErrors.GetRange(atPos); }

const ErrorRange& ProgramDescription::GetSemanticError (uint atPos)
	{ return m_semanticErrors.GetRange(atPos); }

const Range& ProgramDescription::GetComment (uint atPos)
	{ return m_comments.GetRange(atPos); }

const Range& ProgramDescription::GetCpp (uint atPos)
	{ return m_cpp.GetRange(atPos); }

const Range& ProgramDescription::GetNotParsed (uint atPos)
	{ return m_notParsed.GetRange(atPos); }

//**********************************************************************

DeltaASTNode* ProgramDescription::GetNodeAbove (uint pos)
{
	DeltaASTNode* node = this->GetNode(pos);
	node = node ? node : m_programAst;

	if (node && node->GetType() == StmtsASTNode::Type) {
		StmtsASTNode* stmts = static_cast<StmtsASTNode*>(node);
		StmtsASTNode::Iterator iter = stmts->Begin();
		for (; iter != stmts->End(); ++iter) {
			if ((*iter)->GetRange().right > pos)
				break;
			node = *iter;
		}
	}
	return node;
}

//**********************************************************************

DeltaASTNode* ProgramDescription::getTopLevelStmtInRange (const Range& range)
{
	if (m_programAst) {
		StmtsASTNode::Iterator iter = m_programAst->Begin(), end = m_programAst->End();
		for (; iter != end; ++iter) {
			if ((*iter)->GetRange().Contains(range))
				return *iter;
		}
	}
	return 0;
}

//**********************************************************************

DeltaASTNode* ProgramDescription::getCurrentStmt (StmtsASTNode* node, uint pos)
{
	StmtsASTNode::Iterator iter = node->Begin(), end = node->End();
	for (; iter != end; ++iter) {
		if ((*iter)->GetRange().Contains(pos))
			return *iter;
	}
	return 0;
}

//**********************************************************************

DeltaASTNode* ProgramDescription::getNextStmt (StmtsASTNode* node, uint pos)
{
	StmtsASTNode::Iterator iter = node->Begin(), end = node->End();
	for (; iter != end; ++iter) {
		if ((*iter)->GetRange().IsRightOf(pos))
			return *iter;
	}

	if (!node->Empty() && node->Back()->GetRange().Contains(pos))
		return node->Back();
	return 0;
}

//*********************************************************************

DeltaASTNode* ProgramDescription::getPrevStmt (StmtsASTNode* node, uint pos)
{
	StmtsASTNode::Iterator iter = node->Begin(), end = node->End();
	StmtsASTNode::Iterator prevIter = end;
	for (; iter != end; ++iter) {
		if ((*iter)->GetRange().IsLeftOf(pos))
			prevIter = iter;
		else
			break;
	}
	if (prevIter != end)
		return *prevIter;

	if (!node->Empty() && node->Front()->GetRange().Contains(pos))
			return node->Front();
	return 0;
}

//**********************************************************************

void ProgramDescription::removeStmts (StmtsASTNode* node, const Range& range)
{
	VIEWS(m_views)->OnRemove(node, range);

	StmtsASTNode::Iterator iter = node->Begin();
	while (iter != node->End()) {
		const Range& childRange = (*iter)->GetRange();
		if (range.Contains(childRange)) {
			NotifyDeleted(*iter);	// i-sense call.
			iter = node->EraseChild(iter);
		}
		else if (childRange.IsRightOf(range))
			break;
		else
			++iter;
	}
}

//**********************************************************************

void ProgramDescription::removeNode (DeltaASTNode* node)
{
	assert(node->GetParent());

	VIEWS(m_views)->OnRemove(node);
	NotifyDeleted(node);	// i-sense call.
	node->GetParent()->RemoveChild(node);
}

//**********************************************************************

void ProgramDescription::replaceNode (DeltaASTNode* node, DeltaASTNode* replacement)
{
	DeltaASTNode* parent = node->GetParent();
	assert(parent);
	NotifyReplaced(node, replacement);	// i-sense call.
	parent->ReplaceChild(node, replacement);	
	VIEWS(m_views)->OnRefreshChildren(parent);
}

//**********************************************************************

void ProgramDescription::mergeStmts (StmtsASTNode* src, StmtsASTNode* dest)
{
	StmtsASTNode::Iterator iter = dest->Begin(), end = dest->End();
	for (; iter != end; ++iter)
		if ((*iter)->GetRange().IsRightOf(src->GetRange()))
			break;

	VIEWS(m_views)->OnMerge(dest, iter, src);
	NotifyInserted(dest, iter != end ? *iter : (DeltaASTNode*) 0, src); // i-sense call (null means at end).
	if (iter == dest->Begin())	// Added before
		dest->GetRange().ExpandLeft(src->GetRange());
	dest->Splice(iter, src);	
}

////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG

static void _InsNPrint (const Range* ranges, int size)
{
	ProgramDescription::RangeList list;

	DBGOUT << "Original ranges:" << std::endl;
	for (int i = 0; i < size; ++i) {
		list.AddRange(ranges[i]);
		DBGOUT << "(" << ranges[i].left << ", " << ranges[i].right << ")" << std::endl;
	}

	DBGOUT << "RangeList(TM) ranges:" << std::endl;
	ProgramDescription::RangeList::iterator iter = list.begin();
	for (; iter != list.end(); ++iter)
		DBGOUT << "(" << iter->left << ", " << iter->right << ")" << std::endl;
}

//**********************************************************************

#define BEGIN_TEST(N)												\
	DBGOUT << "RangeList(TM) TEST " << STRINGIFY(N) << std::endl;	\
	const Range test##N[] = {

#define END_TEST(N)	\
	};				\
	_InsNPrint(test##N, SIZEOF_ARRAY(test##N))

//**********************************************************************

void ProgramDescription::Test (void)
{
	BEGIN_TEST(1)
		Range(0, 1), Range(1, 2), Range(2, 3), Range(4, 5), Range(6, 7)
	END_TEST(1);

	BEGIN_TEST(2)
		Range(1, 2), Range(1, 2), Range(2, 3), Range(4, 5), Range(6, 7),
		Range(15, 20), Range(25, 30), Range(1, 30), Range(2, 30)
	END_TEST(2);

	BEGIN_TEST(3)
		Range(0, 5), Range(10, 20), Range(30, 40), Range(50, 60), Range(31, 49)
	END_TEST(3);

	BEGIN_TEST(4)
		Range(0, 5), Range(10, 20), Range(30, 40), Range(50, 60), Range(42, 45)
	END_TEST(4);

}
#endif

////////////////////////////////////////////////////////////////////////

} // namespace delta

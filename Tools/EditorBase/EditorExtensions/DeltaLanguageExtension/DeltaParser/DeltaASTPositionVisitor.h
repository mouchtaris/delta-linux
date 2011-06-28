/**
 *	DeltaASTPositionVisitor.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTPOSITIONVISITOR_H
#define DELTA_DELTAASTPOSITIONVISITOR_H

#include "Common.h"
#include "DeltaASTGenericVisitor.h"
#include "DeltaAST.h"

class Accessor;

namespace delta {

////////////////////////////////////////////////////////////////////////
// Useful predicates for DeltaASTNodeFinderVisitor
//
struct TrueNodePred : public std::unary_function<DeltaASTNode*, bool> {
	bool operator() (DeltaASTNode*) const { return true; }
};

//**********************************************************************

template<class TNodeType>
struct TypeOfNodePred : public std::unary_function<DeltaASTNode*, bool> {
	bool operator() (DeltaASTNode* node) const { return node->GetType() == TNodeType::Type; }
};

//**********************************************************************

template<class TNodeType>
struct TypeOfParentNodePred : public std::unary_function<DeltaASTNode*, bool> {
	bool operator() (DeltaASTNode* node) const
		{ return node->GetParent() && node->GetParent()->GetType() == TNodeType::Type; }
};

////////////////////////////////////////////////////////////////////////

template<class TRange, class TPred = TrueNodePred>
class DeltaASTNodeFinderVisitor : public DeltaASTGenericVisitor {
public:

	DeltaASTNodeFinderVisitor (PARAM(TRange) range) : m_range(range), m_node(0){}
	DeltaASTNode* GetNode (void) const { return m_node; }

protected:
	virtual bool VisitEnter (DeltaASTNode* node)	{ return this->check(node); }
	virtual bool VisitLeave (DeltaASTNode* node)	{ return true; }
	virtual bool Visit (DeltaASTNode* node)			{ return this->check(node); }

	virtual bool check (DeltaASTNode* node) {
		const Range& range = node->GetRange();
		if (range.Contains(m_range)) {
			if (TPred()(node) && (!m_node || range.Length() <= m_node->GetRange().Length()))
				m_node = node;
			return true;
		}
		else if (range.IsRightOf(m_range))
			return false;
		return true;
	}

	const TRange	m_range;
	DeltaASTNode*	m_node;
};

////////////////////////////////////////////////////////////////////////
// i-sense addition (determine focus node from cursor position).

template<class TRange, class TPred = TrueNodePred>
class DeltaASTNodeFocusFinderVisitor : public DeltaASTNodeFinderVisitor<TRange, TPred> {
	public:
	DeltaASTNodeFocusFinderVisitor (PARAM(TRange) range) : DeltaASTNodeFinderVisitor(range){}

	protected:
	virtual bool check (DeltaASTNode* node) {
		const Range& range = node->GetRange();
		if (range.IsLeftOf(m_range) || range.Contains(m_range)) 
			{ m_node = node; return true; }
		else 
		if (range.IsRightOf(m_range))
			return false;
		else
			return true;
	}
};

////////////////////////////////////////////////////////////////////////

class DeltaASTPositionUpdateVisitor : public DeltaASTGenericVisitor {
public:
	DeltaASTPositionUpdateVisitor (uint from, int offset) :
		m_from(from), m_offset(offset) {}
	~DeltaASTPositionUpdateVisitor (void);

	const DeltaASTNodeList&	GetRemovedNodes (void) const { return m_removedNodes; }

protected:
	virtual bool VisitEnter (DeltaASTNode* node);
	virtual bool VisitLeave (DeltaASTNode* node);
	virtual bool Visit (DeltaASTNode* node);

private:
	inline bool update (DeltaASTNode* node);

	uint				m_from;
	int					m_offset;
	DeltaASTNodeList	m_removedNodes;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTPOSITIONVISITOR_H

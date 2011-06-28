/**
 *	DeltaASTPositionVisitor.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaASTPositionVisitor.h"
#include "DeltaAST.h"

namespace delta {

////////////////////////////////////////////////////////////////////////
// Utility functions
//
inline bool _IsStmt (DeltaASTNode* node)
{
	switch (node->GetType()) {
		case OtherStmtASTNode::Type:
		case WhileASTNode::Type:
		case ForASTNode::Type:
		case ForeachASTNode::Type:
		case IfASTNode::Type:
		case CompoundASTNode::Type:
		case TryASTNode::Type:
			return true;
		case FunctionASTNode::Type:
 			return node->GetParent()->GetType() != UnaryOpASTNode::Type;
		case UnaryKwdASTNode::Type: {
			UnaryKwdASTNode* kwd = static_cast<UnaryKwdASTNode*>(node);
			return !strcmp(kwd->GetValue(), "assert") || 
				!strcmp(kwd->GetValue(), "return") || 
				!strcmp(kwd->GetValue(), "throw");
		}
		default:
			return false;
	}
}

////////////////////////////////////////////////////////////////////////
// DeltaASTPositionUpdateVisitor class
//
DeltaASTPositionUpdateVisitor::~DeltaASTPositionUpdateVisitor (void)
{
}

//**********************************************************************

bool DeltaASTPositionUpdateVisitor::VisitEnter (DeltaASTNode* node) { return this->update(node); }
bool DeltaASTPositionUpdateVisitor::VisitLeave (DeltaASTNode* node) { return true; }
bool DeltaASTPositionUpdateVisitor::Visit (DeltaASTNode* node) { return this->update(node); }

//**********************************************************************

bool DeltaASTPositionUpdateVisitor::update (DeltaASTNode* node)
{
	node->GetRange().Update(m_from, m_offset);
	if (node->GetRange().Empty()) {
		assert(node->GetParent());
		m_removedNodes.push_back(node);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace delta

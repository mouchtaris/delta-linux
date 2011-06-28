/**
 *	DeltaASTAlgorithms.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTALGORITHMS_H
#define DELTA_DELTAASTALGORITHMS_H

#include "Common.h"
#include "DeltaAST.h"

namespace delta {

////////////////////////////////////////////////////////////////////////
// Useful predicates for DeltaASTNodeFinderVisitor
//
/*
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
*/
////////////////////////////////////////////////////////////////////////

template<class TRange, class TFunc>
DeltaASTNode* FindNodeContainingRange (PARAM(TRange) range, DeltaASTNode* root)
{
	DeltaASTNode* result = root;
	
}

bool UpdatePosition (DeltaASTNode* root, DeltaASTNodeList& removedNodes, uint pos, int offset)
{
	DeltaASTNodeList nodes;
	root->GetChildren(nodes);
	while (!nodes.empty()) {
		DeltaASTNode* node = nodes.back();
		nodes.pop_back();
		node->GetRange().Update(pos, offset);
		if (node->GetRange().Empty()) {
			if (!node->GetParent())
				return false;
			removedNodes.push_back(node);
		}
		node->GetChildren(nodes);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTALGORITHMS_H

/**
 *	DeltaASTGenericVisitor.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaASTGenericVisitor.h"
#include "DeltaAST.h"

namespace delta {

////////////////////////////////////////////////////////////////////////
// DeltaASTGenericVisitor class
//
DeltaASTGenericVisitor::DeltaASTGenericVisitor (void)
{
}

//**********************************************************************

DeltaASTGenericVisitor::~DeltaASTGenericVisitor (void)
{
}

//**********************************************************************

#define IMPLEMENT_VISITOR_VISIT_ENTER(type)				\
	bool DeltaASTGenericVisitor::VisitEnter (type* node)\
		{ return this->dispatchVisitEnter(node); }

#define IMPLEMENT_VISITOR_VISIT_LEAVE(type)				\
	bool DeltaASTGenericVisitor::VisitLeave (type* node)\
		{ return this->dispatchVisitLeave(node); }

#define IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(type)	\
	IMPLEMENT_VISITOR_VISIT_ENTER(type)					\
	IMPLEMENT_VISITOR_VISIT_LEAVE(type)

#define IMPLEMENT_VISITOR_VISIT(type)					\
	bool DeltaASTGenericVisitor::Visit (type* node)		\
		{ return this->dispatchVisit(node); }

IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(StmtsASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(ExpressionListASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(ArgListASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(UnaryKwdASTNode)

IMPLEMENT_VISITOR_VISIT(LeafKwdASTNode)

IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(WhileASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(ForASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(ForeachASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(IfASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(ElseASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(CompoundASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(TryASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(TrapASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(FunctionASTNode)

IMPLEMENT_VISITOR_VISIT(FunctionNameASTNode)

IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(TernaryOpASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(PrefixOpASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(SuffixOpASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(BinaryOpASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(UnaryOpASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(CallASTNode)

IMPLEMENT_VISITOR_VISIT(StringifiedIdASTNode)
IMPLEMENT_VISITOR_VISIT(StdLibfuncASTNode)
IMPLEMENT_VISITOR_VISIT(LibfuncASTNode)
IMPLEMENT_VISITOR_VISIT(AttributeASTNode)
IMPLEMENT_VISITOR_VISIT(VariableASTNode)
IMPLEMENT_VISITOR_VISIT(ConstASTNode)
IMPLEMENT_VISITOR_VISIT(ArgASTNode)

IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(NewAttributeASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(TableElemASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(TableElemsASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(TableIndexListASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(TableConstructASTNode)

IMPLEMENT_VISITOR_VISIT(TableConstKeyASTNode)

IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(OtherStmtASTNode)
IMPLEMENT_VISITOR_VISIT_ENTER_AND_LEAVE(UsingASTNode)

//**********************************************************************

bool DeltaASTGenericVisitor::dispatchVisitEnter (DeltaASTNode* node)
	{ return this->VisitEnter(node); }

bool DeltaASTGenericVisitor::dispatchVisitLeave (DeltaASTNode* node)
	{ return this->VisitLeave(node); }

bool DeltaASTGenericVisitor::dispatchVisit (DeltaASTNode* node)
	{ return this->Visit(node); }

////////////////////////////////////////////////////////////////////////
// DeltaASTGenericNodeVisitor
//
#define IMPLEMENT_NODE_VISITOR_VISIT(type)				\
	void DeltaASTGenericNodeVisitor::Visit (type* node)	\
		{ this->dispatchVisit(node); }

IMPLEMENT_NODE_VISITOR_VISIT(StmtsASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(ExpressionListASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(ArgListASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(UnaryKwdASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(LeafKwdASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(WhileASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(ForASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(ForeachASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(IfASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(ElseASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(CompoundASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(TryASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(TrapASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(FunctionASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(FunctionNameASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(TernaryOpASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(PrefixOpASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(SuffixOpASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(BinaryOpASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(UnaryOpASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(CallASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(StringifiedIdASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(StdLibfuncASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(LibfuncASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(AttributeASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(VariableASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(ConstASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(ArgASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(NewAttributeASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(TableElemASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(TableElemsASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(TableIndexListASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(TableConstructASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(TableConstKeyASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(OtherStmtASTNode)
IMPLEMENT_NODE_VISITOR_VISIT(UsingASTNode)

//**********************************************************************

void DeltaASTGenericNodeVisitor::dispatchVisit (DeltaASTNode* node)
	{ this->Visit(node); }

////////////////////////////////////////////////////////////////////////

} // namespace delta

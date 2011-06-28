/**
 *	DeltaASTGenericVisitor.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTGENERICVISITOR_H
#define DELTA_DELTAASTGENERICVISITOR_H

#include "Common.h"
#include "DeltaASTVisitor.h"

class Accessor;

namespace delta {

////////////////////////////////////////////////////////////////////////

class DeltaASTGenericVisitor : public DeltaASTVisitor {
public:
	DeltaASTGenericVisitor (void);
	~DeltaASTGenericVisitor (void);

	virtual bool VisitEnter (DeltaASTNode* node) = 0;
	virtual bool VisitLeave (DeltaASTNode* node) = 0;

	virtual bool Visit (DeltaASTNode* node) = 0;

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

	virtual bool VisitEnter (TryASTNode* node);
	virtual bool VisitLeave (TryASTNode* node);

	virtual bool VisitEnter (TrapASTNode* node);
	virtual bool VisitLeave (TrapASTNode* node);

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
	inline bool dispatchVisitEnter (DeltaASTNode* node);
	inline bool dispatchVisitLeave (DeltaASTNode* node);
	inline bool dispatchVisit (DeltaASTNode* node);
};

////////////////////////////////////////////////////////////////////////

class DeltaASTGenericNodeVisitor {
public:
	virtual bool Visit (DeltaASTNode* node) = 0;

protected:
	virtual void Visit (StmtsASTNode* node);
	virtual void Visit (ExpressionListASTNode* node);
	virtual void Visit (ArgListASTNode* node);
	virtual void Visit (UnaryKwdASTNode* node);
	virtual void Visit (LeafKwdASTNode* node);
	virtual void Visit (WhileASTNode* node);
	virtual void Visit (ForASTNode* node);
	virtual void Visit (ForeachASTNode* node);
	virtual void Visit (IfASTNode* node);
	virtual void Visit (ElseASTNode* node);
	virtual void Visit (CompoundASTNode* node);
	virtual void Visit (TryASTNode* node);
	virtual void Visit (TrapASTNode* node);
	virtual void Visit (FunctionASTNode* node);
	virtual void Visit (FunctionNameASTNode* node);
	virtual void Visit (TernaryOpASTNode* node);
	virtual void Visit (PrefixOpASTNode* node);
	virtual void Visit (SuffixOpASTNode* node);
	virtual void Visit (BinaryOpASTNode* node);
	virtual void Visit (UnaryOpASTNode* node);
	virtual void Visit (CallASTNode* node);
	virtual void Visit (StringifiedIdASTNode* node);
	virtual void Visit (StdLibfuncASTNode* node);
	virtual void Visit (LibfuncASTNode* node);
	virtual void Visit (AttributeASTNode* node);
	virtual void Visit (VariableASTNode* node);
	virtual void Visit (ConstASTNode* node);
	virtual void Visit (ArgASTNode* node);
	virtual void Visit (NewAttributeASTNode* node);
	virtual void Visit (TableElemASTNode* node);
	virtual void Visit (TableElemsASTNode* node);
	virtual void Visit (TableIndexListASTNode* node);
	virtual void Visit (TableConstructASTNode* node);
	virtual void Visit (TableConstKeyASTNode* node);
	virtual void Visit (OtherStmtASTNode* node);
	virtual void Visit (UsingASTNode* node);

private:
	inline void dispatchVisit (DeltaASTNode* node);
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTGENERICVISITOR_H

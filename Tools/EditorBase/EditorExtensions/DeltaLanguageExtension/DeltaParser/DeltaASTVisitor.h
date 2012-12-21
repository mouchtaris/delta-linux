/**
 *	DeltaASTVisitor.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTVISITOR_H
#define DELTA_DELTAASTVISITOR_H

#include "Common.h"
#include "DeltaASTFwdDecls.h"

namespace delta {

////////////////////////////////////////////////////////////////////////

class DeltaASTVisitor {
public:
	virtual bool VisitEnter (StmtsASTNode* node) = 0;
	virtual bool VisitLeave (StmtsASTNode* node) = 0;

	virtual bool VisitEnter (ExpressionListASTNode* node) = 0;
	virtual bool VisitLeave (ExpressionListASTNode* node) = 0;

	virtual bool VisitEnter (ArgListASTNode* node) = 0;
	virtual bool VisitLeave (ArgListASTNode* node) = 0;

	virtual bool VisitEnter (UnaryKwdASTNode* node) = 0;
	virtual bool VisitLeave (UnaryKwdASTNode* node) = 0;

	virtual bool Visit (LeafKwdASTNode* node) = 0;

	virtual bool VisitEnter (WhileASTNode* node) = 0;
	virtual bool VisitLeave (WhileASTNode* node) = 0;

	virtual bool VisitEnter (ForASTNode* node) = 0;
	virtual bool VisitLeave (ForASTNode* node) = 0;

	virtual bool VisitEnter (ForeachASTNode* node) = 0;
	virtual bool VisitLeave (ForeachASTNode* node) = 0;

	virtual bool VisitEnter (IfASTNode* node) = 0;
	virtual bool VisitLeave (IfASTNode* node) = 0;

	virtual bool VisitEnter (ElseASTNode* node) = 0;
	virtual bool VisitLeave (ElseASTNode* node) = 0;

	virtual bool VisitEnter (CompoundASTNode* node) = 0;
	virtual bool VisitLeave (CompoundASTNode* node) = 0;

	virtual bool VisitEnter (TryASTNode* node) = 0;
	virtual bool VisitLeave (TryASTNode* node) = 0;

	virtual bool VisitEnter (TrapASTNode* node) = 0;
	virtual bool VisitLeave (TrapASTNode* node) = 0;

	virtual bool VisitEnter (FunctionASTNode* node) = 0;
	virtual bool VisitLeave (FunctionASTNode* node) = 0;

	virtual bool Visit (FunctionNameASTNode* node) = 0;

	virtual bool VisitEnter (TernaryOpASTNode* node) = 0;
	virtual bool VisitLeave (TernaryOpASTNode* node) = 0;

	virtual bool VisitEnter (PrefixOpASTNode* node) = 0;
	virtual bool VisitLeave (PrefixOpASTNode* node) = 0;

	virtual bool VisitEnter (SuffixOpASTNode* node) = 0;
	virtual bool VisitLeave (SuffixOpASTNode* node) = 0;

	virtual bool VisitEnter (BinaryOpASTNode* node) = 0;
	virtual bool VisitLeave (BinaryOpASTNode* node) = 0;

	virtual bool VisitEnter (UnaryOpASTNode* node) = 0;
	virtual bool VisitLeave (UnaryOpASTNode* node) = 0;

	virtual bool VisitEnter (CallASTNode* node) = 0;
	virtual bool VisitLeave (CallASTNode* node) = 0;

	virtual bool Visit (StringifiedIdASTNode* node) = 0;
	virtual bool Visit (StdLibfuncASTNode* node) = 0;
	virtual bool Visit (LibfuncASTNode* node) = 0;
	virtual bool Visit (AttributeASTNode* node) = 0;
	virtual bool Visit (VariableASTNode* node) = 0;
	virtual bool Visit (ConstASTNode* node) = 0;
	virtual bool Visit (ArgASTNode* node) = 0;

	virtual bool VisitEnter (NewAttributeASTNode* node) = 0;
	virtual bool VisitLeave (NewAttributeASTNode* node) = 0;

	virtual bool VisitEnter (TableElemASTNode* node) = 0;
	virtual bool VisitLeave (TableElemASTNode* node) = 0;

	virtual bool VisitEnter (TableElemsASTNode* node) = 0;
	virtual bool VisitLeave (TableElemsASTNode* node) = 0;

	virtual bool VisitEnter (TableIndexListASTNode* node) = 0;
	virtual bool VisitLeave (TableIndexListASTNode* node) = 0;

	virtual bool VisitEnter (TableConstructASTNode* node) = 0;
	virtual bool VisitLeave (TableConstructASTNode* node) = 0;

	virtual bool Visit (TableConstKeyASTNode* node) = 0;

	virtual bool VisitEnter (OtherStmtASTNode* node) = 0;
	virtual bool VisitLeave (OtherStmtASTNode* node) = 0;

	virtual bool VisitEnter (UsingASTNode* node) = 0;
	virtual bool VisitLeave (UsingASTNode* node) = 0;

	virtual bool VisitEnter (QuotedElementsASTNode* node) = 0;
	virtual bool VisitLeave (QuotedElementsASTNode* node) = 0;
};

////////////////////////////////////////////////////////////////////////

class DeltaASTNodeVisitor {
public:
	virtual void Visit (StmtsASTNode* node) = 0;
	virtual void Visit (ExpressionListASTNode* node) = 0;
	virtual void Visit (ArgListASTNode* node) = 0;
	virtual void Visit (UnaryKwdASTNode* node) = 0;
	virtual void Visit (LeafKwdASTNode* node) = 0;
	virtual void Visit (WhileASTNode* node) = 0;
	virtual void Visit (ForASTNode* node) = 0;
	virtual void Visit (ForeachASTNode* node) = 0;
	virtual void Visit (IfASTNode* node) = 0;
	virtual void Visit (ElseASTNode* node) = 0;
	virtual void Visit (CompoundASTNode* node) = 0;
	virtual void Visit (TryASTNode* node) = 0;
	virtual void Visit (TrapASTNode* node) = 0;
	virtual void Visit (FunctionASTNode* node) = 0;
	virtual void Visit (FunctionNameASTNode* node) = 0;
	virtual void Visit (TernaryOpASTNode* node) = 0;
	virtual void Visit (PrefixOpASTNode* node) = 0;
	virtual void Visit (SuffixOpASTNode* node) = 0;
	virtual void Visit (BinaryOpASTNode* node) = 0;
	virtual void Visit (UnaryOpASTNode* node) = 0;
	virtual void Visit (CallASTNode* node) = 0;
	virtual void Visit (StringifiedIdASTNode* node) = 0;
	virtual void Visit (StdLibfuncASTNode* node) = 0;
	virtual void Visit (LibfuncASTNode* node) = 0;
	virtual void Visit (AttributeASTNode* node) = 0;
	virtual void Visit (VariableASTNode* node) = 0;
	virtual void Visit (ConstASTNode* node) = 0;
	virtual void Visit (ArgASTNode* node) = 0;
	virtual void Visit (TableElemASTNode* node) = 0;
	virtual void Visit (NewAttributeASTNode* node) = 0;
	virtual void Visit (TableElemsASTNode* node) = 0;
	virtual void Visit (TableIndexListASTNode* node) = 0;
	virtual void Visit (TableConstructASTNode* node) = 0;
	virtual void Visit (TableConstKeyASTNode* node) = 0;
	virtual void Visit (OtherStmtASTNode* node) = 0;
	virtual void Visit (UsingASTNode* node) = 0;
	virtual void Visit (QuotedElementsASTNode* node) = 0;
};

////////////////////////////////////////////////////////////////////////

class DeltaASTNullVisitor : public DeltaASTVisitor {
public:
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
	
	virtual bool VisitEnter (QuotedElementsASTNode* node);
	virtual bool VisitLeave (QuotedElementsASTNode* node);
};

////////////////////////////////////////////////////////////////////////

class DeltaASTNullNodeVisitor : public DeltaASTNodeVisitor {
public:
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
	virtual void Visit (QuotedElementsASTNode* node);
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTVISITOR_H

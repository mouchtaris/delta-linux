/**
 *	DeltaASTPrinter.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTPRINTER_H
#define DELTA_DELTAASTPRINTER_H

#include "Common.h"
#include "DeltaASTVisitor.h"

namespace delta {

////////////////////////////////////////////////////////////////////////

class DeltaASTPrinter : public DeltaASTNodeVisitor {
public:
	const String& GetDescription (void) const { return m_desc; }

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
	String m_desc;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTPRINTER_H

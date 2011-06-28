/**
 *	DeltaASTStyler.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTSTYLER_H
#define DELTA_DELTAASTSTYLER_H

#include "Common.h"
#include "DeltaParser/DeltaASTVisitor.h"

namespace editor {
	class EditorWindow;
}

namespace delta {

////////////////////////////////////////////////////////////////////////

class DeltaASTStyler : public DeltaASTVisitor {
public:
	DeltaASTStyler (editor::EditorWindow* window, uint start, uint end);
	~DeltaASTStyler (void);

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
	inline void highlight (DeltaASTNode* node);
	inline void highlight (uint start, uint len, DeltaASTNode* node);
	inline void highlight (uint start, uint len, int style);

	inline void fold (DeltaASTNode* node);

	editor::EditorWindow*	m_window;
	uint					m_foldLevel;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTSTYLER_H

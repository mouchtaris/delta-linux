/**
 *	DeltaASTPrinter.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaASTPrinter.h"
#include "Streams.h"

namespace delta {

////////////////////////////////////////////////////////////////////////
// DeltaASTPrinter class
//
#define IMPLEMENT_VISIT(type, str)	void DeltaASTPrinter::Visit (type* node) { m_desc = _(str); }

IMPLEMENT_VISIT(StmtsASTNode, "Collection of statements")
IMPLEMENT_VISIT(ExpressionListASTNode, "Expression list")
IMPLEMENT_VISIT(ArgListASTNode, "Argument list")
IMPLEMENT_VISIT(UnaryKwdASTNode, "Unary keyword")
IMPLEMENT_VISIT(LeafKwdASTNode, "Keyword")
IMPLEMENT_VISIT(WhileASTNode, "While statement")
IMPLEMENT_VISIT(ForASTNode, "For statement")
IMPLEMENT_VISIT(ForeachASTNode, "Foreach statement")
IMPLEMENT_VISIT(IfASTNode, "If statement")
IMPLEMENT_VISIT(ElseASTNode, "Else statement")
IMPLEMENT_VISIT(CompoundASTNode, "Block of statements")
IMPLEMENT_VISIT(TryASTNode, "Try statement")
IMPLEMENT_VISIT(TrapASTNode, "Trap statement")
IMPLEMENT_VISIT(FunctionASTNode, "Function definition")
IMPLEMENT_VISIT(FunctionNameASTNode, "The function's name")
IMPLEMENT_VISIT(TernaryOpASTNode, "Ternary operator expression")
IMPLEMENT_VISIT(PrefixOpASTNode, "Prefix operator expression")
IMPLEMENT_VISIT(SuffixOpASTNode, "Suffix operator expression")
IMPLEMENT_VISIT(BinaryOpASTNode, "Binary operator expression")
IMPLEMENT_VISIT(UnaryOpASTNode, "Unary operator expression")
IMPLEMENT_VISIT(CallASTNode, "Call expression")
IMPLEMENT_VISIT(StringifiedIdASTNode, "Stringified id")
IMPLEMENT_VISIT(StdLibfuncASTNode, "Std library function")
IMPLEMENT_VISIT(LibfuncASTNode, "Library function")
IMPLEMENT_VISIT(AttributeASTNode, "Attribute")
IMPLEMENT_VISIT(VariableASTNode, "Variable or program function")
IMPLEMENT_VISIT(ConstASTNode, "Constant expression")
IMPLEMENT_VISIT(ArgASTNode, "Argument")
IMPLEMENT_VISIT(NewAttributeASTNode, "Attribute construction expression")
IMPLEMENT_VISIT(TableElemASTNode, "Table element")
IMPLEMENT_VISIT(TableElemsASTNode, "List of table elements")
IMPLEMENT_VISIT(TableIndexListASTNode, "Table index list")
IMPLEMENT_VISIT(TableConstructASTNode, "Table construction expression")
IMPLEMENT_VISIT(TableConstKeyASTNode, "Constant key of table")
IMPLEMENT_VISIT(OtherStmtASTNode, "Other statement")
IMPLEMENT_VISIT(UsingASTNode, "Using")
IMPLEMENT_VISIT(QuotedElementsASTNode, "Quoted element list ")

////////////////////////////////////////////////////////////////////////

} // namespace delta

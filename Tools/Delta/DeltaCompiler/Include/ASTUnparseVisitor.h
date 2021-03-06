// ASTUnparseVisitor.h
// A tree visitor to unaparse an AST (not precise unparsing,
// as white space is not stored inside our ASTs.
// ScriptFighter Project.
// A. Savidis, September 2009.
// Added ast to source line mappings (although tightly
// coupled on the unparse logic in Unparse.cpp)
// Y. Lilis, September 2011.
//

#ifndef	ASTUNPARSEVISITOR_H
#define	ASTUNPARSEVISITOR_H

#include "TreeVisitor.h"
#include "ASTNode.h"
#include "ASTEvaluationHelpers.h"
#include "ASTTags.h"
#include "ustrings.h"
#include <map>
#include <set>
#include <list>
#include <string>

/////////////////////////////////////////////////////////

namespace AST {

class DCOMPLIB_CLASS UnparseVisitor : public TreeVisitor {

	public:
	typedef std::map<util_ui32, std::set<util_ui32> > LineMappings;

	private:
	static void Handle_Program (AST_VISITOR_ARGS);
	static void Handle_Stmts (AST_VISITOR_ARGS);
	static void Handle_Name (AST_VISITOR_ARGS);
	static void Handle_Function (AST_VISITOR_ARGS);
	static void Handle_LambdaFunction (AST_VISITOR_ARGS);
	static void Handle_FunctionStmt (AST_VISITOR_ARGS);
	static void Handle_FormalArgs (AST_VISITOR_ARGS);
	static void Handle_Compound (AST_VISITOR_ARGS);
	static void Handle_ExpressionListStmt (AST_VISITOR_ARGS);
	static void Handle_EmptyStmt (AST_VISITOR_ARGS);
	static void Handle_AssertStmt (AST_VISITOR_ARGS);
	static void Handle_Return (AST_VISITOR_ARGS);
	static void Handle_UsingNamespace (AST_VISITOR_ARGS);	
	static void Handle_UsingByteCodeLibrary (AST_VISITOR_ARGS);	
	static void Handle_ConstDefExpression (AST_VISITOR_ARGS);
	static void Handle_AssignExpr (AST_VISITOR_ARGS);
	static void Handle_AssignArithExpr (AST_VISITOR_ARGS);
	static void Handle_RelationalExpr (AST_VISITOR_ARGS);
	static void Handle_ExprANDExpr (AST_VISITOR_ARGS);
	static void Handle_ExprORExpr (AST_VISITOR_ARGS);
	static void Handle_ArithmeticExpression (AST_VISITOR_ARGS);
	static void Handle_NOTExpression (AST_VISITOR_ARGS);	
	static void Handle_UMINUSExpression (AST_VISITOR_ARGS);
	static void Handle_TermLvalueArith (AST_VISITOR_ARGS);
	static void Handle_Ternary (AST_VISITOR_ARGS);
	static void Handle_ParenthesisedExpr (AST_VISITOR_ARGS);
	static void Handle_ExprList (AST_VISITOR_ARGS);
	static void Handle_NumConst (AST_VISITOR_ARGS);
	static void Handle_NilConst (AST_VISITOR_ARGS);
	static void Handle_BoolConst (AST_VISITOR_ARGS);
	static void Handle_StringConst (AST_VISITOR_ARGS);
	static void Handle_StringifyDottedIdents (AST_VISITOR_ARGS);
	static void Handle_StringifyNamespaceIdent (AST_VISITOR_ARGS);
	static void Handle_NamespaceLvalue (AST_VISITOR_ARGS);
	static void Handle_SELF (AST_VISITOR_ARGS);
	static void Handle_ARGUMENTS (AST_VISITOR_ARGS);
	static void Handle_TRIPLE_DOT (AST_VISITOR_ARGS);
	static void Handle_LAMBDA_REF (AST_VISITOR_ARGS);	
	static void Handle_NEWSELF (AST_VISITOR_ARGS);
	static void Handle_BREAK (AST_VISITOR_ARGS);
	static void Handle_CONTINUE (AST_VISITOR_ARGS);
	static void Handle_FunctionParenthesisForm (AST_VISITOR_ARGS);
	static void Handle_LateBoundArgument (AST_VISITOR_ARGS);
	static void Handle_FunctionCall (AST_VISITOR_ARGS);
	static void Handle_Lvalue (AST_VISITOR_ARGS);	
	static void Handle_AttrLvalue (AST_VISITOR_ARGS);
	static void Handle_StaticLvalue (AST_VISITOR_ARGS);
	static void Handle_LocalLvalue (AST_VISITOR_ARGS);
	static void Handle_DotIndexIdent (AST_VISITOR_ARGS);
	static void Handle_DotIndexOpString (AST_VISITOR_ARGS);
	static void Handle_DotIndexStringConst (AST_VISITOR_ARGS);
	static void Handle_BracketIndexOperatorMethod (AST_VISITOR_ARGS);
	static void Handle_TableContentDot (AST_VISITOR_ARGS);
	static void Handle_BoundedTableContentDot (AST_VISITOR_ARGS);
	static void Handle_TableContentBracket (AST_VISITOR_ARGS);
	static void Handle_BoundedTableContentBracket (AST_VISITOR_ARGS);	
	static void Handle_TableConstructor (AST_VISITOR_ARGS);
	static void Handle_IdentIndexElement (AST_VISITOR_ARGS);	
	static void Handle_UnindexedValue (AST_VISITOR_ARGS);	
	static void Handle_FunctionExpr (AST_VISITOR_ARGS);	
	static void Handle_DottedIdent (AST_VISITOR_ARGS);
	static void Handle_OperatorIndex (AST_VISITOR_ARGS);
	static void Handle_TableValues (AST_VISITOR_ARGS);
	static void Handle_TableIndices (AST_VISITOR_ARGS);
	static void Handle_IndexedValues (AST_VISITOR_ARGS);
	static void Handle_NewAttribute (AST_VISITOR_ARGS);
	static void Handle_IfStmt (AST_VISITOR_ARGS);
	static void Handle_IfElseStmt (AST_VISITOR_ARGS);
	static void Handle_WhileStmt (AST_VISITOR_ARGS);
	static void Handle_ForStmt (AST_VISITOR_ARGS);
	static void Handle_ForInit (AST_VISITOR_ARGS);
	static void Handle_ForeachStmt (AST_VISITOR_ARGS);
	static void Handle_THROW (AST_VISITOR_ARGS);
	static void Handle_ExceptionVar (AST_VISITOR_ARGS);
	static void Handle_Exception (AST_VISITOR_ARGS);
	static void Handle_Escape (AST_VISITOR_ARGS);
	static void Handle_QuotedElements (AST_VISITOR_ARGS);
	static void Handle_QuasiQuotes (AST_VISITOR_ARGS);
	static void Handle_Inline (AST_VISITOR_ARGS);
	static void Handle_Execute (AST_VISITOR_ARGS);
	static void Handle_IncLineOnExit (AST_VISITOR_ARGS);

	static bool IsEscapeIdent (const TreeNode* node);

	/////////////////////////////////////////////////////////

	struct IsUndefFunctor {
		bool operator()(const std::string& s) const
			{ return false; }
	};
	
	typedef AST::EvaluationStack<std::string, IsUndefFunctor> EvaluationStack;

	util_ui32			currline;
	LineMappings		lineMappings;
	EvaluationStack		evalStack;
	util_ui16			tabs;

	EvaluationStack&	GetEvalStack	(void) { return evalStack; }
	const std::string	TabStops		(void) const { return utabstops(tabs); }
	util_ui16			TotalTabStops	(void) const { return tabs; }
	void				IncTabStops		(void) { ++tabs; }
	void				DecTabStops		(void) { --tabs; }
	
	void				IncLine			(void) { ++currline; }
	void				SetLineMapping	(util_ui32 astLine);

	const std::string	UnparseBinary(const TreeNode* node, const std::string leftTag, util_ui32 op, const std::string rightTag);

	public:
	const std::string	operator()(AST::Node* node);
	const LineMappings&	GetLineMappings (void) const { return lineMappings; }

	const std::string	Unparse (const TreeNode* node);	//for incomplete or partial TreeNodes (not necessarilly AST::Nodes)

	UnparseVisitor (void);
	virtual ~UnparseVisitor(){}
};

}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

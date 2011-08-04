// ASTTranslationVisitor.h
// Tree visitor for translation to intermediate code.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#ifndef	ASTTRANSLATIONVISITOR_H
#define	ASTTRANSLATIONVISITOR_H

#include "TreeVisitor.h"
#include "ASTNode.h"
#include "ASTEvaluationHelpers.h"
#include "Symbol.h"
#include "Expr.h"
#include "ParseActions.h"
#include "uvector.h"
#include "ufunctors.h"
#include <string>

/////////////////////////////////////////////////////////

namespace AST {

	class TranslationVisitor : public TreeVisitor {

	private:

	/////////////////////////////////////////////////////////
	// Translation handlers.

	static void	Handle_Program (AST_VISITOR_ARGS); 
	static void Handle_Stmts (AST_VISITOR_ARGS); 
	static void Handle_Stmt (AST_VISITOR_ARGS);
	static void Handle_BasicStmt (AST_VISITOR_ARGS);
	static void Handle_FunctionBasic (AST_VISITOR_ARGS, const std::string (*unparser)(DeltaSymbol*, Stmt*)); 
	static void Handle_Function (AST_VISITOR_ARGS); 
	static void Handle_LambdaFunction (AST_VISITOR_ARGS); 
	static void Handle_LambdaStmt (AST_VISITOR_ARGS); 
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
	static void Handle_ExpressionANDOR (AST_VISITOR_ARGS); 
	static void Handle_ExprANDExpr (AST_VISITOR_ARGS); 
	static void Handle_ExprORExpr (AST_VISITOR_ARGS); 
	static void Handle_ArithmeticExpression (AST_VISITOR_ARGS); 
	static void Handle_NOTExpression (AST_VISITOR_ARGS);	
	static void Handle_UMINUSExpression (AST_VISITOR_ARGS); 
	static void Handle_AdaptBoolExpression (AST_VISITOR_ARGS);	
	static void Handle_TermLvalueArith (AST_VISITOR_ARGS); 
	static void Handle_PrimaryFunctionAndTableObject (AST_VISITOR_ARGS); 
	static void Handle_PrimaryConstValue (AST_VISITOR_ARGS); 
	static void Handle_TernaryCondition (AST_VISITOR_ARGS); 
	static void Handle_TernaryLeftExpr (AST_VISITOR_ARGS); 
	static void Handle_TernaryRightExpr (AST_VISITOR_ARGS); 
	static void Handle_Ternary (AST_VISITOR_ARGS); 
	static void Handle_Expr (AST_VISITOR_ARGS); 
	static void Handle_ParenthesisedExpr (AST_VISITOR_ARGS);
	static void Handle_ExprList (AST_VISITOR_ARGS); 
	static void Handle_ExpressionListItem (AST_VISITOR_ARGS); 
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
	static void Handle_Argument (AST_VISITOR_ARGS); 
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
	static void Handle_TableObject (AST_VISITOR_ARGS); 
	static void Handle_TableConstructor (AST_VISITOR_ARGS); 
	static void Handle_UnindexedValue (AST_VISITOR_ARGS);	
	static void Handle_IdentIndexElement (AST_VISITOR_ARGS);	
	static void Handle_FunctionExpression (AST_VISITOR_ARGS);
	static void Handle_DottedIdent (AST_VISITOR_ARGS); 
	static void Handle_OperatorIndex (AST_VISITOR_ARGS); 
	static void Handle_TableValues (AST_VISITOR_ARGS); 
	static void Handle_TableIndices (AST_VISITOR_ARGS); 
	static void Handle_IndexedValues (AST_VISITOR_ARGS); 
	static void Handle_NewAttribute (AST_VISITOR_ARGS); 
	static void Handle_Condition (AST_VISITOR_ARGS); 
	static void Handle_IfStmt (AST_VISITOR_ARGS); 
	static void Handle_IfElseStmt (AST_VISITOR_ARGS); 
	static void Handle_IfCondition (AST_VISITOR_ARGS); 
	static void Handle_ElsePrefix (AST_VISITOR_ARGS); 
	static void Handle_WhileStmt (AST_VISITOR_ARGS); 
	static void Handle_WhileCondition (AST_VISITOR_ARGS); 
	static void Handle_ForStmt (AST_VISITOR_ARGS); 
	static void Handle_ForInit (AST_VISITOR_ARGS); 
	static void Handle_ForCond (AST_VISITOR_ARGS); 
	static void Handle_ForSuffix (AST_VISITOR_ARGS); 
	static void Handle_ForeachStmt (AST_VISITOR_ARGS); 
	static void Handle_ForeachContainer (AST_VISITOR_ARGS); 
	static void Handle_THROW (AST_VISITOR_ARGS); 
	static void Handle_ExceptionVar (AST_VISITOR_ARGS); 
	static void Handle_Exception (AST_VISITOR_ARGS); 

	/////////////////////////////////////////////////////////
	// Translation helpers.

	static void Handle_BooleanExpression (
					AST_VISITOR_ARGS, 
					DeltaExpr* (*f)(DeltaExpr*, DeltaQuadAddress, DeltaExpr*),
					util_ui32 token
				);

	static void Handle_UnaryExpression (
					AST_VISITOR_ARGS, 
					DeltaExpr* (*f)(DeltaExpr*), 
					util_ui32 token, 
					const std::string& infix = ""
				);

	static void Handle_StringItems (
					AST_VISITOR_ARGS, 
					DeltaExpr* (*f)(const char*, DeltaExpr*), 
					void(*g)(DeltaExpr*)
				);

	static void Handle_KwdExpr (
					AST_VISITOR_ARGS, 
					DeltaExpr* (*f)(void), 
					util_ui32 token
				);
	
	static void Handle_BuiltInStmt (
					AST_VISITOR_ARGS, 
					Stmt* (*f)(void), 
					util_ui32 token
				);

	static void Handle_LvalueByName (
					AST_VISITOR_ARGS, 
					DeltaExpr* (*f)(const std::string& id),
					util_i32	token = -1	// no token
				);

	static void Handle_StringValue (AST_VISITOR_ARGS);

	static void Handle_TableContent (
					AST_VISITOR_ARGS, 
					DeltaExpr*	(*f)(DeltaExpr*, DeltaExpr*), 
					bool singleDot
				);

	/////////////////////////////////////////////////////////
	// Evaluation stack value.

	class StackValue {
		
		public:
		enum Type { 
			expr_t		= 0, 
			sym_t		= 1, 
			elems_t		= 2, 
			quadno_t	= 3, 
			stmt_t		= 4, 
			undef_t		= 5
		};
		
		private:
		Type						type;
		union {
			DeltaExpr*				expr;
			DeltaSymbol*			sym;
			TableElements*			elems;
			DeltaQuadAddress		quadNo;
			Stmt*					stmt;
		} data;
		
		public:
		UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(StackValue)

		bool					operator==(const StackValue& v) const	
									{ return !memcmp(this, &v, sizeof(StackValue)); }
		bool					operator!=(const StackValue& v) const	
									{ return !(*this == v); }
		
		Type					GetType (void) const	
									{ return type; }

		DeltaExpr*				expr (void)						
									{ DASSERT(type == expr_t);	return data.expr; }
		DeltaSymbol*			sym (void)						
									{ DASSERT(type == sym_t); return data.sym; }
		TableElements*			elems (void)					
									{ DASSERT(type == elems_t); return data.elems; }
		DeltaQuadAddress		quadNo (void)					
									{ DASSERT(type == quadno_t); return data.quadNo; }
		Stmt*					stmt (void)						
									{ DASSERT(type == stmt_t); return data.stmt; }
		
		DeltaExpr*				operator= (DeltaExpr*		p)	
									{ udestructor_invocation(this); new (this) StackValue(p); return p; }
		DeltaSymbol*			operator= (DeltaSymbol*		p)	
									{ udestructor_invocation(this); new (this) StackValue(p); return p; }
		TableElements*			operator= (TableElements*	p)	
									{ udestructor_invocation(this); new (this) StackValue(p); return p; }
		DeltaQuadAddress		operator= (DeltaQuadAddress	p)	
									{ udestructor_invocation(this); new (this) StackValue(p); return p; }
		Stmt*					operator= (Stmt*			p)	
									{ udestructor_invocation(this); new (this) StackValue(p); return p; }

		StackValue (DeltaExpr*			p)	: type(expr_t)	{ data.expr		= p; }
		StackValue (DeltaSymbol*		p)	: type(sym_t)	{ data.sym		= p; }
		StackValue (TableElements*		p)	: type(elems_t)	{ data.elems	= p; }
		StackValue (DeltaQuadAddress	p)	: type(quadno_t){ data.quadNo	= p; }
		StackValue (Stmt*				p)	: type(stmt_t)	{ data.stmt		= p; }

		StackValue (void) { memset(this, 0, sizeof(StackValue)); type = undef_t; }
		StackValue (const StackValue& v) { memcpy(this, &v, sizeof(StackValue)); }
	};

	/////////////////////////////////////////////////////////
	// Translation value stack.

	struct IsUndefFunctor {
		bool operator()(const StackValue& v) const
			{ return v.GetType() == StackValue::undef_t; }
	};
	
	typedef AST::EvaluationStack<StackValue, IsUndefFunctor> EvaluationStack;

	EvaluationStack		evalStack;
	EvaluationStack&	GetEvalStack (void) 
							{ return evalStack; }

	/////////////////////////////////////////////////////////

	public:
	void operator()(AST::Node* root);

	TranslationVisitor (void);
	~TranslationVisitor(){}
};

}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

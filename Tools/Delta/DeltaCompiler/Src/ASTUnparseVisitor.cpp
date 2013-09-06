// ASTUnparseVisitor.cpp
// A tree visitor to unaparse an AST (not precise unparsing,
// as white space is not stored inside our ASTs.
// ScriptFighter Project.
// A. Savidis, September 2009.
// Added ast to source line mappings (although tightly
// coupled on the unparse logic in Unparse.cpp)
// Y. Lilis, September 2011.
//
#include "ASTUnparseVisitor.h"
#include "ASTCreationActions.h"
#include "ASTMetaTags.h"
#include "Unparse.h"
#include "DeltaSyntax.h"
#include "Symbol.h"

#define VISITOR	DNULLCHECK((AST::UnparseVisitor*) closure)

#define	PRETTY_PRINT

#ifdef	PRETTY_PRINT

#define	yytabs						VISITOR->TabStops
#define	yyinctabs					VISITOR->IncTabStops
#define	yydectabs					VISITOR->DecTabStops
#define	yynumtabs					VISITOR->TotalTabStops
#define	yyincline					VISITOR->IncLine

#define	STMT_SEPARATION				std::string("\n")
#define	STMT_IDENTATION				yytabs()
#define UNPARSE_TABLE_ELEMENTS		Unparse_TableElementsPrettyPrint
#define	UNPARSE_COMPOUND(s)			Unparse_CompoundPrettyPrint(s, yynumtabs() - 1)
#define	UNPARSE_ELEMENTS()			UNPARSE_LIST(UNPARSE_TABLE_ELEMENTS, yytabs())
#define	UNPARSE_TABLE(s)			Unparse_TableConstructorPrettyPrint(s, yynumtabs() - 1)
#define	UNPARSE_FUNCTION(p,f,c)		Unparse_FunctionPrettyPrint(p, f, c, yynumtabs())
#define	UNPARSE_IF(c,s)				Unparse_IfPrettyPrint(c, s, yynumtabs())
#define	UNPARSE_IFELSE(c,is,es)		Unparse_IfElsePrettyPrint(c, is, es, yynumtabs())
#define	UNPARSE_WHILE(c,s)			Unparse_WhilePrettyPrint(c, s, yynumtabs())
#define	UNPARSE_FOREACH(p,s)		Unparse_ForeachStmtPrettyPrint(p, s, yynumtabs())
#define	UNPARSE_FOR(i,c,r,s)		Unparse_ForPrettyPrint(i, c, r, s, yynumtabs())
#define	UNPARSE_TRYTRAP(t,c,v)		Unparse_TryTrapPrettyPrint(t, c, v, yynumtabs())

#else

#define	yyinctabs()					
#define	yydectabs()	
#define	yyincline()					

#define	STMT_SEPARATION				std::string("")
#define	STMT_IDENTATION				std::string("")
#define UNPARSE_TABLE_ELEMENTS		Unparse_TableElements
#define	UNPARSE_COMPOUND(s)			Unparse_Compound(s)
#define	UNPARSE_ELEMENTS()			UNPARSE_LIST(UNPARSE_TABLE_ELEMENTS, "")
#define	UNPARSE_TABLE(s)			Unparse_TableConstructor(s)
#define	UNPARSE_FUNCTION(p,f,c)		Unparse_Function(p, f, c)
#define	UNPARSE_IF(c,s)				Unparse_If(c, s)
#define	UNPARSE_IFELSE(c,is,es)		Unparse_IfElse(c, is, es)
#define	UNPARSE_WHILE(c,s)			Unparse_While(c, s)
#define	UNPARSE_FOREACH(p,s)		Unparse_ForeachStmt(p, s)
#define	UNPARSE_FOR(i,c,r,s)		Unparse_For(i, c, r, s)
#define	UNPARSE_TRYTRAP(t,c,v)		Unparse_TryTrap(t, c, v)

#endif

///////////////////////////////////////////////////////////

#define	UNPARSE_UNARY_OP(token, infix)					\
	yysetline();										\
	if (!entering) {									\
		yyrule(1);										\
		yv = Unparse_UnaryOp(token, yy[1], infix);		\
	}

#define	UNPARSE_BINARY_OP(token)						\
	yysetline();										\
	if (!entering) {									\
		yyrule(2);										\
		yv = Unparse_BinaryOp(yy[1], token, yy[2]);		\
	}

#define	UNPARSE_BUILTIN_EXPR(kwd)						\
	yysetline();										\
	if (!entering)										\
		yypush(Unparse_BuiltIn(kwd))

#define	UNPARSE_BUILTIN_STMT(kwd)						\
	yysetline();										\
	if (!entering)										\
		yypush(Unparse_BuiltInStmt(kwd))

#define	UNPARSE_PUSH_STRING(e)							\
	yysetline();										\
	if (!entering)										\
		yypush(e)

#define	UNPARSE_TABLE_ELEM(f)							\
	yysetline();										\
	if (!entering) {									\
		yyrule(2);										\
		yv = f(yy[1], yy[2]);							\
	}

#define	UNPARSE_DECORATE_SINGLE(f)						\
	yysetline();										\
	if (!entering) {									\
		yyrule(1);										\
		yv = f(yy[1]);									\
	}

#define	UNPARSE_LIST(f, tabs)							\
		util_ui32 n	 = DPTR(node)->GetTotalChildren();	\
		yyrule(n);										\
		if (n == 0)										\
			yv = "";									\
		else {											\
			yv = tabs + yy[1];							\
			for (util_ui32 i = 2; i <= n; ++i)			\
				yv = f(yv, tabs + yy[i]);				\
		}												\

#define	UNPARSE_RETAIN()

///////////////////////////////////////////////////////////

#define Rule		AST::UnparseVisitor::EvaluationStack::TopRule
#define	EvalStack()	VISITOR->GetEvalStack()
#define	yysetline()	VISITOR->SetLineMapping(						\
						entering ?								\
						((AST::Node*)node)->GetStartLine() :		\
						((AST::Node*)node)->GetEndLine()			\
					)
#define	yyrule		Rule yy(EvalStack().GetValues());			\
					yy.Apply(__LINE__, node->GetTag().c_str());	\
					yy = Rule::RuleSize()
#define	yypush		EvalStack().Set(__LINE__, node->GetTag().c_str()).Push
#define	yv				yy[0]

///////////////////////////////////////////////////////////

#define	_H(name, tag)	\
			SetHandler(tag, &Handle_##name, this)

#define	_C(name, tag, id)	\
		SetContextDependentHandler(tag, id, &Handle_##name, this)

///////////////////////////////////////////////////////////

AST::UnparseVisitor::UnparseVisitor (void) : tabs(0) {

	// For some nodes with a single child we do not introduce an unparse function
	// if we retain the unparsed form.

	_H(Program,							AST_TAG_PROGRAM);
	_H(Stmts,							AST_TAG_STMTS);
	_H(IncLineOnExit,					AST_TAG_STMT);
	_H(Name,							AST_TAG_NAME);
	_H(Function,						AST_TAG_FUNCTION);
	_H(LambdaFunction,					AST_TAG_LAMBDA_FUNCTION);
	_H(FunctionStmt,					AST_TAG_FUNCTION_STMT);
	_H(FormalArgs,						AST_TAG_FORMAL_ARGS);
	_H(Compound,						AST_TAG_COMPOUND);
	_H(ExpressionListStmt,				AST_TAG_EXPRLIST_STMT);
	_H(EmptyStmt,						AST_TAG_EMPTY_STMT);
	_H(AssertStmt,						AST_TAG_ASSERT);
	_H(Return,							AST_TAG_RETURN);
	_H(UsingNamespace,					AST_TAG_USING_NAMESPACE);	
	_H(UsingByteCodeLibrary,			AST_TAG_USING_BYTECODE_LIBRARY);	
	_H(ConstDefExpression,				AST_TAG_CONSTDEF);
	_H(AssignExpr,						AST_TAG_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_ADD_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_SUB_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_MUL_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_DIV_ASSIGN);
	_H(AssignArithExpr,					AST_TAG_OP_MOD_ASSIGN);
	_H(RelationalExpr,					AST_TAG_OP_GT);
	_H(RelationalExpr,					AST_TAG_OP_LT);
	_H(RelationalExpr,					AST_TAG_OP_GE);
	_H(RelationalExpr,					AST_TAG_OP_LE);
	_H(RelationalExpr,					AST_TAG_OP_EQ);
	_H(RelationalExpr,					AST_TAG_OP_NE);
	_H(ExprANDExpr,						AST_TAG_AND);
	_H(ExprORExpr,						AST_TAG_OR);
	_H(ArithmeticExpression,			AST_TAG_OP_ADD);
	_H(ArithmeticExpression,			AST_TAG_OP_SUB);
	_H(ArithmeticExpression,			AST_TAG_OP_MUL);
	_H(ArithmeticExpression,			AST_TAG_OP_DIV);
	_H(ArithmeticExpression,			AST_TAG_OP_MOD);
	_H(NOTExpression,					AST_TAG_NOT);	
	_H(UMINUSExpression,				AST_TAG_UMINUS);
	_H(TermLvalueArith,					AST_LVALUE_PLUSPLUS);
	_H(TermLvalueArith,					AST_LVALUE_MINUSMINUS);
	_H(TermLvalueArith,					AST_PLUSPLUS_LVALUE);
	_H(TermLvalueArith,					AST_MINUSMINUS_LVALUE);
	_H(Ternary,							AST_TAG_TERNARY);
	_H(ParenthesisedExpr,				AST_TAG_PARENTHESISED_EXPR);
	_H(ExprList,						AST_TAG_EXPR_LIST);
	_H(NumConst, 						AST_TAG_NUM_CONST);
	_H(NilConst, 						AST_TAG_NIL);
	_H(BoolConst, 						AST_TAG_BOOL_CONST);
	_H(StringConst, 					AST_TAG_STRING_CONST);
	_H(StringifyDottedIdents, 			AST_TAG_STRINGIFY_DOTTED_IDENTS);
	_H(StringifyNamespaceIdent,			AST_TAG_STRINGIFY_NAMESPACE_IDENT);
	_H(NamespaceLvalue,					AST_TAG_LVALUE_NAMESPACE_IDENT);
	_H(SELF,							AST_TAG_SELF);
	_H(ARGUMENTS,						AST_TAG_ARGUMENTS);
	_H(TRIPLE_DOT,						AST_TAG_TRIPLE_DOT);	
	_H(LAMBDA_REF,						AST_TAG_LAMBDA_REF);	
	_H(NEWSELF,							AST_TAG_LVALUE_NEWSELF);
	_H(BREAK,							AST_TAG_BREAK);			
	_H(CONTINUE,						AST_TAG_CONTINUE);		
	_H(FunctionParenthesisForm,			AST_TAG_FUNCTION_PARENTHESIS);
	_H(LateBoundArgument,				AST_TAG_LATEBOUND_ARGUMENT);
	_H(FunctionCall,					AST_TAG_FUNCTION_CALL);
	_H(Lvalue,							AST_TAG_LVALUE_IDENT);
	_H(AttrLvalue,						AST_TAG_LVALUE_ATTRIBUTE);
	_H(StaticLvalue,					AST_TAG_LVALUE_STATIC_IDENT);
	_H(LocalLvalue,						AST_TAG_LVALUE_LOCAL_IDENT);
	_H(DotIndexIdent,					AST_TAG_DOTINDEX_IDENT);
	_H(DotIndexOpString,				AST_TAG_DOTINDEX_OPSTRING);
	_H(DotIndexStringConst,				AST_TAG_DOTINDEX_STRINGCONST);
	_H(BracketIndexOperatorMethod,		AST_TAG_BRACKETINDEX_OPMETHOD);
	_H(TableContentDot,					AST_TAG_TABLE_CONTENT_DOT);
	_H(BoundedTableContentDot,			AST_TAG_TABLE_CONTENT_DOUBLE_DOT);
	_H(TableContentBracket,				AST_TAG_TABLE_CONTENT_BRACKET);
	_H(BoundedTableContentBracket,		AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET);	
	_H(TableConstructor,				AST_TAG_TABLE_CONSTRUCTOR);
	_H(UnindexedValue,					AST_TAG_TABLE_UNINDEXED_ELEM);	
	_H(IdentIndexElement,				AST_TAG_TABLE_IDENTINDEX_ELEM);
	_H(FunctionExpr,					AST_TAG_FUNCTION_EXPR);
	_H(DottedIdent,						AST_TAG_TABLE_DOTTED_IDENT);
	_H(OperatorIndex,					AST_TAG_TABLE_OPERATOR_INDEX);
	_H(TableValues,						AST_TAG_TABLE_VALUES);
	_H(TableIndices,					AST_TAG_TABLE_INDICES);
	_H(IndexedValues,					AST_TAG_TABLE_INDEXED_VALUES);
	_H(NewAttribute,					AST_TAG_TABLE_NEW_ATTRIBUTE);
	_H(IfStmt,							AST_TAG_IF);
	_H(IfElseStmt,						AST_TAG_IF_ELSE);
	_H(WhileStmt,						AST_TAG_WHILE);
	_H(ForStmt,							AST_TAG_FOR);
	_H(ForeachStmt,						AST_TAG_FOREACH);
	_H(THROW,							AST_TAG_THROW);
	_H(ExceptionVar,					AST_TAG_EXCEPTION_VAR);
	_H(Exception,						AST_TAG_TRYTRAP);
	_H(Escape,							AST_TAG_ESCAPE);
	_H(QuotedElements,					AST_TAG_QUOTED_ELEMENTS);
	_H(QuasiQuotes,						AST_TAG_QUASI_QUOTES);
	_H(Inline,							AST_TAG_INLINE);
	_H(Execute,							AST_TAG_EXECUTE);

	_C(IncLineOnExit,	AST_TAG_IF,			AST_CHILD_COND);
	_C(IncLineOnExit,	AST_TAG_IF_ELSE,	AST_CHILD_COND);
	_C(IncLineOnExit,	AST_TAG_IF_ELSE,	AST_CHILD_IF);
	_C(IncLineOnExit,	AST_TAG_IF_ELSE,	AST_CHILD_ELSE);
	_C(IncLineOnExit,	AST_TAG_WHILE,		AST_CHILD_COND);
	_C(IncLineOnExit,	AST_TAG_FOR,		AST_CHILD_SUFFIX);
	_C(IncLineOnExit,	AST_TAG_FOREACH,	AST_CHILD_CONTAINER);
	_C(IncLineOnExit,	AST_TAG_TRYTRAP,	AST_CHILD_TRY);
	_C(IncLineOnExit,	AST_TAG_TRYTRAP,	AST_CHILD_VAR);
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::SetLineMapping (util_ui32 astLine) {
	if (astLine)
		lineMappings[astLine].insert(currline);
}

///////////////////////////////////////////////////////////

const std::string AST::UnparseVisitor::operator()(AST::Node* root) {

	std::string result;
	if (root) {
		currline = 1;
		GetEvalStack().Initialise();

		DPTR(root)->AcceptPreOrder(this);
		DASSERT(GetEvalStack().GetValues().size() == 1);	// Reduction to start symbol.

		result = GetEvalStack().Top();
		GetEvalStack().CleanUp();
	}
	return result;
}

///////////////////////////////////////////////////////////
// CodeDef*
//
void AST::UnparseVisitor::Handle_Program (AST_VISITOR_ARGS)
 { Handle_Stmts(AST_VISITOR_ACTUALS); }

// Stmt+
//
void AST::UnparseVisitor::Handle_Stmts (AST_VISITOR_ARGS) {
	if (!entering) {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n || node->GetTag() == AST_TAG_PROGRAM);
		yyrule(n);
		if (n) {
			yv = STMT_IDENTATION + yy[1];
			for (util_ui32 i = 2; i <= n; ++i)
				yv = Unparse_Stmts(yv, STMT_SEPARATION + STMT_IDENTATION + yy[i]);
		}
		else
			yv = "";
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_Name (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		std::string name = NAME(node);
		const TreeAttribute* attr;
		if ((attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_RENAME)) && DPTR(attr)->GetBool())
			name = Unparse_Token(META_RENAME) + name;
		yypush(name);
	}
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_Function (AST_VISITOR_ARGS) {
	if (entering)
		yysetline();
	else {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n == 2 || n == 3);
		yyrule(n);
		const std::string	name		= n == 2 ? "" : yy[1];
		const std::string	formals		= yy[n - 1];
		const std::string	compound	= yy[n];
		util_ui8			funcClass	= AST::GetFuncClass((AST::Node*) node);
		util_ui8			funcLinkage	= AST::GetFuncLinkage((AST::Node*) node);
		yv =	UNPARSE_FUNCTION(
					Unparse_FunctionPrefix(name, funcClass, funcLinkage == DELTA_FUNCTION_DEFAULT_EXPORT), 
					formals,
					compound
				);
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_FormalArgs (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		yyrule(n);
		std::list<std::string> formals;
		for (util_ui32 i = 1; i <= n; ++i)
			formals.push_back(yy[i]);
		yv = Unparse_FunctionFormals(formals);
		TreeNode* parent = DPTR(node)->GetParent();
		DASSERT(parent);
		if (DPTR(parent)->GetTag() == AST_TAG_FUNCTION)
			yyincline();
	}
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_LambdaFunction (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(2);
		yv = Unparse_LambdaFunction(yy[1], Unparse_LambdaStmt(yy[2]));
	}
} 

///////////////////////////////////////////////////////////
// Function
//
void AST::UnparseVisitor::Handle_FunctionStmt (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(1);
		yv = Unparse_FunctionDef(yy[1]);
	}
}

///////////////////////////////////////////////////////////
// [ Stmts ]
//
void AST::UnparseVisitor::Handle_Compound (AST_VISITOR_ARGS) {
	yysetline();
	util_ui32 n = DPTR(node)->GetTotalChildren();
	DASSERT(n == 1 || n == 0);
	if (entering) {
		if (n)
			yyincline();
		yyinctabs();
	}
	else {
		yyrule(n);
		if (!n)
			yv = Unparse_Compound("");
		else
			yv = UNPARSE_COMPOUND(yy[1]);
		yydectabs();
	}
} 

///////////////////////////////////////////////////////////
// ExprList
//
void AST::UnparseVisitor::Handle_ExpressionListStmt (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n);
		yyrule(n);
		std::list<std::string> l;
		for (util_ui32 i = 1; i <= n; ++i)
			l.push_back(yy[i]);
		yv = Unparse_ExprListStmt(Unparse_ExprList(l));
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_EmptyStmt (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_EmptyStmt()); } 

///////////////////////////////////////////////////////////
// ASSERT Expr
//
void AST::UnparseVisitor::Handle_AssertStmt (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(1);
		yv = Unparse_ExprStmt(ASSERT, yy[1]);
	}
}

///////////////////////////////////////////////////////////
// RETURN [ Expr ]
//
void AST::UnparseVisitor::Handle_Return (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		TreeNode* expr = DPTR(node)->GetChild(AST_CHILD_EXPR);
		util_ui32 n = expr ? 1 : 0;
		yyrule(n);
		yv = n ?	Unparse_ExprStmt(RETURN, yy[1], DPTR(expr)->GetTag() != AST_TAG_FUNCTION_EXPR) :
					Unparse_BuiltInStmt(RETURN);
	}
} 

///////////////////////////////////////////////////////////
// USING ( ID | NamespacePath :: ID )
//
void AST::UnparseVisitor::Handle_UsingNamespace (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n);
		yyrule(n);
		if (n == 1)
			yv = Unparse_UsingNamespace(yy[1]);
		else {
			std::list<std::string> namespacePath;
			for (util_ui32 i = 1; i <= n; ++i)
				namespacePath.push_back(yy[i]);
			yv = Unparse_UsingNamespace(namespacePath);
		}
	}
}	

///////////////////////////////////////////////////////////
// USING StringConst : ID
//
void AST::UnparseVisitor::Handle_UsingByteCodeLibrary (AST_VISITOR_ARGS)
	{ UNPARSE_DECORATE_SINGLE(Unparse_UsingByteCodeLibrary); }

///////////////////////////////////////////////////////////
// CONST ID = Expr
//
void AST::UnparseVisitor::Handle_ConstDefExpression (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(2);
		yv = Unparse_ConstDef(yy[1], yy[2]);
	}
}

///////////////////////////////////////////////////////////
// Lvalue = Expr

void AST::UnparseVisitor::Handle_AssignExpr (AST_VISITOR_ARGS) 
	{ UNPARSE_BINARY_OP(ASSIGN); }

///////////////////////////////////////////////////////////

static util_ui32 GetArithOpToken (const std::string& op) {
	if (op == AST_TAG_OP_ADD_ASSIGN)	return ADD_A;
	if (op == AST_TAG_OP_MUL_ASSIGN)	return MUL_A;
	if (op == AST_TAG_OP_SUB_ASSIGN)	return SUB_A;
	if (op == AST_TAG_OP_DIV_ASSIGN)	return DIV_A;
	if (op == AST_TAG_OP_MOD_ASSIGN)	return MOD_A;
	if (op == AST_TAG_OP_ADD)			return ADD;
	if (op == AST_TAG_OP_MUL)			return MUL;
	if (op == AST_TAG_OP_SUB)			return SUB;
	if (op == AST_TAG_OP_DIV)			return DIV;
	if (op == AST_TAG_OP_MOD)			return MOD;

	DASSERT(false);
	return 0;
}

static util_ui32 GetRelatOpToken (const std::string& op) {
	if (op == AST_TAG_OP_LT)			return LT;
	if (op == AST_TAG_OP_GT)			return GT;
	if (op == AST_TAG_OP_LE)			return LE;
	if (op == AST_TAG_OP_GE)			return GE;
	if (op == AST_TAG_OP_EQ)			return EQ;
	if (op == AST_TAG_OP_NE)			return NE;

	DASSERT(false);
	return 0;
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_AssignArithExpr (AST_VISITOR_ARGS) 
	{ UNPARSE_BINARY_OP(GetArithOpToken(node->GetTag())); }
 
void AST::UnparseVisitor::Handle_RelationalExpr (AST_VISITOR_ARGS)
	{ UNPARSE_BINARY_OP(GetRelatOpToken(node->GetTag())); }

void AST::UnparseVisitor::Handle_ExprANDExpr (AST_VISITOR_ARGS) 
	{ UNPARSE_BINARY_OP(AND); }

void AST::UnparseVisitor::Handle_ExprORExpr (AST_VISITOR_ARGS)
	{ UNPARSE_BINARY_OP(OR); }

void AST::UnparseVisitor::Handle_ArithmeticExpression (AST_VISITOR_ARGS)
	{ UNPARSE_BINARY_OP(GetArithOpToken(node->GetTag())); }

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_NOTExpression (AST_VISITOR_ARGS)
	{ UNPARSE_UNARY_OP(NOT, " "); }

void AST::UnparseVisitor::Handle_UMINUSExpression (AST_VISITOR_ARGS)
	{ UNPARSE_UNARY_OP(UMINUS, ""); }

///////////////////////////////////////////////////////////

#define	RESULT(token, after) \
	return std::pair<util_ui32, bool>(token, after)

static std::pair<util_ui32, bool> GetIncDecOpCode (const std::string& op) {
	if (op == AST_LVALUE_PLUSPLUS)		RESULT(PLUSPLUS,	true);
	if (op == AST_LVALUE_MINUSMINUS)	RESULT(MINUSMINUS,	true);
	if (op == AST_PLUSPLUS_LVALUE)		RESULT(PLUSPLUS,	false);
	if (op == AST_MINUSMINUS_LVALUE)	RESULT(MINUSMINUS,	false);

	DASSERT(false);
	return std::pair<util_ui32, bool>(0,false);
}

#undef	RESULT

void AST::UnparseVisitor::Handle_TermLvalueArith (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(1);
		std::pair<util_ui32, bool> op = GetIncDecOpCode(node->GetTag());
		yv = Unparse_UnaryIncDec(op.first, yy[1], op.second);
	}
}

///////////////////////////////////////////////////////////
// Expr ? Expr : Expr
//
void AST::UnparseVisitor::Handle_Ternary (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(3);
		yv = Unparse_TernaryOp(yy[1], yy[2], yy[3]);
	}
} 

///////////////////////////////////////////////////////////
// ( Expr )
//
void AST::UnparseVisitor::Handle_ParenthesisedExpr (AST_VISITOR_ARGS)
	{ UNPARSE_DECORATE_SINGLE(Unparse_ParenthesisedExpr); }

///////////////////////////////////////////////////////////
// [ Expr [ , Expr ]* ]
//
void AST::UnparseVisitor::Handle_ExprList (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		util_ui32 n	= DPTR(node)->GetTotalChildren();
		yyrule(n);
		std::list<std::string> l;
		for (util_ui32 i = 1; i <= n; ++i)
			l.push_back(yy[i]);
		yv = Unparse_ExprList(l);	
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_NumConst (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_Number(DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE)->GetDouble())); }

void AST::UnparseVisitor::Handle_NilConst (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_Nil()); }

void AST::UnparseVisitor::Handle_BoolConst (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_Bool(DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE)->GetBool())); }

///////////////////////////////////////////////////////////

static std::list<std::string>& GetStringList (TreeNode* node)
	{ return DPTR(node)->GetAttribute(AST_ATTRIBUTE_ITEMS)->GetStringList(); }
	
void AST::UnparseVisitor::Handle_StringConst (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_StringConst(GetStringList(node))); }

void AST::UnparseVisitor::Handle_StringifyDottedIdents (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_StringifyDottedIdents(GetStringList(node))); }

void AST::UnparseVisitor::Handle_StringifyNamespaceIdent (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n > 1);
		yyrule(n);
		std::list<std::string> namespacePath;
		for (util_ui32 i = 1; i <= n; ++i)
			namespacePath.push_back(yy[i]);
		yv = Unparse_StringifyNamespaceIdent(namespacePath);
	}
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_NamespaceLvalue (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n > 1);
		yyrule(n);
		std::list<std::string> namespacePath;
		for (util_ui32 i = 1; i < n; ++i)
			namespacePath.push_back(yy[i]);
		yv = Unparse_NamespaceIdent(namespacePath, yy[n]);
	}
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_SELF (AST_VISITOR_ARGS) 
	{ UNPARSE_BUILTIN_EXPR(SELF); } 

void AST::UnparseVisitor::Handle_ARGUMENTS (AST_VISITOR_ARGS)
	{ UNPARSE_BUILTIN_EXPR(ARGUMENTS); } 

void AST::UnparseVisitor::Handle_TRIPLE_DOT (AST_VISITOR_ARGS)
	{ UNPARSE_BUILTIN_EXPR(TRIPLE_DOT); } 

void AST::UnparseVisitor::Handle_LAMBDA_REF (AST_VISITOR_ARGS)
	{ UNPARSE_BUILTIN_EXPR(LAMBDA_REF); } 

void AST::UnparseVisitor::Handle_NEWSELF (AST_VISITOR_ARGS) 
	{ UNPARSE_BUILTIN_EXPR(NEWSELF); } 

void AST::UnparseVisitor::Handle_BREAK (AST_VISITOR_ARGS)
	{ UNPARSE_BUILTIN_STMT(BREAK); } 

void AST::UnparseVisitor::Handle_CONTINUE (AST_VISITOR_ARGS) 
	{ UNPARSE_BUILTIN_STMT(CONTINUE ); } 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_FunctionParenthesisForm (AST_VISITOR_ARGS) 
	{ UNPARSE_DECORATE_SINGLE(Unparse_FunctionParenthesisForm); }

void AST::UnparseVisitor::Handle_LateBoundArgument (AST_VISITOR_ARGS) 
	{ UNPARSE_DECORATE_SINGLE(Unparse_LateBound); }

// Expr ( Arguments )
void AST::UnparseVisitor::Handle_FunctionCall (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(2);
		yv = Unparse_FunctionCall(yy[1], yy[2]);
	}
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_Lvalue (AST_VISITOR_ARGS)
	{ UNPARSE_DECORATE_SINGLE(Unparse_Var); }

void AST::UnparseVisitor::Handle_AttrLvalue (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(1);
		yv = Unparse_Var(yy[1], ATTRIBUTE);
	}
}

void AST::UnparseVisitor::Handle_StaticLvalue (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(1);
		yv = Unparse_Var(yy[1], STATIC);
	}
}

void AST::UnparseVisitor::Handle_LocalLvalue (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(1);
		yv = Unparse_Var(yy[1], LOCAL);
	}
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_DotIndexIdent (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(VALUE_STRING(node)); }

void AST::UnparseVisitor::Handle_DotIndexOpString (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_OperatorIndex(VALUE_STRING(node))); }

void AST::UnparseVisitor::Handle_DotIndexStringConst (AST_VISITOR_ARGS) 
	{ UNPARSE_RETAIN(); }

void AST::UnparseVisitor::Handle_BracketIndexOperatorMethod (AST_VISITOR_ARGS) 
	{ UNPARSE_PUSH_STRING(Unparse_OperatorIndex(VALUE_STRING(node))); }

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_TableContentDot (AST_VISITOR_ARGS) 
	{ UNPARSE_TABLE_ELEM(Unparse_TableContentDot); } 

void AST::UnparseVisitor::Handle_BoundedTableContentDot (AST_VISITOR_ARGS) 
	{ UNPARSE_TABLE_ELEM(Unparse_TableContentDoubleDot); } 

void AST::UnparseVisitor::Handle_TableContentBracket (AST_VISITOR_ARGS)
	{ UNPARSE_TABLE_ELEM(Unparse_TableContentBracket); } 

void AST::UnparseVisitor::Handle_BoundedTableContentBracket (AST_VISITOR_ARGS)
	{ UNPARSE_TABLE_ELEM(Unparse_TableContentDoubleBracket); } 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_TableConstructor (AST_VISITOR_ARGS) {
	yysetline();
	if (entering) {
		if (DPTR(node)->GetTotalChildren())
			yyincline();
		yyinctabs();
	}
	else {
		UNPARSE_ELEMENTS();
		yv = UNPARSE_TABLE(yv);
		yydectabs();
	}
}

void AST::UnparseVisitor::Handle_IdentIndexElement (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(2);
		yv = Unparse_IdentIndexElement(yy[1], yy[2]);
		TreeNode* parent = DPTR(node)->GetParent();
		DASSERT(parent);
		if (DPTR(parent)->GetTag() == AST_TAG_TABLE_CONSTRUCTOR)
			yyincline();
	}
}

void AST::UnparseVisitor::Handle_UnindexedValue (AST_VISITOR_ARGS) {
	UNPARSE_DECORATE_SINGLE(Unparse_UnindexedValue);
	TreeNode* parent = DPTR(node)->GetParent();
	DASSERT(parent);
	if (DPTR(parent)->GetTag() == AST_TAG_TABLE_CONSTRUCTOR)
		Handle_IncLineOnExit(AST_VISITOR_ACTUALS);
}

void AST::UnparseVisitor::Handle_FunctionExpr (AST_VISITOR_ARGS)
	{ UNPARSE_RETAIN(); }

void AST::UnparseVisitor::Handle_DottedIdent (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_DottedIdent(NAME(node))); }

void AST::UnparseVisitor::Handle_OperatorIndex (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_OperatorIndex(VALUE_STRING(node))); }

void AST::UnparseVisitor::Handle_TableValues (AST_VISITOR_ARGS) 
	{ Handle_ExprList(AST_VISITOR_ACTUALS); } 

// ExprList
void AST::UnparseVisitor::Handle_TableIndices (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering)
		{ UNPARSE_LIST(Unparse_TableIndices, ""); }
} 

// Indices : Values
void AST::UnparseVisitor::Handle_IndexedValues (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(2);
		yv = Unparse_IndexedValues(yy[1], yy[2]);
		TreeNode* parent = DPTR(node)->GetParent();
		DASSERT(parent);
		if (DPTR(parent)->GetTag() == AST_TAG_TABLE_CONSTRUCTOR)
			yyincline();
	}
} 

// @ID SetExpr GetExpr
void AST::UnparseVisitor::Handle_NewAttribute (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(3);
		yv = Unparse_NewAttributeSet(yy[1], yy[2], yy[3]);
		TreeNode* parent = DPTR(node)->GetParent();
		DASSERT(parent);
		if (DPTR(parent)->GetTag() == AST_TAG_TABLE_CONSTRUCTOR)
			yyincline();
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_IfStmt (AST_VISITOR_ARGS) {
	if (entering) {
		yysetline();
		yyinctabs();
	}
	else {
		yyrule(2);
		yv = UNPARSE_IF(yy[1], yy[2]);
		yydectabs();
	}
} 

void AST::UnparseVisitor::Handle_IfElseStmt (AST_VISITOR_ARGS) {
	if (entering) {
		yysetline();
		yyinctabs();
	}
	else {
		yyrule(3);
		yv = UNPARSE_IFELSE(yy[1], yy[2], yy[3]);
		yydectabs();
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_WhileStmt (AST_VISITOR_ARGS) {
	yysetline();
	if (entering)
		yyinctabs();
	else {
		yyrule(2);
		yv = UNPARSE_WHILE(yy[1], yy[2]);
		yydectabs();
	}
} 

// FOR (InitList Expr RepeatList) Stmt
void AST::UnparseVisitor::Handle_ForStmt (AST_VISITOR_ARGS) {
	yysetline();
	if (entering)
		yyinctabs();
	else {
		yyrule(4);
		yv = UNPARSE_FOR(yy[1], yy[2], yy[3], yy[4]);
		yydectabs();
	}
} 

void AST::UnparseVisitor::Handle_ForInit (AST_VISITOR_ARGS) 
	{ UNPARSE_DECORATE_SINGLE(Unparse_ForInitList); }

// FOREACH (Lvalue, Expr) Stmt | (Lvalue : Lvalue, Expr, Stmt)

void AST::UnparseVisitor::Handle_ForeachStmt (AST_VISITOR_ARGS) {
	yysetline();
	if (entering)
		yyinctabs();
	else {
		#define	VALUE_CONTAINER_FORM		3
		#define	INDEX_VALUE_CONTAINER_FORM	4
		#define	STMT_POS					n
		#define	CONTAINER_POS				(n - 1)
		#define	VALUE_POS					(n == VALUE_CONTAINER_FORM ? 1 : 2)
		#define	INDEX_POS					1

		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n == VALUE_CONTAINER_FORM || n == INDEX_VALUE_CONTAINER_FORM);
		yyrule(n);

		yv =	UNPARSE_FOREACH(
					Unparse_ForeachPrefix(
						yy[VALUE_POS], 
						n == VALUE_CONTAINER_FORM ? "" : yy[INDEX_POS],
						yy[CONTAINER_POS]
					),
					yy[STMT_POS]
				);

		yydectabs();

		#undef	VALUE_CONTAINER_FORM		
		#undef	INDEX_VALUE_CONTAINER_FORM	
		#undef	STMT_POS					
		#undef	CONTAINER_POS				
		#undef	VALUE_POS					
		#undef	INDEX_POS
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_THROW (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(1);
		yv = Unparse_ExprStmt(THROW, yy[1]);
	}
} 

void AST::UnparseVisitor::Handle_ExceptionVar (AST_VISITOR_ARGS) 
	{ UNPARSE_RETAIN(); } 

// TRY Stmt TRAP ID Stmt
void AST::UnparseVisitor::Handle_Exception (AST_VISITOR_ARGS) {
	yysetline();
	if (entering)		
		yyinctabs();
	else {
		yyrule(3);
		yv = UNPARSE_TRYTRAP(yy[1], yy[3], yy[2]);
		yydectabs();
	}
}

///////////////////////////////////////////////////////////
// Meta
//

bool AST::UnparseVisitor::IsEscapeIdent (const TreeNode* node) {
	DASSERT(node && DPTR(node)->GetTag() == AST_TAG_ESCAPE);
	if (const TreeNode* primary = DPTR(node)->GetChild(AST_CHILD_EXPR))
		if (DPTR(primary)->GetTag() == AST_TAG_PRIMARY_EXPRESSION)
			if (const TreeNode* ident = DPTR(primary)->GetChild(AST_CHILD_EXPR))
				if (DPTR(ident)->GetTag() == AST_TAG_LVALUE_IDENT)
					if (const TreeNode* name = DPTR(ident)->GetChild(AST_CHILD_NAME))
						if (DPTR(name)->GetTag() == AST_TAG_NAME)
							return true;
	return false;
}

void AST::UnparseVisitor::Handle_Escape (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		yyrule(1);
		util_ui32 cardinality = CARDINALITY(node);
		DASSERT(cardinality >= 1);
		yv = Unparse_Escape(cardinality, yy[1], IsEscapeIdent(node));
	}
	TreeNode* parent = node->GetParent();
	DASSERT(parent);
	if (DPTR(parent)->GetTag() == AST_TAG_TABLE_CONSTRUCTOR)
		Handle_IncLineOnExit(AST_VISITOR_ACTUALS);
}

void AST::UnparseVisitor::Handle_QuotedElements (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n);
		yyrule(n);
		yv = yy[1];
		for (util_ui32 i = 2; i <= n; ++i)
			yv = Unparse_QuotedElements(yv, yy[i]);
	}
}

void AST::UnparseVisitor::Handle_QuasiQuotes (AST_VISITOR_ARGS) {
	yysetline();
	TreeNode* child = DPTR(node)->GetChild(AST_CHILD_EXPR);
	DASSERT(child);
	bool isStmts = DPTR(child)->GetTag() == AST_TAG_STMTS;
	if (entering) {
		if (isStmts)
			yyincline();
		yyinctabs();
	}
	else {
		yyrule(1);
		yydectabs();
		std::string prefix, suffix;
		if (isStmts) {
			prefix = STMT_SEPARATION;
			suffix = STMT_SEPARATION + STMT_IDENTATION;
		}
		yv = Unparse_QuasiQuotes(prefix, yy[1], suffix);
	}
}

void AST::UnparseVisitor::Handle_Inline (AST_VISITOR_ARGS) {
	yysetline();
	if (!entering) {
		util_ui32 n = DPTR(node)->GetChild(AST_CHILD_EXPR) ? 1 : 0;
		yyrule(n);
		yv = Unparse_Inline(n ? yy[1] : "");
	}
	TreeNode* parent = node->GetParent();
	DASSERT(parent);
	if (DPTR(parent)->GetTag() == AST_TAG_TABLE_CONSTRUCTOR)
		Handle_IncLineOnExit(AST_VISITOR_ACTUALS);
}

void AST::UnparseVisitor::Handle_Execute (AST_VISITOR_ARGS)
	{ UNPARSE_UNARY_OP(META_EXECUTE, ""); }

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_IncLineOnExit (AST_VISITOR_ARGS) {
	if (!entering)
		yyincline();
}

///////////////////////////////////////////////////////////

#undef VISITOR
#define VISITOR this

static const std::string UnparseStringAttribute(const TreeNode* node, const std::string& attribute) {
	const TreeAttribute* attr;
	return	(attr = DPTR(node)->GetAttribute(attribute)) && DPTR(attr)->IsString() ? 
			DPTR(attr)->GetString() : "";
}

const std::string AST::UnparseVisitor::UnparseBinary(const TreeNode* node, const std::string leftTag, util_ui32 op, const std::string rightTag) {
	const std::string left	= Unparse(DPTR(node)->GetChild(leftTag));
	const std::string right	= Unparse(DPTR(node)->GetChild(rightTag));
	return Unparse_BinaryOp(left, op, right);
}

const std::string AST::UnparseVisitor::Unparse(const TreeNode* node) {
	std::string result;
	if (!node)
		return result;

	const TreeAttribute* attr;
	const std::string tag = DPTR(node)->GetTag();
	if (tag == AST_TAG_PROGRAM || tag == AST_TAG_STMTS) {
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		if (n)
			result = STMT_IDENTATION + Unparse(DPTR(node)->GetChild(0));		
		for (util_ui32 i = 1; i < n; ++i)
			result = Unparse_Stmts(result, STMT_SEPARATION + STMT_IDENTATION + Unparse(DPTR(node)->GetChild(i)));
	}
	else if (tag == AST_TAG_NAME) {
		if ((attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_RENAME)) && DPTR(attr)->IsBool() && DPTR(attr)->GetBool())
			result = Unparse_Token(META_RENAME);
		result += UnparseStringAttribute(node, AST_ATTRIBUTE_NAME);
	}
	else if (tag == AST_TAG_FUNCTION) {
		const std::string	name		= Unparse(DPTR(node)->GetChild(AST_CHILD_NAME));
		const std::string	formals		= Unparse(DPTR(node)->GetChild(AST_CHILD_FORMALS));
		const std::string	compound	= Unparse(DPTR(node)->GetChild(AST_CHILD_BODY));
		util_ui8			funcClass	= AST::GetFuncClass(UnparseStringAttribute(node, AST_ATTRIBUTE_CLASS));
		util_ui8			funcLinkage	= AST::GetFuncLinkage(UnparseStringAttribute(node, AST_ATTRIBUTE_LINKAGE));
		result = UNPARSE_FUNCTION(
			Unparse_FunctionPrefix(name, funcClass, funcLinkage == DELTA_FUNCTION_DEFAULT_EXPORT),
			formals,
			compound
		);
	}
	else if (tag == AST_TAG_FORMAL_ARGS) {
		std::list<std::string> formals;
		for (util_ui32 i = 0, n = DPTR(node)->GetTotalChildren(); i < n; ++i)
			formals.push_back(Unparse(DPTR(node)->GetChild(i)));
		result = Unparse_FunctionFormals(formals);
	}
	else if (tag == AST_TAG_LAMBDA_FUNCTION) {
		const std::string formals	= Unparse(DPTR(node)->GetChild(AST_CHILD_FORMALS));
		const std::string expr		= Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR));
		result = Unparse_LambdaFunction(formals, Unparse_LambdaStmt(expr));
	}
	else if (tag == AST_TAG_FUNCTION_STMT)
		result = Unparse_FunctionDef(Unparse(DPTR(node)->GetChild(AST_CHILD_FUNCTION)));
	else if (tag == AST_TAG_COMPOUND) {
		yyinctabs();
		if (const TreeNode* stmts = DPTR(node)->GetChild(AST_CHILD_STMTS))
			result = UNPARSE_COMPOUND(Unparse(DPTR(stmts)));
		else
			result = Unparse_Compound("");
		yydectabs();
	}
	else if (tag == AST_TAG_EXPRLIST_STMT) {
		std::list<std::string> l;
		for (util_ui32 i = 0, n = DPTR(node)->GetTotalChildren(); i < n; ++i)
			l.push_back(Unparse(DPTR(node)->GetChild(i)));
		result = Unparse_ExprListStmt(Unparse_ExprList(l));
	}
	else if (tag == AST_TAG_EMPTY_STMT)
		result = Unparse_EmptyStmt();
	else if (tag == AST_TAG_ASSERT)
		result = Unparse_ExprStmt(ASSERT, Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)));
	else if (tag == AST_TAG_RETURN) {
		const TreeNode* expr = DPTR(node)->GetChild(AST_CHILD_EXPR);
		result = expr ? Unparse_ExprStmt(RETURN, Unparse(DPTR(expr))) : Unparse_BuiltInStmt(RETURN);
	}
	else if (tag == AST_TAG_USING_NAMESPACE) {
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		if (n == 1)
			result = Unparse_UsingNamespace(Unparse(DPTR(node)->GetChild(0)));
		else {
			std::list<std::string> namespacePath;
			for (util_ui32 i = 0; i < n; ++i)
				namespacePath.push_back(Unparse(DPTR(node)->GetChild(i)));
			result = Unparse_UsingNamespace(namespacePath);
		}
	}
	else if (tag == AST_TAG_USING_BYTECODE_LIBRARY)
		result = Unparse_UsingByteCodeLibrary(Unparse(DPTR(node)->GetChild(AST_CHILD_NAME)));
	else if (tag == AST_TAG_CONSTDEF) {
		const std::string id	= Unparse(DPTR(node)->GetChild(AST_CHILD_NAME));
		const std::string expr	= Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR));
		result = Unparse_ConstDef(id, expr);
	}
	else if (tag == AST_TAG_ASSIGN)
		result = UnparseBinary(node, AST_CHILD_LVALUE, ASSIGN, AST_CHILD_RVALUE);
	else if (	tag == AST_TAG_OP_ADD_ASSIGN || tag == AST_TAG_OP_SUB_ASSIGN ||
				tag == AST_TAG_OP_MUL_ASSIGN || tag == AST_TAG_OP_DIV_ASSIGN || tag == AST_TAG_OP_MOD_ASSIGN)
		result = UnparseBinary(node, AST_CHILD_LVALUE, GetArithOpToken(tag), AST_CHILD_RVALUE);
	else if (	tag == AST_TAG_OP_GT	|| tag == AST_TAG_OP_LT		|| tag == AST_TAG_OP_GE		||
				tag == AST_TAG_OP_LE	|| tag == AST_TAG_OP_EQ		|| tag == AST_TAG_OP_NE)
		result = UnparseBinary(node, AST_CHILD_LEFT, GetRelatOpToken(tag), AST_CHILD_RIGHT);
	else if (tag == AST_TAG_AND)
		result = UnparseBinary(node, AST_CHILD_LEFT, AND, AST_CHILD_RIGHT);
	else if (tag == AST_TAG_OR)
		result = UnparseBinary(node, AST_CHILD_LEFT, OR, AST_CHILD_RIGHT);
	else if (tag == AST_TAG_OP_ADD || tag == AST_TAG_OP_SUB || tag == AST_TAG_OP_MUL || tag == AST_TAG_OP_DIV || tag == AST_TAG_OP_MOD)
		result = UnparseBinary(node, AST_CHILD_LEFT, GetArithOpToken(tag), AST_CHILD_RIGHT);
	else if (tag == AST_TAG_NOT)
		result = Unparse_UnaryOp(NOT, Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)), " ");
	else if (tag == AST_TAG_UMINUS)
		result = Unparse_UnaryOp(UMINUS, Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)), "");
	else if (tag == AST_LVALUE_PLUSPLUS || tag == AST_LVALUE_MINUSMINUS || tag == AST_PLUSPLUS_LVALUE || tag == AST_MINUSMINUS_LVALUE) {
		const std::pair<util_ui32, bool> op = GetIncDecOpCode(tag);
		result = Unparse_UnaryIncDec(op.first, AST_CHILD_LVALUE, op.second);
	}
	else if (tag == AST_TAG_TERNARY) {
		const std::string cond	= Unparse(DPTR(node)->GetChild(AST_CHILD_COND));
		const std::string left	= Unparse(DPTR(node)->GetChild(AST_CHILD_LEFT));
		const std::string right	= Unparse(DPTR(node)->GetChild(AST_CHILD_RIGHT));
		result = Unparse_TernaryOp(cond, left, right);
	}
	else if (tag == AST_TAG_PARENTHESISED_EXPR)
		result = Unparse_ParenthesisedExpr(Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)));
	else if (tag == AST_TAG_EXPR_LIST || tag == AST_TAG_TABLE_VALUES) {
		std::list<std::string> l;
		for (util_ui32 i = 0, n = DPTR(node)->GetTotalChildren(); i < n; ++i)
			l.push_back(Unparse(DPTR(node)->GetChild(i)));
		result = Unparse_ExprList(l);
	}
	else if (tag == AST_TAG_NUM_CONST) {
		if ((attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE)) && DPTR(attr)->IsDouble())
			result = Unparse_Number(DPTR(attr)->GetDouble());
	}
	else if (tag == AST_TAG_NIL)
		result = Unparse_Nil();
	else if (tag == AST_TAG_BOOL_CONST) {
		if ((attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE)) && DPTR(attr)->IsBool())
			result = Unparse_Bool(DPTR(attr)->GetBool());
	}
	else if (tag == AST_TAG_STRING_CONST) {
		if ((attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_ITEMS)) && DPTR(attr)->IsStringList())
			result = Unparse_StringConst(DPTR(attr)->GetStringList());
	}
	else if (tag == AST_TAG_STRINGIFY_DOTTED_IDENTS) {
		if ((attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_ITEMS)) && DPTR(attr)->IsStringList())
			result = Unparse_StringifyDottedIdents(DPTR(attr)->GetStringList());
	}
	else if (tag == AST_TAG_STRINGIFY_NAMESPACE_IDENT) {
		std::list<std::string> namespacePath;
		for (util_ui32 i = 0, n = DPTR(node)->GetTotalChildren(); i < n; ++i)
			namespacePath.push_back(Unparse(DPTR(node)->GetChild(i)));
		result = Unparse_StringifyNamespaceIdent(namespacePath);
	}
	else if (tag == AST_TAG_LVALUE_NAMESPACE_IDENT){
		std::list<std::string> namespacePath;
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		for (util_ui32 i = 0; i < n - 1; ++i)
			namespacePath.push_back(Unparse(DPTR(node)->GetChild(i)));
		result = Unparse_NamespaceIdent(namespacePath, Unparse(DPTR(node)->GetChild(n - 1)));
	}
	else if (tag == AST_TAG_SELF)
		result = Unparse_BuiltIn(SELF);
	else if (tag == AST_TAG_ARGUMENTS)
		result = Unparse_BuiltIn(ARGUMENTS);
	else if (tag == AST_TAG_TRIPLE_DOT)
		result = Unparse_BuiltIn(TRIPLE_DOT);
	else if (tag == AST_TAG_LAMBDA_REF)
		result = Unparse_BuiltIn(LAMBDA_REF);
	else if (tag == AST_TAG_LVALUE_NEWSELF)
		result = Unparse_BuiltIn(NEWSELF);
	else if (tag == AST_TAG_BREAK)
		result = Unparse_BuiltIn(BREAK);
	else if (tag == AST_TAG_CONTINUE)
		result = Unparse_BuiltIn(CONTINUE);
	else if (tag == AST_TAG_FUNCTION_PARENTHESIS)
		result = Unparse_FunctionParenthesisForm(Unparse(DPTR(node)->GetChild(AST_CHILD_FUNCTION)));
	else if (tag == AST_TAG_LATEBOUND_ARGUMENT)
		result = Unparse_LateBound(Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)));
	else if (tag == AST_TAG_FUNCTION_CALL) {
		const std::string func	= Unparse(DPTR(node)->GetChild(AST_CHILD_FUNCTION));
		const std::string args	= Unparse(DPTR(node)->GetChild(AST_CHILD_ACTUALS));
		result = Unparse_FunctionCall(func, args);
	}
	else if (tag == AST_TAG_LVALUE_IDENT)
		result = Unparse_Var(Unparse(DPTR(node)->GetChild(AST_CHILD_NAME)));
	else if (tag == AST_TAG_LVALUE_ATTRIBUTE)
		result = Unparse_Var(Unparse(DPTR(node)->GetChild(AST_CHILD_NAME)), ATTRIBUTE);
	else if (tag == AST_TAG_LVALUE_STATIC_IDENT)
		result = Unparse_Var(Unparse(DPTR(node)->GetChild(AST_CHILD_NAME)), STATIC);
	else if (tag == AST_TAG_LVALUE_LOCAL_IDENT)
		result = Unparse_Var(Unparse(DPTR(node)->GetChild(AST_CHILD_NAME)), LOCAL);
	else if (tag == AST_TAG_DOTINDEX_IDENT)
		result = UnparseStringAttribute(node, AST_ATTRIBUTE_VALUE);
	else if (tag == AST_TAG_DOTINDEX_OPSTRING || tag == AST_TAG_BRACKETINDEX_OPMETHOD)
		result = UnparseStringAttribute(node, AST_ATTRIBUTE_VALUE);
	else if (tag == AST_TAG_TABLE_CONTENT_DOT)
		result = Unparse_TableContentDot(Unparse(DPTR(node)->GetChild(AST_CHILD_TABLE)), Unparse(DPTR(node)->GetChild(AST_CHILD_INDEX)));
	else if (tag == AST_TAG_TABLE_CONTENT_DOUBLE_DOT)
		result = Unparse_TableContentDoubleDot(Unparse(DPTR(node)->GetChild(AST_CHILD_TABLE)), Unparse(DPTR(node)->GetChild(AST_CHILD_INDEX)));
	else if (tag == AST_TAG_TABLE_CONTENT_BRACKET)
		result = Unparse_TableContentBracket(Unparse(DPTR(node)->GetChild(AST_CHILD_TABLE)), Unparse(DPTR(node)->GetChild(AST_CHILD_INDEX)));
	else if (tag == AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET)
		result = Unparse_TableContentDoubleBracket(Unparse(DPTR(node)->GetChild(AST_CHILD_TABLE)), Unparse(DPTR(node)->GetChild(AST_CHILD_INDEX)));
	else if (tag == AST_TAG_TABLE_CONSTRUCTOR) {
		yyinctabs();
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		if (n)
			result = yytabs() + Unparse(DPTR(node)->GetChild(0));
		for (util_ui32 i = 1; i < n; ++i)
			result = UNPARSE_TABLE_ELEMENTS(result, yytabs() + Unparse(DPTR(node)->GetChild(i)));
		result = UNPARSE_TABLE(result);
		yydectabs();
	}
	else if (tag == AST_TAG_TABLE_IDENTINDEX_ELEM)
		result = Unparse_IdentIndexElement(Unparse(DPTR(node)->GetChild(AST_CHILD_NAME)), Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)));
	else if (tag == AST_TAG_TABLE_UNINDEXED_ELEM)
		result = Unparse_UnindexedValue(Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)));
	else if (tag == AST_TAG_TABLE_DOTTED_IDENT)
		result = Unparse_DottedIdent(UnparseStringAttribute(node, AST_ATTRIBUTE_NAME));
	else if (tag == AST_TAG_TABLE_OPERATOR_INDEX)
		result = Unparse_OperatorIndex(UnparseStringAttribute(node, AST_ATTRIBUTE_VALUE));
	else if (tag == AST_TAG_TABLE_INDICES) {
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		if (n)
			result = Unparse(DPTR(node)->GetChild(0));
		for (util_ui32 i = 1; i < n; ++i)
			result = Unparse_TableIndices(result, Unparse(DPTR(node)->GetChild(i)));
	}
	else if (tag == AST_TAG_TABLE_INDEXED_VALUES)
		result = Unparse_IndexedValues(Unparse(DPTR(node)->GetChild(AST_CHILD_INDICES)), Unparse(DPTR(node)->GetChild(AST_CHILD_VALUES)));
	else if (tag == AST_TAG_TABLE_NEW_ATTRIBUTE) {
		const std::string name	= Unparse(DPTR(node)->GetChild(AST_CHILD_NAME));
		const std::string set	= Unparse(DPTR(node)->GetChild(AST_CHILD_SET));
		const std::string get	= Unparse(DPTR(node)->GetChild(AST_CHILD_GET));
		result = Unparse_NewAttributeSet(name, set, get);
	}
	else if (tag == AST_TAG_IF) {
		yyinctabs();
		result = UNPARSE_IF(Unparse(DPTR(node)->GetChild(AST_CHILD_COND)), Unparse(DPTR(node)->GetChild(AST_CHILD_STMT)));
		yydectabs();
	}
	else if (tag == AST_TAG_IF_ELSE) {
		yyinctabs();
		const std::string cond		= Unparse(DPTR(node)->GetChild(AST_CHILD_COND));
		const std::string ifStmt	= Unparse(DPTR(node)->GetChild(AST_CHILD_IF));
		const std::string elseStmt	= Unparse(DPTR(node)->GetChild(AST_CHILD_ELSE));
		result = UNPARSE_IFELSE(cond, ifStmt, elseStmt);
		yydectabs();
	}
	else if (tag == AST_TAG_WHILE) {
		yyinctabs();
		result = UNPARSE_WHILE(Unparse(DPTR(node)->GetChild(AST_CHILD_COND)), Unparse(DPTR(node)->GetChild(AST_CHILD_STMT)));
		yydectabs();
	}
	else if (tag == AST_TAG_FOR) {
		yyinctabs();
		const std::string init	= Unparse(DPTR(node)->GetChild(AST_CHILD_INIT));
		const std::string cond	= Unparse(DPTR(node)->GetChild(AST_CHILD_COND));
		const std::string suffix= Unparse(DPTR(node)->GetChild(AST_CHILD_SUFFIX));
		const std::string stmt	= Unparse(DPTR(node)->GetChild(AST_CHILD_STMT));
		result = UNPARSE_FOR(init, cond, suffix, stmt);
		yydectabs();
	}
	else if (tag == AST_TAG_FOREACH) {
		yyinctabs();
		const std::string index		= Unparse(DPTR(node)->GetChild(AST_CHILD_INDEX));
		const std::string value		= Unparse(DPTR(node)->GetChild(AST_CHILD_LVALUE));
		const std::string container	= Unparse(DPTR(node)->GetChild(AST_CHILD_CONTAINER));
		const std::string stmt		= Unparse(DPTR(node)->GetChild(AST_CHILD_STMT));
		result = UNPARSE_FOREACH(Unparse_ForeachPrefix(value, index, container), stmt);
		yydectabs();
	}
	else if (tag == AST_TAG_THROW)
		result = Unparse_ExprStmt(THROW, Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)));
	else if (tag == AST_TAG_TRYTRAP) {
		yyinctabs();
		const std::string tryStmt	= Unparse(DPTR(node)->GetChild(AST_CHILD_TRY));
		const std::string var		= Unparse(DPTR(node)->GetChild(AST_CHILD_VAR));
		const std::string trapStmt	= Unparse(DPTR(node)->GetChild(AST_CHILD_TRAP));
		result = UNPARSE_TRYTRAP(tryStmt, trapStmt, var);
		yydectabs();
	}
	else if (tag == AST_TAG_ESCAPE) {
		if ((attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_CARDINALITY)) && DPTR(attr)->IsUInt())
			result = Unparse_Escape(DPTR(attr)->GetUInt(), Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)), IsEscapeIdent(node));
	}
	else if (tag == AST_TAG_QUOTED_ELEMENTS) {
		const util_ui32 n = DPTR(node)->GetTotalChildren();
		if (n)
			result = Unparse(DPTR(node)->GetChild(0));
		for (util_ui32 i = 1; i < n; ++i)
			result = Unparse_QuotedElements(result, Unparse(DPTR(node)->GetChild(i)));
	}
	else if (tag == AST_TAG_QUASI_QUOTES) {
		std::string prefix, suffix;
		if (const TreeNode* child = DPTR(node)->GetChild(AST_CHILD_EXPR)) {
			yyinctabs();
			result = Unparse(child);
			yydectabs();
			if (DPTR(child)->GetTag() == AST_TAG_STMTS) {
				prefix = STMT_SEPARATION;
				suffix = STMT_SEPARATION + STMT_IDENTATION;
			}
		}
		result = Unparse_QuasiQuotes(prefix, result, suffix);
	}
	else if (tag == AST_TAG_INLINE)
		result = Unparse_Inline(Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)));
	else if (tag == AST_TAG_EXECUTE)
		result = Unparse_UnaryOp(META_EXECUTE, Unparse(DPTR(node)->GetChild(AST_CHILD_EXPR)), "");
	else if (DPTR(node)->GetTotalChildren() == 1)
		result = Unparse(DPTR(node)->GetChild(0));
	else //else that would be an error in the given ast so just return an error indication
		result = "<error>";
	return result;
}
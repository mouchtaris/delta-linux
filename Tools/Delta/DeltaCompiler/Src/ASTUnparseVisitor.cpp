// ASTUnparseVisitor.cpp
// A tree visitor to unaparse an AST (not precise unparsing,
// as white space is not stored inside our ASTs.
// ScriptFighter Project.
// A. Savidis, September 2009.
//
#include "ASTUnparseVisitor.h"
#include "ASTCreationActions.h"
#include "Unparse.h"
#include "DeltaSyntax.h"
#include "Symbol.h"

#define	PRETTY_PRINT

#ifdef	PRETTY_PRINT

#define	yytabs		DNULLCHECK((AST::UnparseVisitor*) closure)->TabStops
#define	yyinctabs	DNULLCHECK((AST::UnparseVisitor*) closure)->IncTabStops
#define	yydectabs	DNULLCHECK((AST::UnparseVisitor*) closure)->DecTabStops
#define	yynumtabs	DNULLCHECK((AST::UnparseVisitor*) closure)->TotalTabStops

#define	STMT_SEPARATION				std::string("\n")
#define	STMT_IDENTATION				yytabs()
#define	UNPARSE_COMPOUND()			Unparse_CompoundPrettyPrint(yy[1], yynumtabs() - 1)
#define	UNPARSE_ELEMENTS()			UNPARSE_LIST(Unparse_TableElementsPrettyPrint, yytabs())
#define	UNPARSE_TABLE(s)			Unparse_TableConstructorPrettyPrint(s, yynumtabs() - 1)
#define	UNPARSE_FUNCTION(p,f,c)		Unparse_FunctionPrettyPrint(p, f, c, yynumtabs())
#define	UNPARSE_IF(c,s)				Unparse_IfPrettyPrint(c, s, yynumtabs())
#define	UNPARSE_IFELSE(c,is,es)		Unparse_IfElsePrettyPrint(c, is, es, yynumtabs())
#define	UNPARSE_WHILE(c,s)			Unparse_WhilePrettyPrint(c, s, yynumtabs())
#define	UNPARSE_FOREACH(p,s)		Unparse_ForeachStmtPrettyPrint(p, s, yynumtabs())
#define	UNPARSE_FOR(i,c,r,s)		Unparse_ForPrettyPrint(i, c, r, s, yynumtabs())
#define	UNPARSE_TRYTRAP(t,c,v)		Unparse_TryTrapPrettyPrint(t, c, v, yynumtabs())

#else

#define	STMT_SEPARATION				std::string("")
#define	STMT_IDENTATION				std::string("")
#define	UNPARSE_COMPOUND()			Unparse_Compound(yy[1])
#define	UNPARSE_ELEMENTS()			UNPARSE_LIST(Unparse_TableElements, "")
#define	UNPARSE_TABLE(s)			Unparse_TableConstructor(s)
#define	UNPARSE_FUNCTION(p,f,c)		Unparse_Function(p, f, c)
#define	UNPARSE_IF(c,s)				Unparse_If(c, s)
#define	UNPARSE_IFELSE(c,is,es)		Unparse_IfElse(c, is, es)
#define	UNPARSE_WHILE(c,s)			Unparse_While(c, s)
#define	UNPARSE_FOREACH(p,s)		Unparse_ForeachStmt(p, s)
#define	UNPARSE_FOR(i,c,r,s)		Unparse_For(i, c, r, s)
#define	UNPARSE_TRYTRAP(t,c,v)		Unparse_TryTrap(t, c, v)

#define	yyinctabs()					
#define	yydectabs()	

#endif

///////////////////////////////////////////////////////////

#define	UNPARSE_UNARY_OP(token, infix)					\
	if (!entering) {									\
		yyrule(1);										\
		yv = Unparse_UnaryOp(token, yy[1], infix);		\
	}

#define	UNPARSE_BINARY_OP(token)						\
	if (!entering) {									\
		yyrule(2);										\
		yv = Unparse_BinaryOp(yy[1], token, yy[2]);		\
	}

#define	UNPARSE_BUILTIN_EXPR(kwd)						\
	if (!entering) yypush(Unparse_BuiltIn(kwd))

#define	UNPARSE_BUILTIN_STMT(kwd)						\
	if (!entering) yypush(Unparse_BuiltInStmt(kwd))

#define	UNPARSE_PUSH_STRING(e)							\
	if (!entering) yypush(e)

#define	UNPARSE_TABLE_ELEM(f)							\
	if (!entering) {									\
		yyrule(2);										\
		yv = f(yy[1], yy[2]);							\
	}

#define	UNPARSE_DECORATE_SINGLE(f)						\
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
		}

#define	UNPARSE_RETAIN()

///////////////////////////////////////////////////////////

#define Rule		AST::UnparseVisitor::EvaluationStack::TopRule
#define	EvalStack()	DNULLCHECK((AST::UnparseVisitor*) closure)->GetEvalStack()
#define	yyrule		Rule yy(EvalStack().GetValues());			\
					yy.Apply(__LINE__, node->GetTag().c_str());	\
					yy = Rule::RuleSize()
#define	yypush		EvalStack().Set(__LINE__, node->GetTag().c_str()).Push
#define	yv			yy[0]

///////////////////////////////////////////////////////////

#define	_H(name, tag)	\
			SetHandler(tag, &Handle_##name, this)

AST::UnparseVisitor::UnparseVisitor (void) : tabs(0) {

	// For some nodes with a single child we do not introduce an unparse function
	// if we retain the unaprsed form.

	_H(Program,							AST_TAG_PROGRAM);
	_H(Stmts,							AST_TAG_STMTS);
	_H(Function,						AST_TAG_FUNCTION);
	_H(LambdaFunction,					AST_TAG_LAMBDA_FUNCTION);
	_H(FunctionStmt,					AST_TAG_FUNCTION_STMT);
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
	_H(IdentIndexElement,				AST_TAG_TABLE_INDENTINDEX_ELEM);
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
}

///////////////////////////////////////////////////////////

const std::string AST::UnparseVisitor::operator()(AST::Node* root) {

	if (root) {

		GetEvalStack().Initialise();

		DPTR(root)->AcceptPreOrder(this);
		DASSERT(GetEvalStack().GetValues().size() == 1);	// Reduction to start symbol.

		std::string result = GetEvalStack().Top();
		GetEvalStack().CleanUp();

		return result;
	}
	else
		return "";
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
		yyrule(n);
		DASSERT(n || node->GetTag() == AST_TAG_PROGRAM);
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

const std::string AST::UnparseVisitor::UparseFormals (TreeNode* func) {

	TreeNode* formals = DPTR(func)->GetChild(AST_CHILD_FORMALS);
	DASSERT(formals);

	std::string result;
	if (AST::IdList* idList = (AST::IdList*) DPTR(formals)->GetAttribute(AST_ATTRIBUTE_ITEMS))
		result = Unparse_FunctionFormals(DPTR(idList)->l);
	return result;
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_Function (AST_VISITOR_ARGS) { 
	if (!entering) {
		yyrule(1);
		util_ui8	funcClass	= AST::GetFuncClass((const char*) DPTR(node)->GetAttribute(AST_ATTRIBUTE_CLASS));
		util_ui8	funcLinkage	= AST::GetFuncLinkage((const char*) DPTR(node)->GetAttribute(AST_ATTRIBUTE_LINKAGE));
		const char*	name		= NAME(node);
		yv =	UNPARSE_FUNCTION(
					Unparse_FunctionPrefix(name ? name : "", funcClass, funcLinkage == DELTA_FUNCTION_DEFAULT_EXPORT), 
					UparseFormals(node), 
					yy[1]
				);
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_LambdaFunction (AST_VISITOR_ARGS) { 
	if (!entering) {
		yyrule(1);
		yv = Unparse_LambdaFunction(UparseFormals(node), Unparse_LambdaStmt(yy[1]));
	}
} 

///////////////////////////////////////////////////////////
// Function
//
void AST::UnparseVisitor::Handle_FunctionStmt (AST_VISITOR_ARGS) { 
	if (!entering) {
		yyrule(1);
		yv = Unparse_FunctionDef(yy[1]);
	}
}

///////////////////////////////////////////////////////////
// [ Stmts ]
//
void AST::UnparseVisitor::Handle_Compound (AST_VISITOR_ARGS) { 
	if (entering)
		yyinctabs();
	else {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n == 1 || n == 0);
		yyrule(n);
		if (!n)
			yv = Unparse_Compound("");
		else
			yv = UNPARSE_COMPOUND();
		yydectabs();
	}
} 

///////////////////////////////////////////////////////////
// ExprList
//
void AST::UnparseVisitor::Handle_ExpressionListStmt (AST_VISITOR_ARGS) { 
	if (!entering) {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		yyrule(n);
		DASSERT(n);
		std::list<std::string> l;
		for (util_ui32 i = 1; i <= n; ++i)
			l.push_front(yy[i]);	// Were in reverse order.
		yv = Unparse_ExprListStmt(Unparse_ExprList(l));
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_EmptyStmt (AST_VISITOR_ARGS) { 
	if (!entering)
		yypush(Unparse_EmptyStmt());
} 

///////////////////////////////////////////////////////////
// ASSERT Expr
//
void AST::UnparseVisitor::Handle_AssertStmt (AST_VISITOR_ARGS) { 
	if (!entering) {
		yyrule(1);
		yv = Unparse_ExprStmt(ASSERT, yy[1]);
	}
} 

///////////////////////////////////////////////////////////
// RETURN [ Expr ]
//
void AST::UnparseVisitor::Handle_Return (AST_VISITOR_ARGS) { 
	if (!entering) {
		util_ui32 n = DPTR(node)->GetChild(AST_CHILD_EXPR) ? 1 : 0;
		yyrule(n);
		yv = n ? Unparse_ExprStmt(RETURN, yy[1]) : Unparse_BuiltInStmt(RETURN);
	}
} 

///////////////////////////////////////////////////////////
// USING ( ID | Namespace :: ID )
//
void AST::UnparseVisitor::Handle_UsingNamespace (AST_VISITOR_ARGS) { 
	if (!entering) {
		if (AST::IdList* path = NAMESPACE(node))
			yypush(Unparse_UsingNamespace(DPTR(path)->l));
		else
			yypush(Unparse_UsingNamespace(NAME(node)));
	}
}	

///////////////////////////////////////////////////////////
// USING StringConst : ID
//
void AST::UnparseVisitor::Handle_UsingByteCodeLibrary (AST_VISITOR_ARGS) { 
	if (!entering) {
		if (const char* file = VALUE_STRING(node))
			yypush(Unparse_UsingByteCodeLibrary(file, NAME(node)));
		else
			yypush(Unparse_UsingByteCodeLibrary(NAME(node)));
	}
}	

///////////////////////////////////////////////////////////
// CONST ID = Expr
//
void AST::UnparseVisitor::Handle_ConstDefExpression (AST_VISITOR_ARGS) { 
	if (!entering) {
		yyrule(1);
		yv = Unparse_ConstDef(NAME(node), yy[1]);
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
	if (!entering) {
		yyrule(3);
		yv = Unparse_TernaryOp(yy[1], yy[2], yy[3]);
	}
} 

///////////////////////////////////////////////////////////
// ( Expr )
//
void AST::UnparseVisitor::Handle_ParenthesisedExpr (AST_VISITOR_ARGS) { 
	if (!entering) {
		yyrule(1);
		yv = Unparse_ParenthesisedExpr(yy[1]);
	}
}

///////////////////////////////////////////////////////////
// [ Expr [ , Expr ]* ]
//
void AST::UnparseVisitor::Handle_ExprList (AST_VISITOR_ARGS) { 
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

void AST::UnparseVisitor::Handle_NumConst (AST_VISITOR_ARGS) { 
	if (!entering)
		yypush(
			Unparse_Number(
				DPTR((AST::NumItem*) DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE))->Get()
			)
		);
} 

void AST::UnparseVisitor::Handle_NilConst (AST_VISITOR_ARGS) { 
	if (!entering)
		yypush(Unparse_Nil());
} 

void AST::UnparseVisitor::Handle_BoolConst (AST_VISITOR_ARGS) { 
	if (!entering) {
		util_ui32 i = (util_ui32) DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE);
		DASSERT(i == 1 || i == 0);
		yypush(Unparse_Bool(i == 1));
	}
} 

///////////////////////////////////////////////////////////

static std::list<std::string>& GetStringList (TreeNode* node) {
		AST::StringList* sl	 = (AST::StringList*) DPTR(node)->GetAttribute(AST_ATTRIBUTE_ITEMS);
		return DPTR(sl)->l;
}

void AST::UnparseVisitor::Handle_StringConst (AST_VISITOR_ARGS) { 
	if (!entering)
		yypush(Unparse_StringConst(GetStringList(node)));
} 

void AST::UnparseVisitor::Handle_StringifyDottedIdents (AST_VISITOR_ARGS) { 
	if (!entering)
		yypush(Unparse_StringifyDottedIdents(GetStringList(node)));
} 

void AST::UnparseVisitor::Handle_StringifyNamespaceIdent (AST_VISITOR_ARGS) { 
	if (!entering) {
		AST::StringList*		sl		= NAMESPACE(node);
		std::list<std::string>& l		= DPTR(sl)->l;
		DASSERT(!l.empty());
		yypush(Unparse_StringifyNamespaceIdent(l));
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_NamespaceLvalue (AST_VISITOR_ARGS) { 
	if (!entering) {
		AST::IdList* ids	= NAMESPACE(node);
		yypush(Unparse_NamespaceIdent(DPTR(ids)->l, NAME(node)));
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
	if (!entering) {
		yyrule(2);
		yv = Unparse_FunctionCall(yy[1], yy[2]);
	}
}

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_Lvalue (AST_VISITOR_ARGS) 
	{ if (!entering) yypush(Unparse_Var(NAME(node))); }	

void AST::UnparseVisitor::Handle_AttrLvalue (AST_VISITOR_ARGS) 
	{ if (!entering) yypush(NAME(node)); } 

void AST::UnparseVisitor::Handle_StaticLvalue (AST_VISITOR_ARGS)
	{ if (!entering) yypush(Unparse_Var(NAME(node), STATIC)); } 

void AST::UnparseVisitor::Handle_LocalLvalue (AST_VISITOR_ARGS)
	{ if (!entering) yypush(Unparse_Var(NAME(node), LOCAL)); } 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_DotIndexIdent (AST_VISITOR_ARGS) {
	{ UNPARSE_PUSH_STRING(VALUE_STRING(node)); }
}

void AST::UnparseVisitor::Handle_DotIndexOpString (AST_VISITOR_ARGS)
	{ UNPARSE_PUSH_STRING(Unparse_OperatorIndex(VALUE_STRING(node))); }

void AST::UnparseVisitor::Handle_DotIndexStringConst (AST_VISITOR_ARGS) 
	{ }	// $$=$1 

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
	if (entering)
		yyinctabs();
	else {
		UNPARSE_ELEMENTS();
		yv = UNPARSE_TABLE(yv);
		yydectabs();
	}
}

void AST::UnparseVisitor::Handle_IdentIndexElement (AST_VISITOR_ARGS) {
	if (!entering) {
		yyrule(1);
		yv = Unparse_IdentIndexElement(NAME(node), yy[1]);
	}
}

void AST::UnparseVisitor::Handle_UnindexedValue (AST_VISITOR_ARGS)
	{ UNPARSE_DECORATE_SINGLE(Unparse_UnindexedValue); }

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
	if (!entering)
		{ UNPARSE_LIST(Unparse_TableIndices, ""); }
} 

// Indices : Values
void AST::UnparseVisitor::Handle_IndexedValues (AST_VISITOR_ARGS) { 
	if (!entering) {
		yyrule(2);
		yv = Unparse_IndexedValues(yy[1], yy[2]);
	}
} 

// @ID SetExpr GetExpr
void AST::UnparseVisitor::Handle_NewAttribute (AST_VISITOR_ARGS) { 
	if (!entering) {
		yyrule(2);
		yv = Unparse_NewAttributeSet(NAME(node), yy[1], yy[2]);
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_IfStmt (AST_VISITOR_ARGS) { 
	if (entering)
		yyinctabs();
	else {
		yyrule(2);
		yv = UNPARSE_IF(yy[1], yy[2]);
		yydectabs();
	}
} 

void AST::UnparseVisitor::Handle_IfElseStmt (AST_VISITOR_ARGS) { 
	if (entering)
		yyinctabs();
	else {
		yyrule(3);
		yv = UNPARSE_IFELSE(yy[1], yy[2], yy[3]);
		yydectabs();
	}
} 

///////////////////////////////////////////////////////////

void AST::UnparseVisitor::Handle_WhileStmt (AST_VISITOR_ARGS) { 
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
	if (!entering) {
		yyrule(1);
		yv = Unparse_ExprStmt(THROW, yy[1]);
	}
} 

void AST::UnparseVisitor::Handle_ExceptionVar (AST_VISITOR_ARGS) 
	{ UNPARSE_PUSH_STRING(NAME(node)); } 

// TRY Stmt TRAP ID Stmt
void AST::UnparseVisitor::Handle_Exception (AST_VISITOR_ARGS) { 
	if (entering)
		yyinctabs();
	else {
		yyrule(3);
		yv = UNPARSE_TRYTRAP(yy[1], yy[3], yy[2]);
		yydectabs();
	}
}

///////////////////////////////////////////////////////////

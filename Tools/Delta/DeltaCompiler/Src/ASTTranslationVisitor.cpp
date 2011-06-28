// ASTTranslationVisitor.cpp
// Tree visitor for translation to intermediate code.
// ScriptFighter Project.
// A. Savidis, September 2009.
//
#include "ASTTranslationVisitor.h"
#include "ASTCreationActions.h"
#include "ParseActions.h"
#include "CompilerAPI.h"
#include "GlobalData.h"
#include "ASTTags.h"
#include "Unparsable.h"
#include "Unparse.h"
#include "DeltaSyntax.h"
#include "ufunctors.h"
#include "uerrorclass.h"
#include "ustlhelpers.h"

///////////////////////////////////////////////////////////

#define Rule		AST::TranslationVisitor::EvaluationStack::TopRule
#define	EvalStack()	DNULLCHECK((AST::TranslationVisitor*) closure)->GetEvalStack()
#define	yysetline()	DeltaCompiler::SetLine(NEWLINE(node))
#define	yyrule		yysetline();							\
					Rule yy(EvalStack().GetValues());		\
					yy = Rule::RuleSize()
#define	yyapply		yy.Apply(__LINE__, node->GetTag().c_str())
#define	yypreserve	yy.Preserve()
#define	yypush		EvalStack().Set(__LINE__, node->GetTag().c_str()).Push
#define	yytop		EvalStack().Top()
#define	yv			yy[0]
#define	M()			if (true) { yyrule;	yv = QUADS.NextQuadNo();	yyapply; } else
#define	N()			if (true) { yyrule;	yv = Translate_N();			yyapply; } else

///////////////////////////////////////////////////////////

#define	_H(name, tag)		\
		SetHandler(tag, &Handle_##name, this)

#define	_C(name, tag, id)	\
		SetContextDependentHandler(tag, id, &Handle_##name, this)

#define	SET_QUAD_LINE(yypos, is_expr)							\
		util_ui32 line = LINE(node);							\
		DASSERT(line);											\
		QUADS.SetQuadLine(yy[yypos].quadNo(), line, is_expr)	\

///////////////////////////////////////////////////////////

AST::TranslationVisitor::TranslationVisitor (void) {

	// Context independent handlers.
	_H(Program,							AST_TAG_PROGRAM);
	_H(Stmts,							AST_TAG_STMTS);
	_H(Stmt,							AST_TAG_STMT);
	_H(BasicStmt,						AST_TAG_BASIC_EXPR_STMT);
	_H(BasicStmt,						AST_TAG_BASIC_NON_EXPR_STMT);
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
	_H(PrimaryFunctionAndTableObject,	AST_TAG_PRIMARY_FUNCTION_TABLE_OBJECT);
	_H(PrimaryConstValue,				AST_TAG_PRIMARY_CONST_VALUE);	
	_H(Ternary,							AST_TAG_TERNARY);
	_H(Expr,							AST_TAG_EXPR);
	_H(ParenthesisedExpr,				AST_TAG_PARENTHESISED_EXPR);
	_H(ExprList,						AST_TAG_EXPR_LIST);
	_H(ExpressionListItem,				AST_TAG_EXPRLIST_ITEM);
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
	_H(Argument,						AST_TAG_ARGUMENT);
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
	_H(TableObject,						AST_TAG_TABLE_OBJECT);
	_H(TableConstructor,				AST_TAG_TABLE_CONSTRUCTOR);
	_H(UnindexedValue,					AST_TAG_TABLE_UNINDEXED_ELEM);	
	_H(UnindexedValue,					AST_TAG_TABLE_UNINDEXED_ELEM);	
	_H(IdentIndexElement,				AST_TAG_TABLE_INDENTINDEX_ELEM);
	_H(FunctionExpression,				AST_TAG_FUNCTION_EXPR);
	_H(FunctionExpression,				AST_TAG_PRIMARY_LAMBDA_DEFINITION);
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

	// Context dependent handlers.
	_C(AdaptBoolExpression, AST_TAG_OP_ADD, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_SUB, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_MUL, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_DIV, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_MOD, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_GT, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_LT, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_GE, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_LE, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_EQ, AST_CHILD_LEFT);
	_C(AdaptBoolExpression, AST_TAG_OP_NE, AST_CHILD_LEFT);

	_C(AdaptBoolExpression, AST_TAG_OP_ADD, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_SUB, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_MUL, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_DIV, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_MOD, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_GT, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_LT, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_GE, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_LE, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_EQ, AST_CHILD_RIGHT);
	_C(AdaptBoolExpression, AST_TAG_OP_NE, AST_CHILD_RIGHT);

	_C(IfCondition,						AST_TAG_IF,					AST_CHILD_COND);
	_C(IfCondition,						AST_TAG_IF_ELSE,			AST_CHILD_COND);
	_C(ElsePrefix,						AST_TAG_IF_ELSE,			AST_CHILD_ELSE);
	_C(WhileCondition,					AST_TAG_WHILE,				AST_CHILD_COND);
	_C(ForCond,							AST_TAG_FOR,				AST_CHILD_COND);
	_C(ForeachContainer,				AST_TAG_FOREACH,			AST_CHILD_CONTAINER);
	_C(ExpressionANDOR,					AST_TAG_AND,				AST_CHILD_LEFT);
	_C(ExpressionANDOR,					AST_TAG_OR,					AST_CHILD_LEFT);
	_C(TernaryCondition,				AST_TAG_TERNARY,			AST_CHILD_COND);
	_C(TernaryLeftExpr,					AST_TAG_TERNARY,			AST_CHILD_LEFT);
	_C(TernaryRightExpr,				AST_TAG_TERNARY,			AST_CHILD_RIGHT);
	_C(ForInit,							AST_TAG_FOR,				AST_CHILD_INIT);
	_C(ForSuffix,						AST_TAG_FOR,				AST_CHILD_SUFFIX);
	_C(LambdaStmt,						AST_TAG_LAMBDA_FUNCTION,	AST_CHILD_EXPR);
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::operator()(AST::Node* root) {
	if (root) {
		GetEvalStack().Initialise();
		DPTR(root)->AcceptPreOrder(this);
		GetEvalStack().CleanUp();
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_Program (AST_VISITOR_ARGS){ 
	if (entering)
		GlobalData::Start();
	else {
		GlobalData::End();
		DASSERT(DeltaExpr::GetReturnValue()->sym->GetOffset() == DELTA_RETVALUE_OFFSET);
		DASSERT(DeltaExpr::GetReturnValue()->sym->GetBlockId() == DELTA_MAIN_BLOCK_VALUE);
	}
}

//*************************
// Function: M(1.quadNo) Class Name(2.sym) FormalArgs Compound(3.stmt)
//
void AST::TranslationVisitor::Handle_FunctionBasic (AST_VISITOR_ARGS, const std::string (*unparser)(DeltaSymbol*, Stmt*)) {

	if (entering) {	

		M();

		yyrule;

		const char*		name		= NAME(node);
		util_ui8		funcClass	= AST::GetFuncClass((AST::Node*) node);
		util_ui8		funcLinkage	= AST::GetFuncLinkage((AST::Node*) node);
		yv = Translate_Function(name, funcClass, funcLinkage);

		yyapply;

	} else {

		yyrule(3);

		UNPARSABLE_GET((*unparser)(yy[2].sym(), yy[3].stmt()));
		if (yv = Translate_Function(yy[2].sym(), yy[3].stmt(), yy[1].quadNo()))
			DPTR(yv.sym())->SetSourceCode(unparsed);

		yyapply;
	}
}

//*************************

void AST::TranslationVisitor::Handle_Function (AST_VISITOR_ARGS)
	{  Handle_FunctionBasic(AST_VISITOR_ACTUALS, &Unparse_Function); }

void AST::TranslationVisitor::Handle_LambdaFunction (AST_VISITOR_ARGS)
	{  Handle_FunctionBasic(AST_VISITOR_ACTUALS, &Unparse_LambdaFunction); }

//*************************
// LambdaStmt: M(1.quadNo) Expr(2.expr)

void AST::TranslationVisitor::Handle_LambdaStmt (AST_VISITOR_ARGS) {
	if (entering) {
		yysetline();
		Translate_CompoundBegin();
		M();
	}
	else {
		yyrule(2);

		DeltaExpr* expr = yy[2].expr();
		DASSERT(expr);

		Unparse_LambdaStmt(yv = Translate_RETURN(expr), expr);
		Translate_CompoundEnd();
		SET_QUAD_LINE(1, true);
		yyapply;
	}
}

//*************************

void AST::TranslationVisitor::Handle_FunctionStmt (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		Unparse_FunctionDef(yv = NEW_STMT, yy[1].sym());
		yyapply;
	}
}

//*************************
// FormalArgs: IdentList (as attribute).
//
void AST::TranslationVisitor::Handle_FormalArgs (AST_VISITOR_ARGS){ 

	if (entering) {

		if (AST::IdList* formals = (AST::IdList*) node->GetAttribute(AST_ATTRIBUTE_ITEMS)) {
			std::list<std::string>&	l = DPTR(formals)->l;
			for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i) {
				if (*i == AST_VALUE_VARARGS_FORMAL_NAME) {
					DASSERT(&*i == &l.back());	// It is the last element.
					Translate_VarArgs();
				}
				else
					Translate_IdentList(i->c_str());
			}
		}

		Translate_FunctionHeader(yytop.sym());
		DELTASYMBOLS.PushAndResetTempCounter();
	}
} 

//*************************
// Compound: [Stmts] (1.stmt)
//
void AST::TranslationVisitor::Handle_Compound (AST_VISITOR_ARGS){ 
	if (entering) {
		if (DPTR(node)->GetTotalChildren())
			Translate_CompoundBegin();
	}
	else {
		util_ui32 n = DPTR(node)->GetTotalChildren();
		DASSERT(n==1 || n==0);

		yyrule(n);
		
		if (n) {
			Unparse_Compound(yv = yy[1].stmt()); 
			Translate_CompoundEnd();
		}
		else {
			Unparse_Compound(yv = NEW_STMT);
			Translate_EmptyCompound();
		}

		yyapply;
	}
} 

//*************************
//Stmts: Stmt* (i.stmt)
//
void AST::TranslationVisitor::Handle_Stmts (AST_VISITOR_ARGS){ 

	if (!entering) {

		Stmt*		stmt	= NIL_STMT;
		util_ui32	n		= DPTR(node)->GetTotalChildren();

		yyrule(n);	// All stmts on the stack.

		for (util_ui32 i = 1; i <= n; ++i)
			if (i == 1)
				stmt = yy[1].stmt();
			else {
				UNPARSABLE_GET(Unparse_Stmts(stmt, yy[i].stmt()));
				stmt = Translate_Stmts(stmt, yy[i].stmt()); 
				UNPARSABLE_SET(stmt);
			}

		yv = stmt;
		yyapply;
	}
}

//*************************
// Stmt: Any stmt (1.stmt)
//
void AST::TranslationVisitor::Handle_Stmt (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = Translate_Stmts(yy[1].stmt());
		yyapply;
	}
} 

//*************************
// BasicStmt: M(1.quadNo) Stmt(2.stmt)
//
void AST::TranslationVisitor::Handle_BasicStmt (AST_VISITOR_ARGS){ 
	if (entering)
		M();
	else {
		yyrule(2);
		SET_QUAD_LINE(1, DPTR(node)->GetTag() == AST_TAG_BASIC_EXPR_STMT);
		Translate_BasicStmt(line);
		yv = yy[2].stmt();
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// ExpressionListStmt: Expr*(i.expr) Semi
//
void AST::TranslationVisitor::Handle_ExpressionListStmt (AST_VISITOR_ARGS){

	if (!entering) {

		DeltaExpr*	expr	= NIL_EXPR;
		util_ui32	n		= DPTR(node)->GetTotalChildren();

		yyrule(n);	// All exprs on the stack.

		for (util_ui32 i = 1; i <= n; ++i)
			expr = Translate_ExpressionList(expr, yy[i].expr());

		yv = Translate_ExprListStmt(expr); 
		Unparse_ExprListStmt(yv.stmt(), expr);

		yyapply;
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_EmptyStmt (AST_VISITOR_ARGS){
	if (!entering) {
		yyrule; 
		yv = NEW_STMT; 
		yv.stmt()->SetUnparsed(";");
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// Assert: assert Expr(1.expr)
//
void AST::TranslationVisitor::Handle_AssertStmt (AST_VISITOR_ARGS){ 
	if (entering) 
		ParseParms::InAssertStmt().enter();
	else {
		yyrule(1);
		Translate_ASSERT(yy[1].expr()); 
		ParseParms::InAssertStmt().exit(); 
		Unparse_ExprStmt(yv = NEW_STMT, ASSERT, yy[1].expr());
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// Return: return [ Expr(1.expr) ]
//
void AST::TranslationVisitor::Handle_Return (AST_VISITOR_ARGS){ 
	if (!entering) {
		if (node->GetChild(AST_CHILD_EXPR)) {
			yyrule(1);
			yv = Translate_RETURN(yy[1].expr()); 
			Unparse_ExprStmt(yv.stmt(), RETURN, yy[1].expr());
			yyapply;
		}
		else {
			yyrule;
			Unparse_BuiltInStmt(yv = Translate_RETURN(), RETURN);
			yyapply;
		}
	}
} 

///////////////////////////////////////////////////////////
// Using: using  ( Namespace (attr namespace)  | IDENT (attr name) )
//
void AST::TranslationVisitor::Handle_UsingNamespace (AST_VISITOR_ARGS){ 
	if (!entering) {
		if (AST::IdList* path = NAMESPACE(node))
			Translate_UsingNamespace(DPTR(path)->l);
		else
			Translate_UsingNamespace(NAME(node));
	}
}	

///////////////////////////////////////////////////////////
// Using: using  StringConst (value) : IDENT (attr name)
//
void AST::TranslationVisitor::Handle_UsingByteCodeLibrary (AST_VISITOR_ARGS){ 
	if (!entering)
		Translate_UsingByteCodeLibrary(NAME(node), LINE(node));
}	

///////////////////////////////////////////////////////////
// Const: const IDENT (attr) Expr (1.expr)
//
void AST::TranslationVisitor::Handle_ConstDefExpression (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = Translate_ConstAssignExpression(NAME(node), yy[1].expr());
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// Assign: Lvalue(1.expr) Rvalue(2.expr)
//

#define	lvalue	yy[1].expr()

void AST::TranslationVisitor::Handle_AssignExpr (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		yv = Translate_AssignExpr(lvalue, yy[2].expr()); 
		lvalue->SetInitialised(); 
		Unparse_BinaryOp(yv.expr(), lvalue, ASSIGN, yy[2].expr());
		yyapply;
	}
}

//*****************************

#define	RESULT(opcode,token)	\
		return std::pair<util_ui8, util_ui32>(DeltaIC_##opcode, token)

static std::pair<util_ui32,util_ui32> GetArithOpCode (const std::string& op) {
	if (op == AST_TAG_OP_ADD_ASSIGN)	RESULT(ADD, ADD_A);
	if (op == AST_TAG_OP_MUL_ASSIGN)	RESULT(MUL, MUL_A);
	if (op == AST_TAG_OP_SUB_ASSIGN)	RESULT(SUB, SUB_A);
	if (op == AST_TAG_OP_DIV_ASSIGN)	RESULT(DIV, DIV_A);
	if (op == AST_TAG_OP_MOD_ASSIGN)	RESULT(MOD, MOD_A);
	if (op == AST_TAG_OP_ADD)			RESULT(ADD, ADD);
	if (op == AST_TAG_OP_MUL)			RESULT(MUL, MUL);
	if (op == AST_TAG_OP_SUB)			RESULT(SUB, SUB);
	if (op == AST_TAG_OP_DIV)			RESULT(DIV, DIV);
	if (op == AST_TAG_OP_MOD)			RESULT(MOD, MOD);

	DASSERT(false);
	return std::pair<util_ui8, util_ui32>(0,0);
}

static std::pair<util_ui32,util_ui32> GetRelatOpCode (const std::string& op) {
	if (op == AST_TAG_OP_LT)			RESULT(JLT, LT);
	if (op == AST_TAG_OP_GT)			RESULT(JGT, GT);
	if (op == AST_TAG_OP_LE)			RESULT(JLE, LE);
	if (op == AST_TAG_OP_GE)			RESULT(JGE, GE);
	if (op == AST_TAG_OP_EQ)			RESULT(JEQ, EQ);
	if (op == AST_TAG_OP_NE)			RESULT(JNE, NE);

	DASSERT(false);
	return std::pair<util_ui8, util_ui32>(0,0);
}

#undef	RESULT

//*****************************

void AST::TranslationVisitor::Handle_AssignArithExpr (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		std::string	op							= node->GetTag();
		std::pair<util_ui8, util_ui32>	opcode	= GetArithOpCode(op);
		yv = Translate_AssignArithExpr(lvalue, yy[2].expr(), opcode.first, Unparse_Token(opcode.second).c_str()); 
		Unparse_BinaryOp(yv.expr(), lvalue, opcode.second, yy[2].expr());
		yyapply;
	}
} 

#undef lvalue

//*****************************
// BinaryExpr: left(1.expr) op (from tag) right(2.expr)
//

#define	left	yy[1].expr()
#define	right	yy[2].expr()

void AST::TranslationVisitor::Handle_ArithmeticExpression (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		std::string	op							= node->GetTag();
		std::pair<util_ui8, util_ui32>	opcode	= GetArithOpCode(op);
		yv = Translate_ArithmeticExpression(left, opcode.first, right, Unparse_Token(opcode.second).c_str()); 
		Unparse_BinaryOp(yv.expr(), left, opcode.second, right);
		yyapply;
	}
}
 
//*****************************

void AST::TranslationVisitor::Handle_RelationalExpr (AST_VISITOR_ARGS){ 

	if (!entering) {
		yyrule(2);
		std::string	op							= node->GetTag();
		std::pair<util_ui8, util_ui32>	opcode	= GetRelatOpCode(op);
		yv = Translate_RelationalExpr(left, right, opcode.first, Unparse_Token(opcode.second).c_str()); 
		Unparse_BinaryOp(yv.expr(), left, opcode.second, right);
		yyapply;
	}
} 

//*****************************

void AST::TranslationVisitor::Handle_ExpressionANDOR( AST_VISITOR_ARGS){ 
	if (!entering) {
		{
		yyrule(1);
		DNPTR(left)->AdaptToBool();
		yv = left;
		yyapply;
		}		// Block necessary to emulate successive rules.
		M();	// Mark for backpatching.
	}
}

//*****************************

void AST::TranslationVisitor::Handle_BooleanExpression (
		AST_VISITOR_ARGS, 
		DeltaExpr* (*f)(DeltaExpr*, DeltaQuadAddress, DeltaExpr*),
		util_ui32 token
	){ 
	if (!entering) {
		yyrule(3);
		yv = (*f)(yy[1].expr(), yy[2].quadNo(), yy[3].expr()); 
		Unparse_BinaryOp(yv.expr(), yy[1].expr(), token,  yy[3].expr());
		yyapply;
	}
} 

void AST::TranslationVisitor::Handle_ExprANDExpr (AST_VISITOR_ARGS)
	{ Handle_BooleanExpression(AST_VISITOR_ACTUALS, &Translate_ExprANDExpr, AND); }

void AST::TranslationVisitor::Handle_ExprORExpr (AST_VISITOR_ARGS)
	{ Handle_BooleanExpression(AST_VISITOR_ACTUALS, &Translate_ExprORExpr, OR); }

//*****************************

void  AST::TranslationVisitor::Handle_UnaryExpression (AST_VISITOR_ARGS, DeltaExpr* (*f)(DeltaExpr*), util_ui32 token, const std::string& infix){ 
	if (!entering) {
		yyrule(1);
		yv = (*f)(left); 
		Unparse_UnaryOp(yv.expr(), token, left, infix);
		yyapply;
	}
}

void AST::TranslationVisitor::Handle_NOTExpression (AST_VISITOR_ARGS)
	{  Handle_UnaryExpression(AST_VISITOR_ACTUALS, &Translate_NOTExpression, NOT, " "); }

void AST::TranslationVisitor::Handle_UMINUSExpression (AST_VISITOR_ARGS)
	{  Handle_UnaryExpression(AST_VISITOR_ACTUALS, &Translate_UMINUSExpression, SUB); }

//*****************************

void AST::TranslationVisitor::Handle_AdaptBoolExpression (AST_VISITOR_ARGS) {
	if (!entering) 
		{ yyrule(1); yv = yy[1].expr()->AdaptIfBool(); yyapply; }
}

//*****************************

#define	RESULT(opcode,token,after)	\
		return utriple<util_ui8, util_ui32,bool>(DeltaIC_##opcode, token, after)

static utriple<util_ui8, util_ui32,bool> GetIncDecOpCode (const std::string& op) {
	if (op == AST_LVALUE_PLUSPLUS)		RESULT(ADD, PLUSPLUS,	true);
	if (op == AST_LVALUE_MINUSMINUS)	RESULT(SUB, MINUSMINUS,	true);
	if (op == AST_PLUSPLUS_LVALUE)		RESULT(ADD, PLUSPLUS,	false);
	if (op == AST_MINUSMINUS_LVALUE)	RESULT(SUB, MINUSMINUS,	false);

	DASSERT(false);
	return utriple<util_ui8, util_ui32,bool>(0,0,false);
}

#undef	RESULT

void AST::TranslationVisitor::Handle_TermLvalueArith (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		utriple<util_ui8, util_ui32, bool> opcodes = GetIncDecOpCode(node->GetTag());
		yv = Translate_TermLvalue(yy[1].expr(), opcodes.third, opcodes.first); 
		Unparse_UnaryIncDec(yv.expr(), opcodes.second, yy[1].expr(), opcodes.third);
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_PrimaryFunctionAndTableObject (AST_VISITOR_ARGS) {
	if (!entering) {
		yyrule(1);
		yv = DNPTR(yy[1].expr())->AdaptIfTableContent();
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_PrimaryConstValue (AST_VISITOR_ARGS) {
	if (!entering)
		Unparse_ConstValue(yytop.expr());
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_TernaryCondition (AST_VISITOR_ARGS) {
	if (!entering) {
		yyrule(1);
		yv = Translate_Condition(left);
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_TernaryLeftExpr (AST_VISITOR_ARGS) {
	if (entering)
		M();
	else {
		yyrule(1);
		yv = DNPTR(left)->AdaptIfBool(); 
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_TernaryRightExpr (AST_VISITOR_ARGS) {
	if (entering) {
		N(); 
		M();
	}
	else {
		yyrule(1);
		yv = DNPTR(left)->AdaptIfBool(); 
		yyapply;
	}
}

//*****************************
// Ternary:	Condition(1.expr) ? M(2.quadNo) LeftExpr(3.expr) : N(4.quadNo) M(5.quadNo) RightExpr(6.expr)
//
void AST::TranslationVisitor::Handle_Ternary (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(6);
		yv =	Translate_Ternary(
					yy[1].expr(),	yy[3].expr(),	yy[6].expr(), 
					yy[2].quadNo(), yy[5].quadNo(), yy[4].quadNo()
				); 
		Unparse_TernaryOp(yv.expr(), yy[1].expr(), yy[3].expr(), yy[6].expr());
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_Expr (AST_VISITOR_ARGS){ 
	if (util_ui32 line = LINE(node)) {
		if (entering)
			M();
		else {
			yyrule(2);
			QUADS.SetQuadLine(yy[1].quadNo(), line);
			yv = yy[2].expr();
			yyapply;
		}
	}
}

//*****************************

void AST::TranslationVisitor::Handle_ParenthesisedExpr (AST_VISITOR_ARGS){
	if (!entering) {
		yyrule(1);
		Unparse_ParenthesisedExpr(yv = DNPTR(yy[1].expr())->CheckUninitialised());
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_ExprList (AST_VISITOR_ARGS){ 

	if (!entering) {

		DeltaExpr*	expr	= NIL_EXPR;
		util_ui32	n		= DPTR(node)->GetTotalChildren();

		yyrule(n);	// All exprs on the stack.

		for (util_ui32 i = 1; i <= n; ++i)
			expr = Translate_ExpressionList(expr, yy[i].expr());

		yv = expr;
		yyapply;	
	}	
}

//*****************************

void AST::TranslationVisitor::Handle_ExpressionListItem (AST_VISITOR_ARGS) {
	if (!entering) {
		yyrule(1);
		yv = Translate_ExpressionListItem(yy[1].expr());
		yyapply;	
	}	
}

///////////////////////////////////////////////////////////

#define	HANDLE_CONSTVALUE(val)					\
	if (!entering) {							\
		yyrule;									\
		yv = Translate_ConstValue(val);			\
		yyapply;								\
	}

void AST::TranslationVisitor::Handle_NumConst (AST_VISITOR_ARGS)
	{ HANDLE_CONSTVALUE(DPTR((AST::NumItem*) DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE))->Get()); }

//*****************************

void AST::TranslationVisitor::Handle_NilConst (AST_VISITOR_ARGS)
	{ HANDLE_CONSTVALUE(UEMPTY); }

//*****************************

void AST::TranslationVisitor::Handle_BoolConst (AST_VISITOR_ARGS) {
	util_ui32 i = (util_ui32) DPTR(node)->GetAttribute(AST_ATTRIBUTE_CONSTVALUE);
	DASSERT(i == 1 || i == 0);
	HANDLE_CONSTVALUE(i == 1); 
}

//*****************************

void AST::TranslationVisitor::Handle_StringItems (AST_VISITOR_ARGS, DeltaExpr* (*f)(const char*, DeltaExpr*), void(*g)(DeltaExpr*)){ 
	if (!entering) {
		yyrule;
		AST::StringList*		sl		= (AST::StringList*) DPTR(node)->GetAttribute(AST_ATTRIBUTE_ITEMS);
		std::list<std::string>& l		= DPTR(sl)->l;
		DeltaExpr*				expr	= NIL_EXPR;

		for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i)
			expr = (*f)(i->c_str(), expr);

		(*g)(yv = expr);
		yyapply;
	}
} 

void AST::TranslationVisitor::Handle_StringConst (AST_VISITOR_ARGS)
	{ Handle_StringItems(AST_VISITOR_ACTUALS, &Translate_StringConst, &Unparse_ConstValue); }

//*****************************

void AST::TranslationVisitor::Handle_StringifyDottedIdents (AST_VISITOR_ARGS)
	{ Handle_StringItems(AST_VISITOR_ACTUALS, &Translate_StringifyDottedIdents, &Unparse_Stringify); }

//*****************************

void AST::TranslationVisitor::Handle_StringifyNamespaceIdent (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule;
		AST::StringList*		sl		= NAMESPACE(node);
		std::list<std::string>& l		= DPTR(sl)->l;
		DASSERT(!l.empty());
		Unparse_Stringify(yv = Translate_StringifyNamespaceIdent(l));
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_KwdExpr (AST_VISITOR_ARGS, DeltaExpr* (*f)(void), util_ui32 token){ 
	if (!entering) {
		yyrule;
		Unparse_BuiltIn(yv = (*f)(), token);
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_SELF (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translate_SELF, SELF); }

void AST::TranslationVisitor::Handle_ARGUMENTS (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translate_ARGUMENTS, ARGUMENTS); }

void AST::TranslationVisitor::Handle_TRIPLE_DOT (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translate_TRIPLE_DOT, TRIPLE_DOT); }

static DeltaExpr* Translate_LAMBDA_REF_Adapter (void)
	{ return Translate_LAMBDA_REF(); }

void AST::TranslationVisitor::Handle_LAMBDA_REF (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translate_LAMBDA_REF_Adapter, LAMBDA_REF); }

void AST::TranslationVisitor::Handle_NEWSELF (AST_VISITOR_ARGS)
	{ Handle_KwdExpr(AST_VISITOR_ACTUALS, &Translate_NEWSELF, NEWSELF); }

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_BuiltInStmt (AST_VISITOR_ARGS, Stmt* (*f)(void), util_ui32 token){ 
	if (!entering) {
		yyrule;
		Unparse_BuiltInStmt(yv = (*f)(), token);
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_BREAK (AST_VISITOR_ARGS)
	{ Handle_BuiltInStmt(AST_VISITOR_ACTUALS, &Translate_BREAK, BREAK); }

void AST::TranslationVisitor::Handle_CONTINUE (AST_VISITOR_ARGS)
	{ Handle_BuiltInStmt(AST_VISITOR_ACTUALS, &Translate_CONTINUE, CONTINUE); }

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_FunctionParenthesisForm (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = Translate_FunctionParenthesisForm(yy[1].sym()); 
		Unparse_FunctionParenthesisForm(yv.expr(), yy[1].sym());
		yyapply;
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_LateBoundArgument (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = left->AdaptAsArgumentVariable(); 
		DNPTR(yv.expr())->SetLateBound();
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_Argument (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = left->AdaptAsArgumentVariable(); 
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_FunctionCall (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		UNPARSABLE_GET(Unparse_FunctionCall(yy[1].expr(), yy[2].expr()));
		yv = Translate_FunctionCall(yy[1].expr()->CheckUninitialised(), yy[2].expr()); 
		UNPARSABLE_SET(yv.expr());	
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_NamespaceLvalue (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule;
		std::string unparsed;
		AST::IdList*	ids	= NAMESPACE(node);
		NameList&		ns	= DPTR(ids)->l;
		yv = Translate_NamespaceLvalue(ns, NAME(node), &unparsed); 
		Unparse_Var(yv.expr(), unparsed);
		yyapply;
	}
}

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_LvalueByName (AST_VISITOR_ARGS, DeltaExpr* (*f)(const std::string& id), util_i32 token) {
	if (!entering) {
		yyrule;
		const char* name = NAME(node);
		if (token != -1) {
			DASSERT(token >= 0);
			Unparse_Var(yv = (*f)(name), name, (util_ui32) token);
		}
		else
			Unparse_Var(yv = (*f)(name), name);
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_Lvalue (AST_VISITOR_ARGS)
	{ Handle_LvalueByName(AST_VISITOR_ACTUALS, &Translate_Lvalue); }

void AST::TranslationVisitor::Handle_AttrLvalue (AST_VISITOR_ARGS)
	{ Handle_LvalueByName(AST_VISITOR_ACTUALS, &Translate_AttrLvalue, ATTRIBUTE); }

void AST::TranslationVisitor::Handle_StaticLvalue (AST_VISITOR_ARGS)
	{ Handle_LvalueByName(AST_VISITOR_ACTUALS, &Translate_StaticLvalue); }

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_LocalLvalue (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule;
		const char* name = NAME(node);
		Unparse_Var(yv = Translate_Lvalue(name, 0), name, LOCAL);
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_StringValue (AST_VISITOR_ARGS) {
	if (!entering) {
		yyrule;
		yv = Translate_StringConst((const char*) node->GetAttribute(AST_ATTRIBUTE_VALUE));
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_DotIndexIdent (AST_VISITOR_ARGS)
	{ Handle_StringValue(AST_VISITOR_ACTUALS); }

void AST::TranslationVisitor::Handle_DotIndexOpString (AST_VISITOR_ARGS)
	{ Handle_StringValue(AST_VISITOR_ACTUALS); }

void AST::TranslationVisitor::Handle_DotIndexStringConst (AST_VISITOR_ARGS)
	{ }	// $$ = $1

void AST::TranslationVisitor::Handle_BracketIndexOperatorMethod (AST_VISITOR_ARGS)
	{ Handle_StringValue(AST_VISITOR_ACTUALS); }

///////////////////////////////////////////////////////////

static bool IsDotIndex (TreeNode* node) {
	const std::string& tag = node->GetTag();
	if (tag ==AST_TAG_DOTINDEX_IDENT)		return true;
	if (tag ==AST_TAG_DOTINDEX_OPSTRING)	return true;
	if (tag ==AST_TAG_DOTINDEX_STRINGCONST)	return true;
	return false;
}			  

//*****************************
// TableContent: Table(1.expr) Index(2.expr)
//

void AST::TranslationVisitor::Handle_TableContent (
		AST_VISITOR_ARGS, 
		DeltaExpr*	(*f)(DeltaExpr*, DeltaExpr*), 
		bool bounded
	) {
	if (!entering) {

		yyrule(2);

		DeltaExpr* index = yy[2].expr();
		yv = (*f)(yy[1].expr()->CheckUninitialised(), index); 

		if (IsDotIndex(node->GetChild(AST_CHILD_INDEX))) {
			DASSERT(index->GetType() == DeltaExprString);
			if (!bounded)
				Unparse_TableContentDot(yv.expr(), yy[1].expr(), DPTR(index)->strConst.c_str()); 
			else
				Unparse_TableContentDoubleDot(yv.expr(), yy[1].expr(), DPTR(index)->strConst.c_str()); 
		}
		else
			if (!bounded)
				Unparse_TableContentBracket(yv.expr(), yy[1].expr(), index); 
			else
				Unparse_TableContentDoubleBracket(yv.expr(), yy[1].expr(), index); 

		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_TableContentDot (AST_VISITOR_ARGS)
	{ Handle_TableContent(AST_VISITOR_ACTUALS, &Translate_TableContent, false); }

void AST::TranslationVisitor::Handle_BoundedTableContentDot (AST_VISITOR_ARGS)
	{ Handle_TableContent(AST_VISITOR_ACTUALS, &Translate_BoundedTableContent, true); }

void AST::TranslationVisitor::Handle_TableContentBracket (AST_VISITOR_ARGS)
	{ Handle_TableContent(AST_VISITOR_ACTUALS, &Translate_TableContent, false); }

void AST::TranslationVisitor::Handle_BoundedTableContentBracket (AST_VISITOR_ARGS)
	{ Handle_TableContent(AST_VISITOR_ACTUALS, &Translate_BoundedTableContent, true); }

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_TableObject (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = DNPTR(yy[1].expr())->AdaptIfTableContent();
		yyapply;
	}
}

//*****************************
// TableConstructor: Table(1.elems) Elem*(i.elems)
//
void AST::TranslationVisitor::Handle_TableConstructor (AST_VISITOR_ARGS){ 

	if (entering) {
		ParseParms::InTableExpr().enter(); 
		yypush(Translate_TablePrefix());
	} 
	else {

		TableElements*	elems	= (TableElements*) 0;
		util_ui32		n		= DPTR(node)->GetTotalChildren() + 1;	// The table being the first element.

		yyrule(n);	// All exprs on the stack.

		for (util_ui32 i = 2; i <= n; ++i)
			if (i == 2)
				elems = yy[i].elems();
			else {
				UNPARSABLE_GET(Unparse_TableElements(elems, yy[i].elems()));
				elems = Translate_TableElements(elems, yy[i].elems()); 
				UNPARSABLE_SET(elems);
			}			

		DeltaExpr* table = yy[1].expr();
		ParseParms::InTableExpr().exit();
		UNPARSABLE_GET(Unparse_TableConstructor(elems)); 
		table = Translate_TableConstructor(table, elems); 
		UNPARSABLE_SET(table);

		yv = table;
		yyapply;	
	}	
} 

//*****************************

void AST::TranslationVisitor::Handle_UnindexedValue (AST_VISITOR_ARGS){ 
	if (entering)
		M();
	else {
		yyrule(2);
		yv = Translate_UnindexedValue(yy[2].expr(), yy[1].quadNo(), LINE(node));
		yyapply;
	}
}	

//*****************************

void  AST::TranslationVisitor::Handle_IdentIndexElement (AST_VISITOR_ARGS) {
	if (entering)
		M();
	else {
		yyrule(2);
		yv = Translate_IdentIndexElement(NAME(node), yy[2].expr(), yy[1].quadNo(), LINE(node));
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_FunctionExpression (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(1);
		yv = Translate_FunctionExpresssion(yy[1].sym());
		yyapply;
	}
}	

//*****************************

void AST::TranslationVisitor::Handle_DottedIdent (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule;
		std::string name = NAME(node);
		Unparse_DottedIdent(yv = Translate_ConstValue(name), name.c_str());
		yyapply;
	}
} 

//*****************************

void AST::TranslationVisitor::Handle_OperatorIndex (AST_VISITOR_ARGS)
	{ Handle_StringValue(AST_VISITOR_ACTUALS); }

//*****************************

void AST::TranslationVisitor::Handle_TableValues (AST_VISITOR_ARGS)
	{ Handle_ExprList(AST_VISITOR_ACTUALS); }

void AST::TranslationVisitor::Handle_TableIndices (AST_VISITOR_ARGS) {

	if (!entering) {

		DeltaExpr*	expr	= NIL_EXPR;
		util_ui32	n		= DPTR(node)->GetTotalChildren();

		yyrule(n);	// All exprs on the stack.

		for (util_ui32 i = 1; i <= n; ++i)
			expr = Translate_ExpressionList(expr, yy[i].expr()->AdaptAsArgumentVariable());

		yv = expr;
		yyapply;	
	}						
}

//*****************************
// IndexedValues: { IndexedList(1.expr) : ExpressionList(2.expr) }
//
void AST::TranslationVisitor::Handle_IndexedValues (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		yv = Translate_IndexedValues(yy[1].expr(), yy[2].expr());
		Unparse_IndexedValues(yv.elems());
		yyapply;
	}
} 

//*****************************
// Attribute: IDENT (attr) Set(1.expr) Get(2.expr)
//
void AST::TranslationVisitor::Handle_NewAttribute (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(2);
		const char* name = NAME(node);
		DeltaExpr* attr = Translate_NewAttribute(name, yy[1].expr(), yy[2].expr());
		Unparse_UnindexedValue(yv = Translate_TableElement(attr), attr); 
		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_Condition (AST_VISITOR_ARGS){ 
	if (!entering) {
		{
		yyrule(1);
		yv = Translate_Condition(yy[1].expr()); 
		Translate_BasicStmt(LINE(node));
		yyapply;
		}
		M();
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_IfCondition (AST_VISITOR_ARGS){ 
	if (!entering) {
		Handle_Condition(AST_VISITOR_ACTUALS);
		Translate_IfStmtPrefix();
	}
}

//*****************************
// If: Condition(1.expr) M(2.quadNo) Stmt(3.stmt)
//
void AST::TranslationVisitor::Handle_IfStmt (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(3);
		Translate_IfStmt(yy[1].expr(), yy[2].quadNo());
		Unparse_If(yv = yy[3].stmt(), yy[1].expr(), yy[3].stmt());
		yyapply;
	}
}

//*****************************

void AST::TranslationVisitor::Handle_ElsePrefix (AST_VISITOR_ARGS){ 
	if (entering) {
		Translate_ElseStmtPrefix();
		N(); 
		M(); 
	} 
}

//*****************************
// IfElse: Condition(1.expr) M(2.quadNo) Stmt(3.stmt) N(4.quadNo) M(5.quadNo) Stmt(6.stmt)
//
void AST::TranslationVisitor::Handle_IfElseStmt (AST_VISITOR_ARGS){ 
	
	if (!entering) {
		yyrule(6);

		Translate_IfElseStmt(yy[1].expr(), yy[2].quadNo(), yy[4].quadNo(), yy[5].quadNo());
		UNPARSABLE_GET(Unparse_IfElse(yy[1].expr(), yy[3].stmt(), yy[6].stmt()));
		DELTASYMBOLS.ResetTemp();
		yv = Translate_Stmts(yy[3].stmt(), yy[6].stmt()); 
		UNPARSABLE_SET(yv.stmt());

		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_WhileCondition (AST_VISITOR_ARGS){ 
	if (!entering) {
		Handle_Condition(AST_VISITOR_ACTUALS);
		Translate_WhilePrefix();
	}
}

//*****************************
// While: WHILE M(1.quadNo) Condition(2.expr) M(3.quadNo) Stmt(4.stmt)
//
void AST::TranslationVisitor::Handle_WhileStmt (AST_VISITOR_ARGS){ 
	if (entering)
		M();
	else {
		yyrule(4);

		util_ui32 line = LINE(DPTR(node)->GetChild(AST_CHILD_STMT));
		UNPARSABLE_GET(Unparse_While(yy[2].expr(), yy[4].stmt()));
		yv = Translate_WhileStmt(yy[2].expr(), yy[1].quadNo(), yy[3].quadNo(), yy[4].stmt(), line); 
		UNPARSABLE_SET(yv.stmt());

		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// ForInit: M(1.quadNo) ExprList(2.expr) M(3.quadNo) => Stmt M
//
void AST::TranslationVisitor::Handle_ForInit (AST_VISITOR_ARGS){
	if (entering) {
		Translate_ForOpening(); 
		M();
	}
	else {
		{
		yyrule(2);

		Unparse_ForInitList(yv = NEW_STMT, yy[2].expr());

		util_ui32 line = LINE(node);
		DASSERT(line);

		Translate_ForPrefix(yy[1].quadNo(), line);  
		Translate_BasicStmt(line);

		yyapply;
		}

		M();
	}
}

//*****************************
// ForSuffix: M(1.quadNo) ExprList(2.expr) N(3.quadNo) M(4.quadNo) => Stmt N M
//
void AST::TranslationVisitor::Handle_ForSuffix (AST_VISITOR_ARGS){
	
	if (entering) {
		if (DPTR(node)->GetTotalChildren()) {
			M();
		}
	}
	else {
		{
		util_ui32 n = DPTR(node)->GetTotalChildren(); // Subexpressions.
		yyrule(n ? 2 : 1); // M and ExprList already translated

		yv = NEW_STMT; 
		if (n) {		// Has an expression list.
			UNPARSABLE_GET(Unparse_ExprList(yy[2].expr()));
			UNPARSABLE_SET(yv.stmt()); 		
			SET_QUAD_LINE(1, true);
			Translate_BasicStmt(line);
		}
		else // Has a null expr for expression list.
			DASSERT(!yy[1].expr());
		yyapply;
		}

		N();
		M();
	}
}

//*****************************
// Condition: M Expr => Expr M
//
void AST::TranslationVisitor::Handle_ForCond (AST_VISITOR_ARGS){

	if (entering)
		M();
	else {
		{
		yyrule(2);
		yv = Translate_Condition(yy[2].expr()); 
		
		SET_QUAD_LINE(1, true);
		Translate_BasicStmt(line);

		yyapply;
		}

		M();
	}
}

//*****************************
//For: Init(1.stmt) M(2.quadNo) Cond(3.expr) M(4.quadNo) Suffix(5.stmt) N(6.quadNo) M(7.quadNo) Stmt(8.stmt)

void AST::TranslationVisitor::Handle_ForStmt (AST_VISITOR_ARGS){ 
	if (!entering) {
		yyrule(8);
		UNPARSABLE_GET(Unparse_For(yy[1].stmt(), yy[3].expr(), yy[5].stmt(), yy[8].stmt()));
		yv = Translate_ForStmt(yy[3].expr(), yy[2].quadNo(), yy[4].quadNo(), yy[7].quadNo(), yy[6].quadNo(), yy[8].stmt()); 
		UNPARSABLE_SET(yv.stmt());
		yyapply;
	}
} 

///////////////////////////////////////////////////////////
// ForeachPrefix: Lvalue(1.expr) [Lvalue(2.expr)] M(3.quadNo) Container(4.expr)
//
void AST::TranslationVisitor::Handle_ForeachContainer (AST_VISITOR_ARGS){ 
	if (entering)
		M();
	else {
		
		{
#define	VALUE_CONTAINER_FORM		3
#define	INDEX_VALUE_CONTAINER_FORM	4
#define	CONTAINER_POS				n
#define	M_POS						(n-1)
#define	VALUE_POS					(n == VALUE_CONTAINER_FORM ? 1 : 2)
#define	INDEX_POS					1

		util_ui32 n = DPTR(DPTR(node->GetParent()))->GetTotalChildren();
		DASSERT(n == VALUE_CONTAINER_FORM || n == INDEX_VALUE_CONTAINER_FORM);
		yyrule(n);
		
		SET_QUAD_LINE(M_POS, false);
		
		UNPARSABLE_GET(
			Unparse_ForeachPrefix(
				yy[VALUE_POS].expr(), 
				n == VALUE_CONTAINER_FORM ? NIL_EXPR : yy[INDEX_POS].expr(),
				yy[CONTAINER_POS].expr()
			)
		); 

		if (n == VALUE_CONTAINER_FORM)
			yv = Translate_ForeachPrefix(yy[VALUE_POS].expr(), NIL_EXPR, yy[CONTAINER_POS].expr()); 
		else
			yv = Translate_ForeachPrefix(yy[VALUE_POS].expr(),  yy[INDEX_POS].expr(), yy[CONTAINER_POS].expr()); 
		UNPARSABLE_SET(yv.stmt());		

		yyapply;

#undef	VALUE_CONTAINER_FORM
#undef	INDEX_VALUE_CONTAINER_FORM
#undef	CONTAINER_POS
#undef	M_POS		
#undef	VALUE_POS	
#undef	INDEX_POS	

		}

		M();
	}
} 

//*****************************
// Foreach: Prefix(1.stmt) M(2.quadNo) Stmt(3.stmt)
//
void AST::TranslationVisitor::Handle_ForeachStmt (AST_VISITOR_ARGS){ 

	if (!entering) {
		yyrule(3);

		UNPARSABLE_GET(Unparse_ForeachStmt(yy[1].stmt(), yy[3].stmt()));
		yv = Translate_ForeachStmt(yy[1].stmt(), yy[3].stmt(), yy[2].quadNo());
		SET_QUAD_LINE(2, false);
		UNPARSABLE_SET(yv.stmt());	

		yyapply;
	}
} 

///////////////////////////////////////////////////////////

void AST::TranslationVisitor::Handle_THROW (AST_VISITOR_ARGS){ 

	if (!entering) {
		yyrule(1);
		yv = Translate_THROW(yy[1].expr()); 
		Unparse_ExprStmt(yv.stmt(), THROW, yy[1].expr());
		yyapply;
	}
} 

//*****************************
// Try(1.quadNo) Stmt(2.stmt) Trap(3.quadNo) JumpOver(4.quadNo) Var(Expr)
//
void AST::TranslationVisitor::Handle_ExceptionVar (AST_VISITOR_ARGS){ 
	if (entering) {
		yypush(Translate_TRAP());			// Trap quad
		yypush(Translate_TrapJumpOver());	// JumpOver quad
	}
	else {
		yyrule(4);
		if (yv = Translate_Lvalue(NAME(node))) 
			DPTR(yv.expr())->SetInitialised();

		Translate_TrapStart(yy[1].quadNo(), yy[3].quadNo(), yv.expr());
		yypreserve;
		yyapply;
	}
} 

//*****************************
// Try(1.quadNo) Stmt(2.stmt) Trap(3.quadNo) JumpOver(4.quadNo) Var(5.expr) Stmt(6.stmt)
//
void AST::TranslationVisitor::Handle_Exception (AST_VISITOR_ARGS){ 
	if (entering)
		yypush(Translate_TRY());	// Try
	else {
		yyrule(6);

		Translate_TrapEnd(yy[4].quadNo()); ; 
		UNPARSABLE_GET(Unparse_TryTrap(yy[2].stmt(), yy[6].stmt(), yy[5].expr()));
		DELTASYMBOLS.ResetTemp();
		yv = Translate_Stmts(yy[2].stmt(), yy[6].stmt());
		UNPARSABLE_SET(yv.stmt());

		yyapply;
	}
} 

///////////////////////////////////////////////////////////

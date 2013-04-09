%{
// Delta syntax directed parsing for AST generation.
// Extended December 2009, added lambda functions.
// ScriptFigher Project.
// A. Savidis, September 2009.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "DDebug.h"
#include "DeltaStdDefs.h"
#include "ASTNode.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"
#include "ASTCreationActions.h"
#include "DescriptiveParseErrorHandler.h"
#include "LibraryNamespace.h"
#include "ParseParms.h"
#include "CompilerStringHolder.h"
#include "DeltaScanner.h"
#include "DeltaSyntax.h"

#define yyFlexLexer DeltaScannerFlexLexer

using namespace AST;

#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

extern int DeltaSyntax_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer);

///////////////////////////////////////////////////////////
// Helper macros to get specific components from the lexer
// component directory.
#define DIRECTORY								(lexer.GetDirectory())

#define GET_DESCRIPTIVE_ERROR_HANDLER			DESCRIPTIVE_ERROR_HANDLER_EX(DIRECTORY)
#define ASTCREATOR_BY_CONTEXT					ASTCREATOR_EX(DIRECTORY)
#define PARSEPARMS_BY_CONTEXT					PARSEPARMS_EX(DIRECTORY)
#define STRINGHOLDER_BY_CONTEXT					STRINGHOLDER_EX(DIRECTORY)

///////////////////////////////////////////////////////////

static void DeltaSyntax_yyerror (YYLTYPE* yylloc, yyFlexLexer& lexer, const char* unused)
	{ GET_DESCRIPTIVE_ERROR_HANDLER.HandleSyntaxError(); }

#define	DYNAMIC_STRING(s) \
	STRINGHOLDER_BY_CONTEXT.StringWithLateDestruction(ucopystr(s))

void SET_LOCATION(AST::Node* node, const YYLTYPE& start, const YYLTYPE& end) {
	AST::Node::Location location(start.first_line, end.last_line, start.first_column, end.last_column);
	DPTR(node)->SetLocation(location);
}

void SET_LOCATION(YYLTYPE& pos, const YYLTYPE& start, const YYLTYPE& end) {
	pos.first_line = start.first_line;
	pos.last_line = end.last_line;
	pos.first_column = start.first_column;
	pos.last_column = end.last_column;
}

#define	MAKE_FUNCTION_EXPR(_p, _pp)										\
	if (true) {															\
		PE(S_FUNC_(GetFuncClass(_p)));									\
		_pp = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionExpression(_p);	\
		(_pp)->SetLocation((_p)->GetLocation());							\
	} else%}

%union {
	double				numberConst;
	util_ui32			line;
	util_ui32			count;
	char*				dynamicStr;
	const char*			constStr;
	AST::NodeList*		nodeList;
	AST::Node*			node;
}

%start	DeltaCode
%type	<node>			ConstValue Lvalue TableContent Expression TablePrefix  ExceptionVar
%type	<node>			Primary FunctionCall TableConstructor Term TableContentDot TableContentBracket
%type	<node>			FunctionCallObject FunctionAndTableObject ActualArgument ReturnValue
%type	<node>			TableContentBoundedDot TableContentBoundedBracket TableObject BasicExprStmt BasicNonExprStmt
%type	<node>			FunctionName Function FormalArgsNode IndexedValues TableIndices TableValues QuotedIndexedValues TableElement UnindexedValue
%type	<node>			ArithmeticExpression AssignExpression BooleanExpression RelationalExpression
%type	<node>			Condition ForCondition DottedIdent  IndexContent IndexExpression 
%type	<node>			TernaryExpression TernaryCondition TernarySelection1 TernarySelection2
%type	<node>			NewAttribute AttributeSet AttributeGet ConstDefExpression ForeachCont ForeachValue
%type	<node>			Stmt BasicStmt QuotedStmt NoContextStmt NonFunctionQuotedStmt MiscStmt
%type	<node>			IfStmt Compound  ExceptionStmt ThrowStmt ForStmt WhileStmt ForSuffix ForInitList
%type	<node>			AssertStmt Assertion BreakStmt ContinueStmt ReturnStmt ExpressionListStmt
%type	<node>			ForeachStmt OperatorIndex Stmts QuotedStmts NonEmptyStmts StringConstUsed FunctionElement
%type	<node>			UsingNamespace UsingByteCodeLibrary UsingSpecifications UsingDirective
%type	<node>			StringConst StringifyDottedIdents StringifyNamespaceIdent DotIndex SpecialDotIndex BracketIndex
%type	<node>			LambdaFunction LambdaCode LambdaStmt ContentExpression IdentIndexElement QuotedIdentIndexElement
%type	<nodeList>		IdentList NamespacePath FormalArgs
%type	<nodeList>		ExpressionList QuotedExpressionList QuotedElementList QuotedElements TableElements NonEmptyActualArgumentsList ContentList
%type	<nodeList>		ActualArgumentsList ActualArguments IndexedList QuotedIndexedList
%type	<constStr>		IDENT ATTRIBUTE_IDENT FunctionClass FuncLinkage
%type	<constStr>		StringIdent OperatorMethod OpString DottedOpString KwdIdent
%type	<line>			LN
%type	<count>			MultipleEscapes
%type	<node>			QuasiQuoted NonExprQuotedElement MetaExpression MetaGeneratedCode MetaStmt Ident AttributeId
%type	<node>			ConstAttributeId IdentIndex QuotedIdentIndex AttributeIdent FormalArgsSuffix

%token	<numberConst>	NUMBER_CONST
%token	<dynamicStr>	STRING_CONST

%token		FUNCTION RETURN ONEVENT IF ELSE WHILE FOR FOREACH NIL LOCAL USING
%token		AND NOT OR PLUSPLUS MINUSMINUS LAMBDA  TRY TRAP THROW ASSERT
%token		TRUE FALSE BREAK CONTINUE STATIC CONST METHOD SELF ARGUMENTS 
%token		LAMBDA_REF OPERATOR NEWSELF SET GET
%token		LT GT LE GE EQ NE DOT DOUBLE_DOT TRIPLE_DOT DOT_ASSIGN DOT_EQUAL DOT_CAST DOT_EQUAL_RHS
%token		UMINUS ADD SUB MUL DIV MOD ATTRIBUTE STRINGIFY GLOBAL_SCOPE
%token		ASSIGN ADD_A SUB_A MUL_A DIV_A MOD_A  DOUBLE_LB DOUBLE_RB
%token		'(' ')' '[' ']' '{' '}' '?' ':' ','
%token		CALL CAST SEMI IDENT ATTRIBUTE_IDENT
%token		ADD_POSTFIX SUB_POSTFIX MUL_POSTFIX DIV_POSTFIX MOD_POSTFIX
%token		GT_POSTFIX LT_POSTFIX NE_POSTFIX EQ_POSTFIX GE_POSTFIX LE_POSTFIX
%token		PARENTHESIS SQUARE_BRACKETS
%token		META_LSHIFT META_RSHIFT META_ESCAPE META_INLINE META_EXECUTE META_RENAME

// Pseudo-tokens for parsing specific grammar elements
%token		PARSE_QUOTED_ELEMENTS

// Priorities
%right		ASSIGN ADD_A SUB_A MUL_A DIV_A MOD_A
%left		OR 
%left		AND
%nonassoc	EQ NE
%nonassoc	GE LE GT LT
%left		ADD SUB
%left		MUL DIV MOD
%right		NOT PLUSPLUS MINUSMINUS UMINUS
%nonassoc	DOT_ASSIGN
%right		META_ESCAPE
%left		DOT
%left		SQUARE_BRACKETS
%left		PARENTHESIS

%output="DeltaSyntax.cpp"
%name-prefix="DeltaSyntax_yy"
%debug
%defines
%verbose
%pure-parser
%parse-param {yyFlexLexer& lexer}
%lex-param   {YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer}
%locations
%%

/**************************************************************************/
/* PROGRAM*/


DeltaCode:					Stmts	{
								Node* program = ASTCREATOR_BY_CONTEXT.MakeNode_Program($1);
								if ($1)
									SET_LOCATION(program, @1, @1);
								ASTCREATOR_BY_CONTEXT.SetSyntaxTree(program);
							}
						|	PARSE_QUOTED_ELEMENTS QuasiQuoted { ASTCREATOR_BY_CONTEXT.SetSyntaxTree($2); }
						;
										
Stmts:						Stmts Stmt		{
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts($1, $2);
								if ($1) {
									SET_LOCATION($$, @1, @2);
									SET_LOCATION(@$, @1, @2);
								}
								else {
									SET_LOCATION($$, @2, @2);
									SET_LOCATION(@$, @2, @2);								
								}
							}
						|	/*no stmts*/	{ $$ = NIL_NODE; }
						;
						
LN:						{ $$ = PARSEPARMS_BY_CONTEXT.GetLine(); }
						;

/**************************************************************************/
/* FUNCTIONS, METHODS, LAMBDA FUNCTIONS */

Function:				FunctionClass 
							{ SM(S_FUNC_($1)); PE2(T_FUNC_($1), T_LOCAL_OR_FUNCNAME); }
						FuncLinkage FunctionName
							{ PE(T_LPAR); SG(S_FARGS); }
						FormalArgsNode 
							{ SG(S_BLOCK); }
						Compound LN {
							EG(S_BLOCK);
							EM(S_FUNC_($1));
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Function($1, $4, $3, $6, $8);
							DPTR($$)->SetLine($9);
							SET_LOCATION($$, @1, @8);
						}
						;

FuncLinkage:				LOCAL	{ $$ = AST_VALUE_FUNCLINKAGE_NO_EXPORT;			}
						|			{ $$ = AST_VALUE_FUNCLINKAGE_DEFAULT_EXPORT;	}
						;

FunctionClass:				FUNCTION	
								{ $$ = AST_VALUE_FUNCCLASS_PROGRAMFUNCTION;	}
						|	ONEVENT		
								{ $$ = AST_VALUE_FUNCCLASS_ONEVENT;			}			
						|	METHOD		
								{ $$ = AST_VALUE_FUNCCLASS_METHOD;			}			
						;

Ident:						META_RENAME IDENT	{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Name($2, true);	SET_LOCATION($$, @1, @2); }
						|	IDENT				{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Name($1);		SET_LOCATION($$, @1, @1); }
						|	MetaGeneratedCode	{ $$ = $1; }
						;

FunctionName:				Ident
								{ $$ = $1; }
						|	OPERATOR OperatorMethod
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Name($2); SET_LOCATION($$, @1, @2); }
						|	ATTRIBUTE {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Name(AST_VALUE_TOSTRING_SYMBOLIC_NAME);
								SET_LOCATION($$, @1, @1);
							}
						|	/* anonymous function */	
								{ unullify($$); }
						;

OperatorMethod:				OpString		{ $$ = $1;							}
						|	DOT				{ $$ = DELTA_OPERATOR_OBJECT_GET;	}
						|	DOT_ASSIGN		{ $$ = DELTA_OPERATOR_OBJECT_SET;	}
						;

IdentList:					IdentList ',' 
								{ PE2(T_COMMA, T_IDENT); } 
							Ident
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $4); }
						|	Ident
								{ PE(T_IDENT); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, $1); }
						|	/* no formal arguments */
								{ unullify($$); }
						;

FormalArgsSuffix:			')'	
								{ EG(S_FARGS); PE(T_RPAR); unullify($$); }
						|	TRIPLE_DOT
								{ EG(S_FARGS); PE2(T_TDOT, T_RPAR); }
							')' { 
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Name(AST_VALUE_VARARGS_FORMAL_NAME);
								SET_LOCATION($$, @1, @1);
							}
						;

FormalArgsNode:				FormalArgs {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_FormalArgs($1);
								SET_LOCATION($$, @1, @1);
							}
						;

FormalArgs:					CALL	
								{ EG(S_FARGS); PE(T_RPAR); unullify($$); }
						|	'('	IdentList 
								{ OE(T_LPAR_OR_TDOT); }
							FormalArgsSuffix
								{ $$ = $4 ? ASTCREATOR_BY_CONTEXT.MakeNode_List($2, $4) : $2; }
						|	/* empty, arg signature can be skipped */
								{ EG(S_FARGS); PE(T_RPAR); unullify($$); }
						;

CompoundOpening:		'{'	 { PE(T_LBC); SET_LOCATION(@$, @1, @1); }
						;
						
Compound:				CompoundOpening Stmts { PE(T_RBC); } '}'
							{  $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Compound($2); SET_LOCATION($$, @1, @4); SET_LOCATION(@$, @1, @4); }
						;

LambdaFunction:			LambdaPrefix FormalArgsNode
							{ PE(T_LBC); }
						LambdaCode {
							EM(S_FUNC_(AST_VALUE_FUNCCLASS_PROGRAMFUNCTION));
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_LambdaFunction($2, $4);
							SET_LOCATION($$, @1, @4);
						}
						;
						
LambdaPrefix:			LAMBDA {
							SM(S_FUNC_(AST_VALUE_FUNCCLASS_PROGRAMFUNCTION)); 
							PE2(T_LAMBDA, T_LPAR); 
							SG(S_FARGS);
						}
						;

LambdaCode:				'{' { PE(S_EXPR); } LambdaStmt '}' { $$ = $3; }
						;
						
LambdaStmt:				LN Expression { PE(T_RBC); $$ = $2; DPTR($$)->SetLine($1); SET_LOCATION(@$, @2, @2); }
						;	
								
/**************************************************************************/
/* STMTS */

Semi:						{ PE(T_SEMI); }	
						SEMI
							{ SET_LOCATION(@$, @2, @2); }
						;
	
ExpressionListStmt:			{ SM(S_STMT); }
						ExpressionList Semi
							{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_ExpressionListStmt($2); SET_LOCATION($$, @2, @3); SET_LOCATION(@$, @2, @3); }
						;

Stmt:						LN BasicStmt			{
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt($2);  DPTR($$)->SetLine($1);
								SET_LOCATION($$, @2, @2); SET_LOCATION(@$, @2, @2);
							}
						|	Compound				{ $$ = $1; }
						|	Function				{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionStmt($1); SET_LOCATION($$, @1, @1); }
						|	SEMI					{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_EmptyStmt();  SET_LOCATION($$, @1, @1);}
						|	UsingDirective			{ $$ = $1; }
						|	MetaStmt				{ $$ = $1; }
						;

BasicStmt:					BasicExprStmt		{ $$ = $1; }
						|	BasicNonExprStmt	{ $$ = $1; }
						;

BasicExprStmt:				ExpressionListStmt	{ EM(S_STMT); $$ = $1; }
						|	AssertStmt			{ EM(S_STMT); $$ = $1; }
						|	ReturnStmt			{ $$ = $1; }
						;


BasicNonExprStmt:			WhileStmt			{ $$ = $1;}
						|	ForStmt				{ $$ = $1;}
						|	ForeachStmt			{ $$ = $1; }
						|	IfStmt				{ $$ = $1;}
						|	BreakStmt			{ EM(S_STMT); $$ = $1; }
						|	ContinueStmt		{ EM(S_STMT); $$ = $1;}
						|	ExceptionStmt		{ $$ = $1; }
						|	ThrowStmt			{ $$ = $1; }
						;

/* ASSERT*******************/

Assertion:				ASSERT
							{ SM(S_ASSRT); SG(S_EXPR); }
						Expression {
							EG(S_EXPR);
							PE(S_EXPR);
							EM(S_ASSRT);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_AssertStmt($3);
							SET_LOCATION($$, @1, @3);
						}

AssertStmt:					{ SM(S_STMT); PE(S_ASSRT);	}	
						Assertion Semi
							{ $$ = $2; SET_LOCATION(@$, @2, @3); }
						;

/* RETURN*******************/

ReturnPrefix:			RETURN 
							{ SM(S_RET); }	
						;
ReturnValue:				Expression Semi
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Return($1); }
						|	Function
								{ MAKE_FUNCTION_EXPR($1, $$); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Return($$); }
						;

ReturnStmt:					ReturnPrefix
								{ PE2(T_RET, S_EXPR); } 
							ReturnValue
								{ EM(S_RET); $$ = $3; SET_LOCATION($$, @1, @3); }
						|	ReturnPrefix Semi
								{ EM(S_RET); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Return(); SET_LOCATION($$, @1, @2); }
						;

/* USING********************/

UsingDirective:			USING UsingSpecifications	{ $$ = $2; SET_LOCATION($$, @1, @2); }
						;

UsingSpecifications:		UsingNamespace			{ $$ = $1; }
						|	UsingByteCodeLibrary	{ $$ = $1; }
						;
				
UsingNamespace:				NamespacePath Ident SEMI
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_UsingNamespace($1, $2); }
						|	Ident SEMI
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_UsingNamespace(NIL_NODELIST, $1); }
						;

UsingByteCodeLibrary:		
						STRINGIFY Ident SEMI LN 
							{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_UsingByteCodeLibrary($2); DPTR($$)->SetLine($4); SET_LOCATION(@$, @1, @3); }
						;

/**************************************************************************/
/* EXPRESSIONS */
							
Expression:		AssignExpression		{ $$ = $1; }
			|	ConstDefExpression		{ $$ = $1; }
			|	RelationalExpression	{ $$ = $1; }
			|	BooleanExpression		{ $$ = $1; }
			|	ArithmeticExpression	{ $$ = $1; }
			|	TernaryExpression		{ $$ = $1; }
			|	Term					{ $$ = $1; }
			;
							
ConstDefExpression:		CONST		{ SM(S_CONST); PE(T_IDENT); } 
						Ident		{ PE(T_ASSIGN); }
						ASSIGN		{ PE(S_EXPR); } 
						Expression	{
							EM(S_CONST);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ConstDefExpression($3, $7);
							SET_LOCATION($$, @1, @7);
						}
						;

AssignExpression:			Lvalue ASSIGN	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ASSIGN, S_EXPR); }	
							Expression {
								EM(S_ASSIGNEXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_AssignExpr($1, $4);
								SET_LOCATION($$, @1, @4);
							}
									
						|	Lvalue ADD_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ADDA, S_EXPR); }		
							Expression {
								EM(S_ASSIGNEXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_ADD_ASSIGN);
								SET_LOCATION($$, @1, @4);
							}
									
						|	Lvalue MUL_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MULA, S_EXPR); }		
							Expression {
								EM(S_ASSIGNEXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_MUL_ASSIGN);
								SET_LOCATION($$, @1, @4);
							}
									
						|	Lvalue SUB_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_SUBA, S_EXPR); }		
							Expression {
								EM(S_ASSIGNEXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_SUB_ASSIGN);
								SET_LOCATION($$, @1, @4);
							}
									
						|	Lvalue DIV_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_DIVA, S_EXPR); }		
							Expression {
								EM(S_ASSIGNEXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_DIV_ASSIGN);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Lvalue MOD_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MODA, S_EXPR); }	
							Expression {
								EM(S_ASSIGNEXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_MOD_ASSIGN);
								SET_LOCATION($$, @1, @4);
							}
						;

RelationalExpression:		Expression GT 
								{ SM(S_RELAT); PE3(S_EXPR, T_GT, S_EXPR); } 
							Expression {
								EM(S_RELAT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr($1, $4, AST_TAG_OP_GT);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression LT 
								{ SM(S_RELAT); PE3(S_EXPR, T_LT, S_EXPR); } 
							Expression {
								EM(S_RELAT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr($1, $4, AST_TAG_OP_LT);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression GE 
								{ SM(S_RELAT); PE3(S_EXPR, T_GE, S_EXPR); } 
							Expression {
								EM(S_RELAT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr($1, $4,AST_TAG_OP_GE);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression LE 
								{ SM(S_RELAT); PE3(S_EXPR, T_LE, S_EXPR); } 
							Expression {
								EM(S_RELAT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr($1, $4, AST_TAG_OP_LE);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression EQ 
								{ SM(S_RELAT); PE3(S_EXPR, T_EQ, S_EXPR); } 
							Expression {
								EM(S_RELAT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr($1, $4, AST_TAG_OP_EQ);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression NE 
								{ SM(S_RELAT); PE3(S_EXPR, T_NE, S_EXPR); } 
							Expression {
								EM(S_RELAT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr($1, $4, AST_TAG_OP_NE);
								SET_LOCATION($$, @1, @4);
							}
						;

BooleanExpression:			Expression AND
								{ SM(S_LOGICAL); PE3(S_EXPR, T_AND, S_EXPR); }
							Expression {
								EM(S_LOGICAL);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ExprANDExpr($1, $4);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression OR
								{ SM(S_LOGICAL); PE3(S_EXPR, T_OR, S_EXPR); }
							Expression {
								EM(S_LOGICAL);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ExprORExpr($1, $4);
								SET_LOCATION($$, @1, @4);
							}
						;
							
ArithmeticExpression:		Expression ADD 
								{ SM(S_ARITH); PE3(S_EXPR, T_ADD, S_EXPR); } 
							Expression {
								EM(S_ARITH);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression($1, $4, AST_TAG_OP_ADD);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression SUB 
								{ SM(S_ARITH); PE3(S_EXPR, T_SUB, S_EXPR); } 
							Expression {
								EM(S_ARITH);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression($1, $4, AST_TAG_OP_SUB);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression DIV 
								{ SM(S_ARITH); PE3(S_EXPR, T_DIV, S_EXPR); } 
							Expression {
								EM(S_ARITH);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression($1, $4,AST_TAG_OP_DIV);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression MUL 
								{ SM(S_ARITH); PE3(S_EXPR, T_MUL, S_EXPR); } 
							Expression {
								EM(S_ARITH);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression($1,  $4, AST_TAG_OP_MUL);
								SET_LOCATION($$, @1, @4);
							}
								
						|	Expression MOD 
								{ SM(S_ARITH); PE3(S_EXPR, T_MOD, S_EXPR); } 
							Expression {
								EM(S_ARITH);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression($1, $4, AST_TAG_OP_MOD);
								SET_LOCATION($$, @1, @4);
							}
						;

TernaryExpression:		'(' TernaryCondition TernarySelection1 ':' TernarySelection2 ')' %prec ASSIGN
							{ EM(S_TERNARY); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Ternary($2, $3, $5); SET_LOCATION($$, @1, @6); }
						;

TernaryCondition:		Expression '?'
							{ SM(S_TERNARY); PE3(S_EXPR, T_QMARK, S_EXPR); $$ = $1; }
						;

TernarySelection1:		Expression
							{ PE(T_COLON); $$ = $1; }
						;
						
TernarySelection2:			{ PE(S_EXPR); } 
						Expression
							{ PE(T_RPAR); $$ = $2; SET_LOCATION(@$, @2, @2); }
						;
					
Term:						Lvalue PLUSPLUS {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_TermLvalueArith($1, AST_LVALUE_PLUSPLUS);
								SET_LOCATION($$, @1, @2);
							}
								
						|	PLUSPLUS
								{ SM(S_EXPR); PE2(T_PLUSPLUS, S_LVAL); } 
							Lvalue {
								EM(S_EXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_TermLvalueArith($3, AST_PLUSPLUS_LVALUE);
								SET_LOCATION($$, @1, @3);
							}
								
						|	Lvalue MINUSMINUS {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_TermLvalueArith($1, AST_LVALUE_MINUSMINUS);
								SET_LOCATION($$, @1, @2);
							}
								
						|	MINUSMINUS
								{ SM(S_EXPR); PE2(T_MINUSMINUS, S_LVAL); } 
							Lvalue {
								EM(S_EXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_TermLvalueArith($3, AST_MINUSMINUS_LVALUE);
								SET_LOCATION($$, @1, @3);
							}
								
						|	SUB
								{ SM(S_EXPR); PE2(T_SUB, S_EXPR); }
							Expression %prec UMINUS {
								EM(S_EXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_UMINUSExpression($3);
								SET_LOCATION($$, @1, @3);
							}
								
						|	NOT 
								{ SM(S_EXPR); PE2(T_NOT, S_EXPR); } 
							Expression {
								EM(S_EXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_NOTExpression($3);
								SET_LOCATION($$, @1, @3);
							}
						
						|	Primary				
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_PrimaryExpression($1); SET_LOCATION($$, @1, @1); }

						|	MetaExpression
								{ $$ = $1; }
						;

Primary:					FunctionCallObject		{ $$ = $1; }
						|	ConstValue				{ $$ = $1; SET_LOCATION($$, @1, @1); }
						|	LambdaFunction			{ $$ = $1; }
						;

ConstValue:					NUMBER_CONST	{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_ConstValue($1);		}
						|	NIL				{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_ConstValue();		}
						|	TRUE			{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_ConstValue(true);	}
						|	FALSE			{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_ConstValue(false);	}
						;

OpString:					ADD				{ $$ = "+";			}
						|	SUB				{ $$ = "-";			}
						|	MUL				{ $$ = "*";			}
						|	DIV				{ $$ = "/";			}
						|	MOD				{ $$ = "%";			}
						|	GT				{ $$ = ">";			}
						|	LT				{ $$ = "<";			}
						|	NE				{ $$ = "!=";		}
						|	EQ				{ $$ = "==";		}
						|	GE				{ $$ = ">=";		}
						|	LE				{ $$ = "<=";		}
						|	ASSIGN			{ $$ = "=";			}
						|	ADD_POSTFIX		{ $$ = "+_";		}
						|	SUB_POSTFIX		{ $$ = "-_";		}
						|	MUL_POSTFIX		{ $$ = "*_";		}
						|	DIV_POSTFIX		{ $$ = "/_";		}
						|	MOD_POSTFIX		{ $$ = "%_";		}
						|	GT_POSTFIX		{ $$ = ">_";		}
						|	LT_POSTFIX		{ $$ = "<_";		}
						|	NE_POSTFIX		{ $$ = "!=_";		}
						|	EQ_POSTFIX		{ $$ = "==_";		}
						|	GE_POSTFIX		{ $$ = ">=_";		}
						|	LE_POSTFIX		{ $$ = "<=_";		}
						|	CALL			{ $$ = "()";		}
						|	CAST			{ $$ = "=()";		}
						;
KwdIdent:					IF				{ $$ = "if";		}
						|	ELSE			{ $$ = "else";		}
						|	FUNCTION		{ $$ = "function";	} 
						|	RETURN			{ $$ = "return";	} 
						|	ONEVENT			{ $$ = "onevent";	} 
						|	WHILE			{ $$ = "while";		} 
						|	FOR				{ $$ = "for";		} 
						|	FOREACH			{ $$ = "foreach";	} 
						|	NIL				{ $$ = "nil";		} 
						|	LOCAL			{ $$ = "local";		}
						|	AND				{ $$ = "and";		} 
						|	NOT				{ $$ = "not";		} 
						|	OR				{ $$ = "or";		} 
						|	LAMBDA			{ $$ = "lambda";	} 
						|	TRY				{ $$ = "try";		} 
						|	TRAP			{ $$ = "trap";		} 
						|	THROW			{ $$ = "throw";		} 
						|	USING			{ $$ = "using";		} 
						|	ASSERT			{ $$ = "assert";	}
						|	TRUE			{ $$ = "true";		} 
						|	FALSE			{ $$ = "false";		} 
						|	BREAK			{ $$ = "break";		} 
						|	CONTINUE		{ $$ = "continue";	} 
						|	STATIC			{ $$ = "static";	} 
						|	CONST			{ $$ = "const";		} 
						|	METHOD			{ $$ = "method";	} 
						|	SELF			{ $$ = "self";		} 
						|	ARGUMENTS		{ $$ = "arguments"; }
						;
				
StringConst:				StringConst STRING_CONST
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_StringConst($1, $2); SET_LOCATION($$, @1, @2); }
						|	STRING_CONST
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_StringConst($1); SET_LOCATION($$, @1, @1); }
						;

Stringify:					STRINGIFY 
								{ EI(S_EXPR); PE(T_STRINGIFY); }
						;
						
StringIdent:				IDENT		{ $$ = $1; }
						|	KwdIdent	{ $$ = $1; }
						;

StringifyDottedIdents:		Stringify StringIdent
								{ PE(T_IDENT); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_StringifyDottedIdents($2); }
						|	StringifyDottedIdents DOT 
								{ PE2(T_DOT, T_IDENT); }
							StringIdent
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_StringifyDottedIdents($1, $4); }
						;
						
StringifyNamespaceIdent:	Stringify NamespacePath Ident {
								EG(S_NAMESPACE);
								PE(T_IDENT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_StringifyNamespaceIdent($2, $3);
								SET_LOCATION($$, @1, @3);
							}
						;

StringConstUsed:			StringConst 
								{ PE(T_STRINGCONST); $$ = $1; }
						| 	StringifyDottedIdents
								{ $$ = $1; }
						|	StringifyNamespaceIdent
								{ $$ = $1; }
						;

/**************************************************************************/
/* FUNCTION CALLS */
				
FunctionAndTableObject:		SELF {
								EI(S_EXPR); PE(T_SELF);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_SELF();
								SET_LOCATION($$, @1, @1);
							}
						|	LAMBDA_REF {
								EI(S_EXPR); PE(T_LAMBDA_REF);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_LAMBDA_REF();
								SET_LOCATION($$, @1, @1);
							}
						|	NEWSELF {
								EI(S_EXPR); PE(T_NEWSELF);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_NEWSELF();
								SET_LOCATION($$, @1, @1);
							}
						|	ARGUMENTS {
								EI(S_EXPR); PE(T_ARGUMENTS);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ARGUMENTS();
								SET_LOCATION($$, @1, @1);
							}
						|	Lvalue { $$ = $1; }
						|	TableConstructor	
								{ PE(S_TABLE); $$ = $1; }
						|	'(' Expression		
								{ PE3(T_LPAR, S_EXPR, T_RPAR); } 
							')'					
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_ParenthesisedExpr($2); SET_LOCATION($$, @1, @3); }
						|	FunctionCall		
								{ PE(S_CALL); $$ = $1; }
						|	'(' Function		
								{ SM(S_EXPR); PE3(T_LPAR, S_FUNC, T_RPAR); }  
							')' {
								EM(S_EXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionParenthesisForm($2);
								SET_LOCATION($$, @1, @4);
							}
						;

/*ACTUAL ARGUMENTS***********/

ActualArgumentsList:		/* Empty */	
								{ unullify($$); }
						|	NonEmptyActualArgumentsList 
								{ $$ = $1; }
						;
					
NonEmptyActualArgumentsList:	
							NonEmptyActualArgumentsList ','		
								{ PE(T_COMMA); }  
							ActualArgument						
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $4); }
						|	ActualArgument						
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, $1); }
						;

ActualArguments:			'('						
								{ SM(S_CALL); PE2(S_EXPR, T_LPAR); SG(S_AARGS); }
							ActualArgumentsList		
								{ PE(T_RPAR); }
							')'	%prec PARENTHESIS
								{ EG(S_AARGS); EM(S_CALL); $$ = $3; }
						|	CALL					
								{ PE2(T_LPAR, T_RPAR); unullify($$); }
						;
					
ActualArgument:					{ PE(S_EXPR); }
							Expression
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Argument($2); SET_LOCATION($$, @1, @2); SET_LOCATION(@$, @2, @2); }
						|	'|'
								{ PE2(T_BAR, S_EXPR); }
							Expression
								{ PE(T_BAR); }
							'|'
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_LateBoundArgument($3); SET_LOCATION($$, @1, @5); }
						|	TRIPLE_DOT
								{ PE(T_TDOT); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_TRIPLE_DOT(); SET_LOCATION($$, @1, @1); }
						|	Function
								{ MAKE_FUNCTION_EXPR($1, $$); SET_LOCATION($$, @1, @1); }
						;

/*FUNCTION CALL EXPRESSION***/

FunctionCallObject:			FunctionAndTableObject	{ $$ = $1; }
						|	StringConstUsed			{ EI(S_EXPR); $$ = $1; }
						;

FunctionCall:			FunctionCallObject ActualArguments {
							Node* actuals = ASTCREATOR_BY_CONTEXT.MakeNode_ActualArguments($2);
							SET_LOCATION(actuals, @2, @2);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionCall($1, actuals);
							SET_LOCATION($$, @1, @2);
						}
						;

ExpressionList:				ExpressionList ',' LN Expression
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $4); DPTR($4)->SetLine($3); }
						|	LN Expression {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, $2); DPTR($2)->SetLine($1);
								SET_LOCATION(@$, @2, @2);
							}
						;

/**************************************************************************/
/* LVALUES */

NamespacePath:				GLOBAL_SCOPE {
								EI(S_EXPR); SG(S_NAMESPACE); PE2(T_GLOBAL, T_IDENT);
								Node* global = ASTCREATOR_BY_CONTEXT.MakeNode_Name(DELTA_LIBRARYNAMESPACE_SEPARATOR);
								SET_LOCATION(global, @1, @1);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, global);
							}
						|	Ident GLOBAL_SCOPE {
								EI(S_EXPR); SG(S_NAMESPACE); PE3(T_IDENT, T_GLOBAL, T_IDENT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, $1);
							}
						|	NamespacePath Ident GLOBAL_SCOPE {
								PE2(T_GLOBAL, T_IDENT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $2);
							}
						;

Lvalue:						Ident {
								EI(S_EXPR); PE(T_IDENT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Lvalue($1);
								SET_LOCATION($$, @1, @1);
							}

						|	NamespacePath Ident {
								EG(S_NAMESPACE);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_NamespaceLvalue($1, $2);
								SET_LOCATION($$, @1, @2);
							}

						|	AttributeId
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_AttrLvalue($1); SET_LOCATION($$, @1, @1); }

						|	STATIC 
								{ EI(S_EXPR); PE2(T_STATIC, T_IDENT); } 
							Ident
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_StaticLvalue($3); SET_LOCATION($$, @1, @3); }

						|	LOCAL 
								{ EI(S_EXPR); PE2(T_LOCAL, T_IDENT); } 
							Ident
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_LocalLvalue($3); SET_LOCATION($$, @1, @3); }
																					
						|	TableContent
								{ $$ = $1; }
						;

TableObject:				FunctionAndTableObject { $$ = $1; }	
						;
									
TableContentDot:		TableObject DOT			
							{ PE(T_DOT); OE(S_DOTINDEX); $$ = $1; }
						;


TableContentBoundedDot:	TableObject DOUBLE_DOT
							{ PE(T_DDOT); OE(S_DOTINDEX); $$ = $1; }
						;

TableContentBracket:	TableObject '['	
							{ PE(T_LB); OE(S_BRACKET_INDEX); $$ = $1; }
						;

TableContentBoundedBracket:	
						TableObject DOUBLE_LB
							{ PE(T_DLB); OE(S_BRACKET_INDEX); $$ = $1; }
						;

DottedOpString			:	DOT_ASSIGN		{ $$ = ".=";	}
						|	DOT_EQUAL		{ $$ = ".==";	}
						|	DOT_CAST		{ $$ = ".=()";	}
						|	DOT_EQUAL_RHS	{ $$ = ".==_";	}
						;
						
DotIndex:					IDENT	
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexIdent($1); SET_LOCATION($$, @1, @1); }
						|	KwdIdent	
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexIdent($1); SET_LOCATION($$, @1, @1); }
						|	OpString
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexOpString($1); SET_LOCATION($$, @1, @1); }
						|	StringConstUsed
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexStringConst($1); SET_LOCATION($$, @1, @1); }
						;

SpecialDotIndex:		DottedOpString
							{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexOpString($1+1); SET_LOCATION($$, @1, @1);  }	
						;
						
BracketIndex:				Expression
								{ $$ = $1; }
						|	OperatorMethod
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_BracketIndexOperatorMethod($1); SET_LOCATION($$, @1, @1);  }
						;
												
/*TABLECONTENT**************/

TableContent:				TableContentDot DotIndex {
								PE(S_DOTINDEX_(DPTR($2)->GetTag())); 
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_TableContentDot($1, $2);
								SET_LOCATION($$, @1, @2);
							}
						|	TableObject SpecialDotIndex {
								PE(S_DOTINDEX_(DPTR($2)->GetTag())); 
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_TableContentDot($1, $2);
								SET_LOCATION($$, @1, @2);
							}
						|	TableContentBoundedDot DotIndex {
								PE(S_DOTINDEX_(DPTR($2)->GetTag())); 
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_BoundedTableContentDot($1, $2);
								SET_LOCATION($$, @1, @2);
							}
						|	TableContentBracket BracketIndex 
								{ PE2(S_BRACKET_INDEX_(DPTR($2)->GetTag()), T_RB); } 
							']'	%prec SQUARE_BRACKETS
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_TableContentBracket($1, $2); SET_LOCATION($$, @1, @4); }
						|	TableContentBoundedBracket BracketIndex 
								{ PE2(S_BRACKET_INDEX_(DPTR($2)->GetTag()), T_DRB); } 
							DOUBLE_RB %prec SQUARE_BRACKETS {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_BoundedTableContentBracket($1, $2);
								SET_LOCATION($$, @1, @4);
							}
						;

/**************************************************************************/
/* TABLE CONSTRUCTOR */

TablePrefix:			'[' 
							{ SM(S_TABLE); PE2(T_LB, S_TABLIST); }
						;

TableSuffix:				{ PE(T_RB); }  
						']' 
							{ EM(S_TABLE); }
						;

TableConstructor:			TablePrefix	TableElements TableSuffix
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_TableConstructor($2); SET_LOCATION($$, @1, @3); }
									
						|	TablePrefix TableSuffix
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_TableConstructor(); SET_LOCATION($$, @1, @2); }
						;

TableElements:				TableElements ',' 
								{ PE(T_COMMA); } 
							TableElement
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_TableElements($1, $4); }
						|	TableElement
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_TableElements(NIL_NODELIST, $1); }
						;

UnindexedValue:				Expression
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_UnindexedValue($1); SET_LOCATION($$, @1, @1); }
						|	FunctionElement
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_UnindexedValue($1); SET_LOCATION($$, @1, @1); }
						;
						
FunctionElement:			Function
								{ MAKE_FUNCTION_EXPR($1, $$); SET_LOCATION($$, @1, @1); }
						;

PE_elem:				{ OE(T_TABLE_ELEM); }						
						;
						
TableElement:				PE_elem UnindexedValue LN	{ $$ = $2; DPTR($$)->SetLine($3); SET_LOCATION(@$, @2, @2); }
						|	PE_elem IndexedValues		{ $$ = $2; SET_LOCATION(@$, @2, @2); }
						|   PE_elem NewAttribute		{ $$ = $2; SET_LOCATION(@$, @2, @2); }
						|	PE_elem IdentIndexElement   { $$ = $2; SET_LOCATION(@$, @2, @2); }
						;
									
DottedIdent:			DOT
							{ PE2(T_DOT, T_IDENT); }
						StringIdent
							{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_DottedIdent($3); SET_LOCATION($$, @1, @3); }
						;

OperatorIndex:			OpString 
							{ PE(T_OPINDEX); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_OperatorIndex($1); SET_LOCATION($$, @1, @1); }
						;

IndexContent:				Expression		{ $$ = $1; }
						|	DottedIdent		{ $$ = $1; }
						|	OperatorIndex	{ $$ = $1; }
						;

IndexExpression:			{ PE(S_INDEXEXPR); } 
						IndexContent 
							{ $$ = $2; SET_LOCATION(@$, @2, @2); }
						;

IndexedList:				IndexedList 
								{ PE(T_COMMA); } 
							',' IndexExpression LN
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $4); DPTR($4)->SetLine($5); SET_LOCATION(@$, @1, @4); }
						|	IndexExpression LN
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, $1); DPTR($1)->SetLine($2); SET_LOCATION(@$, @1, @1); }
						;

TableIndices:				IndexedList { $$ = ASTCREATOR_BY_CONTEXT.MakeNode_TableIndices($1); SET_LOCATION($$, @1, @1); }
						;

TableValues:				ContentList { $$ = ASTCREATOR_BY_CONTEXT.MakeNode_TableValues($1); SET_LOCATION($$, @1, @1); }
						;

IndexedValues:			'{' 
							{ PE(T_LBC); SG(N_EMPTY); } 
						TableIndices 
							{ EG(N_EMPTY); PE2(S_INDICES, T_COLON); } 
						':' 
							{ SG(S_ELIST); } 
						TableValues 
							{ EG(S_ELIST); PE2(S_ELIST, T_RBC); } 
						'}'
							{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_IndexedValues($3, $7); SET_LOCATION($$, @1, @9); }
						;

IdentIndex:					AttributeId	':'
								{ $$ = $1; }
						|	DOT 
								{ PE(T_DOT); OE(T_IDENT_OR_KWDIDENT); } 
							AttributeIdent ':'
								{ $$ = $3; PE(T_COLON); }
						;

IdentIndexElement:		IdentIndex
							{ SG(S_EXPR); }
						ContentExpression LN {
							EG(S_EXPR); PE(S_EXPR);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_IdentIndexElement($1, $3);
							DPTR($$)->SetLine($4);
							SET_LOCATION($$, @1, @3);
						}
						;

ContentList:				ContentList ',' LN ContentExpression 
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $4); DPTR($4)->SetLine($3); }
						|	LN ContentExpression {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, $2); DPTR($2)->SetLine($1);
								SET_LOCATION(@$, @2, @2);
							}
						;

ContentExpression:			Expression		{ $$ = $1; }
						|	Function		{ MAKE_FUNCTION_EXPR($1, $$); }
						;
					
/**************************************************************************/
/* ATTRIBUTES */

AttributeIdent:				IDENT	{
								PE(T_IDENT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Name($1);
								SET_LOCATION($$, @1, @1);
							}
						|	KwdIdent	{
								PE(T_KWDIDENT);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Name($1);
								SET_LOCATION($$, @1, @1);
							}
						;
						
ConstAttributeId:			ATTRIBUTE_IDENT {
								PE(T_ATTRIBUTEID); OE(T_LBC_OR_COLON);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Name($1 + 1);
								SET_LOCATION($$, @1, @1);
							}
						|	ATTRIBUTE 
								{ PE(T_ATTRIBUTE); OE(T_IDENT_OR_KWDIDENT); }
							AttributeIdent
								{ $$ = $3; }
						;

AttributeId:				ConstAttributeId			{ $$ = $1; }
						|	ATTRIBUTE MetaGeneratedCode	{ $$ = $2; }
						;
				
NewAttribute:			AttributeId AttributeSet AttributeGet
							{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_NewAttribute($1, $2, $3); SET_LOCATION($$, @1, @3); }
						;
						
AttributeSet:			'{'	
							{ PE(T_SET);  }
						SET 				
							{ SG(S_EXPR); } 
						ContentExpression LN
							{ EG(S_EXPR); PE2(S_EXPR, T_GET); $$ = $5; DPTR($$)->SetLine($6); SET_LOCATION(@$, @1, @5); }
						;

AttributeGet:			GET
							{ SG(S_EXPR); }
						ContentExpression LN
							{ EG(S_EXPR); PE2(S_EXPR, T_RBC); }
						'}'	
							{  $$ = $3; DPTR($$)->SetLine($4); }
						;

/**************************************************************************/
/* BRANCHES AND LOOPS */

BreakStmt:					{ SM(S_STMT); PE(T_BREAK);	}	
						BREAK Semi	
							{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_BREAK(); SET_LOCATION($$, @1, @2); SET_LOCATION(@$, @2, @3); }
						;
						
ContinueStmt:				{ SM(S_STMT); PE(T_CONT);	}	
						CONTINUE Semi	
							{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_CONTINUE(); SET_LOCATION($$, @1, @2); SET_LOCATION(@$, @2, @3); }
						;

Condition:				'('				{ SG(S_EXPR); } 
						LN Expression	{ EG(S_EXPR); PE2(S_EXPR, T_RPAR); } 
						')'				{ $$ = $4; DPTR($$)->SetLine($3); }	
						;

/* IF************************/

IfPrefix:				IF		{ SM(S_IF); PE2(T_IF, T_LPAR);  }		;
ElsePrefix:				ELSE	{ EM(S_IF); SM(S_ELSE); PE(T_ELSE); }	;
		
IfStmt:						IfPrefix Condition Stmt
								{ EM(S_IF);   $$ = ASTCREATOR_BY_CONTEXT.MakeNode_IfStmt($2, $3); SET_LOCATION($$, @1, @3); }
						|	IfPrefix Condition Stmt ElsePrefix Stmt {
								EM(S_ELSE);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_IfElseStmt($2, $3, $5);
								SET_LOCATION($$, @1, @5);
							}
						;

/* WHILE*********************/

WhilePrefix:			WHILE	
							{ SM(S_WHILE); PE2(T_WHILE, T_LPAR); }	
						;

WhileStmt:				WhilePrefix Condition LN Stmt {
							EM(S_WHILE);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_WhileStmt($2, $4);
							DPTR($4)->SetLine($3);
							SET_LOCATION($$, @1, @4);
						}
						;

/* FOR***********************/

ForStmt:				ForPrefix ForInitList ForCondition ForSuffix Stmt {
							EM(S_FOR);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ForStmt($2, $3, $4, $5);
							SET_LOCATION($$, @1, @5);
						}
						;

ForPrefix:				FOR		
							{ SM(S_FOR); PE2(T_FOR, T_LPAR); } 
						'('		
							{ PE(S_ELIST); } 
						;
						
ForInitList	:				LN ExpressionList Semi {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ForInit($2); DPTR($$)->SetLine($1);
								SET_LOCATION($$, @2, @2); SET_LOCATION(@$, @2, @3);
							}
						|	LN Semi {
								EI(S_ELIST);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ForInit(NIL_NODELIST); DPTR($$)->SetLine($1);
								SET_LOCATION($$, @2, @2); SET_LOCATION(@$, @2, @2);
							}
						;

ForCondition:				{ PE(S_EXPR); }		
						 Expression LN Semi
							{ $$ = $2; DPTR($$)->SetLine($3); SET_LOCATION(@$, @2, @4); }
						;

ForSuffix:						{ PE(S_ELIST); } 
							LN ExpressionList ForEnd {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ForSuffix($3); DPTR($$)->SetLine($2);
								SET_LOCATION($$, @3, @4); SET_LOCATION(@$, @3, @4);
							}
						|	LN ForEnd {
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ForSuffix(NIL_NODELIST); DPTR($$)->SetLine($1);
								SET_LOCATION($$, @2, @2); SET_LOCATION(@$, @2, @2);
							}
						;

ForEnd:							{ PE(T_RPAR); } 
							')'	
								{ SET_LOCATION(@$, @2, @2); }
						;

/* FOREACH*******************/

ForeachCont:			LN Expression	{ $$ = $2; DPTR($$)->SetLine($1); SET_LOCATION(@$, @2, @2); }
						;

ForeachValue:				':'				{	PE(S_LVAL);		}
							Lvalue			{	PE(T_COMMA);	}
							','				{	$$ = $3;		}
						|	','				{	unullify($$);	}
						;
						
ForeachStmt:			FOREACH			{	SM(S_FOREACH); PE2(T_FOREACH, T_LPAR);	}
						'('				{	PE(S_LVAL);				}
						Lvalue			{	PE(T_SEMI_OR_COMMA);	}
						ForeachValue	{	PE(S_EXPR);				}
						ForeachCont		{	PE(T_RPAR);				}
						')'				{	PE(S_STMT);				}						
						LN Stmt			{	if ($7)
												$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ForeachStmt($7, $5, $9, $14); 
											else
												$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ForeachStmt($5, NIL_NODE, $9, $14); 
											DPTR($$)->SetLine($13);
											SET_LOCATION($$, @1, @14);
											EM(S_FOREACH);	}
						;

/**************************************************************************/
/* EXCEPTION HANDLING */

ThrowStmt:				THROW 
							{ SM(S_THROW); PE2(T_THROW, S_EXPR); } 
						Expression Semi
							{ EM(S_THROW); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_THROW($3); SET_LOCATION($$, @1, @4); }
						;

ExceptionVar:			Ident	{ PE(S_STMT); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_ExceptionVar($1); SET_LOCATION($$, @1, @1); }
						;
						
ExceptionStmt:			TRY		{ SM(S_TRY); PE2(T_TRY, S_STMT); }
						Stmt	{ PE(T_TRAP); }
						TRAP	{ PE(T_IDENT); }
						ExceptionVar
						Stmt	{
							EM(S_TRY);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Exception($3, $7, $8);
							SET_LOCATION($$, @1, @8);
						}
						;
				
/**************************************************************************/
/* METAPROGRAMMING SUPPORT */

MultipleEscapes:		MultipleEscapes META_ESCAPE	{ $$ = $1 + 1; }
					|	META_ESCAPE					{ $$ = 1; }
					;

MetaGeneratedCode:		 MultipleEscapes '(' LN Expression ')' {
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_ESCAPE, $4);
							$$->SetAttribute(AST_ATTRIBUTE_CARDINALITY, $1);
							$$->SetLine($3);
							SET_LOCATION($$, @1, @5);
						}
					|	MultipleEscapes LN IDENT {
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Name($3);				SET_LOCATION($$, @3, @3);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Lvalue($$);			SET_LOCATION($$, @3, @3);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_PrimaryExpression($$); SET_LOCATION($$, @3, @3);
							
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_ESCAPE, $$);
							$$->SetAttribute(AST_ATTRIBUTE_CARDINALITY, $1);
							$$->SetLine($2);
							SET_LOCATION($$, @1, @3);
						}
					|	META_INLINE '(' LN Expression ')' {
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_INLINE, $4);
							$$->SetLine($3);
							SET_LOCATION($$, @1, @5);
						}
					;

QuotedExpressionList:		QuotedExpressionList ',' Expression
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $3); }
						|	Expression
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, $1); }
						;

QuotedIndexedList:			QuotedIndexedList ',' IndexContent	{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $3); }
						|	IndexContent						{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, $1); }
						;

QuotedIndexedValues:	'{'  QuotedIndexedList ':'
							{ SG(S_ELIST); } 
						ContentList 
							{ EG(S_ELIST); PE2(S_ELIST, T_RBC); } 
						'}'	{
							Node* indices = ASTCREATOR_BY_CONTEXT.MakeNode_TableIndices($2); SET_LOCATION(indices, @2, @2);
							Node* values = ASTCREATOR_BY_CONTEXT.MakeNode_TableValues($5); SET_LOCATION(values, @5, @5);
							$$ = ASTCREATOR_BY_CONTEXT.MakeNode_IndexedValues(indices, values); SET_LOCATION($$, @1, @7);
						}
					;

QuotedIdentIndex:			AttributeId	':' 		{ $$ = $1; }
						|	DOT AttributeIdent ':'	{ $$ = $2; PE(T_COLON); }
						;

QuotedIdentIndexElement:	QuotedIdentIndex
								{ SG(S_EXPR); }
							ContentExpression LN {
								EG(S_EXPR); PE(S_EXPR);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_IdentIndexElement($1, $3);
								DPTR($$)->SetLine($4);
								SET_LOCATION($$, @1, @4);
								SET_LOCATION(@$, @1, @3);
							}
							;

NonExprQuotedElement:		Function				{ $$ = $1; }
						|	QuotedIndexedValues		{ $$ = $1; }
						|	NewAttribute			{ $$ = $1; }
						|	QuotedIdentIndexElement	{ $$ = $1; }
						|	DOT StringIdent 		{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_DottedIdent($2); SET_LOCATION($$, @1, @2); }
						|	OperatorIndex			{ $$ = $1; }
						|	'|'
								{ PE2(T_BAR, S_EXPR); }
							Expression
								{ PE(T_BAR); }
							'|'
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_LateBoundArgument($3); SET_LOCATION($$, @1, @5); }
						|	TRIPLE_DOT	{ PE(T_TDOT); $$ = ASTCREATOR_BY_CONTEXT.MakeNode_TRIPLE_DOT(); SET_LOCATION($$, @1, @1); }
						;

QuotedElementList:		QuotedExpressionList ',' NonExprQuotedElement	{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $3); }
					|	QuotedElementList ',' NonExprQuotedElement		{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $3); }
					|	QuotedElementList ',' Expression				{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List($1, $3); }
					|	NonExprQuotedElement							{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, $1); }
					;

QuotedElements:		QuotedElementList		{ $$ = $1; }
				|	QuotedExpressionList	{ $$ = $1; }
				;

MiscStmt:			Assertion Semi			{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt($1); SET_LOCATION($$, @1, @2); }
				|	ReturnStmt				{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt($1); SET_LOCATION($$, @1, @1); }
				|	BasicNonExprStmt		{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt($1); SET_LOCATION($$, @1, @1); }
				|	SEMI					{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_EmptyStmt();	  SET_LOCATION($$, @1, @1); }
				|	UsingDirective			{ $$ = $1; }
				|	MetaStmt				{ $$ = $1; }
				;

NoContextStmt:		QuotedIndexedList Semi {
						Node* elist = ASTCREATOR_BY_CONTEXT.MakeNode_ExpressionListStmt($1);
						SET_LOCATION(elist, @1, @1);
						$$ = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt(elist);
						SET_LOCATION($$, @1, @2);
					}
				|	Compound	{ $$ = $1; }
				|	Function	{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionStmt($1); SET_LOCATION($$, @1, @1); }
				|	MiscStmt	{ $$ = $1; }
				;

NonEmptyStmts:		NonEmptyStmts NoContextStmt
						{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts($1, $2); SET_LOCATION($$, @1, @2); }
				|	NoContextStmt
						{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts(NIL_NODE, $1); SET_LOCATION($$, @1, @1); }
				;

QuotedStmt:			NonFunctionQuotedStmt	{ $$ = $1; }
				|	Function				{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionStmt($1); SET_LOCATION($$, @1, @1); }
				;

NonFunctionQuotedStmt:		QuotedExpressionList Semi {
								Node* elist = ASTCREATOR_BY_CONTEXT.MakeNode_ExpressionListStmt($1);
								SET_LOCATION(elist, @1, @1);
								$$ = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt(elist);
								SET_LOCATION($$, @1, @2);
							}
						|	'{' NonEmptyStmts { PE(T_RBC); } '}'
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Compound($2); SET_LOCATION($$, @1, @4); }
						|	'{' '}' 
								{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Compound(); SET_LOCATION($$, @1, @2); }
						|	MiscStmt
								{ $$ = $1; }
						;

QuotedStmts:		QuotedStmts QuotedStmt 	{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts($1, $2); SET_LOCATION($$, @1, @2); }
				|	QuotedStmt QuotedStmt	{
						$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts(NIL_NODE, $1);
						$$ = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts($$, $2);
						SET_LOCATION($$, @1, @2);
					}
				;

QuasiQuoted:		QuotedElements			{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_FromList(AST_TAG_QUOTED_ELEMENTS, $1); SET_LOCATION($$, @1, @1); }
				|	NonFunctionQuotedStmt	{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts(NIL_NODE, $1); SET_LOCATION($$, @1, @1); }
				|	QuotedStmts				{ $$ = $1; }
				;

MetaExpression:		META_LSHIFT QuasiQuoted META_RSHIFT
						{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_QUASI_QUOTES, $2);	SET_LOCATION($$, @1, @3); }
				;

MetaStmt:			META_EXECUTE Stmt
						{ $$ = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_EXECUTE, $2); SET_LOCATION($$, @1, @2); }
				;
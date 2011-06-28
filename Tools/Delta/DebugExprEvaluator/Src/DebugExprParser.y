%{

// Debug expression evaluator.
// Bison parser generator.
// Uses hand-made lexical analyser.
// ScriptFighter Project.
// A. Savidis, Ferbuary 2002 (original version).
//

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "DDebug.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DebugExprParseActions.h"
#include "DebugExprLexAn.h"
#include "DebugEvaluatedExpr.h"

// Bison assumes alloca is the memory allocation
// function.
//
#ifndef	alloca
#define alloca malloc
#endif

// We want the yylval to be passed as a parameter to yylex,
// not being a global variable. This is why we define YYPURE.
//
#define	YYPURE

#define	yyparse	DebugExprParser_yyparse
#define yylex	DebugExprLexAn::GetToken
#define	yydebug	DebugExprParser_yydebug
#define	yyerror	DebugExprParser_yyerror

static void DebugExprParser_yyerror (const char* s) {
	if (!DebugExprLexAn::LastEOFWasForced())
		DebugExprParseActions::Error("%s", s);
}

#define	ACTIONS \
	DebugExprParseActions

%}

%union {
	char*					id;					// Variable identifier.
	DeltaNumberValueType	realConst;			// Constant numeric value.
	util_ui32				intConst;			// Constant integer value.
	char*					strConst;			// String constant value.
	DebugEvaluatedExpr*		expr;
	DebugExprList*			exprList;
}

%start	DebugExpression
%type	<expr>			ConstValue Lvalue TableContent Expression RelationalExpression NewMonitor DeleteMonitor
%type	<expr>			Primary Term BooleanExpression ArithmeticExpression FunctionCall Table Assign LvalueForAssign
%type	<id>			Name DotIdent
%type	<exprList>		ExpressionList
%token	<id>			IDENT RESERVEDATTR MONITOR ATTRIBUTE
%token	<realConst>		REAL_CONST
%token	<intConst>		INT_CONST HEXINT_CONST
%token	<strConst>		STRING_CONST

%token	STATIC AND NOT OR UMINUS TRUE FALSE FUNCRES NEW
%token	RESERVEDATTR SINGLE_RIGHT_SB SINGLE_LEFT_SB FORMAL DELETE
%token	NIL LOCAL LT GT LE GE EQ NE DOT DOUBLE_DOT MONITOR
%token	DOUBLE_LEFT_SB DOUBLE_RIGHT_SB '(' ')'
%token	PARENTHESIS SQUARE_BRACKETS LAMBDA


%left	OR 
%left	AND
%left	EQ NE
%left	GE LE GT LT
%left	'+' '-'
%left	'*' '/' '%'
%right	NOT UMINUS
%left	DOT
%left	SQUARE_BRACKETS
%left	PARENTHESIS

%%

DebugExpression:			Expression
								{ ACTIONS::SetResultExpression($1); }
						|	Assign
								{ ACTIONS::SetResultExpression($1); }
						|	NewMonitor
								{ ACTIONS::SetResultExpression($1); }
						|	DeleteMonitor
								{ ACTIONS::SetResultExpression($1); }
						;

Expression:					RelationalExpression
								{ $$ = $1; }
						|	BooleanExpression
								{ $$ = $1; }
						|	ArithmeticExpression
								{ $$ = $1; }
						|	Term
								{ $$ = $1; }
						;
				
RelationalExpression:		Expression GT Expression
								{ $$ = ACTIONS::Manage_RelationalExpr($1,$3, DebugExprRelOp_GT); }
						|	Expression LT Expression
								{ $$ = ACTIONS::Manage_RelationalExpr($1,$3, DebugExprRelOp_LT); }
						|	Expression GE Expression
								{ $$ = ACTIONS::Manage_RelationalExpr($1,$3, DebugExprRelOp_GE); }
						|	Expression LE Expression
								{ $$ = ACTIONS::Manage_RelationalExpr($1,$3, DebugExprRelOp_LE); }
						|	Expression EQ Expression
								{ $$ = ACTIONS::Manage_RelationalExpr($1,$3, DebugExprRelOp_EQ); }
						|	Expression NE Expression
								{ $$ = ACTIONS::Manage_RelationalExpr($1,$3, DebugExprRelOp_NE); }
						;


BooleanExpression:			Expression AND Expression
								{ $$ = ACTIONS::Manage_ExprANDExpr($1, $3); }
						|	Expression OR Expression
								{ $$ = ACTIONS::Manage_ExprORExpr($1, $3); }
						;
							
ArithmeticExpression:		Expression '+' Expression
								{ $$ = ACTIONS::Manage_ArithmeticExpr($1, $3, DebugExpArithOp_ADD); }
						|	Expression '-' Expression
								{ $$ = ACTIONS::Manage_ArithmeticExpr($1, $3, DebugExpArithOp_SUB); }
						|	Expression '/' Expression
								{ $$ = ACTIONS::Manage_ArithmeticExpr($1, $3, DebugExpArithOp_DIV); }
						|	Expression '*' Expression
								{ $$ = ACTIONS::Manage_ArithmeticExpr($1, $3, DebugExpArithOp_MUL); }
						|	Expression '%' Expression
								{ $$ = ACTIONS::Manage_ArithmeticExpr($1, $3, DebugExpArithOp_MOD); }
						;

Term:					'-' Expression %prec UMINUS
							{ $$ = ACTIONS::Manage_UMINUSExpression($2); }
						|	NOT Expression
								{ $$ = ACTIONS::Manage_NOTExpression($2); }
						|	'(' Expression ')'
								{ $$ = $2; }
						|	Primary
								{ $$ = $1; }
						;

ExpressionList			:	Expression 
								{ $$ = ACTIONS::Manage_ExpressionList($1); }
						|	ExpressionList ',' Expression
								{ $$ = ACTIONS::Manage_ExpressionList($1, $3); }
						;

FunctionCall:			RESERVEDATTR '(' ExpressionList ')'
							{ $$ = ACTIONS::Manage_UnaryLibraryFuncCall($1, $3); }
						;

Primary:					Lvalue
								{ $$ = $1; }
						|	ConstValue
								{ $$ = $1; }
						|	FunctionCall
								{ $$ = $1; }
						;

ConstValue:					REAL_CONST
								{ $$ = ACTIONS::Manage_ConstValue($1); }
						|	INT_CONST
								{ $$ = ACTIONS::Manage_ConstValue((DeltaNumberValueType) $1); }
						|	HEXINT_CONST
								{ $$ = ACTIONS::Manage_ConstValue((DeltaNumberValueType) $1); }
						|	STRING_CONST
								{ $$ = ACTIONS::Manage_ConstValue($1); }
						|	NIL
								{ $$ = ACTIONS::Manage_ConstValue(); }
						|	TRUE
								{ $$ = ACTIONS::Manage_ConstValue(true); }
						|	FALSE
								{ $$ = ACTIONS::Manage_ConstValue(false); }
						;

Name:						IDENT 
								{ $$ = $1; }
						|	Name FUNCRES IDENT
								{ $$ = ACTIONS::Manage_Name($1, $3); }
						;

LvalueForAssign:			FORMAL DOT IDENT
								{ $$ = ACTIONS::Manage_FormalLvalue($3); }
						|	LOCAL IDENT
								{ $$ = ACTIONS::Manage_LocalLvalue($2); }
						|	FUNCRES IDENT
								{ $$ = ACTIONS::Manage_GlobalLvalue($2, false); }
						|	STATIC DOT Name
								{ $$ = ACTIONS::Manage_GlobalLvalue($3, true); }
						;
						
Assign:					LvalueForAssign	'=' Expression
							{ $$ = ACTIONS::Manage_AssignLvalue($1, $3); }
						;
												
Lvalue:						Name
								{ $$ = ACTIONS::Manage_AnyLvalue($1); }
						|	LvalueForAssign
								{ $$ = $1; }
						|	TableContent
								{ $$ = $1; }
						|	MONITOR
								{ $$ = ACTIONS::Manage_MONITOR($1); }
						|	ATTRIBUTE
								{ $$ = ACTIONS::Manage_ATTRIBUTE($1); }
						|	LAMBDA
								{ $$ = ACTIONS::Manage_LAMBDA(); }
						;

Table:						Lvalue			{ $$ = $1; }
						|	FunctionCall	{ $$ = $1; }
						;

DotIdent:					IDENT			{ $$ = $1; }
						|	ATTRIBUTE		{ $$ = ACTIONS::Manage_AttributeIndex($1); }
						;
			
TableContent:				Table DOT DotIdent										%prec DOT
								{ $$ = ACTIONS::Manage_TableContent($1, $3, false); }
						|	Table DOUBLE_DOT DotIdent								%prec DOT
								{ $$ = ACTIONS::Manage_TableContent($1, $3, true); }

						|	Table SINGLE_LEFT_SB Expression SINGLE_RIGHT_SB		%prec SQUARE_BRACKETS
								{ $$ = ACTIONS::Manage_TableContent($1, $3, false); }
						|	Table DOUBLE_LEFT_SB Expression DOUBLE_RIGHT_SB		%prec SQUARE_BRACKETS
								{ $$ = ACTIONS::Manage_TableContent($1, $3, true); }
														
						|   Table DOT RESERVEDATTR									%prec DOT
								{ $$ = ACTIONS::Manage_TableContent_ReservedAttr($1, $3); }
						;
		
NewMonitor:				NEW MONITOR '=' Expression
							{ $$ = ACTIONS::Manage_NewMonitor($2, $4); }
						;
				
DeleteMonitor:			DELETE MONITOR
							{ $$ = ACTIONS::Manage_DeleteMonitor($2); }
						;

%%
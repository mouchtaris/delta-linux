%{

// Delta parser, syntax directed parsing file, using Bison parser generator.
// Technical notice: I use a static char* buffer for IDENT, so
// if IDENT is to be taken its char* value when being on the left or
// intermediate part of a rule that has at right another non-terminal
// that includes IDENT too, the last use of IDENT would have overwritten
// all previous, so those have to be saved with a dynamic copy at the
// time the IDENT rule is engaged (see ConstId for an example). To avoid
// memory leaks in case of syntax errors for such temp string copies, 
// late string destruction is adopted.
// ScriptFighter Project.
// A. Savidis, Start October 1999 (original full version).
// Revised January 2005, changing self, putting agruments and
// generalizing table content for any expression type.
// Revised May 2006 for better source line calculation of quads.
// Extended July 2007, adding support for precise syntax error messages.
// Extended September 2008, added support for unparsed forms and 
// selective step-in in case of multiple calls in a single stmt.
// Extended January 2009, added support for object attributes (properties).
// Extended July 2009, added foreach stmt.
// Extended December 2009, added lambda functions.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "DDebug.h"
#include "Symbol.h"
#include "Expr.h"
#include "ParseActions.h"
#include "ParseTableConstruction.h"
#include "Unparsable.h"
#include "DescriptiveParseErrorHandler.h"
#include "LibraryNamespace.h"
#include "CompilerStringHolder.h"

#include "DeltaLexAnalyser.h"
#include "DeltaParser.h"

#define yyFlexLexer DeltaLexAnalyserFlexLexer

// Bison assumes alloca is the memory allocation
// function.
//
#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

extern int DeltaCompiler_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer);

///////////////////////////////////////////////////////////
// Helper macros to get specific components from the lexer
// component directory.

#define DIRECTORY						(lexer.GetDirectory())

#define GET_DESCRIPTIVE_ERROR_HANDLER	DESCRIPTIVE_ERROR_HANDLER_EX(DIRECTORY)
#define GET_PARSEPARMS					PARSEPARMS_EX(DIRECTORY)
#define GET_STRINGHOLDER				STRINGHOLDER_EX(DIRECTORY)
#define GET_DELTASYMBOLS				DELTASYMBOLS_EX(DIRECTORY)
#define GET_TRANSLATOR					TRANSLATOR_EX(DIRECTORY)
#define GET_QUADS						QUADS_EX(DIRECTORY)
#define GET_STMTFACTORY					STMTFACTORY_EX(DIRECTORY)

// Need to undef since its original version uses
// the curr component directory.

#undef	NEW_STMT
#define	NEW_STMT							GET_STMTFACTORY.New()

///////////////////////////////////////////////////////////

static void DeltaCompiler_yyerror (YYLTYPE* yylloc, yyFlexLexer& lexer, const char* unused)
	{ GET_DESCRIPTIVE_ERROR_HANDLER.HandleSyntaxError(); }

%}

%union {
	const char*				id;
	DeltaNumberValueType	numberConst;
	char*					strConst;
	int						intVal;
	DeltaExpr*				expr;
	DeltaSymbol*			sym;
	TableElements*			table;
	DeltaQuadAddress		quadNo;
	Stmt*					stmt;
}

%start	DeltaCode
%type	<strConst>		ConstId AttributeId IdentIndex
%type	<expr>			ConstValue Lvalue TableContent Expression ExpressionList TablePrefix IndexedList
%type	<expr>			Primary FunctionCall TableConstructor Term TableContentDot TableContentBracket ContentList
%type	<expr>			FunctionCallObject FunctionAndTableObject StringifyDottedIdents StringifyNamespaceIdent
%type	<expr>			ActualArguments ActualArgumentsList ActualArgument NonEmptyActualArgumentsList ContentExpression
%type	<expr>			TableContentBoundedDot TableContentBoundedBracket TableObject StringConst StringConstUsed ForeachValue
%type	<sym>			FunctionPrefix Function LambdaFunction LambdaPrefix
%type	<table>			IndexedValues TableElement TableElements UnindexedValue IdentIndexElement
%type	<intVal>		FuncClass LN ForEnd FuncLinkage
%type	<quadNo>		M M_If N TryHeader TrapHeader TrapJumpOver M_elem
%type	<expr>			ArithmeticExpression AssignExpression BooleanExpression RelationalExpression DotIndex SpecialDotIndex
%type	<expr>			Condition ForCondition DottedIdent OperatorIndex IndexContent IndexExpression
%type	<expr>			TernaryExpression TernaryCondition TernarySelection1 TernarySelection2 FunctionElement
%type	<expr>			NewAttribute AttributeSet AttributeGet ExceptionVar ConstAssignExpression
%type	<stmt>			Stmts Stmt IfStmt Compound  ExceptionStmt ThrowStmt ForStmt WhileStmt ForRepeatList
%type	<stmt>			ExpressionStmt  ForInitList ForPrefix ForSuffix ReturnStmt
%type	<stmt>			BasicExprStmt BasicNonExprStmt AssertStmt Assertion
%type	<stmt>			ForeachPrefix ForeachStmt LambdaStmt LambdaCode
%type	<id>			FunctionName  OperatorMethod OpString KwdIdent StringIdent DottedOpString AttributeIdent
%token	<id>			IDENT ATTRIBUTE_IDENT 
%token	<numberConst>	NUMBER_CONST
%token	<strConst>		STRING_CONST

%token	FUNCTION RETURN ONEVENT IF ELSE WHILE FOR FOREACH NIL LOCAL 
%token	AND NOT OR LAMBDA TRY TRAP THROW USING ASSERT
%token	TRUE FALSE BREAK CONTINUE STATIC CONST METHOD SELF ARGUMENTS 
%token	ASSIGN ADD_A SUB_A MUL_A DIV_A MOD_A DOUBLE_LB DOUBLE_RB SEMI UMINUS MINUSMINUS
%token	LT GT LE GE EQ NE DOT DOUBLE_DOT TRIPLE_DOT DOT_ASSIGN GLOBAL_SCOPE
%token	ADD SUB MUL DIV MOD ATTRIBUTE STRINGIFY PLUSPLUS  
%token	'(' ')' '[' ']' '{' '}' '?' ':' ','
%token	PARENTHESIS SQUARE_BRACKETS CALL CAST DOT_EQUAL DOT_CAST DOT_EQUAL_RHS
%token	ADD_POSTFIX SUB_POSTFIX MUL_POSTFIX DIV_POSTFIX MOD_POSTFIX
%token	GT_POSTFIX LT_POSTFIX NE_POSTFIX EQ_POSTFIX GE_POSTFIX LE_POSTFIX
%token	OPERATOR NEWSELF SET GET LAMBDA_REF

%right		ASSIGN ADD_A SUB_A MUL_A DIV_A MOD_A
%left		OR 
%left		AND
%nonassoc	EQ NE
%nonassoc	GE LE GT LT
%left		ADD SUB
%left		MUL DIV MOD
%right		NOT PLUSPLUS MINUSMINUS UMINUS
%nonassoc	DOT_ASSIGN
%left		DOT
%left		SQUARE_BRACKETS
%left		PARENTHESIS

%output="DeltaParser.cpp"
%name-prefix="DeltaCompiler_yy"
%debug
%defines
%verbose
%pure-parser
%parse-param {yyFlexLexer& lexer}
%lex-param   {YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer}
%locations
%expect 2
%%

/**************************************************************************/
/* PROGRAM*/

DeltaCode:			/* Empty code allowed. */ 
				|	Stmts { /* Intentionally empty actions. */ }
				|	UsingDirectives OptionalStmts
					;

UsingDirectives:	UsingDirectives UsingDirective
				|	UsingDirective
				;

OptionalStmts:		Stmts
				|	/* empty */
				;
								
Stmts:				Stmts Stmt
						{	UNPARSABLE_GET(Unparse_Stmts($1, $2));
							GET_DELTASYMBOLS.ResetTemp();
							$$ = GET_TRANSLATOR.Translate_Stmts($1, $2); 
							UNPARSABLE_SET($$);	}
				|	Stmt
						{ $$ = GET_TRANSLATOR.Translate_Stmts($1); }
				;

/**************************************************************************/
/* FUNCTIONS AND METHODS */

Function:		M FunctionPrefix FormalArgs
					{	GET_TRANSLATOR.Translate_FunctionHeader($2); 
						GET_DELTASYMBOLS.PushAndResetTempCounter(); SG(S_BLOCK); }
				Compound LN
					{	EG(S_BLOCK); EM(S_FUNC_($2)); 
						UNPARSABLE_GET(Unparse_Function($2, $5));
						if ($$ = GET_TRANSLATOR.Translate_Function($2, $5, $1, $6))
							DPTR($$)->SetSourceCode(unparsed);
					}
				;

FuncClass:			FUNCTION
						{ $$ = DELTA_FUNCCLASS_PROGRAMFUNCTION; }
				|	ONEVENT
						{ $$ = DELTA_FUNCCLASS_ONEVENT; }
				|	METHOD
						{ $$ = DELTA_FUNCCLASS_METHOD; }
				;

FuncLinkage:		LOCAL	{ $$ = DELTA_FUNCTION_NO_EXPORT; }
				|			{ $$ = DELTA_FUNCTION_DEFAULT_EXPORT; }
				;
				
FunctionPrefix:  FuncClass
					{ SM(S_FUNC_($1)); PE2(T_FUNC_($1), T_LOCAL_OR_FUNCNAME); }
				 FuncLinkage FunctionName
					{ PE(T_LPAR); SG(S_FARGS); $$ = GET_TRANSLATOR.Translate_Function($4, $1, $3); }
				;
			 
FunctionName:	IDENT
					{ $$ = $1; }
			|	OPERATOR OperatorMethod
					{ $$ = $2; }
			|	ATTRIBUTE
					{ $$ = DELTA_TOSTRING_SYMBOLIC_NAME; }
			|	/* anonymous function */
					{ unullify($$); }
			;

OperatorMethod:		OpString		{ $$ = $1;		}
				|	DOT				{ $$ = DELTA_OPERATOR_OBJECT_GET;	}
				|	DOT_ASSIGN		{ $$ = DELTA_OPERATOR_OBJECT_SET;	}
				;
			
IdentList:		IdentList ',' 
					{ PE2(T_COMMA, T_IDENT); } 
				IDENT
					{ GET_TRANSLATOR.Translate_IdentList($4); }
			|	IDENT
					{ PE(T_IDENT); GET_TRANSLATOR.Translate_IdentList($1); }
			|	/* no formal arguments */
			;

FormalArgsSuffix:	
				')'	
					{ EG(S_FARGS); PE(T_RPAR);  }
			|	TRIPLE_DOT
					{ EG(S_FARGS); PE2(T_TDOT, T_RPAR); }
				')'
					{  GET_TRANSLATOR.Translate_VarArgs(); }
			;
									
FormalArgs:		CALL	
					{ EG(S_FARGS); PE(T_RPAR); }
			|	'('	IdentList 
					{ OE(T_LPAR_OR_TDOT); }
				FormalArgsSuffix
			|		{ EG(S_FARGS); PE(T_RPAR); } /* empty, arg signature can be skipped */
			;
			
Compound :		'{' 
					{ GET_TRANSLATOR.Translate_CompoundBegin(); PE(T_LBC); }
				Stmts 
					{ PE(T_RBC); } 
				'}'
					{  Unparse_Compound($$ = $3); GET_TRANSLATOR.Translate_CompoundEnd(); }
				
			|	'{' 
					{ PE(T_LBC); }  
				'}'
					{  PE(T_RBC); Unparse_Compound($$ = NEW_STMT); GET_TRANSLATOR.Translate_EmptyCompound(); }
			;

LambdaFunction:		M LambdaPrefix FormalArgs
						{	GET_TRANSLATOR.Translate_FunctionHeader($2); 
							GET_DELTASYMBOLS.PushAndResetTempCounter(); 
							PE(T_LBC); }
					LambdaCode
						{	EM(S_FUNC_($2)); 
							UNPARSABLE_GET(Unparse_LambdaFunction($2, $5));
							if ($$ = GET_TRANSLATOR.Translate_Function($2, $5, $1, 0))
								DPTR($$)->SetSourceCode(unparsed);
							GET_DELTASYMBOLS.PopAndRestoreTempCounter(); }
					;
					
LambdaPrefix:		LAMBDA
						{	SM(S_FUNC_(DELTA_FUNCCLASS_PROGRAMFUNCTION)); PE2(T_LAMBDA, T_LPAR); SG(S_FARGS); 
							$$ = GET_TRANSLATOR.Translate_Function((char*) 0, DELTA_FUNCCLASS_PROGRAMFUNCTION); }
					;

LambdaCode:			'{' 
						{ GET_TRANSLATOR.Translate_CompoundBegin(); PE(S_EXPR); }
					LN M LambdaStmt '}'
						{ $$ = $5; GET_TRANSLATOR.Translate_CompoundEnd(); GET_QUADS.SetQuadLine($4, $3, true); }
					;
					
LambdaStmt:			Expression
						{ $$ = GET_TRANSLATOR.Translate_RETURN($1); Unparse_LambdaStmt($$, $1); PE(T_RBC); }
					;
							
/**************************************************************************/
/* STMTS */

LN:		{ $$ = GET_PARSEPARMS.GetLine(); }	;
M:		{ $$ = GET_QUADS.NextQuadNo(); }		;
Semi:	{ PE(T_SEMI); }	SEMI				;
	
ExpressionStmt:			{ SM(S_STMT); }	
					ExpressionList Semi 
						{ $$ =	GET_TRANSLATOR.Translate_ExprListStmt($2); Unparse_ExprListStmt($$, $2); } 
					;
					
Stmt:		LN M BasicExprStmt		{ $$ = $3; GET_QUADS.SetQuadLine($2, $1, true);  GET_TRANSLATOR.Translate_BasicStmt($1); } 
		|	LN M BasicNonExprStmt	{ $$ = $3; GET_QUADS.SetQuadLine($2, $1);  GET_TRANSLATOR.Translate_BasicStmt($1); } 
		|	Compound				{ $$ = $1; }
		|	Function				{ $$ = NEW_STMT; Unparse_FunctionDef($$, $1); }
		|	SEMI					{ $$ = NEW_STMT; $$->SetUnparsed(";"); }
		;

BasicExprStmt:		ExpressionStmt		{ EM(S_STMT); $$ = $1; }
				|	AssertStmt			{ EM(S_STMT); $$ = $1; }
				|	ReturnStmt			{ $$ = $1;}
				;
				
BasicNonExprStmt:	WhileStmt			{ $$ = $1;}
				|	ForStmt				{ $$ = $1;}
				|	ForeachStmt			{ $$ = $1; }
				|	IfStmt				{ $$ = $1;}
				|	BreakStmt			{ EM(S_STMT); $$ = GET_TRANSLATOR.Translate_BREAK(); Unparse_BuiltInStmt($$, BREAK); }
				|	ContinueStmt		{ EM(S_STMT); $$ = GET_TRANSLATOR.Translate_CONTINUE(); Unparse_BuiltInStmt($$, CONTINUE); }
				|	ExceptionStmt		{ $$ = $1; }
				|	ThrowStmt			{ $$ = $1; }
				;

/* ASSERT*******************/

Assertion:		ASSERT
					{	SM(S_ASSRT); SG(S_EXPR); 
						GET_PARSEPARMS.InAssertStmt().enter(); }
				Expression
					{	EG(S_EXPR); PE(S_EXPR); EM(S_ASSRT); 
						GET_TRANSLATOR.Translate_ASSERT($3); 
						GET_PARSEPARMS.InAssertStmt().exit(); 
						Unparse_ExprStmt($$ = NEW_STMT, ASSERT, $3); }
				;
				
AssertStmt:			{ SM(S_STMT); PE(S_ASSRT);	}	
				Assertion Semi 
					{ $$ = $2; }
				;

/* RETURN*******************/

ReturnPrefix:	RETURN LN { SM(S_RET); }	;

ReturnStmt:		ReturnPrefix
					{ PE2(T_RET, S_EXPR); } 
				Expression Semi
					{	EM(S_RET); 
						$$ = GET_TRANSLATOR.Translate_RETURN($3); 
						Unparse_ExprStmt($$, RETURN, $3); }
			|	ReturnPrefix Semi
					{	EM(S_RET); 
						$$ = GET_TRANSLATOR.Translate_RETURN(); 
						Unparse_BuiltInStmt($$, RETURN); }
			;

/* USING********************/

UsingDirective:			USING UsingSpecifications
						;

UsingSpecifications:		UsingNamespace
						|	UsingByteCodeLibrary
						;
				
UsingNamespace:				NamespacePath IDENT SEMI
								{ GET_TRANSLATOR.Translate_UsingNamespace($2, true); }
						|	IDENT SEMI
								{ GET_TRANSLATOR.Translate_UsingNamespace($1, false); }
						;

UsingByteCodeLibrary:	STRINGIFY IDENT SEMI LN
							{ GET_TRANSLATOR.Translate_UsingByteCodeLibrary($2, $4); }
						;

/**************************************************************************/
/* EXPRESSIONS */

Expression:			AssignExpression		{ $$ = $1; }
				|	ConstAssignExpression	{ $$ = $1; }
				|	RelationalExpression	{ $$ = $1; }
				|	BooleanExpression		{ $$ = $1; }
				|	ArithmeticExpression	{ $$ = $1; }
				|	TernaryExpression		{ $$ = $1; }
				|	Term					{ $$ = $1; }
				;

ConstId	:	IDENT
				{ $$ = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr($1)); }
			;

ConstAssignExpression:	CONST	{ SM(S_CONST); PE(T_IDENT); } 
						ConstId	{ PE(T_ASSIGN); }
						ASSIGN	{ PE(S_EXPR); } 
						Expression
							{ EM(S_CONST); $$ = GET_TRANSLATOR.Translate_ConstAssignExpression($3, $7); }
						;

AssignExpression:		Lvalue ASSIGN	
							{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ASSIGN, S_EXPR); }	
						Expression
							{	EM(S_ASSIGNEXPR); 
								$$ = GET_TRANSLATOR.Translate_AssignExpr($1, $4); $1->SetInitialised(); 
								Unparse_BinaryOp($$, $1, ASSIGN, $4);	}
								
					|	Lvalue ADD_A	
							{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ADDA, S_EXPR); }		
						Expression
							{	EM(S_ASSIGNEXPR); 
								$$ = GET_TRANSLATOR.Translate_AssignArithExpr($1, $4, DeltaIC_ADD, "+="); 
								Unparse_BinaryOp($$, $1, ADD_A, $4);	}
								
					|	Lvalue MUL_A	
							{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MULA, S_EXPR); }		
						Expression
							{	EM(S_ASSIGNEXPR); 
								$$ = GET_TRANSLATOR.Translate_AssignArithExpr($1, $4, DeltaIC_MUL, "*="); 
								Unparse_BinaryOp($$, $1, MUL_A, $4);	}
								
					|	Lvalue SUB_A	
							{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_SUBA, S_EXPR); }		
						Expression
							{	EM(S_ASSIGNEXPR); 
								$$ = GET_TRANSLATOR.Translate_AssignArithExpr($1, $4, DeltaIC_SUB, "-="); 
								Unparse_BinaryOp($$, $1, SUB_A, $4);	}
								
					|	Lvalue DIV_A	
							{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_DIVA, S_EXPR); }		
						Expression
							{	EM(S_ASSIGNEXPR); 
								$$ = GET_TRANSLATOR.Translate_AssignArithExpr($1, $4, DeltaIC_DIV, "/="); 
								Unparse_BinaryOp($$, $1, DIV_A, $4);	}
							
					|	Lvalue MOD_A	
							{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MODA, S_EXPR); }	
						Expression
							{	EM(S_ASSIGNEXPR); 
								$$ = GET_TRANSLATOR.Translate_AssignArithExpr($1, $4, DeltaIC_MOD, "%="); 
								Unparse_BinaryOp($$, $1, MOD_A, $4);	}
					;

RelationalExpression:		Expression GT 
								{ SM(S_RELAT); PE3(S_EXPR, T_GT, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_RELAT); $$ = GET_TRANSLATOR.Translate_RelationalExpr($1, $4->AdaptIfBool(), DeltaIC_JGT, ">"); 
									Unparse_BinaryOp($$, $1, GT, $4);	}
								
						|	Expression LT 
								{ SM(S_RELAT); PE3(S_EXPR, T_LT, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_RELAT); $$ = GET_TRANSLATOR.Translate_RelationalExpr($1, $4->AdaptIfBool(), DeltaIC_JLT, "<"); 
									Unparse_BinaryOp($$, $1, LT, $4);	}
								
						|	Expression GE 
								{ SM(S_RELAT); PE3(S_EXPR, T_GE, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_RELAT); $$ = GET_TRANSLATOR.Translate_RelationalExpr($1, $4->AdaptIfBool(), DeltaIC_JGE, ">="); 
									Unparse_BinaryOp($$, $1, GE, $4);	}
								
						|	Expression LE 
								{ SM(S_RELAT); PE3(S_EXPR, T_LE, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_RELAT); $$ = GET_TRANSLATOR.Translate_RelationalExpr($1, $4->AdaptIfBool(), DeltaIC_JLE, "<="); 
									Unparse_BinaryOp($$, $1, LE, $4);	}
								
						|	Expression EQ 
								{ SM(S_RELAT); PE3(S_EXPR, T_EQ, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_RELAT); $$ = GET_TRANSLATOR.Translate_RelationalExpr($1, $4->AdaptIfBool(), DeltaIC_JEQ, "=="); 
									Unparse_BinaryOp($$, $1, EQ, $4);	}
								
						|	Expression NE 
								{ SM(S_RELAT); PE3(S_EXPR, T_NE, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_RELAT); $$ = GET_TRANSLATOR.Translate_RelationalExpr($1, $4->AdaptIfBool(), DeltaIC_JNE, "!="); 
									Unparse_BinaryOp($$, $1, NE, $4);	}
						;

BooleanExpression:			Expression AND
								{ SM(S_LOGICAL); PE3(S_EXPR, T_AND, S_EXPR); DNPTR($1)->AdaptToBool(); }
							M Expression
								{	EM(S_LOGICAL); $$ = GET_TRANSLATOR.Translate_ExprANDExpr($1, $4, $5); 
									Unparse_BinaryOp($$, $1, AND, $5);	}
								
						|	Expression OR
								{ SM(S_LOGICAL); PE3(S_EXPR, T_OR, S_EXPR); DNPTR($1)->AdaptToBool(); }
							M Expression
								{	EM(S_LOGICAL); $$ = GET_TRANSLATOR.Translate_ExprORExpr($1, $4, $5); 
									Unparse_BinaryOp($$, $1, OR, $5);	}
						;
							
ArithmeticExpression:		Expression ADD 
								{ SM(S_ARITH); PE3(S_EXPR, T_ADD, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_ARITH); $$ = GET_TRANSLATOR.Translate_ArithmeticExpression($1, DeltaIC_ADD, $4->AdaptIfBool(), "+"); 
									Unparse_BinaryOp($$, $1, ADD, $4);	}
								
						|	Expression SUB 
								{ SM(S_ARITH); PE3(S_EXPR, T_SUB, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_ARITH); $$ = GET_TRANSLATOR.Translate_ArithmeticExpression($1, DeltaIC_SUB, $4->AdaptIfBool(), "-"); 
									Unparse_BinaryOp($$, $1, SUB, $4);	}
								
						|	Expression DIV 
								{ SM(S_ARITH); PE3(S_EXPR, T_DIV, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_ARITH); $$ = GET_TRANSLATOR.Translate_ArithmeticExpression($1, DeltaIC_DIV, $4->AdaptIfBool(), "/"); 
									Unparse_BinaryOp($$, $1, DIV, $4);	}
								
						|	Expression MUL 
								{ SM(S_ARITH); PE3(S_EXPR, T_MUL, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_ARITH); $$ = GET_TRANSLATOR.Translate_ArithmeticExpression($1, DeltaIC_MUL, $4->AdaptIfBool(), "*"); 
									Unparse_BinaryOp($$, $1, MUL, $4);	}
								
						|	Expression MOD 
								{ SM(S_ARITH); PE3(S_EXPR, T_MOD, S_EXPR); $1->SelfAdaptIfBool(); } 
							Expression
								{	EM(S_ARITH); $$ = GET_TRANSLATOR.Translate_ArithmeticExpression($1, DeltaIC_MOD, $4->AdaptIfBool(), "%"); 
									Unparse_BinaryOp($$, $1, MOD, $4);	}
						;

TernaryExpression:	'(' TernaryCondition M TernarySelection1 ':' N M TernarySelection2 ')' %prec ASSIGN
						{	EM(S_TERNARY); $$ = GET_TRANSLATOR.Translate_Ternary($2, $4, $8, $3, $7, $6); 
							Unparse_TernaryOp($$, $2, $4, $8);	}
					;

TernaryCondition:	Expression '?'
						{	SM(S_TERNARY); PE3(S_EXPR, T_QMARK, S_EXPR); 
							$$ = GET_TRANSLATOR.Translate_Condition($1); }
					;

TernarySelection1:	Expression
						{ PE(T_COLON); $$ = DNPTR($1)->AdaptIfBool(); }
					;
					
TernarySelection2:		{ PE(S_EXPR); } 
					Expression
						{ PE(T_RPAR); $$ = DNPTR($2)->AdaptIfBool(); }
					;
					
Term:		Lvalue PLUSPLUS
				{	$$ = GET_TRANSLATOR.Translate_TermLvalue($1, true, DeltaIC_ADD); 
					Unparse_UnaryIncDec($$, PLUSPLUS, $1, true);	}
				
		|	PLUSPLUS 
				{ SM(S_EXPR); PE2(T_PLUSPLUS, S_LVAL); } 
			Lvalue
				{	EM(S_EXPR); 
					$$ = GET_TRANSLATOR.Translate_TermLvalue($3, false, DeltaIC_ADD); 
					Unparse_UnaryIncDec($$, PLUSPLUS, $3, false);	}
				
		|	Lvalue MINUSMINUS
				{	$$ = GET_TRANSLATOR.Translate_TermLvalue($1, true, DeltaIC_SUB); 
					Unparse_UnaryIncDec($$, MINUSMINUS, $1, true);	}
				
		|	MINUSMINUS 
				{ SM(S_EXPR); PE2(T_MINUSMINUS, S_LVAL); } 
			Lvalue
				{	EM(S_EXPR); 
					$$ = GET_TRANSLATOR.Translate_TermLvalue($3, false, DeltaIC_SUB); 
					Unparse_UnaryIncDec($$, MINUSMINUS, $3, false); }
				
		|	SUB 
				{ SM(S_EXPR); PE2(T_SUB, S_EXPR); } 
			Expression %prec UMINUS
				{ EM(S_EXPR); $$ = GET_TRANSLATOR.Translate_UMINUSExpression($3); Unparse_UnaryOp($$, SUB, $3); }
				
		|	NOT 
				{ SM(S_EXPR); PE2(T_NOT, S_EXPR); } 
			Expression
				{ EM(S_EXPR); $$ = GET_TRANSLATOR.Translate_NOTExpression($3); Unparse_UnaryOp($$, NOT, $3); }
		
		|	Primary				
				{ $$ = $1; }
		;

Primary:		FunctionAndTableObject	{ $$ = DNPTR($1)->AdaptIfTableContent(); }
			|	ConstValue				{ $$ = $1; Unparse_ConstValue($$); }
			|	LambdaFunction			{ $$ = GET_TRANSLATOR.Translate_FunctionExpresssion($1); }
			;

ConstValue:		NUMBER_CONST	{ $$ = GET_TRANSLATOR.Translate_ConstValue($1);	}
			|	NIL				{ $$ = GET_TRANSLATOR.Translate_ConstValue();		}
			|	TRUE			{ $$ = GET_TRANSLATOR.Translate_ConstValue(true);	}
			|	FALSE			{ $$ = GET_TRANSLATOR.Translate_ConstValue(false);}
			;

OpString:		ADD				{ $$ = "+";		}
			|	SUB				{ $$ = "-";		}
			|	MUL				{ $$ = "*";		}
			|	DIV				{ $$ = "/";		}
			|	MOD				{ $$ = "%";		}
			|	GT				{ $$ = ">";		}
			|	LT				{ $$ = "<";		}
			|	NE				{ $$ = "!=";	}
			|	EQ				{ $$ = "==";	}
			|	GE				{ $$ = ">=";	}
			|	LE				{ $$ = "<=";	}
			|	ASSIGN			{ $$ = "=";		}
			|	ADD_POSTFIX		{ $$ = "+_";	}
			|	SUB_POSTFIX		{ $$ = "-_";	}
			|	MUL_POSTFIX		{ $$ = "*_";	}
			|	DIV_POSTFIX		{ $$ = "/_";	}
			|	MOD_POSTFIX		{ $$ = "%_";	}
			|	GT_POSTFIX		{ $$ = ">_";	}
			|	LT_POSTFIX		{ $$ = "<_";	}
			|	NE_POSTFIX		{ $$ = "!=_";	}
			|	EQ_POSTFIX		{ $$ = "==_";	}
			|	GE_POSTFIX		{ $$ = ">=_";	}
			|	LE_POSTFIX		{ $$ = "<=_";	}
			|	CALL			{ $$ = "()";	}
			|	CAST			{ $$ = "=()";	}
			;

KwdIdent:			IF			{ $$ = "if"; }
				|	ELSE		{ $$ = "else"; }
				|	FUNCTION	{ $$ = "function"; } 
				|	RETURN		{ $$ = "return"; } 
				|	ONEVENT		{ $$ = "onevent"; } 
				|	WHILE		{ $$ = "while"; } 
				|	FOR			{ $$ = "for"; } 
				|	FOREACH		{ $$ = "foreach"; } 
				|	NIL			{ $$ = "nil"; } 
				|	LOCAL		{ $$ = "local"; }
				|	AND			{ $$ = "and"; } 
				|	NOT			{ $$ = "not"; } 
				|	OR			{ $$ = "or"; } 
				|	LAMBDA		{ $$ = "lambda"; } 
				|	TRY			{ $$ = "try"; } 
				|	TRAP		{ $$ = "trap"; } 
				|	THROW		{ $$ = "throw"; } 
				|	USING		{ $$ = "using"; } 
				|	ASSERT		{ $$ = "assert"; }
				|	TRUE		{ $$ = "true"; } 
				|	FALSE		{ $$ = "false"; } 
				|	BREAK		{ $$ = "break"; } 
				|	CONTINUE	{ $$ = "continue"; } 
				|	STATIC		{ $$ = "static"; } 
				|	CONST		{ $$ = "const"; } 
				|	METHOD		{ $$ = "method"; } 
				|	SELF		{ $$ = "self"; } 
				|	ARGUMENTS	{ $$ = "arguments"; }
				;
				
StringConst:				StringConst STRING_CONST
								{ $$ = GET_TRANSLATOR.Translate_StringConst($2, $1); }
						|	STRING_CONST
								{ $$ = GET_TRANSLATOR.Translate_StringConst($1); }
						;

Stringify:					STRINGIFY 
								{ EI(S_EXPR); PE(T_STRINGIFY); }
						;

StringIdent:				IDENT		{ $$ = $1; }
						|	KwdIdent	{ $$ = $1; }
						;
						
StringifyDottedIdents:		Stringify StringIdent
								{ PE(T_IDENT); $$ = GET_TRANSLATOR.Translate_StringifyDottedIdents($2); }
						|	StringifyDottedIdents DOT 
								{ PE2(T_DOT, T_IDENT); }
							StringIdent
								{ $$ = GET_TRANSLATOR.Translate_StringifyDottedIdents($4, $1); }
						;
						
StringifyNamespaceIdent:	Stringify NamespacePath IDENT
								{ EG(S_NAMESPACE); PE(T_IDENT); $$ = GET_TRANSLATOR.Translate_StringifyNamespaceIdent($3); }
						;

StringConstUsed:			StringConst 
								{  PE(T_STRINGCONST); Unparse_ConstValue($$ = $1); }
						| 	StringifyDottedIdents
								{ Unparse_Stringify($$ = $1); }
						|	StringifyNamespaceIdent
								{ Unparse_Stringify($$ = $1); }
						;

/**************************************************************************/
/* FUNCTION CALLS */
				
FunctionAndTableObject:	
			SELF 
				{ EI(S_EXPR); PE(T_SELF); Unparse_BuiltIn($$ = GET_TRANSLATOR.Translate_SELF(), SELF); }
		|	LAMBDA_REF
				{ EI(S_EXPR); PE(T_LAMBDA_REF); Unparse_BuiltIn($$ = GET_TRANSLATOR.Translate_LAMBDA_REF(), LAMBDA_REF); }		
		|	NEWSELF
				{ EI(S_EXPR); PE(T_NEWSELF); Unparse_BuiltIn($$ = GET_TRANSLATOR.Translate_NEWSELF(), NEWSELF); }
		|	StringConstUsed		
				{ EI(S_EXPR); $$ = $1; }
		|	ARGUMENTS 			
				{ EI(S_EXPR); PE(T_ARGUMENTS); $$ = GET_TRANSLATOR.Translate_ARGUMENTS(); Unparse_BuiltIn($$, ARGUMENTS); }
		|	Lvalue				
				{ $$ = $1; }
		|	TableConstructor	
				{ PE(S_TABLE); $$ = $1; }
		
		|	'(' Expression		
				{ PE3(T_LPAR, S_EXPR, T_RPAR); } 
			')'					
				{ $$ = DNPTR($2)->CheckUninitialised(); Unparse_ParenthesisedExpr($$); }
			
		|	FunctionCall		
				{ PE(S_CALL); $$ = $1; }
		
		|	'(' Function		
				{ SM(S_EXPR); PE3(T_LPAR, S_FUNC_($2), T_RPAR); }  
			')'					
				{	EM(S_EXPR); $$ = GET_TRANSLATOR.Translate_FunctionParenthesisForm($2); 
					Unparse_FunctionParenthesisForm($$, $2);	}
		;

/*ACTUAL ARGUMENTS***********/

ActualArgumentsList:	/* Empty */	
							{ unullify($$); }
					|	NonEmptyActualArgumentsList 
							{ $$ = $1; }
					;
					
NonEmptyActualArgumentsList:	
						NonEmptyActualArgumentsList ','		
							{ PE(T_COMMA); }  
						ActualArgument						
							{ $$ = GET_TRANSLATOR.Translate_ExpressionList($1, GET_TRANSLATOR.Translate_ExpressionListItem($4)); }						
					|	ActualArgument						
							{ $$ = GET_TRANSLATOR.Translate_ExpressionList(NIL_EXPR, GET_TRANSLATOR.Translate_ExpressionListItem($1)); }
					;

ActualArguments:		'('						
							{ SM(S_CALL); PE2(S_EXPR, T_LPAR); SG(S_AARGS); }
						ActualArgumentsList		
							{ PE(T_RPAR); }
						')'	%prec PARENTHESIS	
							{ EG(S_AARGS); EM(S_CALL); $$ = $3; }
					|	CALL					
							{ PE2(T_LPAR, T_RPAR); unullify($$); }
					;
					
ActualArgument:				{ PE(S_EXPR); } 
						Expression	
							{ $$ = $2->AdaptAsArgumentVariable(); }						
					|	'|'			
							{ PE2(T_BAR, S_EXPR); } 
						Expression	
							{ PE(T_BAR); } 
						'|'			
							{	$$ = $3->AdaptAsArgumentVariable(); 
								DNPTR($$)->SetLateBound(); 
								Unparse_LateBound($$, $3);	}					
					|	TRIPLE_DOT	
							{ PE(T_TDOT); Unparse_BuiltIn($$ = GET_TRANSLATOR.Translate_TRIPLE_DOT(), TRIPLE_DOT); }
					|	Function 
							{ $$ = GET_TRANSLATOR.Translate_FunctionExpresssion($1); }
					;
				
/*FUNCTION CALL EXPRESSION***/

FunctionCall:		FunctionCallObject ActualArguments			
						{	UNPARSABLE_GET(Unparse_FunctionCall($1, $2));
							$$ = GET_TRANSLATOR.Translate_FunctionCall($1->CheckUninitialised(), $2); 
							UNPARSABLE_SET($$);		}
					;

ExpressionList:			ExpressionList ',' LN M Expression 
							{	$$ = GET_TRANSLATOR.Translate_ExpressionList($1, GET_TRANSLATOR.Translate_ExpressionListItem($5)); 
								GET_QUADS.SetQuadLine($4, $3);	}
					|	LN M Expression 
							{	$$ = GET_TRANSLATOR.Translate_ExpressionList(NIL_EXPR, GET_TRANSLATOR.Translate_ExpressionListItem($3)); 
								GET_QUADS.SetQuadLine($2, $1);	}
					;

/**************************************************************************/
/* LVALUES */

				
NamespacePath:		GLOBAL_SCOPE
						{ EI(S_EXPR); SG(S_NAMESPACE); PE2(T_GLOBAL, T_IDENT); 
						  GET_PARSEPARMS.AppendToNamespacePath(DELTA_LIBRARYNAMESPACE_SEPARATOR); }
				|	IDENT GLOBAL_SCOPE
						{ EI(S_EXPR); SG(S_NAMESPACE); PE3(T_IDENT, T_GLOBAL, T_IDENT); 
						  GET_PARSEPARMS.AppendToNamespacePath($1); }
				|	NamespacePath IDENT GLOBAL_SCOPE
						{ PE2(T_GLOBAL, T_IDENT); 
						  GET_PARSEPARMS.AppendToNamespacePath($2); }
				;
		
Lvalue:				IDENT
						{ EI(S_EXPR); PE(T_IDENT); 
						  $$ = GET_TRANSLATOR.Translate_Lvalue($1); Unparse_Var($$, $1); }
						  
				|	NamespacePath IDENT
						{ EG(S_NAMESPACE); std::string ns;
						  $$ = GET_TRANSLATOR.Translate_NamespaceLvalue($2, &ns); Unparse_Var($$, ns); }
						  
				|	AttributeId
						{ $$ = GET_TRANSLATOR.Translate_AttrLvalue($1); Unparse_Var($$, $1, ATTRIBUTE); }
						
				|	STATIC 
						{ EI(S_EXPR); PE2(T_STATIC, T_IDENT); } 
					IDENT
						{ $$ = GET_TRANSLATOR.Translate_StaticLvalue($3); Unparse_Var($$, $3, STATIC); }
						
				|	LOCAL 
						{ EI(S_EXPR); PE2(T_LOCAL, T_IDENT); } 
					IDENT
						{ $$ = GET_TRANSLATOR.Translate_Lvalue($3, 0); Unparse_Var($$, $3, LOCAL); }
													
				|	TableContent
						{ $$ = $1; }
				;

TableObject:				FunctionAndTableObject 
								{ $$ = DNPTR($1)->AdaptIfTableContent(); }	
							;
							
FunctionCallObject:			FunctionAndTableObject 
								{ $$ = $1; }		
							;

TableContentDot:			TableObject DOT			
								{ PE(T_DOT); SG(S_DOTINDEX); $$ = $1; }
							;

TableContentBracket:		TableObject '['	
								{ PE2(T_LB, S_EXPR); $$ = $1; }
							;

TableContentBoundedDot:		TableObject DOUBLE_DOT
								{ PE(T_DDOT); SG(S_DOTINDEX); $$ = $1; }
							;

TableContentBoundedBracket:	TableObject DOUBLE_LB
								{ PE2(T_DLB, S_EXPR); $$ = $1; }
							;

DottedOpString:					DOT_ASSIGN		{ $$ = ".=";	}
							|	DOT_EQUAL		{ $$ = ".==";	}
							|	DOT_CAST		{ $$ = ".=()";	}
							|	DOT_EQUAL_RHS	{ $$ = ".==_";	}
							;
								
DotIndex:						IDENT	
									{ $$ = GET_TRANSLATOR.Translate_StringConst(GET_STRINGHOLDER.StringWithLateDestruction(ucopystr($1))); }
							|	KwdIdent	
									{ $$ = GET_TRANSLATOR.Translate_StringConst($1); }
							|	OpString
									{ $$ = GET_TRANSLATOR.Translate_StringConst($1); }
							|	StringConstUsed
									{ $$ = $1; }
							;
							
SpecialDotIndex:			DottedOpString
								{ $$ = GET_TRANSLATOR.Translate_StringConst($1); }	
							;

/*TABLECONTENT**************/

TableContent:		TableContentDot DotIndex
						{	EG(S_DOTINDEX); PE(S_DOTINDEX); 
							$$ = GET_TRANSLATOR.Translate_TableContent($1->CheckUninitialised(), $2); 
							Unparse_TableContentDot($$, $1, $2->strConst.c_str()); }
							
				|	TableObject SpecialDotIndex
						{	EG(S_DOTINDEX); PE(S_DOTINDEX); 
							$$ = GET_TRANSLATOR.Translate_TableContent($1->CheckUninitialised(), $2); 
							Unparse_TableContentDot($$, $1, $2->strConst.c_str()); }		
																								
				|	TableContentBoundedDot DotIndex
						{	EG(S_DOTINDEX); PE(S_DOTINDEX);
							$$ = GET_TRANSLATOR.Translate_BoundedTableContent($1->CheckUninitialised(), $2); 
							Unparse_TableContentDoubleDot($$, $1, $2->strConst.c_str()); }
						
				|	TableContentBracket Expression 
						{ PE(T_RB); } 
					']'	%prec SQUARE_BRACKETS
						{	$$ = GET_TRANSLATOR.Translate_TableContent($1->CheckUninitialised(), $2->CheckUninitialised()); 
							Unparse_TableContentBracket($$, $1, $2);	}
						
				|	TableContentBracket OperatorMethod 
						{ PE(T_RB); } 
					']'	%prec SQUARE_BRACKETS
						{	$$ = GET_TRANSLATOR.Translate_TableContent($1->CheckUninitialised(), $2); 
							Unparse_TableContentBracket($$, $1, $2);	}
							
				|	TableContentBoundedBracket Expression 
						{ PE(T_DRB); } 
					DOUBLE_RB %prec SQUARE_BRACKETS
						 {	$$ = GET_TRANSLATOR.Translate_BoundedTableContent($1->CheckUninitialised(), $2->CheckUninitialised()); 
							Unparse_TableContentDoubleBracket($$, $1, $2);	}				

				|	TableContentBoundedBracket OperatorMethod 
						{ PE(T_DRB); } 
					DOUBLE_RB %prec SQUARE_BRACKETS
						 {	$$ = GET_TRANSLATOR.Translate_BoundedTableContent($1->CheckUninitialised(), $2); 
							Unparse_TableContentDoubleBracket($$, $1, $2);	}				
				;

/**************************************************************************/
/* TABLE CONSTRUCTOR */

TablePrefix:		'['
						{	SM(S_TABLE); PE2(T_LB, S_TABLIST); 
							GET_PARSEPARMS.InTableExpr().enter(); $$ = GET_TRANSLATOR.Translate_TablePrefix(); }
					;

TableSuffix:			{ PE(T_RB); }  
					']' 
						{ EM(S_TABLE); }
					;
				
TableConstructor:		TablePrefix	TableElements TableSuffix
							{	GET_PARSEPARMS.InTableExpr().exit();
								UNPARSABLE_GET(Unparse_TableConstructor($2)); 
								$$ = GET_TRANSLATOR.Translate_TableConstructor($1, $2); 
								UNPARSABLE_SET($$); }
								
					|	TablePrefix TableSuffix
							{	GET_PARSEPARMS.InTableExpr().exit();
								UNPARSABLE_GET(Unparse_TableConstructor()); 
								$$ = GET_TRANSLATOR.Translate_TableConstructor($1); 
								UNPARSABLE_SET($$); }
					;

TableElements:			TableElements ',' { PE(T_COMMA); } TableElement
							{	UNPARSABLE_GET(Unparse_TableElements($1, $4));
								$$ = GET_TRANSLATOR.Translate_TableElements($1, $4); 
								UNPARSABLE_SET($$); }
					|	TableElement
							{ $$ = $1; }
					;

FunctionElement:	Function
						{ PE(S_FUNC_($1)); $$ = GET_TRANSLATOR.Translate_FunctionExpresssion($1); }
					;

/* 
	This ungly hack combined with the repetition of M_elem in TableElement is needed
	to ensure UnindexedValue and IdentIndexElement can be handled in parallel in the
	parser stack.
*/

UnindexedValue:			Expression LN
							{ $$ = GET_TRANSLATOR.Translate_UnindexedValue($1, $<quadNo>-1, $2); }
					|	FunctionElement LN
							{ $$ = GET_TRANSLATOR.Translate_UnindexedValue($1, $<quadNo>-1, $2); }
					;

M_elem:				M { OE(T_TABLE_ELEM); $$ = $1; }
					;
					
TableElement:			M_elem UnindexedValue
							{ $$ = $2; }		
					|	M_elem IndexedValues
							{ Unparse_IndexedValues($$ = $2); }
					|   M_elem NewAttribute
							{ Unparse_UnindexedValue($$ = GET_TRANSLATOR.Translate_TableElement($2), $2); }
					|	M_elem IdentIndexElement
							{ $$ = $2; }
					;
			
DottedIdent:		DOT 
						{ PE2(T_DOT, T_IDENT); } 
					StringIdent
						{ Unparse_DottedIdent($$ = GET_TRANSLATOR.Translate_ConstValue($3), $3); }
					;

OperatorIndex:		OpString 
						{	PE(T_OPINDEX); 
							$$ = GET_TRANSLATOR.Translate_ConstValue($1); 
							Unparse_OperatorIndex($$, $1);	}
					;

IndexContent:			Expression			{ $$ = $1; }
					|	DottedIdent			{ $$ = $1; }
					|	OperatorIndex		{ $$ = $1; }
					;

IndexExpression:		{ PE(S_INDEXEXPR); } 
					IndexContent 
						{ $$ = $2; }
					;

IndexedList:		IndexedList 
							{ PE(T_COMMA); } 
					','  M IndexExpression LN
							{	$$ = GET_TRANSLATOR.Translate_ExpressionList($1, GET_TRANSLATOR.Translate_ExpressionListItem($5->AdaptAsArgumentVariable()));
								GET_QUADS.SetQuadLine($4, $6);	}
					| M IndexExpression LN
							{	$$ = GET_TRANSLATOR.Translate_ExpressionList(NIL_EXPR, GET_TRANSLATOR.Translate_ExpressionListItem($2->AdaptAsArgumentVariable()));
								GET_QUADS.SetQuadLine($1, $3);	}
					;

IndexedValues:		'{' 
						{ PE(T_LBC); SG(N_EMPTY); } 
					IndexedList 
						{ EG(N_EMPTY); PE2(S_INDICES, T_COLON); } 
					':' 
						{ SG(S_ELIST); } 
					ContentList 
						{ EG(S_ELIST); PE2(S_ELIST, T_RBC); } 
					'}'
						{ $$ = GET_TRANSLATOR.Translate_IndexedValues($3, $7); }
					;

IdentIndex:				AttributeId	
							{ $$ = $1; }
					|	DOT 
							{ PE(T_DOT); OE(T_IDENT_OR_KWDIDENT); } 
						AttributeIdent
							{ $$ = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr($3)); }
					;
					
IdentIndexElement:	IdentIndex ':'
						{ PE(T_COLON); SG(S_EXPR); }
					M ContentExpression LN
						{ EG(S_EXPR); PE(S_EXPR); $$ = GET_TRANSLATOR.Translate_IdentIndexElement($1, $5, $4, $6); }
					;
					
ContentList:			ContentList ',' LN M ContentExpression 
							{	$$ = GET_TRANSLATOR.Translate_ExpressionList($1, GET_TRANSLATOR.Translate_ExpressionListItem($5));
								GET_QUADS.SetQuadLine($4, $3);	}
					|	LN M ContentExpression 
							{	$$ = GET_TRANSLATOR.Translate_ExpressionList(NIL_EXPR, GET_TRANSLATOR.Translate_ExpressionListItem($3));
								GET_QUADS.SetQuadLine($2, $1);	}
					;

ContentExpression:		Expression		{ $$ = $1; }
					|	Function		{ PE(S_FUNC_($1)); $$ = GET_TRANSLATOR.Translate_FunctionExpresssion($1); }
					;
					
/**************************************************************************/
/* ATTRIBUTES */

AttributeIdent:			IDENT			{ PE(T_IDENT); $$ = $1; }
					|	KwdIdent		{ PE(T_KWDIDENT); $$ = $1; }
					;
						
AttributeId:			ATTRIBUTE_IDENT 
							{	PE(T_ATTRIBUTEID); OE(T_LBC_OR_COLON); 
								$$ = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr($1 + 1)); }
					|	ATTRIBUTE 
								{ PE(T_ATTRIBUTE); OE(T_IDENT_OR_KWDIDENT); }
						AttributeIdent
								{ $$ = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr($3)); }
					;
				
NewAttribute:	AttributeId AttributeSet AttributeGet
					{ $$ = GET_TRANSLATOR.Translate_NewAttribute($1, $2, $3); }
			;
				
AttributeSet:	'{'	
					{ PE2(T_LBC, T_SET);  }
				SET 				
					{ SG(S_EXPR); } 
				M ContentExpression LN		
					{	EG(S_EXPR); 
						PE2(S_EXPR, T_GET); 
						$$ = GET_TRANSLATOR.Translate_AttributeMethod($6, $5, $7); }
				;
				
AttributeGet:	GET					
					{ SG(S_EXPR); }
				M ContentExpression LN	
					{ EG(S_EXPR); PE2(S_EXPR, T_RBC); }
				'}'	
					{	 
						$$ = GET_TRANSLATOR.Translate_AttributeMethod($4, $3, $5); }
				;

/**************************************************************************/
/* BRANCHES AND LOOPS */

BreakStmt:		{ SM(S_STMT); PE(T_BREAK);	}	BREAK		Semi
ContinueStmt:	{ SM(S_STMT); PE(T_CONT);	}	CONTINUE	Semi

Condition:		'('				{ SG(S_EXPR); } 
				LN Expression	{ EG(S_EXPR); PE2(S_EXPR, T_RPAR); } 
				')'				{  $$ = GET_TRANSLATOR.Translate_Condition($4); GET_TRANSLATOR.Translate_BasicStmt($3);	}	
				;

N:				{ $$ = GET_TRANSLATOR.Translate_N(); }					
				;

/* WHILE*********************/

WhilePrefix:	WHILE	
					{ SM(S_WHILE); PE2(T_WHILE, T_LPAR); GET_TRANSLATOR.Translate_WhilePrefix(); }	
				;

WhileStmt:		WhilePrefix M Condition LN M Stmt
					{	EM(S_WHILE); 
						UNPARSABLE_GET(Unparse_While($3, $6));
						$$ = GET_TRANSLATOR.Translate_WhileStmt($3, $2, $5, $6, $4); 
						UNPARSABLE_SET($$); }
				;

/* IF************************/

IfPrefix:		IF		{ SM(S_IF); PE2(T_IF, T_LPAR);  }												;
ElsePrefix:		ELSE	{ EM(S_IF); SM(S_ELSE); PE(T_ELSE); GET_TRANSLATOR.Translate_ElseStmtPrefix(); }	;
M_If:			M		{ GET_TRANSLATOR.Translate_IfStmtPrefix(); $$ = $1; }								;
		
IfStmt:				IfPrefix Condition M_If Stmt
						{	EM(S_IF);
							GET_TRANSLATOR.Translate_IfStmt($2, $3);
							Unparse_If($$ = $4, $2, $4); }
				|	IfPrefix Condition M_If Stmt ElsePrefix N M Stmt
						{	EM(S_ELSE);
							GET_TRANSLATOR.Translate_IfElseStmt($2, $3, $6, $7);
							UNPARSABLE_GET(Unparse_IfElse($2, $4, $8));
							GET_DELTASYMBOLS.ResetTemp();
							$$ = GET_TRANSLATOR.Translate_Stmts($4, $8); 
							UNPARSABLE_SET($$);	}
				;

/* FOR***********************/

ForStmt:		ForPrefix M ForCondition M ForSuffix N M Stmt
					{	EM(S_FOR); 
						UNPARSABLE_GET(Unparse_For($1, $3, $5, $8));
						$$ = GET_TRANSLATOR.Translate_ForStmt($3, $2, $4, $7, $6, $8); 
						UNPARSABLE_SET($$); }
				;

ForOpening:		FOR		
					{ SM(S_FOR); PE2(T_FOR, T_LPAR); } 
				'('		
					{ PE(S_ELIST); GET_TRANSLATOR.Translate_ForOpening(); } 
				;
				
ForPrefix:		ForOpening M LN ForInitList  
					{ GET_TRANSLATOR.Translate_ForPrefix($2, $3);  $$ = $4; GET_TRANSLATOR.Translate_BasicStmt($3); }
				;

ForInitList	:		ExpressionList Semi
						{ $$ = NEW_STMT; Unparse_ForInitList($$, $1); }
				|	Semi
						{ EI(S_ELIST); $$ = NEW_STMT; Unparse_ForInitList($$); }
				;

ForCondition:		{ PE(S_EXPR); }		
				 M Expression LN Semi
					{	$$ = GET_TRANSLATOR.Translate_Condition($3); 
						GET_QUADS.SetQuadLine($2, $4, true); 
						GET_TRANSLATOR.Translate_BasicStmt($4); }
				;
			
ForRepeatList:			{ PE(S_ELIST); } 
					ExpressionList ForEnd
						{	$$ = NEW_STMT; 
							UNPARSABLE_GET(Unparse_ExprList($2));
							UNPARSABLE_SET($$); 
							GET_TRANSLATOR.Translate_BasicStmt($3); }
				|	ForEnd
						{ $$ = NEW_STMT; }
				;

ForEnd:					{ PE(T_RPAR); } 
				LN ')'	{ $$ = $2; }	
				;

ForSuffix:		LN M ForRepeatList
					{  GET_QUADS.SetQuadLine($2, $1, true);  $$ = $3; }
				;

/* FOREACH*******************/

ForeachValue:		':'					{	PE(S_LVAL);		}
					Lvalue				{	PE(T_COMMA);	}
					','					{	$$ = $3;		}
				|	','					{	unullify($$);	}
				;
				
ForeachPrefix:	FOREACH					{	SM(S_FOREACH); PE2(T_FOREACH, T_LPAR);	}
				'('						{	PE(S_LVAL);				}
				Lvalue					{	PE(T_SEMI_OR_COMMA);	}
				ForeachValue			{	PE(S_EXPR);				}
				LN M Expression	')'		{	UNPARSABLE_GET(Unparse_ForeachPrefix($5, $7, $11)); 
											if ($7)
												$$ = GET_TRANSLATOR.Translate_ForeachPrefix($7, $5, $11); 
											else
												$$ = GET_TRANSLATOR.Translate_ForeachPrefix($5, NIL_EXPR, $11); 
											GET_QUADS.SetQuadLine($10, $9);
											UNPARSABLE_SET($$);		}
				;

ForeachStmt:	ForeachPrefix M			{	PE(S_STMT);		}
				Stmt					{	UNPARSABLE_GET(Unparse_ForeachStmt($1, $4));
											$$ = GET_TRANSLATOR.Translate_ForeachStmt($1, $4, $2);
											UNPARSABLE_SET($$);	
											EM(S_FOREACH);	}
				;

/**************************************************************************/
/* EXCEPTION HANDLING */

ThrowStmt:		THROW 
					{ SM(S_THROW); PE2(T_THROW, S_EXPR); } 
				Expression Semi
					{	EM(S_THROW); 
						$$ = GET_TRANSLATOR.Translate_THROW($3); 
						Unparse_ExprStmt($$, THROW, $3); }
				;

TryHeader:		TRY		{ SM(S_TRY); PE2(T_TRY, S_STMT); $$ = GET_TRANSLATOR.Translate_TRY(); }				;
TrapHeader:		TRAP	{ PE(T_IDENT); $$ = GET_TRANSLATOR.Translate_TRAP(); }								;
ExceptionVar:	IDENT	{ $$ = GET_TRANSLATOR.Translate_Lvalue($1); if ($$) DPTR($$)->SetInitialised(); }	;
TrapJumpOver:			{ $$ = GET_TRANSLATOR.Translate_TrapJumpOver(); }									;

ExceptionStmt:	TryHeader Stmt 
					{ PE(T_TRAP); }
				TrapHeader TrapJumpOver ExceptionVar
					{ GET_TRANSLATOR.Translate_TrapStart($1, $4, $6); }
				Stmt
					{	EM(S_TRY);
						GET_TRANSLATOR.Translate_TrapEnd($5);
						UNPARSABLE_GET(Unparse_TryTrap($2, $8, $6));
						GET_DELTASYMBOLS.ResetTemp();
						$$ = GET_TRANSLATOR.Translate_Stmts($2, $8); 
						UNPARSABLE_SET($$);		}
				;			
				
/**************************************************************************/

%%
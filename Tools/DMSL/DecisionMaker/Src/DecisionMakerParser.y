%{
#if defined(WIN32) || defined(_WIN32_WCE)
#define YY_NO_UNISTD_H
#define YYINCLUDED_STDLIB_H
#endif

//Remove automatically generated file warnings
#ifdef  _MSC_VER
#pragma warning(disable:4065)
#endif

#include <stdio.h>
#include <stdlib.h>

#include "ManageExpressions.h"
#include "ManageStatements.h"
#include "util.h"

using namespace dmsl;

#include "DecisionMakerBisonParser.h"	//must be included after ManageExpressions and ManageStatements

void yyerror (DecisionMaker *dm, Statement**, const char *msg) {
	extern int DMSL_yylineno;
	extern char *DMSL_yytext;
	dm->SetError(util::MakeString("Line %d : %s: before token: %s\n", DMSL_yylineno, msg, DMSL_yytext));
}

extern int yylex (YYSTYPE* lvalp);

#define ABORT_ON_ERROR(x)	do { if(!x) YYABORT; } while(false)
%}

%output = "Src/DecisionMakerBisonParser.cpp"
%name-prefix = "DMSL_yy"
%pure-parser
%debug
%parse-param { DecisionMaker *dm }
%parse-param { Statement **logic }
%start logic

%token 	INT_VALUE REAL_VALUE BOOL_VALUE STRING_VALUE ID_VALUE
%token	IF THEN ELSE CASE WHEN OTHERWISE
%token	ACTIVATE CANCEL EVALUATE LIBRARY
%token	IS_ACTIVE TO_NUMBER HAS_ATTR RANDOM CAN_SUPPORT
%token	COMPONENT DMSL_IN PARAMS STEREOTYPE DEFINE

%token	'(' ')' '[' ']' '{' '}'
%token	'+' '-' '*' '/' '%'
%token	'.' ',' ':'
%token	OR AND EQ NEQ GT LT GE LE
%token	AND OR NOT DOUBLEDOT
%token	UNARY DMSL_ERROR

%left		OR
%left		AND
%right		NOT
%left		DMSL_IN
%left		EQ NEQ
%left		GT LT GE LE
%nonassoc	DOUBLEDOT
%left		'+' '-'
%left		'*' '/' '%'
%right		UNARY
%left		'.'
%left		'(' ')'

%union
{
  int 			intVal;
  bool 			boolVal;
  double 		realVal;
  const char*	stringVal;
  Expression*	expr;
  ExprList*		exprList;
  Statement*	stmt;
  StmtList*		stmtList;
}

%token	<intVal> 	INT_VALUE
%token	<realVal>	REAL_VALUE
%token	<stringVal>	STRING_VALUE ID_VALUE
%token	<boolVal> 	BOOL_VALUE

%type	<stringVal>	identifier dottedId
%type	<expr>		expr boolexpr arithexpr
%type	<exprList>	exprList exprListNonEmpty
%type	<stmt>		logic decl compound stmt elseSt when otherwise
%type	<stmtList>	declList stmts stmtList whenList whenListNonEmpty

%destructor { delete[] $$; } ID_VALUE STRING_VALUE identifier dottedId

%expect 1	//if - else shift reduce
%%

logic				:	declList	{ Manage_logic_declList	(dm, logic, $1); }
					|	/*empty*/	{ Manage_logic_empty	(dm, logic);	 }
					;

declList			:	declList decl	{ Manage_declList_declList	(dm, &$$, $1, $2);	ABORT_ON_ERROR($$);	}
					|	decl			{ Manage_declList_decl		(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					;

decl				:	COMPONENT identifier compound		{ Manage_decl_component	(dm, &$$, $2, $3);	ABORT_ON_ERROR($$); }
					|	STEREOTYPE identifier ':' boolexpr	{ Manage_decl_stereotype(dm, &$$, $2, $4);	ABORT_ON_ERROR($$); }
					|	DEFINE identifier expr				{ Manage_decl_define	(dm, &$$, $2, $3);	ABORT_ON_ERROR($$); }
					;

dottedId			:	dottedId '.' identifier	{ Manage_dottedId_dottedId	(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	identifier				{ Manage_dottedId_identifier(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					;
	
identifier			:	STRING_VALUE	{ Manage_identifier_string	(dm, &$$, $1);	ABORT_ON_ERROR($$); }
					|	ID_VALUE		{ Manage_identifier_id		(dm, &$$, $1);	ABORT_ON_ERROR($$); }
					;

compound			:	'[' stmts ']'	{ Manage_compound(dm, &$$, $2); ABORT_ON_ERROR($$); }
					;

stmts				:	stmtList	{ Manage_stmts_stmtList	(dm, &$$, $1);	ABORT_ON_ERROR($$); }
					|	/*empty*/	{ Manage_stmts_empty	(dm, &$$);		ABORT_ON_ERROR($$); }
					;

stmtList			:	stmtList stmt	{ Manage_stmtList_stmtList	(dm, &$$, $1, $2);	ABORT_ON_ERROR($$); }
					|	stmt			{ Manage_stmtList_stmt		(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					;

stmt				:	IF boolexpr THEN stmt elseSt			{ Manage_stmt_if		(dm, &$$, $2, $4, $5);	ABORT_ON_ERROR($$); }
					|	CASE expr '[' whenList otherwise ']'	{ Manage_stmt_case		(dm, &$$, $2, $4, $5);	ABORT_ON_ERROR($$); }
					|	ACTIVATE	expr 						{ Manage_stmt_activate	(dm, &$$, $2);			ABORT_ON_ERROR($$); }
					|	CANCEL		expr 						{ Manage_stmt_cancel	(dm, &$$, $2);			ABORT_ON_ERROR($$); }
					|	EVALUATE 	expr 						{ Manage_stmt_evaluate	(dm, &$$, $2);			ABORT_ON_ERROR($$); }
					|	LIBRARY		'(' exprList ')'			{ Manage_stmt_library	(dm, &$$, $3);			ABORT_ON_ERROR($$); }					
					|	compound 								{ Manage_stmt_compound	(dm, &$$, $1);			ABORT_ON_ERROR($$); }
					;

elseSt				:	ELSE stmt	{ Manage_elseSt_else	(dm, &$$, $2);	ABORT_ON_ERROR($$); }
					|	/*empty*/	{ Manage_elseSt_empty	(dm, &$$);		ABORT_ON_ERROR($$); }
					;

whenList			:	whenListNonEmpty 	{ Manage_whenList_whenListNonEmpty	(dm, &$$, $1);	ABORT_ON_ERROR($$); }
					|	/*empty*/			{ Manage_whenList_empty				(dm, &$$);		ABORT_ON_ERROR($$); }
					;

whenListNonEmpty	:	whenListNonEmpty when 	{ Manage_whenListNonEmpty_whenListNonEmpty	(dm, &$$, $1, $2);	ABORT_ON_ERROR($$); }
					|	when					{ Manage_whenListNonEmpty_when				(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					;

when				:	WHEN expr stmt { Manage_when(dm, &$$, $2, $3);	ABORT_ON_ERROR($$); }
					;	

otherwise			:	OTHERWISE stmt	{ Manage_otherwise_otherwise(dm, &$$, $2);	ABORT_ON_ERROR($$); }
					|	/*empty*/		{ Manage_otherwise_empty	(dm, &$$);		ABORT_ON_ERROR($$); }
					;

expr				:	boolexpr			{ Manage_expr_boolexpr	(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					|	arithexpr			{ Manage_expr_arithexpr	(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					|	expr DOUBLEDOT expr	{ Manage_expr_doubledot	(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	'{' exprList '}'	{ Manage_expr_set		(dm, &$$, $2);		ABORT_ON_ERROR($$); }
					|	'[' exprList ']'	{ Manage_expr_rangelist	(dm, &$$, $2);		ABORT_ON_ERROR($$); }
					;

exprList			:	exprListNonEmpty	{ Manage_exprList_exprListNonEmpty	(dm, &$$, $1);	ABORT_ON_ERROR($$); }
					|	/*empty*/			{ Manage_exprList_empty				(dm, &$$);		ABORT_ON_ERROR($$); }
					;

exprListNonEmpty	:	exprListNonEmpty ',' expr	{ Manage_exprListNonEmpty_exprListNonEmpty	(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr						{ Manage_exprListNonEmpty_expr				(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					;

boolexpr			:	expr OR  expr					{ Manage_boolexpr_or			(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr AND expr					{ Manage_boolexpr_and			(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr LT  expr					{ Manage_boolexpr_lessThan		(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr LE  expr					{ Manage_boolexpr_lessEqual		(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr GT  expr					{ Manage_boolexpr_greaterThan	(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr GE  expr					{ Manage_boolexpr_greaterEqual	(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr EQ  expr					{ Manage_boolexpr_equal			(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr NEQ expr					{ Manage_boolexpr_notEqual		(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr DMSL_IN  expr				{ Manage_boolexpr_in			(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	NOT expr						{ Manage_boolexpr_not			(dm, &$$, $2);		ABORT_ON_ERROR($$); }
					|	IS_ACTIVE	'(' exprList ')'	{ Manage_boolexpr_isactive		(dm, &$$, $3);		ABORT_ON_ERROR($$); }
					|	HAS_ATTR	'(' exprList ')'	{ Manage_boolexpr_hasattr		(dm, &$$, $3);		ABORT_ON_ERROR($$); }
					|	CAN_SUPPORT	'(' exprList ')'	{ Manage_boolexpr_cansupport	(dm, &$$, $3);		ABORT_ON_ERROR($$); }
					|	BOOL_VALUE						{ Manage_boolexpr_bool			(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					// Declare the following here as they may have bool values and we are only interested in boolexrs (and not arithexprs)
					|	'(' expr ')'					{ Manage_boolexpr_parenthesis	(dm, &$$, $2);		ABORT_ON_ERROR($$); }
					|	dottedId						{ Manage_boolexpr_dottedId		(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					|	PARAMS	'.' dottedId			{ Manage_boolexpr_params		(dm, &$$, $3);		ABORT_ON_ERROR($$); }
					;

arithexpr			:	expr '+' expr					{ Manage_arithexpr_add		(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr '-' expr					{ Manage_arithexpr_sub		(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr '*' expr					{ Manage_arithexpr_mul		(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr '/' expr					{ Manage_arithexpr_div		(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	expr '%' expr					{ Manage_arithexpr_mod		(dm, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	'+' expr %prec UNARY			{ Manage_arithexpr_uplus	(dm, &$$, $2);		ABORT_ON_ERROR($$); }
					|	'-' expr %prec UNARY			{ Manage_arithexpr_uminus	(dm, &$$, $2);		ABORT_ON_ERROR($$); }	
					|	TO_NUMBER	'(' exprList ')'	{ Manage_arithexpr_tonumber	(dm, &$$, $3);		ABORT_ON_ERROR($$); }
					|	RANDOM		'(' exprList ')'	{ Manage_arithexpr_random	(dm, &$$, $3);		ABORT_ON_ERROR($$); }
					|	INT_VALUE						{ Manage_arithexpr_int		(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					|	REAL_VALUE						{ Manage_arithexpr_real		(dm, &$$, $1);		ABORT_ON_ERROR($$); }
					;
%%


%{
#define YY_NEVER_INTERACTIVE 1

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

extern int DMSL_yylineno;

void yyerror (DecisionMaker *dm, Statement**, const char *msg) {
	extern char *DMSL_yytext;
	dm->SetError(util::MakeString("Line %d : %s: before token: %s\n", DMSL_yylineno, msg, DMSL_yytext));
}

extern int yylex (YYSTYPE* lvalp);

#define RESULT_ACTION(x) RESULT_ACTION_EX(x, DMSL_yylineno)
#define RESULT_ACTION_EX(x, line)	do { if (x) SetLine(x, line); else YYABORT; } while(false)

template<class T> void SetLine(T*, unsigned) {}
template<> void SetLine<Expression>(Expression* x, unsigned line) { x->SetLine(line); }
template<> void SetLine<Statement>(Statement* x, unsigned line) { x->SetLine(line); }

%}

%output = "Src/DecisionMakerBisonParser.cpp"
%name-prefix = "DMSL_yy"
%pure-parser
%debug
%parse-param { DecisionMaker *dm }
%parse-param { Statement **logic }
%start logic

%token	IF THEN ELSE CASE WHEN OTHERWISE
%token	ACTIVATE CANCEL EVALUATE LIBRARY
%token	IS_ACTIVE TO_NUMBER HAS_ATTR RANDOM CAN_SUPPORT
%token	COMPONENT DMSL_IN PARAMS STEREOTYPE DEFINE

%token	'(' ')' '[' ']' '{' '}'
%token	'+' '-' '*' '/' '%'
%token	'.' ',' ':'
%token	EQ NEQ GT LT GE LE
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
  unsigned		line;
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
%type	<line>		M

%destructor { delete[] $$; } ID_VALUE STRING_VALUE identifier dottedId

%expect 1	//if - else shift reduce
%%

M					:	/*empty*/	{ $$ = DMSL_yylineno; }
	
logic				:	declList	{ Manage_logic_declList	(dm, logic, $1); }
					|	/*empty*/	{ Manage_logic_empty	(dm, logic);	 }
					;

declList			:	declList decl	{ Manage_declList_declList	(dm, &$$, $1, $2);	RESULT_ACTION($$);	}
					|	decl			{ Manage_declList_decl		(dm, &$$, $1);		RESULT_ACTION($$); }
					;

decl				:	COMPONENT identifier M compound			{ Manage_decl_component	(dm, &$$, $2, $4);	RESULT_ACTION_EX($$, $3); }
					|	STEREOTYPE identifier M ':' boolexpr	{ Manage_decl_stereotype(dm, &$$, $2, $5);	RESULT_ACTION_EX($$, $3); }
					|	DEFINE identifier M expr				{ Manage_decl_define	(dm, &$$, $2, $4);	RESULT_ACTION_EX($$, $3); }
					;

dottedId			:	dottedId '.' identifier	{ Manage_dottedId_dottedId	(dm, &$$, $1, $3);	RESULT_ACTION($$); }
					|	identifier				{ Manage_dottedId_identifier(dm, &$$, $1);		RESULT_ACTION($$); }
					;
	
identifier			:	STRING_VALUE	{ Manage_identifier_string	(dm, &$$, $1);	RESULT_ACTION($$); }
					|	ID_VALUE		{ Manage_identifier_id		(dm, &$$, $1);	RESULT_ACTION($$); }
					;

compound			:	'[' stmts ']'	{ Manage_compound(dm, &$$, $2); RESULT_ACTION($$); }
					;

stmts				:	stmtList	{ Manage_stmts_stmtList	(dm, &$$, $1);	RESULT_ACTION($$); }
					|	/*empty*/	{ Manage_stmts_empty	(dm, &$$);		RESULT_ACTION($$); }
					;

stmtList			:	stmtList stmt	{ Manage_stmtList_stmtList	(dm, &$$, $1, $2);	RESULT_ACTION($$); }
					|	stmt			{ Manage_stmtList_stmt		(dm, &$$, $1);		RESULT_ACTION($$); }
					;

stmt				:	IF M boolexpr THEN stmt elseSt			{ Manage_stmt_if		(dm, &$$, $3, $5, $6);	RESULT_ACTION_EX($$, $2); }
					|	CASE M expr '[' whenList otherwise ']'	{ Manage_stmt_case		(dm, &$$, $3, $5, $6);	RESULT_ACTION_EX($$, $2); }
					|	ACTIVATE M	expr 						{ Manage_stmt_activate	(dm, &$$, $3);			RESULT_ACTION_EX($$, $2); }
					|	CANCEL	 M	expr 						{ Manage_stmt_cancel	(dm, &$$, $3);			RESULT_ACTION_EX($$, $2); }
					|	EVALUATE M 	expr 						{ Manage_stmt_evaluate	(dm, &$$, $3);			RESULT_ACTION_EX($$, $2); }
					|	LIBRARY	 M	'(' exprList ')'			{ Manage_stmt_library	(dm, &$$, $4);			RESULT_ACTION_EX($$, $2); }					
					|	compound 								{ Manage_stmt_compound	(dm, &$$, $1);			RESULT_ACTION($$); }
					;

elseSt				:	ELSE stmt	{ Manage_elseSt_else	(dm, &$$, $2);	RESULT_ACTION($$); }
					|	/*empty*/	{ Manage_elseSt_empty	(dm, &$$);		RESULT_ACTION($$); }
					;

whenList			:	whenListNonEmpty 	{ Manage_whenList_whenListNonEmpty	(dm, &$$, $1);	RESULT_ACTION($$); }
					|	/*empty*/			{ Manage_whenList_empty				(dm, &$$);		RESULT_ACTION($$); }
					;

whenListNonEmpty	:	whenListNonEmpty when 	{ Manage_whenListNonEmpty_whenListNonEmpty	(dm, &$$, $1, $2);	RESULT_ACTION($$); }
					|	when					{ Manage_whenListNonEmpty_when				(dm, &$$, $1);		RESULT_ACTION($$); }
					;

when				:	WHEN M expr stmt { Manage_when(dm, &$$, $3, $4);	RESULT_ACTION_EX($$, $2); }
					;	

otherwise			:	OTHERWISE M stmt	{ Manage_otherwise_otherwise(dm, &$$, $3);	RESULT_ACTION_EX($$, $2); }
					|	/*empty*/			{ Manage_otherwise_empty	(dm, &$$);		RESULT_ACTION($$); }
					;

expr				:	boolexpr			{ Manage_expr_boolexpr	(dm, &$$, $1);		RESULT_ACTION($$); }
					|	arithexpr			{ Manage_expr_arithexpr	(dm, &$$, $1);		RESULT_ACTION($$); }
					|	expr DOUBLEDOT expr	{ Manage_expr_doubledot	(dm, &$$, $1, $3);	RESULT_ACTION($$); }
					|	'{' exprList '}'	{ Manage_expr_set		(dm, &$$, $2);		RESULT_ACTION($$); }
					|	'[' exprList ']'	{ Manage_expr_rangelist	(dm, &$$, $2);		RESULT_ACTION($$); }
					;

exprList			:	exprListNonEmpty	{ Manage_exprList_exprListNonEmpty	(dm, &$$, $1);	RESULT_ACTION($$); }
					|	/*empty*/			{ Manage_exprList_empty				(dm, &$$);		RESULT_ACTION($$); }
					;

exprListNonEmpty	:	exprListNonEmpty ',' expr	{ Manage_exprListNonEmpty_exprListNonEmpty	(dm, &$$, $1, $3);	RESULT_ACTION($$); }
					|	expr						{ Manage_exprListNonEmpty_expr				(dm, &$$, $1);		RESULT_ACTION($$); }
					;

boolexpr			:	expr OR  M expr					{ Manage_boolexpr_or			(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr AND M expr					{ Manage_boolexpr_and			(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr LT  M expr					{ Manage_boolexpr_lessThan		(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr LE  M expr					{ Manage_boolexpr_lessEqual		(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr GT  M expr					{ Manage_boolexpr_greaterThan	(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr GE  M expr					{ Manage_boolexpr_greaterEqual	(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr EQ  M expr					{ Manage_boolexpr_equal			(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr NEQ M expr					{ Manage_boolexpr_notEqual		(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr DMSL_IN M expr				{ Manage_boolexpr_in			(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	NOT M expr						{ Manage_boolexpr_not			(dm, &$$, $3);		RESULT_ACTION_EX($$, $2); }
					|	IS_ACTIVE	M '(' exprList ')'	{ Manage_boolexpr_isactive		(dm, &$$, $4);		RESULT_ACTION_EX($$, $2); }
					|	HAS_ATTR	M '(' exprList ')'	{ Manage_boolexpr_hasattr		(dm, &$$, $4);		RESULT_ACTION_EX($$, $2); }
					|	CAN_SUPPORT	M '(' exprList ')'	{ Manage_boolexpr_cansupport	(dm, &$$, $4);		RESULT_ACTION_EX($$, $2); }
					|	BOOL_VALUE						{ Manage_boolexpr_bool			(dm, &$$, $1);		RESULT_ACTION($$); }
					// Declare the following here as they may have bool values and we are only interested in boolexrs (and not arithexprs)
					|	'(' M expr ')'					{ Manage_boolexpr_parenthesis	(dm, &$$, $3);		RESULT_ACTION_EX($$, $2); }
					|	dottedId						{ Manage_boolexpr_dottedId		(dm, &$$, $1);		RESULT_ACTION($$); }
					|	PARAMS M '.' dottedId			{ Manage_boolexpr_params		(dm, &$$, $4);		RESULT_ACTION_EX($$, $2); }
					;

arithexpr			:	expr '+' M expr					{ Manage_arithexpr_add		(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr '-' M expr					{ Manage_arithexpr_sub		(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr '*' M expr					{ Manage_arithexpr_mul		(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr '/' M expr					{ Manage_arithexpr_div		(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	expr '%' M expr					{ Manage_arithexpr_mod		(dm, &$$, $1, $4);	RESULT_ACTION_EX($$, $3); }
					|	'+' M expr %prec UNARY			{ Manage_arithexpr_uplus	(dm, &$$, $3);		RESULT_ACTION_EX($$, $2); }
					|	'-' M expr %prec UNARY			{ Manage_arithexpr_uminus	(dm, &$$, $3);		RESULT_ACTION_EX($$, $2); }	
					|	TO_NUMBER	M '(' exprList ')'	{ Manage_arithexpr_tonumber	(dm, &$$, $4);		RESULT_ACTION_EX($$, $2); }
					|	RANDOM		M '(' exprList ')'	{ Manage_arithexpr_random	(dm, &$$, $4);		RESULT_ACTION_EX($$, $2); }
					|	INT_VALUE						{ Manage_arithexpr_int		(dm, &$$, $1);		RESULT_ACTION($$); }
					|	REAL_VALUE						{ Manage_arithexpr_real		(dm, &$$, $1);		RESULT_ACTION($$); }
					;
%%


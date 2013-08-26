%{
// AOP Pointcut syntax.
// ScriptFigher Project.
// Y. Lilis, August 2013.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "DDebug.h"
#include "DeltaStdDefs.h"
#include "Pointcut.h"
#include "PointcutScanner.h"
#include "PointcutSyntax.h"

#define yyFlexLexer PointcutScannerFlexLexer

#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

extern int PointcutSyntax_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer);

///////////////////////////////////////////////////////////

static void PointcutSyntax_yyerror (YYLTYPE* yylloc, Pointcut **pointcut, yyFlexLexer& lexer, const char* msg) {
	//void yyerror (DecisionMaker *dm, Statement**, const char *msg) {
	//extern char *DMSL_yytext;
	//dm->SetError(util::MakeString("Line %d : %s: before token: %s\n", DMSL_yylineno, msg, DMSL_yytext));
}

%}

%union {
	bool 			boolVal;
	double 			numberVal;
	const char*		stringVal;
	Pointcut*		pointcut;
}

%start	Pointcut
%type	<Pointcut>		Pattern ASTPattern AOPPattern Combinator
%type	<stringVal>		IdentPattern

%type	<stringVal>		OperatorMethod UsedKwdIdent
%token	<numberVal>		BOOLEAN
%token	<numberVal>		NUMBER
%token	<stringVal>		STRING
%token	<stringVal>		IDENT

%token		AST ATTRIBUTE CHILD PARENT DESCENDANT ASCENDANT
%token		EXECUTION CALL EXCEPTION CLASS SETTER GETTER
%token		FUNCTION ONEVENT METHOD DOUBLE_DOT TRIPLE_DOT
%token		AND NOT OR
%token		TRUE FALSE
%token		SELF ARGUMENTS OPERATOR NEWSELF LAMBDA_REF
%token		DOT DOT_ASSIGN CALLOP CAST
%token		ADD SUB MUL DIV MOD LT GT LE GE EQ NE ATTRIBUTEOP ATTRIBUTE_IDENT
%token		ASSIGN ADD_A SUB_A MUL_A DIV_A MOD_A  DOUBLE_LB DOUBLE_RB
%token		ADD_POSTFIX SUB_POSTFIX MUL_POSTFIX DIV_POSTFIX MOD_POSTFIX
%token		GT_POSTFIX LT_POSTFIX NE_POSTFIX EQ_POSTFIX GE_POSTFIX LE_POSTFIX
%token		'(' ')' '[' ']' ','

// Priorities
%left		OR 
%left		AND
%right		NOT

%output="PointcutSyntax.cpp"
%name-prefix="PointcutSyntax_yy"
%debug
%defines
%verbose
%pure-parser
%parse-param {Pointcut **pointcut}
%parse-param {yyFlexLexer& lexer}
%lex-param   {YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer}
%locations
%%

/**************************************************************************/
/* PROGRAM*/


Pointcut:					Pattern	{ *pointcut = $1; }
						;

Pattern:					ASTPattern { $$ = $1; }
						|	AOPPattern { $$ = $1; }
						|	Combinator { $$ = $1; }
						;

Combinator:					Pattern AND Pattern	{ $$ = Manage_AND($1, $3); }
						|	Pattern OR Pattern { $$ = Manage_OR($1, $3); }
						|	NOT Pattern { $$ = Manage_NOT($1, $3); }
						;

ASTPattern:					AST '(' STRING ')' { $$ = Manage_AST($3); }
						|	ATTRIBUTE '(' STRING ')' { $$ = Manage_ATTRIBUTE(); }
						|	CHILD '(' Pattern ')' { $$ = Manage_CHILD($3); }
						|	CHILD '(' Pattern ',' STRING ')' { $$ = Manage_CHILD($3, $5); }
						|	PARENT '(' Pattern ')' { $$ = Manage_PARENT($3); }
						|	PARENT '(' Pattern ',' STRING ')' { $$ = Manage_PARENT($3, $5); }
						|	DESCENDANT '(' Pattern ')' { $$ = Manage_DESCENDANT($3); }
						|	ASCENDANT '(' Pattern ')' { $$ = Manage_ASCENDANT($3); }
						;

AOPPattern:					EXECUTION '(' FuncPattern ')' {}
						|	CALL '(' CallPattern ')' {}
						//|	EXCEPTION '(' ExceptionPattern ')' {}
						//|	CLASS '(' ClassPattern ')' {}
						//|	SETTER '(' Field ')' {}
						//|	GETTER '(' Field ')' {}
						;

/**************************************************************************/
/* FuncPattern */

FuncPattern:				FuncClass FuncNamePattern '(' FormalsPattern ')'
						;

FuncClass:					FUNCTION	{}
						|	ONEVENT		{}
						|	METHOD		{}
						|	MUL	/*match all */ {}
						;
FuncNamePattern:			IdentPattern {}
						|	OPERATOR OperatorMethod {}
						|	ATTRIBUTEOP {}
						|	/* anonymous function */ { /*unullify($$);*/ }
						;

IdentPattern:				IDENT {}
						;

OperatorMethod:				ADD				{ $$ = "+";			}
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
						|	SUB_POSTFIX		{ $$ = "-_";			}
						|	MUL_POSTFIX		{ $$ = "*_";		}
						|	DIV_POSTFIX		{ $$ = "/_";		}
						|	MOD_POSTFIX		{ $$ = "%_";		}
						|	GT_POSTFIX		{ $$ = ">_";		}
						|	LT_POSTFIX		{ $$ = "<_";		}
						|	NE_POSTFIX		{ $$ = "!=_";		}
						|	EQ_POSTFIX		{ $$ = "==_";		}
						|	GE_POSTFIX		{ $$ = ">=_";		}
						|	LE_POSTFIX		{ $$ = "<=_";		}
						|	CALLOP			{ $$ = "()";		}
						|	CAST			{ $$ = "=()";		}
						|	DOT				{ $$ = DELTA_OPERATOR_OBJECT_GET;	}
						|	DOT_ASSIGN		{ $$ = DELTA_OPERATOR_OBJECT_SET;	}
						;

FormalsPattern:				FormalPatternList FormalArgsSuffix {}
						;

FormalPatternList:			FormalPatternNonEmptyList {}
						|	/*empty idlist*/ {}
						;

FormalPatternNonEmptyList:		FormalPatternNonEmptyList ',' FormalPattern
							|	FormalPattern
							;

FormalPattern:				IdentPattern {}
						;

FormalArgsSuffix:			/*exact arguments*/	{}
						|	DOUBLE_DOT	/*any other arguments*/ {}
						|	TRIPLE_DOT	/*variable arguments*/ {}
						;

/**************************************************************************/
/* CallPattern */
CallPattern	:				NormalCall	{}
						|	MethodCall 	{}
						;

NormalCall:					TargetFunc '(' ArgumentsPattern ')'
						;

TargetFunc:					ObjectPattern /*functor*/ {}
						|	LAMBDA_REF {}
						|	STRING {}
						// namespace :: IdentPattern
						;

MethodCall:					ObjectPattern IndexOperator IdentPattern '(' ArgumentsPattern ')'
						;

ObjectPattern:				IdentPattern {}	
						|	AttributeId {}
						|	SELF {}
						|	NEWSELF {}
						|	ARGUMENTS
						// [] ctor
						// () call
						// [] . dot index
						// [[]] .. double dot index
						;

IndexOperator:				DOT {}
						|	DOUBLE_DOT {}
						;

AttributeIdent:				IDENT {}
						|	UsedKwdIdent {}
						;

UsedKwdIdent:				FUNCTION		{ $$ = "function";	} 
						|	ONEVENT			{ $$ = "onevent";	} 
						|	METHOD			{ $$ = "method";	} 
						|	AND				{ $$ = "and";		} 
						|	OR				{ $$ = "or";		} 
						|	NOT				{ $$ = "not";		} 
						|	TRUE			{ $$ = "true";		} 
						|	FALSE			{ $$ = "false";		} 
						|	SELF			{ $$ = "self";		} 
						|	ARGUMENTS		{ $$ = "arguments"; }
						;
						
AttributeId:				ATTRIBUTE_IDENT {}
						|	ATTRIBUTEOP AttributeIdent {}
						;


ArgumentsPattern:			ArgumentPatternList ArgumentsSuffix {}
						;

ArgumentPatternList:		ArgumentPatternNonEmptyList {}
						|	/*empty arglist*/ {}
						;

ArgumentPatternNonEmptyList:	ArgumentPatternNonEmptyList ',' ArgumentPattern
							|	ArgumentPattern
							;

ArgumentPattern:			IdentPattern {}
						|	TRIPLE_DOT	{}
						//|	DYNAMIC_ARGUMENTS {}
						//TODO: all other possible arguments: vars, funcs, consts
						;

ArgumentsSuffix:			/*exact arguments*/	{ /*unullify($$);*/ }
						|	DOUBLE_DOT	/*any other arguments*/ {}
						//|	TODO: dynamic arguments
						;
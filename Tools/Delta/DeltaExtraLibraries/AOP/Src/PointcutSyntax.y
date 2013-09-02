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
#include "DeltaAOPLib.h"
#include "Pointcut.h"
#include "PointcutScanner.h"
#include "PointcutSyntax.h"
#include "PointcutParseActions.h"
#include "ASTTags.h"
#include "ustrings.h"

#define yyFlexLexer PointcutScannerFlexLexer

#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

extern int PointcutSyntax_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer);

///////////////////////////////////////////////////////////

static void PointcutSyntax_yyerror (YYLTYPE* yylloc, Pointcut **pointcut, yyFlexLexer& lexer, const char* msg) {
	const std::string error = uconstructstr("%s : before token %s", msg, lexer.YYText());
	printf("%s\n", error.c_str());
}

%}

%union {
	bool 			boolVal;
	double 			numberVal;
	const char*		stringVal;
	Pointcut*		pointcut;
	IdList*			idList;
}

%start	Pointcut
%type	<pointcut>		Pattern SubPattern ASTPattern AOPPattern Combinator FuncPattern
%type	<stringVal>		IdentPattern FuncClass FuncNamePattern FormalPattern FormalArgsSuffix
%type	<idList>		FormalsPattern FormalPatternList FormalPatternNonEmptyList
%type	<stringVal>		IndexOperator AttributeIdent AttributeId AOPKwd

%type	<stringVal>		CallPattern NormalCall MethodCall ObjectPattern

%type	<stringVal>		OperatorMethod UsedKwdIdent
%token	<boolVal>		BOOLEAN
%token	<numberVal>		NUMBER
%token	<stringVal>		STRING IDENT ATTRIBUTE_IDENT

%token		AST ATTRIBUTE CHILD PARENT DESCENDANT ASCENDANT
%token		EXECUTION CALL EXCEPTION CLASS SETTER GETTER
%token		FUNCTION ONEVENT METHOD DOUBLE_DOT TRIPLE_DOT
%token		AND NOT OR
%token		TRUE FALSE
%token		SELF ARGUMENTS OPERATOR NEWSELF LAMBDA_REF
%token		DOT DOT_ASSIGN CALLOP CAST
%token		ADD SUB MUL DIV MOD LT GT LE GE EQ NE ATTRIBUTEOP
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

Pattern:					ASTPattern		{ $$ = $1; }
						|	AOPPattern		{ $$ = $1; }
						|	Combinator		{ $$ = $1; }
						|	'(' Pattern ')' { $$ = $2; }
						;

Combinator:					Pattern AND Pattern	{ $$ = Manage_AND($1, $3); }
						|	Pattern OR Pattern { $$ = Manage_OR($1, $3); }
						|	NOT Pattern { $$ = Manage_NOT($2); }
						;

ASTPattern:					AST '(' STRING ')' { $$ = Manage_AST($3); }
						|	ATTRIBUTE '(' STRING ')' { $$ = Manage_ATTRIBUTE(); }
						|	CHILD '(' SubPattern ')' { $$ = Manage_CHILD($3); }
						|	CHILD '(' SubPattern ',' STRING ')' { $$ = Manage_CHILD($3, $5); }
						|	PARENT '(' SubPattern ')' { $$ = Manage_PARENT($3); }
						|	PARENT '(' SubPattern ',' STRING ')' { $$ = Manage_PARENT($3, $5); }
						|	DESCENDANT '(' SubPattern ')' { $$ = Manage_DESCENDANT($3); }
						|	ASCENDANT '(' SubPattern ')' { $$ = Manage_ASCENDANT($3); }
						;

SubPattern:					Pattern		{ $$ = $1; }
						|	FuncPattern { $$ = $1; }
						;

AOPPattern:					EXECUTION '(' FuncPattern ')' { $$ = $3; }
						|	CALL '(' CallPattern ')' {}
						|	EXCEPTION '(' IdentPattern ')' { $$ = Manage_EXCEPTION($3); }
						|	CLASS '(' IdentPattern ')' { $$ = Manage_CLASS($3); }
						//|	SETTER '(' Field ')' {}
						//|	GETTER '(' Field ')' {}
						;

/**************************************************************************/
/* FuncPattern */

FuncPattern:				FuncClass FuncNamePattern '(' FormalsPattern ')'
								{ $$ = Manage_FuncPattern($1, $2, $4); }
						;

FuncClass:					FUNCTION	{ $$ = AST_VALUE_FUNCCLASS_PROGRAMFUNCTION; }
						|	ONEVENT		{ $$ = AST_VALUE_FUNCCLASS_ONEVENT; }
						|	METHOD		{ $$ = AST_VALUE_FUNCCLASS_METHOD; }
						|	MUL	/*match all */ { $$ = "*"; }
						;
FuncNamePattern:			IdentPattern { $$ = $1; }
						|	OPERATOR OperatorMethod { $$ = $2; }
						|	ATTRIBUTEOP { $$ = AST_VALUE_TOSTRING_SYMBOLIC_NAME; }
						|	/* anonymous function */ { $$ = ""; }
						;

IdentPattern:				IDENT	{ $$ = $1; }
						|	MUL		{ $$ = "*"; }
						|	AOPKwd	{ $$ = $1; }
						;

AOPKwd:						AST				{ $$ = "ast";		}
						|	ATTRIBUTE		{ $$ = "attribute";	}
						|	CHILD			{ $$ = "child";		}
						|	PARENT			{ $$ = "parent";	}
						|	DESCENDANT		{ $$ = "descendant";}
						|	ASCENDANT		{ $$ = "ascendant"; }
						|	EXECUTION		{ $$ = "execution";	}
						|	CALL			{ $$ = "call";		}
						|	EXCEPTION		{ $$ = "exception";	}
						|	CLASS			{ $$ = "class";		}
						//|	SETTER			{ $$ = "setter";	}
						//|	GETTER			{ $$ = "getter";	}
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

FormalsPattern:				FormalPatternList FormalArgsSuffix { $$ = Manage_Formals($1, $2); }
						;

FormalPatternList:			FormalPatternNonEmptyList { $$ = $1; }
						|	/*empty idlist*/ { $$ = Manage_FormalsEmpty(); }
						;

FormalPatternNonEmptyList:		FormalPatternNonEmptyList ',' FormalPattern { $$ = Manage_FormalPatternList($1, $3); }
							|	FormalPattern { $$ = Manage_FormalPattern($1); }
							;

FormalPattern:				IdentPattern { $$ = $1; }
						;

FormalArgsSuffix:			/*exact arguments*/	{ unullify($$); }
						|	DOUBLE_DOT	/*any other arguments*/ { $$ = AOP_MATCH_MULTIPLE; }
						|	TRIPLE_DOT	/*variable arguments*/ { $$ = AST_VALUE_VARARGS_FORMAL_NAME; }
						;

/**************************************************************************/
/* CallPattern */
CallPattern	:				NormalCall	{ $$ = $1; }
						|	MethodCall 	{ $$ = $1; }
						;

NormalCall:					TargetFunc '(' ArgumentsPattern ')'
								{ /*$$ = Manage_NormalCall($1, $3);*/ }
						;

TargetFunc:					ObjectPattern /*functor*/ {}
						|	LAMBDA_REF {}
						|	STRING {}
						// namespace :: IdentPattern
						;

MethodCall:					ObjectPattern IndexOperator IdentPattern '(' ArgumentsPattern ')'
								{ /*$$ = Manage_MethodCall($1, $2, $3, $5);*/ }
						;

ObjectPattern:				IdentPattern { $$ = $1; }	
						|	AttributeId { $$ = $1; }
						|	SELF { $$ = AST_TAG_SELF; }
						|	NEWSELF { $$ = AST_TAG_LVALUE_NEWSELF; }
						|	ARGUMENTS { $$ = AST_TAG_ARGUMENTS; }
						// [] ctor
						// () call
						// [] . dot index
						// [[]] .. double dot index
						;

IndexOperator:				DOT { $$ = "."; }
						|	DOUBLE_DOT { $$ = ".."; }
						;

AttributeIdent:				IDENT { $$ = $1; }
						|	UsedKwdIdent { $$ = $1; }
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
						
AttributeId:				ATTRIBUTE_IDENT { $$ = $1 + 1; }
						|	ATTRIBUTEOP AttributeIdent { $$ = $2; }
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
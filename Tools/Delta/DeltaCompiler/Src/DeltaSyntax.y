%{
// Delta syntax directed parsing for AST generation.
// Extended December 2009, added lambda functions.
// ScriptFigher Project.
// A. Savidis, September 2009.
//

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "DDebug.h"
#include "DeltaStdDefs.h"
#include "ASTNode.h"
#include "ASTTags.h"
#include "ASTCreationActions.h"
#include "CompilerAPI.h"
#include "DescriptiveParseErrorHandler.h"
#include "LibraryNamespace.h"

using namespace AST;

#ifndef	alloca
#define alloca malloc
#endif

#ifdef	YYPURE
#define	YYLEX_PARAM		&yylval
#else
#define	YYPURE
#endif

#define	yyparse		DeltaSyntax_yyparse
#define yylex		DeltaSyntax_yylex
#define	yydebug		DeltaSyntax_yydebug
#define	yyerror		DeltaSyntax_yyerror

extern int			DeltaSyntax_yylex (void* yylval);

static void DeltaSyntax_yyerror (const char* unused) 
	{ DescriptiveParseErrorHandler::HandleSyntaxError(); }

#define	DYNAMIC_STRING(s) \
	MakeNode_StringWithLateDestruction(ucopystr(s))

%}

%union {
	double				numberConst;
	util_ui32			line;
	char*				dynamicStr;
	const char*			constStr;
	AST::IdList*		idList;
	AST::NodeList*		nodeList;
	AST::Node*			node;
}

%start	DeltaCode
%type	<node>			ConstValue Lvalue TableContent Expression TablePrefix  ExceptionVar
%type	<node>			Primary FunctionCall TableConstructor Term TableContentDot TableContentBracket
%type	<node>			FunctionCallObject FunctionAndTableObject  ActualArgument BasicExprStmt
%type	<node>			TableContentBoundedDot TableContentBoundedBracket TableObject BasicNonExprStmt
%type	<node>			FunctionName Function IndexedValues TableElement UnindexedValue
%type	<node>			ArithmeticExpression AssignExpression BooleanExpression RelationalExpression
%type	<node>			Condition ForCondition DottedIdent  IndexContent IndexExpression 
%type	<node>			TernaryExpression TernaryCondition TernarySelection1 TernarySelection2
%type	<node>			NewAttribute AttributeSet AttributeGet ConstDefExpression ForeachCont ForeachValue 
%type	<node>			Stmt IfStmt Compound  ExceptionStmt ThrowStmt ForStmt WhileStmt ForSuffix ForInitList
%type	<node>			AssertStmt Assertion BreakStmt ContinueStmt ReturnStmt ExpressionListStmt
%type	<node>			ForeachStmt OperatorIndex Stmts StringConstUsed FunctionElement OptionalStmts
%type	<node>			UsingNamespace UsingByteCodeLibrary UsingSpecifications UsingDirective
%type	<node>			StringConst StringifyDottedIdents StringifyNamespaceIdent DotIndex SpecialDotIndex BracketIndex
%type	<node>			LambdaFunction LambdaCode LambdaStmt ContentExpression IdentIndexElement
%type	<idList>		IdentList NamespacePath FormalArgs
%type	<nodeList>		ExpressionList TableElements NonEmptyActualArgumentsList ContentList
%type	<nodeList>		ActualArgumentsList ActualArguments IndexedList CodeDefs UsingDirectives
%type	<constStr>		IDENT ATTRIBUTE_IDENT  FunctionClass FormalArgsSuffix FuncLinkage
%type	<constStr>		StringIdent OperatorMethod OpString DottedOpString KwdIdent AttributeIdent
%type	<dynamicStr>	ConstId AttributeId IdentIndex
%type	<line>			LN FunctionNameSuffix
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

%%

/**************************************************************************/
/* PROGRAM*/


DeltaCode:				CodeDefs	{ SetSyntaxTree(MakeNode_Program($1)); }
						;

CodeDefs:					/* Empty code allowed. */		{ $$ = NIL_NODELIST; }
						|	Stmts							{ $$ = MakeNode_CodeDefs(NIL_NODELIST, $1); }
						|	UsingDirectives OptionalStmts	{ $$ = MakeNode_CodeDefs($1, $2); }
							;

OptionalStmts:				Stmts							{ $$ = $1; }
						|									{ $$ = NIL_NODE; }
						;
						
UsingDirectives:			UsingDirectives UsingDirective	{ $$ = MakeNode_CodeDefs($1, $2); }
						|	UsingDirective					{ $$ = MakeNode_CodeDefs(NIL_NODELIST, $1); }
						;
										
Stmts:						Stmts Stmt			{ $$ = MakeNode_Stmts($1, $2);	}
						|	Stmt				{ $$ = MakeNode_Stmts(NIL_NODE, $1); }
						;
						
LN:						{ $$ = DeltaCompiler::GetLine(); }	
						;

/**************************************************************************/
/* FUNCTIONS, METHODS, LAMBDA FUNCTIONS */

Function:				FunctionClass 
							{ SM(S_FUNC_($1)); PE2(T_FUNC_($1), T_LOCAL_OR_FUNCNAME); }
						FuncLinkage FunctionName FormalArgs 
							{ SG(S_BLOCK); }
						Compound
							{ EG(S_BLOCK); EM(S_FUNC_($1)); $$ = MakeNode_Function($1, $4, $3, $5, $7); }
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

FunctionNameSuffix:		LN
							{ PE(T_LPAR); SG(S_FARGS); $$ = $1; }
						;
						
FunctionName:				IDENT FunctionNameSuffix						
								{ $$ = MakeNode_FunctionName($1); DPTR($$)->SetLine($2); }
						|	OPERATOR OperatorMethod	FunctionNameSuffix
								{ $$ = MakeNode_FunctionName($2); DPTR($$)->SetLine($3); }
						|	ATTRIBUTE FunctionNameSuffix
								{ $$ = MakeNode_FunctionName(AST_VALUE_TOSTRING_SYMBOLIC_NAME); DPTR($$)->SetLine($2); }
						|	/* anonymous function */	
							FunctionNameSuffix
								{ unullify($$); }
						;

OperatorMethod:				OpString		{ $$ = $1;							}
						|	DOT				{ $$ = DELTA_OPERATOR_OBJECT_GET;	}
						|	DOT_ASSIGN		{ $$ = DELTA_OPERATOR_OBJECT_SET;	}
						;
			
IdentList:					IdentList ',' 
								{ PE2(T_COMMA, T_IDENT); } 
							IDENT
								{ $$ = MakeNode_IdentList($1, $4); }
						|	IDENT
								{ PE(T_IDENT); $$ = MakeNode_IdentList(NIL_IDLIST, $1); }
						|	/* no formal arguments */
								{ unullify($$); }
						;

FormalArgsSuffix:			')'	
								{ EG(S_FARGS); PE(T_RPAR); unullify($$); }
						|	TRIPLE_DOT
								{ EG(S_FARGS); PE2(T_TDOT, T_RPAR); }
							')'
								{  $$ = AST_VALUE_VARARGS_FORMAL_NAME; }
						;
									
FormalArgs:					CALL	
								{ EG(S_FARGS); PE(T_RPAR); unullify($$); }
						|	'('	IdentList 
								{ OE(T_LPAR_OR_TDOT); }
							FormalArgsSuffix	
								{ $$ = MakeNode_IdentList($2, $4); }
						|	/* empty, arg signature can be skipped */
								{ EG(S_FARGS); PE(T_RPAR); unullify($$); }
						;

CompoundOpening:		'{'	 { PE(T_LBC); } 
						;
						
Compound :					CompoundOpening Stmts
								{ PE(T_RBC); } 
							'}'
								{  $$ = MakeNode_Compound($2); }
							
						|	CompoundOpening '}'
								{ PE(T_RBC); $$ = MakeNode_Compound(); }
						;

LambdaFunction:			LambdaPrefix FormalArgs
							{ PE(T_LBC); }
						LambdaCode
							{ EM(S_FUNC_(AST_VALUE_FUNCCLASS_PROGRAMFUNCTION)); $$ = MakeNode_LambdaFunction($2, $4); }
						;
						
LambdaPrefix:			LAMBDA
							{	SM(S_FUNC_(AST_VALUE_FUNCCLASS_PROGRAMFUNCTION)); 
								PE2(T_LAMBDA, T_LPAR); 
								SG(S_FARGS); }
						;

LambdaCode:				'{' 
							{ PE(S_EXPR); }
						LambdaStmt '}'
							{ $$ = $3; }
						;
						
LambdaStmt:				LN Expression
							{ PE(T_RBC); $$ = $2; $$->SetLine($1); }
						;	
								
/**************************************************************************/
/* STMTS */

Semi:						{ PE(T_SEMI); }	
						SEMI		
						;
	
ExpressionListStmt:			{ SM(S_STMT); }	
						ExpressionList Semi 
							{ $$ =	MakeNode_ExpressionListStmt($2); } 
						;

Stmt:						LN BasicExprStmt		{ $$ = MakeNode_BasicExprStmt($2);		$$->SetLine($1); } 
						|	LN BasicNonExprStmt		{ $$ = MakeNode_BasicNonExprStmt($2);	$$->SetLine($1); } 
						|	Compound				{ $$ = $1; }
						|	Function				{ $$ = MakeNode_FunctionStmt($1); }
						|	SEMI					{ $$ = MakeNode_EmptyStmt();  }
						;
		
BasicExprStmt:				ExpressionListStmt	{ EM(S_STMT); $$ = $1; }
						|	AssertStmt			{ EM(S_STMT); $$ = $1; }
						|	ReturnStmt			{ $$ = $1;}
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
						Expression
							{ EG(S_EXPR); PE(S_EXPR); EM(S_ASSRT); $$ = MakeNode_AssertStmt($3); }

AssertStmt:					{ SM(S_STMT); PE(S_ASSRT);	}	
						Assertion Semi	
							{ $$ = $2; }	
						;

/* RETURN*******************/

ReturnPrefix:			RETURN 
							{ SM(S_RET); }	
						;

ReturnStmt:					ReturnPrefix
								{ PE2(T_RET, S_EXPR); } 
							Expression Semi
								{ EM(S_RET); $$ = MakeNode_Return($3); }
						|	ReturnPrefix Semi
								{ EM(S_RET); $$ = MakeNode_Return(); }
						;

/* USING********************/

UsingDirective:			USING UsingSpecifications
							{ $$ = $2; }
						;

UsingSpecifications:		UsingNamespace			{ $$ = $1; }
						|	UsingByteCodeLibrary	{ $$ = $1; }
						;
				
UsingNamespace:				NamespacePath IDENT SEMI
								{ $$ = MakeNode_UsingNamespace($1, $2); }
						|	IDENT SEMI
								{ $$ = MakeNode_UsingNamespace($1); }
						;

UsingByteCodeLibrary:		
						STRINGIFY IDENT SEMI LN
							{ $$ = MakeNode_UsingByteCodeLibrary($2); DPTR($$)->SetLine($4); }
						;

/**************************************************************************/
/* EXPRESSIONS */

Expression:					AssignExpression		{ $$ = $1; }
						|	ConstDefExpression		{ $$ = $1; }
						|	RelationalExpression	{ $$ = $1; }
						|	BooleanExpression		{ $$ = $1; }
						|	ArithmeticExpression	{ $$ = $1; }
						|	TernaryExpression		{ $$ = $1; }
						|	Term					{ $$ = $1; }
						;
							
ConstId:				IDENT
							{ $$ = DYNAMIC_STRING($1); }
						;

ConstDefExpression:		CONST		{ SM(S_CONST); PE(T_IDENT); } 
						ConstId		{ PE(T_ASSIGN); }
						ASSIGN		{ PE(S_EXPR); } 
						Expression	{ EM(S_CONST); $$ = MakeNode_ConstDefExpression($3, $7); }
						;

AssignExpression:			Lvalue ASSIGN	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ASSIGN, S_EXPR); }	
							Expression
								{ EM(S_ASSIGNEXPR);  $$ = MakeNode_AssignExpr($1, $4); }
									
						|	Lvalue ADD_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ADDA, S_EXPR); }		
							Expression
								{ EM(S_ASSIGNEXPR); $$ = MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_ADD_ASSIGN); }
									
						|	Lvalue MUL_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MULA, S_EXPR); }		
							Expression
								{ EM(S_ASSIGNEXPR); $$ = MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_MUL_ASSIGN); }
									
						|	Lvalue SUB_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_SUBA, S_EXPR); }		
							Expression
								{ EM(S_ASSIGNEXPR); $$ = MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_SUB_ASSIGN); }
									
						|	Lvalue DIV_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_DIVA, S_EXPR); }		
							Expression
								{ EM(S_ASSIGNEXPR); $$ = MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_DIV_ASSIGN); }
								
						|	Lvalue MOD_A	
								{ SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MODA, S_EXPR); }	
							Expression
								{ EM(S_ASSIGNEXPR); $$ = MakeNode_AssignArithExpr($1, $4, AST_TAG_OP_MOD_ASSIGN); }
						;

RelationalExpression:		Expression GT 
								{ SM(S_RELAT); PE3(S_EXPR, T_GT, S_EXPR); } 
							Expression
								{ EM(S_RELAT); $$ = MakeNode_RelationalExpr($1, $4, AST_TAG_OP_GT); }
								
						|	Expression LT 
								{ SM(S_RELAT); PE3(S_EXPR, T_LT, S_EXPR); } 
							Expression
								{ EM(S_RELAT); $$ = MakeNode_RelationalExpr($1, $4, AST_TAG_OP_LT); }
								
						|	Expression GE 
								{ SM(S_RELAT); PE3(S_EXPR, T_GE, S_EXPR); } 
							Expression
								{ EM(S_RELAT); $$ = MakeNode_RelationalExpr($1, $4,AST_TAG_OP_GE); }
								
						|	Expression LE 
								{ SM(S_RELAT); PE3(S_EXPR, T_LE, S_EXPR); } 
							Expression
								{ EM(S_RELAT); $$ = MakeNode_RelationalExpr($1, $4, AST_TAG_OP_LE); }
								
						|	Expression EQ 
								{ SM(S_RELAT); PE3(S_EXPR, T_EQ, S_EXPR); } 
							Expression
								{ EM(S_RELAT); $$ = MakeNode_RelationalExpr($1, $4, AST_TAG_OP_EQ); }
								
						|	Expression NE 
								{ SM(S_RELAT); PE3(S_EXPR, T_NE, S_EXPR); } 
							Expression
								{ EM(S_RELAT); $$ = MakeNode_RelationalExpr($1, $4, AST_TAG_OP_NE); }
						;

BooleanExpression:			Expression AND
								{ SM(S_LOGICAL); PE3(S_EXPR, T_AND, S_EXPR); }
							Expression
								{ EM(S_LOGICAL); $$ = MakeNode_ExprANDExpr($1, $4); }
								
						|	Expression OR
								{ SM(S_LOGICAL); PE3(S_EXPR, T_OR, S_EXPR); }
							Expression
								{ EM(S_LOGICAL); $$ = MakeNode_ExprORExpr($1, $4); }
						;
							
ArithmeticExpression:		Expression ADD 
								{ SM(S_ARITH); PE3(S_EXPR, T_ADD, S_EXPR); } 
							Expression
								{ EM(S_ARITH); $$ = MakeNode_ArithmeticExpression($1, $4, AST_TAG_OP_ADD); }
								
						|	Expression SUB 
								{ SM(S_ARITH); PE3(S_EXPR, T_SUB, S_EXPR); } 
							Expression
								{ EM(S_ARITH); $$ = MakeNode_ArithmeticExpression($1, $4, AST_TAG_OP_SUB); }
								
						|	Expression DIV 
								{ SM(S_ARITH); PE3(S_EXPR, T_DIV, S_EXPR); } 
							Expression
								{ EM(S_ARITH); $$ = MakeNode_ArithmeticExpression($1, $4,AST_TAG_OP_DIV); }
								
						|	Expression MUL 
								{ SM(S_ARITH); PE3(S_EXPR, T_MUL, S_EXPR); } 
							Expression
								{ EM(S_ARITH); $$ = MakeNode_ArithmeticExpression($1,  $4, AST_TAG_OP_MUL); }
								
						|	Expression MOD 
								{ SM(S_ARITH); PE3(S_EXPR, T_MOD, S_EXPR); } 
							Expression
								{ EM(S_ARITH); $$ = MakeNode_ArithmeticExpression($1, $4, AST_TAG_OP_MOD); }
						;

TernaryExpression:		'(' TernaryCondition TernarySelection1 ':' TernarySelection2 ')' %prec ASSIGN
							{	EM(S_TERNARY); $$ = MakeNode_Ternary($2, $3, $5); }
						;

TernaryCondition:		Expression '?'
							{ SM(S_TERNARY); PE3(S_EXPR, T_QMARK, S_EXPR); $$ = $1; }
						;

TernarySelection1:		Expression
							{ PE(T_COLON); $$ = $1; }
						;
						
TernarySelection2:			{ PE(S_EXPR); } 
						Expression
							{ PE(T_RPAR); $$ = $2; }
						;
					
Term:						Lvalue PLUSPLUS
								{ $$ = MakeNode_TermLvalueArith($1, AST_LVALUE_PLUSPLUS); }
								
						|	PLUSPLUS 
								{ SM(S_EXPR); PE2(T_PLUSPLUS, S_LVAL); } 
							Lvalue
								{ EM(S_EXPR); $$ = MakeNode_TermLvalueArith($3, AST_PLUSPLUS_LVALUE); }
								
						|	Lvalue MINUSMINUS
								{ $$ = MakeNode_TermLvalueArith($1, AST_LVALUE_MINUSMINUS); }
								
						|	MINUSMINUS 
								{ SM(S_EXPR); PE2(T_MINUSMINUS, S_LVAL); } 
							Lvalue
								{ EM(S_EXPR); $$ = MakeNode_TermLvalueArith($3, AST_MINUSMINUS_LVALUE); }
								
						|	SUB 
								{ SM(S_EXPR); PE2(T_SUB, S_EXPR); } 
							Expression %prec UMINUS
								{ EM(S_EXPR); $$ = MakeNode_UMINUSExpression($3); }
								
						|	NOT 
								{ SM(S_EXPR); PE2(T_NOT, S_EXPR); } 
							Expression
								{ EM(S_EXPR); $$ = MakeNode_NOTExpression($3); }
						
						|	Primary				
								{ $$ = $1; }
						;

Primary:					FunctionAndTableObject	{ $$ = MakeNode_PrimaryFunctionAndTableObject($1);	}
						|	ConstValue				{ $$ = MakeNode_PrimaryConstValue($1);				}
						|	LambdaFunction			{ $$ = MakeNode_PrimaryLambdaFunction($1);			}						
						;

ConstValue:					NUMBER_CONST	{ $$ = MakeNode_ConstValue($1);		}
						|	NIL				{ $$ = MakeNode_ConstValue();		}
						|	TRUE			{ $$ = MakeNode_ConstValue(true);	}
						|	FALSE			{ $$ = MakeNode_ConstValue(false);	}
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
								{ $$ = MakeNode_StringConst($1, $2); }
						|	STRING_CONST
								{ $$ = MakeNode_StringConst($1); }
						;

Stringify:					STRINGIFY 
								{ EI(S_EXPR); PE(T_STRINGIFY); }
						;
						
StringIdent:				IDENT		{ $$ = $1; }
						|	KwdIdent	{ $$ = $1; }
						;

StringifyDottedIdents:		Stringify StringIdent
								{ PE(T_IDENT); $$ = MakeNode_StringifyDottedIdents($2); }
						|	StringifyDottedIdents DOT 
								{ PE2(T_DOT, T_IDENT); }
							StringIdent
								{ $$ = MakeNode_StringifyDottedIdents($1, $4); }
						;
						
StringifyNamespaceIdent:	Stringify NamespacePath IDENT
								{ EG(S_NAMESPACE); PE(T_IDENT); $$ = MakeNode_StringifyNamespaceIdent($2, $3); }
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
				
FunctionAndTableObject:		SELF 				
								{ EI(S_EXPR); PE(T_SELF); $$ = MakeNode_SELF(); }
						|	LAMBDA_REF
								{ EI(S_EXPR); PE(T_LAMBDA_REF); $$ = MakeNode_LAMBDA_REF(); }								
						|	NEWSELF
								{ EI(S_EXPR); PE(T_NEWSELF); $$ = MakeNode_NEWSELF(); }
						|	StringConstUsed		
								{ EI(S_EXPR); $$ = $1; }
						|	ARGUMENTS 			
								{ EI(S_EXPR); PE(T_ARGUMENTS); $$ = MakeNode_ARGUMENTS(); }
						|	Lvalue				
								{ $$ = $1; }
						|	TableConstructor	
								{ PE(S_TABLE); $$ = $1; }
						|	'(' Expression		
								{ PE3(T_LPAR, S_EXPR, T_RPAR); } 
							')'					
								{ $$ = MakeNode_ParenthesisedExpr($2); }
						|	FunctionCall		
								{ PE(S_CALL); $$ = $1; }
						|	'(' Function		
								{ SM(S_EXPR); PE3(T_LPAR, S_FUNC, T_RPAR); }  
							')'					
								{ EM(S_EXPR); $$ = MakeNode_FunctionParenthesisForm($2); }
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
								{ $$ = MakeNode_ExpressionList($1, $4); }
						|	ActualArgument						
								{ $$ = MakeNode_ExpressionList(NIL_NODELIST, $1); }
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
								{ $$ = MakeNode_Argument($2); }						
						|	'|'			
								{ PE2(T_BAR, S_EXPR); } 
							Expression	
								{ PE(T_BAR); } 
							'|'			
								{ $$ = MakeNode_LateBoundArgument($3); }
						|	TRIPLE_DOT	
								{ PE(T_TDOT); $$ = MakeNode_TRIPLE_DOT(); }
						|	Function
								{ $$ = MakeNode_FunctionExpression($1); }
						;
				
/*FUNCTION CALL EXPRESSION***/

FunctionCall:			FunctionCallObject ActualArguments
							{ $$ = MakeNode_FunctionCall($1, $2); }
						;

ExpressionList:				ExpressionList ',' LN Expression 
								{ $$ = MakeNode_ExpressionList($1, $4); DPTR($4)->SetLine($3); }
						|	LN Expression 
								{ $$ = MakeNode_ExpressionList(NIL_NODELIST, $2); DPTR($2)->SetLine($1); }
						;

/**************************************************************************/
/* LVALUES */

				
NamespacePath:				GLOBAL_SCOPE
								{ EI(S_EXPR); SG(S_NAMESPACE); PE2(T_GLOBAL, T_IDENT); 
								  $$ = MakeNode_NamespacePath(NIL_IDLIST, DELTA_LIBRARYNAMESPACE_SEPARATOR); }
						|	IDENT GLOBAL_SCOPE
								{ EI(S_EXPR); SG(S_NAMESPACE); PE3(T_IDENT, T_GLOBAL, T_IDENT); 
								  $$ = MakeNode_NamespacePath(NIL_IDLIST, $1); }
						|	NamespacePath IDENT GLOBAL_SCOPE
								{ PE2(T_GLOBAL, T_IDENT); $$ = MakeNode_NamespacePath($1, $2); }
						;
		
Lvalue:						IDENT
								{ EI(S_EXPR); PE(T_IDENT); $$ = MakeNode_Lvalue($1); }
								  
						|	NamespacePath IDENT
								{ EG(S_NAMESPACE); $$ = MakeNode_NamespaceLvalue($1, $2); }
								  
						|	AttributeId
								{ $$ = MakeNode_AttrLvalue($1); }
								
						|	STATIC 
								{ EI(S_EXPR); PE2(T_STATIC, T_IDENT); } 
							IDENT
								{ $$ = MakeNode_StaticLvalue($3); }
								
						|	LOCAL 
								{ EI(S_EXPR); PE2(T_LOCAL, T_IDENT); } 
							IDENT
								{ $$ = MakeNode_LocalLvalue($3); }
																					
						|	TableContent
								{ $$ = $1; }
						;

TableObject:			FunctionAndTableObject 
							{ $$ = MakeNode_TableObject($1); }	
						;
						
FunctionCallObject:		FunctionAndTableObject 
							{ $$ = $1; }		
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
								{ $$ = MakeNode_DotIndexIdent($1); }
						|	KwdIdent	
								{ $$ = MakeNode_DotIndexIdent($1); }
						|	OpString
								{ $$ = MakeNode_DotIndexOpString($1); }
						|	StringConstUsed
								{ $$ = MakeNode_DotIndexStringConst($1); }
						;

SpecialDotIndex:		DottedOpString
							{ $$ = MakeNode_DotIndexOpString($1+1); }	
						;
						
BracketIndex:				Expression
								{ $$ = $1; }
						|	OperatorMethod
								{ $$ = MakeNode_BracketIndexOperatorMethod($1); }
						;
												
/*TABLECONTENT**************/

TableContent:				TableContentDot DotIndex
								{	PE(S_DOTINDEX_(DPTR($2)->GetTag())); 
									$$ = MakeNode_TableContentDot($1, $2); }
						|
							TableObject SpecialDotIndex
								{	PE(S_DOTINDEX_(DPTR($2)->GetTag())); 
									$$ = MakeNode_TableContentDot($1, $2); }
											
						|	TableContentBoundedDot DotIndex
								{	PE(S_DOTINDEX_(DPTR($2)->GetTag())); 
									$$ = MakeNode_BoundedTableContentDot($1, $2); }
																
						|	TableContentBracket BracketIndex 
								{ PE2(S_BRACKET_INDEX_(DPTR($2)->GetTag()), T_RB); } 
							']'	%prec SQUARE_BRACKETS
								{ $$ = MakeNode_TableContentBracket($1, $2); }
																
						|	TableContentBoundedBracket BracketIndex 
								{ PE2(S_BRACKET_INDEX_(DPTR($2)->GetTag()), T_DRB); } 
							DOUBLE_RB %prec SQUARE_BRACKETS
								{ $$ = MakeNode_BoundedTableContentBracket($1, $2); }
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
								{ $$ = MakeNode_TableConstructor($2); }
									
						|	TablePrefix TableSuffix
								{ $$ = MakeNode_TableConstructor(); }
						;

TableElements:				TableElements ',' 
								{ PE(T_COMMA); } 
							TableElement
								{ $$ = MakeNode_TableElements($1, $4); }
						|	TableElement
								{ $$ = MakeNode_TableElements(NIL_NODELIST, $1); }
						;

UnindexedValue:				Expression
								{ $$ = MakeNode_UnindexedValue($1); }
						|	FunctionElement
								{ $$ = MakeNode_UnindexedValue($1); }
						;
						
FunctionElement:			Function
								{ PE(S_FUNC_(GetFuncClass($1))); $$ = MakeNode_FunctionExpression($1); }
						;

PE_elem:				{ OE(T_TABLE_ELEM); }						
						;
						
TableElement:				PE_elem UnindexedValue LN
								{ $$ = $2; DPTR($$)->SetLine($3); }
						|	PE_elem IndexedValues
								{ $$ = $2; }
						|   PE_elem NewAttribute
								{ $$ = $2; }
						|	PE_elem IdentIndexElement
								{ $$ = $2; }
						;
									
DottedIdent:			DOT 
							{ PE2(T_DOT, T_IDENT); } 
						StringIdent 
							{ $$ = MakeNode_DottedIdent($3); }
						;

OperatorIndex:			OpString 
							{ PE(T_OPINDEX); $$ = MakeNode_OperatorIndex($1); }
						;

IndexContent:				Expression		{ $$ = $1; }
						|	DottedIdent		{ $$ = $1; }
						|	OperatorIndex	{ $$ = $1; }
						;

IndexExpression:			{ PE(S_INDEXEXPR); } 
						IndexContent 
							{ $$ = $2; }
						;

IndexedList:				IndexedList 
								{ PE(T_COMMA); } 
							',' IndexExpression LN
								{ $$ = MakeNode_ExpressionList($1, $4); DPTR($4)->SetLine($5); }
						|	IndexExpression LN
								{ $$ = MakeNode_ExpressionList(NIL_NODELIST, $1); DPTR($1)->SetLine($2); }
						;

IndexedValues:			'{' 
							{ PE(T_LBC); SG(N_EMPTY); } 
						IndexedList 
							{ EG(N_EMPTY); PE2(S_INDICES, T_COLON); } 
						':' 
							{ SG(S_ELIST); } 
						ContentList 
							{ EG(S_ELIST); PE2(S_ELIST, T_RBC); } 
						'}'
							{ $$ = MakeNode_IndexedValues($3, $7); }
						;

IdentIndex:					AttributeId	':'
								{ $$ = $1; }
						|	DOT 
								{ PE(T_DOT); OE(T_IDENT_OR_KWDIDENT); } 
							AttributeIdent ':'
								{ $$ = DYNAMIC_STRING($3); PE(T_COLON); }
						;

IdentIndexElement:		IdentIndex
							{ SG(S_EXPR); }
						ContentExpression LN
							{ EG(S_EXPR); PE(S_EXPR); $$ = MakeNode_IdentIndexElement($1, $3); DPTR($$)->SetLine($4); }
						;

ContentList:				ExpressionList ',' LN ContentExpression 
								{ $$ = MakeNode_ExpressionList($1, $4); DPTR($4)->SetLine($3); }
						|	LN ContentExpression 
								{ $$ = MakeNode_ExpressionList(NIL_NODELIST, $2); DPTR($2)->SetLine($1); }
						;

ContentExpression:			Expression		{ $$ = $1; }
						|	Function		{ PE(S_FUNC_(GetFuncClass($1))); $$ = MakeNode_FunctionExpression($1); }
						;
					
/**************************************************************************/
/* ATTRIBUTES */

AttributeIdent:				IDENT			{ PE(T_IDENT);		$$ = $1; }
						|	KwdIdent		{ PE(T_KWDIDENT);	$$ = $1; }
						;
						
AttributeId:				ATTRIBUTE_IDENT 
								{ PE(T_ATTRIBUTEID); OE(T_LBC_OR_COLON); $$ = DYNAMIC_STRING($1 + 1); }
						|	ATTRIBUTE 
								{ PE(T_ATTRIBUTE); OE(T_IDENT_OR_KWDIDENT); }
							AttributeIdent
								{ $$ = DYNAMIC_STRING($3); }
						;

				
NewAttribute:			AttributeId AttributeSet AttributeGet
							{ $$ = MakeNode_NewAttribute($1, $2, $3); }
						;
						
AttributeSet:			'{'	
							{ PE(T_SET);  }
						SET 				
							{ SG(S_EXPR); } 
						ContentExpression LN
							{ EG(S_EXPR); PE2(S_EXPR, T_GET); $$ = $5; DPTR($$)->SetLine($6); }
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
							{ $$ = MakeNode_BREAK(); }
						;
						
ContinueStmt:				{ SM(S_STMT); PE(T_CONT);	}	
						CONTINUE Semi	
							{ $$ = MakeNode_CONTINUE(); }
						;

Condition:				'('				{ SG(S_EXPR); } 
						LN Expression	{ EG(S_EXPR); PE2(S_EXPR, T_RPAR); } 
						')'				{ $$ = $4; DPTR($$)->SetLine($3); }	
						;

/* IF************************/

IfPrefix:				IF		{ SM(S_IF); PE2(T_IF, T_LPAR);  }		;
ElsePrefix:				ELSE	{ EM(S_IF); SM(S_ELSE); PE(T_ELSE); }	;
		
IfStmt:						IfPrefix Condition Stmt
								{ EM(S_IF);   $$ = MakeNode_IfStmt($2, $3); }
						|	IfPrefix Condition Stmt ElsePrefix Stmt
								{ EM(S_ELSE); $$ = MakeNode_IfElseStmt($2, $3, $5); }
						;

/* WHILE*********************/

WhilePrefix:			WHILE	
							{ SM(S_WHILE); PE2(T_WHILE, T_LPAR); }	
						;

WhileStmt:				WhilePrefix Condition LN Stmt
							{ EM(S_WHILE); $$ = MakeNode_WhileStmt($2, $4); DPTR($4)->SetLine($3); }
						;

/* FOR***********************/

ForStmt:				ForPrefix ForInitList ForCondition ForSuffix Stmt
							{ EM(S_FOR); $$ = MakeNode_ForStmt($2, $3, $4, $5); }
						;

ForPrefix:				FOR		
							{ SM(S_FOR); PE2(T_FOR, T_LPAR); } 
						'('		
							{ PE(S_ELIST); } 
						;
						
ForInitList	:				LN ExpressionList Semi
								{ $$ = MakeNode_ForInit($2); DPTR($$)->SetLine($1); }
						|	LN Semi
								{ EI(S_ELIST); $$ = MakeNode_ForInit(NIL_NODELIST); DPTR($$)->SetLine($1); }
						;

ForCondition:				{ PE(S_EXPR); }		
						 Expression LN Semi
							{ $$ = $2; DPTR($$)->SetLine($3); }
						;
			
ForSuffix:						{ PE(S_ELIST); } 
							LN ExpressionList ForEnd
								{ $$ = MakeNode_ForSuffix($3); DPTR($$)->SetLine($2); }
						|	LN ForEnd
								{ $$ = MakeNode_ForSuffix(NIL_NODELIST); DPTR($$)->SetLine($1); }
						;

ForEnd:							{ PE(T_RPAR); } 
							')'	
						;

/* FOREACH*******************/

ForeachCont:			LN Expression	{ $$ = $2; DPTR($$)->SetLine($1); }
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
												$$ = MakeNode_ForeachStmt($7, $5, $9, $14); 
											else
												$$ = MakeNode_ForeachStmt($5, NIL_NODE, $9, $14); 
											DPTR($$)->SetLine($13);
											EM(S_FOREACH);	}
						;

/**************************************************************************/
/* EXCEPTION HANDLING */

ThrowStmt:				THROW 
							{ SM(S_THROW); PE2(T_THROW, S_EXPR); } 
						Expression Semi
							{ EM(S_THROW); $$ = MakeNode_THROW($3); }
						;

ExceptionVar:			IDENT	{ PE(S_STMT); $$ = MakeNode_ExceptionVar($1); }
						;
						
ExceptionStmt:			TRY		{ SM(S_TRY); PE2(T_TRY, S_STMT); }
						Stmt	{ PE(T_TRAP); }
						TRAP	{ PE(T_IDENT); }
						ExceptionVar
						Stmt	{ EM(S_TRY); $$ = MakeNode_Exception($3, $7, $8); } 
						;			
				
/**************************************************************************/

%%
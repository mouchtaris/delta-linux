%{
/**
 *	DeltaParserSpec.y
 *
 * Delta parser, syntax directed parsing file, using Bison parser generator.
 * Based on Anthony Savidis' (as@ics.forth.gr) Delta Parser
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 *  Changes, addition and extensions by JL and latter by AS.
 *  Extension to support selective parsing of partial code segments, 
 *  Anthony Savidis, July 2010.
 *  Added metaprogramming support, Yannis Lilis, August 2011.
 */

#include "Common.h"
#include "Streams.h"
#include "DeltaParserCommon.h"
#include "ProgramDescription.h"
#include "DeltaAST.h"
#include "DeltaASTUtil.h"
#include "DeltaParserGen.hpp"

#include "FlexLexer.h"

////////////////////////////////////////////////////////////////////////

using namespace delta;

extern int yylex (YYSTYPE* yylval, YYLTYPE* yylloc, ParsingCtx& ctx);

////////////////////////////////////////////////////////////////////////

void yyerror (YYLTYPE* yylloc, ParsingCtx& ctx, char* msg)
{
	DBGOUT << msg << " at line: " << yylloc->first_line << ", at pos: (" << yylloc->first_column <<
		"), before: \"" << ctx.GetLexer().YYText() << "\"" << DBGENDL;

	const char* unexpectedEndMsg = msg + strlen(msg) - 4;
	if (!strcmp(unexpectedEndMsg, "$end"))
		return;

	ctx.GetProgramDesc().AddParseError(
			Range(yylloc->first_column, yylloc->last_column), util::std2str(msg)
		);
}

////////////////////////////////////////////////////////////////////////

DeltaASTNode* ExpressionToConstKey (YYLTYPE& pos, DeltaASTNode* node)
{
	if (node) {
		if (node->GetType() == ConstASTNode::Type) {
			ConstASTNode* constNode = static_cast<ConstASTNode*>(node);
			TableConstKeyType type;
			if (constNode->GetValue() == CONST_NUMBER)
				type = CONST_KEY_NUMBER;
			else if (constNode->GetValue() == CONST_STRING)
				type = CONST_KEY_STRING;
			else
				assert(false);

			delete node;
			return CreateNode<TableConstKeyASTNode>(pos, pos, type);
		}
		else if (node->GetType() == StringifiedIdASTNode::Type) {
			delete node;
			return CreateNode<TableConstKeyASTNode>(pos, pos, CONST_KEY_STRINGIFIED_ID);
		}
	}
	return node;
}

////////////////////////////////////////////////////////////////////////

QuotedElementsASTNode* ExpressionListToQuotedElementList (ExpressionListASTNode* node)
{
	assert(node);
	QuotedElementsASTNode* result = new QuotedElementsASTNode(node->GetRange());
	for (ExpressionListASTNode::Iterator i = node->Begin(); i != node->End(); ++i)
		result->AppendChild((*i)->Clone());
	delete node;
	return result;
}

//**********************************************************************

#define YYERROR_VERBOSE 1

////////////////////////////////////////////////////////////////////////

%}

%pure-parser
%parse-param {delta::ParsingCtx& ctx}
%lex-param   {YYSTYPE* yylval, YYLTYPE* lparam, delta::ParsingCtx& ctx}
%locations

////////////////////////////////////////////////////////////////////////
// Token possible values
//
%union {
	const char*						literalValue;

	delta::DeltaASTNode*			nodeValue;
	delta::StmtsASTNode*			stmtsValue;
	
	delta::ExpressionListASTNode*	exprListValue;
	delta::ArgListASTNode*			argListValue;

	delta::UnaryKwdASTNode*			unaryKwdValue;
	delta::LeafKwdASTNode*			leafKwdValue;

	delta::WhileASTNode*			whileValue;
	delta::ForASTNode*				forValue;
	delta::ForeachASTNode*			foreachValue;
	delta::IfASTNode*				ifValue;
	delta::ElseASTNode*				elseValue;
	delta::CompoundASTNode*			compValue;
	delta::TryASTNode*				tryValue;
	delta::TrapASTNode*				trapValue;

	delta::FunctionASTNode*			funcValue;

	delta::TernaryOpASTNode*		ternaryOpValue;
	delta::PrefixOpASTNode*			prefixOpValue;
	delta::SuffixOpASTNode*			suffixOpValue;
	delta::BinaryOpASTNode*			binaryOpValue;
	delta::UnaryOpASTNode*			unaryOpValue;

	delta::CallASTNode*				callValue;

	delta::StringifiedIdASTNode*	stringifiedValue;
	delta::AttributeASTNode*		attributeValue;
	delta::VariableASTNode*			varValue;
	delta::ConstASTNode*			constValue;
	delta::ArgASTNode*				argValue;

	delta::NewAttributeASTNode*		newAttributeValue;
	delta::TableElemASTNode*		tableElemValue;
	delta::TableElemsASTNode*		tableElemsValue;
	delta::TableIndexListASTNode*	tableIListValue;
	delta::TableConstructASTNode*	tableCtorValue;
	delta::UsingASTNode*			usingValue;
	
	delta::QuotedElementsASTNode*	elementsValue;
}

////////////////////////////////////////////////////////////////////////
// Tokens & types
//
%start	DeltaCode

////////////////////////////////////////////////////////////////////////

%token TOKEN_FIRST //--> Keep this the first TOKEN definition (not used in grammar)

////////////////////////////////////////////////////////////////////////

%type	<nodeValue>			Lvalue Primary Term Stmt ExpressionListStmt Expression NonExprListCompoundOrFunctionStmt
%type	<nodeValue>			ContentExpression FunctionAndTableObject AttributeId StringConstUsed LambdaCode
%type	<nodeValue>			FunctionCallObject ActualArgument NonExpressionActualArgument TableObject IndexExpression
%type	<nodeValue>			FunctionName FormalArg DotIndex DottedOpString IdentIndex DottedIdent 
%type	<nodeValue>			SpecialExprListStmt QuotedStmt NonFunctionQuotedStmt NonExpressionElement
%type	<nodeValue>			MetaStmt MetaExpression MetaGeneratedCode QuasiQuoted Ident
%type	<nodeValue>			QualifiedId PureQualifiedId NonGlobalQualifiedId GlobalQualifiedId AttrFunction ForeachOpt
%type	<exprListValue>		ExpressionList ContentList ForInitList ForRepeatList ActualArguments
%type	<elementsValue>		QuotedElements QuotedElementList
%type	<argListValue>		FormalArgs FormalArgList

%destructor { delete $$; }	Lvalue Primary Term Stmt ExpressionListStmt Expression NonExprListCompoundOrFunctionStmt
%destructor { delete $$; }	ContentExpression FunctionAndTableObject AttributeId StringConstUsed LambdaCode
%destructor { delete $$; }	FunctionCallObject ActualArgument NonExpressionActualArgument TableObject IndexExpression
%destructor { delete $$; }	FunctionName FormalArg DotIndex DottedOpString IdentIndex DottedIdent 
%destructor { delete $$; }	SpecialExprListStmt QuotedStmt NonFunctionQuotedStmt NonExpressionElement
%destructor { delete $$; }	MetaStmt MetaExpression MetaGeneratedCode QuasiQuoted Ident
%destructor { delete $$; }	QualifiedId PureQualifiedId NonGlobalQualifiedId GlobalQualifiedId AttrFunction ForeachOpt
%destructor { delete $$; }	ExpressionList ContentList ForInitList ForRepeatList ActualArguments
%destructor { delete $$; }	QuotedElements QuotedElementList
%destructor { delete $$; }	FormalArgs FormalArgList

%type	<stmtsValue>		Stmts CodeDefs TwoOrMoreQuotedStmts SpecialExprListStmts
%type	<unaryKwdValue>		AssertStmt ReturnStmt ThrowStmt AttributeSet AttributeGet FunctionLinkage
%type	<usingValue>		UsingDirective
%type	<argValue>			FormalArgsSuffix
%type	<leafKwdValue>		LoopCtrlStmt
%type	<whileValue>		WhileStmt
%type	<forValue>			ForStmt
%type	<foreachValue>		ForeachStmt
%type	<ifValue>			IfStmt
%type	<elseValue>			ElseStmt
%type	<compValue>			Compound
%type	<funcValue>			Function LambdaFunction 
%type	<ternaryOpValue>	TernaryExpression
%type	<tryValue>			TryStmt
%type	<trapValue>			TrapStmt

%destructor { delete $$; }	Stmts CodeDefs TwoOrMoreQuotedStmts SpecialExprListStmts
%destructor { delete $$; }	AssertStmt ReturnStmt ThrowStmt AttributeSet AttributeGet FunctionLinkage
%destructor { delete $$; }	UsingDirective
%destructor { delete $$; }	FormalArgsSuffix
%destructor { delete $$; }	LoopCtrlStmt
%destructor { delete $$; }	WhileStmt
%destructor { delete $$; }	ForStmt
%destructor { delete $$; }	ForeachStmt
%destructor { delete $$; }	IfStmt
%destructor { delete $$; }	ElseStmt
%destructor { delete $$; }	Compound
%destructor { delete $$; }	Function LambdaFunction
%destructor { delete $$; }	TernaryExpression
%destructor { delete $$; }	TryStmt
%destructor { delete $$; }	TrapStmt

%type	<binaryOpValue>		RelationalExpression BooleanExpression ArithmeticExpression TableContent
%type	<binaryOpValue>		AssignExpression ConstAssignExpression
%type	<callValue>			FunctionCall
%type	<nodeValue>			ConstValueExceptString

%destructor { delete $$; }	RelationalExpression BooleanExpression ArithmeticExpression TableContent
%destructor { delete $$; }	AssignExpression ConstAssignExpression
%destructor { delete $$; }	FunctionCall
%destructor { delete $$; }	ConstValueExceptString

%type	<newAttributeValue>	NewAttribute
%type	<tableElemValue>	TableElement IdentIndexElement IndexedValues
%type	<tableElemsValue>	TableElements
%type	<tableIListValue>	IndexedList
%type	<tableCtorValue>	TableConstructor

%destructor { delete $$; }	NewAttribute
%destructor { delete $$; }	TableElement IdentIndexElement IndexedValues
%destructor { delete $$; }	TableElements
%destructor { delete $$; }	IndexedList
%destructor { delete $$; }	TableConstructor

%type	<literalValue>	OpString FuncClass MultipleEscapes
%token	<literalValue>	FUNCTION RETURN ASSERT LAMBDA LAMBDA_REF ONEVENT METHOD SELF ARGUMENTS STATIC CONST KWD_IF
%token	<literalValue>	ELSE WHILE FOR FOREACH NIL AND NOT OR TRUE FALSE BREAK CONTINUE LOCAL TRY TRAP THROW USING
%token	<literalValue>	ADD SUB MUL DIV MOD GT LT NE EQ GE LE ASSIGN
%token	<literalValue>	ADD_POSTFIX SUB_POSTFIX MUL_POSTFIX DIV_POSTFIX MOD_POSTFIX
%token	<literalValue>	GT_POSTFIX LT_POSTFIX NE_POSTFIX EQ_POSTFIX GE_POSTFIX LE_POSTFIX CAST CALL
%token	<literalValue>	GLOBAL_SCOPE PLUSPLUS MINUSMINUS DOT DOT_ASSIGN DOUBLE_DOT TRIPLE_DOT DOUBLE_LB DOUBLE_RB
%token	<literalValue>	ADD_A SUB_A MUL_A DIV_A MOD_A
%token	<literalValue>	SET GET ATTRIBUTE STRINGIFICATION NEWSELF
%token	<literalValue>	DOT_EQUAL DOT_CAST DOT_EQUAL_RHS
%token	<literalValue>	META_LSHIFT META_RSHIFT META_ESCAPE META_INLINE META_EXECUTE META_RENAME

////////////////////////////////////////////////////////////////////////
// Untyped tokens
//
%token	'(' ')' '[' ']' '{' '}' '?' ':' ','
%token	NUMBER_CONST STRING_CONST IDENT ATTRIBUTE_IDENT OPERATOR
%token	PARENTHESIS SQUARE_BRACKETS							// Used only for priorities
%token	COMMENT	INCOMPLETE_COMMENT INCOMPLETE_STRING CPP	// Not used in grammar

/* For partial selective parsing. */
%token	PARSE_STMT PARSE_FUNCTION_CALL_OBJECT PARSE_TABLE_OBJECT PARSE_NAMESPACE

////////////////////////////////////////////////////////////////////////

%token TOKEN_LAST //<-- Keep this the last TOKEN definition (not used in grammar)

////////////////////////////////////////////////////////////////////////
// Priorities
//
%right		ASSIGN ADD_A SUB_A MUL_A DIV_A MOD_A
%left		OR 
%left		AND
%nonassoc	EQ NE
%nonassoc	GE LE GT LT
%left		ADD SUB
%left		MUL DIV MOD
%right		NOT PLUSPLUS MINUSMINUS UMINUS META_ESCAPE META_INLINE
%nonassoc	DOT_ASSIGN
%left		DOT
%left		SQUARE_BRACKETS
%left		PARENTHESIS

////////////////////////////////////////////////////////////////////////

%expect 2	// 1)if - else 
			// 2) t.#x.y (t["x.y"] instead of t["x"].y)

////////////////////////////////////////////////////////////////////////

%%

////////////////////////////////////////////////////////////////////////
// DeltaCode (Top level definitions)
//

/* For partial selective parsing. */
NullifyTree	:	{ 
					ctx.GetProgramDesc().SetAST(0);	
					ctx.GetProgramDesc().SetStmtAST(0);
					ctx.GetProgramDesc().SetFunctionObjectAST(0);
					ctx.GetProgramDesc().SetTableObjectAST(0);
					ctx.GetProgramDesc().SetNamespaceAST(0);
				}
				;
				
DeltaCode	:	CodeDefs															{ ctx.GetProgramDesc().SetAST($1);	}
			/* For partial selective parsing. */
			|	PARSE_STMT						NullifyTree	Stmt					{ ctx.GetProgramDesc().SetStmtAST($3);	}
			|	PARSE_FUNCTION_CALL_OBJECT		NullifyTree	FunctionAndTableObject	{ ctx.GetProgramDesc().SetFunctionObjectAST($3); }
			|	PARSE_TABLE_OBJECT				NullifyTree	FunctionAndTableObject	{ ctx.GetProgramDesc().SetTableObjectAST($3);	}
			|	PARSE_NAMESPACE					NullifyTree	QualifiedId				{ ctx.GetProgramDesc().SetNamespaceAST($3);	}
			;

CodeDefs	:	Stmts		{ $$ = $1;	}
			|	/* Empty */ { $$ = 0;	}
			;
			
Stmts	:	Stmts Stmt	{ $$ = CreateList<StmtsASTNode>(@1, @2, $1, $2); }
		|	Stmt		{ $$ = CreateList<StmtsASTNode>(@1, @1, 0, $1);	 }
		;

Ident	:	META_RENAME IDENT	{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @2, CreateNode<VariableASTNode>(@2, @2), $1); }
		|	IDENT				{ $$ = CreateNode<VariableASTNode>(@1, @1); }
		|	MetaGeneratedCode	{ $$ = $1; }
		;

////////////////////////////////////////////////////////////////////////
// Functions
//
FuncClass	:	FUNCTION	{ $$ = $1; }
			|	ONEVENT		{ $$ = $1; }
			|	METHOD		{ $$ = $1; }
			;

FunctionLinkage:		LOCAL		{ $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @1, 0, $1);  }
					|	/* Empty */	{ $$ = 0;  }
					;
			
FunctionName:	META_RENAME IDENT		{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @2, CreateNode<FunctionNameASTNode>(@2, @2), $1); }
			|	IDENT					{ $$ = CreateNode<FunctionNameASTNode>(@1, @1); }
			|	MetaGeneratedCode		{ $$ = $1; }
			|	OPERATOR OperatorMethod	{ $$ = CreateNode<FunctionNameASTNode>(@1, @2); }
			|	ATTRIBUTE				{ $$ = CreateNode<FunctionNameASTNode>(@1, @1); }
			|	/* Empty */				{ $$ = 0; }
			;

OperatorMethod	:	OpString
				|	DOT
				|	DOT_ASSIGN
				;

FormalArgs	:	'(' FormalArgList FormalArgsSuffix	{
					$$ = $2;
					if ($3) {
						$$->AppendChild($3);
						$$->GetRange().right = $3->GetRange().right;
					}
				}
			|	'(' FormalArgsSuffix			{
					if ($2) {
						YYLTYPE pos = @2;
						pos.last_column = $2->GetRange().right;
						$$ = CreateNode<ArgListASTNode>(@2, pos);
						$$->AppendChild($2);
					}
					else
						$$ = 0;
				}
			|	CALL		{ $$ = 0; }
			|	/* Empty */	{ $$ = 0; }	/* empty, arg signature can be skipped */
			;

FormalArgsSuffix:	')'				{ $$ = 0; }
				|	TRIPLE_DOT ')'	{ $$ = CreateNode<ArgASTNode>(@1, @1); }
				;


FormalArgList	:	FormalArgList ',' FormalArg	{ $$ = CreateList<ArgListASTNode>(@1, @3, $1, $3);	}
				|	FormalArg					{ $$ = CreateList<ArgListASTNode>(@1, @1, 0, $1);	}
				;

FormalArg	:	META_RENAME IDENT	{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @2, CreateNode<ArgASTNode>(@2, @2), $1); }
			|	IDENT				{ $$ = CreateNode<ArgASTNode>(@1, @1); }
			|	MetaGeneratedCode	{ $$ = $1; }
			;

Function	:	FuncClass FunctionLinkage FunctionName FormalArgs Compound {
					$$ = CreateNode<FunctionASTNode>(@1, @5, $1);
					$$->SetChild<0>($3);
					$$->SetChild<1>($4);
					$$->SetChild<2>($5);
					$$->SetChild<3>($2);
				}
			;

LambdaFunction	:	LAMBDA FormalArgs LambdaCode {
						$$ = CreateNode<FunctionASTNode>(@1, @3, $1);
						$$->SetChild<0>(0);		// Anonymous.
						$$->SetChild<1>($2);
						$$->SetChild<2>($3);
					}
				;
			
LambdaCode	:	'{' Expression '}' { $$ = $2; }
			;
												
////////////////////////////////////////////////////////////////////////
// Attributes
//
AttributeId	:	ATTRIBUTE_IDENT				{ $$ = CreateNode<AttributeASTNode>(@1, @1); }
			|	ATTRIBUTE IDENT				{ $$ = CreateNode<AttributeASTNode>(@1, @2); }
			|	ATTRIBUTE MetaGeneratedCode	{ $$ = $2; }
			|	ATTRIBUTE KwdIdent			{ $$ = CreateNode<AttributeASTNode>(@1, @2); }
			;

DottedIdent	:	DOT StringIdent		{ $$ = CreateNode<TableConstKeyASTNode>(@1, @2, CONST_KEY_DOTID); }
			;

IdentIndex	:	AttributeId { $$ = $1; }
			|	DottedIdent { $$ = $1; }
			;

IdentIndexElement:	IdentIndex ':' AttrFunction {
						$$ =	CreateBinaryNode<TableElemASTNode>(
									@1, 
									@3, 
									CreateNode<ConstASTNode>(@1, @1, CONST_STRING), 
									$3
								);
						delete $1;
					}
					;
						
NewAttribute	:	AttributeId '{'	AttributeSet AttributeGet '}' {
						$$ = CreateNode<NewAttributeASTNode>(@1, @5);
						$$->SetChild<0>($1);
						$$->SetChild<1>($3);
						$$->SetChild<2>($4);
					}
				;

AttrFunction:		Expression	{ $$ = $1; }
				|	Function 	{ $$ = $1; }
				;
				
AttributeSet:	SET AttrFunction	{ $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @2, $2, $1); }
			;
				
AttributeGet:	GET AttrFunction	{ $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @2, $2, $1); }
			;

////////////////////////////////////////////////////////////////////////
// Statements support
//
Stmt:	NonExprListCompoundOrFunctionStmt	{ $$ = $1; }
	|	ExpressionListStmt					{ $$ = $1; }
	|	Compound							{ $$ = $1; }
	|	Function							{ $$ = $1; }
	;

ExpressionListStmt	:	ExpressionList ';'	{ $$ = CreateUnaryNode<OtherStmtASTNode>(@1, @2, $1); }
					;

NonExprListCompoundOrFunctionStmt	:	AssertStmt			{ $$ = $1; }
									|	WhileStmt			{ $$ = $1; }
									|	ForStmt				{ $$ = $1; }
									|	ForeachStmt			{ $$ = $1; }
									|	IfStmt				{ $$ = $1; }
									|	ReturnStmt 			{ $$ = $1; }	
									|	LoopCtrlStmt ';'	{ $$ = CreateUnaryNode<OtherStmtASTNode>(@1, @2, $1); }
									|	TryStmt				{ $$ = $1; }
									|	ThrowStmt 			{ $$ = $1; }
									|	';'					{ $$ = CreateUnaryNode<OtherStmtASTNode>(@1, @1, 0); }
									|	UsingDirective		{ $$ = $1; }
									|	MetaStmt			{ $$ = $1; }
									|	error {
											$$ = 0;
											ctx.GetProgramDesc().AddNotParsed(Range(@1.first_column, @1.last_column));
										}
									;

LoopCtrlStmt:	BREAK		{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
			|	CONTINUE	{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
			;

Compound:	'{' Stmts '}'	{ $$ = CreateUnaryNode<CompoundASTNode>(@1, @3, $2); }
		|	'{' '}'			{ $$ = CreateUnaryNode<CompoundASTNode>(@1, @2, 0);	 }
		;

AssertStmt	:	ASSERT Expression ';' { $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @3, $2, $1); }
			;

WhileStmt	:	WHILE '(' Expression ')' Stmt {
					$$ = CreateNode<WhileASTNode>(@1, @5);
					$$->SetChild<0>($3);
					$$->SetChild<1>($5);
				}
			;

IfStmt	:	KWD_IF '(' Expression ')' Stmt ElseStmt {
				$$ = CreateNode<IfASTNode>(@1, @6);
				$$->SetChild<0>($3);
				$$->SetChild<1>($5);
				$$->SetChild<2>($6);
			}
		;

ElseStmt:	ELSE Stmt	{ $$ = CreateUnaryNode<ElseASTNode>(@1, @2, $2); }
		|	/* Empty */	{ $$ = 0; }
		;

ForStmt:	FOR '(' ForInitList ';'  Expression ';' ForRepeatList ')' Stmt {
				$$ = CreateNode<ForASTNode>(@1, @9);
				$$->SetChild<0>($3);
				$$->SetChild<1>($5);
				$$->SetChild<2>($7);
				$$->SetChild<3>($9);
			}
		;

ForInitList	:	ExpressionList	{ $$ = $1; }
			|	/* Empty */		{ $$ = 0; }
			;

ForRepeatList	:	ExpressionList	{ $$ = $1; }
				|	/* Empty */		{ $$ = 0; }
				;

ForeachOpt:			':' Lvalue ','	{ $$ = $2; }
				|	','				{ $$ = 0; }
				;
				
ForeachStmt:	FOREACH '(' Lvalue ForeachOpt Expression ')' Stmt {
					$$ = CreateNode<ForeachASTNode>(@1, @7);
					$$->SetChild<0>($3);
					$$->SetChild<1>($5);
					$$->SetChild<2>($7);
					$$->SetChild<3>($4);
				}
			;

ReturnStmt	:	RETURN Expression ';'	{ $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @3, $2, $1); }
			|	RETURN ';'				{ $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @2, 0, $1); }
			|	RETURN Function			{ $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @2, $2, $1); }	
			;

ThrowStmt	:	THROW Expression ';' { $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @3, $2, $1); }
			;

TryStmt	:	TRY Stmt TrapStmt {
				$$ = CreateNode<TryASTNode>(@1, @3);
				$$->SetChild<0>($2);
				$$->SetChild<1>($3);
			}
		;

TrapStmt:	TRAP Ident Stmt {
				$$ = CreateNode<TrapASTNode>(@1, @3);
				$$->SetChild<0>($2);
				$$->SetChild<1>($3);
			}
		;

////////////////////////////////////////////////////////////////////////
// Namespaces
//
UsingDirective	:	USING QualifiedId ';' {
						$$ = CreateNode<UsingASTNode>(@1, @3, $1);
						$$->SetChild<0>($2);
					}
				|	USING STRINGIFICATION IDENT ';' {
						$$ = CreateNode<UsingASTNode>(@1, @4, $1);
						$$->SetChild<1>(CreateNode<StringifiedIdASTNode>(@2, @3));
					}
				|	USING STRINGIFICATION MetaGeneratedCode ';' {
						$$ = CreateNode<UsingASTNode>(@1, @4, $1);
						$$->SetChild<1>($3);
					}
				;

QualifiedId	:	NonGlobalQualifiedId	{ $$ = $1; }
			|	GlobalQualifiedId		{ $$ = $1; }
			;

PureQualifiedId	:	NonGlobalQualifiedId GLOBAL_SCOPE Ident
						{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
				;

NonGlobalQualifiedId:	PureQualifiedId	{ $$ = $1; }
					|	Ident			{ $$ = $1; }
					;

GlobalQualifiedId	:	GLOBAL_SCOPE NonGlobalQualifiedId { $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @2, $2, $1); }
					;

////////////////////////////////////////////////////////////////////////
// Expressions
//
Expression	:	AssignExpression		{ $$ = $1; }
			|	ConstAssignExpression	{ $$ = $1; }
			|	RelationalExpression	{ $$ = $1; }
			|	BooleanExpression		{ $$ = $1; }
			|	ArithmeticExpression	{ $$ = $1; }
			|	TernaryExpression		{ $$ = $1; }
			|	Term					{ $$ = $1; }
			;

ConstAssignExpression	:	CONST Ident ASSIGN Expression {
								UnaryKwdASTNode* kwd = CreateUnaryNode<UnaryKwdASTNode>(@1, @2, $2, $1);
								$$ = CreateBinaryNode<BinaryOpASTNode>(@1, @4, kwd, $4, $3); 
							}
						;

AssignExpression	:	Lvalue ASSIGN Expression	{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
					|	Lvalue ADD_A Expression		{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
					|	Lvalue MUL_A Expression		{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
					|	Lvalue SUB_A Expression		{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
					|	Lvalue DIV_A Expression		{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
					|	Lvalue MOD_A Expression		{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
					;

RelationalExpression	:	Expression GT Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						|	Expression LT Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						|	Expression GE Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						|	Expression LE Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						|	Expression EQ Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						|	Expression NE Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						;

BooleanExpression	:	Expression AND Expression
							{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
					|	Expression OR Expression
							{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
					;

ArithmeticExpression	:	Expression ADD Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						|	Expression SUB Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						|	Expression DIV Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						|	Expression MUL Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						|	Expression MOD Expression
								{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
						;

TernaryExpression	:	'(' Expression '?' Expression ':' Expression ')' %prec ASSIGN {
							$$ = CreateNode<TernaryOpASTNode>(@1, @7);
							$$->SetChild<0>($2);
							$$->SetChild<1>($4);
							$$->SetChild<2>($6);
						}
					;

Term:	Lvalue PLUSPLUS				{ $$ = CreateUnaryNode<SuffixOpASTNode>(@1, @2, $1, $2); }
	|	PLUSPLUS Lvalue				{ $$ = CreateUnaryNode<PrefixOpASTNode>(@1, @2, $2, $1); }
	|	Lvalue MINUSMINUS			{ $$ = CreateUnaryNode<SuffixOpASTNode>(@1, @2, $1, $2); }
	|	MINUSMINUS Lvalue			{ $$ = CreateUnaryNode<PrefixOpASTNode>(@1, @2, $2, $1); }
	|	SUB Expression %prec UMINUS { $$ = CreateUnaryNode<PrefixOpASTNode>(@1, @2, $2, $1); }
	|	NOT Expression				{ $$ = CreateUnaryNode<PrefixOpASTNode>(@1, @2, $2, $1); }
	|	Primary						{ $$ = $1; }
	|	MetaExpression				{ $$ = $1; }
	;

Primary	:	FunctionCallObject		{ $$ = $1; }
		|	ConstValueExceptString	{ $$ = $1; }
		|	LambdaFunction			{ $$ = $1; }
		;

FunctionAndTableObject	:	Lvalue				{ $$ = $1; }
						|	FunctionCall		{ $$ = $1; }
						|	TableConstructor	{ $$ = $1; }
						|	'(' Function ')'	{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @3, $2, "()"); }
						|	'(' Expression ')'	{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @3, $2, "()"); }
						|	SELF				{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
						|	ARGUMENTS			{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
						|	LAMBDA_REF			{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
						|	NEWSELF				{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
						;

ConstValueExceptString	:	NUMBER_CONST	{ $$ = CreateNode<ConstASTNode>(@1, @1, CONST_NUMBER); }
						|	NIL				{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
						|	TRUE			{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
						|	FALSE			{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
						;

StringConst	:				StringConst STRING_CONST
						|	STRING_CONST
						;

KwdIdent:					KWD_IF
						|	ELSE
						|	FUNCTION
						|	RETURN
						|	ONEVENT
						|	WHILE
						|	FOR
						|	FOREACH
						|	NIL
						|	LOCAL
						|	AND
						|	NOT
						|	OR
						|	LAMBDA
						|	TRY
						|	TRAP
						|	THROW
						|	USING
						|	ASSERT
						|	TRUE
						|	FALSE
						|	BREAK
						|	CONTINUE
						|	STATIC
						|	CONST
						|	METHOD
						|	SELF
						|	ARGUMENTS
						;
						
StringIdent				:	IDENT
						|	KwdIdent
						;
						
StringifyDottedIdents	:	STRINGIFICATION StringIdent
						|	StringifyDottedIdents DOT StringIdent
						;

StringifyNamespaceIdent	:	STRINGIFICATION PureQualifiedId		{ delete $2; }
						|	STRINGIFICATION GlobalQualifiedId	{ delete $2; }
						;

StringConstUsed	:	StringConst				{ $$ = CreateNode<ConstASTNode>(@1, @1, CONST_STRING); }
				| 	StringifyDottedIdents	{ $$ = CreateNode<StringifiedIdASTNode>(@1, @1); }
				|	StringifyNamespaceIdent { $$ = CreateNode<StringifiedIdASTNode>(@1, @1); }
				;

OpString:	ADD
		|	SUB
		|	MUL
		|	DIV
		|	MOD
		|	GT
		|	LT
		|	NE
		|	EQ
		|	GE
		|	LE
		|	ASSIGN
		|	ADD_POSTFIX
		|	SUB_POSTFIX
		|	MUL_POSTFIX
		|	DIV_POSTFIX
		|	MOD_POSTFIX
		|	GT_POSTFIX
		|	LT_POSTFIX
		|	NE_POSTFIX
		|	EQ_POSTFIX
		|	GE_POSTFIX
		|	LE_POSTFIX
		|	CALL
		|	CAST
		;

////////////////////////////////////////////////////////////////////////
// Calls
//
FunctionCallObject	:	FunctionAndTableObject	{ $$ = $1; }
					|	StringConstUsed			{ $$ = $1; }
					;

ActualArguments	:	ActualArguments ',' ActualArgument	{ $$ = CreateList<ExpressionListASTNode>(@1, @3, $1, $3);	}
				|	ActualArgument						{ $$ = CreateList<ExpressionListASTNode>(@1, @1, 0, $1);	}
				;

ActualArgument	:	Expression					{ $$ = $1; }
				|	NonExpressionActualArgument	{ $$ = $1; }
				;
				
NonExpressionActualArgument	:	'|' Expression '|'	{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @3, $2, "||"); }
							|	TRIPLE_DOT			{ $$ = CreateNode<LeafKwdASTNode>(@1, @1, $1); }
							|	Function			{ $$ = $1; }
							;

FunctionCall	:	FunctionCallObject '(' ActualArguments ')' %prec PARENTHESIS {
						$$ = CreateNode<CallASTNode>(@1, @4);
						$$->SetChild<0>($1);
						$$->SetChild<1>($3);
					}
				|	FunctionCallObject '(' ')' %prec PARENTHESIS {
						$$ = CreateNode<CallASTNode>(@1, @3);
						$$->SetChild<0>($1);
					}
				|	FunctionCallObject CALL %prec PARENTHESIS {
						$$ = CreateNode<CallASTNode>(@1, @2);
						$$->SetChild<0>($1);
					}
				;

ExpressionList	:	ExpressionList ',' Expression	{ $$ = CreateList<ExpressionListASTNode>(@1, @3, $1, $3);	}
				|	Expression						{ $$ = CreateList<ExpressionListASTNode>(@1, @1, 0, $1);	}
				;

////////////////////////////////////////////////////////////////////////
// Lvalues
//
Lvalue	:	QualifiedId		{ $$ = $1; }
		|	AttributeId		{ $$ = $1; }
		|	STATIC Ident	{ $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @2, $2, $1); }
		|	LOCAL Ident 	{ $$ = CreateUnaryNode<UnaryKwdASTNode>(@1, @2, $2, $1); }
		|	TableContent	{ $$ = $1; }
		;

////////////////////////////////////////////////////////////////////////
// Tables
//
TableObject	:	FunctionAndTableObject	{ $$ = $1; }
			;


DotIndex:	IDENT			{ $$ = CreateNode<TableConstKeyASTNode>(@1, @1, CONST_KEY_IDENT); }
		|	KwdIdent		{ $$ = CreateNode<TableConstKeyASTNode>(@1, @1, CONST_KEY_IDENT); }
		|	OpString		{ $$ = CreateNode<TableConstKeyASTNode>(@1, @1, CONST_KEY_STRING); }
		|	StringConstUsed	{ $$ = ExpressionToConstKey(@1, $1); }
		;

DottedOpString:		DOT_ASSIGN		
						{ $$ = CreateNode<TableConstKeyASTNode>(@1, @1, CONST_KEY_STRING); ++($$->GetRange().left); }
				|	DOT_EQUAL		
						{ $$ = CreateNode<TableConstKeyASTNode>(@1, @1, CONST_KEY_STRING); ++($$->GetRange().left);}
				|	DOT_CAST		
						{ $$ = CreateNode<TableConstKeyASTNode>(@1, @1, CONST_KEY_STRING); ++($$->GetRange().left);}
				|	DOT_EQUAL_RHS	
						{ $$ = CreateNode<TableConstKeyASTNode>(@1, @1, CONST_KEY_STRING); ++($$->GetRange().left);}
				;

TableContent:	TableObject DOT DotIndex		{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
			|	TableObject DottedOpString		{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @2, $1, $2, "."); }
			|	TableObject DOUBLE_DOT DotIndex { $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @3, $1, $3, $2); }
			|	TableObject '[' Expression ']' %prec SQUARE_BRACKETS
					{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @4, $1, ExpressionToConstKey(@3, $3), "[]"); }
			|	TableObject '[' OperatorMethod ']' %prec SQUARE_BRACKETS
					{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @4, $1, CreateNode<LeafKwdASTNode>(@3, @3), "[]"); }
			|	TableObject DOUBLE_LB Expression DOUBLE_RB %prec SQUARE_BRACKETS
					{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @4, $1, ExpressionToConstKey(@3, $3), "[[]]"); }
			|	TableObject DOUBLE_LB OperatorMethod DOUBLE_RB %prec SQUARE_BRACKETS
					{ $$ = CreateBinaryNode<BinaryOpASTNode>(@1, @4, $1, CreateNode<LeafKwdASTNode>(@3, @3), "[[]]"); }
			;

TableConstructor:	'[' TableElements ']'	{ $$ = CreateUnaryNode<TableConstructASTNode>(@1, @3, $2); }
				|	'[' ']'					{ $$ = CreateUnaryNode<TableConstructASTNode>(@1, @2, 0); }
				;

TableElements	:	TableElements ',' TableElement	{ $$ = CreateList<TableElemsASTNode>(@1, @3, $1, $3);	}
				|	TableElement					{ $$ = CreateList<TableElemsASTNode>(@1, @1, 0, $1);	}
				;

TableElement:	Expression								{ $$ = CreateBinaryNode<TableElemASTNode>(@1, @1, 0, $1);	}
			|	Function								{ $$ = CreateBinaryNode<TableElemASTNode>(@1, @1, 0, $1);	}
			|	IndexedValues							{ $$ = $1; }
			|   NewAttribute							{ $$ = CreateBinaryNode<TableElemASTNode>(@1, @1, 0, $1);	}
			|	IdentIndexElement						{ $$ = $1;}
			;

ContentList:		ContentList ',' ContentExpression	{ $$ = CreateList<ExpressionListASTNode>(@1, @3, $1, $3);	}
				|	ContentExpression					{ $$ = CreateList<ExpressionListASTNode>(@1, @1, 0, $1);	}
				;

ContentExpression:		Expression		{ $$ = $1; }
					|	Function		{ $$ = $1; }
						;

IndexExpression	:	Expression		{ $$ = ExpressionToConstKey(@1, $1); }
				|	DottedIdent		{ $$ = $1; }
				|	OpString		{ $$ = CreateNode<TableConstKeyASTNode>(@1, @1, CONST_KEY_OPSTRING); }
				;

IndexedList	:	IndexedList ',' IndexExpression	{ $$ = CreateList<TableIndexListASTNode>(@1, @3, $1, $3);	}
			|	IndexExpression					{ $$ = CreateList<TableIndexListASTNode>(@1, @1, 0, $1);	}
			;

IndexedValues	:	'{' IndexedList ':' ContentList '}'	{ $$ = CreateBinaryNode<TableElemASTNode>(@1, @5, $2, $4);	}
				;

////////////////////////////////////////////////////////////////////////
// Metaprogramming
//
MultipleEscapes	:	MultipleEscapes META_ESCAPE	{ $$ = $1; }
				|	META_ESCAPE					{ $$ = $1; }
				;

MetaGeneratedCode	:	MultipleEscapes '(' Expression ')'	{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @4, $3, $1);	}
					|	MultipleEscapes IDENT
							{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @2, CreateNode<VariableASTNode>(@2, @2), $1); }
					|	META_INLINE		'(' Expression ')'	{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @4, $3, $1); }
					;

NonExpressionElement	:	IndexedValues				{ $$ = $1; }
						|	NewAttribute				{ $$ = $1; }
						|	IdentIndexElement			{ $$ = $1; }
						|	DottedIdent		 			{ $$ = $1; }
						|	OpString					{ $$ = CreateNode<TableConstKeyASTNode>(@1, @1, CONST_KEY_OPSTRING); }
						|	NonExpressionActualArgument	{ $$ = $1; }
						;

QuotedElementList	:	ExpressionList ',' NonExpressionElement
							{ $$ = CreateList<QuotedElementsASTNode>(@1, @3, ExpressionListToQuotedElementList($1), $3); }
					|	QuotedElementList ',' NonExpressionElement
							{ $$ = CreateList<QuotedElementsASTNode>(@1, @3, $1, $3);	}
					|	QuotedElementList ',' Expression
							{ $$ = CreateList<QuotedElementsASTNode>(@1, @3, $1, $3);	}
					|	NonExpressionElement	{ $$ = CreateList<QuotedElementsASTNode>(@1, @1, 0, $1); }
					;
					
QuotedElements	:	QuotedElementList	{ $$ = $1; }
				|	ExpressionList		{ $$ = ExpressionListToQuotedElementList($1); }
				;

SpecialExprListStmt	:	NonExprListCompoundOrFunctionStmt	{ $$ = $1; }
					|	IndexedList ';'						{ $$ = $1; }
					|	Compound							{ $$ = $1; }
					|	Function							{ $$ = $1; }
					;

SpecialExprListStmts:	SpecialExprListStmts SpecialExprListStmt	{ $$ = CreateList<StmtsASTNode>(@1, @2, $1, $2); }
					|	SpecialExprListStmt							{ $$ = CreateList<StmtsASTNode>(@1, @1, 0, $1);	 }
					;

NonFunctionQuotedStmt	:	NonExprListCompoundOrFunctionStmt	{ $$ = $1; }
						|	ExpressionListStmt					{ $$ = $1; }
						|	'{' SpecialExprListStmts '}'		{ $$ = CreateUnaryNode<CompoundASTNode>(@1, @3, $2); }
						|	'{' '}'								{ $$ = CreateUnaryNode<CompoundASTNode>(@1, @2, 0);	 }
						;

QuotedStmt	:	NonFunctionQuotedStmt	{ $$ = $1; }
			|	Function				{ $$ = $1; }
			;
				
TwoOrMoreQuotedStmts	:	TwoOrMoreQuotedStmts QuotedStmt	{ $$ = CreateList<StmtsASTNode>(@1, @2, $1, $2); }
						|	QuotedStmt QuotedStmt {
								$$ = CreateList<StmtsASTNode>(@1, @2, 0, $1);
								$$->AppendChild($2);
							}
						;

QuasiQuoted	:	QuotedElements			{ $$ = $1; }
			|	NonFunctionQuotedStmt	{ $$ = CreateList<StmtsASTNode>(@1, @1, 0, $1); }
			|	TwoOrMoreQuotedStmts	{ $$ = $1; }
			;

MetaExpression	:	META_LSHIFT QuasiQuoted META_RSHIFT	{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @3, $2, "<<>>"); }
				;

MetaStmt:	META_EXECUTE Stmt	{ $$ = CreateUnaryNode<UnaryOpASTNode>(@1, @2, $2, $1); }
		;

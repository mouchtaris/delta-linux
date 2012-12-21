%{
/**
 *	DeltaLexAnalyzerSpec.l
 *
 * Lexical analyser of the Delta language
 *
 *	Based on ScriptFighter project
 *	A. Savidis, Start October 1999.
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "Common.h"
#include "DeltaParserCommon.h"
#include "DeltaAST.h"
#include "DeltaASTFwdDecls.h"
#include "DeltaParserGen.hpp"
#include "ProgramDescription.h"
#include "DeltaASTDataTypes.h"

#include <stdlib.h>

////////////////////////////////////////////////////////////////////////
// Lex configuration defines
//
#define YY_USE_PROTOS
#undef YY_INTERACTIVE

#define YY_DECL	\
	int yyFlexLexer::yylex (YYSTYPE* yylval, YYLTYPE* yylloc)

#define YY_USER_ACTION				\
	yylloc->first_line = yylineno;	\
	yylloc->first_column = yypos;	\
	yypos += yyleng;				\
	yylloc->last_line = yylineno;	\
	yylloc->last_column = yypos;

////////////////////////////////////////////////////////////////////////
// Utility macros
//
#define LITERAL(symbol, lit)	\
	yylval->literalValue = lit;	\
	return symbol

%}

%option 8bit
%option warn nodefault
%option noyywrap
%option	never-interactive 
%option c++

newline		\n
ws			[ \t\r\v]
decdigit	[0-9]
decnumber	{decdigit}+("."{decdigit}+)?
hexnumber	0x[0-9a-fA-F]+
id			[a-zA-Z][a-zA-Z0-9_]*
flag		{ws}+{decdigit}
cpp			#{ws}+{decdigit}+{ws}+\"[^"]+{flag}?
attr		@

%%

"/*"				{
						int c = 0, prevc, count = 1, token = COMMENT;
						while (true) {
							prevc = c;
							while (++yypos && (c = yyinput()) != '/' && c != EOF)
								prevc = c;
							if (c == EOF) {
								token = INCOMPLETE_COMMENT;
								--yypos;
								break;
							}
							else if (prevc == '*') {
								if (!--count)
									break;
							}
							else if (++yypos && (c = yyinput()) == '*')
								++count;
						}
						yylloc->last_line = yylineno;
						yylloc->last_column = yypos;
						return token;
					}

"//".*				return COMMENT;

\"					{
						std::string s;
						int c, token = STRING_CONST;
						while (++yypos && (c = yyinput()) != EOF) {
							if (c == '"') {
								if (s.empty())
									break;
								else {
									bool evenNumOfBackSlashes = true;
									for (	std::string::reverse_iterator i = s.rbegin();
											i != s.rend() && *i == '\\';
											++i, evenNumOfBackSlashes = !evenNumOfBackSlashes	)
											;
									if (evenNumOfBackSlashes)	// Quote is normal end.
										break;
									else
										s.push_back(c);
								}
							}
							else
								s.push_back(c);
						}
						yylloc->last_line = yylineno;
						yylloc->last_column = yypos;
						return c == EOF ? INCOMPLETE_STRING : STRING_CONST;
					}

function			LITERAL(FUNCTION, "function");
return				LITERAL(RETURN, "return");
assert				LITERAL(ASSERT, "assert");
lambda				LITERAL(LAMBDA, "lambda");
@lambda				LITERAL(LAMBDA_REF, "@lambda");
onevent				LITERAL(ONEVENT, "onevent");
method				LITERAL(METHOD, "method");
self				LITERAL(SELF, "self");
arguments			LITERAL(ARGUMENTS, "arguments");
static				LITERAL(STATIC, "static");
const				LITERAL(CONST, "const");
if					LITERAL(KWD_IF, "if");
else				LITERAL(ELSE, "else");
while				LITERAL(WHILE, "while");
for					LITERAL(FOR, "for");
foreach				LITERAL(FOREACH, "foreach");
nil					LITERAL(NIL, "nil");
and					LITERAL(AND, "and");
not					LITERAL(NOT, "not");
or					LITERAL(OR, "or");
true				LITERAL(TRUE, "true");
false				LITERAL(FALSE, "false");
break				LITERAL(BREAK, "break");
continue			LITERAL(CONTINUE, "continue");
local				LITERAL(LOCAL, "local");
try					LITERAL(TRY, "try");
trap				LITERAL(TRAP, "trap");
throw				LITERAL(THROW, "throw");
using				LITERAL(USING, "using");
@set				LITERAL(SET, "@set");
@get				LITERAL(GET, "@get");
@operator			LITERAL(OPERATOR, "@operator");
@self				LITERAL(NEWSELF, "@self");

"@"					LITERAL(ATTRIBUTE, "@");
"#"					LITERAL(STRINGIFICATION, "#");
"+"					LITERAL(ADD, "+");
"-"					LITERAL(SUB, "-");
"*"					LITERAL(MUL, "*");
"/"					LITERAL(DIV, "/");
"%"					LITERAL(MOD, "%");
">"					LITERAL(GT, ">");
"<"					LITERAL(LT, "<");
"!="				LITERAL(NE, "!=");
"=="				LITERAL(EQ, "==");
">="				LITERAL(GE, ">=");
"<="				LITERAL(LE, "<=");
"="					LITERAL(ASSIGN, "=");
"()"				LITERAL(CALL, "()");
"=()"				LITERAL(CAST, "=()");

"+_"				LITERAL(ADD_POSTFIX, "+_");
"-_"				LITERAL(SUB_POSTFIX, "-_");
"*_"				LITERAL(MUL_POSTFIX, "*_");
"/_"				LITERAL(DIV_POSTFIX, "/_");
"%_"				LITERAL(MOD_POSTFIX, "%_");
">_"				LITERAL(GT_POSTFIX, ">_");
"<_"				LITERAL(LT_POSTFIX, "<_");
"!=_"				LITERAL(NE_POSTFIX, "!=_");
"==_"				LITERAL(EQ_POSTFIX, "==_");
">=_"				LITERAL(GE_POSTFIX, ">=_");
"<=_"				LITERAL(LE_POSTFIX, "<=_");

"::"				LITERAL(GLOBAL_SCOPE, "::");
"++"				LITERAL(PLUSPLUS, "++");
"--"				LITERAL(MINUSMINUS, "--");
"."					LITERAL(DOT, ".");
".="				LITERAL(DOT_ASSIGN, ".=");
".."				LITERAL(DOUBLE_DOT, "..");
"..."				LITERAL(TRIPLE_DOT, "...");
"[["				LITERAL(DOUBLE_LB, "[[");
"]]"				LITERAL(DOUBLE_RB, "]]");
"+="				LITERAL(ADD_A, "+=");
"-="				LITERAL(SUB_A, "-=");
"*="				LITERAL(MUL_A, "*=");
"/="				LITERAL(DIV_A, "/=");
"%="				LITERAL(MOD_A, "%=");
".=="				LITERAL(DOT_EQUAL, ".==");
".=()"				LITERAL(DOT_CAST, ".=()");
".==_"				LITERAL(DOT_EQUAL_RHS, ".==_");

"<<"				LITERAL(META_LSHIFT, "<<");
">>"				LITERAL(META_RSHIFT, ">>");
"~"					LITERAL(META_ESCAPE, "~");
"!"					LITERAL(META_INLINE, "!");
"&"					LITERAL(META_EXECUTE, "&");
"$"					LITERAL(META_RENAME, "$");

{newline}+			yylineno += yyleng;
{ws}+

{decnumber}			{
						//yylval->numValue = static_cast<float>(strtod(yytext, NULL));
						return NUMBER_CONST;
					}

{hexnumber}			{
						//yylval->numValue = static_cast<float>(strtol(yytext, NULL, 16));
						return NUMBER_CONST;
					}
				
{id}				{
						//yylval->strValue = new std::string(yytext, yyleng);
						return IDENT;
					}

{attr}{id}			{	
						//yylval->strValue = new std::string(yytext, yyleng);
						return ATTRIBUTE_IDENT;
					}

{cpp}				{
						int c;
						while (++yypos && (c = yyinput()) != EOF && c != '\n'){} // Read until end of line
						yylloc->last_column = yypos;
						return CPP;
					}

.					return yytext[0];

%%

////////////////////////////////////////////////////////////////////////

int yylex (YYSTYPE* yylval, YYLTYPE* yylloc, delta::ParsingCtx& ctx)
{
	while (true) {
		switch (int token = ctx.GetLexer().yylex(yylval, yylloc)) {
			case COMMENT: {
				delta::Range range(yylloc->first_column, yylloc->last_column);
				ctx.GetProgramDesc().AddComment(range);
				break;
			}
			case INCOMPLETE_COMMENT:
			case INCOMPLETE_STRING:
			{
				delta::Range range(yylloc->first_column, yylloc->last_column);
				ctx.GetProgramDesc().AddNotParsed(range);
				break;
			}
			case CPP:
			{
				delta::Range range(yylloc->first_column, yylloc->last_column);
				ctx.GetProgramDesc().AddCpp(range);
				break;
			}
			default:
				return token;
		}
	}
}

//**********************************************************************

int s_tokenTypes[];

int yylex (YYSTYPE* yylval, YYLTYPE* yylloc, int* tokenType, yyFlexLexer& lexer)
{
	int token = lexer.yylex(yylval, yylloc);
	if (token < TOKEN_FIRST)
		*tokenType = 0;
	else {
		assert(token - TOKEN_FIRST < TOKEN_LAST - TOKEN_FIRST);
		*tokenType = s_tokenTypes[token - TOKEN_FIRST - 1];
	}
	return token;
}

//**********************************************************************

using namespace delta;

static int s_tokenTypes[TOKEN_LAST - TOKEN_FIRST - 1] = {
	FunctionASTNode::Type,		// FUNCTION 259
	UnaryKwdASTNode::Type,		// RETURN 260
	UnaryKwdASTNode::Type,		// ASSERT 261
	FunctionASTNode::Type,		// LAMBDA 262
	LeafKwdASTNode::Type,		// LAMBDA_REF 263
	FunctionASTNode::Type,		// ONEVENT 264
	FunctionASTNode::Type,		// METHOD 265
	LeafKwdASTNode::Type,		// SELF 266
	LeafKwdASTNode::Type,		// ARGUMENTS 267
	UnaryKwdASTNode::Type,		// STATIC 268
	UnaryKwdASTNode::Type,		// CONST 269
	IfASTNode::Type,			// IF 270
	ElseASTNode::Type,			// ELSE 271
	WhileASTNode::Type,			// WHILE 272
	ForASTNode::Type,			// FOR 273
	ForeachASTNode::Type,		// FOREACH 274
	LeafKwdASTNode::Type,		// NIL 275
	BinaryOpASTNode::Type,		// AND 276
	UnaryOpASTNode::Type,		// NOT 277
	BinaryOpASTNode::Type,		// OR 278
	LeafKwdASTNode::Type,		// TRUE 279
	LeafKwdASTNode::Type,		// FALSE 280
	LeafKwdASTNode::Type,		// BREAK 281
	LeafKwdASTNode::Type,		// CONTINUE 282
	LeafKwdASTNode::Type,		// LOCAL 283
	TryASTNode::Type,			// TRY 284
	TrapASTNode::Type,			// TRAP 285
	UnaryKwdASTNode::Type,		// THROW 286
	UnaryKwdASTNode::Type,		// USING 287
	BinaryOpASTNode::Type,		// ADD 288
	BinaryOpASTNode::Type,		// SUB 289
	BinaryOpASTNode::Type,		// MUL 290
	BinaryOpASTNode::Type,		// DIV 291
	BinaryOpASTNode::Type,		// MOD 292
	BinaryOpASTNode::Type,		// GT 293
	BinaryOpASTNode::Type,		// LT 294
	BinaryOpASTNode::Type,		// NE 295
	BinaryOpASTNode::Type,		// EQ 296
	BinaryOpASTNode::Type,		// GE 297
	BinaryOpASTNode::Type,		// LE 298
	BinaryOpASTNode::Type,		// ASSIGN 299
	BinaryOpASTNode::Type,		// ADD_POSTFIX 300
	BinaryOpASTNode::Type,		// SUB_POSTFIX 301
	BinaryOpASTNode::Type,		// MUL_POSTFIX 302
	BinaryOpASTNode::Type,		// DIV_POSTFIX 303
	BinaryOpASTNode::Type,		// MOD_POSTFIX 304
	BinaryOpASTNode::Type,		// GT_POSTFIX 305
	BinaryOpASTNode::Type,		// LT_POSTFIX 306
	BinaryOpASTNode::Type,		// NE_POSTFIX 307
	BinaryOpASTNode::Type,		// EQ_POSTFIX 308
	BinaryOpASTNode::Type,		// GE_POSTFIX 309
	BinaryOpASTNode::Type,		// LE_POSTFIX 310
	BinaryOpASTNode::Type,		// CAST 311
	BinaryOpASTNode::Type,		// CALL 312
	UnaryKwdASTNode::Type,		// GLOBAL_SCOPE 313
	PrefixOpASTNode::Type,		// PLUSPLUS 314
	PrefixOpASTNode::Type,		// MINUSMINUS 315
	BinaryOpASTNode::Type,		// DOT 316
	BinaryOpASTNode::Type,		// DOT_ASSIGN 317
	BinaryOpASTNode::Type,		// DOUBLE_DOT 318
	LeafKwdASTNode::Type,		// TRIPLE_DOT 319
	BinaryOpASTNode::Type,		// DOUBLE_LB 320
	BinaryOpASTNode::Type,		// DOUBLE_RB 321
	BinaryOpASTNode::Type,		// ADD_A 322
	BinaryOpASTNode::Type,		// SUB_A 323
	BinaryOpASTNode::Type,		// MUL_A 324
	BinaryOpASTNode::Type,		// DIV_A 325
	BinaryOpASTNode::Type,		// MOD_A 326
	UnaryKwdASTNode::Type,		// SET 327
	UnaryKwdASTNode::Type,		// GET 328
	AttributeASTNode::Type,		// ATTRIBUTE 329
	ConstASTNode::Type,			// STRINGIFICATION 330
	UnaryKwdASTNode::Type,		// NEWSELF 331
	BinaryOpASTNode::Type,		// DOT_EQUAL 332
	BinaryOpASTNode::Type,		// DOT_CAST 333
	BinaryOpASTNode::Type,		// DOT_EQUAL_RHS 334
	LeafKwdASTNode::Type,		// META_LSHIFT 335
	LeafKwdASTNode::Type,		// META_RSHIFT 336
	UnaryKwdASTNode::Type,		// META_ESCAPE 337
	UnaryKwdASTNode::Type,		// META_INLINE 338
	UnaryKwdASTNode::Type,		// META_EXECUTE 339
	UnaryKwdASTNode::Type,		// META_RENAME 340
	ConstASTNode::Type,			// NUMBER_CONST 341
	ConstASTNode::Type,			// STRING_CONST 342
	VariableASTNode::Type,		// IDENT 343
	VariableASTNode::Type,		// ATTRIBUTE_IDENT 344
	UnaryKwdASTNode::Type,		// OPERATOR 345
	0,							// PARENTHESIS 346
	0,							// SQUARE_BRACKETS 347
	CommentsASTNodeType,		// COMMENT 348
	CommentsASTNodeType,		// INCOMPLETE_COMMENT 349
	ConstASTNode::Type,			// INCOMPLETE_STRING 350
	CppASTNodeType,				// CPP 351
	0,							// PARSE_STMT 352
	0,							// PARSE_FUNCTION_CALL_OBJECT 353
	0,							// PARSE_TABLE_OBJECT 354
	0,							// PARSE_NAMESPACE 355
};

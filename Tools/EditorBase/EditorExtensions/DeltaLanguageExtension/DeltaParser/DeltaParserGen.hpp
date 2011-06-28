/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_FIRST = 258,
     FUNCTION = 259,
     RETURN = 260,
     ASSERT = 261,
     LAMBDA = 262,
     LAMBDA_REF = 263,
     ONEVENT = 264,
     METHOD = 265,
     SELF = 266,
     ARGUMENTS = 267,
     STATIC = 268,
     CONST = 269,
     KWD_IF = 270,
     ELSE = 271,
     WHILE = 272,
     FOR = 273,
     FOREACH = 274,
     NIL = 275,
     AND = 276,
     NOT = 277,
     OR = 278,
     TRUE = 279,
     FALSE = 280,
     BREAK = 281,
     CONTINUE = 282,
     LOCAL = 283,
     TRY = 284,
     TRAP = 285,
     THROW = 286,
     USING = 287,
     ADD = 288,
     SUB = 289,
     MUL = 290,
     DIV = 291,
     MOD = 292,
     GT = 293,
     LT = 294,
     NE = 295,
     EQ = 296,
     GE = 297,
     LE = 298,
     ASSIGN = 299,
     ADD_POSTFIX = 300,
     SUB_POSTFIX = 301,
     MUL_POSTFIX = 302,
     DIV_POSTFIX = 303,
     MOD_POSTFIX = 304,
     GT_POSTFIX = 305,
     LT_POSTFIX = 306,
     NE_POSTFIX = 307,
     EQ_POSTFIX = 308,
     GE_POSTFIX = 309,
     LE_POSTFIX = 310,
     CAST = 311,
     CALL = 312,
     GLOBAL_SCOPE = 313,
     PLUSPLUS = 314,
     MINUSMINUS = 315,
     DOT = 316,
     DOT_ASSIGN = 317,
     DOUBLE_DOT = 318,
     TRIPLE_DOT = 319,
     DOUBLE_LB = 320,
     DOUBLE_RB = 321,
     ADD_A = 322,
     SUB_A = 323,
     MUL_A = 324,
     DIV_A = 325,
     MOD_A = 326,
     SET = 327,
     GET = 328,
     ATTRIBUTE = 329,
     STRINGIFICATION = 330,
     NEWSELF = 331,
     DOT_EQUAL = 332,
     DOT_CAST = 333,
     DOT_EQUAL_RHS = 334,
     NUMBER_CONST = 335,
     STRING_CONST = 336,
     IDENT = 337,
     ATTRIBUTE_IDENT = 338,
     OPERATOR = 339,
     PARENTHESIS = 340,
     SQUARE_BRACKETS = 341,
     COMMENT = 342,
     INCOMPLETE_COMMENT = 343,
     INCOMPLETE_STRING = 344,
     CPP = 345,
     PARSE_STMT = 346,
     PARSE_FUNCTION_CALL_OBJECT = 347,
     PARSE_TABLE_OBJECT = 348,
     PARSE_NAMESPACE = 349,
     TOKEN_LAST = 350,
     UMINUS = 351
   };
#endif
/* Tokens.  */
#define TOKEN_FIRST 258
#define FUNCTION 259
#define RETURN 260
#define ASSERT 261
#define LAMBDA 262
#define LAMBDA_REF 263
#define ONEVENT 264
#define METHOD 265
#define SELF 266
#define ARGUMENTS 267
#define STATIC 268
#define CONST 269
#define KWD_IF 270
#define ELSE 271
#define WHILE 272
#define FOR 273
#define FOREACH 274
#define NIL 275
#define AND 276
#define NOT 277
#define OR 278
#define TRUE 279
#define FALSE 280
#define BREAK 281
#define CONTINUE 282
#define LOCAL 283
#define TRY 284
#define TRAP 285
#define THROW 286
#define USING 287
#define ADD 288
#define SUB 289
#define MUL 290
#define DIV 291
#define MOD 292
#define GT 293
#define LT 294
#define NE 295
#define EQ 296
#define GE 297
#define LE 298
#define ASSIGN 299
#define ADD_POSTFIX 300
#define SUB_POSTFIX 301
#define MUL_POSTFIX 302
#define DIV_POSTFIX 303
#define MOD_POSTFIX 304
#define GT_POSTFIX 305
#define LT_POSTFIX 306
#define NE_POSTFIX 307
#define EQ_POSTFIX 308
#define GE_POSTFIX 309
#define LE_POSTFIX 310
#define CAST 311
#define CALL 312
#define GLOBAL_SCOPE 313
#define PLUSPLUS 314
#define MINUSMINUS 315
#define DOT 316
#define DOT_ASSIGN 317
#define DOUBLE_DOT 318
#define TRIPLE_DOT 319
#define DOUBLE_LB 320
#define DOUBLE_RB 321
#define ADD_A 322
#define SUB_A 323
#define MUL_A 324
#define DIV_A 325
#define MOD_A 326
#define SET 327
#define GET 328
#define ATTRIBUTE 329
#define STRINGIFICATION 330
#define NEWSELF 331
#define DOT_EQUAL 332
#define DOT_CAST 333
#define DOT_EQUAL_RHS 334
#define NUMBER_CONST 335
#define STRING_CONST 336
#define IDENT 337
#define ATTRIBUTE_IDENT 338
#define OPERATOR 339
#define PARENTHESIS 340
#define SQUARE_BRACKETS 341
#define COMMENT 342
#define INCOMPLETE_COMMENT 343
#define INCOMPLETE_STRING 344
#define CPP 345
#define PARSE_STMT 346
#define PARSE_FUNCTION_CALL_OBJECT 347
#define PARSE_TABLE_OBJECT 348
#define PARSE_NAMESPACE 349
#define TOKEN_LAST 350
#define UMINUS 351




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 89 "DeltaParserSpec.y"
typedef union YYSTYPE {
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
	delta::FunctionNameASTNode*		funcNameValue;

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
	delta::TableIndexListASTNode*	tableIListVlaue;
	delta::TableConstructASTNode*	tableCtorValue;
	delta::UsingASTNode*			usingValue;
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 276 "DeltaParserGen.hpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif





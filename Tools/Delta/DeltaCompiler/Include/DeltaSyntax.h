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
     NUMBER_CONST = 258,
     STRING_CONST = 259,
     FUNCTION = 260,
     RETURN = 261,
     ONEVENT = 262,
     IF = 263,
     ELSE = 264,
     WHILE = 265,
     FOR = 266,
     FOREACH = 267,
     NIL = 268,
     LOCAL = 269,
     USING = 270,
     AND = 271,
     NOT = 272,
     OR = 273,
     PLUSPLUS = 274,
     MINUSMINUS = 275,
     LAMBDA = 276,
     TRY = 277,
     TRAP = 278,
     THROW = 279,
     ASSERT = 280,
     TRUE = 281,
     FALSE = 282,
     BREAK = 283,
     CONTINUE = 284,
     STATIC = 285,
     CONST = 286,
     METHOD = 287,
     SELF = 288,
     ARGUMENTS = 289,
     LAMBDA_REF = 290,
     OPERATOR = 291,
     NEWSELF = 292,
     SET = 293,
     GET = 294,
     LT = 295,
     GT = 296,
     LE = 297,
     GE = 298,
     EQ = 299,
     NE = 300,
     DOT = 301,
     DOUBLE_DOT = 302,
     TRIPLE_DOT = 303,
     DOT_ASSIGN = 304,
     DOT_EQUAL = 305,
     DOT_CAST = 306,
     DOT_EQUAL_RHS = 307,
     UMINUS = 308,
     ADD = 309,
     SUB = 310,
     MUL = 311,
     DIV = 312,
     MOD = 313,
     ATTRIBUTE = 314,
     STRINGIFY = 315,
     GLOBAL_SCOPE = 316,
     ASSIGN = 317,
     ADD_A = 318,
     SUB_A = 319,
     MUL_A = 320,
     DIV_A = 321,
     MOD_A = 322,
     DOUBLE_LB = 323,
     DOUBLE_RB = 324,
     CALL = 325,
     CAST = 326,
     SEMI = 327,
     IDENT = 328,
     ATTRIBUTE_IDENT = 329,
     ADD_POSTFIX = 330,
     SUB_POSTFIX = 331,
     MUL_POSTFIX = 332,
     DIV_POSTFIX = 333,
     MOD_POSTFIX = 334,
     GT_POSTFIX = 335,
     LT_POSTFIX = 336,
     NE_POSTFIX = 337,
     EQ_POSTFIX = 338,
     GE_POSTFIX = 339,
     LE_POSTFIX = 340,
     PARENTHESIS = 341,
     SQUARE_BRACKETS = 342,
     META_LSHIFT = 343,
     META_RSHIFT = 344,
     META_ESCAPE = 345,
     META_INLINE = 346,
     META_EXECUTE = 347,
     META_RENAME = 348,
     PARSE_QUOTED_ELEMENTS = 349
   };
#endif
/* Tokens.  */
#define NUMBER_CONST 258
#define STRING_CONST 259
#define FUNCTION 260
#define RETURN 261
#define ONEVENT 262
#define IF 263
#define ELSE 264
#define WHILE 265
#define FOR 266
#define FOREACH 267
#define NIL 268
#define LOCAL 269
#define USING 270
#define AND 271
#define NOT 272
#define OR 273
#define PLUSPLUS 274
#define MINUSMINUS 275
#define LAMBDA 276
#define TRY 277
#define TRAP 278
#define THROW 279
#define ASSERT 280
#define TRUE 281
#define FALSE 282
#define BREAK 283
#define CONTINUE 284
#define STATIC 285
#define CONST 286
#define METHOD 287
#define SELF 288
#define ARGUMENTS 289
#define LAMBDA_REF 290
#define OPERATOR 291
#define NEWSELF 292
#define SET 293
#define GET 294
#define LT 295
#define GT 296
#define LE 297
#define GE 298
#define EQ 299
#define NE 300
#define DOT 301
#define DOUBLE_DOT 302
#define TRIPLE_DOT 303
#define DOT_ASSIGN 304
#define DOT_EQUAL 305
#define DOT_CAST 306
#define DOT_EQUAL_RHS 307
#define UMINUS 308
#define ADD 309
#define SUB 310
#define MUL 311
#define DIV 312
#define MOD 313
#define ATTRIBUTE 314
#define STRINGIFY 315
#define GLOBAL_SCOPE 316
#define ASSIGN 317
#define ADD_A 318
#define SUB_A 319
#define MUL_A 320
#define DIV_A 321
#define MOD_A 322
#define DOUBLE_LB 323
#define DOUBLE_RB 324
#define CALL 325
#define CAST 326
#define SEMI 327
#define IDENT 328
#define ATTRIBUTE_IDENT 329
#define ADD_POSTFIX 330
#define SUB_POSTFIX 331
#define MUL_POSTFIX 332
#define DIV_POSTFIX 333
#define MOD_POSTFIX 334
#define GT_POSTFIX 335
#define LT_POSTFIX 336
#define NE_POSTFIX 337
#define EQ_POSTFIX 338
#define GE_POSTFIX 339
#define LE_POSTFIX 340
#define PARENTHESIS 341
#define SQUARE_BRACKETS 342
#define META_LSHIFT 343
#define META_RSHIFT 344
#define META_ESCAPE 345
#define META_INLINE 346
#define META_EXECUTE 347
#define META_RENAME 348
#define PARSE_QUOTED_ELEMENTS 349




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
	double				numberConst;
	util_ui32			line;
	util_ui32			count;
	char*				dynamicStr;
	const char*			constStr;
	AST::NodeList*		nodeList;
	AST::Node*			node;
} YYSTYPE;
/* Line 1447 of yacc.c.  */

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





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
     IDENT = 258,
     ATTRIBUTE_IDENT = 259,
     NUMBER_CONST = 260,
     STRING_CONST = 261,
     FUNCTION = 262,
     RETURN = 263,
     ONEVENT = 264,
     IF = 265,
     ELSE = 266,
     WHILE = 267,
     FOR = 268,
     FOREACH = 269,
     NIL = 270,
     LOCAL = 271,
     AND = 272,
     NOT = 273,
     OR = 274,
     LAMBDA = 275,
     TRY = 276,
     TRAP = 277,
     THROW = 278,
     USING = 279,
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
     ASSIGN = 290,
     ADD_A = 291,
     SUB_A = 292,
     MUL_A = 293,
     DIV_A = 294,
     MOD_A = 295,
     DOUBLE_LB = 296,
     DOUBLE_RB = 297,
     SEMI = 298,
     UMINUS = 299,
     MINUSMINUS = 300,
     LT = 301,
     GT = 302,
     LE = 303,
     GE = 304,
     EQ = 305,
     NE = 306,
     DOT = 307,
     DOUBLE_DOT = 308,
     TRIPLE_DOT = 309,
     DOT_ASSIGN = 310,
     GLOBAL_SCOPE = 311,
     ADD = 312,
     SUB = 313,
     MUL = 314,
     DIV = 315,
     MOD = 316,
     ATTRIBUTE = 317,
     STRINGIFY = 318,
     PLUSPLUS = 319,
     PARENTHESIS = 320,
     SQUARE_BRACKETS = 321,
     CALL = 322,
     CAST = 323,
     DOT_EQUAL = 324,
     DOT_CAST = 325,
     DOT_EQUAL_RHS = 326,
     ADD_POSTFIX = 327,
     SUB_POSTFIX = 328,
     MUL_POSTFIX = 329,
     DIV_POSTFIX = 330,
     MOD_POSTFIX = 331,
     GT_POSTFIX = 332,
     LT_POSTFIX = 333,
     NE_POSTFIX = 334,
     EQ_POSTFIX = 335,
     GE_POSTFIX = 336,
     LE_POSTFIX = 337,
     OPERATOR = 338,
     NEWSELF = 339,
     SET = 340,
     GET = 341,
     LAMBDA_REF = 342
   };
#endif
/* Tokens.  */
#define IDENT 258
#define ATTRIBUTE_IDENT 259
#define NUMBER_CONST 260
#define STRING_CONST 261
#define FUNCTION 262
#define RETURN 263
#define ONEVENT 264
#define IF 265
#define ELSE 266
#define WHILE 267
#define FOR 268
#define FOREACH 269
#define NIL 270
#define LOCAL 271
#define AND 272
#define NOT 273
#define OR 274
#define LAMBDA 275
#define TRY 276
#define TRAP 277
#define THROW 278
#define USING 279
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
#define ASSIGN 290
#define ADD_A 291
#define SUB_A 292
#define MUL_A 293
#define DIV_A 294
#define MOD_A 295
#define DOUBLE_LB 296
#define DOUBLE_RB 297
#define SEMI 298
#define UMINUS 299
#define MINUSMINUS 300
#define LT 301
#define GT 302
#define LE 303
#define GE 304
#define EQ 305
#define NE 306
#define DOT 307
#define DOUBLE_DOT 308
#define TRIPLE_DOT 309
#define DOT_ASSIGN 310
#define GLOBAL_SCOPE 311
#define ADD 312
#define SUB 313
#define MUL 314
#define DIV 315
#define MOD 316
#define ATTRIBUTE 317
#define STRINGIFY 318
#define PLUSPLUS 319
#define PARENTHESIS 320
#define SQUARE_BRACKETS 321
#define CALL 322
#define CAST 323
#define DOT_EQUAL 324
#define DOT_CAST 325
#define DOT_EQUAL_RHS 326
#define ADD_POSTFIX 327
#define SUB_POSTFIX 328
#define MUL_POSTFIX 329
#define DIV_POSTFIX 330
#define MOD_POSTFIX 331
#define GT_POSTFIX 332
#define LT_POSTFIX 333
#define NE_POSTFIX 334
#define EQ_POSTFIX 335
#define GE_POSTFIX 336
#define LE_POSTFIX 337
#define OPERATOR 338
#define NEWSELF 339
#define SET 340
#define GET 341
#define LAMBDA_REF 342




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
	const char*				id;
	DeltaNumberValueType	numberConst;
	char*					strConst;
	int						intVal;
	DeltaExpr*				expr;
	DeltaSymbol*			sym;
	TableElements*			table;
	DeltaQuadAddress		quadNo;
	Stmt*					stmt;
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





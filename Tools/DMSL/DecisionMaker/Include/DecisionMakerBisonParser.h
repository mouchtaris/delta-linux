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
     INT_VALUE = 258,
     REAL_VALUE = 259,
     BOOL_VALUE = 260,
     STRING_VALUE = 261,
     ID_VALUE = 262,
     IF = 263,
     THEN = 264,
     ELSE = 265,
     CASE = 266,
     WHEN = 267,
     OTHERWISE = 268,
     ACTIVATE = 269,
     CANCEL = 270,
     EVALUATE = 271,
     LIBRARY = 272,
     IS_ACTIVE = 273,
     TO_NUMBER = 274,
     HAS_ATTR = 275,
     RANDOM = 276,
     CAN_SUPPORT = 277,
     COMPONENT = 278,
     DMSL_IN = 279,
     PARAMS = 280,
     STEREOTYPE = 281,
     DEFINE = 282,
     OR = 283,
     AND = 284,
     EQ = 285,
     NEQ = 286,
     GT = 287,
     LT = 288,
     GE = 289,
     LE = 290,
     NOT = 291,
     DOUBLEDOT = 292,
     UNARY = 293,
     DMSL_ERROR = 294
   };
#endif
/* Tokens.  */
#define INT_VALUE 258
#define REAL_VALUE 259
#define BOOL_VALUE 260
#define STRING_VALUE 261
#define ID_VALUE 262
#define IF 263
#define THEN 264
#define ELSE 265
#define CASE 266
#define WHEN 267
#define OTHERWISE 268
#define ACTIVATE 269
#define CANCEL 270
#define EVALUATE 271
#define LIBRARY 272
#define IS_ACTIVE 273
#define TO_NUMBER 274
#define HAS_ATTR 275
#define RANDOM 276
#define CAN_SUPPORT 277
#define COMPONENT 278
#define DMSL_IN 279
#define PARAMS 280
#define STEREOTYPE 281
#define DEFINE 282
#define OR 283
#define AND 284
#define EQ 285
#define NEQ 286
#define GT 287
#define LT 288
#define GE 289
#define LE 290
#define NOT 291
#define DOUBLEDOT 292
#define UNARY 293
#define DMSL_ERROR 294




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 69 "Src/DecisionMakerParser.y"
typedef union YYSTYPE {
  int 			intVal;
  bool 			boolVal;
  double 		realVal;
  const char*	stringVal;
  Expression*	expr;
  ExprList*		exprList;
  Statement*	stmt;
  StmtList*		stmtList;
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 127 "Src/DecisionMakerBisonParser.hpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif






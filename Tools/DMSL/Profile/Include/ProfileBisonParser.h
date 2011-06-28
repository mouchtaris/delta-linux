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
     DOUBLEDOT = 262,
     PROFILE_ERROR = 263
   };
#endif
/* Tokens.  */
#define INT_VALUE 258
#define REAL_VALUE 259
#define BOOL_VALUE 260
#define STRING_VALUE 261
#define DOUBLEDOT 262
#define PROFILE_ERROR 263




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 47 "Src/ProfileParser.y"
typedef union YYSTYPE {
  int 				intVal;
  bool 				boolVal;
  double 			realVal;
  const char*		stringVal;
  Pair*				pair;
  PairMap*			pairMap;
  ExprValue*		value;
  ExprValueList*	valueList;
  Range*			range;
  RangeList*		rangeList;
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 67 "Src/ProfileBisonParser.hpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif






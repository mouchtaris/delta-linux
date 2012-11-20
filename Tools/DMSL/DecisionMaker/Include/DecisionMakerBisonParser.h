
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IF = 258,
     THEN = 259,
     ELSE = 260,
     CASE = 261,
     WHEN = 262,
     OTHERWISE = 263,
     ACTIVATE = 264,
     CANCEL = 265,
     EVALUATE = 266,
     LIBRARY = 267,
     IS_ACTIVE = 268,
     TO_NUMBER = 269,
     HAS_ATTR = 270,
     RANDOM = 271,
     CAN_SUPPORT = 272,
     COMPONENT = 273,
     DMSL_IN = 274,
     PARAMS = 275,
     STEREOTYPE = 276,
     DEFINE = 277,
     EQ = 278,
     NEQ = 279,
     GT = 280,
     LT = 281,
     GE = 282,
     LE = 283,
     AND = 284,
     OR = 285,
     NOT = 286,
     DOUBLEDOT = 287,
     UNARY = 288,
     DMSL_ERROR = 289,
     INT_VALUE = 290,
     REAL_VALUE = 291,
     STRING_VALUE = 292,
     ID_VALUE = 293,
     BOOL_VALUE = 294
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 72 "Src/DecisionMakerParser.y"

  int 			intVal;
  bool 			boolVal;
  double 		realVal;
  const char*	stringVal;
  Expression*	expr;
  ExprList*		exprList;
  Statement*	stmt;
  StmtList*		stmtList;
  unsigned		line;



/* Line 1676 of yacc.c  */
#line 105 "Src/DecisionMakerBisonParser.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif





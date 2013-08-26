
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
     BOOLEAN = 258,
     NUMBER = 259,
     STRING = 260,
     IDENT = 261,
     AST = 262,
     ATTRIBUTE = 263,
     CHILD = 264,
     PARENT = 265,
     DESCENDANT = 266,
     ASCENDANT = 267,
     EXECUTION = 268,
     CALL = 269,
     EXCEPTION = 270,
     CLASS = 271,
     SETTER = 272,
     GETTER = 273,
     FUNCTION = 274,
     ONEVENT = 275,
     METHOD = 276,
     DOUBLE_DOT = 277,
     TRIPLE_DOT = 278,
     AND = 279,
     NOT = 280,
     OR = 281,
     TRUE = 282,
     FALSE = 283,
     SELF = 284,
     ARGUMENTS = 285,
     OPERATOR = 286,
     NEWSELF = 287,
     LAMBDA_REF = 288,
     DOT = 289,
     DOT_ASSIGN = 290,
     CALLOP = 291,
     CAST = 292,
     ADD = 293,
     SUB = 294,
     MUL = 295,
     DIV = 296,
     MOD = 297,
     LT = 298,
     GT = 299,
     LE = 300,
     GE = 301,
     EQ = 302,
     NE = 303,
     ATTRIBUTEOP = 304,
     ATTRIBUTE_IDENT = 305,
     ASSIGN = 306,
     ADD_A = 307,
     SUB_A = 308,
     MUL_A = 309,
     DIV_A = 310,
     MOD_A = 311,
     DOUBLE_LB = 312,
     DOUBLE_RB = 313,
     ADD_POSTFIX = 314,
     SUB_POSTFIX = 315,
     MUL_POSTFIX = 316,
     DIV_POSTFIX = 317,
     MOD_POSTFIX = 318,
     GT_POSTFIX = 319,
     LT_POSTFIX = 320,
     NE_POSTFIX = 321,
     EQ_POSTFIX = 322,
     GE_POSTFIX = 323,
     LE_POSTFIX = 324
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{


	bool 			boolVal;
	double 			numberVal;
	const char*		stringVal;
	Pointcut*		pointcut;



} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
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




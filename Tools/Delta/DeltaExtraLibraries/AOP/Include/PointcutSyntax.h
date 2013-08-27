
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
     ATTRIBUTE_IDENT = 262,
     AST = 263,
     ATTRIBUTE = 264,
     CHILD = 265,
     PARENT = 266,
     DESCENDANT = 267,
     ASCENDANT = 268,
     EXECUTION = 269,
     CALL = 270,
     EXCEPTION = 271,
     CLASS = 272,
     SETTER = 273,
     GETTER = 274,
     FUNCTION = 275,
     ONEVENT = 276,
     METHOD = 277,
     DOUBLE_DOT = 278,
     TRIPLE_DOT = 279,
     AND = 280,
     NOT = 281,
     OR = 282,
     TRUE = 283,
     FALSE = 284,
     SELF = 285,
     ARGUMENTS = 286,
     OPERATOR = 287,
     NEWSELF = 288,
     LAMBDA_REF = 289,
     DOT = 290,
     DOT_ASSIGN = 291,
     CALLOP = 292,
     CAST = 293,
     ADD = 294,
     SUB = 295,
     MUL = 296,
     DIV = 297,
     MOD = 298,
     LT = 299,
     GT = 300,
     LE = 301,
     GE = 302,
     EQ = 303,
     NE = 304,
     ATTRIBUTEOP = 305,
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
	IdList*			idList;



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




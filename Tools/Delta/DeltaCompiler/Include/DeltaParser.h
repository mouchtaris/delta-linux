
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



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{


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




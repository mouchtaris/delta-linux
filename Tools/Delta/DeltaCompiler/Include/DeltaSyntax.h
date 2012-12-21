
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



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{


	double				numberConst;
	util_ui32			line;
	util_ui32			count;
	char*				dynamicStr;
	const char*			constStr;
	AST::NodeList*		nodeList;
	AST::Node*			node;



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




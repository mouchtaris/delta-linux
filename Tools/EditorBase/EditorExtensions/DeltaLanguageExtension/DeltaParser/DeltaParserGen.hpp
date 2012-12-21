
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
     META_LSHIFT = 335,
     META_RSHIFT = 336,
     META_ESCAPE = 337,
     META_INLINE = 338,
     META_EXECUTE = 339,
     META_RENAME = 340,
     NUMBER_CONST = 341,
     STRING_CONST = 342,
     IDENT = 343,
     ATTRIBUTE_IDENT = 344,
     OPERATOR = 345,
     PARENTHESIS = 346,
     SQUARE_BRACKETS = 347,
     COMMENT = 348,
     INCOMPLETE_COMMENT = 349,
     INCOMPLETE_STRING = 350,
     CPP = 351,
     PARSE_STMT = 352,
     PARSE_FUNCTION_CALL_OBJECT = 353,
     PARSE_TABLE_OBJECT = 354,
     PARSE_NAMESPACE = 355,
     TOKEN_LAST = 356,
     UMINUS = 357
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 102 "DeltaParserSpec.y"

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
	delta::TableIndexListASTNode*	tableIListValue;
	delta::TableConstructASTNode*	tableCtorValue;
	delta::UsingASTNode*			usingValue;
	
	delta::QuotedElementsASTNode*	elementsValue;



/* Line 1676 of yacc.c  */
#line 203 "DeltaParserGen.hpp"
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




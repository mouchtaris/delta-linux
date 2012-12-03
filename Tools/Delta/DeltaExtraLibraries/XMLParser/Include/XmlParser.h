
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
     NAME = 258,
     ATTVALUE = 259,
     REFERENCE = 260,
     CDSECT = 261,
     CHARDATA = 262,
     ENTITYVALUE = 263,
     LT = 264,
     GT = 265,
     LTSLASH = 266,
     SLASHGT = 267,
     PERCENT = 268,
     LTQMXML = 269,
     QMGT = 270,
     S = 271,
     PI = 272,
     EQ = 273,
     COMMENT = 274,
     DOCTYPE = 275,
     EXTERNALID = 276,
     LBRACKET = 277,
     RBRACKET = 278,
     PEREFERENCE = 279,
     ELEMENTDECL = 280,
     EMPTY = 281,
     ANY = 282,
     MIXED = 283,
     ATTLIST = 284,
     DEFAULTDECL = 285,
     STRINGTYPE = 286,
     TOKENIZEDTYPE = 287,
     NOTATIONTYPE = 288,
     ENUMERATION = 289,
     ENTITY = 290,
     NDATADECL = 291,
     NOTATION = 292,
     PUBLICID = 293,
     VERSIONINFO = 294,
     ENCODINGDECL = 295,
     SDDECL = 296
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 32 "XmlParser.y"

	std::string*		strValue;
	DeltaValue*		obj;
	DeltaObject*		proto;
	TagValue*			tag;
	XmlElement*			element;



/* Line 1676 of yacc.c  */
#line 103 "XmlParser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;



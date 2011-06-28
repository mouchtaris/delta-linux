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
/* Tokens.  */
#define NAME 258
#define ATTVALUE 259
#define REFERENCE 260
#define CDSECT 261
#define CHARDATA 262
#define ENTITYVALUE 263
#define LT 264
#define GT 265
#define LTSLASH 266
#define SLASHGT 267
#define PERCENT 268
#define LTQMXML 269
#define QMGT 270
#define S 271
#define PI 272
#define EQ 273
#define COMMENT 274
#define DOCTYPE 275
#define EXTERNALID 276
#define LBRACKET 277
#define RBRACKET 278
#define PEREFERENCE 279
#define ELEMENTDECL 280
#define EMPTY 281
#define ANY 282
#define MIXED 283
#define ATTLIST 284
#define DEFAULTDECL 285
#define STRINGTYPE 286
#define TOKENIZEDTYPE 287
#define NOTATIONTYPE 288
#define ENUMERATION 289
#define ENTITY 290
#define NDATADECL 291
#define NOTATION 292
#define PUBLICID 293
#define VERSIONINFO 294
#define ENCODINGDECL 295
#define SDDECL 296




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 32 "XmlParser.y"
typedef union YYSTYPE {
	std::string*		strValue;
	DeltaValue*		obj;
	DeltaObject*		proto;
	TagValue*			tag;
	XmlElement*			element;
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 128 "XmlParser.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;




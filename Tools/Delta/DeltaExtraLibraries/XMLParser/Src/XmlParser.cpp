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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 5 "XmlParser.y"

	//Includes and function signatures in here
	#include <iostream>
	#include <string>
	#include <list>
	#include "DeltaObject.h"
	#include "DeltaValue.h"
	#include "XmlParseActions.h"
	#include "TagValue.h"
	#include "XmlElement.h"
	#include "XmlParser.h"
	#include "XmlLoaderErrorMsg.h"

	using namespace XmlLoaderActions;

	#define yylex XmlLex_yylex
	#define	yyparse	XmlLex_yyparse
	#define yytext XmlLex_yytext

	extern char *yytext;
	extern int yylex();
	int yyerror (DeltaObject **return_value, std::string yaccProvidedMessage);


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 32 "XmlParser.y"
typedef union YYSTYPE {
	std::string*		strValue;
	DeltaValue*		obj;
	DeltaObject*		proto;
	TagValue*			tag;
	XmlElement*			element;
} YYSTYPE;
/* Line 196 of yacc.c.  */
#line 199 "XmlParser.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */
#line 211 "XmlParser.tab.c"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   102

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  35
/* YYNRULES -- Number of rules. */
#define YYNRULES  75
/* YYNRULES -- Number of states. */
#define YYNSTATES  134

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   296

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     7,    10,    11,    15,    22,    23,    25,
      26,    28,    29,    32,    33,    40,    44,    46,    48,    49,
      54,    55,    58,    61,    62,    64,    66,    68,    70,    72,
      74,    82,    84,    86,    88,    94,    97,    99,   106,   108,
     110,   112,   114,   116,   118,   120,   128,   130,   133,   135,
     145,   147,   149,   157,   165,   167,   169,   171,   173,   175,
     179,   181,   186,   192,   194,   195,   199,   202,   205,   208,
     211,   214,   217,   220,   221,   224
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      43,     0,    -1,    45,    71,    44,    -1,    70,    44,    -1,
      -1,    46,    44,    49,    -1,    14,    39,    47,    48,    52,
      15,    -1,    -1,    40,    -1,    -1,    41,    -1,    -1,    50,
      44,    -1,    -1,    20,    16,     3,    51,    53,    10,    -1,
      16,    21,    52,    -1,    52,    -1,    16,    -1,    -1,    22,
      54,    23,    52,    -1,    -1,    54,    55,    -1,    54,    69,
      -1,    -1,    56,    -1,    58,    -1,    63,    -1,    68,    -1,
      17,    -1,    19,    -1,    25,    16,     3,    16,    57,    52,
      10,    -1,    26,    -1,    27,    -1,    28,    -1,    29,    16,
       3,    59,    10,    -1,    60,    59,    -1,    52,    -1,    16,
       3,    16,    61,    16,    30,    -1,    31,    -1,    32,    -1,
      62,    -1,    33,    -1,    34,    -1,    64,    -1,    66,    -1,
      35,    16,     3,    16,    65,    52,    10,    -1,     8,    -1,
      21,    36,    -1,    21,    -1,    35,    16,    13,    16,     3,
      16,    67,    52,    10,    -1,     8,    -1,    21,    -1,    37,
      16,     3,    16,    21,    52,    10,    -1,    37,    16,     3,
      16,    38,    52,    10,    -1,    24,    -1,    16,    -1,    19,
      -1,    17,    -1,    16,    -1,    72,    75,    74,    -1,    76,
      -1,     9,     3,    73,    10,    -1,    16,     3,    18,     4,
      73,    -1,    16,    -1,    -1,    11,     3,    10,    -1,    75,
      71,    -1,    75,     5,    -1,    75,     6,    -1,    75,    17,
      -1,    75,    19,    -1,    75,     7,    -1,    75,    16,    -1,
      -1,    75,     1,    -1,     9,     3,    73,    12,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    56,    56,    72,    73,    76,    79,    80,    83,    84,
      87,    88,    91,    92,    95,    97,    98,   101,   102,   105,
     106,   109,   110,   111,   114,   115,   116,   117,   118,   119,
     122,   125,   126,   127,   130,   133,   134,   137,   140,   141,
     142,   145,   146,   149,   150,   153,   156,   157,   158,   161,
     164,   165,   168,   169,   172,   173,   176,   177,   178,   181,
     184,   189,   194,   197,   200,   205,   210,   213,   216,   219,
     222,   225,   228,   231,   234,   242
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAME", "ATTVALUE", "REFERENCE",
  "CDSECT", "CHARDATA", "ENTITYVALUE", "LT", "GT", "LTSLASH", "SLASHGT",
  "PERCENT", "LTQMXML", "QMGT", "S", "PI", "EQ", "COMMENT", "DOCTYPE",
  "EXTERNALID", "LBRACKET", "RBRACKET", "PEREFERENCE", "ELEMENTDECL",
  "EMPTY", "ANY", "MIXED", "ATTLIST", "DEFAULTDECL", "STRINGTYPE",
  "TOKENIZEDTYPE", "NOTATIONTYPE", "ENUMERATION", "ENTITY", "NDATADECL",
  "NOTATION", "PUBLICID", "VERSIONINFO", "ENCODINGDECL", "SDDECL",
  "$accept", "document", "MiscList", "prolog", "XmlDeclQM",
  "EncodingDeclQM", "SDDeclQM", "prologDecl", "doctypeDecl",
  "ExternalIDQM", "SQM", "IntSubsetQM", "IntSubset", "MarkupDecl",
  "ElementDecl", "ContentSpec", "AttlistDecl", "AttDefs", "AttDef",
  "AttType", "EnumeratedType", "EntityDecl", "GEDecl", "EntityDef",
  "PEDecl", "PEDef", "NotationDecl", "DeclSep", "Misc", "element", "STag",
  "AttList", "ETag", "content", "EmptyElemTag", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    42,    43,    44,    44,    45,    46,    46,    47,    47,
      48,    48,    49,    49,    50,    51,    51,    52,    52,    53,
      53,    54,    54,    54,    55,    55,    55,    55,    55,    55,
      56,    57,    57,    57,    58,    59,    59,    60,    61,    61,
      61,    62,    62,    63,    63,    64,    65,    65,    65,    66,
      67,    67,    68,    68,    69,    69,    70,    70,    70,    71,
      71,    72,    73,    73,    73,    74,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    76
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     3,     2,     0,     3,     6,     0,     1,     0,
       1,     0,     2,     0,     6,     3,     1,     1,     0,     4,
       0,     2,     2,     0,     1,     1,     1,     1,     1,     1,
       7,     1,     1,     1,     5,     2,     1,     6,     1,     1,
       1,     1,     1,     1,     1,     7,     1,     2,     1,     9,
       1,     1,     7,     7,     1,     1,     1,     1,     1,     3,
       1,     4,     5,     1,     0,     3,     2,     2,     2,     2,
       2,     2,     2,     0,     2,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       7,     0,     0,     0,     4,     9,     1,     0,     4,    73,
      60,    58,    57,    56,    13,     4,     8,    11,    64,     2,
       0,     0,     5,     4,     3,    10,    18,    63,     0,    74,
      67,    68,    71,     0,    72,    69,    70,    66,    59,     0,
      12,    17,     0,     0,    61,    75,     0,    18,     6,     0,
      65,    17,    20,    16,    64,    18,    23,     0,    62,    15,
       0,    14,    55,    28,    29,    18,    54,     0,     0,     0,
       0,    21,    24,    25,    26,    43,    44,    27,    22,    19,
       0,     0,     0,     0,     0,    18,     0,     0,     0,     0,
      17,    36,     0,    18,     0,     0,     0,    31,    32,    33,
      18,     0,    34,    35,    46,    48,    18,     0,    18,    18,
       0,     0,    47,     0,     0,     0,     0,    30,    38,    39,
      41,    42,     0,    40,    45,    50,    51,    18,    52,    53,
       0,     0,    37,    49
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     2,    14,     3,     4,    17,    26,    22,    23,    52,
      91,    57,    60,    71,    72,   100,    73,    92,    93,   122,
     123,    74,    75,   106,    76,   127,    77,    78,    15,     8,
       9,    28,    38,    20,    10
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -29
static const yysigned_char yypact[] =
{
      -8,   -28,    28,    16,    21,    -7,   -29,    32,    21,   -29,
     -29,   -29,   -29,   -29,    33,    21,   -29,     2,    36,   -29,
      25,    38,   -29,    21,   -29,   -29,    39,    53,     5,   -29,
     -29,   -29,   -29,    54,   -29,   -29,   -29,   -29,   -29,    55,
     -29,   -29,    44,    42,   -29,   -29,    51,    46,   -29,    59,
     -29,    43,    45,   -29,    36,    39,   -29,    56,   -29,   -29,
     -15,   -29,   -29,   -29,   -29,    39,   -29,    49,    52,    57,
      60,   -29,   -29,   -29,   -29,   -29,   -29,   -29,   -29,   -29,
      66,    67,    10,    68,    61,    62,    63,    65,    69,    23,
      72,   -29,    74,    62,    -5,    83,   -14,   -29,   -29,   -29,
      39,    71,   -29,   -29,   -29,    58,    39,    73,    39,    39,
      78,    14,   -29,    80,    -3,    81,    82,   -29,   -29,   -29,
     -29,   -29,    77,   -29,   -29,   -29,   -29,    39,   -29,   -29,
      70,    85,   -29,   -29
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -29,   -29,     4,   -29,   -29,   -29,   -29,   -29,   -29,   -29,
     -26,   -29,   -29,   -29,   -29,   -29,   -29,   -21,   -29,   -29,
     -29,   -29,   -29,   -29,   -29,   -29,   -29,   -29,   -29,    76,
     -29,    48,   -29,   -29,   -29
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      42,    62,    63,   104,    64,   125,     1,   108,    65,    66,
      67,     5,    19,    86,    68,    44,   105,    45,   126,    24,
      69,    53,    70,    87,   109,     7,    29,    40,     6,    59,
      30,    31,    32,    16,     7,    18,    33,    11,    12,    79,
      13,    34,    35,    25,    36,   118,   119,   120,   121,    97,
      98,    99,    27,    21,    39,    41,    43,    46,    47,    48,
      49,    50,    51,    54,    55,    80,    61,    56,    81,    84,
      85,    88,   103,    82,   110,   101,    83,    89,    90,    94,
     113,    95,   115,   116,   102,    96,   107,   111,   117,   114,
     124,   128,   129,   130,   112,   133,    37,     0,     0,     0,
     132,   131,    58
};

static const yysigned_char yycheck[] =
{
      26,    16,    17,     8,    19,     8,    14,    21,    23,    24,
      25,    39,     8,     3,    29,    10,    21,    12,    21,    15,
      35,    47,    37,    13,    38,     9,     1,    23,     0,    55,
       5,     6,     7,    40,     9,     3,    11,    16,    17,    65,
      19,    16,    17,    41,    19,    31,    32,    33,    34,    26,
      27,    28,    16,    20,    16,    16,     3,     3,     3,    15,
      18,    10,    16,     4,    21,    16,    10,    22,    16,     3,
       3,     3,    93,    16,   100,     3,    16,    16,    16,    16,
     106,    16,   108,   109,    10,    16,     3,    16,    10,    16,
      10,    10,    10,    16,    36,    10,    20,    -1,    -1,    -1,
      30,   127,    54
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    14,    43,    45,    46,    39,     0,     9,    71,    72,
      76,    16,    17,    19,    44,    70,    40,    47,     3,    44,
      75,    20,    49,    50,    44,    41,    48,    16,    73,     1,
       5,     6,     7,    11,    16,    17,    19,    71,    74,    16,
      44,    16,    52,     3,    10,    12,     3,     3,    15,    18,
      10,    16,    51,    52,     4,    21,    22,    53,    73,    52,
      54,    10,    16,    17,    19,    23,    24,    25,    29,    35,
      37,    55,    56,    58,    63,    64,    66,    68,    69,    52,
      16,    16,    16,    16,     3,     3,     3,    13,     3,    16,
      16,    52,    59,    60,    16,    16,    16,    26,    27,    28,
      57,     3,    10,    59,     8,    21,    65,     3,    21,    38,
      52,    16,    36,    52,    16,    52,    52,    10,    31,    32,
      33,    34,    61,    62,    10,     8,    21,    67,    10,    10,
      16,    52,    30,    10
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (return_value, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (DeltaObject **return_value);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (DeltaObject **return_value)
#else
int
yyparse (return_value)
    DeltaObject **return_value;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 56 "XmlParser.y"
    {
																			if ((yyvsp[-1].element) == NULL) {
																				*return_value = (DeltaObject*) 0;
																			} else {
																				DeltaValue index, value;
																				index.FromString("$Name");
																				value.FromString(*((yyvsp[-1].element)->GetName()));
																				
																				*return_value = (yyvsp[-1].element)->GetContent();
																				DPTR((*return_value))->Set(index, value);

																				DDELETE((yyvsp[-1].element));
																			}
																		;}
    break;

  case 3:
#line 72 "XmlParser.y"
    { ;}
    break;

  case 4:
#line 73 "XmlParser.y"
    { ;}
    break;

  case 5:
#line 76 "XmlParser.y"
    { ;}
    break;

  case 6:
#line 79 "XmlParser.y"
    { ;}
    break;

  case 7:
#line 80 "XmlParser.y"
    { ;}
    break;

  case 8:
#line 83 "XmlParser.y"
    { ;}
    break;

  case 9:
#line 84 "XmlParser.y"
    { ;}
    break;

  case 10:
#line 87 "XmlParser.y"
    { ;}
    break;

  case 11:
#line 88 "XmlParser.y"
    { ;}
    break;

  case 12:
#line 91 "XmlParser.y"
    { ;}
    break;

  case 13:
#line 92 "XmlParser.y"
    { ;}
    break;

  case 14:
#line 95 "XmlParser.y"
    { ;}
    break;

  case 15:
#line 97 "XmlParser.y"
    { ;}
    break;

  case 16:
#line 98 "XmlParser.y"
    { ;}
    break;

  case 17:
#line 101 "XmlParser.y"
    { ;}
    break;

  case 18:
#line 102 "XmlParser.y"
    { ;}
    break;

  case 19:
#line 105 "XmlParser.y"
    { ;}
    break;

  case 20:
#line 106 "XmlParser.y"
    { ;}
    break;

  case 21:
#line 109 "XmlParser.y"
    { ;}
    break;

  case 22:
#line 110 "XmlParser.y"
    { ;}
    break;

  case 23:
#line 111 "XmlParser.y"
    { ;}
    break;

  case 24:
#line 114 "XmlParser.y"
    { ;}
    break;

  case 25:
#line 115 "XmlParser.y"
    { ;}
    break;

  case 26:
#line 116 "XmlParser.y"
    { ;}
    break;

  case 27:
#line 117 "XmlParser.y"
    { ;}
    break;

  case 28:
#line 118 "XmlParser.y"
    { ;}
    break;

  case 29:
#line 119 "XmlParser.y"
    { ;}
    break;

  case 30:
#line 122 "XmlParser.y"
    { ;}
    break;

  case 31:
#line 125 "XmlParser.y"
    { ;}
    break;

  case 32:
#line 126 "XmlParser.y"
    { ;}
    break;

  case 33:
#line 127 "XmlParser.y"
    { ;}
    break;

  case 34:
#line 130 "XmlParser.y"
    { ;}
    break;

  case 35:
#line 133 "XmlParser.y"
    { ;}
    break;

  case 36:
#line 134 "XmlParser.y"
    { ;}
    break;

  case 37:
#line 137 "XmlParser.y"
    { ;}
    break;

  case 38:
#line 140 "XmlParser.y"
    { ;}
    break;

  case 39:
#line 141 "XmlParser.y"
    { ;}
    break;

  case 40:
#line 142 "XmlParser.y"
    { ;}
    break;

  case 41:
#line 145 "XmlParser.y"
    { ;}
    break;

  case 42:
#line 146 "XmlParser.y"
    { ;}
    break;

  case 43:
#line 149 "XmlParser.y"
    { ;}
    break;

  case 44:
#line 150 "XmlParser.y"
    { ;}
    break;

  case 45:
#line 153 "XmlParser.y"
    { ;}
    break;

  case 46:
#line 156 "XmlParser.y"
    { ;}
    break;

  case 47:
#line 157 "XmlParser.y"
    { ;}
    break;

  case 48:
#line 158 "XmlParser.y"
    { ;}
    break;

  case 49:
#line 161 "XmlParser.y"
    { ;}
    break;

  case 50:
#line 164 "XmlParser.y"
    { ;}
    break;

  case 51:
#line 165 "XmlParser.y"
    { ;}
    break;

  case 52:
#line 168 "XmlParser.y"
    { ;}
    break;

  case 53:
#line 169 "XmlParser.y"
    { ;}
    break;

  case 54:
#line 172 "XmlParser.y"
    { ;}
    break;

  case 55:
#line 173 "XmlParser.y"
    { ;}
    break;

  case 56:
#line 176 "XmlParser.y"
    { ;}
    break;

  case 57:
#line 177 "XmlParser.y"
    { ;}
    break;

  case 58:
#line 178 "XmlParser.y"
    { ;}
    break;

  case 59:
#line 181 "XmlParser.y"
    {
																			(yyval.element) = Manage_ElementWithContent((yyvsp[-2].tag), (yyvsp[-1].proto), (yyvsp[0].strValue));
																		;}
    break;

  case 60:
#line 184 "XmlParser.y"
    {
																			(yyval.element) = Manage_ElementWithoutContent((yyvsp[0].tag));
																		;}
    break;

  case 61:
#line 189 "XmlParser.y"
    {
																			(yyval.tag) = Manage_Stag((yyvsp[-2].strValue), (yyvsp[-1].tag));
																		;}
    break;

  case 62:
#line 194 "XmlParser.y"
    {
																			(yyval.tag) = Manage_Attlist((yyvsp[-3].strValue), (yyvsp[-1].strValue), (yyvsp[0].tag));
																		;}
    break;

  case 63:
#line 197 "XmlParser.y"
    {
																			(yyval.tag) = Manage_AttlistSpaces();
																		;}
    break;

  case 64:
#line 200 "XmlParser.y"
    {
																			(yyval.tag) = Manage_AttlistNothing();
																		;}
    break;

  case 65:
#line 205 "XmlParser.y"
    {
																			(yyval.strValue) = (yyvsp[-1].strValue);
																		;}
    break;

  case 66:
#line 210 "XmlParser.y"
    {
																			(yyval.proto) = Manage_ContentElement((yyvsp[-1].proto), (yyvsp[0].element));
																		;}
    break;

  case 67:
#line 213 "XmlParser.y"
    {
																			(yyval.proto) = Manage_ContentReference((yyvsp[-1].proto), (yyvsp[0].strValue));
																		;}
    break;

  case 68:
#line 216 "XmlParser.y"
    {
																			(yyval.proto) = Manage_ContentCdsect((yyvsp[-1].proto), (yyvsp[0].strValue));
																		;}
    break;

  case 69:
#line 219 "XmlParser.y"
    {
																			(yyval.proto) = (yyvsp[-1].proto);
																		;}
    break;

  case 70:
#line 222 "XmlParser.y"
    {
																			(yyval.proto) = (yyvsp[-1].proto);
																		;}
    break;

  case 71:
#line 225 "XmlParser.y"
    {
																			(yyval.proto) = Manage_ContentChardata((yyvsp[-1].proto), (yyvsp[0].strValue));
																		;}
    break;

  case 72:
#line 228 "XmlParser.y"
    {
																			(yyval.proto) = (yyvsp[-1].proto);
																		;}
    break;

  case 73:
#line 231 "XmlParser.y"
    {
																			(yyval.proto) = Manage_ContentNothing();
																		;}
    break;

  case 74:
#line 234 "XmlParser.y"
    {
																			if ((yyvsp[-1].proto) != NULL) {
																				DPTR((yyvsp[-1].proto))->DecRefCounter((DeltaValue*) 0);
																			}
																			(yyval.proto) = (DeltaObject *)0;
																		;}
    break;

  case 75:
#line 242 "XmlParser.y"
    {
																			(yyval.tag) = Manage_EmptyElemTag((yyvsp[-2].strValue), (yyvsp[-1].tag));
																		;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 1739 "XmlParser.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (return_value, yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (return_value, YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (return_value, YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (return_value, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 247 "XmlParser.y"


/*
 * Prints an error message in case of a syntax error.
 */
int yyerror (DeltaObject **return_value, std::string yaccProvidedMessage)
{
	XmlLoaderError("%s before token %s", yaccProvidedMessage.c_str(), XmlLex_yytext);
	return -1;
}



/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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


/* Line 189 of yacc.c  */
#line 98 "XmlParser.tab.c"

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

/* Line 214 of yacc.c  */
#line 32 "XmlParser.y"

	std::string*		strValue;
	DeltaValue*		obj;
	DeltaObject*		proto;
	TagValue*			tag;
	XmlElement*			element;



/* Line 214 of yacc.c  */
#line 185 "XmlParser.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 197 "XmlParser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
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
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   116

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNRULES -- Number of states.  */
#define YYNSTATES  133

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   296

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     7,    10,    11,    15,    22,    23,    25,
      26,    28,    29,    32,    33,    40,    44,    46,    48,    49,
      54,    55,    58,    61,    62,    64,    66,    68,    70,    72,
      74,    82,    84,    86,    88,    94,    97,    99,   106,   108,
     110,   112,   114,   116,   118,   120,   128,   130,   133,   135,
     145,   147,   149,   157,   165,   167,   169,   171,   173,   175,
     179,   181,   186,   192,   194,   195,   199,   202,   205,   208,
     211,   214,   217,   218,   221
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
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
      71,    -1,    75,     6,    -1,    75,    17,    -1,    75,    19,
      -1,    75,     7,    -1,    75,    16,    -1,    -1,    75,     1,
      -1,     9,     3,    73,    12,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    56,    56,    73,    74,    77,    80,    81,    84,    85,
      88,    89,    92,    93,    96,    98,    99,   102,   103,   106,
     107,   110,   111,   112,   115,   116,   117,   118,   119,   120,
     123,   126,   127,   128,   131,   134,   135,   138,   141,   142,
     143,   146,   147,   150,   151,   154,   157,   158,   159,   162,
     165,   166,   169,   170,   173,   174,   177,   178,   179,   182,
     185,   190,   195,   198,   201,   206,   211,   214,   217,   220,
     223,   226,   229,   232,   240
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
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
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    42,    43,    44,    44,    45,    46,    46,    47,    47,
      48,    48,    49,    49,    50,    51,    51,    52,    52,    53,
      53,    54,    54,    54,    55,    55,    55,    55,    55,    55,
      56,    57,    57,    57,    58,    59,    59,    60,    61,    61,
      61,    62,    62,    63,    63,    64,    65,    65,    65,    66,
      67,    67,    68,    68,    69,    69,    70,    70,    70,    71,
      71,    72,    73,    73,    73,    74,    75,    75,    75,    75,
      75,    75,    75,    75,    76
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     2,     0,     3,     6,     0,     1,     0,
       1,     0,     2,     0,     6,     3,     1,     1,     0,     4,
       0,     2,     2,     0,     1,     1,     1,     1,     1,     1,
       7,     1,     1,     1,     5,     2,     1,     6,     1,     1,
       1,     1,     1,     1,     1,     7,     1,     2,     1,     9,
       1,     1,     7,     7,     1,     1,     1,     1,     1,     3,
       1,     4,     5,     1,     0,     3,     2,     2,     2,     2,
       2,     2,     0,     2,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       7,     0,     0,     0,     4,     9,     1,     0,     4,    72,
      60,    58,    57,    56,    13,     4,     8,    11,    64,     2,
       0,     0,     5,     4,     3,    10,    18,    63,     0,    73,
      67,    70,     0,    71,    68,    69,    66,    59,     0,    12,
      17,     0,     0,    61,    74,     0,    18,     6,     0,    65,
      17,    20,    16,    64,    18,    23,     0,    62,    15,     0,
      14,    55,    28,    29,    18,    54,     0,     0,     0,     0,
      21,    24,    25,    26,    43,    44,    27,    22,    19,     0,
       0,     0,     0,     0,    18,     0,     0,     0,     0,    17,
      36,     0,    18,     0,     0,     0,    31,    32,    33,    18,
       0,    34,    35,    46,    48,    18,     0,    18,    18,     0,
       0,    47,     0,     0,     0,     0,    30,    38,    39,    41,
      42,     0,    40,    45,    50,    51,    18,    52,    53,     0,
       0,    37,    49
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,    14,     3,     4,    17,    26,    22,    23,    51,
      90,    56,    59,    70,    71,    99,    72,    91,    92,   121,
     122,    73,    74,   105,    75,   126,    76,    77,    15,     8,
       9,    28,    37,    20,    10
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -28
static const yytype_int8 yypact[] =
{
      -6,   -27,    32,    27,    14,     7,   -28,    46,    14,   -28,
     -28,   -28,   -28,   -28,    30,    14,   -28,    10,    37,   -28,
      18,    38,   -28,    14,   -28,   -28,    39,    49,    36,   -28,
     -28,   -28,    53,   -28,   -28,   -28,   -28,   -28,    54,   -28,
     -28,    43,    41,   -28,   -28,    50,    45,   -28,    58,   -28,
      42,    44,   -28,    37,    39,   -28,    55,   -28,   -28,   -14,
     -28,   -28,   -28,   -28,    39,   -28,    48,    51,    52,    56,
     -28,   -28,   -28,   -28,   -28,   -28,   -28,   -28,   -28,    66,
      67,     3,    68,    59,    60,    61,    62,    64,    17,    71,
     -28,    73,    60,    -4,    81,   -20,   -28,   -28,   -28,    39,
      69,   -28,   -28,   -28,    57,    39,    70,    39,    39,    77,
       8,   -28,    78,     5,    79,    80,   -28,   -28,   -28,   -28,
     -28,    75,   -28,   -28,   -28,   -28,    39,   -28,   -28,    65,
      82,   -28,   -28
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -28,   -28,    -1,   -28,   -28,   -28,   -28,   -28,   -28,   -28,
     -26,   -28,   -28,   -28,   -28,   -28,   -28,     2,   -28,   -28,
     -28,   -28,   -28,   -28,   -28,   -28,   -28,   -28,   -28,    76,
     -28,    63,   -28,   -28,   -28
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      41,   107,    61,    62,   103,    63,    85,    19,     1,    64,
      65,    66,     5,   124,    24,    67,    86,   104,   108,    29,
      52,    68,    39,    69,    30,    31,   125,     7,    58,    32,
      11,    12,     6,    13,    33,    34,     7,    35,    78,   117,
     118,   119,   120,    96,    97,    98,    43,    16,    44,    18,
      21,    25,    42,    27,    38,    40,    45,    46,    47,    48,
      49,    50,    53,    54,    79,    60,    55,    80,    81,    83,
      84,    87,    82,   109,   100,    88,    89,    93,    94,   112,
      95,   114,   115,   101,   106,   110,   113,   116,   123,   127,
     128,   129,   132,   111,   102,   131,    36,     0,     0,     0,
     130,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    57
};

static const yytype_int8 yycheck[] =
{
      26,    21,    16,    17,     8,    19,     3,     8,    14,    23,
      24,    25,    39,     8,    15,    29,    13,    21,    38,     1,
      46,    35,    23,    37,     6,     7,    21,     9,    54,    11,
      16,    17,     0,    19,    16,    17,     9,    19,    64,    31,
      32,    33,    34,    26,    27,    28,    10,    40,    12,     3,
      20,    41,     3,    16,    16,    16,     3,     3,    15,    18,
      10,    16,     4,    21,    16,    10,    22,    16,    16,     3,
       3,     3,    16,    99,     3,    16,    16,    16,    16,   105,
      16,   107,   108,    10,     3,    16,    16,    10,    10,    10,
      10,    16,    10,    36,    92,    30,    20,    -1,    -1,    -1,
     126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    14,    43,    45,    46,    39,     0,     9,    71,    72,
      76,    16,    17,    19,    44,    70,    40,    47,     3,    44,
      75,    20,    49,    50,    44,    41,    48,    16,    73,     1,
       6,     7,    11,    16,    17,    19,    71,    74,    16,    44,
      16,    52,     3,    10,    12,     3,     3,    15,    18,    10,
      16,    51,    52,     4,    21,    22,    53,    73,    52,    54,
      10,    16,    17,    19,    23,    24,    25,    29,    35,    37,
      55,    56,    58,    63,    64,    66,    68,    69,    52,    16,
      16,    16,    16,     3,     3,     3,    13,     3,    16,    16,
      52,    59,    60,    16,    16,    16,    26,    27,    28,    57,
       3,    10,    59,     8,    21,    65,     3,    21,    38,    52,
      16,    36,    52,    16,    52,    52,    10,    31,    32,    33,
      34,    61,    62,    10,     8,    21,    67,    10,    10,    16,
      52,    30,    10
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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (return_value, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
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
    while (YYID (0))
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, return_value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, DeltaObject **return_value)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, return_value)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    DeltaObject **return_value;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (return_value);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, DeltaObject **return_value)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, return_value)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    DeltaObject **return_value;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, return_value);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, DeltaObject **return_value)
#else
static void
yy_reduce_print (yyvsp, yyrule, return_value)
    YYSTYPE *yyvsp;
    int yyrule;
    DeltaObject **return_value;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , return_value);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, return_value); \
} while (YYID (0))

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
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
      YYSIZE_T yyn = 0;
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
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
      int yychecklim = YYLAST - yyn + 1;
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
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
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
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, DeltaObject **return_value)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, return_value)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    DeltaObject **return_value;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (return_value);

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
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (DeltaObject **return_value);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

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
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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

/* Line 1455 of yacc.c  */
#line 56 "XmlParser.y"
    {
																			if ((yyvsp[(2) - (3)].element) == NULL) {
																				*return_value = (DeltaObject*) 0;
																			} else {
																				DeltaValue index, value;
																				index.FromString("$Name");
																				value.FromString(*((yyvsp[(2) - (3)].element)->GetName()));
																				
																				*return_value = (yyvsp[(2) - (3)].element)->GetContent();
																				DPTR((*return_value))->Set(index, value);

																				DDELETE((yyvsp[(2) - (3)].element)->GetName());
																				DDELETE((yyvsp[(2) - (3)].element));
																			}
																		;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 73 "XmlParser.y"
    { ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 74 "XmlParser.y"
    { ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 77 "XmlParser.y"
    { ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 80 "XmlParser.y"
    { ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 81 "XmlParser.y"
    { ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 84 "XmlParser.y"
    { ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 85 "XmlParser.y"
    { ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 88 "XmlParser.y"
    { ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 89 "XmlParser.y"
    { ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 92 "XmlParser.y"
    { ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 93 "XmlParser.y"
    { ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 96 "XmlParser.y"
    { ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 98 "XmlParser.y"
    { ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 99 "XmlParser.y"
    { ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 102 "XmlParser.y"
    { ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 103 "XmlParser.y"
    { ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 106 "XmlParser.y"
    { ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 107 "XmlParser.y"
    { ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 110 "XmlParser.y"
    { ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 111 "XmlParser.y"
    { ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 112 "XmlParser.y"
    { ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 115 "XmlParser.y"
    { ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 116 "XmlParser.y"
    { ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 117 "XmlParser.y"
    { ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 118 "XmlParser.y"
    { ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 119 "XmlParser.y"
    { ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 120 "XmlParser.y"
    { ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 123 "XmlParser.y"
    { ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 126 "XmlParser.y"
    { ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 127 "XmlParser.y"
    { ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 128 "XmlParser.y"
    { ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 131 "XmlParser.y"
    { ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 134 "XmlParser.y"
    { ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 135 "XmlParser.y"
    { ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 138 "XmlParser.y"
    { ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 141 "XmlParser.y"
    { ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 142 "XmlParser.y"
    { ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 143 "XmlParser.y"
    { ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 146 "XmlParser.y"
    { ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 147 "XmlParser.y"
    { ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 150 "XmlParser.y"
    { ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 151 "XmlParser.y"
    { ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 154 "XmlParser.y"
    { ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 157 "XmlParser.y"
    { ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 158 "XmlParser.y"
    { ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 159 "XmlParser.y"
    { ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 162 "XmlParser.y"
    { ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 165 "XmlParser.y"
    { ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 166 "XmlParser.y"
    { ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 169 "XmlParser.y"
    { ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 170 "XmlParser.y"
    { ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 173 "XmlParser.y"
    { ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 174 "XmlParser.y"
    { ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 177 "XmlParser.y"
    { ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 178 "XmlParser.y"
    { ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 179 "XmlParser.y"
    { ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 182 "XmlParser.y"
    {
																			(yyval.element) = Manage_ElementWithContent((yyvsp[(1) - (3)].tag), (yyvsp[(2) - (3)].proto), (yyvsp[(3) - (3)].strValue));
																		;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 185 "XmlParser.y"
    {
																			(yyval.element) = Manage_ElementWithoutContent((yyvsp[(1) - (1)].tag));
																		;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 190 "XmlParser.y"
    {
																			(yyval.tag) = Manage_Stag((yyvsp[(2) - (4)].strValue), (yyvsp[(3) - (4)].tag));
																		;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 195 "XmlParser.y"
    {
																			(yyval.tag) = Manage_Attlist((yyvsp[(2) - (5)].strValue), (yyvsp[(4) - (5)].strValue), (yyvsp[(5) - (5)].tag));
																		;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 198 "XmlParser.y"
    {
																			(yyval.tag) = Manage_AttlistSpaces();
																		;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 201 "XmlParser.y"
    {
																			(yyval.tag) = Manage_AttlistNothing();
																		;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 206 "XmlParser.y"
    {
																			(yyval.strValue) = (yyvsp[(2) - (3)].strValue);
																		;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 211 "XmlParser.y"
    {
																			(yyval.proto) = Manage_ContentElement((yyvsp[(1) - (2)].proto), (yyvsp[(2) - (2)].element));
																		;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 214 "XmlParser.y"
    {
																			(yyval.proto) = Manage_ContentCdsect((yyvsp[(1) - (2)].proto), (yyvsp[(2) - (2)].strValue));
																		;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 217 "XmlParser.y"
    {
																			(yyval.proto) = (yyvsp[(1) - (2)].proto);
																		;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 220 "XmlParser.y"
    {
																			(yyval.proto) = (yyvsp[(1) - (2)].proto);
																		;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 223 "XmlParser.y"
    {
																			(yyval.proto) = Manage_ContentChardata((yyvsp[(1) - (2)].proto), (yyvsp[(2) - (2)].strValue));
																		;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 226 "XmlParser.y"
    {
																			(yyval.proto) = (yyvsp[(1) - (2)].proto);
																		;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 229 "XmlParser.y"
    {
																			(yyval.proto) = Manage_ContentNothing();
																		;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 232 "XmlParser.y"
    {
																			if ((yyvsp[(1) - (2)].proto) != NULL) {
																				DPTR((yyvsp[(1) - (2)].proto))->DecRefCounter((DeltaValue*) 0);
																			}
																			(yyval.proto) = (DeltaObject *)0;
																		;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 240 "XmlParser.y"
    {
																			(yyval.tag) = Manage_EmptyElemTag((yyvsp[(2) - (4)].strValue), (yyvsp[(3) - (4)].tag));
																		;}
    break;



/* Line 1455 of yacc.c  */
#line 2077 "XmlParser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (return_value, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (return_value, yymsg);
	  }
	else
	  {
	    yyerror (return_value, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, return_value);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, return_value);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (return_value, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, return_value);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, return_value);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 245 "XmlParser.y"


/*
 * Prints an error message in case of a syntax error.
 */
int yyerror (DeltaObject **return_value, std::string yaccProvidedMessage)
{
	XmlLoaderError("%s before token %s", yaccProvidedMessage.c_str(), XmlLex_yytext);
	return -1;
}


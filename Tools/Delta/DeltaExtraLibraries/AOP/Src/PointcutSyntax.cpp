
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
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse         PointcutSyntax_yyparse
#define yylex           PointcutSyntax_yylex
#define yyerror         PointcutSyntax_yyerror
#define yylval          PointcutSyntax_yylval
#define yychar          PointcutSyntax_yychar
#define yydebug         PointcutSyntax_yydebug
#define yynerrs         PointcutSyntax_yynerrs
#define yylloc          PointcutSyntax_yylloc

/* Copy the first part of user declarations.  */


// AOP Pointcut syntax.
// ScriptFigher Project.
// Y. Lilis, August 2013.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "DDebug.h"
#include "DeltaStdDefs.h"
#include "DeltaAOPLib.h"
#include "Pointcut.h"
#include "PointcutScanner.h"
#include "PointcutSyntax.h"
#include "PointcutParseActions.h"
#include "ASTTags.h"
#include "ustrings.h"

#define yyFlexLexer PointcutScannerFlexLexer

#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

extern int PointcutSyntax_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer);

///////////////////////////////////////////////////////////

static void PointcutSyntax_yyerror (YYLTYPE* yylloc, Pointcut **pointcut, yyFlexLexer& lexer, const char* msg) {
	const std::string error = uconstructstr("%s : before token %s", msg, lexer.YYText());
	printf("%s\n", error.c_str());
}




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
     GLOBAL_SCOPE = 280,
     AND = 281,
     NOT = 282,
     OR = 283,
     TRUE = 284,
     FALSE = 285,
     SELF = 286,
     ARGUMENTS = 287,
     OPERATOR = 288,
     NEWSELF = 289,
     LAMBDA_REF = 290,
     DOT = 291,
     DOT_ASSIGN = 292,
     CALLOP = 293,
     CAST = 294,
     ADD = 295,
     SUB = 296,
     MUL = 297,
     DIV = 298,
     MOD = 299,
     LT = 300,
     GT = 301,
     LE = 302,
     GE = 303,
     EQ = 304,
     NE = 305,
     ATTRIBUTEOP = 306,
     ASSIGN = 307,
     ADD_A = 308,
     SUB_A = 309,
     MUL_A = 310,
     DIV_A = 311,
     MOD_A = 312,
     DOUBLE_LB = 313,
     DOUBLE_RB = 314,
     ADD_POSTFIX = 315,
     SUB_POSTFIX = 316,
     MUL_POSTFIX = 317,
     DIV_POSTFIX = 318,
     MOD_POSTFIX = 319,
     GT_POSTFIX = 320,
     LT_POSTFIX = 321,
     NE_POSTFIX = 322,
     EQ_POSTFIX = 323,
     GE_POSTFIX = 324,
     LE_POSTFIX = 325
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


/* Copy the second part of user declarations.  */



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
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  30
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   222

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  125
/* YYNRULES -- Number of states.  */
#define YYNSTATES  177

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   325

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      71,    72,     2,     2,    75,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    73,     2,    74,     2,     2,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    15,    19,    23,
      26,    31,    36,    41,    48,    53,    60,    65,    70,    72,
      74,    79,    84,    89,    94,    98,   100,   102,   104,   106,
     108,   111,   113,   114,   116,   118,   120,   122,   124,   126,
     128,   130,   132,   134,   136,   138,   140,   142,   144,   146,
     148,   150,   152,   154,   156,   158,   160,   162,   164,   166,
     168,   170,   172,   174,   176,   178,   180,   182,   184,   186,
     188,   190,   192,   194,   198,   200,   203,   205,   206,   210,
     212,   214,   215,   217,   219,   221,   223,   226,   229,   232,
     234,   236,   238,   241,   245,   250,   252,   254,   256,   258,
     260,   262,   264,   266,   268,   270,   272,   274,   276,   278,
     280,   282,   284,   286,   288,   290,   293,   297,   299,   302,
     304,   305,   309,   311,   313,   314
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      77,     0,    -1,    78,    -1,    80,    -1,    82,    -1,    79,
      -1,    71,    78,    72,    -1,    78,    26,    78,    -1,    78,
      28,    78,    -1,    27,    78,    -1,     8,    71,     5,    72,
      -1,     9,    71,     5,    72,    -1,    10,    71,    81,    72,
      -1,    10,    71,    81,    75,     5,    72,    -1,    11,    71,
      81,    72,    -1,    11,    71,    81,    75,     5,    72,    -1,
      12,    71,    81,    72,    -1,    13,    71,    81,    72,    -1,
      78,    -1,    83,    -1,    14,    71,    83,    72,    -1,    15,
      71,    95,    72,    -1,    16,    71,    86,    72,    -1,    17,
      71,    86,    72,    -1,    84,    85,    89,    -1,    20,    -1,
      21,    -1,    22,    -1,    42,    -1,    86,    -1,    33,    88,
      -1,    51,    -1,    -1,     6,    -1,    42,    -1,    87,    -1,
       8,    -1,     9,    -1,    10,    -1,    11,    -1,    12,    -1,
      13,    -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,
      40,    -1,    41,    -1,    42,    -1,    43,    -1,    44,    -1,
      46,    -1,    45,    -1,    50,    -1,    49,    -1,    48,    -1,
      47,    -1,    52,    -1,    60,    -1,    61,    -1,    62,    -1,
      63,    -1,    64,    -1,    65,    -1,    66,    -1,    67,    -1,
      68,    -1,    69,    -1,    70,    -1,    38,    -1,    39,    -1,
      36,    -1,    37,    -1,    71,    90,    72,    -1,    38,    -1,
      91,    94,    -1,    92,    -1,    -1,    92,    75,    93,    -1,
      93,    -1,    86,    -1,    -1,    23,    -1,    24,    -1,    96,
      -1,    99,    -1,    97,   105,    -1,    98,    86,    -1,    98,
      23,    -1,    86,    -1,    23,    -1,    25,    -1,    86,    25,
      -1,    98,    86,    25,    -1,   100,   101,    86,   105,    -1,
      86,    -1,   104,    -1,    31,    -1,    34,    -1,    32,    -1,
      36,    -1,    23,    -1,     6,    -1,   103,    -1,    20,    -1,
      21,    -1,    22,    -1,    26,    -1,    28,    -1,    27,    -1,
      29,    -1,    30,    -1,    31,    -1,    32,    -1,     7,    -1,
      51,   102,    -1,    71,   106,    72,    -1,    38,    -1,   107,
     110,    -1,   108,    -1,    -1,   108,    75,   109,    -1,   109,
      -1,    42,    -1,    -1,    23,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   101,   101,   104,   105,   106,   107,   110,   111,   112,
     115,   116,   117,   118,   119,   120,   121,   122,   125,   126,
     129,   130,   131,   132,   140,   144,   145,   146,   147,   149,
     150,   151,   152,   155,   156,   157,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   203,   204,   207,   210,   211,   214,   215,
     218,   221,   222,   223,   228,   229,   232,   236,   237,   238,
     239,   244,   245,   246,   249,   253,   254,   255,   256,   257,
     264,   265,   268,   269,   272,   273,   274,   275,   276,   277,
     278,   279,   280,   281,   284,   285,   288,   289,   292,   295,
     296,   299,   301,   304,   309,   310
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BOOLEAN", "NUMBER", "STRING", "IDENT",
  "ATTRIBUTE_IDENT", "AST", "ATTRIBUTE", "CHILD", "PARENT", "DESCENDANT",
  "ASCENDANT", "EXECUTION", "CALL", "EXCEPTION", "CLASS", "SETTER",
  "GETTER", "FUNCTION", "ONEVENT", "METHOD", "DOUBLE_DOT", "TRIPLE_DOT",
  "GLOBAL_SCOPE", "AND", "NOT", "OR", "TRUE", "FALSE", "SELF", "ARGUMENTS",
  "OPERATOR", "NEWSELF", "LAMBDA_REF", "DOT", "DOT_ASSIGN", "CALLOP",
  "CAST", "ADD", "SUB", "MUL", "DIV", "MOD", "LT", "GT", "LE", "GE", "EQ",
  "NE", "ATTRIBUTEOP", "ASSIGN", "ADD_A", "SUB_A", "MUL_A", "DIV_A",
  "MOD_A", "DOUBLE_LB", "DOUBLE_RB", "ADD_POSTFIX", "SUB_POSTFIX",
  "MUL_POSTFIX", "DIV_POSTFIX", "MOD_POSTFIX", "GT_POSTFIX", "LT_POSTFIX",
  "NE_POSTFIX", "EQ_POSTFIX", "GE_POSTFIX", "LE_POSTFIX", "'('", "')'",
  "'['", "']'", "','", "$accept", "Pointcut", "Pattern", "Combinator",
  "ASTPattern", "SubPattern", "AOPPattern", "FuncPattern", "FuncClass",
  "FuncNamePattern", "IdentPattern", "AOPKwd", "OperatorMethod", "Formals",
  "FormalsPattern", "FormalPatternList", "FormalPatternNonEmptyList",
  "FormalPattern", "FormalArgsSuffix", "CallPattern", "NormalCall",
  "TargetFunc", "NamespacePath", "MethodCall", "ObjectPattern",
  "IndexOperator", "AttributeIdent", "UsedKwdIdent", "AttributeId",
  "Arguments", "ArgumentsPattern", "ArgumentPatternList",
  "ArgumentPatternNonEmptyList", "ArgumentPattern", "ArgumentsSuffix", 0
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,    40,    41,    91,    93,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    76,    77,    78,    78,    78,    78,    79,    79,    79,
      80,    80,    80,    80,    80,    80,    80,    80,    81,    81,
      82,    82,    82,    82,    83,    84,    84,    84,    84,    85,
      85,    85,    85,    86,    86,    86,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    89,    89,    90,    91,    91,    92,    92,
      93,    94,    94,    94,    95,    95,    96,    97,    97,    97,
      97,    98,    98,    98,    99,   100,   100,   100,   100,   100,
     101,   101,   102,   102,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   104,   104,   105,   105,   106,   107,
     107,   108,   108,   109,   110,   110
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     3,     3,     3,     2,
       4,     4,     4,     6,     4,     6,     4,     4,     1,     1,
       4,     4,     4,     4,     3,     1,     1,     1,     1,     1,
       2,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     2,     1,     0,     3,     1,
       1,     0,     1,     1,     1,     1,     2,     2,     2,     1,
       1,     1,     2,     3,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     1,     2,     1,
       0,     3,     1,     1,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     2,     5,     3,     4,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     9,     0,
       1,     0,     0,     0,     0,    25,    26,    27,    28,    18,
       0,    19,    32,     0,     0,     0,     0,    33,   114,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    90,
      91,    97,    99,    98,    34,     0,    89,    35,     0,    84,
       0,     0,    85,     0,    96,     0,     0,     6,     7,     8,
      10,    11,    12,     0,     0,    31,     0,    29,    14,     0,
      16,    17,    20,   102,   104,   105,   106,   107,   109,   108,
     110,   111,   112,   113,   115,   103,    92,    21,   117,   120,
      86,    88,    87,   101,   100,     0,    22,    23,     0,    71,
      72,    69,    70,    46,    47,    48,    49,    50,    52,    51,
      56,    55,    54,    53,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    30,    74,    77,    24,
       0,   123,     0,   124,   119,   122,    93,     0,    13,    80,
       0,    81,    76,    79,    15,   116,   125,   118,     0,    94,
      73,    82,    83,    75,     0,   121,    78
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    13,    39,    15,    16,    40,    17,    41,    42,    86,
     159,    67,   146,   149,   160,   161,   162,   163,   173,    68,
      69,    70,    71,    72,    73,   115,   104,   105,    74,   110,
     152,   153,   154,   155,   167
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -56
static const yytype_int16 yypact[] =
{
      18,   -55,   -47,   -29,     4,     9,    10,    11,    12,    13,
      14,    18,    18,    86,    38,   -56,   -56,   -56,    83,    87,
      -2,    -2,    -2,    -2,     1,    45,   136,   136,   -56,   -23,
     -56,    18,    18,    19,    21,   -56,   -56,   -56,   -56,    38,
     -36,   -56,    89,   -31,    22,    36,    47,   -56,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,   185,    42,   -56,    48,   -56,
     -34,   101,   -56,    27,   -56,    49,    53,   -56,   -56,    97,
     -56,   -56,   -56,   121,   119,   -56,     0,   -56,   -56,   122,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,    88,
     -56,   -56,   103,   -56,   -56,   136,   -56,   -56,    57,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   136,   -56,
      60,   -56,    61,   111,    62,   -56,   -56,   -34,   -56,   -56,
      63,    24,    64,   -56,   -56,   -56,   -56,   -56,    88,   -56,
     -56,   -56,   -56,   -56,   136,   -56,   -56
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -56,   -56,   190,   -56,   -56,    51,   -56,   112,   -56,   -56,
     -25,   -56,   -56,   -56,   -56,   -56,   -56,   -33,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -19,
     -56,   -56,   -56,   -14,   -56
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -96
static const yytype_int16 yytable[] =
{
      66,    75,    76,    31,   108,    32,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    18,    87,    35,    36,
      37,    35,    36,    37,    19,    11,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    82,   109,   147,    83,
      38,    88,    20,    38,    89,    11,   112,   171,   172,    77,
     113,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,   114,    31,   -95,    32,   106,    59,    12,
      60,   148,    43,    44,    45,    21,    61,    62,   -95,    63,
      22,    23,    24,    25,    26,    27,    30,    64,    33,    12,
     157,    80,    34,    81,    90,    47,    65,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    47,    91,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    92,
     107,   116,    84,    31,   111,   117,   118,   150,   156,   158,
     151,    64,   164,   165,   166,   170,    46,   168,   169,   174,
      85,   176,    47,    64,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,   175,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
       0,   134,     0,     0,     0,     0,     0,     0,    64,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
      14,    93,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,    29,     0,     0,    94,    95,    96,     0,     0,
       0,    97,    98,    99,   100,   101,   102,   103,     0,     0,
       0,    78,    79
};

static const yytype_int16 yycheck[] =
{
      25,    26,    27,    26,    38,    28,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    71,    42,    20,    21,
      22,    20,    21,    22,    71,    27,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    72,    71,    38,    75,
      42,    72,    71,    42,    75,    27,    71,    23,    24,    72,
      23,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    36,    26,    23,    28,    25,    23,    71,
      25,    71,    21,    22,    23,    71,    31,    32,    36,    34,
      71,    71,    71,    71,    71,    71,     0,    42,     5,    71,
     115,    72,     5,    72,    72,     6,    51,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,     6,    72,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    72,
      72,    72,    33,    26,    23,    72,     5,     5,    25,    72,
      42,    42,    72,    72,    23,    72,    24,    75,   157,    75,
      51,   174,     6,    42,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,   168,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      -1,    52,    -1,    -1,    -1,    -1,    -1,    -1,    42,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
       0,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    11,    12,    -1,    -1,    20,    21,    22,    -1,    -1,
      -1,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    31,    32
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    27,    71,    77,    78,    79,    80,    82,    71,    71,
      71,    71,    71,    71,    71,    71,    71,    71,    78,    78,
       0,    26,    28,     5,     5,    20,    21,    22,    42,    78,
      81,    83,    84,    81,    81,    81,    83,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    23,
      25,    31,    32,    34,    42,    51,    86,    87,    95,    96,
      97,    98,    99,   100,   104,    86,    86,    72,    78,    78,
      72,    72,    72,    75,    33,    51,    85,    86,    72,    75,
      72,    72,    72,     6,    20,    21,    22,    26,    27,    28,
      29,    30,    31,    32,   102,   103,    25,    72,    38,    71,
     105,    23,    86,    23,    36,   101,    72,    72,     5,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    52,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    88,    38,    71,    89,
       5,    42,   106,   107,   108,   109,    25,    86,    72,    86,
      90,    91,    92,    93,    72,    72,    23,   110,    75,   105,
      72,    23,    24,    94,    75,   109,    93
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
      yyerror (&yylloc, pointcut, lexer, YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, lexer)
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
		  Type, Value, Location, pointcut, lexer); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Pointcut **pointcut, yyFlexLexer& lexer)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pointcut, lexer)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    Pointcut **pointcut;
    yyFlexLexer& lexer;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (pointcut);
  YYUSE (lexer);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Pointcut **pointcut, yyFlexLexer& lexer)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, pointcut, lexer)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    Pointcut **pointcut;
    yyFlexLexer& lexer;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pointcut, lexer);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, Pointcut **pointcut, yyFlexLexer& lexer)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, pointcut, lexer)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    Pointcut **pointcut;
    yyFlexLexer& lexer;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , pointcut, lexer);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, pointcut, lexer); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, Pointcut **pointcut, yyFlexLexer& lexer)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, pointcut, lexer)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    Pointcut **pointcut;
    yyFlexLexer& lexer;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (pointcut);
  YYUSE (lexer);

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
int yyparse (Pointcut **pointcut, yyFlexLexer& lexer);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





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
yyparse (Pointcut **pointcut, yyFlexLexer& lexer)
#else
int
yyparse (pointcut, lexer)
    Pointcut **pointcut;
    yyFlexLexer& lexer;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

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

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
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
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

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
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
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
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

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
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

    { *pointcut = (yyvsp[(1) - (1)].pointcut); ;}
    break;

  case 3:

    { (yyval.pointcut) = (yyvsp[(1) - (1)].pointcut); ;}
    break;

  case 4:

    { (yyval.pointcut) = (yyvsp[(1) - (1)].pointcut); ;}
    break;

  case 5:

    { (yyval.pointcut) = (yyvsp[(1) - (1)].pointcut); ;}
    break;

  case 6:

    { (yyval.pointcut) = (yyvsp[(2) - (3)].pointcut); ;}
    break;

  case 7:

    { (yyval.pointcut) = Manage_AND((yyvsp[(1) - (3)].pointcut), (yyvsp[(3) - (3)].pointcut)); ;}
    break;

  case 8:

    { (yyval.pointcut) = Manage_OR((yyvsp[(1) - (3)].pointcut), (yyvsp[(3) - (3)].pointcut)); ;}
    break;

  case 9:

    { (yyval.pointcut) = Manage_NOT((yyvsp[(2) - (2)].pointcut)); ;}
    break;

  case 10:

    { (yyval.pointcut) = Manage_AST((yyvsp[(3) - (4)].stringVal)); ;}
    break;

  case 11:

    { (yyval.pointcut) = Manage_ATTRIBUTE(); ;}
    break;

  case 12:

    { (yyval.pointcut) = Manage_CHILD((yyvsp[(3) - (4)].pointcut)); ;}
    break;

  case 13:

    { (yyval.pointcut) = Manage_CHILD((yyvsp[(3) - (6)].pointcut), (yyvsp[(5) - (6)].stringVal)); ;}
    break;

  case 14:

    { (yyval.pointcut) = Manage_PARENT((yyvsp[(3) - (4)].pointcut)); ;}
    break;

  case 15:

    { (yyval.pointcut) = Manage_PARENT((yyvsp[(3) - (6)].pointcut), (yyvsp[(5) - (6)].stringVal)); ;}
    break;

  case 16:

    { (yyval.pointcut) = Manage_DESCENDANT((yyvsp[(3) - (4)].pointcut)); ;}
    break;

  case 17:

    { (yyval.pointcut) = Manage_ASCENDANT((yyvsp[(3) - (4)].pointcut)); ;}
    break;

  case 18:

    { (yyval.pointcut) = (yyvsp[(1) - (1)].pointcut); ;}
    break;

  case 19:

    { (yyval.pointcut) = (yyvsp[(1) - (1)].pointcut); ;}
    break;

  case 20:

    { (yyval.pointcut) = (yyvsp[(3) - (4)].pointcut); ;}
    break;

  case 21:

    { (yyval.pointcut) = (yyvsp[(3) - (4)].pointcut); ;}
    break;

  case 22:

    { (yyval.pointcut) = Manage_EXCEPTION((yyvsp[(3) - (4)].stringVal)); ;}
    break;

  case 23:

    { (yyval.pointcut) = Manage_CLASS((yyvsp[(3) - (4)].stringVal)); ;}
    break;

  case 24:

    { (yyval.pointcut) = Manage_FuncPattern((yyvsp[(1) - (3)].stringVal), (yyvsp[(2) - (3)].stringVal), (yyvsp[(3) - (3)].idList)); ;}
    break;

  case 25:

    { (yyval.stringVal) = AST_VALUE_FUNCCLASS_PROGRAMFUNCTION; ;}
    break;

  case 26:

    { (yyval.stringVal) = AST_VALUE_FUNCCLASS_ONEVENT; ;}
    break;

  case 27:

    { (yyval.stringVal) = AST_VALUE_FUNCCLASS_METHOD; ;}
    break;

  case 28:

    { (yyval.stringVal) = "*"; ;}
    break;

  case 29:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 30:

    { (yyval.stringVal) = (yyvsp[(2) - (2)].stringVal); ;}
    break;

  case 31:

    { (yyval.stringVal) = AST_VALUE_TOSTRING_SYMBOLIC_NAME; ;}
    break;

  case 32:

    { (yyval.stringVal) = ""; ;}
    break;

  case 33:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 34:

    { (yyval.stringVal) = "*"; ;}
    break;

  case 35:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 36:

    { (yyval.stringVal) = "ast";		;}
    break;

  case 37:

    { (yyval.stringVal) = "attribute";	;}
    break;

  case 38:

    { (yyval.stringVal) = "child";		;}
    break;

  case 39:

    { (yyval.stringVal) = "parent";	;}
    break;

  case 40:

    { (yyval.stringVal) = "descendant";;}
    break;

  case 41:

    { (yyval.stringVal) = "ascendant"; ;}
    break;

  case 42:

    { (yyval.stringVal) = "execution";	;}
    break;

  case 43:

    { (yyval.stringVal) = "call";		;}
    break;

  case 44:

    { (yyval.stringVal) = "exception";	;}
    break;

  case 45:

    { (yyval.stringVal) = "class";		;}
    break;

  case 46:

    { (yyval.stringVal) = "+";			;}
    break;

  case 47:

    { (yyval.stringVal) = "-";			;}
    break;

  case 48:

    { (yyval.stringVal) = "*";			;}
    break;

  case 49:

    { (yyval.stringVal) = "/";			;}
    break;

  case 50:

    { (yyval.stringVal) = "%";			;}
    break;

  case 51:

    { (yyval.stringVal) = ">";			;}
    break;

  case 52:

    { (yyval.stringVal) = "<";			;}
    break;

  case 53:

    { (yyval.stringVal) = "!=";		;}
    break;

  case 54:

    { (yyval.stringVal) = "==";		;}
    break;

  case 55:

    { (yyval.stringVal) = ">=";		;}
    break;

  case 56:

    { (yyval.stringVal) = "<=";		;}
    break;

  case 57:

    { (yyval.stringVal) = "=";			;}
    break;

  case 58:

    { (yyval.stringVal) = "+_";		;}
    break;

  case 59:

    { (yyval.stringVal) = "-_";			;}
    break;

  case 60:

    { (yyval.stringVal) = "*_";		;}
    break;

  case 61:

    { (yyval.stringVal) = "/_";		;}
    break;

  case 62:

    { (yyval.stringVal) = "%_";		;}
    break;

  case 63:

    { (yyval.stringVal) = ">_";		;}
    break;

  case 64:

    { (yyval.stringVal) = "<_";		;}
    break;

  case 65:

    { (yyval.stringVal) = "!=_";		;}
    break;

  case 66:

    { (yyval.stringVal) = "==_";		;}
    break;

  case 67:

    { (yyval.stringVal) = ">=_";		;}
    break;

  case 68:

    { (yyval.stringVal) = "<=_";		;}
    break;

  case 69:

    { (yyval.stringVal) = "()";		;}
    break;

  case 70:

    { (yyval.stringVal) = "=()";		;}
    break;

  case 71:

    { (yyval.stringVal) = DELTA_OPERATOR_OBJECT_GET;	;}
    break;

  case 72:

    { (yyval.stringVal) = DELTA_OPERATOR_OBJECT_SET;	;}
    break;

  case 73:

    { (yyval.idList) = (yyvsp[(2) - (3)].idList); ;}
    break;

  case 74:

    { (yyval.idList) = Manage_IdList(); ;}
    break;

  case 75:

    { (yyval.idList) = Manage_IdList((yyvsp[(1) - (2)].idList), (yyvsp[(2) - (2)].stringVal)); ;}
    break;

  case 76:

    { (yyval.idList) = (yyvsp[(1) - (1)].idList); ;}
    break;

  case 77:

    { (yyval.idList) = Manage_IdList(); ;}
    break;

  case 78:

    { (yyval.idList) = Manage_IdList((yyvsp[(1) - (3)].idList), (yyvsp[(3) - (3)].stringVal)); ;}
    break;

  case 79:

    { (yyval.idList) = Manage_IdList(0, (yyvsp[(1) - (1)].stringVal)); ;}
    break;

  case 80:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 81:

    { unullify((yyval.stringVal)); ;}
    break;

  case 82:

    { (yyval.stringVal) = AOP_MATCH_MULTIPLE; ;}
    break;

  case 83:

    { (yyval.stringVal) = AST_VALUE_VARARGS_FORMAL_NAME; ;}
    break;

  case 84:

    { (yyval.pointcut) = (yyvsp[(1) - (1)].pointcut); ;}
    break;

  case 85:

    { (yyval.pointcut) = (yyvsp[(1) - (1)].pointcut); ;}
    break;

  case 86:

    { (yyval.pointcut) = Manage_NormalCall((yyvsp[(1) - (2)].idList), (yyvsp[(2) - (2)].idList)); ;}
    break;

  case 87:

    { (yyval.idList) = Manage_IdList((yyvsp[(1) - (2)].idList), (yyvsp[(2) - (2)].stringVal)); ;}
    break;

  case 88:

    { (yyval.idList) = Manage_IdList((yyvsp[(1) - (2)].idList), AOP_MATCH_MULTIPLE); ;}
    break;

  case 89:

    { (yyval.idList) = Manage_IdList(0, (yyvsp[(1) - (1)].stringVal)); ;}
    break;

  case 90:

    { (yyval.idList) = Manage_IdList(0, AOP_MATCH_MULTIPLE); ;}
    break;

  case 91:

    { (yyval.idList) = Manage_IdList(0, DELTA_LIBRARYNAMESPACE_SEPARATOR); ;}
    break;

  case 92:

    { (yyval.idList) = Manage_IdList(0, (yyvsp[(1) - (2)].stringVal)); ;}
    break;

  case 93:

    { (yyval.idList) = Manage_IdList((yyvsp[(1) - (3)].idList), (yyvsp[(2) - (3)].stringVal)); ;}
    break;

  case 94:

    { (yyval.pointcut) = Manage_MethodCall((yyvsp[(1) - (4)].stringVal), (yyvsp[(2) - (4)].boolVal), (yyvsp[(3) - (4)].stringVal), (yyvsp[(4) - (4)].idList)); ;}
    break;

  case 95:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 96:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 97:

    { (yyval.stringVal) = AST_TAG_SELF; ;}
    break;

  case 98:

    { (yyval.stringVal) = AST_TAG_LVALUE_NEWSELF; ;}
    break;

  case 99:

    { (yyval.stringVal) = AST_TAG_ARGUMENTS; ;}
    break;

  case 100:

    { (yyval.boolVal) = false; ;}
    break;

  case 101:

    { (yyval.boolVal) = true; ;}
    break;

  case 102:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 103:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 104:

    { (yyval.stringVal) = "function";	;}
    break;

  case 105:

    { (yyval.stringVal) = "onevent";	;}
    break;

  case 106:

    { (yyval.stringVal) = "method";	;}
    break;

  case 107:

    { (yyval.stringVal) = "and";		;}
    break;

  case 108:

    { (yyval.stringVal) = "or";		;}
    break;

  case 109:

    { (yyval.stringVal) = "not";		;}
    break;

  case 110:

    { (yyval.stringVal) = "true";		;}
    break;

  case 111:

    { (yyval.stringVal) = "false";		;}
    break;

  case 112:

    { (yyval.stringVal) = "self";		;}
    break;

  case 113:

    { (yyval.stringVal) = "arguments"; ;}
    break;

  case 114:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal) + 1; ;}
    break;

  case 115:

    { (yyval.stringVal) = (yyvsp[(2) - (2)].stringVal); ;}
    break;

  case 116:

    { (yyval.idList) = (yyvsp[(2) - (3)].idList); ;}
    break;

  case 117:

    { (yyval.idList) = Manage_ArgumentsEmpty(); ;}
    break;

  case 118:

    { (yyval.idList) = Manage_Arguments((yyvsp[(1) - (2)].idList), (yyvsp[(2) - (2)].stringVal)); ;}
    break;

  case 119:

    { (yyval.idList) = (yyvsp[(1) - (1)].idList); ;}
    break;

  case 120:

    { (yyval.idList) = Manage_ArgumentsEmpty(); ;}
    break;

  case 121:

    { (yyval.idList) = Manage_ArgumentPatternList((yyvsp[(1) - (3)].idList), (yyvsp[(3) - (3)].stringVal)); ;}
    break;

  case 122:

    { (yyval.idList) = Manage_ArgumentPattern((yyvsp[(1) - (1)].stringVal)); ;}
    break;

  case 123:

    { (yyval.stringVal) = "*"; ;}
    break;

  case 124:

    { unullify((yyval.stringVal)); ;}
    break;

  case 125:

    { (yyval.stringVal) = AOP_MATCH_MULTIPLE; ;}
    break;



      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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
      yyerror (&yylloc, pointcut, lexer, YY_("syntax error"));
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
	    yyerror (&yylloc, pointcut, lexer, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, pointcut, lexer, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

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
		      yytoken, &yylval, &yylloc, pointcut, lexer);
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

  yyerror_range[0] = yylsp[1-yylen];
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, pointcut, lexer);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

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
  yyerror (&yylloc, pointcut, lexer, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, pointcut, lexer);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, pointcut, lexer);
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




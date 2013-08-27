
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

#define yyFlexLexer PointcutScannerFlexLexer

#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

extern int PointcutSyntax_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer);

///////////////////////////////////////////////////////////

static void PointcutSyntax_yyerror (YYLTYPE* yylloc, Pointcut **pointcut, yyFlexLexer& lexer, const char* msg) {
	//void yyerror (DecisionMaker *dm, Statement**, const char *msg) {
	//extern char *DMSL_yytext;
	//dm->SetError(util::MakeString("Line %d : %s: before token: %s\n", DMSL_yylineno, msg, DMSL_yytext));
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
#define YYFINAL  24
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   128

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  101
/* YYNRULES -- Number of states.  */
#define YYNSTATES  146

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   324

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      70,    71,     2,     2,    74,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    72,     2,    73,     2,     2,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    15,    19,    22,
      27,    32,    37,    44,    49,    56,    61,    66,    71,    76,
      82,    84,    86,    88,    90,    92,    95,    97,    98,   100,
     102,   104,   106,   108,   110,   112,   114,   116,   118,   120,
     122,   124,   126,   128,   130,   132,   134,   136,   138,   140,
     142,   144,   146,   148,   150,   152,   154,   157,   159,   160,
     164,   166,   168,   169,   171,   173,   175,   177,   182,   184,
     186,   188,   195,   197,   199,   201,   203,   205,   207,   209,
     211,   213,   215,   217,   219,   221,   223,   225,   227,   229,
     231,   233,   235,   238,   241,   243,   244,   248,   250,   252,
     254,   255
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      76,     0,    -1,    77,    -1,    79,    -1,    80,    -1,    78,
      -1,    77,    25,    77,    -1,    77,    27,    77,    -1,    26,
      77,    -1,     8,    70,     5,    71,    -1,     9,    70,     5,
      71,    -1,    10,    70,    77,    71,    -1,    10,    70,    77,
      74,     5,    71,    -1,    11,    70,    77,    71,    -1,    11,
      70,    77,    74,     5,    71,    -1,    12,    70,    77,    71,
      -1,    13,    70,    77,    71,    -1,    14,    70,    81,    71,
      -1,    15,    70,    91,    71,    -1,    82,    83,    70,    86,
      71,    -1,    20,    -1,    21,    -1,    22,    -1,    41,    -1,
      84,    -1,    32,    85,    -1,    50,    -1,    -1,     6,    -1,
      39,    -1,    40,    -1,    41,    -1,    42,    -1,    43,    -1,
      45,    -1,    44,    -1,    49,    -1,    48,    -1,    47,    -1,
      46,    -1,    51,    -1,    59,    -1,    60,    -1,    61,    -1,
      62,    -1,    63,    -1,    64,    -1,    65,    -1,    66,    -1,
      67,    -1,    68,    -1,    69,    -1,    37,    -1,    38,    -1,
      35,    -1,    36,    -1,    87,    90,    -1,    88,    -1,    -1,
      88,    74,    89,    -1,    89,    -1,    84,    -1,    -1,    23,
      -1,    24,    -1,    92,    -1,    94,    -1,    93,    70,   100,
      71,    -1,    95,    -1,    34,    -1,     5,    -1,    95,    96,
      84,    70,   100,    71,    -1,    84,    -1,    99,    -1,    30,
      -1,    33,    -1,    31,    -1,    35,    -1,    23,    -1,     6,
      -1,    98,    -1,    20,    -1,    21,    -1,    22,    -1,    25,
      -1,    27,    -1,    26,    -1,    28,    -1,    29,    -1,    30,
      -1,    31,    -1,     7,    -1,    50,    97,    -1,   101,   104,
      -1,   102,    -1,    -1,   102,    74,   103,    -1,   103,    -1,
      84,    -1,    24,    -1,    -1,    23,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    96,    96,    99,   100,   101,   104,   105,   106,   109,
     110,   111,   112,   113,   114,   115,   116,   119,   120,   130,
     134,   135,   136,   137,   139,   140,   141,   142,   145,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   177,   180,   181,   184,
     185,   188,   191,   192,   193,   198,   199,   202,   206,   207,
     208,   212,   216,   217,   218,   219,   220,   227,   228,   231,
     232,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   247,   248,   251,   254,   255,   258,   259,   262,   263,
     268,   269
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
  "AND", "NOT", "OR", "TRUE", "FALSE", "SELF", "ARGUMENTS", "OPERATOR",
  "NEWSELF", "LAMBDA_REF", "DOT", "DOT_ASSIGN", "CALLOP", "CAST", "ADD",
  "SUB", "MUL", "DIV", "MOD", "LT", "GT", "LE", "GE", "EQ", "NE",
  "ATTRIBUTEOP", "ASSIGN", "ADD_A", "SUB_A", "MUL_A", "DIV_A", "MOD_A",
  "DOUBLE_LB", "DOUBLE_RB", "ADD_POSTFIX", "SUB_POSTFIX", "MUL_POSTFIX",
  "DIV_POSTFIX", "MOD_POSTFIX", "GT_POSTFIX", "LT_POSTFIX", "NE_POSTFIX",
  "EQ_POSTFIX", "GE_POSTFIX", "LE_POSTFIX", "'('", "')'", "'['", "']'",
  "','", "$accept", "Pointcut", "Pattern", "Combinator", "ASTPattern",
  "AOPPattern", "FuncPattern", "FuncClass", "FuncNamePattern",
  "IdentPattern", "OperatorMethod", "FormalsPattern", "FormalPatternList",
  "FormalPatternNonEmptyList", "FormalPattern", "FormalArgsSuffix",
  "CallPattern", "NormalCall", "TargetFunc", "MethodCall", "ObjectPattern",
  "IndexOperator", "AttributeIdent", "UsedKwdIdent", "AttributeId",
  "ArgumentsPattern", "ArgumentPatternList", "ArgumentPatternNonEmptyList",
  "ArgumentPattern", "ArgumentsSuffix", 0
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
      40,    41,    91,    93,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    76,    77,    77,    77,    78,    78,    78,    79,
      79,    79,    79,    79,    79,    79,    79,    80,    80,    81,
      82,    82,    82,    82,    83,    83,    83,    83,    84,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    86,    87,    87,    88,
      88,    89,    90,    90,    90,    91,    91,    92,    93,    93,
      93,    94,    95,    95,    95,    95,    95,    96,    96,    97,
      97,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    99,    99,   100,   101,   101,   102,   102,   103,   103,
     104,   104
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     3,     3,     2,     4,
       4,     4,     6,     4,     6,     4,     4,     4,     4,     5,
       1,     1,     1,     1,     1,     2,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     0,     3,
       1,     1,     0,     1,     1,     1,     1,     4,     1,     1,
       1,     6,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     0,     3,     1,     1,     1,
       0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     2,     5,     3,     4,     0,     0,     0,     0,     0,
       0,     0,     0,     8,     1,     0,     0,     0,     0,     0,
       0,     0,     0,    20,    21,    22,    23,     0,    27,    70,
      28,    91,    74,    76,    75,    69,     0,    72,     0,    65,
       0,    66,    68,    73,     6,     7,     9,    10,    11,     0,
      13,     0,    15,    16,    17,     0,    26,     0,    24,    79,
      81,    82,    83,    84,    86,    85,    87,    88,    89,    90,
      92,    80,    18,    95,    78,    77,     0,     0,     0,    54,
      55,    52,    53,    29,    30,    31,    32,    33,    35,    34,
      39,    38,    37,    36,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    25,    58,    99,    98,
       0,   100,    94,    97,     0,    12,    14,    61,     0,    62,
      57,    60,    67,   101,    93,     0,    95,    19,    63,    64,
      56,     0,    96,     0,    59,    71
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    10,    11,    12,    13,    14,    37,    38,    67,   119,
     116,   128,   129,   130,   131,   140,    48,    49,    50,    51,
      52,    86,    80,    81,    53,   120,   121,   122,   123,   134
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -53
static const yytype_int8 yypact[] =
{
      11,   -52,   -30,   -29,   -28,   -27,   -26,   -16,   -14,    11,
      57,     3,   -53,   -53,   -53,    53,    54,    11,    11,    11,
      11,    -7,     5,   -53,   -53,    11,    11,    -8,    16,   -24,
     -23,   -19,   -18,   -53,   -53,   -53,   -53,    18,    -1,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,    40,   -53,    19,   -53,
      22,   -53,    -6,   -53,   -53,    66,   -53,   -53,   -53,    88,
     -53,    89,   -53,   -53,   -53,    37,   -53,    38,   -53,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,
     -53,   -53,   -53,    21,   -53,   -53,   103,    39,    41,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,   -53,   103,   -53,   -53,
      42,    91,    46,   -53,    51,   -53,   -53,   -53,    55,     9,
      48,   -53,   -53,   -53,   -53,    21,    21,   -53,   -53,   -53,
     -53,   103,   -53,    56,   -53,   -53
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -53,   -53,    98,   -53,   -53,   -53,   -53,   -53,   -53,   -22,
     -53,   -53,   -53,   -53,   -13,   -53,   -53,   -53,   -53,   -53,
     -53,   -53,   -53,   -53,   -53,   -25,   -53,   -53,   -10,   -53
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      47,    25,    25,    26,    26,    40,    25,    25,    26,    26,
      39,    40,    41,    33,    34,    35,    68,    84,    15,     1,
       2,     3,     4,     5,     6,     7,     8,    40,    25,    85,
      26,    65,   138,   139,    36,    42,    43,     9,    44,    45,
      16,    17,    18,    19,    20,   118,    69,    58,    60,    66,
      59,    61,    62,    63,    21,    46,    22,    24,    27,    28,
      70,    71,    72,    56,   124,    73,    74,    75,    76,    77,
      78,    79,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,    57,   104,    64,
      82,    25,    83,    87,    88,   127,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,    23,   117,    40,
     125,   143,   126,   132,   133,    29,    30,    31,    32,   127,
     135,   136,   141,    54,    55,   142,   137,   145,   144
};

static const yytype_uint8 yycheck[] =
{
      22,    25,    25,    27,    27,     6,    25,    25,    27,    27,
       5,     6,     7,    20,    21,    22,    38,    23,    70,     8,
       9,    10,    11,    12,    13,    14,    15,     6,    25,    35,
      27,    32,    23,    24,    41,    30,    31,    26,    33,    34,
      70,    70,    70,    70,    70,    24,     6,    71,    71,    50,
      74,    74,    71,    71,    70,    50,    70,     0,     5,     5,
      20,    21,    22,    71,    86,    25,    26,    27,    28,    29,
      30,    31,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    71,    51,    71,
      71,    25,    70,     5,     5,   117,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,     9,    70,     6,
      71,   136,    71,    71,    23,    17,    18,    19,    20,   141,
      74,    70,    74,    25,    26,   135,    71,    71,   141
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     8,     9,    10,    11,    12,    13,    14,    15,    26,
      76,    77,    78,    79,    80,    70,    70,    70,    70,    70,
      70,    70,    70,    77,     0,    25,    27,     5,     5,    77,
      77,    77,    77,    20,    21,    22,    41,    81,    82,     5,
       6,     7,    30,    31,    33,    34,    50,    84,    91,    92,
      93,    94,    95,    99,    77,    77,    71,    71,    71,    74,
      71,    74,    71,    71,    71,    32,    50,    83,    84,     6,
      20,    21,    22,    25,    26,    27,    28,    29,    30,    31,
      97,    98,    71,    70,    23,    35,    96,     5,     5,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    51,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    85,    70,    24,    84,
     100,   101,   102,   103,    84,    71,    71,    84,    86,    87,
      88,    89,    71,    23,   104,    74,    70,    71,    23,    24,
      90,    74,   103,   100,    89,    71
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

    { (yyval.pointcut) = Manage_AND((yyvsp[(1) - (3)].pointcut), (yyvsp[(3) - (3)].pointcut)); ;}
    break;

  case 7:

    { (yyval.pointcut) = Manage_OR((yyvsp[(1) - (3)].pointcut), (yyvsp[(3) - (3)].pointcut)); ;}
    break;

  case 8:

    { (yyval.pointcut) = Manage_NOT((yyvsp[(2) - (2)].pointcut)); ;}
    break;

  case 9:

    { (yyval.pointcut) = Manage_AST((yyvsp[(3) - (4)].stringVal)); ;}
    break;

  case 10:

    { (yyval.pointcut) = Manage_ATTRIBUTE(); ;}
    break;

  case 11:

    { (yyval.pointcut) = Manage_CHILD((yyvsp[(3) - (4)].pointcut)); ;}
    break;

  case 12:

    { (yyval.pointcut) = Manage_CHILD((yyvsp[(3) - (6)].pointcut), (yyvsp[(5) - (6)].stringVal)); ;}
    break;

  case 13:

    { (yyval.pointcut) = Manage_PARENT((yyvsp[(3) - (4)].pointcut)); ;}
    break;

  case 14:

    { (yyval.pointcut) = Manage_PARENT((yyvsp[(3) - (6)].pointcut), (yyvsp[(5) - (6)].stringVal)); ;}
    break;

  case 15:

    { (yyval.pointcut) = Manage_DESCENDANT((yyvsp[(3) - (4)].pointcut)); ;}
    break;

  case 16:

    { (yyval.pointcut) = Manage_ASCENDANT((yyvsp[(3) - (4)].pointcut)); ;}
    break;

  case 17:

    { (yyval.pointcut) = (yyvsp[(3) - (4)].pointcut); ;}
    break;

  case 18:

    {;}
    break;

  case 19:

    { (yyval.pointcut) = Manage_FuncPattern((yyvsp[(1) - (5)].stringVal), (yyvsp[(2) - (5)].stringVal), (yyvsp[(4) - (5)].idList)); ;}
    break;

  case 20:

    { (yyval.stringVal) = AST_VALUE_FUNCCLASS_PROGRAMFUNCTION; ;}
    break;

  case 21:

    { (yyval.stringVal) = AST_VALUE_FUNCCLASS_ONEVENT; ;}
    break;

  case 22:

    { (yyval.stringVal) = AST_VALUE_FUNCCLASS_METHOD; ;}
    break;

  case 23:

    { (yyval.stringVal) = "*"; ;}
    break;

  case 24:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 25:

    { (yyval.stringVal) = (yyvsp[(2) - (2)].stringVal); ;}
    break;

  case 26:

    { (yyval.stringVal) = AST_VALUE_TOSTRING_SYMBOLIC_NAME; ;}
    break;

  case 27:

    { (yyval.stringVal) = ""; ;}
    break;

  case 28:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 29:

    { (yyval.stringVal) = "+";			;}
    break;

  case 30:

    { (yyval.stringVal) = "-";			;}
    break;

  case 31:

    { (yyval.stringVal) = "*";			;}
    break;

  case 32:

    { (yyval.stringVal) = "/";			;}
    break;

  case 33:

    { (yyval.stringVal) = "%";			;}
    break;

  case 34:

    { (yyval.stringVal) = ">";			;}
    break;

  case 35:

    { (yyval.stringVal) = "<";			;}
    break;

  case 36:

    { (yyval.stringVal) = "!=";		;}
    break;

  case 37:

    { (yyval.stringVal) = "==";		;}
    break;

  case 38:

    { (yyval.stringVal) = ">=";		;}
    break;

  case 39:

    { (yyval.stringVal) = "<=";		;}
    break;

  case 40:

    { (yyval.stringVal) = "=";			;}
    break;

  case 41:

    { (yyval.stringVal) = "+_";		;}
    break;

  case 42:

    { (yyval.stringVal) = "-_";			;}
    break;

  case 43:

    { (yyval.stringVal) = "*_";		;}
    break;

  case 44:

    { (yyval.stringVal) = "/_";		;}
    break;

  case 45:

    { (yyval.stringVal) = "%_";		;}
    break;

  case 46:

    { (yyval.stringVal) = ">_";		;}
    break;

  case 47:

    { (yyval.stringVal) = "<_";		;}
    break;

  case 48:

    { (yyval.stringVal) = "!=_";		;}
    break;

  case 49:

    { (yyval.stringVal) = "==_";		;}
    break;

  case 50:

    { (yyval.stringVal) = ">=_";		;}
    break;

  case 51:

    { (yyval.stringVal) = "<=_";		;}
    break;

  case 52:

    { (yyval.stringVal) = "()";		;}
    break;

  case 53:

    { (yyval.stringVal) = "=()";		;}
    break;

  case 54:

    { (yyval.stringVal) = DELTA_OPERATOR_OBJECT_GET;	;}
    break;

  case 55:

    { (yyval.stringVal) = DELTA_OPERATOR_OBJECT_SET;	;}
    break;

  case 56:

    { (yyval.idList) = Manage_Formals((yyvsp[(1) - (2)].idList), (yyvsp[(2) - (2)].stringVal)); ;}
    break;

  case 57:

    { (yyval.idList) = (yyvsp[(1) - (1)].idList); ;}
    break;

  case 58:

    { unullify((yyval.idList)); ;}
    break;

  case 59:

    { (yyval.idList) = Manage_FormalPatternList((yyvsp[(1) - (3)].idList), (yyvsp[(3) - (3)].stringVal)); ;}
    break;

  case 60:

    { (yyval.idList) = Manage_FormalPattern((yyvsp[(1) - (1)].stringVal)); ;}
    break;

  case 61:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 62:

    { unullify((yyval.stringVal)); ;}
    break;

  case 63:

    { (yyval.stringVal) = AOP_MATCH_MULTIPLE; ;}
    break;

  case 64:

    { (yyval.stringVal) = AST_VALUE_VARARGS_FORMAL_NAME; ;}
    break;

  case 65:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 66:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 67:

    { /*$$ = Manage_NormalCall($1, $3);*/ ;}
    break;

  case 68:

    {;}
    break;

  case 69:

    {;}
    break;

  case 70:

    {;}
    break;

  case 71:

    { /*$$ = Manage_MethodCall($1, $2, $3, $5);*/ ;}
    break;

  case 72:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 73:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 74:

    { (yyval.stringVal) = AST_TAG_SELF; ;}
    break;

  case 75:

    { (yyval.stringVal) = AST_TAG_LVALUE_NEWSELF; ;}
    break;

  case 76:

    { (yyval.stringVal) = AST_TAG_ARGUMENTS; ;}
    break;

  case 77:

    { (yyval.stringVal) = "."; ;}
    break;

  case 78:

    { (yyval.stringVal) = ".."; ;}
    break;

  case 79:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 80:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal); ;}
    break;

  case 81:

    { (yyval.stringVal) = "function";	;}
    break;

  case 82:

    { (yyval.stringVal) = "onevent";	;}
    break;

  case 83:

    { (yyval.stringVal) = "method";	;}
    break;

  case 84:

    { (yyval.stringVal) = "and";		;}
    break;

  case 85:

    { (yyval.stringVal) = "or";		;}
    break;

  case 86:

    { (yyval.stringVal) = "not";		;}
    break;

  case 87:

    { (yyval.stringVal) = "true";		;}
    break;

  case 88:

    { (yyval.stringVal) = "false";		;}
    break;

  case 89:

    { (yyval.stringVal) = "self";		;}
    break;

  case 90:

    { (yyval.stringVal) = "arguments"; ;}
    break;

  case 91:

    { (yyval.stringVal) = (yyvsp[(1) - (1)].stringVal) + 1; ;}
    break;

  case 92:

    { (yyval.stringVal) = (yyvsp[(2) - (2)].stringVal); ;}
    break;

  case 93:

    {;}
    break;

  case 94:

    {;}
    break;

  case 95:

    {;}
    break;

  case 98:

    {;}
    break;

  case 99:

    {;}
    break;

  case 100:

    { /*unullify($$);*/ ;}
    break;

  case 101:

    {;}
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




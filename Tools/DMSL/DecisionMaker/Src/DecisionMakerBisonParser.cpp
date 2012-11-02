
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
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         DMSL_yyparse
#define yylex           DMSL_yylex
#define yyerror         DMSL_yyerror
#define yylval          DMSL_yylval
#define yychar          DMSL_yychar
#define yydebug         DMSL_yydebug
#define yynerrs         DMSL_yynerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "Src/DecisionMakerParser.y"

#define YY_NEVER_INTERACTIVE 1

//Remove automatically generated file warnings
#ifdef  _MSC_VER
#pragma warning(disable:4065)
#endif

#include <stdio.h>
#include <stdlib.h>

#include "ManageExpressions.h"
#include "ManageStatements.h"
#include "util.h"

using namespace dmsl;

#include "DecisionMakerBisonParser.h"	//must be included after ManageExpressions and ManageStatements

void yyerror (DecisionMaker *dm, Statement**, const char *msg) {
	extern int DMSL_yylineno;
	extern char *DMSL_yytext;
	dm->SetError(util::MakeString("Line %d : %s: before token: %s\n", DMSL_yylineno, msg, DMSL_yytext));
}

extern int yylex (YYSTYPE* lvalp);

#define ABORT_ON_ERROR(x)	do { if(!x) YYABORT; } while(false)


/* Line 189 of yacc.c  */
#line 112 "Src/DecisionMakerBisonParser.cpp"

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
     IF = 258,
     THEN = 259,
     ELSE = 260,
     CASE = 261,
     WHEN = 262,
     OTHERWISE = 263,
     ACTIVATE = 264,
     CANCEL = 265,
     EVALUATE = 266,
     LIBRARY = 267,
     IS_ACTIVE = 268,
     TO_NUMBER = 269,
     HAS_ATTR = 270,
     RANDOM = 271,
     CAN_SUPPORT = 272,
     COMPONENT = 273,
     DMSL_IN = 274,
     PARAMS = 275,
     STEREOTYPE = 276,
     DEFINE = 277,
     EQ = 278,
     NEQ = 279,
     GT = 280,
     LT = 281,
     GE = 282,
     LE = 283,
     AND = 284,
     OR = 285,
     NOT = 286,
     DOUBLEDOT = 287,
     UNARY = 288,
     DMSL_ERROR = 289,
     INT_VALUE = 290,
     REAL_VALUE = 291,
     STRING_VALUE = 292,
     ID_VALUE = 293,
     BOOL_VALUE = 294
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 65 "Src/DecisionMakerParser.y"

  int 			intVal;
  bool 			boolVal;
  double 		realVal;
  const char*	stringVal;
  Expression*	expr;
  ExprList*		exprList;
  Statement*	stmt;
  StmtList*		stmtList;



/* Line 214 of yacc.c  */
#line 200 "Src/DecisionMakerBisonParser.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 212 "Src/DecisionMakerBisonParser.cpp"

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
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   338

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  20
/* YYNRULES -- Number of rules.  */
#define YYNRULES  70
/* YYNRULES -- Number of states.  */
#define YYNSTATES  138

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   294

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    33,     2,     2,
      23,    24,    31,    29,    35,    30,    34,    32,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    36,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    25,     2,    26,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    27,     2,    28,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    15,    20,    24,
      28,    30,    32,    34,    38,    40,    41,    44,    46,    52,
      59,    62,    65,    68,    73,    75,    78,    79,    81,    82,
      85,    87,    91,    94,    95,    97,    99,   103,   107,   111,
     113,   114,   118,   120,   124,   128,   132,   136,   140,   144,
     148,   152,   156,   159,   164,   169,   174,   176,   180,   182,
     186,   190,   194,   198,   202,   206,   209,   212,   217,   222,
     224
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      55,     0,    -1,    56,    -1,    -1,    56,    57,    -1,    57,
      -1,    18,    59,    60,    -1,    21,    59,    36,    72,    -1,
      22,    59,    69,    -1,    58,    34,    59,    -1,    59,    -1,
      51,    -1,    52,    -1,    25,    61,    26,    -1,    62,    -1,
      -1,    62,    63,    -1,    63,    -1,     3,    72,     4,    63,
      64,    -1,     6,    69,    25,    65,    68,    26,    -1,     9,
      69,    -1,    10,    69,    -1,    11,    69,    -1,    12,    23,
      70,    24,    -1,    60,    -1,     5,    63,    -1,    -1,    66,
      -1,    -1,    66,    67,    -1,    67,    -1,     7,    69,    63,
      -1,     8,    63,    -1,    -1,    72,    -1,    73,    -1,    69,
      46,    69,    -1,    27,    70,    28,    -1,    25,    70,    26,
      -1,    71,    -1,    -1,    71,    35,    69,    -1,    69,    -1,
      69,    44,    69,    -1,    69,    43,    69,    -1,    69,    40,
      69,    -1,    69,    42,    69,    -1,    69,    39,    69,    -1,
      69,    41,    69,    -1,    69,    37,    69,    -1,    69,    38,
      69,    -1,    69,    19,    69,    -1,    45,    69,    -1,    13,
      23,    70,    24,    -1,    15,    23,    70,    24,    -1,    17,
      23,    70,    24,    -1,    53,    -1,    23,    69,    24,    -1,
      58,    -1,    20,    34,    58,    -1,    69,    29,    69,    -1,
      69,    30,    69,    -1,    69,    31,    69,    -1,    69,    32,
      69,    -1,    69,    33,    69,    -1,    29,    69,    -1,    30,
      69,    -1,    14,    23,    70,    24,    -1,    16,    23,    70,
      24,    -1,    49,    -1,    50,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    92,    92,    93,    96,    97,   100,   101,   102,   105,
     106,   109,   110,   113,   116,   117,   120,   121,   124,   125,
     126,   127,   128,   129,   130,   133,   134,   137,   138,   141,
     142,   145,   148,   149,   152,   153,   154,   155,   156,   159,
     160,   163,   164,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   182,   183,   184,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IF", "THEN", "ELSE", "CASE", "WHEN",
  "OTHERWISE", "ACTIVATE", "CANCEL", "EVALUATE", "LIBRARY", "IS_ACTIVE",
  "TO_NUMBER", "HAS_ATTR", "RANDOM", "CAN_SUPPORT", "COMPONENT", "DMSL_IN",
  "PARAMS", "STEREOTYPE", "DEFINE", "'('", "')'", "'['", "']'", "'{'",
  "'}'", "'+'", "'-'", "'*'", "'/'", "'%'", "'.'", "','", "':'", "EQ",
  "NEQ", "GT", "LT", "GE", "LE", "AND", "OR", "NOT", "DOUBLEDOT", "UNARY",
  "DMSL_ERROR", "INT_VALUE", "REAL_VALUE", "STRING_VALUE", "ID_VALUE",
  "BOOL_VALUE", "$accept", "logic", "declList", "decl", "dottedId",
  "identifier", "compound", "stmts", "stmtList", "stmt", "elseSt",
  "whenList", "whenListNonEmpty", "when", "otherwise", "expr", "exprList",
  "exprListNonEmpty", "boolexpr", "arithexpr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,    40,    41,    91,    93,   123,   125,    43,
      45,    42,    47,    37,    46,    44,    58,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    54,    55,    55,    56,    56,    57,    57,    57,    58,
      58,    59,    59,    60,    61,    61,    62,    62,    63,    63,
      63,    63,    63,    63,    63,    64,    64,    65,    65,    66,
      66,    67,    68,    68,    69,    69,    69,    69,    69,    70,
      70,    71,    71,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      73
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     3,     4,     3,     3,
       1,     1,     1,     3,     1,     0,     2,     1,     5,     6,
       2,     2,     2,     4,     1,     2,     0,     1,     0,     2,
       1,     3,     2,     0,     1,     1,     3,     3,     3,     1,
       0,     3,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     4,     4,     4,     1,     3,     1,     3,
       3,     3,     3,     3,     3,     2,     2,     4,     4,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     0,     0,     2,     5,    11,    12,     0,
       0,     0,     1,     4,    15,     6,     0,     0,     0,     0,
       0,     0,     0,     0,    40,    40,     0,     0,     0,    69,
      70,    56,    58,    10,     8,    34,    35,     0,     0,     0,
       0,     0,     0,    24,     0,    14,    17,     0,    34,    40,
      40,    40,    40,    40,     0,     0,    42,     0,    39,     0,
      65,    66,    52,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
       0,    20,    21,    22,    40,    13,    16,     0,     0,     0,
       0,     0,    59,    57,    38,     0,    37,     9,    51,    60,
      61,    62,    63,    64,    49,    50,    47,    45,    48,    46,
      44,    43,    36,     0,    28,     0,    53,    67,    54,    68,
      55,    41,    26,     0,    33,    27,    30,    23,     0,    18,
       0,     0,     0,    29,    25,    31,    32,    19
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     6,    32,    33,    43,    44,    45,    46,
     129,   124,   125,   126,   132,    56,    57,    58,    35,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -44
static const yytype_int16 yypact[] =
{
      21,   -38,   -38,   -38,     4,    21,   -44,   -44,   -44,   -16,
     -25,   120,   -44,   -44,    68,   -44,   120,    -5,    -3,    -2,
       2,    17,     7,   120,   120,   120,   120,   120,   120,   -44,
     -44,   -44,    10,   -44,   207,   -44,   -44,   120,   120,   120,
     120,   120,    22,   -44,    20,    68,   -44,   207,     1,   120,
     120,   120,   120,   120,   -38,   155,   207,    23,    12,    24,
     -44,   -44,   122,   -38,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,    44,
     181,   207,   207,   207,   120,   -44,   -44,    26,    27,    48,
      49,    51,    10,   -44,   -44,   120,   -44,   -44,   251,     0,
       0,   -44,   -44,   -44,   269,   269,   274,   274,   274,   274,
     122,   233,   292,    68,    69,    57,   -44,   -44,   -44,   -44,
     -44,   207,    77,   120,    75,    69,   -44,   -44,    68,   -44,
      86,    68,    60,   -44,   -44,   -44,   -44,   -44
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -44,   -44,   -44,    85,    37,     5,    90,   -44,   -44,   -43,
     -44,   -44,   -44,   -31,   -44,   -11,   -15,   -44,   -13,   -44
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -8
static const yytype_int16 yytable[] =
{
      34,    -7,    86,    48,    12,    47,     9,    10,    11,    14,
      59,    16,    55,     7,     8,    60,    61,    62,    49,    -7,
      50,    51,    -7,    -7,    79,    52,    47,    80,    81,    82,
      83,    67,    68,    69,    87,    88,    89,    90,    91,     1,
      53,    54,     2,     3,    63,    84,    85,    95,   113,    94,
     116,   117,    96,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,    97,   115,
     122,    37,   118,   119,    38,   120,   123,    39,    40,    41,
      42,   127,   128,   131,   121,   134,   137,   135,   136,    37,
      13,    92,    38,    14,   133,    39,    40,    41,    42,    15,
       0,     0,     0,     0,     0,    64,     0,     0,     0,     0,
       0,    14,   130,     0,     0,    65,    66,    67,    68,    69,
       0,     0,     0,    70,    71,    72,    73,    74,    75,    76,
      77,     0,    78,    17,    18,    19,    20,    21,     0,     0,
      22,    64,     0,    23,     0,    24,     0,    25,     0,    26,
      27,    65,    66,    67,    68,    69,     0,     0,     0,    70,
      71,    72,    73,    74,    75,    28,     0,     0,    78,    29,
      30,     7,     8,    31,    64,     0,     0,     0,     0,    93,
       0,     0,     0,     0,    65,    66,    67,    68,    69,     0,
       0,     0,    70,    71,    72,    73,    74,    75,    76,    77,
      64,    78,     0,     0,     0,     0,   114,     0,     0,     0,
      65,    66,    67,    68,    69,     0,     0,     0,    70,    71,
      72,    73,    74,    75,    76,    77,    64,    78,     0,     0,
       0,     0,     0,     0,     0,     0,    65,    66,    67,    68,
      69,     0,     0,     0,    70,    71,    72,    73,    74,    75,
      76,    77,    64,    78,     0,     0,     0,     0,     0,     0,
       0,     0,    65,    66,    67,    68,    69,     0,     0,     0,
      70,    71,    72,    73,    74,    75,    76,     0,     0,    78,
      65,    66,    67,    68,    69,     0,     0,     0,    70,    71,
      72,    73,    74,    75,     0,     0,     0,    78,    65,    66,
      67,    68,    69,    65,    66,    67,    68,    69,    72,    73,
      74,    75,     0,     0,     0,    78,     0,     0,     0,     0,
      78,    65,    66,    67,    68,    69,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    -8
};

static const yytype_int16 yycheck[] =
{
      11,     0,    45,    16,     0,    16,     1,     2,     3,    25,
      25,    36,    23,    51,    52,    26,    27,    28,    23,    18,
      23,    23,    21,    22,    37,    23,    37,    38,    39,    40,
      41,    31,    32,    33,    49,    50,    51,    52,    53,    18,
      23,    34,    21,    22,    34,    23,    26,    35,     4,    26,
      24,    24,    28,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    63,    84,
     113,     3,    24,    24,     6,    24,     7,     9,    10,    11,
      12,    24,     5,     8,    95,   128,    26,   130,   131,     3,
       5,    54,     6,    25,   125,     9,    10,    11,    12,     9,
      -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    25,   123,    -1,    -1,    29,    30,    31,    32,    33,
      -1,    -1,    -1,    37,    38,    39,    40,    41,    42,    43,
      44,    -1,    46,    13,    14,    15,    16,    17,    -1,    -1,
      20,    19,    -1,    23,    -1,    25,    -1,    27,    -1,    29,
      30,    29,    30,    31,    32,    33,    -1,    -1,    -1,    37,
      38,    39,    40,    41,    42,    45,    -1,    -1,    46,    49,
      50,    51,    52,    53,    19,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    -1,
      -1,    -1,    37,    38,    39,    40,    41,    42,    43,    44,
      19,    46,    -1,    -1,    -1,    -1,    25,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    -1,    -1,    -1,    37,    38,
      39,    40,    41,    42,    43,    44,    19,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    -1,    -1,    -1,    37,    38,    39,    40,    41,    42,
      43,    44,    19,    46,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    -1,    -1,    -1,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    46,
      29,    30,    31,    32,    33,    -1,    -1,    -1,    37,    38,
      39,    40,    41,    42,    -1,    -1,    -1,    46,    29,    30,
      31,    32,    33,    29,    30,    31,    32,    33,    39,    40,
      41,    42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,
      46,    29,    30,    31,    32,    33,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    18,    21,    22,    55,    56,    57,    51,    52,    59,
      59,    59,     0,    57,    25,    60,    36,    13,    14,    15,
      16,    17,    20,    23,    25,    27,    29,    30,    45,    49,
      50,    53,    58,    59,    69,    72,    73,     3,     6,     9,
      10,    11,    12,    60,    61,    62,    63,    69,    72,    23,
      23,    23,    23,    23,    34,    69,    69,    70,    71,    70,
      69,    69,    69,    34,    19,    29,    30,    31,    32,    33,
      37,    38,    39,    40,    41,    42,    43,    44,    46,    72,
      69,    69,    69,    69,    23,    26,    63,    70,    70,    70,
      70,    70,    58,    24,    26,    35,    28,    59,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,     4,    25,    70,    24,    24,    24,    24,
      24,    69,    63,     7,    65,    66,    67,    24,     5,    64,
      69,     8,    68,    67,    63,    63,    63,    26
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
      yyerror (dm, logic, YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
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
		  Type, Value, dm, logic); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, DecisionMaker *dm, Statement **logic)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, dm, logic)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    DecisionMaker *dm;
    Statement **logic;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (dm);
  YYUSE (logic);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, DecisionMaker *dm, Statement **logic)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, dm, logic)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    DecisionMaker *dm;
    Statement **logic;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, dm, logic);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, DecisionMaker *dm, Statement **logic)
#else
static void
yy_reduce_print (yyvsp, yyrule, dm, logic)
    YYSTYPE *yyvsp;
    int yyrule;
    DecisionMaker *dm;
    Statement **logic;
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
		       		       , dm, logic);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, dm, logic); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, DecisionMaker *dm, Statement **logic)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, dm, logic)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    DecisionMaker *dm;
    Statement **logic;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (dm);
  YYUSE (logic);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 51: /* "STRING_VALUE" */

/* Line 1000 of yacc.c  */
#line 87 "Src/DecisionMakerParser.y"
	{ delete[] (yyvaluep->stringVal); };

/* Line 1000 of yacc.c  */
#line 1280 "Src/DecisionMakerBisonParser.cpp"
	break;
      case 52: /* "ID_VALUE" */

/* Line 1000 of yacc.c  */
#line 87 "Src/DecisionMakerParser.y"
	{ delete[] (yyvaluep->stringVal); };

/* Line 1000 of yacc.c  */
#line 1289 "Src/DecisionMakerBisonParser.cpp"
	break;
      case 58: /* "dottedId" */

/* Line 1000 of yacc.c  */
#line 87 "Src/DecisionMakerParser.y"
	{ delete[] (yyvaluep->stringVal); };

/* Line 1000 of yacc.c  */
#line 1298 "Src/DecisionMakerBisonParser.cpp"
	break;
      case 59: /* "identifier" */

/* Line 1000 of yacc.c  */
#line 87 "Src/DecisionMakerParser.y"
	{ delete[] (yyvaluep->stringVal); };

/* Line 1000 of yacc.c  */
#line 1307 "Src/DecisionMakerBisonParser.cpp"
	break;

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
int yyparse (DecisionMaker *dm, Statement **logic);
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
yyparse (DecisionMaker *dm, Statement **logic)
#else
int
yyparse (dm, logic)
    DecisionMaker *dm;
    Statement **logic;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

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
#line 92 "Src/DecisionMakerParser.y"
    { Manage_logic_declList	(dm, logic, (yyvsp[(1) - (1)].stmtList)); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 93 "Src/DecisionMakerParser.y"
    { Manage_logic_empty	(dm, logic);	 ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 96 "Src/DecisionMakerParser.y"
    { Manage_declList_declList	(dm, &(yyval.stmtList), (yyvsp[(1) - (2)].stmtList), (yyvsp[(2) - (2)].stmt));	ABORT_ON_ERROR((yyval.stmtList));	;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 97 "Src/DecisionMakerParser.y"
    { Manage_declList_decl		(dm, &(yyval.stmtList), (yyvsp[(1) - (1)].stmt));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 100 "Src/DecisionMakerParser.y"
    { Manage_decl_component	(dm, &(yyval.stmt), (yyvsp[(2) - (3)].stringVal), (yyvsp[(3) - (3)].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 101 "Src/DecisionMakerParser.y"
    { Manage_decl_stereotype(dm, &(yyval.stmt), (yyvsp[(2) - (4)].stringVal), (yyvsp[(4) - (4)].expr));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 102 "Src/DecisionMakerParser.y"
    { Manage_decl_define	(dm, &(yyval.stmt), (yyvsp[(2) - (3)].stringVal), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 105 "Src/DecisionMakerParser.y"
    { Manage_dottedId_dottedId	(dm, &(yyval.stringVal), (yyvsp[(1) - (3)].stringVal), (yyvsp[(3) - (3)].stringVal));	ABORT_ON_ERROR((yyval.stringVal)); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 106 "Src/DecisionMakerParser.y"
    { Manage_dottedId_identifier(dm, &(yyval.stringVal), (yyvsp[(1) - (1)].stringVal));		ABORT_ON_ERROR((yyval.stringVal)); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 109 "Src/DecisionMakerParser.y"
    { Manage_identifier_string	(dm, &(yyval.stringVal), (yyvsp[(1) - (1)].stringVal));	ABORT_ON_ERROR((yyval.stringVal)); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 110 "Src/DecisionMakerParser.y"
    { Manage_identifier_id		(dm, &(yyval.stringVal), (yyvsp[(1) - (1)].stringVal));	ABORT_ON_ERROR((yyval.stringVal)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 113 "Src/DecisionMakerParser.y"
    { Manage_compound(dm, &(yyval.stmt), (yyvsp[(2) - (3)].stmtList)); ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 116 "Src/DecisionMakerParser.y"
    { Manage_stmts_stmtList	(dm, &(yyval.stmtList), (yyvsp[(1) - (1)].stmtList));	ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 117 "Src/DecisionMakerParser.y"
    { Manage_stmts_empty	(dm, &(yyval.stmtList));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 120 "Src/DecisionMakerParser.y"
    { Manage_stmtList_stmtList	(dm, &(yyval.stmtList), (yyvsp[(1) - (2)].stmtList), (yyvsp[(2) - (2)].stmt));	ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 121 "Src/DecisionMakerParser.y"
    { Manage_stmtList_stmt		(dm, &(yyval.stmtList), (yyvsp[(1) - (1)].stmt));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 124 "Src/DecisionMakerParser.y"
    { Manage_stmt_if		(dm, &(yyval.stmt), (yyvsp[(2) - (5)].expr), (yyvsp[(4) - (5)].stmt), (yyvsp[(5) - (5)].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 125 "Src/DecisionMakerParser.y"
    { Manage_stmt_case		(dm, &(yyval.stmt), (yyvsp[(2) - (6)].expr), (yyvsp[(4) - (6)].stmtList), (yyvsp[(5) - (6)].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 126 "Src/DecisionMakerParser.y"
    { Manage_stmt_activate	(dm, &(yyval.stmt), (yyvsp[(2) - (2)].expr));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 127 "Src/DecisionMakerParser.y"
    { Manage_stmt_cancel	(dm, &(yyval.stmt), (yyvsp[(2) - (2)].expr));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 128 "Src/DecisionMakerParser.y"
    { Manage_stmt_evaluate	(dm, &(yyval.stmt), (yyvsp[(2) - (2)].expr));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 129 "Src/DecisionMakerParser.y"
    { Manage_stmt_library	(dm, &(yyval.stmt), (yyvsp[(3) - (4)].exprList));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 130 "Src/DecisionMakerParser.y"
    { Manage_stmt_compound	(dm, &(yyval.stmt), (yyvsp[(1) - (1)].stmt));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 133 "Src/DecisionMakerParser.y"
    { Manage_elseSt_else	(dm, &(yyval.stmt), (yyvsp[(2) - (2)].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 134 "Src/DecisionMakerParser.y"
    { Manage_elseSt_empty	(dm, &(yyval.stmt));		ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 137 "Src/DecisionMakerParser.y"
    { Manage_whenList_whenListNonEmpty	(dm, &(yyval.stmtList), (yyvsp[(1) - (1)].stmtList));	ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 138 "Src/DecisionMakerParser.y"
    { Manage_whenList_empty				(dm, &(yyval.stmtList));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 141 "Src/DecisionMakerParser.y"
    { Manage_whenListNonEmpty_whenListNonEmpty	(dm, &(yyval.stmtList), (yyvsp[(1) - (2)].stmtList), (yyvsp[(2) - (2)].stmt));	ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 142 "Src/DecisionMakerParser.y"
    { Manage_whenListNonEmpty_when				(dm, &(yyval.stmtList), (yyvsp[(1) - (1)].stmt));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 145 "Src/DecisionMakerParser.y"
    { Manage_when(dm, &(yyval.stmt), (yyvsp[(2) - (3)].expr), (yyvsp[(3) - (3)].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 148 "Src/DecisionMakerParser.y"
    { Manage_otherwise_otherwise(dm, &(yyval.stmt), (yyvsp[(2) - (2)].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 149 "Src/DecisionMakerParser.y"
    { Manage_otherwise_empty	(dm, &(yyval.stmt));		ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 152 "Src/DecisionMakerParser.y"
    { Manage_expr_boolexpr	(dm, &(yyval.expr), (yyvsp[(1) - (1)].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 153 "Src/DecisionMakerParser.y"
    { Manage_expr_arithexpr	(dm, &(yyval.expr), (yyvsp[(1) - (1)].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 154 "Src/DecisionMakerParser.y"
    { Manage_expr_doubledot	(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 155 "Src/DecisionMakerParser.y"
    { Manage_expr_set		(dm, &(yyval.expr), (yyvsp[(2) - (3)].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 156 "Src/DecisionMakerParser.y"
    { Manage_expr_rangelist	(dm, &(yyval.expr), (yyvsp[(2) - (3)].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 159 "Src/DecisionMakerParser.y"
    { Manage_exprList_exprListNonEmpty	(dm, &(yyval.exprList), (yyvsp[(1) - (1)].exprList));	ABORT_ON_ERROR((yyval.exprList)); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 160 "Src/DecisionMakerParser.y"
    { Manage_exprList_empty				(dm, &(yyval.exprList));		ABORT_ON_ERROR((yyval.exprList)); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 163 "Src/DecisionMakerParser.y"
    { Manage_exprListNonEmpty_exprListNonEmpty	(dm, &(yyval.exprList), (yyvsp[(1) - (3)].exprList), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.exprList)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 164 "Src/DecisionMakerParser.y"
    { Manage_exprListNonEmpty_expr				(dm, &(yyval.exprList), (yyvsp[(1) - (1)].expr));		ABORT_ON_ERROR((yyval.exprList)); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 167 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_or			(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 168 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_and			(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 169 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_lessThan		(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 170 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_lessEqual		(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 171 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_greaterThan	(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 172 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_greaterEqual	(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 173 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_equal			(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 174 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_notEqual		(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 175 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_in			(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 176 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_not			(dm, &(yyval.expr), (yyvsp[(2) - (2)].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 177 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_isactive		(dm, &(yyval.expr), (yyvsp[(3) - (4)].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 178 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_hasattr		(dm, &(yyval.expr), (yyvsp[(3) - (4)].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 179 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_cansupport	(dm, &(yyval.expr), (yyvsp[(3) - (4)].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 180 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_bool			(dm, &(yyval.expr), (yyvsp[(1) - (1)].boolVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 182 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_parenthesis	(dm, &(yyval.expr), (yyvsp[(2) - (3)].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 183 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_dottedId		(dm, &(yyval.expr), (yyvsp[(1) - (1)].stringVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 184 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_params		(dm, &(yyval.expr), (yyvsp[(3) - (3)].stringVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 187 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_add		(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 188 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_sub		(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 189 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_mul		(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 190 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_div		(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 191 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_mod		(dm, &(yyval.expr), (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 192 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_uplus	(dm, &(yyval.expr), (yyvsp[(2) - (2)].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 193 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_uminus	(dm, &(yyval.expr), (yyvsp[(2) - (2)].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 194 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_tonumber	(dm, &(yyval.expr), (yyvsp[(3) - (4)].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 195 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_random	(dm, &(yyval.expr), (yyvsp[(3) - (4)].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 196 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_int		(dm, &(yyval.expr), (yyvsp[(1) - (1)].intVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 197 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_real		(dm, &(yyval.expr), (yyvsp[(1) - (1)].realVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;



/* Line 1455 of yacc.c  */
#line 2095 "Src/DecisionMakerBisonParser.cpp"
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
      yyerror (dm, logic, YY_("syntax error"));
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
	    yyerror (dm, logic, yymsg);
	  }
	else
	  {
	    yyerror (dm, logic, YY_("syntax error"));
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
		      yytoken, &yylval, dm, logic);
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
		  yystos[yystate], yyvsp, dm, logic);
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
  yyerror (dm, logic, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, dm, logic);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, dm, logic);
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
#line 199 "Src/DecisionMakerParser.y"




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
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse DMSL_yyparse
#define yylex   DMSL_yylex
#define yyerror DMSL_yyerror
#define yylval  DMSL_yylval
#define yychar  DMSL_yychar
#define yydebug DMSL_yydebug
#define yynerrs DMSL_yynerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT_VALUE = 258,
     REAL_VALUE = 259,
     BOOL_VALUE = 260,
     STRING_VALUE = 261,
     ID_VALUE = 262,
     IF = 263,
     THEN = 264,
     ELSE = 265,
     CASE = 266,
     WHEN = 267,
     OTHERWISE = 268,
     ACTIVATE = 269,
     CANCEL = 270,
     EVALUATE = 271,
     LIBRARY = 272,
     IS_ACTIVE = 273,
     TO_NUMBER = 274,
     HAS_ATTR = 275,
     RANDOM = 276,
     CAN_SUPPORT = 277,
     COMPONENT = 278,
     DMSL_IN = 279,
     PARAMS = 280,
     STEREOTYPE = 281,
     DEFINE = 282,
     OR = 283,
     AND = 284,
     EQ = 285,
     NEQ = 286,
     GT = 287,
     LT = 288,
     GE = 289,
     LE = 290,
     NOT = 291,
     DOUBLEDOT = 292,
     UNARY = 293,
     DMSL_ERROR = 294
   };
#endif
/* Tokens.  */
#define INT_VALUE 258
#define REAL_VALUE 259
#define BOOL_VALUE 260
#define STRING_VALUE 261
#define ID_VALUE 262
#define IF 263
#define THEN 264
#define ELSE 265
#define CASE 266
#define WHEN 267
#define OTHERWISE 268
#define ACTIVATE 269
#define CANCEL 270
#define EVALUATE 271
#define LIBRARY 272
#define IS_ACTIVE 273
#define TO_NUMBER 274
#define HAS_ATTR 275
#define RANDOM 276
#define CAN_SUPPORT 277
#define COMPONENT 278
#define DMSL_IN 279
#define PARAMS 280
#define STEREOTYPE 281
#define DEFINE 282
#define OR 283
#define AND 284
#define EQ 285
#define NEQ 286
#define GT 287
#define LT 288
#define GE 289
#define LE 290
#define NOT 291
#define DOUBLEDOT 292
#define UNARY 293
#define DMSL_ERROR 294




/* Copy the first part of user declarations.  */
#line 1 "Src/DecisionMakerParser.y"

#if defined(WIN32) || defined(_WIN32_WCE)
#define YY_NO_UNISTD_H
#define YYINCLUDED_STDLIB_H
#endif

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
#line 69 "Src/DecisionMakerParser.y"
typedef union YYSTYPE {
  int 			intVal;
  bool 			boolVal;
  double 		realVal;
  const char*	stringVal;
  Expression*	expr;
  ExprList*		exprList;
  Statement*	stmt;
  StmtList*		stmtList;
} YYSTYPE;
/* Line 196 of yacc.c.  */
#line 215 "Src/DecisionMakerBisonParser.cpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */
#line 227 "Src/DecisionMakerBisonParser.cpp"

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
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
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
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   342

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  20
/* YYNRULES -- Number of rules. */
#define YYNRULES  70
/* YYNRULES -- Number of states. */
#define YYNSTATES  138

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   294

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    38,     2,     2,
      28,    29,    36,    34,    40,    35,    39,    37,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    41,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    30,     2,    31,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,    33,     2,     2,     2,     2,
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
      25,    26,    27,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
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

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      55,     0,    -1,    56,    -1,    -1,    56,    57,    -1,    57,
      -1,    23,    59,    60,    -1,    26,    59,    41,    72,    -1,
      27,    59,    69,    -1,    58,    39,    59,    -1,    59,    -1,
       6,    -1,     7,    -1,    30,    61,    31,    -1,    62,    -1,
      -1,    62,    63,    -1,    63,    -1,     8,    72,     9,    63,
      64,    -1,    11,    69,    30,    65,    68,    31,    -1,    14,
      69,    -1,    15,    69,    -1,    16,    69,    -1,    17,    28,
      70,    29,    -1,    60,    -1,    10,    63,    -1,    -1,    66,
      -1,    -1,    66,    67,    -1,    67,    -1,    12,    69,    63,
      -1,    13,    63,    -1,    -1,    72,    -1,    73,    -1,    69,
      51,    69,    -1,    32,    70,    33,    -1,    30,    70,    31,
      -1,    71,    -1,    -1,    71,    40,    69,    -1,    69,    -1,
      69,    42,    69,    -1,    69,    43,    69,    -1,    69,    47,
      69,    -1,    69,    49,    69,    -1,    69,    46,    69,    -1,
      69,    48,    69,    -1,    69,    44,    69,    -1,    69,    45,
      69,    -1,    69,    24,    69,    -1,    50,    69,    -1,    18,
      28,    70,    29,    -1,    20,    28,    70,    29,    -1,    22,
      28,    70,    29,    -1,     5,    -1,    28,    69,    29,    -1,
      58,    -1,    25,    39,    58,    -1,    69,    34,    69,    -1,
      69,    35,    69,    -1,    69,    36,    69,    -1,    69,    37,
      69,    -1,    69,    38,    69,    -1,    34,    69,    -1,    35,
      69,    -1,    19,    28,    70,    29,    -1,    21,    28,    70,
      29,    -1,     3,    -1,     4,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    96,    96,    97,   100,   101,   104,   105,   106,   109,
     110,   113,   114,   117,   120,   121,   124,   125,   128,   129,
     130,   131,   132,   133,   134,   137,   138,   141,   142,   145,
     146,   149,   152,   153,   156,   157,   158,   159,   160,   163,
     164,   167,   168,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   186,   187,   188,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT_VALUE", "REAL_VALUE", "BOOL_VALUE",
  "STRING_VALUE", "ID_VALUE", "IF", "THEN", "ELSE", "CASE", "WHEN",
  "OTHERWISE", "ACTIVATE", "CANCEL", "EVALUATE", "LIBRARY", "IS_ACTIVE",
  "TO_NUMBER", "HAS_ATTR", "RANDOM", "CAN_SUPPORT", "COMPONENT", "DMSL_IN",
  "PARAMS", "STEREOTYPE", "DEFINE", "'('", "')'", "'['", "']'", "'{'",
  "'}'", "'+'", "'-'", "'*'", "'/'", "'%'", "'.'", "','", "':'", "OR",
  "AND", "EQ", "NEQ", "GT", "LT", "GE", "LE", "NOT", "DOUBLEDOT", "UNARY",
  "DMSL_ERROR", "$accept", "logic", "declList", "decl", "dottedId",
  "identifier", "compound", "stmts", "stmtList", "stmt", "elseSt",
  "whenList", "whenListNonEmpty", "when", "otherwise", "expr", "exprList",
  "exprListNonEmpty", "boolexpr", "arithexpr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,    40,    41,
      91,    93,   123,   125,    43,    45,    42,    47,    37,    46,
      44,    58,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
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
static const unsigned char yyr2[] =
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
static const unsigned char yydefact[] =
{
       3,     0,     0,     0,     0,     2,     5,    11,    12,     0,
       0,     0,     1,     4,    15,     6,     0,    69,    70,    56,
       0,     0,     0,     0,     0,     0,     0,    40,    40,     0,
       0,     0,    58,    10,     8,    34,    35,     0,     0,     0,
       0,     0,     0,    24,     0,    14,    17,     0,    34,    40,
      40,    40,    40,    40,     0,     0,    42,     0,    39,     0,
      65,    66,    52,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
       0,    20,    21,    22,    40,    13,    16,     0,     0,     0,
       0,     0,    59,    57,    38,     0,    37,     9,    51,    60,
      61,    62,    63,    64,    43,    44,    49,    50,    47,    45,
      48,    46,    36,     0,    28,     0,    53,    67,    54,    68,
      55,    41,    26,     0,    33,    27,    30,    23,     0,    18,
       0,     0,     0,    29,    25,    31,    32,    19
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     4,     5,     6,    32,    33,    43,    44,    45,    46,
     129,   124,   125,   126,   132,    56,    57,    58,    35,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -44
static const short int yypact[] =
{
      18,    15,    15,    15,     1,    18,   -44,   -44,   -44,   -26,
     -29,    71,   -44,   -44,   263,   -44,    71,   -44,   -44,   -44,
     -14,    -5,    -3,     3,     4,   -22,    71,    71,    71,    71,
      71,    71,    -6,   -44,   177,   -44,   -44,    71,    71,    71,
      71,    71,    12,   -44,    17,   263,   -44,   177,    16,    71,
      71,    71,    71,    71,    15,   125,   177,    19,     6,    14,
     -44,   -44,   221,    15,    71,    71,    71,    71,    71,    71,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    40,
     151,   177,   177,   177,    71,   -44,   -44,    22,    23,    42,
      43,    44,    -6,   -44,   -44,    71,   -44,   -44,   250,   -27,
     -27,   -44,   -44,   -44,   203,   221,   268,   268,   273,   273,
     273,   273,   291,   263,    67,    51,   -44,   -44,   -44,   -44,
     -44,   177,    72,    71,    68,    67,   -44,   -44,   263,   -44,
      99,   263,    52,   -44,   -44,   -44,   -44,   -44
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -44,   -44,   -44,    81,    41,     5,    85,   -44,   -44,   -43,
     -44,   -44,   -44,   -28,   -44,   -11,   -15,   -44,   -13,   -44
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -8
static const short int yytable[] =
{
      34,    12,    86,    48,    14,    47,     9,    10,    11,    67,
      68,    69,    16,    59,    49,    55,    -7,    54,    60,    61,
      62,     7,     8,    50,    79,    51,    47,    80,    81,    82,
      83,    52,    53,    63,    87,    88,    89,    90,    91,    -7,
      84,     1,    -7,    -7,     2,     3,    95,    96,    85,   113,
      94,   116,   117,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,    97,   115,
     122,   118,   119,   120,    17,    18,    19,     7,     8,   123,
     127,   131,   128,   137,   121,   134,    13,   135,   136,    20,
      21,    22,    23,    24,    15,    92,    25,   133,     0,    26,
       0,    27,     0,    28,     0,    29,    30,    37,     0,     0,
      38,     0,   130,    39,    40,    41,    42,     0,     0,     0,
       0,    31,     0,    64,     0,     0,     0,     0,     0,    14,
       0,     0,     0,    65,    66,    67,    68,    69,     0,     0,
       0,    70,    71,    72,    73,    74,    75,    76,    77,    64,
      78,     0,     0,     0,    93,     0,     0,     0,     0,    65,
      66,    67,    68,    69,     0,     0,     0,    70,    71,    72,
      73,    74,    75,    76,    77,    64,    78,     0,     0,     0,
       0,   114,     0,     0,     0,    65,    66,    67,    68,    69,
       0,     0,     0,    70,    71,    72,    73,    74,    75,    76,
      77,    64,    78,     0,     0,     0,     0,     0,     0,     0,
       0,    65,    66,    67,    68,    69,     0,     0,     0,    70,
      71,    72,    73,    74,    75,    76,    77,    64,    78,     0,
       0,     0,     0,     0,     0,     0,     0,    65,    66,    67,
      68,    69,     0,     0,     0,    64,    71,    72,    73,    74,
      75,    76,    77,     0,    78,    65,    66,    67,    68,    69,
       0,     0,     0,     0,     0,    72,    73,    74,    75,    76,
      77,    37,    78,     0,    38,     0,     0,    39,    40,    41,
      42,     0,     0,     0,    65,    66,    67,    68,    69,     0,
       0,     0,     0,    14,    72,    73,    74,    75,    76,    77,
       0,    78,    65,    66,    67,    68,    69,    65,    66,    67,
      68,    69,     0,     0,    74,    75,    76,    77,     0,    78,
       0,     0,     0,     0,    78,    65,    66,    67,    68,    69,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    -8
};

static const short int yycheck[] =
{
      11,     0,    45,    16,    30,    16,     1,     2,     3,    36,
      37,    38,    41,    28,    28,    26,     0,    39,    29,    30,
      31,     6,     7,    28,    37,    28,    37,    38,    39,    40,
      41,    28,    28,    39,    49,    50,    51,    52,    53,    23,
      28,    23,    26,    27,    26,    27,    40,    33,    31,     9,
      31,    29,    29,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    63,    84,
     113,    29,    29,    29,     3,     4,     5,     6,     7,    12,
      29,    13,    10,    31,    95,   128,     5,   130,   131,    18,
      19,    20,    21,    22,     9,    54,    25,   125,    -1,    28,
      -1,    30,    -1,    32,    -1,    34,    35,     8,    -1,    -1,
      11,    -1,   123,    14,    15,    16,    17,    -1,    -1,    -1,
      -1,    50,    -1,    24,    -1,    -1,    -1,    -1,    -1,    30,
      -1,    -1,    -1,    34,    35,    36,    37,    38,    -1,    -1,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    24,
      51,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,
      35,    36,    37,    38,    -1,    -1,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    24,    51,    -1,    -1,    -1,
      -1,    30,    -1,    -1,    -1,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    24,    51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    34,    35,    36,    37,    38,    -1,    -1,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    24,    51,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    35,    36,
      37,    38,    -1,    -1,    -1,    24,    43,    44,    45,    46,
      47,    48,    49,    -1,    51,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,
      49,     8,    51,    -1,    11,    -1,    -1,    14,    15,    16,
      17,    -1,    -1,    -1,    34,    35,    36,    37,    38,    -1,
      -1,    -1,    -1,    30,    44,    45,    46,    47,    48,    49,
      -1,    51,    34,    35,    36,    37,    38,    34,    35,    36,
      37,    38,    -1,    -1,    46,    47,    48,    49,    -1,    51,
      -1,    -1,    -1,    -1,    51,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    23,    26,    27,    55,    56,    57,     6,     7,    59,
      59,    59,     0,    57,    30,    60,    41,     3,     4,     5,
      18,    19,    20,    21,    22,    25,    28,    30,    32,    34,
      35,    50,    58,    59,    69,    72,    73,     8,    11,    14,
      15,    16,    17,    60,    61,    62,    63,    69,    72,    28,
      28,    28,    28,    28,    39,    69,    69,    70,    71,    70,
      69,    69,    69,    39,    24,    34,    35,    36,    37,    38,
      42,    43,    44,    45,    46,    47,    48,    49,    51,    72,
      69,    69,    69,    69,    28,    31,    63,    70,    70,    70,
      70,    70,    58,    29,    31,    40,    33,    59,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,     9,    30,    70,    29,    29,    29,    29,
      29,    69,    63,    12,    65,    66,    67,    29,    10,    64,
      69,    13,    68,    67,    63,    63,    63,    31
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
      yyerror (dm, logic, YY_("syntax error: cannot back up")); \
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
      case 6: /* "STRING_VALUE" */
#line 91 "Src/DecisionMakerParser.y"
        { delete[] (yyvaluep->stringVal); };
#line 1075 "Src/DecisionMakerBisonParser.cpp"
        break;
      case 7: /* "ID_VALUE" */
#line 91 "Src/DecisionMakerParser.y"
        { delete[] (yyvaluep->stringVal); };
#line 1080 "Src/DecisionMakerBisonParser.cpp"
        break;
      case 58: /* "dottedId" */
#line 91 "Src/DecisionMakerParser.y"
        { delete[] (yyvaluep->stringVal); };
#line 1085 "Src/DecisionMakerBisonParser.cpp"
        break;
      case 59: /* "identifier" */
#line 91 "Src/DecisionMakerParser.y"
        { delete[] (yyvaluep->stringVal); };
#line 1090 "Src/DecisionMakerBisonParser.cpp"
        break;

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
int yyparse (DecisionMaker *dm, Statement **logic);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






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
yyparse (DecisionMaker *dm, Statement **logic)
#else
int
yyparse (dm, logic)
    DecisionMaker *dm;
    Statement **logic;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

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
#line 96 "Src/DecisionMakerParser.y"
    { Manage_logic_declList	(dm, logic, (yyvsp[0].stmtList)); ;}
    break;

  case 3:
#line 97 "Src/DecisionMakerParser.y"
    { Manage_logic_empty	(dm, logic);	 ;}
    break;

  case 4:
#line 100 "Src/DecisionMakerParser.y"
    { Manage_declList_declList	(dm, &(yyval.stmtList), (yyvsp[-1].stmtList), (yyvsp[0].stmt));	ABORT_ON_ERROR((yyval.stmtList));	;}
    break;

  case 5:
#line 101 "Src/DecisionMakerParser.y"
    { Manage_declList_decl		(dm, &(yyval.stmtList), (yyvsp[0].stmt));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 6:
#line 104 "Src/DecisionMakerParser.y"
    { Manage_decl_component	(dm, &(yyval.stmt), (yyvsp[-1].stringVal), (yyvsp[0].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 7:
#line 105 "Src/DecisionMakerParser.y"
    { Manage_decl_stereotype(dm, &(yyval.stmt), (yyvsp[-2].stringVal), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 8:
#line 106 "Src/DecisionMakerParser.y"
    { Manage_decl_define	(dm, &(yyval.stmt), (yyvsp[-1].stringVal), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 9:
#line 109 "Src/DecisionMakerParser.y"
    { Manage_dottedId_dottedId	(dm, &(yyval.stringVal), (yyvsp[-2].stringVal), (yyvsp[0].stringVal));	ABORT_ON_ERROR((yyval.stringVal)); ;}
    break;

  case 10:
#line 110 "Src/DecisionMakerParser.y"
    { Manage_dottedId_identifier(dm, &(yyval.stringVal), (yyvsp[0].stringVal));		ABORT_ON_ERROR((yyval.stringVal)); ;}
    break;

  case 11:
#line 113 "Src/DecisionMakerParser.y"
    { Manage_identifier_string	(dm, &(yyval.stringVal), (yyvsp[0].stringVal));	ABORT_ON_ERROR((yyval.stringVal)); ;}
    break;

  case 12:
#line 114 "Src/DecisionMakerParser.y"
    { Manage_identifier_id		(dm, &(yyval.stringVal), (yyvsp[0].stringVal));	ABORT_ON_ERROR((yyval.stringVal)); ;}
    break;

  case 13:
#line 117 "Src/DecisionMakerParser.y"
    { Manage_compound(dm, &(yyval.stmt), (yyvsp[-1].stmtList)); ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 14:
#line 120 "Src/DecisionMakerParser.y"
    { Manage_stmts_stmtList	(dm, &(yyval.stmtList), (yyvsp[0].stmtList));	ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 15:
#line 121 "Src/DecisionMakerParser.y"
    { Manage_stmts_empty	(dm, &(yyval.stmtList));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 16:
#line 124 "Src/DecisionMakerParser.y"
    { Manage_stmtList_stmtList	(dm, &(yyval.stmtList), (yyvsp[-1].stmtList), (yyvsp[0].stmt));	ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 17:
#line 125 "Src/DecisionMakerParser.y"
    { Manage_stmtList_stmt		(dm, &(yyval.stmtList), (yyvsp[0].stmt));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 18:
#line 128 "Src/DecisionMakerParser.y"
    { Manage_stmt_if		(dm, &(yyval.stmt), (yyvsp[-3].expr), (yyvsp[-1].stmt), (yyvsp[0].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 19:
#line 129 "Src/DecisionMakerParser.y"
    { Manage_stmt_case		(dm, &(yyval.stmt), (yyvsp[-4].expr), (yyvsp[-2].stmtList), (yyvsp[-1].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 20:
#line 130 "Src/DecisionMakerParser.y"
    { Manage_stmt_activate	(dm, &(yyval.stmt), (yyvsp[0].expr));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 21:
#line 131 "Src/DecisionMakerParser.y"
    { Manage_stmt_cancel	(dm, &(yyval.stmt), (yyvsp[0].expr));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 22:
#line 132 "Src/DecisionMakerParser.y"
    { Manage_stmt_evaluate	(dm, &(yyval.stmt), (yyvsp[0].expr));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 23:
#line 133 "Src/DecisionMakerParser.y"
    { Manage_stmt_library	(dm, &(yyval.stmt), (yyvsp[-1].exprList));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 24:
#line 134 "Src/DecisionMakerParser.y"
    { Manage_stmt_compound	(dm, &(yyval.stmt), (yyvsp[0].stmt));			ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 25:
#line 137 "Src/DecisionMakerParser.y"
    { Manage_elseSt_else	(dm, &(yyval.stmt), (yyvsp[0].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 26:
#line 138 "Src/DecisionMakerParser.y"
    { Manage_elseSt_empty	(dm, &(yyval.stmt));		ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 27:
#line 141 "Src/DecisionMakerParser.y"
    { Manage_whenList_whenListNonEmpty	(dm, &(yyval.stmtList), (yyvsp[0].stmtList));	ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 28:
#line 142 "Src/DecisionMakerParser.y"
    { Manage_whenList_empty				(dm, &(yyval.stmtList));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 29:
#line 145 "Src/DecisionMakerParser.y"
    { Manage_whenListNonEmpty_whenListNonEmpty	(dm, &(yyval.stmtList), (yyvsp[-1].stmtList), (yyvsp[0].stmt));	ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 30:
#line 146 "Src/DecisionMakerParser.y"
    { Manage_whenListNonEmpty_when				(dm, &(yyval.stmtList), (yyvsp[0].stmt));		ABORT_ON_ERROR((yyval.stmtList)); ;}
    break;

  case 31:
#line 149 "Src/DecisionMakerParser.y"
    { Manage_when(dm, &(yyval.stmt), (yyvsp[-1].expr), (yyvsp[0].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 32:
#line 152 "Src/DecisionMakerParser.y"
    { Manage_otherwise_otherwise(dm, &(yyval.stmt), (yyvsp[0].stmt));	ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 33:
#line 153 "Src/DecisionMakerParser.y"
    { Manage_otherwise_empty	(dm, &(yyval.stmt));		ABORT_ON_ERROR((yyval.stmt)); ;}
    break;

  case 34:
#line 156 "Src/DecisionMakerParser.y"
    { Manage_expr_boolexpr	(dm, &(yyval.expr), (yyvsp[0].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 35:
#line 157 "Src/DecisionMakerParser.y"
    { Manage_expr_arithexpr	(dm, &(yyval.expr), (yyvsp[0].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 36:
#line 158 "Src/DecisionMakerParser.y"
    { Manage_expr_doubledot	(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 37:
#line 159 "Src/DecisionMakerParser.y"
    { Manage_expr_set		(dm, &(yyval.expr), (yyvsp[-1].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 38:
#line 160 "Src/DecisionMakerParser.y"
    { Manage_expr_rangelist	(dm, &(yyval.expr), (yyvsp[-1].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 39:
#line 163 "Src/DecisionMakerParser.y"
    { Manage_exprList_exprListNonEmpty	(dm, &(yyval.exprList), (yyvsp[0].exprList));	ABORT_ON_ERROR((yyval.exprList)); ;}
    break;

  case 40:
#line 164 "Src/DecisionMakerParser.y"
    { Manage_exprList_empty				(dm, &(yyval.exprList));		ABORT_ON_ERROR((yyval.exprList)); ;}
    break;

  case 41:
#line 167 "Src/DecisionMakerParser.y"
    { Manage_exprListNonEmpty_exprListNonEmpty	(dm, &(yyval.exprList), (yyvsp[-2].exprList), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.exprList)); ;}
    break;

  case 42:
#line 168 "Src/DecisionMakerParser.y"
    { Manage_exprListNonEmpty_expr				(dm, &(yyval.exprList), (yyvsp[0].expr));		ABORT_ON_ERROR((yyval.exprList)); ;}
    break;

  case 43:
#line 171 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_or			(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 44:
#line 172 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_and			(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 45:
#line 173 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_lessThan		(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 46:
#line 174 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_lessEqual		(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 47:
#line 175 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_greaterThan	(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 48:
#line 176 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_greaterEqual	(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 49:
#line 177 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_equal			(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 50:
#line 178 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_notEqual		(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 51:
#line 179 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_in			(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 52:
#line 180 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_not			(dm, &(yyval.expr), (yyvsp[0].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 53:
#line 181 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_isactive		(dm, &(yyval.expr), (yyvsp[-1].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 54:
#line 182 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_hasattr		(dm, &(yyval.expr), (yyvsp[-1].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 55:
#line 183 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_cansupport	(dm, &(yyval.expr), (yyvsp[-1].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 56:
#line 184 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_bool			(dm, &(yyval.expr), (yyvsp[0].boolVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 57:
#line 186 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_parenthesis	(dm, &(yyval.expr), (yyvsp[-1].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 58:
#line 187 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_dottedId		(dm, &(yyval.expr), (yyvsp[0].stringVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 59:
#line 188 "Src/DecisionMakerParser.y"
    { Manage_boolexpr_params		(dm, &(yyval.expr), (yyvsp[0].stringVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 60:
#line 191 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_add		(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 61:
#line 192 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_sub		(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 62:
#line 193 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_mul		(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 63:
#line 194 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_div		(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 64:
#line 195 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_mod		(dm, &(yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr));	ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 65:
#line 196 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_uplus	(dm, &(yyval.expr), (yyvsp[0].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 66:
#line 197 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_uminus	(dm, &(yyval.expr), (yyvsp[0].expr));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 67:
#line 198 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_tonumber	(dm, &(yyval.expr), (yyvsp[-1].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 68:
#line 199 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_random	(dm, &(yyval.expr), (yyvsp[-1].exprList));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 69:
#line 200 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_int		(dm, &(yyval.expr), (yyvsp[0].intVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;

  case 70:
#line 201 "Src/DecisionMakerParser.y"
    { Manage_arithexpr_real		(dm, &(yyval.expr), (yyvsp[0].realVal));		ABORT_ON_ERROR((yyval.expr)); ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 1743 "Src/DecisionMakerBisonParser.cpp"

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
	      yyerror (dm, logic, yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (dm, logic, YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (dm, logic, YY_("syntax error"));
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
  yyerror (dm, logic, YY_("memory exhausted"));
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


#line 203 "Src/DecisionMakerParser.y"




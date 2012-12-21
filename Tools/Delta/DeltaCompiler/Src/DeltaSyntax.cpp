
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
#define yyparse         DeltaSyntax_yyparse
#define yylex           DeltaSyntax_yylex
#define yyerror         DeltaSyntax_yyerror
#define yylval          DeltaSyntax_yylval
#define yychar          DeltaSyntax_yychar
#define yydebug         DeltaSyntax_yydebug
#define yynerrs         DeltaSyntax_yynerrs
#define yylloc          DeltaSyntax_yylloc

/* Copy the first part of user declarations.  */


// Delta syntax directed parsing for AST generation.
// Extended December 2009, added lambda functions.
// ScriptFigher Project.
// A. Savidis, September 2009.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "DDebug.h"
#include "DeltaStdDefs.h"
#include "ASTNode.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"
#include "ASTCreationActions.h"
#include "DescriptiveParseErrorHandler.h"
#include "LibraryNamespace.h"
#include "ParseParms.h"
#include "CompilerStringHolder.h"
#include "ParsingContext.h"

#include "DeltaSyntax.h"

using namespace AST;

#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

extern int DeltaSyntax_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, ParsingContext& ctx);

///////////////////////////////////////////////////////////

#undef DESCRIPTIVE_ERROR_HANDLER
#define DESCRIPTIVE_ERROR_HANDLER	DESCRIPTIVE_ERROR_HANDLER_EX(&ctx)

#undef ASTCREATOR
#define ASTCREATOR					ASTCREATOR_EX(&ctx)

#undef PARSEPARMS
#define PARSEPARMS					PARSEPARMS_EX(&ctx)

#undef STRINGHOLDER
#define STRINGHOLDER				STRINGHOLDER_EX(&ctx)

///////////////////////////////////////////////////////////

static void DeltaSyntax_yyerror (YYLTYPE* yylloc, ParsingContext& ctx, const char* unused)
	{ DESCRIPTIVE_ERROR_HANDLER.HandleSyntaxError(); }

#define	DYNAMIC_STRING(s) \
	STRINGHOLDER.StringWithLateDestruction(ucopystr(s))
void SET_LOCATION(AST::Node* node, const YYLTYPE& start, const YYLTYPE& end) {
	AST::Node::Location location(start.first_line, end.last_line, start.first_column, end.last_column);
	DPTR(node)->SetLocation(location);
}

void SET_LOCATION(YYLTYPE& pos, const YYLTYPE& start, const YYLTYPE& end) {
	pos.first_line = start.first_line;
	pos.last_line = end.last_line;
	pos.first_column = start.first_column;
	pos.last_column = end.last_column;
}

#define	MAKE_FUNCTION_EXPR(_p, _pp)							\
	if (true) {												\
		PE(S_FUNC_(GetFuncClass(_p)));						\
		_pp = ASTCREATOR.MakeNode_FunctionExpression(_p);	\
	} else


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
#define YYFINAL  146
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1935

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  105
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  237
/* YYNRULES -- Number of rules.  */
#define YYNRULES  448
/* YYNRULES -- Number of states.  */
#define YYNSTATES  640

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   349

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      70,    71,     2,     2,    78,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    77,     2,
       2,     2,     2,    76,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    72,     2,    73,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    74,   104,    75,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    11,    12,    13,    14,    15,
      16,    26,    28,    29,    31,    33,    35,    38,    40,    42,
      44,    47,    49,    50,    52,    54,    56,    57,    62,    64,
      65,    67,    68,    72,    74,    76,    77,    82,    83,    85,
      86,    91,    92,    97,    99,   100,   105,   108,   109,   112,
     113,   117,   120,   122,   124,   126,   128,   130,   132,   134,
     136,   138,   140,   142,   144,   146,   148,   150,   152,   154,
     156,   157,   161,   162,   166,   168,   171,   173,   174,   178,
     181,   184,   186,   188,   192,   195,   200,   202,   204,   206,
     208,   210,   212,   214,   215,   216,   217,   225,   226,   231,
     232,   237,   238,   243,   244,   249,   250,   255,   256,   261,
     262,   267,   268,   273,   274,   279,   280,   285,   286,   291,
     292,   297,   298,   303,   304,   309,   310,   315,   316,   321,
     322,   327,   328,   333,   334,   339,   346,   349,   351,   352,
     355,   358,   359,   363,   366,   367,   371,   372,   376,   377,
     381,   383,   385,   387,   389,   391,   393,   395,   397,   399,
     401,   403,   405,   407,   409,   411,   413,   415,   417,   419,
     421,   423,   425,   427,   429,   431,   433,   435,   437,   439,
     441,   443,   445,   447,   449,   451,   453,   455,   457,   459,
     461,   463,   465,   467,   469,   471,   473,   475,   477,   479,
     481,   483,   485,   487,   489,   491,   493,   495,   497,   499,
     501,   503,   505,   508,   510,   512,   514,   516,   519,   520,
     525,   529,   531,   533,   535,   537,   539,   541,   543,   545,
     547,   548,   553,   555,   556,   561,   562,   564,   565,   570,
     572,   573,   574,   580,   582,   583,   586,   587,   588,   594,
     596,   598,   600,   602,   605,   610,   613,   615,   618,   622,
     624,   627,   629,   630,   634,   635,   639,   641,   643,   646,
     649,   652,   655,   657,   659,   661,   663,   665,   667,   669,
     671,   673,   675,   677,   680,   683,   686,   687,   692,   693,
     698,   700,   701,   704,   708,   711,   712,   717,   719,   721,
     723,   725,   726,   730,   733,   736,   739,   740,   744,   746,
     748,   750,   752,   753,   756,   757,   763,   766,   768,   770,
     771,   772,   773,   774,   784,   787,   788,   793,   794,   799,
     804,   807,   809,   811,   813,   815,   817,   818,   822,   824,
     827,   831,   832,   833,   840,   841,   842,   849,   850,   854,
     855,   859,   860,   861,   868,   870,   872,   876,   882,   884,
     889,   895,   896,   900,   904,   907,   908,   913,   914,   919,
     922,   923,   926,   929,   930,   931,   937,   939,   940,   941,
     942,   943,   944,   945,   960,   961,   966,   968,   969,   970,
     971,   980,   983,   985,   991,   995,  1001,  1005,  1007,  1011,
    1013,  1014,  1015,  1023,  1026,  1030,  1031,  1036,  1038,  1040,
    1042,  1044,  1047,  1049,  1050,  1051,  1057,  1059,  1063,  1067,
    1071,  1073,  1075,  1077,  1080,  1082,  1084,  1086,  1088,  1090,
    1093,  1095,  1097,  1099,  1102,  1104,  1106,  1108,  1111,  1112,
    1117,  1120,  1122,  1125,  1128,  1130,  1132,  1134,  1138
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     106,     0,    -1,   107,    -1,   103,   339,    -1,   107,   138,
      -1,    -1,    -1,    -1,    -1,    -1,   114,   110,   113,   116,
     111,   122,   112,   126,   108,    -1,    14,    -1,    -1,     5,
      -1,     7,    -1,    32,    -1,   102,    82,    -1,    82,    -1,
     318,    -1,   115,    -1,    36,   117,    -1,    59,    -1,    -1,
     194,    -1,    46,    -1,    49,    -1,    -1,   118,    78,   119,
     115,    -1,   115,    -1,    -1,    71,    -1,    -1,    48,   121,
      71,    -1,   123,    -1,    79,    -1,    -1,    70,   118,   124,
     120,    -1,    -1,    74,    -1,    -1,   125,   107,   127,    75,
      -1,    -1,   130,   122,   129,   131,    -1,    21,    -1,    -1,
      74,   132,   133,    75,    -1,   108,   154,    -1,    -1,   135,
      81,    -1,    -1,   137,   218,   134,    -1,   108,   139,    -1,
     126,    -1,   109,    -1,    81,    -1,   150,    -1,   341,    -1,
     140,    -1,   141,    -1,   136,    -1,   144,    -1,   148,    -1,
     288,    -1,   289,    -1,   303,    -1,   286,    -1,   277,    -1,
     279,    -1,   313,    -1,   310,    -1,    -1,    25,   143,   154,
      -1,    -1,   145,   142,   134,    -1,     6,    -1,   154,   134,
      -1,   109,    -1,    -1,   146,   149,   147,    -1,   146,   134,
      -1,    15,   151,    -1,   152,    -1,   153,    -1,   219,   115,
      81,    -1,   115,    81,    -1,    60,   115,    81,   108,    -1,
     159,    -1,   155,    -1,   166,    -1,   173,    -1,   176,    -1,
     182,    -1,   187,    -1,    -1,    -1,    -1,    31,   156,   115,
     157,    62,   158,   154,    -1,    -1,   220,    62,   160,   154,
      -1,    -1,   220,    63,   161,   154,    -1,    -1,   220,    65,
     162,   154,    -1,    -1,   220,    64,   163,   154,    -1,    -1,
     220,    66,   164,   154,    -1,    -1,   220,    67,   165,   154,
      -1,    -1,   154,    41,   167,   154,    -1,    -1,   154,    40,
     168,   154,    -1,    -1,   154,    43,   169,   154,    -1,    -1,
     154,    42,   170,   154,    -1,    -1,   154,    44,   171,   154,
      -1,    -1,   154,    45,   172,   154,    -1,    -1,   154,    16,
     174,   154,    -1,    -1,   154,    18,   175,   154,    -1,    -1,
     154,    54,   177,   154,    -1,    -1,   154,    55,   178,   154,
      -1,    -1,   154,    57,   179,   154,    -1,    -1,   154,    56,
     180,   154,    -1,    -1,   154,    58,   181,   154,    -1,    70,
     183,   184,    77,   185,    71,    -1,   154,    76,    -1,   154,
      -1,    -1,   186,   154,    -1,   220,    19,    -1,    -1,    19,
     188,   220,    -1,   220,    20,    -1,    -1,    20,   189,   220,
      -1,    -1,    55,   190,   154,    -1,    -1,    17,   191,   154,
      -1,   192,    -1,   340,    -1,   216,    -1,   193,    -1,   128,
      -1,     3,    -1,    13,    -1,    26,    -1,    27,    -1,    54,
      -1,    55,    -1,    56,    -1,    57,    -1,    58,    -1,    41,
      -1,    40,    -1,    45,    -1,    44,    -1,    43,    -1,    42,
      -1,    62,    -1,    84,    -1,    85,    -1,    86,    -1,    87,
      -1,    88,    -1,    89,    -1,    90,    -1,    91,    -1,    92,
      -1,    93,    -1,    94,    -1,    79,    -1,    80,    -1,     8,
      -1,     9,    -1,     5,    -1,     6,    -1,     7,    -1,    10,
      -1,    11,    -1,    12,    -1,    13,    -1,    14,    -1,    16,
      -1,    17,    -1,    18,    -1,    21,    -1,    22,    -1,    23,
      -1,    24,    -1,    15,    -1,    25,    -1,    26,    -1,    27,
      -1,    28,    -1,    29,    -1,    30,    -1,    31,    -1,    32,
      -1,    33,    -1,    34,    -1,   196,     4,    -1,     4,    -1,
      60,    -1,    82,    -1,   195,    -1,   197,   198,    -1,    -1,
     199,    46,   200,   198,    -1,   197,   219,   115,    -1,   196,
      -1,   199,    -1,   201,    -1,    33,    -1,    35,    -1,    37,
      -1,    34,    -1,   220,    -1,   238,    -1,    -1,    70,   154,
     204,    71,    -1,   217,    -1,    -1,    70,   109,   205,    71,
      -1,    -1,   207,    -1,    -1,   207,    78,   208,   212,    -1,
     212,    -1,    -1,    -1,    70,   210,   206,   211,    71,    -1,
      79,    -1,    -1,   213,   154,    -1,    -1,    -1,   104,   214,
     154,   215,   104,    -1,    48,    -1,   109,    -1,   203,    -1,
     202,    -1,   216,   209,    -1,   218,    78,   108,   154,    -1,
     108,   154,    -1,    61,    -1,   115,    61,    -1,   219,   115,
      61,    -1,   115,    -1,   219,   115,    -1,   267,    -1,    -1,
      30,   221,   115,    -1,    -1,    14,   222,   115,    -1,   232,
      -1,   203,    -1,   223,    46,    -1,   223,    47,    -1,   223,
      72,    -1,   223,    68,    -1,    49,    -1,    50,    -1,    51,
      -1,    52,    -1,    82,    -1,   195,    -1,   194,    -1,   202,
      -1,   228,    -1,   154,    -1,   117,    -1,   224,   229,    -1,
     223,   230,    -1,   225,   229,    -1,    -1,   226,   231,   233,
      73,    -1,    -1,   227,   231,   234,    69,    -1,    72,    -1,
      -1,   237,    73,    -1,   235,   239,   236,    -1,   235,   236,
      -1,    -1,   239,    78,   240,   244,    -1,   244,    -1,   154,
      -1,   242,    -1,   109,    -1,    -1,   243,   241,   108,    -1,
     243,   255,    -1,   243,   270,    -1,   243,   262,    -1,    -1,
      46,   246,   198,    -1,   194,    -1,   154,    -1,   245,    -1,
     247,    -1,    -1,   250,   248,    -1,    -1,   251,   252,    78,
     249,   108,    -1,   249,   108,    -1,   251,    -1,   264,    -1,
      -1,    -1,    -1,    -1,    74,   256,   253,   257,    77,   258,
     254,   259,    75,    -1,   269,    77,    -1,    -1,    46,   261,
     266,    77,    -1,    -1,   260,   263,   265,   108,    -1,   264,
      78,   108,   265,    -1,   108,   265,    -1,   154,    -1,   109,
      -1,    82,    -1,   195,    -1,    83,    -1,    -1,    59,   268,
     266,    -1,   267,    -1,    59,   318,    -1,   269,   271,   274,
      -1,    -1,    -1,    74,   272,    38,   273,   265,   108,    -1,
      -1,    -1,    39,   275,   265,   108,   276,    75,    -1,    -1,
     278,    28,   134,    -1,    -1,   280,    29,   134,    -1,    -1,
      -1,    70,   282,   108,   154,   283,    71,    -1,     8,    -1,
       9,    -1,   284,   281,   138,    -1,   284,   281,   138,   285,
     138,    -1,    10,    -1,   287,   281,   108,   138,    -1,   290,
     292,   293,   295,   138,    -1,    -1,    11,   291,    70,    -1,
     108,   218,   134,    -1,   108,   134,    -1,    -1,   294,   154,
     108,   134,    -1,    -1,   296,   108,   218,   297,    -1,   108,
     297,    -1,    -1,   298,    71,    -1,   108,   154,    -1,    -1,
      -1,    77,   301,   220,   302,    78,    -1,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    12,   304,    70,   305,   220,
     306,   300,   307,   299,   308,    71,   309,   108,   138,    -1,
      -1,    24,   311,   154,   134,    -1,   115,    -1,    -1,    -1,
      -1,    22,   314,   138,   315,    23,   316,   312,   138,    -1,
     317,    99,    -1,    99,    -1,   317,    70,   108,   154,    71,
      -1,   317,   108,    82,    -1,   100,    70,   108,   154,    71,
      -1,   319,    78,   154,    -1,   154,    -1,   320,    78,   248,
      -1,   248,    -1,    -1,    -1,    74,   320,    77,   322,   264,
     323,    75,    -1,   269,    77,    -1,    46,   266,    77,    -1,
      -1,   324,   326,   265,   108,    -1,   109,    -1,   321,    -1,
     270,    -1,   325,    -1,    46,   198,    -1,   247,    -1,    -1,
      -1,   104,   328,   154,   329,   104,    -1,    48,    -1,   319,
      78,   327,    -1,   330,    78,   327,    -1,   330,    78,   154,
      -1,   327,    -1,   330,    -1,   319,    -1,   142,   134,    -1,
     148,    -1,   141,    -1,    81,    -1,   150,    -1,   341,    -1,
     320,   134,    -1,   126,    -1,   109,    -1,   332,    -1,   334,
     333,    -1,   333,    -1,   336,    -1,   109,    -1,   319,   134,
      -1,    -1,    74,   334,   337,    75,    -1,    74,    75,    -1,
     332,    -1,   338,   335,    -1,   335,   335,    -1,   331,    -1,
     336,    -1,   338,    -1,    97,   339,    98,    -1,   101,   138,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   164,   164,   170,   173,   184,   187,   194,   196,   198,
     193,   208,   209,   212,   214,   216,   220,   221,   222,   225,
     227,   229,   234,   237,   238,   239,   243,   242,   246,   249,
     252,   255,   254,   262,   268,   271,   270,   275,   278,   281,
     281,   286,   285,   294,   301,   301,   304,   310,   310,   315,
     315,   320,   324,   325,   326,   327,   328,   331,   332,   335,
     336,   337,   341,   342,   343,   344,   345,   346,   347,   348,
     354,   353,   363,   363,   370,   373,   375,   380,   379,   383,
     389,   392,   393,   396,   398,   403,   410,   411,   412,   413,
     414,   415,   416,   419,   420,   421,   419,   430,   429,   438,
     437,   446,   445,   454,   453,   462,   461,   470,   469,   479,
     478,   487,   486,   495,   494,   503,   502,   511,   510,   519,
     518,   528,   527,   536,   535,   545,   544,   553,   552,   561,
     560,   569,   568,   577,   576,   585,   589,   593,   597,   597,
     602,   608,   607,   615,   621,   620,   629,   628,   637,   636,
     644,   647,   651,   652,   653,   656,   657,   658,   659,   662,
     663,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,   677,   678,   679,   680,   681,   682,
     683,   684,   685,   686,   688,   689,   690,   691,   692,   693,
     694,   695,   696,   697,   698,   699,   700,   701,   702,   703,
     704,   705,   706,   707,   708,   709,   710,   711,   712,   713,
     714,   715,   718,   720,   724,   728,   729,   732,   735,   734,
     740,   748,   750,   752,   759,   764,   769,   774,   779,   780,
     783,   782,   786,   789,   788,   800,   801,   807,   806,   810,
     815,   817,   814,   820,   824,   824,   828,   830,   827,   833,
     835,   841,   842,   845,   853,   855,   864,   870,   874,   880,
     886,   892,   896,   895,   901,   900,   905,   909,   912,   917,
     921,   926,   930,   931,   932,   933,   936,   938,   940,   942,
     946,   950,   952,   958,   963,   968,   974,   973,   978,   977,
     988,   992,   992,   997,  1000,  1005,  1004,  1008,  1012,  1014,
    1018,  1022,  1025,  1026,  1027,  1028,  1032,  1031,  1037,  1041,
    1042,  1043,  1046,  1046,  1052,  1051,  1055,  1059,  1062,  1066,
    1068,  1070,  1072,  1065,  1077,  1080,  1079,  1086,  1085,  1095,
    1097,  1103,  1104,  1110,  1115,  1122,  1128,  1127,  1133,  1134,
    1137,  1142,  1144,  1141,  1150,  1152,  1149,  1160,  1160,  1165,
    1165,  1170,  1171,  1170,  1177,  1178,  1180,  1182,  1191,  1195,
    1205,  1213,  1212,  1218,  1222,  1229,  1229,  1234,  1234,  1239,
    1245,  1245,  1252,  1255,  1256,  1255,  1258,  1261,  1262,  1263,
    1264,  1265,  1266,  1261,  1280,  1279,  1285,  1288,  1289,  1290,
    1288,  1302,  1303,  1306,  1312,  1322,  1329,  1331,  1335,  1336,
    1340,  1342,  1339,  1350,  1351,  1355,  1354,  1365,  1366,  1367,
    1368,  1369,  1370,  1372,  1374,  1371,  1377,  1380,  1381,  1382,
    1383,  1386,  1387,  1390,  1391,  1392,  1393,  1394,  1395,  1398,
    1404,  1405,  1406,  1409,  1411,  1415,  1416,  1419,  1425,  1425,
    1427,  1429,  1433,  1434,  1441,  1442,  1443,  1446,  1450
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMBER_CONST", "STRING_CONST",
  "FUNCTION", "RETURN", "ONEVENT", "IF", "ELSE", "WHILE", "FOR", "FOREACH",
  "NIL", "LOCAL", "USING", "AND", "NOT", "OR", "PLUSPLUS", "MINUSMINUS",
  "LAMBDA", "TRY", "TRAP", "THROW", "ASSERT", "TRUE", "FALSE", "BREAK",
  "CONTINUE", "STATIC", "CONST", "METHOD", "SELF", "ARGUMENTS",
  "LAMBDA_REF", "OPERATOR", "NEWSELF", "SET", "GET", "LT", "GT", "LE",
  "GE", "EQ", "NE", "DOT", "DOUBLE_DOT", "TRIPLE_DOT", "DOT_ASSIGN",
  "DOT_EQUAL", "DOT_CAST", "DOT_EQUAL_RHS", "UMINUS", "ADD", "SUB", "MUL",
  "DIV", "MOD", "ATTRIBUTE", "STRINGIFY", "GLOBAL_SCOPE", "ASSIGN",
  "ADD_A", "SUB_A", "MUL_A", "DIV_A", "MOD_A", "DOUBLE_LB", "DOUBLE_RB",
  "'('", "')'", "'['", "']'", "'{'", "'}'", "'?'", "':'", "','", "CALL",
  "CAST", "SEMI", "IDENT", "ATTRIBUTE_IDENT", "ADD_POSTFIX", "SUB_POSTFIX",
  "MUL_POSTFIX", "DIV_POSTFIX", "MOD_POSTFIX", "GT_POSTFIX", "LT_POSTFIX",
  "NE_POSTFIX", "EQ_POSTFIX", "GE_POSTFIX", "LE_POSTFIX", "PARENTHESIS",
  "SQUARE_BRACKETS", "META_LSHIFT", "META_RSHIFT", "META_ESCAPE",
  "META_INLINE", "META_EXECUTE", "META_RENAME", "PARSE_QUOTED_ELEMENTS",
  "'|'", "$accept", "DeltaCode", "Stmts", "LN", "Function", "$@1", "$@2",
  "$@3", "FuncLinkage", "FunctionClass", "Ident", "FunctionName",
  "OperatorMethod", "IdentList", "$@4", "FormalArgsSuffix", "$@5",
  "FormalArgsNode", "FormalArgs", "$@6", "CompoundOpening", "Compound",
  "$@7", "LambdaFunction", "$@8", "LambdaPrefix", "LambdaCode", "$@9",
  "LambdaStmt", "Semi", "$@10", "ExpressionListStmt", "$@11", "Stmt",
  "BasicStmt", "BasicExprStmt", "BasicNonExprStmt", "Assertion", "$@12",
  "AssertStmt", "$@13", "ReturnPrefix", "ReturnValue", "ReturnStmt",
  "$@14", "UsingDirective", "UsingSpecifications", "UsingNamespace",
  "UsingByteCodeLibrary", "Expression", "ConstDefExpression", "$@15",
  "$@16", "$@17", "AssignExpression", "$@18", "$@19", "$@20", "$@21",
  "$@22", "$@23", "RelationalExpression", "$@24", "$@25", "$@26", "$@27",
  "$@28", "$@29", "BooleanExpression", "$@30", "$@31",
  "ArithmeticExpression", "$@32", "$@33", "$@34", "$@35", "$@36",
  "TernaryExpression", "TernaryCondition", "TernarySelection1",
  "TernarySelection2", "$@37", "Term", "$@38", "$@39", "$@40", "$@41",
  "Primary", "ConstValue", "OpString", "KwdIdent", "StringConst",
  "Stringify", "StringIdent", "StringifyDottedIdents", "$@42",
  "StringifyNamespaceIdent", "StringConstUsed", "FunctionAndTableObject",
  "$@43", "$@44", "ActualArgumentsList", "NonEmptyActualArgumentsList",
  "$@45", "ActualArguments", "$@46", "$@47", "ActualArgument", "$@48",
  "$@49", "$@50", "FunctionCallObject", "FunctionCall", "ExpressionList",
  "NamespacePath", "Lvalue", "$@51", "$@52", "TableObject",
  "TableContentDot", "TableContentBoundedDot", "TableContentBracket",
  "TableContentBoundedBracket", "DottedOpString", "DotIndex",
  "SpecialDotIndex", "BracketIndex", "TableContent", "$@53", "$@54",
  "TablePrefix", "TableSuffix", "$@55", "TableConstructor",
  "TableElements", "$@56", "UnindexedValue", "FunctionElement", "PE_elem",
  "TableElement", "DottedIdent", "$@57", "OperatorIndex", "IndexContent",
  "IndexExpression", "$@58", "IndexedList", "$@59", "TableIndices",
  "TableValues", "IndexedValues", "$@60", "$@61", "$@62", "$@63",
  "IdentIndex", "$@64", "IdentIndexElement", "$@65", "ContentList",
  "ContentExpression", "AttributeIdent", "ConstAttributeId", "$@66",
  "AttributeId", "NewAttribute", "AttributeSet", "$@67", "$@68",
  "AttributeGet", "$@69", "$@70", "BreakStmt", "$@71", "ContinueStmt",
  "$@72", "Condition", "$@73", "$@74", "IfPrefix", "ElsePrefix", "IfStmt",
  "WhilePrefix", "WhileStmt", "ForStmt", "ForPrefix", "$@75",
  "ForInitList", "ForCondition", "$@76", "ForSuffix", "$@77", "ForEnd",
  "$@78", "ForeachCont", "ForeachValue", "$@79", "$@80", "ForeachStmt",
  "$@81", "$@82", "$@83", "$@84", "$@85", "$@86", "ThrowStmt", "$@87",
  "ExceptionVar", "ExceptionStmt", "$@88", "$@89", "$@90",
  "MultipleEscapes", "MetaGeneratedCode", "QuotedExpressionList",
  "QuotedIndexedList", "QuotedIndexedValues", "$@91", "$@92",
  "QuotedIdentIndex", "QuotedIdentIndexElement", "$@93",
  "NonExprQuotedElement", "$@94", "$@95", "QuotedElementList",
  "QuotedElements", "MiscStmt", "NoContextStmt", "NonEmptyStmts",
  "QuotedStmt", "NonFunctionQuotedStmt", "$@96", "QuotedStmts",
  "QuasiQuoted", "MetaExpression", "MetaStmt", 0
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
      40,    41,    91,    93,   123,   125,    63,    58,    44,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   105,   106,   106,   107,   107,   108,   110,   111,   112,
     109,   113,   113,   114,   114,   114,   115,   115,   115,   116,
     116,   116,   116,   117,   117,   117,   119,   118,   118,   118,
     120,   121,   120,   122,   123,   124,   123,   123,   125,   127,
     126,   129,   128,   130,   132,   131,   133,   135,   134,   137,
     136,   138,   138,   138,   138,   138,   138,   139,   139,   140,
     140,   140,   141,   141,   141,   141,   141,   141,   141,   141,
     143,   142,   145,   144,   146,   147,   147,   149,   148,   148,
     150,   151,   151,   152,   152,   153,   154,   154,   154,   154,
     154,   154,   154,   156,   157,   158,   155,   160,   159,   161,
     159,   162,   159,   163,   159,   164,   159,   165,   159,   167,
     166,   168,   166,   169,   166,   170,   166,   171,   166,   172,
     166,   174,   173,   175,   173,   177,   176,   178,   176,   179,
     176,   180,   176,   181,   176,   182,   183,   184,   186,   185,
     187,   188,   187,   187,   189,   187,   190,   187,   191,   187,
     187,   187,   192,   192,   192,   193,   193,   193,   193,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   196,   196,   197,   198,   198,   199,   200,   199,
     201,   202,   202,   202,   203,   203,   203,   203,   203,   203,
     204,   203,   203,   205,   203,   206,   206,   208,   207,   207,
     210,   211,   209,   209,   213,   212,   214,   215,   212,   212,
     212,   216,   216,   217,   218,   218,   219,   219,   219,   220,
     220,   220,   221,   220,   222,   220,   220,   223,   224,   225,
     226,   227,   228,   228,   228,   228,   229,   229,   229,   229,
     230,   231,   231,   232,   232,   232,   233,   232,   234,   232,
     235,   237,   236,   238,   238,   240,   239,   239,   241,   241,
     242,   243,   244,   244,   244,   244,   246,   245,   247,   248,
     248,   248,   250,   249,   252,   251,   251,   253,   254,   256,
     257,   258,   259,   255,   260,   261,   260,   263,   262,   264,
     264,   265,   265,   266,   266,   267,   268,   267,   269,   269,
     270,   272,   273,   271,   275,   276,   274,   278,   277,   280,
     279,   282,   283,   281,   284,   285,   286,   286,   287,   288,
     289,   291,   290,   292,   292,   294,   293,   296,   295,   295,
     298,   297,   299,   301,   302,   300,   300,   304,   305,   306,
     307,   308,   309,   303,   311,   310,   312,   314,   315,   316,
     313,   317,   317,   318,   318,   318,   319,   319,   320,   320,
     322,   323,   321,   324,   324,   326,   325,   327,   327,   327,
     327,   327,   327,   328,   329,   327,   327,   330,   330,   330,
     330,   331,   331,   332,   332,   332,   332,   332,   332,   333,
     333,   333,   333,   334,   334,   335,   335,   336,   337,   336,
     336,   336,   338,   338,   339,   339,   339,   340,   341
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     0,     0,     0,     0,     0,
       9,     1,     0,     1,     1,     1,     2,     1,     1,     1,
       2,     1,     0,     1,     1,     1,     0,     4,     1,     0,
       1,     0,     3,     1,     1,     0,     4,     0,     1,     0,
       4,     0,     4,     1,     0,     4,     2,     0,     2,     0,
       3,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     3,     0,     3,     1,     2,     1,     0,     3,     2,
       2,     1,     1,     3,     2,     4,     1,     1,     1,     1,
       1,     1,     1,     0,     0,     0,     7,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     6,     2,     1,     0,     2,
       2,     0,     3,     2,     0,     3,     0,     3,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     2,     0,     4,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     1,     0,     4,     0,     1,     0,     4,     1,
       0,     0,     5,     1,     0,     2,     0,     0,     5,     1,
       1,     1,     1,     2,     4,     2,     1,     2,     3,     1,
       2,     1,     0,     3,     0,     3,     1,     1,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     2,     0,     4,     0,     4,
       1,     0,     2,     3,     2,     0,     4,     1,     1,     1,
       1,     0,     3,     2,     2,     2,     0,     3,     1,     1,
       1,     1,     0,     2,     0,     5,     2,     1,     1,     0,
       0,     0,     0,     9,     2,     0,     4,     0,     4,     4,
       2,     1,     1,     1,     1,     1,     0,     3,     1,     2,
       3,     0,     0,     6,     0,     0,     6,     0,     3,     0,
       3,     0,     0,     6,     1,     1,     3,     5,     1,     4,
       5,     0,     3,     3,     2,     0,     4,     0,     4,     2,
       0,     2,     2,     0,     0,     5,     1,     0,     0,     0,
       0,     0,     0,    14,     0,     4,     1,     0,     0,     0,
       8,     2,     1,     5,     3,     5,     3,     1,     3,     1,
       0,     0,     7,     2,     3,     0,     4,     1,     1,     1,
       1,     2,     1,     0,     0,     5,     1,     3,     3,     3,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     2,     1,     1,     1,     2,     0,     4,
       2,     1,     2,     2,     1,     1,     1,     3,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       5,   347,     0,     6,   155,   213,    13,    74,    14,   354,
     358,   361,   377,   156,   264,     0,   148,   141,   144,    43,
     387,   384,    70,   157,   158,   262,    93,    15,   224,   227,
     225,   226,   165,   164,   169,   168,   167,   166,     0,   416,
     159,   146,   161,   162,   163,   336,   214,   256,   170,     0,
     290,   347,   182,   183,   426,    17,   335,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   347,   392,
       0,     6,     0,   413,   436,     7,   259,   154,    37,   425,
      47,    77,   424,   427,   397,    87,    86,    88,    89,    90,
      91,    92,   150,   153,   308,   221,     0,   222,   223,   252,
     251,   152,   232,     0,   228,     0,     0,     0,     0,     0,
     266,   301,   229,   412,   261,     0,   409,    66,     0,    67,
       0,     0,    65,     0,    62,    63,     6,    64,    69,    68,
       6,    18,   422,   408,   405,   410,   420,   421,   444,   441,
     347,   435,   446,     3,   151,   428,     1,    38,    54,    49,
      53,     5,    52,     4,    55,    56,     0,     0,     0,     0,
       0,    80,    81,    82,     0,     0,     0,     0,     6,     0,
       0,     0,     0,   186,   187,   188,   184,   185,   189,   190,
     191,   192,   193,   201,   194,   195,   196,   197,   198,   199,
     200,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   215,   216,   411,     0,     0,     0,   339,   146,   336,
     233,   230,     0,   261,   306,   440,   431,   430,   309,   310,
     311,   399,    47,   432,   434,   347,     0,     6,   448,    16,
       0,    12,   257,    29,    34,    41,    33,   423,     0,    79,
       0,   121,   123,   111,   109,   115,   113,   117,   119,   125,
     127,   131,   129,   133,   212,   215,     0,   216,   217,     0,
     218,   240,   243,   253,   260,   140,   143,    97,    99,   103,
     101,   105,   107,   268,   269,   272,   273,   274,   275,   271,
     270,   280,   284,   160,   276,   278,   277,   279,   283,   285,
      24,    25,   282,   281,    23,   286,   288,   294,     0,   291,
       0,   297,   341,   403,     0,    47,    47,   351,     6,     6,
       6,   365,     6,   391,     0,     0,   437,     0,     0,   347,
     436,    47,   443,   435,   442,    59,     6,    51,    57,    58,
      60,     0,    61,     6,   362,   378,   265,     0,    84,     0,
     149,     0,     0,   142,   145,   388,    47,    71,   263,    94,
     404,   147,   333,   334,   337,     0,   136,     0,   137,     0,
       0,   400,     0,   429,    47,   433,     0,   447,     0,   414,
      11,    22,    28,    35,     0,    48,    76,    78,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   220,     0,   244,   258,     0,     0,     0,     0,
       0,     0,     0,     0,   292,   295,   293,   325,   319,   300,
     298,     6,   299,   303,   327,   305,     0,   304,     0,   344,
     340,   348,   350,     6,   356,     6,     0,   364,    47,   367,
       0,     0,   394,     0,   407,   396,   417,   332,   331,     6,
     419,   418,     0,    47,    47,     0,     0,     6,    83,   230,
       0,   385,     0,   234,   231,   138,   215,   307,     6,   398,
     439,     0,     0,     0,    21,    19,     8,    26,     0,    44,
      42,    75,   122,   124,   112,   110,   116,   114,   118,   120,
     126,   128,   132,   130,   134,   219,   249,   246,   250,   241,
     236,   239,     0,    98,   100,   104,   102,   106,   108,   287,
     289,   301,     0,   312,   302,     0,   324,   342,     0,     0,
     355,     6,   359,   255,     6,   363,   370,     6,     6,     6,
       0,     0,   406,    50,    73,    40,   228,    85,   389,    95,
       0,     0,     0,   401,   395,   415,    20,    37,     0,    31,
      30,    36,     6,     0,     0,   237,   245,   296,     0,     6,
       0,   314,   320,     6,     0,     6,   352,   357,     0,   369,
       0,   360,     6,    47,   393,     0,     0,     0,   135,   139,
     330,     6,     0,     9,    27,     0,     0,     0,   247,   242,
     244,   326,   316,   313,     0,     0,   328,     6,   345,     0,
     254,   371,   370,   366,   373,   376,   380,   386,     6,    96,
       0,   402,     0,    32,    46,    45,     0,   238,   312,   321,
     343,     0,   353,   368,     0,     6,   390,   329,     6,   248,
       6,     6,   346,   228,     0,   381,    10,   315,   322,   318,
       0,   372,     0,     0,   375,   382,   323,     6,     6,   383
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,   149,   150,   231,   537,   602,   371,    75,
      76,   466,   292,   373,   538,   541,   575,   235,   236,   468,
     151,   152,   445,    77,   374,    78,   470,   542,   577,   316,
     238,   325,   326,   153,   327,   328,    79,    80,   170,   330,
     331,    81,   377,    82,   240,   154,   161,   162,   163,   218,
      85,   172,   452,   567,    86,   396,   397,   399,   398,   400,
     401,    87,   382,   381,   384,   383,   385,   386,    88,   379,
     380,    89,   387,   388,   390,   389,   391,    90,   212,   359,
     530,   531,    91,   166,   167,   205,   165,    92,    93,    94,
     257,    95,    96,   203,    97,   393,    98,    99,   100,   357,
     355,   489,   490,   580,   263,   394,   544,   491,   492,   543,
     606,   101,   102,   428,   103,   104,   171,   158,   105,   106,
     107,   108,   109,   281,   288,   282,   295,   110,   402,   403,
     111,   297,   298,   112,   299,   501,   411,   412,   300,   301,
     219,   360,   220,   221,   549,   550,   551,   584,   552,   628,
     413,   503,   585,   621,   633,   414,   502,   415,   505,   533,
     439,   204,   213,   206,   115,   116,   304,   418,   554,   420,
     508,   611,   117,   118,   119,   120,   308,   423,   589,   121,
     511,   122,   123,   124,   125,   126,   156,   311,   429,   430,
     517,   518,   559,   560,   625,   596,   614,   630,   127,   157,
     446,   565,   615,   632,   637,   128,   169,   598,   129,   168,
     450,   566,   130,   131,   132,   364,   133,   458,   572,   134,
     135,   317,   136,   230,   462,   137,   138,   139,   224,   225,
     140,   141,   366,   142,   143,   144,   155
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -412
static const yytype_int16 yypact[] =
{
     -76,   605,    33,    30,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,   155,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  1780,  -412,
    -412,    32,  -412,  -412,  -412,    60,  -412,  -412,  -412,  1475,
    -412,   809,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,   605,  -412,
     -19,    80,   -21,  -412,    39,  -412,    17,  -412,   -30,  -412,
    -412,     9,  -412,  -412,   540,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,    95,  1627,    68,  -412,  -412,
     356,    -7,  -412,    16,   109,   810,  1690,  1690,  1001,  1001,
    -412,    63,  -412,  -412,     6,    19,  -412,  -412,    78,  -412,
     113,    81,  -412,    81,  -412,  -412,  -412,  -412,  -412,  -412,
     -34,  -412,    -5,  -412,  -412,  -412,  -412,    70,  -412,  -412,
    1293,    29,  1193,  -412,  -412,  -412,  -412,  -412,  -412,   472,
    -412,  -412,  -412,  -412,  -412,  -412,    92,   122,    16,    16,
     -15,  -412,  -412,  -412,    16,  1529,   191,   191,    80,  1529,
    1529,    16,    16,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,    61,   101,  -412,   103,  1529,  1810,  -412,  -412,  -412,
    -412,   225,  1529,  -412,  -412,  -412,  -412,  -412,   540,  -412,
    -412,  -412,    89,  -412,  -412,   909,   100,  -412,  -412,  -412,
    1529,   179,  -412,    16,  -412,  -412,  -412,  -412,   119,  -412,
    1475,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,   140,    17,  -412,  -412,    16,
    -412,  -412,  -412,  -412,   141,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,   540,  -412,  -412,  -412,  -412,   130,   128,
    1384,  -412,  -412,  -412,   168,  -412,  -412,  -412,    80,  -412,
       9,  -412,  -412,  -412,   127,   707,  -412,  1475,   707,   809,
    -412,   132,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,   183,  -412,    45,  -412,  -412,  -412,   131,  -412,    -2,
    -412,  1475,    -7,   363,   363,  -412,   540,   540,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,   143,  -412,   151,   540,   146,
    1842,  -412,  1093,  -412,   149,  -412,   156,  -412,  1529,   540,
    -412,    97,  -412,   152,   159,  -412,  -412,  -412,   540,  1529,
    1529,  1529,  1529,  1529,  1529,  1529,  1529,  1529,  1529,  1529,
    1529,  1529,   141,  1842,    21,  -412,  1529,  1529,  1529,  1529,
    1529,  1529,   171,   177,  -412,  -412,  -412,  -412,  -412,  -412,
     540,  -412,  -412,  -412,  -412,  -412,    26,  -412,   211,  -412,
    -412,  -412,  -412,  -412,   244,    80,  1529,  -412,   178,   188,
    1529,  1529,  -412,  1093,  -412,   540,  -412,  -412,   540,  -412,
     540,  -412,  1529,   178,  -412,   185,   191,  -412,  -412,   540,
     239,  -412,   202,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,   431,   173,  1841,  -412,  -412,  -412,  -412,   -17,  -412,
    -412,  -412,   548,  1215,   255,   255,   255,   255,  1301,  1301,
     126,   126,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
     194,  -412,  1529,   540,   540,   540,   540,   540,   540,  -412,
    -412,  -412,  1810,  -412,  -412,  1475,  -412,  -412,  1475,  1529,
    -412,    80,  -412,   540,  -412,  -412,  -412,    80,  -412,   540,
     493,    89,  -412,  -412,  -412,  -412,   108,  -412,  -412,  -412,
     204,  1529,  1475,   200,  -412,  -412,  -412,   -30,    16,  -412,
    -412,  -412,  -412,  1529,   213,  -412,   540,  -412,   208,  -412,
    1093,   212,  -412,  -412,  1475,  -412,   540,  -412,  1529,  -412,
     231,  -412,  -412,  -412,  -412,   110,    16,  1529,  -412,   540,
    -412,  -412,   228,  -412,  -412,   233,  1529,   230,   540,  -412,
      36,  -412,  -412,  -412,   237,   229,  -412,  -412,  -412,   250,
     540,  -412,   178,  -412,  -412,  -412,  -412,  -412,    80,   540,
    1475,  -412,   249,  -412,   540,  -412,   223,  -412,  -412,  -412,
    -412,   253,  -412,  -412,   191,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,   251,  1529,  -412,  -412,  -412,  -412,   200,
     252,   540,   263,   264,  -412,  -412,  -412,  -412,    80,  -412
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -412,  -412,   190,   -92,     7,  -412,  -412,  -412,  -412,  -412,
     -14,  -412,  -120,  -412,  -412,  -412,  -412,  -193,  -412,  -412,
    -412,   -48,  -412,  -412,  -412,  -412,  -412,  -412,  -412,   -70,
    -412,  -412,  -412,   -66,  -412,  -412,   196,    15,  -412,  -412,
    -412,  -412,  -412,   201,  -412,    13,  -412,  -412,  -412,    -1,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,   -91,
     -36,  -412,  -412,   -73,  -412,  -412,  -412,    84,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -231,  -412,  -412,
    -412,  -160,  -412,  -304,    27,  -154,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,   245,  -412,   242,  -412,  -412,  -412,
    -412,    54,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -147,
    -412,  -412,    20,  -337,  -252,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -261,
    -411,  -202,    18,  -412,    62,    64,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,   238,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -229,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,  -412,
    -412,  -412,  -412,   320,   -85,   -42,  -412,  -412,  -412,  -412,
    -412,  -412,  -271,  -412,  -412,  -412,  -412,   -31,   144,  -412,
     -18,    -8,  -412,  -412,   300,  -412,    23
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -446
static const yytype_int16 yytable[] =
{
      84,   160,   202,   217,   354,   228,   342,   342,    74,   222,
     237,   239,   343,   344,    83,   285,   285,   294,   294,   114,
     223,   113,   443,   258,   145,   459,     6,     1,     8,  -445,
      -2,   539,  -160,   146,   310,     6,   312,     8,   314,  -407,
     233,     6,   164,     8,   436,    15,   232,   441,   211,   234,
       6,   227,     8,    27,   540,   321,   210,   321,   216,   395,
      15,   229,    27,   261,    83,   313,   338,    84,    27,   486,
     286,   286,   262,   315,   145,    74,   -47,    27,   232,   448,
    -338,    83,   256,  -338,   486,     6,   114,     8,   113,   264,
     -47,   145,  -235,   302,   553,    15,   303,   555,    55,   254,
     302,  -160,   345,   506,   147,  -160,   305,   293,   293,  -160,
    -160,   148,    27,  -160,   260,    69,    70,  -407,    72,   147,
     -39,   570,   322,   259,   324,   487,   148,  -445,   265,   266,
    -160,    71,   323,   463,   323,   368,  -291,  -407,  -333,    84,
     487,    84,   306,   587,   336,   337,    71,   320,   318,   320,
     339,   307,   363,    83,   147,    83,   464,   348,   349,    69,
      70,   148,   334,   145,   340,   145,   361,   362,   346,   347,
     353,   267,   268,   269,   270,   271,   272,   217,  -334,    55,
     350,    71,   251,   252,   253,  -379,  -379,   594,   595,   617,
     287,   287,   335,   370,   223,     5,    69,    70,   367,    72,
     375,   -17,   395,   404,   351,    14,   405,   419,    22,   432,
     442,   358,   447,   583,   453,   159,    47,   425,   426,   372,
     431,    25,   454,   455,    28,    29,    30,   362,    31,   369,
     467,   460,   216,   469,   426,   421,   422,    55,    83,   378,
     427,   241,   424,   242,   499,   392,   500,   376,   145,   507,
     209,    46,    47,   510,    69,    70,   514,    72,   592,    -6,
     525,   341,   528,    50,   529,   243,   244,   245,   246,   247,
     248,   217,   545,    55,    56,   568,   451,   535,   571,   249,
     250,   251,   252,   253,   579,   581,   342,   457,   223,  -317,
      69,    70,   526,    72,   363,  -446,  -446,  -446,  -446,   410,
     548,   356,   591,   601,   603,   605,   609,   409,   471,   249,
     250,   251,   252,   253,   435,   608,   438,   440,   114,   504,
     485,   612,   434,   147,   437,   434,   216,   619,   622,  -374,
     634,   509,    83,   114,   635,   113,   114,   516,   113,   636,
     449,   333,   145,   536,   573,   329,   444,   522,   210,   607,
     332,   296,   289,   406,   547,   527,   620,   465,   515,   512,
     629,   309,   416,   613,   417,   207,   532,   461,   226,   365,
       0,     0,   294,   523,   524,     0,     0,     0,   472,   473,
     474,   475,   476,   477,   478,   479,   480,   481,   482,   483,
     484,   521,     0,     0,     0,   493,   494,   495,   496,   497,
     498,   488,  -267,  -267,     0,  -267,  -267,  -267,  -267,  -228,
    -228,     0,  -228,  -228,  -228,  -228,     0,     0,     0,     0,
       0,     0,   558,     0,  -267,   513,   562,   563,  -267,   519,
     520,  -228,     0,  -228,     0,  -228,     0,     0,     0,     0,
       0,   435,  -228,     0,     0,   557,     0,   241,     0,   242,
     576,   561,     0,     0,   342,     0,     0,   582,     0,     0,
     623,   586,     0,   588,     0,     0,   353,     0,     0,     0,
     426,   243,   244,   245,   246,   247,   248,     0,     7,   600,
       9,     0,    10,    11,    12,   249,   250,   251,   252,   253,
       0,   546,     0,   593,    20,   610,    21,   -72,     0,     0,
    -347,  -349,   534,     0,   438,     0,     0,   438,   556,   241,
       0,   242,   437,     0,     0,   437,     0,     0,     0,     0,
       0,     0,     0,   624,   574,     0,   626,     0,   627,   532,
     569,   438,   616,   243,   244,   245,   246,   247,   248,   437,
       0,     0,   578,     0,     0,   638,     0,   249,   250,   251,
     252,   253,   597,   438,   618,     0,   241,   590,   242,     0,
       0,   437,     0,     0,   564,     0,   599,     0,     0,     0,
       0,     0,   639,     0,     0,   604,     0,     0,     0,     0,
     243,   244,   245,   246,   247,   248,     0,   488,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   438,
       0,     0,   249,   250,   251,   252,   253,   437,     4,     5,
       6,     7,     8,     9,     0,    10,    11,    12,    13,    14,
      15,     0,    16,   631,    17,    18,    19,    20,     0,    21,
      22,    23,    24,     0,  -349,    25,    26,    27,    28,    29,
      30,     0,    31,     0,     0,    32,    33,    34,    35,    36,
      37,    38,     0,    39,     0,     0,     0,     0,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,     0,     0,
       0,     0,     0,     0,     0,    49,     0,    50,     0,    51,
       0,     0,     0,     0,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,     0,    68,     0,    69,    70,    71,    72,     0,    73,
       4,     5,     6,     0,     8,     0,     0,     0,     0,     0,
      13,    14,     0,     0,    16,     0,    17,    18,    19,     0,
       0,     0,     0,    23,    24,     0,     0,    25,    26,    27,
      28,    29,    30,     0,    31,     0,     0,    32,    33,    34,
      35,    36,    37,    38,     0,    39,     0,     0,     0,     0,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
       0,     0,     0,     0,     0,     0,     0,    49,     0,    50,
       0,   433,     0,     0,     0,     0,    52,    53,     0,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,     0,    68,     0,    69,    70,     0,    72,
       0,    73,     4,     5,     6,     7,     8,     9,     0,    10,
      11,    12,    13,    14,    15,     0,    16,     0,    17,    18,
      19,    20,     0,    21,    22,    23,    24,     0,  -349,    25,
      26,    27,    28,    29,    30,     0,    31,     0,     0,    32,
      33,    34,    35,    36,    37,   214,   273,   274,     0,   275,
     276,   277,   278,    40,    41,    42,    43,    44,   209,    46,
      47,    48,     0,     0,     0,     0,     0,     0,   279,    49,
       0,    50,   280,   147,   215,     0,     0,     0,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,     0,    68,     0,    69,    70,
      71,    72,     4,     5,     6,     7,     8,     9,     0,    10,
      11,    12,    13,    14,    15,     0,    16,     0,    17,    18,
      19,    20,     0,    21,    22,    23,    24,     0,  -349,    25,
      26,    27,    28,    29,    30,     0,    31,     0,     0,    32,
      33,    34,    35,    36,    37,   214,     0,     0,     0,     0,
       0,     0,     0,    40,    41,    42,    43,    44,   209,    46,
      47,    48,     0,     0,     0,     0,     0,     0,     0,    49,
       0,    50,     0,   147,  -438,     0,     0,     0,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     4,     5,    68,     0,    69,    70,
      71,    72,     0,     0,    13,    14,     0,     0,    16,     0,
      17,    18,    19,     0,     0,     0,     0,    23,    24,     0,
       0,    25,    26,     0,    28,    29,    30,     0,    31,     0,
       0,    32,    33,    34,    35,    36,    37,   290,     0,     0,
     291,     0,     0,     0,     0,    40,    41,    42,    43,    44,
     209,    46,    47,    48,     0,     0,     0,     0,     0,     0,
       0,    49,     0,    50,     0,     0,     0,     0,     0,     0,
      52,    53,     0,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     4,     5,    68,     0,
      69,    70,     0,    72,     0,     0,    13,    14,     0,     0,
      16,     0,    17,    18,    19,     0,     0,     0,     0,    23,
      24,     0,     0,    25,    26,     0,    28,    29,    30,     0,
      31,     0,     0,    32,    33,    34,    35,    36,    37,   214,
       0,     0,     0,     0,     0,     0,     0,    40,    41,    42,
      43,    44,   209,    46,    47,    48,     0,     0,     0,     0,
       0,     0,     0,    49,     0,    50,     0,     0,     0,     0,
       0,     0,    52,    53,     0,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,     0,
      68,     0,    69,    70,     0,    72,     4,     5,     6,     7,
       8,     9,     0,    10,    11,    12,    13,    14,    15,     0,
      16,     0,    17,    18,    19,    20,     0,    21,    22,    23,
      24,  -347,  -349,    25,    26,    27,    28,    29,    30,     0,
      31,   241,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   208,     0,
       0,     0,   209,    46,    47,   243,   244,   245,   246,   247,
     248,     0,     0,    49,     0,    50,     0,   319,     0,   249,
     250,   251,   252,   253,    54,    55,    56,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      68,     0,    69,    70,    71,    72,     4,     5,     6,     7,
       8,     9,     0,    10,    11,    12,    13,    14,    15,     0,
      16,     0,    17,    18,    19,    20,     0,    21,    22,    23,
      24,     0,  -349,    25,    26,    27,    28,    29,    30,     0,
      31,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   243,   244,   245,   246,  -446,  -446,     0,   208,     0,
       0,     0,   209,    46,    47,   249,   250,   251,   252,   253,
       0,     0,     0,    49,     0,    50,     0,   319,     0,     0,
       0,     0,     0,     0,    54,    55,    56,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,     6,
      68,     8,    69,    70,    71,    72,     0,    13,    14,     0,
       0,    16,     0,    17,    18,    19,     0,     0,     0,     0,
      23,    24,     0,     0,    25,    26,    27,    28,    29,    30,
       0,    31,     0,     0,     0,     0,     0,     0,     0,     0,
     407,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,    49,     0,    50,     0,   408,     0,
       0,     0,     0,     0,     0,     0,    55,    56,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     5,
       6,    68,     8,    69,    70,     0,    72,     0,    13,    14,
       0,     0,    16,     0,    17,    18,    19,     0,     0,     0,
       0,    23,    24,     0,     0,    25,    26,    27,    28,    29,
      30,     0,    31,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     208,     0,     4,     5,   209,    46,    47,     0,     0,     0,
       0,     0,    13,    14,     0,    49,    16,    50,    17,    18,
      19,     0,     0,     0,     0,    23,    24,    55,    56,    25,
      26,     0,    28,    29,    30,     0,    31,     0,     0,     0,
       0,     0,    68,     0,    69,    70,     0,    72,     0,     0,
       0,     0,     0,     0,   208,     0,     0,     0,   209,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,    49,
       0,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    55,    56,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    68,     0,    69,    70,
       0,    72,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,     0,     0,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    47,     0,
       0,     0,     0,     0,     5,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   255,
       0,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,     0,    69,    70,     0,    72,
      32,    33,    34,    35,    36,    37,     0,     0,     0,     0,
       0,     0,     0,     0,    40,   283,    42,    43,    44,     0,
      46,     0,    48,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    52,
      53,     0,   284,     0,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,     0,
       0,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,     0,
       0,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,     0,     0,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,     0,   201,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,     0,     0,     0,
       0,    32,    33,    34,    35,    36,    37,   290,     0,     0,
     291,     0,   352,     0,     0,    40,   283,    42,    43,    44,
       0,     0,     0,    48,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      52,    53,     0,     0,   456,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67
};

static const yytype_int16 yycheck[] =
{
       1,    15,    38,    51,   206,    71,   166,   167,     1,    51,
      80,    81,   166,   167,     1,   106,   107,   108,   109,     1,
      51,     1,   326,    96,     1,   362,     5,   103,     7,     0,
       0,    48,     0,     0,   126,     5,    70,     7,   130,     0,
      70,     5,    15,     7,   315,    15,    61,   318,    49,    79,
       5,    70,     7,    32,    71,   140,    49,   142,    51,    61,
      15,    82,    32,    70,    51,    99,    81,    68,    32,    48,
     106,   107,    79,    78,    51,    68,    81,    32,    61,    81,
      74,    68,    96,    77,    48,     5,    68,     7,    68,   103,
      81,    68,    71,    74,   505,    15,    77,   508,    82,     4,
      74,    69,   168,    77,    74,    73,    28,   108,   109,    77,
      78,    81,    32,    81,    46,    99,   100,    78,   102,    74,
      75,   532,   140,    96,   142,   104,    81,    98,    19,    20,
      98,   101,   140,    36,   142,   227,    73,    98,    77,   140,
     104,   142,    29,   554,   158,   159,   101,   140,    78,   142,
     164,    70,   222,   140,    74,   142,    59,   171,   172,    99,
     100,    81,    70,   140,   165,   142,    77,    78,   169,   170,
     206,    62,    63,    64,    65,    66,    67,   225,    77,    82,
      77,   101,    56,    57,    58,    77,    78,    77,    78,   600,
     106,   107,    70,    14,   225,     4,    99,   100,    98,   102,
      81,    61,    61,    73,   205,    14,    78,    39,    25,    82,
      78,   212,    81,   550,    71,    60,    61,   309,   310,   233,
     312,    30,    71,    77,    33,    34,    35,    78,    37,   230,
      78,    75,   225,    74,   326,   305,   306,    82,   225,   240,
     310,    16,   308,    18,    73,   259,    69,   240,   225,    38,
      59,    60,    61,     9,    99,   100,    78,   102,   562,    71,
      75,    70,    23,    72,    62,    40,    41,    42,    43,    44,
      45,   319,    78,    82,    83,    71,   346,   104,    78,    54,
      55,    56,    57,    58,    71,    77,   446,   360,   319,    77,
      99,   100,   446,   102,   364,    40,    41,    42,    43,   300,
     502,    76,    71,    75,    71,    75,    77,   300,   378,    54,
      55,    56,    57,    58,   315,    78,   317,   318,   300,   411,
     393,    71,   315,    74,   317,   318,   319,   104,    75,    78,
      78,   423,   319,   315,    71,   315,   318,   429,   318,    75,
     341,   151,   319,   463,   537,   149,   331,   439,   341,   580,
     149,   109,   107,   299,   501,   447,   608,   371,   428,   425,
     621,   123,   300,   592,   300,    45,   458,   368,    68,   225,
      -1,    -1,   463,   443,   444,    -1,    -1,    -1,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,   389,   390,
     391,   433,    -1,    -1,    -1,   396,   397,   398,   399,   400,
     401,   394,    46,    47,    -1,    49,    50,    51,    52,    46,
      47,    -1,    49,    50,    51,    52,    -1,    -1,    -1,    -1,
      -1,    -1,   514,    -1,    68,   426,   518,   519,    72,   430,
     431,    68,    -1,    70,    -1,    72,    -1,    -1,    -1,    -1,
      -1,   442,    79,    -1,    -1,   511,    -1,    16,    -1,    18,
     542,   517,    -1,    -1,   614,    -1,    -1,   549,    -1,    -1,
     614,   553,    -1,   555,    -1,    -1,   502,    -1,    -1,    -1,
     562,    40,    41,    42,    43,    44,    45,    -1,     6,   571,
       8,    -1,    10,    11,    12,    54,    55,    56,    57,    58,
      -1,   492,    -1,   563,    22,   587,    24,    25,    -1,    -1,
      28,    29,    71,    -1,   505,    -1,    -1,   508,   509,    16,
      -1,    18,   505,    -1,    -1,   508,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   615,   538,    -1,   618,    -1,   620,   621,
     531,   532,   598,    40,    41,    42,    43,    44,    45,   532,
      -1,    -1,   543,    -1,    -1,   637,    -1,    54,    55,    56,
      57,    58,   566,   554,   602,    -1,    16,   558,    18,    -1,
      -1,   554,    -1,    -1,    71,    -1,   567,    -1,    -1,    -1,
      -1,    -1,   638,    -1,    -1,   576,    -1,    -1,    -1,    -1,
      40,    41,    42,    43,    44,    45,    -1,   580,    40,    41,
      42,    43,    44,    45,    54,    55,    56,    57,    58,   600,
      -1,    -1,    54,    55,    56,    57,    58,   600,     3,     4,
       5,     6,     7,     8,    -1,    10,    11,    12,    13,    14,
      15,    -1,    17,   624,    19,    20,    21,    22,    -1,    24,
      25,    26,    27,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    37,    -1,    -1,    40,    41,    42,    43,    44,
      45,    46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    70,    -1,    72,    -1,    74,
      -1,    -1,    -1,    -1,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      -1,    -1,    97,    -1,    99,   100,   101,   102,    -1,   104,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      13,    14,    -1,    -1,    17,    -1,    19,    20,    21,    -1,
      -1,    -1,    -1,    26,    27,    -1,    -1,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    42,
      43,    44,    45,    46,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    -1,    72,
      -1,    74,    -1,    -1,    -1,    -1,    79,    80,    -1,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    -1,    -1,    97,    -1,    99,   100,    -1,   102,
      -1,   104,     3,     4,     5,     6,     7,     8,    -1,    10,
      11,    12,    13,    14,    15,    -1,    17,    -1,    19,    20,
      21,    22,    -1,    24,    25,    26,    27,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    -1,    -1,    40,
      41,    42,    43,    44,    45,    46,    46,    47,    -1,    49,
      50,    51,    52,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    -1,    -1,    -1,    -1,    -1,    -1,    68,    70,
      -1,    72,    72,    74,    75,    -1,    -1,    -1,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    -1,    -1,    97,    -1,    99,   100,
     101,   102,     3,     4,     5,     6,     7,     8,    -1,    10,
      11,    12,    13,    14,    15,    -1,    17,    -1,    19,    20,
      21,    22,    -1,    24,    25,    26,    27,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    -1,    -1,    40,
      41,    42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,
      -1,    72,    -1,    74,    75,    -1,    -1,    -1,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,     3,     4,    97,    -1,    99,   100,
     101,   102,    -1,    -1,    13,    14,    -1,    -1,    17,    -1,
      19,    20,    21,    -1,    -1,    -1,    -1,    26,    27,    -1,
      -1,    30,    31,    -1,    33,    34,    35,    -1,    37,    -1,
      -1,    40,    41,    42,    43,    44,    45,    46,    -1,    -1,
      49,    -1,    -1,    -1,    -1,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    -1,    72,    -1,    -1,    -1,    -1,    -1,    -1,
      79,    80,    -1,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,     3,     4,    97,    -1,
      99,   100,    -1,   102,    -1,    -1,    13,    14,    -1,    -1,
      17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,
      27,    -1,    -1,    30,    31,    -1,    33,    34,    35,    -1,
      37,    -1,    -1,    40,    41,    42,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    -1,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    79,    80,    -1,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    -1,    -1,
      97,    -1,    99,   100,    -1,   102,     3,     4,     5,     6,
       7,     8,    -1,    10,    11,    12,    13,    14,    15,    -1,
      17,    -1,    19,    20,    21,    22,    -1,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    -1,    59,    60,    61,    40,    41,    42,    43,    44,
      45,    -1,    -1,    70,    -1,    72,    -1,    74,    -1,    54,
      55,    56,    57,    58,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      97,    -1,    99,   100,   101,   102,     3,     4,     5,     6,
       7,     8,    -1,    10,    11,    12,    13,    14,    15,    -1,
      17,    -1,    19,    20,    21,    22,    -1,    24,    25,    26,
      27,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    41,    42,    43,    44,    45,    -1,    55,    -1,
      -1,    -1,    59,    60,    61,    54,    55,    56,    57,    58,
      -1,    -1,    -1,    70,    -1,    72,    -1,    74,    -1,    -1,
      -1,    -1,    -1,    -1,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      97,     7,    99,   100,   101,   102,    -1,    13,    14,    -1,
      -1,    17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,
      26,    27,    -1,    -1,    30,    31,    32,    33,    34,    35,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      -1,    -1,    -1,    59,    60,    61,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    -1,    72,    -1,    74,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    83,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    97,     7,    99,   100,    -1,   102,    -1,    13,    14,
      -1,    -1,    17,    -1,    19,    20,    21,    -1,    -1,    -1,
      -1,    26,    27,    -1,    -1,    30,    31,    32,    33,    34,
      35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,     3,     4,    59,    60,    61,    -1,    -1,    -1,
      -1,    -1,    13,    14,    -1,    70,    17,    72,    19,    20,
      21,    -1,    -1,    -1,    -1,    26,    27,    82,    83,    30,
      31,    -1,    33,    34,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    97,    -1,    99,   100,    -1,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,    59,    60,
      61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,
      -1,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,    99,   100,
      -1,   102,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    -1,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,
      -1,    -1,    -1,    -1,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    82,
      -1,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    99,   100,    -1,   102,
      40,    41,    42,    43,    44,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    55,    56,    57,    58,    -1,
      60,    -1,    62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,
      80,    -1,    82,    -1,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    -1,
      -1,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    -1,
      -1,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    -1,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    -1,    82,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    -1,    -1,    -1,
      -1,    40,    41,    42,    43,    44,    45,    46,    -1,    -1,
      49,    -1,    82,    -1,    -1,    54,    55,    56,    57,    58,
      -1,    -1,    -1,    62,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      79,    80,    -1,    -1,    82,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   103,   106,   107,     3,     4,     5,     6,     7,     8,
      10,    11,    12,    13,    14,    15,    17,    19,    20,    21,
      22,    24,    25,    26,    27,    30,    31,    32,    33,    34,
      35,    37,    40,    41,    42,    43,    44,    45,    46,    48,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    70,
      72,    74,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    97,    99,
     100,   101,   102,   104,   109,   114,   115,   128,   130,   141,
     142,   146,   148,   150,   154,   155,   159,   166,   173,   176,
     182,   187,   192,   193,   194,   196,   197,   199,   201,   202,
     203,   216,   217,   219,   220,   223,   224,   225,   226,   227,
     232,   235,   238,   247,   267,   269,   270,   277,   278,   279,
     280,   284,   286,   287,   288,   289,   290,   303,   310,   313,
     317,   318,   319,   321,   324,   325,   327,   330,   331,   332,
     335,   336,   338,   339,   340,   341,     0,    74,    81,   108,
     109,   125,   126,   138,   150,   341,   291,   304,   222,    60,
     115,   151,   152,   153,   219,   191,   188,   189,   314,   311,
     143,   221,   156,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    82,   195,   198,   266,   190,   268,   318,    55,    59,
     109,   154,   183,   267,    46,    75,   109,   126,   154,   245,
     247,   248,   320,   332,   333,   334,   339,    70,   138,    82,
     328,   110,    61,    70,    79,   122,   123,   134,   135,   134,
     149,    16,    18,    40,    41,    42,    43,    44,    45,    54,
      55,    56,    57,    58,     4,    82,   115,   195,   198,   219,
      46,    70,    79,   209,   115,    19,    20,    62,    63,    64,
      65,    66,    67,    46,    47,    49,    50,    51,    52,    68,
      72,   228,   230,    55,    82,   194,   195,   202,   229,   229,
      46,    49,   117,   154,   194,   231,   231,   236,   237,   239,
     243,   244,    74,    77,   271,    28,    29,    70,   281,   281,
     108,   292,    70,    99,   108,    78,   134,   326,    78,    74,
     109,   319,   335,   336,   335,   136,   137,   139,   140,   141,
     144,   145,   148,   107,    70,    70,   115,   115,    81,   115,
     154,    70,   216,   220,   220,   138,   154,   154,   115,   115,
      77,   154,    82,   195,   266,   205,    76,   204,   154,   184,
     246,    77,    78,   134,   320,   333,   337,    98,   108,   154,
      14,   113,   115,   118,   129,    81,   109,   147,   154,   174,
     175,   168,   167,   170,   169,   171,   172,   177,   178,   180,
     179,   181,   115,   200,   210,    61,   160,   161,   163,   162,
     164,   165,   233,   234,    73,    78,   236,    46,    74,   109,
     154,   241,   242,   255,   260,   262,   269,   270,   272,    39,
     274,   134,   134,   282,   138,   108,   108,   134,   218,   293,
     294,   108,    82,    74,   109,   154,   327,   109,   154,   265,
     154,   327,    78,   218,   142,   127,   305,    81,    81,   154,
     315,   134,   157,    71,    71,    77,    82,   198,   322,   248,
      75,   154,   329,    36,    59,   115,   116,    78,   124,    74,
     131,   134,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   198,    48,   104,   109,   206,
     207,   212,   213,   154,   154,   154,   154,   154,   154,    73,
      69,   240,   261,   256,   108,   263,    77,    38,   275,   108,
       9,   285,   138,   154,    78,   134,   108,   295,   296,   154,
     154,   320,   108,   134,   134,    75,   220,   108,    23,    62,
     185,   186,   108,   264,    71,   104,   117,   111,   119,    48,
      71,   120,   132,   214,   211,    78,   154,   244,   266,   249,
     250,   251,   253,   265,   273,   265,   154,   138,   108,   297,
     298,   138,   108,   108,    71,   306,   316,   158,    71,   154,
     265,    78,   323,   122,   115,   121,   108,   133,   154,    71,
     208,    77,   108,   248,   252,   257,   108,   265,   108,   283,
     154,    71,   218,   134,    77,    78,   300,   115,   312,   154,
     108,    75,   112,    71,   154,    75,   215,   212,    78,    77,
     108,   276,    71,   297,   301,   307,   138,   265,   126,   104,
     249,   258,    75,   220,   108,   299,   108,   108,   254,   264,
     302,   154,   308,   259,    78,    71,    75,   309,   108,   138
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
      yyerror (&yylloc, ctx, YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (&yylval, &yylloc, ctx)
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
		  Type, Value, Location, ctx); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ParsingContext& ctx)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, ctx)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    ParsingContext& ctx;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (ctx);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ParsingContext& ctx)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, ctx)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    ParsingContext& ctx;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, ctx);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, ParsingContext& ctx)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, ctx)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    ParsingContext& ctx;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , ctx);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, ctx); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, ParsingContext& ctx)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, ctx)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    ParsingContext& ctx;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (ctx);

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
int yyparse (ParsingContext& ctx);
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
yyparse (ParsingContext& ctx)
#else
int
yyparse (ctx)
    ParsingContext& ctx;
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

    {
								Node* program = ASTCREATOR.MakeNode_Program((yyvsp[(1) - (1)].node));
								if ((yyvsp[(1) - (1)].node))
									SET_LOCATION(program, (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
								ASTCREATOR.SetSyntaxTree(program);
							;}
    break;

  case 3:

    { ASTCREATOR.SetSyntaxTree((yyvsp[(2) - (2)].node)); ;}
    break;

  case 4:

    {
								(yyval.node) = ASTCREATOR.MakeNode_Stmts((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
								if ((yyvsp[(1) - (2)].node)) {
									SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
									SET_LOCATION((yyloc), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
								}
								else {
									SET_LOCATION((yyval.node), (yylsp[(2) - (2)]), (yylsp[(2) - (2)]));
									SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)]));								
								}
							;}
    break;

  case 5:

    { (yyval.node) = NIL_NODE; ;}
    break;

  case 6:

    { (yyval.line) = PARSEPARMS.GetLine(); ;}
    break;

  case 7:

    { SM(S_FUNC_((yyvsp[(1) - (1)].constStr))); PE2(T_FUNC_((yyvsp[(1) - (1)].constStr)), T_LOCAL_OR_FUNCNAME); ;}
    break;

  case 8:

    { PE(T_LPAR); SG(S_FARGS); ;}
    break;

  case 9:

    { SG(S_BLOCK); ;}
    break;

  case 10:

    {
							EG(S_BLOCK);
							EM(S_FUNC_((yyvsp[(1) - (9)].constStr)));
							(yyval.node) = ASTCREATOR.MakeNode_Function((yyvsp[(1) - (9)].constStr), (yyvsp[(4) - (9)].node), (yyvsp[(3) - (9)].constStr), (yyvsp[(6) - (9)].node), (yyvsp[(8) - (9)].node));
							DPTR((yyval.node))->SetLine((yyvsp[(9) - (9)].line));
							SET_LOCATION((yyval.node), (yylsp[(1) - (9)]), (yylsp[(8) - (9)]));
						;}
    break;

  case 11:

    { (yyval.constStr) = AST_VALUE_FUNCLINKAGE_NO_EXPORT;			;}
    break;

  case 12:

    { (yyval.constStr) = AST_VALUE_FUNCLINKAGE_DEFAULT_EXPORT;	;}
    break;

  case 13:

    { (yyval.constStr) = AST_VALUE_FUNCCLASS_PROGRAMFUNCTION;	;}
    break;

  case 14:

    { (yyval.constStr) = AST_VALUE_FUNCCLASS_ONEVENT;			;}
    break;

  case 15:

    { (yyval.constStr) = AST_VALUE_FUNCCLASS_METHOD;			;}
    break;

  case 16:

    { (yyval.node) = ASTCREATOR.MakeNode_Name((yyvsp[(2) - (2)].constStr), true);	SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 17:

    { (yyval.node) = ASTCREATOR.MakeNode_Name((yyvsp[(1) - (1)].constStr));		SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 18:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 19:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 20:

    { (yyval.node) = ASTCREATOR.MakeNode_Name((yyvsp[(2) - (2)].constStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 21:

    {
								(yyval.node) = ASTCREATOR.MakeNode_Name(AST_VALUE_TOSTRING_SYMBOLIC_NAME);
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 22:

    { unullify((yyval.node)); ;}
    break;

  case 23:

    { (yyval.constStr) = (yyvsp[(1) - (1)].constStr);							;}
    break;

  case 24:

    { (yyval.constStr) = DELTA_OPERATOR_OBJECT_GET;	;}
    break;

  case 25:

    { (yyval.constStr) = DELTA_OPERATOR_OBJECT_SET;	;}
    break;

  case 26:

    { PE2(T_COMMA, T_IDENT); ;}
    break;

  case 27:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (4)].nodeList), (yyvsp[(4) - (4)].node)); ;}
    break;

  case 28:

    { PE(T_IDENT); (yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, (yyvsp[(1) - (1)].node)); ;}
    break;

  case 29:

    { unullify((yyval.nodeList)); ;}
    break;

  case 30:

    { EG(S_FARGS); PE(T_RPAR); unullify((yyval.node)); ;}
    break;

  case 31:

    { EG(S_FARGS); PE2(T_TDOT, T_RPAR); ;}
    break;

  case 32:

    { 
								(yyval.node) = ASTCREATOR.MakeNode_Name(AST_VALUE_VARARGS_FORMAL_NAME);
								SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(1) - (3)]));
							;}
    break;

  case 33:

    {
								(yyval.node) = ASTCREATOR.MakeNode_FormalArgs((yyvsp[(1) - (1)].nodeList));
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 34:

    { EG(S_FARGS); PE(T_RPAR); unullify((yyval.nodeList)); ;}
    break;

  case 35:

    { OE(T_LPAR_OR_TDOT); ;}
    break;

  case 36:

    { (yyval.nodeList) = (yyvsp[(4) - (4)].node) ? ASTCREATOR.MakeNode_List((yyvsp[(2) - (4)].nodeList), (yyvsp[(4) - (4)].node)) : (yyvsp[(2) - (4)].nodeList); ;}
    break;

  case 37:

    { EG(S_FARGS); PE(T_RPAR); unullify((yyval.nodeList)); ;}
    break;

  case 38:

    { PE(T_LBC); SET_LOCATION((yyloc), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 39:

    { PE(T_RBC); ;}
    break;

  case 40:

    {  (yyval.node) = ASTCREATOR.MakeNode_Compound((yyvsp[(2) - (4)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)])); SET_LOCATION((yyloc), (yylsp[(1) - (4)]), (yylsp[(4) - (4)])); ;}
    break;

  case 41:

    { PE(T_LBC); ;}
    break;

  case 42:

    {
							EM(S_FUNC_(AST_VALUE_FUNCCLASS_PROGRAMFUNCTION));
							(yyval.node) = ASTCREATOR.MakeNode_LambdaFunction((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node));
							SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
						;}
    break;

  case 43:

    {
							SM(S_FUNC_(AST_VALUE_FUNCCLASS_PROGRAMFUNCTION)); 
							PE2(T_LAMBDA, T_LPAR); 
							SG(S_FARGS);
						;}
    break;

  case 44:

    { PE(S_EXPR); ;}
    break;

  case 45:

    { (yyval.node) = (yyvsp[(3) - (4)].node); ;}
    break;

  case 46:

    { PE(T_RBC); (yyval.node) = (yyvsp[(2) - (2)].node); DPTR((yyval.node))->SetLine((yyvsp[(1) - (2)].line)); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 47:

    { PE(T_SEMI); ;}
    break;

  case 48:

    { SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 49:

    { SM(S_STMT); ;}
    break;

  case 50:

    { (yyval.node) = ASTCREATOR.MakeNode_ExpressionListStmt((yyvsp[(2) - (3)].nodeList)); SET_LOCATION((yyval.node), (yylsp[(2) - (3)]), (yylsp[(3) - (3)])); SET_LOCATION((yyloc), (yylsp[(2) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 51:

    {
								(yyval.node) = ASTCREATOR.MakeNode_BasicStmt((yyvsp[(2) - (2)].node));  DPTR((yyval.node))->SetLine((yyvsp[(1) - (2)].line));
								SET_LOCATION((yyval.node), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 52:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 53:

    { (yyval.node) = ASTCREATOR.MakeNode_FunctionStmt((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 54:

    { (yyval.node) = ASTCREATOR.MakeNode_EmptyStmt();  SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));;}
    break;

  case 55:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 56:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 57:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 58:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 59:

    { EM(S_STMT); (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 60:

    { EM(S_STMT); (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 61:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 62:

    { (yyval.node) = (yyvsp[(1) - (1)].node);;}
    break;

  case 63:

    { (yyval.node) = (yyvsp[(1) - (1)].node);;}
    break;

  case 64:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 65:

    { (yyval.node) = (yyvsp[(1) - (1)].node);;}
    break;

  case 66:

    { EM(S_STMT); (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 67:

    { EM(S_STMT); (yyval.node) = (yyvsp[(1) - (1)].node);;}
    break;

  case 68:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 69:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 70:

    { SM(S_ASSRT); SG(S_EXPR); ;}
    break;

  case 71:

    {
							EG(S_EXPR);
							PE(S_EXPR);
							EM(S_ASSRT);
							(yyval.node) = ASTCREATOR.MakeNode_AssertStmt((yyvsp[(3) - (3)].node));
							SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
						;}
    break;

  case 72:

    { SM(S_STMT); PE(S_ASSRT);	;}
    break;

  case 73:

    { (yyval.node) = (yyvsp[(2) - (3)].node); SET_LOCATION((yyloc), (yylsp[(2) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 74:

    { SM(S_RET); ;}
    break;

  case 75:

    { (yyval.node) = ASTCREATOR.MakeNode_Return((yyvsp[(1) - (2)].node)); ;}
    break;

  case 76:

    { MAKE_FUNCTION_EXPR((yyvsp[(1) - (1)].node), (yyval.node)); (yyval.node) = ASTCREATOR.MakeNode_Return((yyval.node)); ;}
    break;

  case 77:

    { PE2(T_RET, S_EXPR); ;}
    break;

  case 78:

    { EM(S_RET); (yyval.node) = (yyvsp[(3) - (3)].node); SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 79:

    { EM(S_RET); (yyval.node) = ASTCREATOR.MakeNode_Return(); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 80:

    { (yyval.node) = (yyvsp[(2) - (2)].node); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 81:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 82:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 83:

    { (yyval.node) = ASTCREATOR.MakeNode_UsingNamespace((yyvsp[(1) - (3)].nodeList), (yyvsp[(2) - (3)].node)); ;}
    break;

  case 84:

    { (yyval.node) = ASTCREATOR.MakeNode_UsingNamespace(NIL_NODELIST, (yyvsp[(1) - (2)].node)); ;}
    break;

  case 85:

    { (yyval.node) = ASTCREATOR.MakeNode_UsingByteCodeLibrary((yyvsp[(2) - (4)].node)); DPTR((yyval.node))->SetLine((yyvsp[(4) - (4)].line)); SET_LOCATION((yyloc), (yylsp[(1) - (4)]), (yylsp[(3) - (4)])); ;}
    break;

  case 86:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 87:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 88:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 89:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 90:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 91:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 92:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 93:

    { SM(S_CONST); PE(T_IDENT); ;}
    break;

  case 94:

    { PE(T_ASSIGN); ;}
    break;

  case 95:

    { PE(S_EXPR); ;}
    break;

  case 96:

    {
							EM(S_CONST);
							(yyval.node) = ASTCREATOR.MakeNode_ConstDefExpression((yyvsp[(3) - (7)].node), (yyvsp[(7) - (7)].node));
							SET_LOCATION((yyval.node), (yylsp[(1) - (7)]), (yylsp[(7) - (7)]));
						;}
    break;

  case 97:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ASSIGN, S_EXPR); ;}
    break;

  case 98:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR.MakeNode_AssignExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 99:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ADDA, S_EXPR); ;}
    break;

  case 100:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR.MakeNode_AssignArithExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_ADD_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 101:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MULA, S_EXPR); ;}
    break;

  case 102:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR.MakeNode_AssignArithExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_MUL_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 103:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_SUBA, S_EXPR); ;}
    break;

  case 104:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR.MakeNode_AssignArithExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_SUB_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 105:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_DIVA, S_EXPR); ;}
    break;

  case 106:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR.MakeNode_AssignArithExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_DIV_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 107:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MODA, S_EXPR); ;}
    break;

  case 108:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR.MakeNode_AssignArithExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_MOD_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 109:

    { SM(S_RELAT); PE3(S_EXPR, T_GT, S_EXPR); ;}
    break;

  case 110:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR.MakeNode_RelationalExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_GT);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 111:

    { SM(S_RELAT); PE3(S_EXPR, T_LT, S_EXPR); ;}
    break;

  case 112:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR.MakeNode_RelationalExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_LT);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 113:

    { SM(S_RELAT); PE3(S_EXPR, T_GE, S_EXPR); ;}
    break;

  case 114:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR.MakeNode_RelationalExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node),AST_TAG_OP_GE);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 115:

    { SM(S_RELAT); PE3(S_EXPR, T_LE, S_EXPR); ;}
    break;

  case 116:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR.MakeNode_RelationalExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_LE);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 117:

    { SM(S_RELAT); PE3(S_EXPR, T_EQ, S_EXPR); ;}
    break;

  case 118:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR.MakeNode_RelationalExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_EQ);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 119:

    { SM(S_RELAT); PE3(S_EXPR, T_NE, S_EXPR); ;}
    break;

  case 120:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR.MakeNode_RelationalExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_NE);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 121:

    { SM(S_LOGICAL); PE3(S_EXPR, T_AND, S_EXPR); ;}
    break;

  case 122:

    {
								EM(S_LOGICAL);
								(yyval.node) = ASTCREATOR.MakeNode_ExprANDExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 123:

    { SM(S_LOGICAL); PE3(S_EXPR, T_OR, S_EXPR); ;}
    break;

  case 124:

    {
								EM(S_LOGICAL);
								(yyval.node) = ASTCREATOR.MakeNode_ExprORExpr((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 125:

    { SM(S_ARITH); PE3(S_EXPR, T_ADD, S_EXPR); ;}
    break;

  case 126:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR.MakeNode_ArithmeticExpression((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_ADD);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 127:

    { SM(S_ARITH); PE3(S_EXPR, T_SUB, S_EXPR); ;}
    break;

  case 128:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR.MakeNode_ArithmeticExpression((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_SUB);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 129:

    { SM(S_ARITH); PE3(S_EXPR, T_DIV, S_EXPR); ;}
    break;

  case 130:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR.MakeNode_ArithmeticExpression((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node),AST_TAG_OP_DIV);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 131:

    { SM(S_ARITH); PE3(S_EXPR, T_MUL, S_EXPR); ;}
    break;

  case 132:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR.MakeNode_ArithmeticExpression((yyvsp[(1) - (4)].node),  (yyvsp[(4) - (4)].node), AST_TAG_OP_MUL);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 133:

    { SM(S_ARITH); PE3(S_EXPR, T_MOD, S_EXPR); ;}
    break;

  case 134:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR.MakeNode_ArithmeticExpression((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), AST_TAG_OP_MOD);
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 135:

    { EM(S_TERNARY); (yyval.node) = ASTCREATOR.MakeNode_Ternary((yyvsp[(2) - (6)].node), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (6)]), (yylsp[(6) - (6)])); ;}
    break;

  case 136:

    { SM(S_TERNARY); PE3(S_EXPR, T_QMARK, S_EXPR); (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 137:

    { PE(T_COLON); (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 138:

    { PE(S_EXPR); ;}
    break;

  case 139:

    { PE(T_RPAR); (yyval.node) = (yyvsp[(2) - (2)].node); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 140:

    {
								(yyval.node) = ASTCREATOR.MakeNode_TermLvalueArith((yyvsp[(1) - (2)].node), AST_LVALUE_PLUSPLUS);
								SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 141:

    { SM(S_EXPR); PE2(T_PLUSPLUS, S_LVAL); ;}
    break;

  case 142:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR.MakeNode_TermLvalueArith((yyvsp[(3) - (3)].node), AST_PLUSPLUS_LVALUE);
								SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
							;}
    break;

  case 143:

    {
								(yyval.node) = ASTCREATOR.MakeNode_TermLvalueArith((yyvsp[(1) - (2)].node), AST_LVALUE_MINUSMINUS);
								SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 144:

    { SM(S_EXPR); PE2(T_MINUSMINUS, S_LVAL); ;}
    break;

  case 145:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR.MakeNode_TermLvalueArith((yyvsp[(3) - (3)].node), AST_MINUSMINUS_LVALUE);
								SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
							;}
    break;

  case 146:

    { SM(S_EXPR); PE2(T_SUB, S_EXPR); ;}
    break;

  case 147:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR.MakeNode_UMINUSExpression((yyvsp[(3) - (3)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
							;}
    break;

  case 148:

    { SM(S_EXPR); PE2(T_NOT, S_EXPR); ;}
    break;

  case 149:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR.MakeNode_NOTExpression((yyvsp[(3) - (3)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
							;}
    break;

  case 150:

    { (yyval.node) = ASTCREATOR.MakeNode_PrimaryExpression((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 151:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 152:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 153:

    { (yyval.node) = (yyvsp[(1) - (1)].node); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 154:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 155:

    { (yyval.node) = ASTCREATOR.MakeNode_ConstValue((yyvsp[(1) - (1)].numberConst));		;}
    break;

  case 156:

    { (yyval.node) = ASTCREATOR.MakeNode_ConstValue();		;}
    break;

  case 157:

    { (yyval.node) = ASTCREATOR.MakeNode_ConstValue(true);	;}
    break;

  case 158:

    { (yyval.node) = ASTCREATOR.MakeNode_ConstValue(false);	;}
    break;

  case 159:

    { (yyval.constStr) = "+";			;}
    break;

  case 160:

    { (yyval.constStr) = "-";			;}
    break;

  case 161:

    { (yyval.constStr) = "*";			;}
    break;

  case 162:

    { (yyval.constStr) = "/";			;}
    break;

  case 163:

    { (yyval.constStr) = "%";			;}
    break;

  case 164:

    { (yyval.constStr) = ">";			;}
    break;

  case 165:

    { (yyval.constStr) = "<";			;}
    break;

  case 166:

    { (yyval.constStr) = "!=";		;}
    break;

  case 167:

    { (yyval.constStr) = "==";		;}
    break;

  case 168:

    { (yyval.constStr) = ">=";		;}
    break;

  case 169:

    { (yyval.constStr) = "<=";		;}
    break;

  case 170:

    { (yyval.constStr) = "=";			;}
    break;

  case 171:

    { (yyval.constStr) = "+_";		;}
    break;

  case 172:

    { (yyval.constStr) = "-_";		;}
    break;

  case 173:

    { (yyval.constStr) = "*_";		;}
    break;

  case 174:

    { (yyval.constStr) = "/_";		;}
    break;

  case 175:

    { (yyval.constStr) = "%_";		;}
    break;

  case 176:

    { (yyval.constStr) = ">_";		;}
    break;

  case 177:

    { (yyval.constStr) = "<_";		;}
    break;

  case 178:

    { (yyval.constStr) = "!=_";		;}
    break;

  case 179:

    { (yyval.constStr) = "==_";		;}
    break;

  case 180:

    { (yyval.constStr) = ">=_";		;}
    break;

  case 181:

    { (yyval.constStr) = "<=_";		;}
    break;

  case 182:

    { (yyval.constStr) = "()";		;}
    break;

  case 183:

    { (yyval.constStr) = "=()";		;}
    break;

  case 184:

    { (yyval.constStr) = "if";		;}
    break;

  case 185:

    { (yyval.constStr) = "else";		;}
    break;

  case 186:

    { (yyval.constStr) = "function";	;}
    break;

  case 187:

    { (yyval.constStr) = "return";	;}
    break;

  case 188:

    { (yyval.constStr) = "onevent";	;}
    break;

  case 189:

    { (yyval.constStr) = "while";		;}
    break;

  case 190:

    { (yyval.constStr) = "for";		;}
    break;

  case 191:

    { (yyval.constStr) = "foreach";	;}
    break;

  case 192:

    { (yyval.constStr) = "nil";		;}
    break;

  case 193:

    { (yyval.constStr) = "local";		;}
    break;

  case 194:

    { (yyval.constStr) = "and";		;}
    break;

  case 195:

    { (yyval.constStr) = "not";		;}
    break;

  case 196:

    { (yyval.constStr) = "or";		;}
    break;

  case 197:

    { (yyval.constStr) = "lambda";	;}
    break;

  case 198:

    { (yyval.constStr) = "try";		;}
    break;

  case 199:

    { (yyval.constStr) = "trap";		;}
    break;

  case 200:

    { (yyval.constStr) = "throw";		;}
    break;

  case 201:

    { (yyval.constStr) = "using";		;}
    break;

  case 202:

    { (yyval.constStr) = "assert";	;}
    break;

  case 203:

    { (yyval.constStr) = "true";		;}
    break;

  case 204:

    { (yyval.constStr) = "false";		;}
    break;

  case 205:

    { (yyval.constStr) = "break";		;}
    break;

  case 206:

    { (yyval.constStr) = "continue";	;}
    break;

  case 207:

    { (yyval.constStr) = "static";	;}
    break;

  case 208:

    { (yyval.constStr) = "const";		;}
    break;

  case 209:

    { (yyval.constStr) = "method";	;}
    break;

  case 210:

    { (yyval.constStr) = "self";		;}
    break;

  case 211:

    { (yyval.constStr) = "arguments"; ;}
    break;

  case 212:

    { (yyval.node) = ASTCREATOR.MakeNode_StringConst((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].dynamicStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 213:

    { (yyval.node) = ASTCREATOR.MakeNode_StringConst((yyvsp[(1) - (1)].dynamicStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 214:

    { EI(S_EXPR); PE(T_STRINGIFY); ;}
    break;

  case 215:

    { (yyval.constStr) = (yyvsp[(1) - (1)].constStr); ;}
    break;

  case 216:

    { (yyval.constStr) = (yyvsp[(1) - (1)].constStr); ;}
    break;

  case 217:

    { PE(T_IDENT); (yyval.node) = ASTCREATOR.MakeNode_StringifyDottedIdents((yyvsp[(2) - (2)].constStr)); ;}
    break;

  case 218:

    { PE2(T_DOT, T_IDENT); ;}
    break;

  case 219:

    { (yyval.node) = ASTCREATOR.MakeNode_StringifyDottedIdents((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].constStr)); ;}
    break;

  case 220:

    {
								EG(S_NAMESPACE);
								PE(T_IDENT);
								(yyval.node) = ASTCREATOR.MakeNode_StringifyNamespaceIdent((yyvsp[(2) - (3)].nodeList), (yyvsp[(3) - (3)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
							;}
    break;

  case 221:

    { PE(T_STRINGCONST); (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 222:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 223:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 224:

    {
								EI(S_EXPR); PE(T_SELF);
								(yyval.node) = ASTCREATOR.MakeNode_SELF();
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 225:

    {
								EI(S_EXPR); PE(T_LAMBDA_REF);
								(yyval.node) = ASTCREATOR.MakeNode_LAMBDA_REF();
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 226:

    {
								EI(S_EXPR); PE(T_NEWSELF);
								(yyval.node) = ASTCREATOR.MakeNode_NEWSELF();
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 227:

    {
								EI(S_EXPR); PE(T_ARGUMENTS);
								(yyval.node) = ASTCREATOR.MakeNode_ARGUMENTS();
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 228:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 229:

    { PE(S_TABLE); (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 230:

    { PE3(T_LPAR, S_EXPR, T_RPAR); ;}
    break;

  case 231:

    { (yyval.node) = ASTCREATOR.MakeNode_ParenthesisedExpr((yyvsp[(2) - (4)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(3) - (4)])); ;}
    break;

  case 232:

    { PE(S_CALL); (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 233:

    { SM(S_EXPR); PE3(T_LPAR, S_FUNC, T_RPAR); ;}
    break;

  case 234:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR.MakeNode_FunctionParenthesisForm((yyvsp[(2) - (4)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 235:

    { unullify((yyval.nodeList)); ;}
    break;

  case 236:

    { (yyval.nodeList) = (yyvsp[(1) - (1)].nodeList); ;}
    break;

  case 237:

    { PE(T_COMMA); ;}
    break;

  case 238:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (4)].nodeList), (yyvsp[(4) - (4)].node)); ;}
    break;

  case 239:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, (yyvsp[(1) - (1)].node)); ;}
    break;

  case 240:

    { SM(S_CALL); PE2(S_EXPR, T_LPAR); SG(S_AARGS); ;}
    break;

  case 241:

    { PE(T_RPAR); ;}
    break;

  case 242:

    { EG(S_AARGS); EM(S_CALL); (yyval.nodeList) = (yyvsp[(3) - (5)].nodeList); ;}
    break;

  case 243:

    { PE2(T_LPAR, T_RPAR); unullify((yyval.nodeList)); ;}
    break;

  case 244:

    { PE(S_EXPR); ;}
    break;

  case 245:

    { (yyval.node) = ASTCREATOR.MakeNode_Argument((yyvsp[(2) - (2)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 246:

    { PE2(T_BAR, S_EXPR); ;}
    break;

  case 247:

    { PE(T_BAR); ;}
    break;

  case 248:

    { (yyval.node) = ASTCREATOR.MakeNode_LateBoundArgument((yyvsp[(3) - (5)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (5)]), (yylsp[(5) - (5)])); ;}
    break;

  case 249:

    { PE(T_TDOT); (yyval.node) = ASTCREATOR.MakeNode_TRIPLE_DOT(); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 250:

    { MAKE_FUNCTION_EXPR((yyvsp[(1) - (1)].node), (yyval.node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 251:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 252:

    { EI(S_EXPR); (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 253:

    {
							Node* actuals = ASTCREATOR.MakeNode_ActualArguments((yyvsp[(2) - (2)].nodeList));
							SET_LOCATION(actuals, (yylsp[(2) - (2)]), (yylsp[(2) - (2)]));
							(yyval.node) = ASTCREATOR.MakeNode_FunctionCall((yyvsp[(1) - (2)].node), actuals);
							SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
						;}
    break;

  case 254:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (4)].nodeList), (yyvsp[(4) - (4)].node)); DPTR((yyvsp[(4) - (4)].node))->SetLine((yyvsp[(3) - (4)].line)); ;}
    break;

  case 255:

    {
								(yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, (yyvsp[(2) - (2)].node)); DPTR((yyvsp[(2) - (2)].node))->SetLine((yyvsp[(1) - (2)].line));
								SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 256:

    {
								EI(S_EXPR); SG(S_NAMESPACE); PE2(T_GLOBAL, T_IDENT);
								Node* global = ASTCREATOR.MakeNode_Name(DELTA_LIBRARYNAMESPACE_SEPARATOR);
								SET_LOCATION(global, (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
								(yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, global);
							;}
    break;

  case 257:

    {
								EI(S_EXPR); SG(S_NAMESPACE); PE3(T_IDENT, T_GLOBAL, T_IDENT);
								(yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, (yyvsp[(1) - (2)].node));
							;}
    break;

  case 258:

    {
								PE2(T_GLOBAL, T_IDENT);
								(yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (3)].nodeList), (yyvsp[(2) - (3)].node));
							;}
    break;

  case 259:

    {
								EI(S_EXPR); PE(T_IDENT);
								(yyval.node) = ASTCREATOR.MakeNode_Lvalue((yyvsp[(1) - (1)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 260:

    {
								EG(S_NAMESPACE);
								(yyval.node) = ASTCREATOR.MakeNode_NamespaceLvalue((yyvsp[(1) - (2)].nodeList), (yyvsp[(2) - (2)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 261:

    { (yyval.node) = ASTCREATOR.MakeNode_AttrLvalue((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 262:

    { EI(S_EXPR); PE2(T_STATIC, T_IDENT); ;}
    break;

  case 263:

    { (yyval.node) = ASTCREATOR.MakeNode_StaticLvalue((yyvsp[(3) - (3)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 264:

    { EI(S_EXPR); PE2(T_LOCAL, T_IDENT); ;}
    break;

  case 265:

    { (yyval.node) = ASTCREATOR.MakeNode_LocalLvalue((yyvsp[(3) - (3)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 266:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 267:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 268:

    { PE(T_DOT); OE(S_DOTINDEX); (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 269:

    { PE(T_DDOT); OE(S_DOTINDEX); (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 270:

    { PE(T_LB); OE(S_BRACKET_INDEX); (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 271:

    { PE(T_DLB); OE(S_BRACKET_INDEX); (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 272:

    { (yyval.constStr) = ".=";	;}
    break;

  case 273:

    { (yyval.constStr) = ".==";	;}
    break;

  case 274:

    { (yyval.constStr) = ".=()";	;}
    break;

  case 275:

    { (yyval.constStr) = ".==_";	;}
    break;

  case 276:

    { (yyval.node) = ASTCREATOR.MakeNode_DotIndexIdent((yyvsp[(1) - (1)].constStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 277:

    { (yyval.node) = ASTCREATOR.MakeNode_DotIndexIdent((yyvsp[(1) - (1)].constStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 278:

    { (yyval.node) = ASTCREATOR.MakeNode_DotIndexOpString((yyvsp[(1) - (1)].constStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 279:

    { (yyval.node) = ASTCREATOR.MakeNode_DotIndexStringConst((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 280:

    { (yyval.node) = ASTCREATOR.MakeNode_DotIndexOpString((yyvsp[(1) - (1)].constStr)+1); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));  ;}
    break;

  case 281:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 282:

    { (yyval.node) = ASTCREATOR.MakeNode_BracketIndexOperatorMethod((yyvsp[(1) - (1)].constStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));  ;}
    break;

  case 283:

    {
								PE(S_DOTINDEX_(DPTR((yyvsp[(2) - (2)].node))->GetTag())); 
								(yyval.node) = ASTCREATOR.MakeNode_TableContentDot((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 284:

    {
								PE(S_DOTINDEX_(DPTR((yyvsp[(2) - (2)].node))->GetTag())); 
								(yyval.node) = ASTCREATOR.MakeNode_TableContentDot((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 285:

    {
								PE(S_DOTINDEX_(DPTR((yyvsp[(2) - (2)].node))->GetTag())); 
								(yyval.node) = ASTCREATOR.MakeNode_BoundedTableContentDot((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 286:

    { PE2(S_BRACKET_INDEX_(DPTR((yyvsp[(2) - (2)].node))->GetTag()), T_RB); ;}
    break;

  case 287:

    { (yyval.node) = ASTCREATOR.MakeNode_TableContentBracket((yyvsp[(1) - (4)].node), (yyvsp[(2) - (4)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)])); ;}
    break;

  case 288:

    { PE2(S_BRACKET_INDEX_(DPTR((yyvsp[(2) - (2)].node))->GetTag()), T_DRB); ;}
    break;

  case 289:

    {
								(yyval.node) = ASTCREATOR.MakeNode_BoundedTableContentBracket((yyvsp[(1) - (4)].node), (yyvsp[(2) - (4)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 290:

    { SM(S_TABLE); PE2(T_LB, S_TABLIST); ;}
    break;

  case 291:

    { PE(T_RB); ;}
    break;

  case 292:

    { EM(S_TABLE); ;}
    break;

  case 293:

    { (yyval.node) = ASTCREATOR.MakeNode_TableConstructor((yyvsp[(2) - (3)].nodeList)); SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 294:

    { (yyval.node) = ASTCREATOR.MakeNode_TableConstructor(); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 295:

    { PE(T_COMMA); ;}
    break;

  case 296:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_TableElements((yyvsp[(1) - (4)].nodeList), (yyvsp[(4) - (4)].node)); ;}
    break;

  case 297:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_TableElements(NIL_NODELIST, (yyvsp[(1) - (1)].node)); ;}
    break;

  case 298:

    { (yyval.node) = ASTCREATOR.MakeNode_UnindexedValue((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 299:

    { (yyval.node) = ASTCREATOR.MakeNode_UnindexedValue((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 300:

    { MAKE_FUNCTION_EXPR((yyvsp[(1) - (1)].node), (yyval.node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 301:

    { OE(T_TABLE_ELEM); ;}
    break;

  case 302:

    { (yyval.node) = (yyvsp[(2) - (3)].node); DPTR((yyval.node))->SetLine((yyvsp[(3) - (3)].line)); SET_LOCATION((yyloc), (yylsp[(2) - (3)]), (yylsp[(2) - (3)])); ;}
    break;

  case 303:

    { (yyval.node) = (yyvsp[(2) - (2)].node); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 304:

    { (yyval.node) = (yyvsp[(2) - (2)].node); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 305:

    { (yyval.node) = (yyvsp[(2) - (2)].node); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 306:

    { PE2(T_DOT, T_IDENT); ;}
    break;

  case 307:

    { (yyval.node) = ASTCREATOR.MakeNode_DottedIdent((yyvsp[(3) - (3)].constStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 308:

    { PE(T_OPINDEX); (yyval.node) = ASTCREATOR.MakeNode_OperatorIndex((yyvsp[(1) - (1)].constStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 309:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 310:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 311:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 312:

    { PE(S_INDEXEXPR); ;}
    break;

  case 313:

    { (yyval.node) = (yyvsp[(2) - (2)].node); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 314:

    { PE(T_COMMA); ;}
    break;

  case 315:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (5)].nodeList), (yyvsp[(4) - (5)].node)); DPTR((yyvsp[(4) - (5)].node))->SetLine((yyvsp[(5) - (5)].line)); SET_LOCATION((yyloc), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 316:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, (yyvsp[(1) - (2)].node)); DPTR((yyvsp[(1) - (2)].node))->SetLine((yyvsp[(2) - (2)].line)); SET_LOCATION((yyloc), (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); ;}
    break;

  case 317:

    { (yyval.node) = ASTCREATOR.MakeNode_TableIndices((yyvsp[(1) - (1)].nodeList)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 318:

    { (yyval.node) = ASTCREATOR.MakeNode_TableValues((yyvsp[(1) - (1)].nodeList)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 319:

    { PE(T_LBC); SG(N_EMPTY); ;}
    break;

  case 320:

    { EG(N_EMPTY); PE2(S_INDICES, T_COLON); ;}
    break;

  case 321:

    { SG(S_ELIST); ;}
    break;

  case 322:

    { EG(S_ELIST); PE2(S_ELIST, T_RBC); ;}
    break;

  case 323:

    { (yyval.node) = ASTCREATOR.MakeNode_IndexedValues((yyvsp[(3) - (9)].node), (yyvsp[(7) - (9)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (9)]), (yylsp[(9) - (9)])); ;}
    break;

  case 324:

    { (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 325:

    { PE(T_DOT); OE(T_IDENT_OR_KWDIDENT); ;}
    break;

  case 326:

    { (yyval.node) = (yyvsp[(3) - (4)].node); PE(T_COLON); ;}
    break;

  case 327:

    { SG(S_EXPR); ;}
    break;

  case 328:

    {
							EG(S_EXPR); PE(S_EXPR);
							(yyval.node) = ASTCREATOR.MakeNode_IdentIndexElement((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
							DPTR((yyval.node))->SetLine((yyvsp[(4) - (4)].line));
							SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(3) - (4)]));
						;}
    break;

  case 329:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (4)].nodeList), (yyvsp[(4) - (4)].node)); DPTR((yyvsp[(4) - (4)].node))->SetLine((yyvsp[(3) - (4)].line)); ;}
    break;

  case 330:

    {
								(yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, (yyvsp[(2) - (2)].node)); DPTR((yyvsp[(2) - (2)].node))->SetLine((yyvsp[(1) - (2)].line));
								SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 331:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 332:

    { MAKE_FUNCTION_EXPR((yyvsp[(1) - (1)].node), (yyval.node)); ;}
    break;

  case 333:

    {
								PE(T_IDENT);
								(yyval.node) = ASTCREATOR.MakeNode_Name((yyvsp[(1) - (1)].constStr));
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 334:

    {
								PE(T_KWDIDENT);
								(yyval.node) = ASTCREATOR.MakeNode_Name((yyvsp[(1) - (1)].constStr));
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 335:

    {
								PE(T_ATTRIBUTEID); OE(T_LBC_OR_COLON);
								(yyval.node) = ASTCREATOR.MakeNode_Name((yyvsp[(1) - (1)].constStr) + 1);
								SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)]));
							;}
    break;

  case 336:

    { PE(T_ATTRIBUTE); OE(T_IDENT_OR_KWDIDENT); ;}
    break;

  case 337:

    { (yyval.node) = (yyvsp[(3) - (3)].node); ;}
    break;

  case 338:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 339:

    { (yyval.node) = (yyvsp[(2) - (2)].node); ;}
    break;

  case 340:

    { (yyval.node) = ASTCREATOR.MakeNode_NewAttribute((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 341:

    { PE(T_SET);  ;}
    break;

  case 342:

    { SG(S_EXPR); ;}
    break;

  case 343:

    { EG(S_EXPR); PE2(S_EXPR, T_GET); (yyval.node) = (yyvsp[(5) - (6)].node); DPTR((yyval.node))->SetLine((yyvsp[(6) - (6)].line)); SET_LOCATION((yyloc), (yylsp[(1) - (6)]), (yylsp[(5) - (6)])); ;}
    break;

  case 344:

    { SG(S_EXPR); ;}
    break;

  case 345:

    { EG(S_EXPR); PE2(S_EXPR, T_RBC); ;}
    break;

  case 346:

    {  (yyval.node) = (yyvsp[(3) - (6)].node); DPTR((yyval.node))->SetLine((yyvsp[(4) - (6)].line)); ;}
    break;

  case 347:

    { SM(S_STMT); PE(T_BREAK);	;}
    break;

  case 348:

    { (yyval.node) = ASTCREATOR.MakeNode_BREAK(); SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); SET_LOCATION((yyloc), (yylsp[(2) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 349:

    { SM(S_STMT); PE(T_CONT);	;}
    break;

  case 350:

    { (yyval.node) = ASTCREATOR.MakeNode_CONTINUE(); SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); SET_LOCATION((yyloc), (yylsp[(2) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 351:

    { SG(S_EXPR); ;}
    break;

  case 352:

    { EG(S_EXPR); PE2(S_EXPR, T_RPAR); ;}
    break;

  case 353:

    { (yyval.node) = (yyvsp[(4) - (6)].node); DPTR((yyval.node))->SetLine((yyvsp[(3) - (6)].line)); ;}
    break;

  case 354:

    { SM(S_IF); PE2(T_IF, T_LPAR);  ;}
    break;

  case 355:

    { EM(S_IF); SM(S_ELSE); PE(T_ELSE); ;}
    break;

  case 356:

    { EM(S_IF);   (yyval.node) = ASTCREATOR.MakeNode_IfStmt((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 357:

    {
								EM(S_ELSE);
								(yyval.node) = ASTCREATOR.MakeNode_IfElseStmt((yyvsp[(2) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node));
								SET_LOCATION((yyval.node), (yylsp[(1) - (5)]), (yylsp[(5) - (5)]));
							;}
    break;

  case 358:

    { SM(S_WHILE); PE2(T_WHILE, T_LPAR); ;}
    break;

  case 359:

    {
							EM(S_WHILE);
							(yyval.node) = ASTCREATOR.MakeNode_WhileStmt((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node));
							DPTR((yyvsp[(4) - (4)].node))->SetLine((yyvsp[(3) - (4)].line));
							SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
						;}
    break;

  case 360:

    {
							EM(S_FOR);
							(yyval.node) = ASTCREATOR.MakeNode_ForStmt((yyvsp[(2) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
							SET_LOCATION((yyval.node), (yylsp[(1) - (5)]), (yylsp[(5) - (5)]));
						;}
    break;

  case 361:

    { SM(S_FOR); PE2(T_FOR, T_LPAR); ;}
    break;

  case 362:

    { PE(S_ELIST); ;}
    break;

  case 363:

    {
								(yyval.node) = ASTCREATOR.MakeNode_ForInit((yyvsp[(2) - (3)].nodeList)); DPTR((yyval.node))->SetLine((yyvsp[(1) - (3)].line));
								SET_LOCATION((yyval.node), (yylsp[(2) - (3)]), (yylsp[(2) - (3)])); SET_LOCATION((yyloc), (yylsp[(2) - (3)]), (yylsp[(3) - (3)]));
							;}
    break;

  case 364:

    {
								EI(S_ELIST);
								(yyval.node) = ASTCREATOR.MakeNode_ForInit(NIL_NODELIST); DPTR((yyval.node))->SetLine((yyvsp[(1) - (2)].line));
								SET_LOCATION((yyval.node), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 365:

    { PE(S_EXPR); ;}
    break;

  case 366:

    { (yyval.node) = (yyvsp[(2) - (4)].node); DPTR((yyval.node))->SetLine((yyvsp[(3) - (4)].line)); SET_LOCATION((yyloc), (yylsp[(2) - (4)]), (yylsp[(4) - (4)])); ;}
    break;

  case 367:

    { PE(S_ELIST); ;}
    break;

  case 368:

    {
								(yyval.node) = ASTCREATOR.MakeNode_ForSuffix((yyvsp[(3) - (4)].nodeList)); DPTR((yyval.node))->SetLine((yyvsp[(2) - (4)].line));
								SET_LOCATION((yyval.node), (yylsp[(3) - (4)]), (yylsp[(4) - (4)])); SET_LOCATION((yyloc), (yylsp[(3) - (4)]), (yylsp[(4) - (4)]));
							;}
    break;

  case 369:

    {
								(yyval.node) = ASTCREATOR.MakeNode_ForSuffix(NIL_NODELIST); DPTR((yyval.node))->SetLine((yyvsp[(1) - (2)].line));
								SET_LOCATION((yyval.node), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 370:

    { PE(T_RPAR); ;}
    break;

  case 371:

    { SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 372:

    { (yyval.node) = (yyvsp[(2) - (2)].node); DPTR((yyval.node))->SetLine((yyvsp[(1) - (2)].line)); SET_LOCATION((yyloc), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 373:

    {	PE(S_LVAL);		;}
    break;

  case 374:

    {	PE(T_COMMA);	;}
    break;

  case 375:

    {	(yyval.node) = (yyvsp[(3) - (5)].node);		;}
    break;

  case 376:

    {	unullify((yyval.node));	;}
    break;

  case 377:

    {	SM(S_FOREACH); PE2(T_FOREACH, T_LPAR);	;}
    break;

  case 378:

    {	PE(S_LVAL);				;}
    break;

  case 379:

    {	PE(T_SEMI_OR_COMMA);	;}
    break;

  case 380:

    {	PE(S_EXPR);				;}
    break;

  case 381:

    {	PE(T_RPAR);				;}
    break;

  case 382:

    {	PE(S_STMT);				;}
    break;

  case 383:

    {	if ((yyvsp[(7) - (14)].node))
												(yyval.node) = ASTCREATOR.MakeNode_ForeachStmt((yyvsp[(7) - (14)].node), (yyvsp[(5) - (14)].node), (yyvsp[(9) - (14)].node), (yyvsp[(14) - (14)].node)); 
											else
												(yyval.node) = ASTCREATOR.MakeNode_ForeachStmt((yyvsp[(5) - (14)].node), NIL_NODE, (yyvsp[(9) - (14)].node), (yyvsp[(14) - (14)].node)); 
											DPTR((yyval.node))->SetLine((yyvsp[(13) - (14)].line));
											SET_LOCATION((yyval.node), (yylsp[(1) - (14)]), (yylsp[(14) - (14)]));
											EM(S_FOREACH);	;}
    break;

  case 384:

    { SM(S_THROW); PE2(T_THROW, S_EXPR); ;}
    break;

  case 385:

    { EM(S_THROW); (yyval.node) = ASTCREATOR.MakeNode_THROW((yyvsp[(3) - (4)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)])); ;}
    break;

  case 386:

    { PE(S_STMT); (yyval.node) = ASTCREATOR.MakeNode_ExceptionVar((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 387:

    { SM(S_TRY); PE2(T_TRY, S_STMT); ;}
    break;

  case 388:

    { PE(T_TRAP); ;}
    break;

  case 389:

    { PE(T_IDENT); ;}
    break;

  case 390:

    {
							EM(S_TRY);
							(yyval.node) = ASTCREATOR.MakeNode_Exception((yyvsp[(3) - (8)].node), (yyvsp[(7) - (8)].node), (yyvsp[(8) - (8)].node));
							SET_LOCATION((yyval.node), (yylsp[(1) - (8)]), (yylsp[(8) - (8)]));
						;}
    break;

  case 391:

    { (yyval.count) = (yyvsp[(1) - (2)].count) + 1; ;}
    break;

  case 392:

    { (yyval.count) = 1; ;}
    break;

  case 393:

    {
							(yyval.node) = ASTCREATOR.MakeNode_ChildExpr(AST_TAG_ESCAPE, (yyvsp[(4) - (5)].node));
							(yyval.node)->SetAttribute(AST_ATTRIBUTE_CARDINALITY, (yyvsp[(1) - (5)].count));
							(yyval.node)->SetLine((yyvsp[(3) - (5)].line));
							SET_LOCATION((yyval.node), (yylsp[(1) - (5)]), (yylsp[(5) - (5)]));
						;}
    break;

  case 394:

    {
							(yyval.node) = ASTCREATOR.MakeNode_Name((yyvsp[(3) - (3)].constStr));				SET_LOCATION((yyval.node), (yylsp[(3) - (3)]), (yylsp[(3) - (3)]));
							(yyval.node) = ASTCREATOR.MakeNode_Lvalue((yyval.node));			SET_LOCATION((yyval.node), (yylsp[(3) - (3)]), (yylsp[(3) - (3)]));
							(yyval.node) = ASTCREATOR.MakeNode_PrimaryExpression((yyval.node)); SET_LOCATION((yyval.node), (yylsp[(3) - (3)]), (yylsp[(3) - (3)]));
							
							(yyval.node) = ASTCREATOR.MakeNode_ChildExpr(AST_TAG_ESCAPE, (yyval.node));
							(yyval.node)->SetAttribute(AST_ATTRIBUTE_CARDINALITY, (yyvsp[(1) - (3)].count));
							(yyval.node)->SetLine((yyvsp[(2) - (3)].line));
							SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
						;}
    break;

  case 395:

    {
							(yyval.node) = ASTCREATOR.MakeNode_ChildExpr(AST_TAG_INLINE, (yyvsp[(4) - (5)].node));
							(yyval.node)->SetLine((yyvsp[(3) - (5)].line));
							SET_LOCATION((yyval.node), (yylsp[(1) - (5)]), (yylsp[(5) - (5)]));
						;}
    break;

  case 396:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (3)].nodeList), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 397:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, (yyvsp[(1) - (1)].node)); ;}
    break;

  case 398:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (3)].nodeList), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 399:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, (yyvsp[(1) - (1)].node)); ;}
    break;

  case 400:

    { SG(S_ELIST); ;}
    break;

  case 401:

    { EG(S_ELIST); PE2(S_ELIST, T_RBC); ;}
    break;

  case 402:

    {
							Node* indices = ASTCREATOR.MakeNode_TableIndices((yyvsp[(2) - (7)].nodeList)); SET_LOCATION(indices, (yylsp[(2) - (7)]), (yylsp[(2) - (7)]));
							Node* values = ASTCREATOR.MakeNode_TableValues((yyvsp[(5) - (7)].nodeList)); SET_LOCATION(values, (yylsp[(5) - (7)]), (yylsp[(5) - (7)]));
							(yyval.node) = ASTCREATOR.MakeNode_IndexedValues(indices, values); SET_LOCATION((yyval.node), (yylsp[(1) - (7)]), (yylsp[(7) - (7)]));
						;}
    break;

  case 403:

    { (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 404:

    { (yyval.node) = (yyvsp[(2) - (3)].node); PE(T_COLON); ;}
    break;

  case 405:

    { SG(S_EXPR); ;}
    break;

  case 406:

    {
								EG(S_EXPR); PE(S_EXPR);
								(yyval.node) = ASTCREATOR.MakeNode_IdentIndexElement((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
								DPTR((yyval.node))->SetLine((yyvsp[(4) - (4)].line));
								SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
								SET_LOCATION((yyloc), (yylsp[(1) - (4)]), (yylsp[(3) - (4)]));
							;}
    break;

  case 407:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 408:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 409:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 410:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 411:

    { (yyval.node) = ASTCREATOR.MakeNode_DottedIdent((yyvsp[(2) - (2)].constStr)); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 412:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 413:

    { PE2(T_BAR, S_EXPR); ;}
    break;

  case 414:

    { PE(T_BAR); ;}
    break;

  case 415:

    { (yyval.node) = ASTCREATOR.MakeNode_LateBoundArgument((yyvsp[(3) - (5)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (5)]), (yylsp[(5) - (5)])); ;}
    break;

  case 416:

    { PE(T_TDOT); (yyval.node) = ASTCREATOR.MakeNode_TRIPLE_DOT(); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 417:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (3)].nodeList), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 418:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (3)].nodeList), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 419:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List((yyvsp[(1) - (3)].nodeList), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 420:

    { (yyval.nodeList) = ASTCREATOR.MakeNode_List(NIL_NODELIST, (yyvsp[(1) - (1)].node)); ;}
    break;

  case 421:

    { (yyval.nodeList) = (yyvsp[(1) - (1)].nodeList); ;}
    break;

  case 422:

    { (yyval.nodeList) = (yyvsp[(1) - (1)].nodeList); ;}
    break;

  case 423:

    { (yyval.node) = ASTCREATOR.MakeNode_BasicStmt((yyvsp[(1) - (2)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 424:

    { (yyval.node) = ASTCREATOR.MakeNode_BasicStmt((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 425:

    { (yyval.node) = ASTCREATOR.MakeNode_BasicStmt((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 426:

    { (yyval.node) = ASTCREATOR.MakeNode_EmptyStmt();	  SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 427:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 428:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 429:

    {
						Node* elist = ASTCREATOR.MakeNode_ExpressionListStmt((yyvsp[(1) - (2)].nodeList));
						SET_LOCATION(elist, (yylsp[(1) - (2)]), (yylsp[(1) - (2)]));
						(yyval.node) = ASTCREATOR.MakeNode_BasicStmt(elist);
						SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
					;}
    break;

  case 430:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 431:

    { (yyval.node) = ASTCREATOR.MakeNode_FunctionStmt((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 432:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 433:

    { (yyval.node) = ASTCREATOR.MakeNode_Stmts((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 434:

    { (yyval.node) = ASTCREATOR.MakeNode_Stmts(NIL_NODE, (yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 435:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 436:

    { (yyval.node) = ASTCREATOR.MakeNode_FunctionStmt((yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 437:

    {
								Node* elist = ASTCREATOR.MakeNode_ExpressionListStmt((yyvsp[(1) - (2)].nodeList));
								SET_LOCATION(elist, (yylsp[(1) - (2)]), (yylsp[(1) - (2)]));
								(yyval.node) = ASTCREATOR.MakeNode_BasicStmt(elist);
								SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
							;}
    break;

  case 438:

    { PE(T_RBC); ;}
    break;

  case 439:

    { (yyval.node) = ASTCREATOR.MakeNode_Compound((yyvsp[(2) - (4)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (4)]), (yylsp[(4) - (4)])); ;}
    break;

  case 440:

    { (yyval.node) = ASTCREATOR.MakeNode_Compound(); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 441:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 442:

    { (yyval.node) = ASTCREATOR.MakeNode_Stmts((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 443:

    {
						(yyval.node) = ASTCREATOR.MakeNode_Stmts(NIL_NODE, (yyvsp[(1) - (2)].node));
						(yyval.node) = ASTCREATOR.MakeNode_Stmts((yyval.node), (yyvsp[(2) - (2)].node));
						SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
					;}
    break;

  case 444:

    { (yyval.node) = ASTCREATOR.MakeNode_FromList(AST_TAG_QUOTED_ELEMENTS, (yyvsp[(1) - (1)].nodeList)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 445:

    { (yyval.node) = ASTCREATOR.MakeNode_Stmts(NIL_NODE, (yyvsp[(1) - (1)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 446:

    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 447:

    { (yyval.node) = ASTCREATOR.MakeNode_ChildExpr(AST_TAG_QUASI_QUOTES, (yyvsp[(2) - (3)].node));	SET_LOCATION((yyval.node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 448:

    { (yyval.node) = ASTCREATOR.MakeNode_ChildExpr(AST_TAG_EXECUTE, (yyvsp[(2) - (2)].node)); SET_LOCATION((yyval.node), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
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
      yyerror (&yylloc, ctx, YY_("syntax error"));
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
	    yyerror (&yylloc, ctx, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, ctx, YY_("syntax error"));
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
		      yytoken, &yylval, &yylloc, ctx);
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
		  yystos[yystate], yyvsp, yylsp, ctx);
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
  yyerror (&yylloc, ctx, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, ctx);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, ctx);
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




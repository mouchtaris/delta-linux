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
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse DeltaSyntax_yyparse
#define yylex   DeltaSyntax_yylex
#define yyerror DeltaSyntax_yyerror
#define yylval  DeltaSyntax_yylval
#define yychar  DeltaSyntax_yychar
#define yydebug DeltaSyntax_yydebug
#define yynerrs DeltaSyntax_yynerrs
#define yylloc DeltaSyntax_yylloc

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
/* Tokens.  */
#define NUMBER_CONST 258
#define STRING_CONST 259
#define FUNCTION 260
#define RETURN 261
#define ONEVENT 262
#define IF 263
#define ELSE 264
#define WHILE 265
#define FOR 266
#define FOREACH 267
#define NIL 268
#define LOCAL 269
#define USING 270
#define AND 271
#define NOT 272
#define OR 273
#define PLUSPLUS 274
#define MINUSMINUS 275
#define LAMBDA 276
#define TRY 277
#define TRAP 278
#define THROW 279
#define ASSERT 280
#define TRUE 281
#define FALSE 282
#define BREAK 283
#define CONTINUE 284
#define STATIC 285
#define CONST 286
#define METHOD 287
#define SELF 288
#define ARGUMENTS 289
#define LAMBDA_REF 290
#define OPERATOR 291
#define NEWSELF 292
#define SET 293
#define GET 294
#define LT 295
#define GT 296
#define LE 297
#define GE 298
#define EQ 299
#define NE 300
#define DOT 301
#define DOUBLE_DOT 302
#define TRIPLE_DOT 303
#define DOT_ASSIGN 304
#define DOT_EQUAL 305
#define DOT_CAST 306
#define DOT_EQUAL_RHS 307
#define UMINUS 308
#define ADD 309
#define SUB 310
#define MUL 311
#define DIV 312
#define MOD 313
#define ATTRIBUTE 314
#define STRINGIFY 315
#define GLOBAL_SCOPE 316
#define ASSIGN 317
#define ADD_A 318
#define SUB_A 319
#define MUL_A 320
#define DIV_A 321
#define MOD_A 322
#define DOUBLE_LB 323
#define DOUBLE_RB 324
#define CALL 325
#define CAST 326
#define SEMI 327
#define IDENT 328
#define ATTRIBUTE_IDENT 329
#define ADD_POSTFIX 330
#define SUB_POSTFIX 331
#define MUL_POSTFIX 332
#define DIV_POSTFIX 333
#define MOD_POSTFIX 334
#define GT_POSTFIX 335
#define LT_POSTFIX 336
#define NE_POSTFIX 337
#define EQ_POSTFIX 338
#define GE_POSTFIX 339
#define LE_POSTFIX 340
#define PARENTHESIS 341
#define SQUARE_BRACKETS 342
#define META_LSHIFT 343
#define META_RSHIFT 344
#define META_ESCAPE 345
#define META_INLINE 346
#define META_EXECUTE 347
#define META_RENAME 348
#define PARSE_QUOTED_ELEMENTS 349




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
#include "DeltaScanner.h"
#include "DeltaSyntax.h"

#define yyFlexLexer DeltaScannerFlexLexer

using namespace AST;

#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

extern int DeltaSyntax_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer);

///////////////////////////////////////////////////////////
// Helper macros to get specific components from the lexer
// component directory.
#define DIRECTORY								(lexer.GetDirectory())

#define GET_DESCRIPTIVE_ERROR_HANDLER			DESCRIPTIVE_ERROR_HANDLER_EX(DIRECTORY)
#define ASTCREATOR_BY_CONTEXT					ASTCREATOR_EX(DIRECTORY)
#define PARSEPARMS_BY_CONTEXT					PARSEPARMS_EX(DIRECTORY)
#define STRINGHOLDER_BY_CONTEXT					STRINGHOLDER_EX(DIRECTORY)

///////////////////////////////////////////////////////////

static void DeltaSyntax_yyerror (YYLTYPE* yylloc, yyFlexLexer& lexer, const char* unused)
	{ GET_DESCRIPTIVE_ERROR_HANDLER.HandleSyntaxError(); }

#define	DYNAMIC_STRING(s) \
	STRINGHOLDER_BY_CONTEXT.StringWithLateDestruction(ucopystr(s))

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

#define	MAKE_FUNCTION_EXPR(_p, _pp)										\
	if (true) {															\
		PE(S_FUNC_(GetFuncClass(_p)));									\
		_pp = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionExpression(_p);	\
		(_pp)->SetLocation((_p)->GetLocation());							\
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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
	double				numberConst;
	util_ui32			line;
	util_ui32			count;
	char*				dynamicStr;
	const char*			constStr;
	AST::NodeList*		nodeList;
	AST::Node*			node;
} YYSTYPE;
/* Line 196 of yacc.c.  */

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
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


/* Line 219 of yacc.c.  */


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
	 || (defined (YYLTYPE_IS_TRIVIAL) && YYLTYPE_IS_TRIVIAL \
             && defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  146
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1988

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  105
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  237
/* YYNRULES -- Number of rules. */
#define YYNRULES  448
/* YYNRULES -- Number of states. */
#define YYNSTATES  639

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   349

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
static const unsigned short int yyprhs[] =
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

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
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
      61,    -1,   115,    -1,   219,   115,    -1,   269,    -1,    -1,
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
static const unsigned short int yyrline[] =
{
       0,   162,   162,   168,   171,   182,   185,   192,   194,   196,
     191,   206,   207,   210,   212,   214,   218,   219,   220,   223,
     225,   227,   232,   235,   236,   237,   241,   240,   244,   247,
     250,   253,   252,   260,   266,   269,   268,   273,   276,   279,
     279,   284,   283,   292,   299,   299,   302,   308,   308,   313,
     313,   318,   322,   323,   324,   325,   326,   329,   330,   333,
     334,   335,   339,   340,   341,   342,   343,   344,   345,   346,
     352,   351,   361,   361,   368,   371,   373,   378,   377,   381,
     387,   390,   391,   394,   396,   401,   408,   409,   410,   411,
     412,   413,   414,   417,   418,   419,   417,   428,   427,   436,
     435,   444,   443,   452,   451,   460,   459,   468,   467,   477,
     476,   485,   484,   493,   492,   501,   500,   509,   508,   517,
     516,   526,   525,   534,   533,   543,   542,   551,   550,   559,
     558,   567,   566,   575,   574,   583,   587,   591,   595,   595,
     600,   606,   605,   613,   619,   618,   627,   626,   635,   634,
     642,   645,   649,   650,   651,   654,   655,   656,   657,   660,
     661,   662,   663,   664,   665,   666,   667,   668,   669,   670,
     671,   672,   673,   674,   675,   676,   677,   678,   679,   680,
     681,   682,   683,   684,   686,   687,   688,   689,   690,   691,
     692,   693,   694,   695,   696,   697,   698,   699,   700,   701,
     702,   703,   704,   705,   706,   707,   708,   709,   710,   711,
     712,   713,   716,   718,   722,   726,   727,   730,   733,   732,
     738,   746,   748,   750,   757,   762,   767,   772,   777,   778,
     781,   780,   784,   787,   786,   798,   799,   805,   804,   808,
     813,   815,   812,   818,   822,   822,   826,   828,   825,   831,
     833,   839,   840,   843,   851,   853,   862,   868,   872,   878,
     884,   890,   894,   893,   899,   898,   903,   907,   910,   915,
     919,   924,   928,   929,   930,   931,   934,   936,   938,   940,
     944,   948,   950,   956,   961,   966,   972,   971,   976,   975,
     986,   990,   990,   995,   998,  1003,  1002,  1006,  1010,  1012,
    1016,  1020,  1023,  1024,  1025,  1026,  1030,  1029,  1035,  1039,
    1040,  1041,  1044,  1044,  1050,  1049,  1053,  1057,  1060,  1064,
    1066,  1068,  1070,  1063,  1075,  1078,  1077,  1084,  1083,  1093,
    1095,  1101,  1102,  1108,  1113,  1120,  1126,  1125,  1131,  1132,
    1135,  1140,  1142,  1139,  1148,  1150,  1147,  1158,  1158,  1163,
    1163,  1168,  1169,  1168,  1175,  1176,  1178,  1180,  1189,  1193,
    1203,  1211,  1210,  1216,  1220,  1227,  1227,  1232,  1232,  1237,
    1243,  1243,  1250,  1253,  1254,  1253,  1256,  1259,  1260,  1261,
    1262,  1263,  1264,  1259,  1278,  1277,  1283,  1286,  1287,  1288,
    1286,  1300,  1301,  1304,  1310,  1320,  1327,  1329,  1333,  1334,
    1338,  1340,  1337,  1348,  1349,  1353,  1352,  1363,  1364,  1365,
    1366,  1367,  1368,  1370,  1372,  1369,  1375,  1378,  1379,  1380,
    1381,  1384,  1385,  1388,  1389,  1390,  1391,  1392,  1393,  1396,
    1402,  1403,  1404,  1407,  1409,  1413,  1414,  1417,  1423,  1423,
    1425,  1427,  1431,  1432,  1439,  1440,  1441,  1444,  1448
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
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
  "'|'", "$accept", "DeltaCode", "Stmts", "LN", "Function", "@1", "@2",
  "@3", "FuncLinkage", "FunctionClass", "Ident", "FunctionName",
  "OperatorMethod", "IdentList", "@4", "FormalArgsSuffix", "@5",
  "FormalArgsNode", "FormalArgs", "@6", "CompoundOpening", "Compound",
  "@7", "LambdaFunction", "@8", "LambdaPrefix", "LambdaCode", "@9",
  "LambdaStmt", "Semi", "@10", "ExpressionListStmt", "@11", "Stmt",
  "BasicStmt", "BasicExprStmt", "BasicNonExprStmt", "Assertion", "@12",
  "AssertStmt", "@13", "ReturnPrefix", "ReturnValue", "ReturnStmt", "@14",
  "UsingDirective", "UsingSpecifications", "UsingNamespace",
  "UsingByteCodeLibrary", "Expression", "ConstDefExpression", "@15", "@16",
  "@17", "AssignExpression", "@18", "@19", "@20", "@21", "@22", "@23",
  "RelationalExpression", "@24", "@25", "@26", "@27", "@28", "@29",
  "BooleanExpression", "@30", "@31", "ArithmeticExpression", "@32", "@33",
  "@34", "@35", "@36", "TernaryExpression", "TernaryCondition",
  "TernarySelection1", "TernarySelection2", "@37", "Term", "@38", "@39",
  "@40", "@41", "Primary", "ConstValue", "OpString", "KwdIdent",
  "StringConst", "Stringify", "StringIdent", "StringifyDottedIdents",
  "@42", "StringifyNamespaceIdent", "StringConstUsed",
  "FunctionAndTableObject", "@43", "@44", "ActualArgumentsList",
  "NonEmptyActualArgumentsList", "@45", "ActualArguments", "@46", "@47",
  "ActualArgument", "@48", "@49", "@50", "FunctionCallObject",
  "FunctionCall", "ExpressionList", "NamespacePath", "Lvalue", "@51",
  "@52", "TableObject", "TableContentDot", "TableContentBoundedDot",
  "TableContentBracket", "TableContentBoundedBracket", "DottedOpString",
  "DotIndex", "SpecialDotIndex", "BracketIndex", "TableContent", "@53",
  "@54", "TablePrefix", "TableSuffix", "@55", "TableConstructor",
  "TableElements", "@56", "UnindexedValue", "FunctionElement", "PE_elem",
  "TableElement", "DottedIdent", "@57", "OperatorIndex", "IndexContent",
  "IndexExpression", "@58", "IndexedList", "@59", "TableIndices",
  "TableValues", "IndexedValues", "@60", "@61", "@62", "@63", "IdentIndex",
  "@64", "IdentIndexElement", "@65", "ContentList", "ContentExpression",
  "AttributeIdent", "ConstAttributeId", "@66", "AttributeId",
  "NewAttribute", "AttributeSet", "@67", "@68", "AttributeGet", "@69",
  "@70", "BreakStmt", "@71", "ContinueStmt", "@72", "Condition", "@73",
  "@74", "IfPrefix", "ElsePrefix", "IfStmt", "WhilePrefix", "WhileStmt",
  "ForStmt", "ForPrefix", "@75", "ForInitList", "ForCondition", "@76",
  "ForSuffix", "@77", "ForEnd", "@78", "ForeachCont", "ForeachValue",
  "@79", "@80", "ForeachStmt", "@81", "@82", "@83", "@84", "@85", "@86",
  "ThrowStmt", "@87", "ExceptionVar", "ExceptionStmt", "@88", "@89", "@90",
  "MultipleEscapes", "MetaGeneratedCode", "QuotedExpressionList",
  "QuotedIndexedList", "QuotedIndexedValues", "@91", "@92",
  "QuotedIdentIndex", "QuotedIdentIndexElement", "@93",
  "NonExprQuotedElement", "@94", "@95", "QuotedElementList",
  "QuotedElements", "MiscStmt", "NoContextStmt", "NonEmptyStmts",
  "QuotedStmt", "NonFunctionQuotedStmt", "@96", "QuotedStmts",
  "QuasiQuoted", "MetaExpression", "MetaStmt", 0
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
static const unsigned short int yyr1[] =
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
static const unsigned char yyr2[] =
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
static const unsigned short int yydefact[] =
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
     266,   301,   229,   412,   338,   261,   409,    66,     0,    67,
       0,     0,    65,     0,    62,    63,     6,    64,    69,    68,
       6,    18,   422,   408,   405,   410,   420,   421,   444,   441,
     347,   435,   446,     3,   151,   428,     1,    38,    54,    49,
      53,     5,    52,     4,    55,    56,     0,     0,     0,     0,
       0,    80,    81,    82,     0,     0,     0,     0,     6,     0,
       0,     0,     0,   186,   187,   188,   184,   185,   189,   190,
     191,   192,   193,   201,   194,   195,   196,   197,   198,   199,
     200,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   215,   216,   411,     0,     0,     0,   339,   146,   233,
     230,     0,   261,   306,   440,   431,   430,   309,   310,   311,
     399,    47,   432,   434,   347,     0,     6,   448,    16,     0,
      12,   257,    29,    34,    41,    33,   423,     0,    79,     0,
     121,   123,   111,   109,   115,   113,   117,   119,   125,   127,
     131,   129,   133,   212,   215,     0,   216,   217,     0,   218,
     240,   243,   253,   260,   140,   143,    97,    99,   103,   101,
     105,   107,   268,   269,   272,   273,   274,   275,   271,   270,
     280,   284,   160,   276,   278,   277,   279,   283,   285,    24,
      25,   282,   281,    23,   286,   288,   294,     0,   291,     0,
     297,   341,   403,     0,    47,    47,   351,     6,     6,     6,
     365,     6,   391,     0,     0,   437,     0,     0,   347,   436,
      47,   443,   435,   442,    59,     6,    51,    57,    58,    60,
       0,    61,     6,   362,   378,   265,     0,    84,     0,   149,
       0,     0,   142,   145,   388,    47,    71,   263,    94,   404,
     147,   333,   334,   337,     0,   136,     0,   137,     0,     0,
     400,     0,   429,    47,   433,     0,   447,     0,   414,    11,
      22,    28,    35,     0,    48,    76,    78,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   220,     0,   244,   258,     0,     0,     0,     0,     0,
       0,     0,     0,   292,   295,   293,   325,   319,   300,   298,
       6,   299,   303,   327,   305,   261,   304,     0,   344,   340,
     348,   350,     6,   356,     6,     0,   364,    47,   367,     0,
       0,   394,     0,   407,   396,   417,   332,   331,     6,   419,
     418,     0,    47,    47,     0,     0,     6,    83,   230,     0,
     385,     0,   234,   231,   138,   215,   307,     6,   398,   439,
       0,     0,     0,    21,    19,     8,    26,     0,    44,    42,
      75,   122,   124,   112,   110,   116,   114,   118,   120,   126,
     128,   132,   130,   134,   219,   249,   246,   250,   241,   236,
     239,     0,    98,   100,   104,   102,   106,   108,   287,   289,
     301,     0,   312,   302,     0,   324,   342,     0,     0,   355,
       6,   359,   255,     6,   363,   370,     6,     6,     6,     0,
       0,   406,    50,    73,    40,   228,    85,   389,    95,     0,
       0,     0,   401,   395,   415,    20,    37,     0,    31,    30,
      36,     6,     0,     0,   237,   245,   296,     0,     6,     0,
     314,   320,     6,     0,     6,   352,   357,     0,   369,     0,
     360,     6,    47,   393,     0,     0,     0,   135,   139,   330,
       6,     0,     9,    27,     0,     0,     0,   247,   242,   244,
     326,   316,   313,     0,     0,   328,     6,   345,     0,   254,
     371,   370,   366,   373,   376,   380,   386,     6,    96,     0,
     402,     0,    32,    46,    45,     0,   238,   312,   321,   343,
       0,   353,   368,     0,     6,   390,   329,     6,   248,     6,
       6,   346,   228,     0,   381,    10,   315,   322,   318,     0,
     372,     0,     0,   375,   382,   323,     6,     6,   383
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     2,     3,   149,   150,   230,   536,   601,   370,    75,
      76,   465,   291,   372,   537,   540,   574,   234,   235,   467,
     151,   152,   444,    77,   373,    78,   469,   541,   576,   315,
     237,   324,   325,   153,   326,   327,    79,    80,   170,   329,
     330,    81,   376,    82,   239,   154,   161,   162,   163,   217,
      85,   172,   451,   566,    86,   395,   396,   398,   397,   399,
     400,    87,   381,   380,   383,   382,   384,   385,    88,   378,
     379,    89,   386,   387,   389,   388,   390,    90,   211,   358,
     529,   530,    91,   166,   167,   205,   165,    92,    93,    94,
     256,    95,    96,   203,    97,   392,    98,    99,   100,   356,
     354,   488,   489,   579,   262,   393,   543,   490,   491,   542,
     605,   101,   102,   427,   103,   104,   171,   158,   105,   106,
     107,   108,   109,   280,   287,   281,   294,   110,   401,   402,
     111,   296,   297,   112,   298,   500,   410,   411,   299,   300,
     218,   359,   219,   220,   548,   549,   550,   583,   551,   627,
     412,   502,   584,   620,   632,   413,   501,   414,   504,   532,
     438,   204,   114,   206,   212,   116,   303,   417,   553,   419,
     507,   610,   117,   118,   119,   120,   307,   422,   588,   121,
     510,   122,   123,   124,   125,   126,   156,   310,   428,   429,
     516,   517,   558,   559,   624,   595,   613,   629,   127,   157,
     445,   564,   614,   631,   636,   128,   169,   597,   129,   168,
     449,   565,   130,   131,   132,   363,   133,   457,   571,   134,
     135,   316,   136,   229,   461,   137,   138,   139,   223,   224,
     140,   141,   365,   142,   143,   144,   155
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -411
static const short int yypact[] =
{
     -76,   604,    33,    30,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,   106,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  1833,  -411,
    -411,    32,  -411,  -411,  -411,     0,  -411,  -411,  -411,  1474,
    -411,   808,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,   604,  -411,
     -19,    80,   -21,  -411,    39,  -411,    17,  -411,   -30,  -411,
    -411,     9,  -411,  -411,   539,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,    99,  1680,    60,  -411,  -411,
     362,    -7,  -411,    16,   340,   386,  1743,  1743,  1000,  1000,
    -411,    41,  -411,  -411,  -411,     6,  -411,  -411,   104,  -411,
     113,    65,  -411,    65,  -411,  -411,  -411,  -411,  -411,  -411,
     -34,  -411,    -5,  -411,  -411,  -411,  -411,    70,  -411,  -411,
    1292,    29,  1192,  -411,  -411,  -411,  -411,  -411,  -411,   471,
    -411,  -411,  -411,  -411,  -411,  -411,    92,   110,    16,    16,
     -15,  -411,  -411,  -411,    16,  1528,  1582,  1582,    80,  1528,
    1528,    16,    16,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,    75,   105,  -411,   107,  1528,  1863,  -411,  -411,  -411,
     208,  1528,  -411,  -411,  -411,  -411,  -411,   539,  -411,  -411,
    -411,    51,  -411,  -411,   908,    85,  -411,  -411,  -411,  1528,
     171,  -411,    16,  -411,  -411,  -411,  -411,   109,  -411,  1474,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,   125,    17,  -411,  -411,    16,  -411,
    -411,  -411,  -411,   126,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,   539,  -411,  -411,  -411,  -411,   118,   114,  1383,
    -411,  -411,  -411,   155,  -411,  -411,  -411,    80,  -411,     9,
    -411,  -411,  -411,   116,   706,  -411,  1474,   706,   808,  -411,
     117,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
     175,  -411,    45,  -411,  -411,  -411,   120,  -411,    -2,  -411,
    1474,    -7,   222,   222,  -411,   539,   539,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,   131,  -411,   132,   539,   130,  1895,
    -411,  1092,  -411,   133,  -411,   134,  -411,  1528,   539,  -411,
      97,  -411,   136,   138,  -411,  -411,  -411,   539,  1528,  1528,
    1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,
    1528,   126,  1895,    21,  -411,  1528,  1528,  1528,  1528,  1528,
    1528,   142,   151,  -411,  -411,  -411,  -411,  -411,  -411,   539,
    -411,  -411,  -411,  -411,  -411,    19,  -411,   183,  -411,  -411,
    -411,  -411,  -411,   213,    80,  1528,  -411,   145,   154,  1528,
    1528,  -411,  1092,  -411,   539,  -411,  -411,   539,  -411,   539,
    -411,  1528,   145,  -411,   152,  1582,  -411,  -411,   539,   206,
    -411,   168,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
     430,   128,  1894,  -411,  -411,  -411,  -411,   -17,  -411,  -411,
    -411,   547,  1214,   613,   613,   613,   613,  1300,  1300,   115,
     115,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,   158,
    -411,  1528,   539,   539,   539,   539,   539,   539,  -411,  -411,
    -411,  1863,  -411,  -411,  1474,  -411,  -411,  1474,  1528,  -411,
      80,  -411,   539,  -411,  -411,  -411,    80,  -411,   539,   492,
      51,  -411,  -411,  -411,  -411,    82,  -411,  -411,  -411,   169,
    1528,  1474,   164,  -411,  -411,  -411,   -30,    16,  -411,  -411,
    -411,  -411,  1528,   172,  -411,   539,  -411,   177,  -411,  1092,
     178,  -411,  -411,  1474,  -411,   539,  -411,  1528,  -411,   174,
    -411,  -411,  -411,  -411,   100,    16,  1528,  -411,   539,  -411,
    -411,   181,  -411,  -411,   186,  1528,   184,   539,  -411,    36,
    -411,  -411,  -411,   182,   190,  -411,  -411,  -411,   205,   539,
    -411,   145,  -411,  -411,  -411,  -411,  -411,    80,   539,  1474,
    -411,   187,  -411,   539,  -411,   173,  -411,  -411,  -411,  -411,
     203,  -411,  -411,  1582,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,   201,  1528,  -411,  -411,  -411,  -411,   164,   202,
     539,   210,   207,  -411,  -411,  -411,  -411,    80,  -411
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -411,  -411,   137,   -92,     7,  -411,  -411,  -411,  -411,  -411,
     -14,  -411,  -179,  -411,  -411,  -411,  -411,  -247,  -411,  -411,
    -411,   -48,  -411,  -411,  -411,  -411,  -411,  -411,  -411,   -70,
    -411,  -411,  -411,   -66,  -411,  -411,   146,   -33,  -411,  -411,
    -411,  -411,  -411,   147,  -411,    13,  -411,  -411,  -411,    -1,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,   -91,
     -36,  -411,  -411,   -73,  -411,  -411,  -411,    68,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -279,  -411,  -411,
    -411,  -160,  -411,  -303,    27,  -154,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,   195,  -411,   194,  -411,  -411,  -411,
    -411,    10,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -196,
    -411,  -411,    20,  -336,  -302,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -311,
    -410,  -202,  -411,  -411,    18,    11,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,   188,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -277,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,  -411,
    -411,  -411,  -411,   267,   -85,   -42,  -411,  -411,  -411,  -411,
    -411,  -411,  -270,  -411,  -411,  -411,  -411,   -31,    96,  -411,
     -18,    -4,  -411,  -411,   254,  -411,    23
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -446
static const short int yytable[] =
{
      84,   160,   202,   216,   353,   227,   341,   341,    74,   221,
     236,   238,   342,   343,    83,   284,   284,   293,   293,   115,
     222,   113,   442,   257,   145,   458,     6,     1,     8,  -445,
      -2,   538,  -160,   146,   309,     6,   311,     8,   313,  -407,
     232,     6,   164,     8,   435,    15,   231,   440,   210,   233,
       6,   226,     8,    27,   539,   320,   209,   320,   215,   394,
      15,   228,    27,   260,    83,   312,   337,    84,    27,   485,
     285,   285,   261,   314,   145,    74,   -47,    27,   231,   447,
     301,    83,   255,   302,   485,     6,   115,     8,   113,   263,
     -47,   145,  -235,   301,   552,    15,   505,   554,    55,    69,
      70,  -160,   344,   253,   147,  -160,   259,   292,   292,  -160,
    -160,   148,    27,  -160,  -291,    69,    70,  -407,    72,   147,
     -39,   569,   321,   258,   323,   486,   148,  -445,   360,   361,
    -160,    71,   304,   462,   367,   306,   322,  -407,   322,    84,
     486,    84,   305,   586,   335,   336,    71,   319,   317,   319,
     338,   362,  -333,    83,   147,    83,   463,   347,   348,  -379,
    -379,   148,   333,   145,   339,   145,   159,    47,   345,   346,
     352,   250,   251,   252,   286,   286,   216,   593,   594,    55,
     334,    71,  -334,   366,   349,   369,   -17,   394,    55,   616,
     374,   403,   404,   222,   418,   441,    69,    70,   431,    72,
      22,   446,   452,   453,   350,    69,    70,   454,    72,   459,
     357,   361,   468,   582,   466,   498,   424,   425,   371,   430,
     499,   506,   509,   513,   240,    -6,   241,   524,   368,   527,
     528,   215,   534,   425,   420,   421,   544,    83,   377,   426,
     567,   423,   570,   578,   391,   590,   375,   145,   242,   243,
     244,   245,   246,   247,   580,  -317,   600,   602,   591,   604,
     607,   147,   248,   249,   250,   251,   252,   608,  -228,  -228,
     216,  -228,  -228,  -228,  -228,   450,   611,   618,   621,  -374,
     633,   634,   635,   535,   355,   341,   456,   222,   332,   572,
    -228,   525,  -228,   362,  -228,   328,   331,   443,   409,   547,
     606,  -228,   288,   295,   546,   619,   408,   470,   405,   628,
     416,   308,   207,   434,   612,   437,   439,   415,   503,   484,
     364,   433,   225,   436,   433,   215,     0,     0,     0,     0,
     508,    83,   115,     0,   113,   115,   515,   113,     0,   448,
       0,   145,     0,     0,     0,     0,   521,   209,     0,     0,
       0,     0,     0,     0,   526,     0,   464,   514,   511,   264,
     265,     0,     0,     0,     0,   531,   460,     0,     0,     0,
       0,   293,   522,   523,     0,     0,     0,   471,   472,   473,
     474,   475,   476,   477,   478,   479,   480,   481,   482,   483,
     520,     0,     0,     0,   492,   493,   494,   495,   496,   497,
     487,     0,   266,   267,   268,   269,   270,   271,  -267,  -267,
       0,  -267,  -267,  -267,  -267,     0,     0,     0,     0,     0,
       0,   557,     0,     0,   512,   561,   562,     0,   518,   519,
    -267,     0,   272,   273,  -267,   274,   275,   276,   277,     0,
     434,     0,     0,     0,   556,     0,   240,     0,   241,   575,
     560,     0,     0,   341,   278,     0,   581,     0,   279,   622,
     585,     0,   587,     0,     0,   352,     0,     0,     0,   425,
     242,   243,   244,   245,   246,   247,     0,     7,   599,     9,
       0,    10,    11,    12,   248,   249,   250,   251,   252,     0,
     545,     0,   592,    20,   609,    21,   -72,     0,     0,  -347,
    -349,   533,     0,   437,     0,     0,   437,   555,   240,     0,
     241,   436,     0,     0,   436,     0,     0,     0,     0,     0,
       0,     0,   623,   573,     0,   625,     0,   626,   531,   568,
     437,   615,   242,   243,   244,   245,   246,   247,   436,     0,
       0,   577,     0,     0,   637,     0,   248,   249,   250,   251,
     252,   596,   437,   617,     0,   240,   589,   241,     0,     0,
     436,     0,     0,   563,     0,   598,     0,     0,     0,     0,
       0,   638,     0,     0,   603,     0,     0,     0,     0,   242,
     243,   244,   245,   246,   247,     0,   487,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   437,     0,
       0,   248,   249,   250,   251,   252,   436,     4,     5,     6,
       7,     8,     9,     0,    10,    11,    12,    13,    14,    15,
       0,    16,   630,    17,    18,    19,    20,     0,    21,    22,
      23,    24,     0,  -349,    25,    26,    27,    28,    29,    30,
       0,    31,     0,     0,    32,    33,    34,    35,    36,    37,
      38,     0,    39,  -446,  -446,  -446,  -446,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,   248,   249,   250,
     251,   252,     0,     0,    49,     0,    50,     0,    51,     0,
       0,     0,     0,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
       0,    68,     0,    69,    70,    71,    72,     0,    73,     4,
       5,     6,     0,     8,     0,     0,     0,     0,     0,    13,
      14,     0,     0,    16,     0,    17,    18,    19,     0,     0,
       0,     0,    23,    24,     0,     0,    25,    26,    27,    28,
      29,    30,     0,    31,     0,     0,    32,    33,    34,    35,
      36,    37,    38,     0,    39,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,     0,
       0,     0,     0,     0,     0,     0,    49,     0,    50,     0,
     432,     0,     0,     0,     0,    52,    53,     0,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,     0,    68,     0,    69,    70,     0,    72,     0,
      73,     4,     5,     6,     7,     8,     9,     0,    10,    11,
      12,    13,    14,    15,     0,    16,     0,    17,    18,    19,
      20,     0,    21,    22,    23,    24,     0,  -349,    25,    26,
      27,    28,    29,    30,     0,    31,     0,     0,    32,    33,
      34,    35,    36,    37,   213,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,     0,     0,     0,     0,     0,     0,     0,    49,     0,
      50,     0,   147,   214,     0,     0,     0,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,     0,    68,     0,    69,    70,    71,
      72,     4,     5,     6,     7,     8,     9,     0,    10,    11,
      12,    13,    14,    15,     0,    16,     0,    17,    18,    19,
      20,     0,    21,    22,    23,    24,     0,  -349,    25,    26,
      27,    28,    29,    30,     0,    31,     0,     0,    32,    33,
      34,    35,    36,    37,   213,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,     0,     0,     0,     0,     0,     0,     0,    49,     0,
      50,     0,   147,  -438,     0,     0,     0,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     4,     5,    68,     0,    69,    70,    71,
      72,     0,     0,    13,    14,     0,     0,    16,     0,    17,
      18,    19,     0,     0,     0,     0,    23,    24,     0,     0,
      25,    26,     0,    28,    29,    30,     0,    31,     0,     0,
      32,    33,    34,    35,    36,    37,   289,     0,     0,   290,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,     0,     0,     0,     0,     0,     0,     0,
      49,     0,    50,     0,     0,     0,     0,     0,     0,    52,
      53,     0,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     4,     5,    68,     0,    69,
      70,     0,    72,     0,     0,    13,    14,     0,     0,    16,
       0,    17,    18,    19,     0,     0,     0,     0,    23,    24,
       0,     0,    25,    26,     0,    28,    29,    30,     0,    31,
       0,     0,    32,    33,    34,    35,    36,    37,   213,     0,
       0,     0,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,     0,     0,     0,     0,     0,
       0,     0,    49,     0,    50,     0,     0,     0,     0,     0,
       0,    52,    53,     0,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,     0,    68,
       0,    69,    70,     0,    72,     4,     5,     6,     7,     8,
       9,     0,    10,    11,    12,    13,    14,    15,     0,    16,
       0,    17,    18,    19,    20,     0,    21,    22,    23,    24,
    -347,  -349,    25,    26,    27,    28,    29,    30,     0,    31,
     240,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   208,     0,     0,
       0,    45,    46,    47,   242,   243,   244,   245,   246,   247,
       0,     0,    49,     0,    50,     0,   318,     0,   248,   249,
     250,   251,   252,    54,    55,    56,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    68,
       0,    69,    70,    71,    72,     4,     5,     6,     7,     8,
       9,     0,    10,    11,    12,    13,    14,    15,     0,    16,
       0,    17,    18,    19,    20,     0,    21,    22,    23,    24,
       0,  -349,    25,    26,    27,    28,    29,    30,     0,    31,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     242,   243,   244,   245,  -446,  -446,     0,   208,     0,     0,
       0,    45,    46,    47,   248,   249,   250,   251,   252,     0,
       0,     0,    49,     0,    50,     0,   318,     0,     0,     0,
       0,     0,     0,    54,    55,    56,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     4,     5,     6,    68,
       8,    69,    70,    71,    72,     0,    13,    14,     0,     0,
      16,     0,    17,    18,    19,     0,     0,     0,     0,    23,
      24,     0,     0,    25,    26,    27,    28,    29,    30,     0,
      31,     0,     0,     0,     0,     0,     0,     0,     0,   406,
       0,     0,     0,     0,     0,     0,     0,     0,   208,     0,
       0,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,    49,     0,    50,     0,   407,     0,     0,
       0,     0,     0,     0,     0,    55,    56,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,     6,
      68,     8,    69,    70,     0,    72,     0,    13,    14,     0,
       0,    16,     0,    17,    18,    19,     0,     0,     0,     0,
      23,    24,     0,     0,    25,    26,    27,    28,    29,    30,
       0,    31,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     4,     5,    45,    46,    47,     0,     0,     0,     0,
       0,    13,    14,     0,    49,    16,    50,    17,    18,    19,
       0,     0,     0,     0,    23,    24,    55,    56,    25,    26,
       0,    28,    29,    30,     0,    31,     0,     0,     0,     0,
       0,    68,     0,    69,    70,     0,    72,     0,     0,     0,
       0,     0,     0,   208,     0,     0,     5,    45,    46,    47,
       0,     0,     0,     0,     0,     0,    14,     0,    49,     0,
      50,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      55,    56,    25,     0,     0,    28,    29,    30,     0,    31,
       0,     0,     0,     0,     0,    68,     0,    69,    70,     0,
      72,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,   340,     0,    50,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,    56,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    69,    70,     0,    72,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,     0,
       0,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    47,     0,     0,     0,     0,     0,     5,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   254,     0,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,     0,    69,
      70,     0,    72,    32,    33,    34,    35,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,    40,   282,    42,
      43,    44,     0,    46,     0,    48,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    52,    53,     0,   283,     0,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,     0,     0,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,     0,     0,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,     0,     0,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,     0,   201,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
       0,     0,     0,     0,    32,    33,    34,    35,    36,    37,
     289,     0,     0,   290,     0,   351,     0,     0,    40,   282,
      42,    43,    44,     0,     0,     0,    48,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    52,    53,     0,     0,   455,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67
};

static const short int yycheck[] =
{
       1,    15,    38,    51,   206,    71,   166,   167,     1,    51,
      80,    81,   166,   167,     1,   106,   107,   108,   109,     1,
      51,     1,   325,    96,     1,   361,     5,   103,     7,     0,
       0,    48,     0,     0,   126,     5,    70,     7,   130,     0,
      70,     5,    15,     7,   314,    15,    61,   317,    49,    79,
       5,    70,     7,    32,    71,   140,    49,   142,    51,    61,
      15,    82,    32,    70,    51,    99,    81,    68,    32,    48,
     106,   107,    79,    78,    51,    68,    81,    32,    61,    81,
      74,    68,    96,    77,    48,     5,    68,     7,    68,   103,
      81,    68,    71,    74,   504,    15,    77,   507,    82,    99,
     100,    69,   168,     4,    74,    73,    46,   108,   109,    77,
      78,    81,    32,    81,    73,    99,   100,    78,   102,    74,
      75,   531,   140,    96,   142,   104,    81,    98,    77,    78,
      98,   101,    28,    36,   226,    70,   140,    98,   142,   140,
     104,   142,    29,   553,   158,   159,   101,   140,    78,   142,
     164,   221,    77,   140,    74,   142,    59,   171,   172,    77,
      78,    81,    70,   140,   165,   142,    60,    61,   169,   170,
     206,    56,    57,    58,   106,   107,   224,    77,    78,    82,
      70,   101,    77,    98,    77,    14,    61,    61,    82,   599,
      81,    73,    78,   224,    39,    78,    99,   100,    82,   102,
      25,    81,    71,    71,   205,    99,   100,    77,   102,    75,
     211,    78,    74,   549,    78,    73,   308,   309,   232,   311,
      69,    38,     9,    78,    16,    71,    18,    75,   229,    23,
      62,   224,   104,   325,   304,   305,    78,   224,   239,   309,
      71,   307,    78,    71,   258,    71,   239,   224,    40,    41,
      42,    43,    44,    45,    77,    77,    75,    71,   561,    75,
      78,    74,    54,    55,    56,    57,    58,    77,    46,    47,
     318,    49,    50,    51,    52,   345,    71,   104,    75,    78,
      78,    71,    75,   462,    76,   445,   359,   318,   151,   536,
      68,   445,    70,   363,    72,   149,   149,   330,   299,   501,
     579,    79,   107,   109,   500,   607,   299,   377,   298,   620,
     299,   123,    45,   314,   591,   316,   317,   299,   410,   392,
     224,   314,    68,   316,   317,   318,    -1,    -1,    -1,    -1,
     422,   318,   314,    -1,   314,   317,   428,   317,    -1,   340,
      -1,   318,    -1,    -1,    -1,    -1,   438,   340,    -1,    -1,
      -1,    -1,    -1,    -1,   446,    -1,   370,   427,   424,    19,
      20,    -1,    -1,    -1,    -1,   457,   367,    -1,    -1,    -1,
      -1,   462,   442,   443,    -1,    -1,    -1,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,   389,   390,
     432,    -1,    -1,    -1,   395,   396,   397,   398,   399,   400,
     393,    -1,    62,    63,    64,    65,    66,    67,    46,    47,
      -1,    49,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,
      -1,   513,    -1,    -1,   425,   517,   518,    -1,   429,   430,
      68,    -1,    46,    47,    72,    49,    50,    51,    52,    -1,
     441,    -1,    -1,    -1,   510,    -1,    16,    -1,    18,   541,
     516,    -1,    -1,   613,    68,    -1,   548,    -1,    72,   613,
     552,    -1,   554,    -1,    -1,   501,    -1,    -1,    -1,   561,
      40,    41,    42,    43,    44,    45,    -1,     6,   570,     8,
      -1,    10,    11,    12,    54,    55,    56,    57,    58,    -1,
     491,    -1,   562,    22,   586,    24,    25,    -1,    -1,    28,
      29,    71,    -1,   504,    -1,    -1,   507,   508,    16,    -1,
      18,   504,    -1,    -1,   507,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   614,   537,    -1,   617,    -1,   619,   620,   530,
     531,   597,    40,    41,    42,    43,    44,    45,   531,    -1,
      -1,   542,    -1,    -1,   636,    -1,    54,    55,    56,    57,
      58,   565,   553,   601,    -1,    16,   557,    18,    -1,    -1,
     553,    -1,    -1,    71,    -1,   566,    -1,    -1,    -1,    -1,
      -1,   637,    -1,    -1,   575,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    -1,   579,    40,    41,    42,
      43,    44,    45,    54,    55,    56,    57,    58,   599,    -1,
      -1,    54,    55,    56,    57,    58,   599,     3,     4,     5,
       6,     7,     8,    -1,    10,    11,    12,    13,    14,    15,
      -1,    17,   623,    19,    20,    21,    22,    -1,    24,    25,
      26,    27,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    -1,    -1,    40,    41,    42,    43,    44,    45,
      46,    -1,    48,    40,    41,    42,    43,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    54,    55,    56,
      57,    58,    -1,    -1,    70,    -1,    72,    -1,    74,    -1,
      -1,    -1,    -1,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    -1,
      -1,    97,    -1,    99,   100,   101,   102,    -1,   104,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    -1,    -1,    13,
      14,    -1,    -1,    17,    -1,    19,    20,    21,    -1,    -1,
      -1,    -1,    26,    27,    -1,    -1,    30,    31,    32,    33,
      34,    35,    -1,    37,    -1,    -1,    40,    41,    42,    43,
      44,    45,    46,    -1,    48,    -1,    -1,    -1,    -1,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    70,    -1,    72,    -1,
      74,    -1,    -1,    -1,    -1,    79,    80,    -1,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    -1,    -1,    97,    -1,    99,   100,    -1,   102,    -1,
     104,     3,     4,     5,     6,     7,     8,    -1,    10,    11,
      12,    13,    14,    15,    -1,    17,    -1,    19,    20,    21,
      22,    -1,    24,    25,    26,    27,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    -1,
      72,    -1,    74,    75,    -1,    -1,    -1,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    -1,    -1,    97,    -1,    99,   100,   101,
     102,     3,     4,     5,     6,     7,     8,    -1,    10,    11,
      12,    13,    14,    15,    -1,    17,    -1,    19,    20,    21,
      22,    -1,    24,    25,    26,    27,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    -1,
      72,    -1,    74,    75,    -1,    -1,    -1,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,     3,     4,    97,    -1,    99,   100,   101,
     102,    -1,    -1,    13,    14,    -1,    -1,    17,    -1,    19,
      20,    21,    -1,    -1,    -1,    -1,    26,    27,    -1,    -1,
      30,    31,    -1,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    42,    43,    44,    45,    46,    -1,    -1,    49,
      -1,    -1,    -1,    -1,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    -1,    72,    -1,    -1,    -1,    -1,    -1,    -1,    79,
      80,    -1,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,     3,     4,    97,    -1,    99,
     100,    -1,   102,    -1,    -1,    13,    14,    -1,    -1,    17,
      -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,    27,
      -1,    -1,    30,    31,    -1,    33,    34,    35,    -1,    37,
      -1,    -1,    40,    41,    42,    43,    44,    45,    46,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    70,    -1,    72,    -1,    -1,    -1,    -1,    -1,
      -1,    79,    80,    -1,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    -1,    -1,    97,
      -1,    99,   100,    -1,   102,     3,     4,     5,     6,     7,
       8,    -1,    10,    11,    12,    13,    14,    15,    -1,    17,
      -1,    19,    20,    21,    22,    -1,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,
      -1,    59,    60,    61,    40,    41,    42,    43,    44,    45,
      -1,    -1,    70,    -1,    72,    -1,    74,    -1,    54,    55,
      56,    57,    58,    81,    82,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,
      -1,    99,   100,   101,   102,     3,     4,     5,     6,     7,
       8,    -1,    10,    11,    12,    13,    14,    15,    -1,    17,
      -1,    19,    20,    21,    22,    -1,    24,    25,    26,    27,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      40,    41,    42,    43,    44,    45,    -1,    55,    -1,    -1,
      -1,    59,    60,    61,    54,    55,    56,    57,    58,    -1,
      -1,    -1,    70,    -1,    72,    -1,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    81,    82,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    97,
       7,    99,   100,   101,   102,    -1,    13,    14,    -1,    -1,
      17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,
      27,    -1,    -1,    30,    31,    32,    33,    34,    35,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    -1,    59,    60,    61,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    -1,    72,    -1,    74,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    83,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      97,     7,    99,   100,    -1,   102,    -1,    13,    14,    -1,
      -1,    17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,
      26,    27,    -1,    -1,    30,    31,    32,    33,    34,    35,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      -1,     3,     4,    59,    60,    61,    -1,    -1,    -1,    -1,
      -1,    13,    14,    -1,    70,    17,    72,    19,    20,    21,
      -1,    -1,    -1,    -1,    26,    27,    82,    83,    30,    31,
      -1,    33,    34,    35,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    97,    -1,    99,   100,    -1,   102,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    -1,    -1,     4,    59,    60,    61,
      -1,    -1,    -1,    -1,    -1,    -1,    14,    -1,    70,    -1,
      72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    83,    30,    -1,    -1,    33,    34,    35,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    97,    -1,    99,   100,    -1,
     102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    60,    61,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    70,    -1,    72,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    99,   100,    -1,   102,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    -1,
      -1,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    82,    -1,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    99,
     100,    -1,   102,    40,    41,    42,    43,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    55,    56,
      57,    58,    -1,    60,    -1,    62,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    79,    80,    -1,    82,    -1,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    -1,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    -1,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    -1,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    82,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,    45,
      46,    -1,    -1,    49,    -1,    82,    -1,    -1,    54,    55,
      56,    57,    58,    -1,    -1,    -1,    62,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    79,    80,    -1,    -1,    82,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
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
      34,    82,   195,   198,   266,   190,   268,   318,    55,   109,
     154,   183,   269,    46,    75,   109,   126,   154,   245,   247,
     248,   320,   332,   333,   334,   339,    70,   138,    82,   328,
     110,    61,    70,    79,   122,   123,   134,   135,   134,   149,
      16,    18,    40,    41,    42,    43,    44,    45,    54,    55,
      56,    57,    58,     4,    82,   115,   195,   198,   219,    46,
      70,    79,   209,   115,    19,    20,    62,    63,    64,    65,
      66,    67,    46,    47,    49,    50,    51,    52,    68,    72,
     228,   230,    55,    82,   194,   195,   202,   229,   229,    46,
      49,   117,   154,   194,   231,   231,   236,   237,   239,   243,
     244,    74,    77,   271,    28,    29,    70,   281,   281,   108,
     292,    70,    99,   108,    78,   134,   326,    78,    74,   109,
     319,   335,   336,   335,   136,   137,   139,   140,   141,   144,
     145,   148,   107,    70,    70,   115,   115,    81,   115,   154,
      70,   216,   220,   220,   138,   154,   154,   115,   115,    77,
     154,    82,   195,   266,   205,    76,   204,   154,   184,   246,
      77,    78,   134,   320,   333,   337,    98,   108,   154,    14,
     113,   115,   118,   129,    81,   109,   147,   154,   174,   175,
     168,   167,   170,   169,   171,   172,   177,   178,   180,   179,
     181,   115,   200,   210,    61,   160,   161,   163,   162,   164,
     165,   233,   234,    73,    78,   236,    46,    74,   109,   154,
     241,   242,   255,   260,   262,   269,   270,   272,    39,   274,
     134,   134,   282,   138,   108,   108,   134,   218,   293,   294,
     108,    82,    74,   109,   154,   327,   109,   154,   265,   154,
     327,    78,   218,   142,   127,   305,    81,    81,   154,   315,
     134,   157,    71,    71,    77,    82,   198,   322,   248,    75,
     154,   329,    36,    59,   115,   116,    78,   124,    74,   131,
     134,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   198,    48,   104,   109,   206,   207,
     212,   213,   154,   154,   154,   154,   154,   154,    73,    69,
     240,   261,   256,   108,   263,    77,    38,   275,   108,     9,
     285,   138,   154,    78,   134,   108,   295,   296,   154,   154,
     320,   108,   134,   134,    75,   220,   108,    23,    62,   185,
     186,   108,   264,    71,   104,   117,   111,   119,    48,    71,
     120,   132,   214,   211,    78,   154,   244,   266,   249,   250,
     251,   253,   265,   273,   265,   154,   138,   108,   297,   298,
     138,   108,   108,    71,   306,   316,   158,    71,   154,   265,
      78,   323,   122,   115,   121,   108,   133,   154,    71,   208,
      77,   108,   248,   252,   257,   108,   265,   108,   283,   154,
      71,   218,   134,    77,    78,   300,   115,   312,   154,   108,
      75,   112,    71,   154,    75,   215,   212,    78,    77,   108,
     276,    71,   297,   301,   307,   138,   265,   126,   104,   249,
     258,    75,   220,   108,   299,   108,   108,   254,   264,   302,
     154,   308,   259,    78,    71,    75,   309,   108,   138
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
      yyerror (&yylloc, lexer, YY_("syntax error: cannot back up")); \
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
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value, Location);	\
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
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");

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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;

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
int yyparse (yyFlexLexer& lexer);
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
yyparse (yyFlexLexer& lexer)
#else
int
yyparse (lexer)
    yyFlexLexer& lexer;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;

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

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended. */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

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
  yylsp = yyls;
#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

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
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
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
  *++yylsp = yylloc;

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

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, yylsp - yylen, yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

    {
								Node* program = ASTCREATOR_BY_CONTEXT.MakeNode_Program((yyvsp[0].node));
								if ((yyvsp[0].node))
									SET_LOCATION(program, (yylsp[0]), (yylsp[0]));
								ASTCREATOR_BY_CONTEXT.SetSyntaxTree(program);
							;}
    break;

  case 3:

    { ASTCREATOR_BY_CONTEXT.SetSyntaxTree((yyvsp[0].node)); ;}
    break;

  case 4:

    {
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts((yyvsp[-1].node), (yyvsp[0].node));
								if ((yyvsp[-1].node)) {
									SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
									SET_LOCATION((yyloc), (yylsp[-1]), (yylsp[0]));
								}
								else {
									SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
									SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0]));								
								}
							;}
    break;

  case 5:

    { (yyval.node) = NIL_NODE; ;}
    break;

  case 6:

    { (yyval.line) = PARSEPARMS_BY_CONTEXT.GetLine(); ;}
    break;

  case 7:

    { SM(S_FUNC_((yyvsp[0].constStr))); PE2(T_FUNC_((yyvsp[0].constStr)), T_LOCAL_OR_FUNCNAME); ;}
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
							EM(S_FUNC_((yyvsp[-8].constStr)));
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Function((yyvsp[-8].constStr), (yyvsp[-5].node), (yyvsp[-6].constStr), (yyvsp[-3].node), (yyvsp[-1].node));
							DPTR((yyval.node))->SetLine((yyvsp[0].line));
							SET_LOCATION((yyval.node), (yylsp[-8]), (yylsp[-1]));
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

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Name((yyvsp[0].constStr), true);	SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 17:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Name((yyvsp[0].constStr));		SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 18:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 19:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 20:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Name((yyvsp[0].constStr)); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 21:

    {
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Name(AST_VALUE_TOSTRING_SYMBOLIC_NAME);
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 22:

    { unullify((yyval.node)); ;}
    break;

  case 23:

    { (yyval.constStr) = (yyvsp[0].constStr);							;}
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

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-3].nodeList), (yyvsp[0].node)); ;}
    break;

  case 28:

    { PE(T_IDENT); (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, (yyvsp[0].node)); ;}
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
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Name(AST_VALUE_VARARGS_FORMAL_NAME);
								SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[-2]));
							;}
    break;

  case 33:

    {
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_FormalArgs((yyvsp[0].nodeList));
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 34:

    { EG(S_FARGS); PE(T_RPAR); unullify((yyval.nodeList)); ;}
    break;

  case 35:

    { OE(T_LPAR_OR_TDOT); ;}
    break;

  case 36:

    { (yyval.nodeList) = (yyvsp[0].node) ? ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-2].nodeList), (yyvsp[0].node)) : (yyvsp[-2].nodeList); ;}
    break;

  case 37:

    { EG(S_FARGS); PE(T_RPAR); unullify((yyval.nodeList)); ;}
    break;

  case 38:

    { PE(T_LBC); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 39:

    { PE(T_RBC); ;}
    break;

  case 40:

    {  (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Compound((yyvsp[-2].node)); SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0])); SET_LOCATION((yyloc), (yylsp[-3]), (yylsp[0])); ;}
    break;

  case 41:

    { PE(T_LBC); ;}
    break;

  case 42:

    {
							EM(S_FUNC_(AST_VALUE_FUNCCLASS_PROGRAMFUNCTION));
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_LambdaFunction((yyvsp[-2].node), (yyvsp[0].node));
							SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
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

    { (yyval.node) = (yyvsp[-1].node); ;}
    break;

  case 46:

    { PE(T_RBC); (yyval.node) = (yyvsp[0].node); DPTR((yyval.node))->SetLine((yyvsp[-1].line)); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 47:

    { PE(T_SEMI); ;}
    break;

  case 48:

    { SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 49:

    { SM(S_STMT); ;}
    break;

  case 50:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ExpressionListStmt((yyvsp[-1].nodeList)); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); SET_LOCATION((yyloc), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 51:

    {
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt((yyvsp[0].node));  DPTR((yyval.node))->SetLine((yyvsp[-1].line));
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 52:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 53:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionStmt((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 54:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_EmptyStmt();  SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));;}
    break;

  case 55:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 56:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 57:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 58:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 59:

    { EM(S_STMT); (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 60:

    { EM(S_STMT); (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 61:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 62:

    { (yyval.node) = (yyvsp[0].node);;}
    break;

  case 63:

    { (yyval.node) = (yyvsp[0].node);;}
    break;

  case 64:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 65:

    { (yyval.node) = (yyvsp[0].node);;}
    break;

  case 66:

    { EM(S_STMT); (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 67:

    { EM(S_STMT); (yyval.node) = (yyvsp[0].node);;}
    break;

  case 68:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 69:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 70:

    { SM(S_ASSRT); SG(S_EXPR); ;}
    break;

  case 71:

    {
							EG(S_EXPR);
							PE(S_EXPR);
							EM(S_ASSRT);
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_AssertStmt((yyvsp[0].node));
							SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0]));
						;}
    break;

  case 72:

    { SM(S_STMT); PE(S_ASSRT);	;}
    break;

  case 73:

    { (yyval.node) = (yyvsp[-1].node); SET_LOCATION((yyloc), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 74:

    { SM(S_RET); ;}
    break;

  case 75:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Return((yyvsp[-1].node)); ;}
    break;

  case 76:

    { MAKE_FUNCTION_EXPR((yyvsp[0].node), (yyval.node)); (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Return((yyval.node)); ;}
    break;

  case 77:

    { PE2(T_RET, S_EXPR); ;}
    break;

  case 78:

    { EM(S_RET); (yyval.node) = (yyvsp[0].node); SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0])); ;}
    break;

  case 79:

    { EM(S_RET); (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Return(); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 80:

    { (yyval.node) = (yyvsp[0].node); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 81:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 82:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 83:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_UsingNamespace((yyvsp[-2].nodeList), (yyvsp[-1].node)); ;}
    break;

  case 84:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_UsingNamespace(NIL_NODELIST, (yyvsp[-1].node)); ;}
    break;

  case 85:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_UsingByteCodeLibrary((yyvsp[-2].node)); DPTR((yyval.node))->SetLine((yyvsp[0].line)); SET_LOCATION((yyloc), (yylsp[-3]), (yylsp[-1])); ;}
    break;

  case 86:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 87:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 88:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 89:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 90:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 91:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 92:

    { (yyval.node) = (yyvsp[0].node); ;}
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
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ConstDefExpression((yyvsp[-4].node), (yyvsp[0].node));
							SET_LOCATION((yyval.node), (yylsp[-6]), (yylsp[0]));
						;}
    break;

  case 97:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ASSIGN, S_EXPR); ;}
    break;

  case 98:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_AssignExpr((yyvsp[-3].node), (yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 99:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ADDA, S_EXPR); ;}
    break;

  case 100:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_ADD_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 101:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MULA, S_EXPR); ;}
    break;

  case 102:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_MUL_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 103:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_SUBA, S_EXPR); ;}
    break;

  case 104:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_SUB_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 105:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_DIVA, S_EXPR); ;}
    break;

  case 106:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_DIV_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 107:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MODA, S_EXPR); ;}
    break;

  case 108:

    {
								EM(S_ASSIGNEXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_AssignArithExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_MOD_ASSIGN);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 109:

    { SM(S_RELAT); PE3(S_EXPR, T_GT, S_EXPR); ;}
    break;

  case 110:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_GT);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 111:

    { SM(S_RELAT); PE3(S_EXPR, T_LT, S_EXPR); ;}
    break;

  case 112:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_LT);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 113:

    { SM(S_RELAT); PE3(S_EXPR, T_GE, S_EXPR); ;}
    break;

  case 114:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr((yyvsp[-3].node), (yyvsp[0].node),AST_TAG_OP_GE);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 115:

    { SM(S_RELAT); PE3(S_EXPR, T_LE, S_EXPR); ;}
    break;

  case 116:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_LE);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 117:

    { SM(S_RELAT); PE3(S_EXPR, T_EQ, S_EXPR); ;}
    break;

  case 118:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_EQ);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 119:

    { SM(S_RELAT); PE3(S_EXPR, T_NE, S_EXPR); ;}
    break;

  case 120:

    {
								EM(S_RELAT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_RelationalExpr((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_NE);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 121:

    { SM(S_LOGICAL); PE3(S_EXPR, T_AND, S_EXPR); ;}
    break;

  case 122:

    {
								EM(S_LOGICAL);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ExprANDExpr((yyvsp[-3].node), (yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 123:

    { SM(S_LOGICAL); PE3(S_EXPR, T_OR, S_EXPR); ;}
    break;

  case 124:

    {
								EM(S_LOGICAL);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ExprORExpr((yyvsp[-3].node), (yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 125:

    { SM(S_ARITH); PE3(S_EXPR, T_ADD, S_EXPR); ;}
    break;

  case 126:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_ADD);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 127:

    { SM(S_ARITH); PE3(S_EXPR, T_SUB, S_EXPR); ;}
    break;

  case 128:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_SUB);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 129:

    { SM(S_ARITH); PE3(S_EXPR, T_DIV, S_EXPR); ;}
    break;

  case 130:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression((yyvsp[-3].node), (yyvsp[0].node),AST_TAG_OP_DIV);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 131:

    { SM(S_ARITH); PE3(S_EXPR, T_MUL, S_EXPR); ;}
    break;

  case 132:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression((yyvsp[-3].node),  (yyvsp[0].node), AST_TAG_OP_MUL);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 133:

    { SM(S_ARITH); PE3(S_EXPR, T_MOD, S_EXPR); ;}
    break;

  case 134:

    {
								EM(S_ARITH);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ArithmeticExpression((yyvsp[-3].node), (yyvsp[0].node), AST_TAG_OP_MOD);
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 135:

    { EM(S_TERNARY); (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Ternary((yyvsp[-4].node), (yyvsp[-3].node), (yyvsp[-1].node)); SET_LOCATION((yyval.node), (yylsp[-5]), (yylsp[0])); ;}
    break;

  case 136:

    { SM(S_TERNARY); PE3(S_EXPR, T_QMARK, S_EXPR); (yyval.node) = (yyvsp[-1].node); ;}
    break;

  case 137:

    { PE(T_COLON); (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 138:

    { PE(S_EXPR); ;}
    break;

  case 139:

    { PE(T_RPAR); (yyval.node) = (yyvsp[0].node); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 140:

    {
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TermLvalueArith((yyvsp[-1].node), AST_LVALUE_PLUSPLUS);
								SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
							;}
    break;

  case 141:

    { SM(S_EXPR); PE2(T_PLUSPLUS, S_LVAL); ;}
    break;

  case 142:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TermLvalueArith((yyvsp[0].node), AST_PLUSPLUS_LVALUE);
								SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0]));
							;}
    break;

  case 143:

    {
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TermLvalueArith((yyvsp[-1].node), AST_LVALUE_MINUSMINUS);
								SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
							;}
    break;

  case 144:

    { SM(S_EXPR); PE2(T_MINUSMINUS, S_LVAL); ;}
    break;

  case 145:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TermLvalueArith((yyvsp[0].node), AST_MINUSMINUS_LVALUE);
								SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0]));
							;}
    break;

  case 146:

    { SM(S_EXPR); PE2(T_SUB, S_EXPR); ;}
    break;

  case 147:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_UMINUSExpression((yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0]));
							;}
    break;

  case 148:

    { SM(S_EXPR); PE2(T_NOT, S_EXPR); ;}
    break;

  case 149:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_NOTExpression((yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0]));
							;}
    break;

  case 150:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_PrimaryExpression((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 151:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 152:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 153:

    { (yyval.node) = (yyvsp[0].node); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 154:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 155:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ConstValue((yyvsp[0].numberConst));		;}
    break;

  case 156:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ConstValue();		;}
    break;

  case 157:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ConstValue(true);	;}
    break;

  case 158:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ConstValue(false);	;}
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

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_StringConst((yyvsp[-1].node), (yyvsp[0].dynamicStr)); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 213:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_StringConst((yyvsp[0].dynamicStr)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 214:

    { EI(S_EXPR); PE(T_STRINGIFY); ;}
    break;

  case 215:

    { (yyval.constStr) = (yyvsp[0].constStr); ;}
    break;

  case 216:

    { (yyval.constStr) = (yyvsp[0].constStr); ;}
    break;

  case 217:

    { PE(T_IDENT); (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_StringifyDottedIdents((yyvsp[0].constStr)); ;}
    break;

  case 218:

    { PE2(T_DOT, T_IDENT); ;}
    break;

  case 219:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_StringifyDottedIdents((yyvsp[-3].node), (yyvsp[0].constStr)); ;}
    break;

  case 220:

    {
								EG(S_NAMESPACE);
								PE(T_IDENT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_StringifyNamespaceIdent((yyvsp[-1].nodeList), (yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0]));
							;}
    break;

  case 221:

    { PE(T_STRINGCONST); (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 222:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 223:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 224:

    {
								EI(S_EXPR); PE(T_SELF);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_SELF();
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 225:

    {
								EI(S_EXPR); PE(T_LAMBDA_REF);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_LAMBDA_REF();
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 226:

    {
								EI(S_EXPR); PE(T_NEWSELF);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_NEWSELF();
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 227:

    {
								EI(S_EXPR); PE(T_ARGUMENTS);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ARGUMENTS();
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 228:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 229:

    { PE(S_TABLE); (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 230:

    { PE3(T_LPAR, S_EXPR, T_RPAR); ;}
    break;

  case 231:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ParenthesisedExpr((yyvsp[-2].node)); SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[-1])); ;}
    break;

  case 232:

    { PE(S_CALL); (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 233:

    { SM(S_EXPR); PE3(T_LPAR, S_FUNC, T_RPAR); ;}
    break;

  case 234:

    {
								EM(S_EXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionParenthesisForm((yyvsp[-2].node));
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
							;}
    break;

  case 235:

    { unullify((yyval.nodeList)); ;}
    break;

  case 236:

    { (yyval.nodeList) = (yyvsp[0].nodeList); ;}
    break;

  case 237:

    { PE(T_COMMA); ;}
    break;

  case 238:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-3].nodeList), (yyvsp[0].node)); ;}
    break;

  case 239:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, (yyvsp[0].node)); ;}
    break;

  case 240:

    { SM(S_CALL); PE2(S_EXPR, T_LPAR); SG(S_AARGS); ;}
    break;

  case 241:

    { PE(T_RPAR); ;}
    break;

  case 242:

    { EG(S_AARGS); EM(S_CALL); (yyval.nodeList) = (yyvsp[-2].nodeList); ;}
    break;

  case 243:

    { PE2(T_LPAR, T_RPAR); unullify((yyval.nodeList)); ;}
    break;

  case 244:

    { PE(S_EXPR); ;}
    break;

  case 245:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Argument((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 246:

    { PE2(T_BAR, S_EXPR); ;}
    break;

  case 247:

    { PE(T_BAR); ;}
    break;

  case 248:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_LateBoundArgument((yyvsp[-2].node)); SET_LOCATION((yyval.node), (yylsp[-4]), (yylsp[0])); ;}
    break;

  case 249:

    { PE(T_TDOT); (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TRIPLE_DOT(); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 250:

    { MAKE_FUNCTION_EXPR((yyvsp[0].node), (yyval.node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 251:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 252:

    { EI(S_EXPR); (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 253:

    {
							Node* actuals = ASTCREATOR_BY_CONTEXT.MakeNode_ActualArguments((yyvsp[0].nodeList));
							SET_LOCATION(actuals, (yylsp[0]), (yylsp[0]));
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionCall((yyvsp[-1].node), actuals);
							SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
						;}
    break;

  case 254:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-3].nodeList), (yyvsp[0].node)); DPTR((yyvsp[0].node))->SetLine((yyvsp[-1].line)); ;}
    break;

  case 255:

    {
								(yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, (yyvsp[0].node)); DPTR((yyvsp[0].node))->SetLine((yyvsp[-1].line));
								SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 256:

    {
								EI(S_EXPR); SG(S_NAMESPACE); PE2(T_GLOBAL, T_IDENT);
								Node* global = ASTCREATOR_BY_CONTEXT.MakeNode_Name(DELTA_LIBRARYNAMESPACE_SEPARATOR);
								SET_LOCATION(global, (yylsp[0]), (yylsp[0]));
								(yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, global);
							;}
    break;

  case 257:

    {
								EI(S_EXPR); SG(S_NAMESPACE); PE3(T_IDENT, T_GLOBAL, T_IDENT);
								(yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, (yyvsp[-1].node));
							;}
    break;

  case 258:

    {
								PE2(T_GLOBAL, T_IDENT);
								(yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-2].nodeList), (yyvsp[-1].node));
							;}
    break;

  case 259:

    {
								EI(S_EXPR); PE(T_IDENT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Lvalue((yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 260:

    {
								EG(S_NAMESPACE);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_NamespaceLvalue((yyvsp[-1].nodeList), (yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
							;}
    break;

  case 261:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_AttrLvalue((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 262:

    { EI(S_EXPR); PE2(T_STATIC, T_IDENT); ;}
    break;

  case 263:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_StaticLvalue((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0])); ;}
    break;

  case 264:

    { EI(S_EXPR); PE2(T_LOCAL, T_IDENT); ;}
    break;

  case 265:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_LocalLvalue((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0])); ;}
    break;

  case 266:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 267:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 268:

    { PE(T_DOT); OE(S_DOTINDEX); (yyval.node) = (yyvsp[-1].node); ;}
    break;

  case 269:

    { PE(T_DDOT); OE(S_DOTINDEX); (yyval.node) = (yyvsp[-1].node); ;}
    break;

  case 270:

    { PE(T_LB); OE(S_BRACKET_INDEX); (yyval.node) = (yyvsp[-1].node); ;}
    break;

  case 271:

    { PE(T_DLB); OE(S_BRACKET_INDEX); (yyval.node) = (yyvsp[-1].node); ;}
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

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexIdent((yyvsp[0].constStr)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 277:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexIdent((yyvsp[0].constStr)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 278:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexOpString((yyvsp[0].constStr)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 279:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexStringConst((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 280:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_DotIndexOpString((yyvsp[0].constStr)+1); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));  ;}
    break;

  case 281:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 282:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BracketIndexOperatorMethod((yyvsp[0].constStr)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));  ;}
    break;

  case 283:

    {
								PE(S_DOTINDEX_(DPTR((yyvsp[0].node))->GetTag())); 
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TableContentDot((yyvsp[-1].node), (yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
							;}
    break;

  case 284:

    {
								PE(S_DOTINDEX_(DPTR((yyvsp[0].node))->GetTag())); 
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TableContentDot((yyvsp[-1].node), (yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
							;}
    break;

  case 285:

    {
								PE(S_DOTINDEX_(DPTR((yyvsp[0].node))->GetTag())); 
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BoundedTableContentDot((yyvsp[-1].node), (yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
							;}
    break;

  case 286:

    { PE2(S_BRACKET_INDEX_(DPTR((yyvsp[0].node))->GetTag()), T_RB); ;}
    break;

  case 287:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TableContentBracket((yyvsp[-3].node), (yyvsp[-2].node)); SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0])); ;}
    break;

  case 288:

    { PE2(S_BRACKET_INDEX_(DPTR((yyvsp[0].node))->GetTag()), T_DRB); ;}
    break;

  case 289:

    {
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BoundedTableContentBracket((yyvsp[-3].node), (yyvsp[-2].node));
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
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

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TableConstructor((yyvsp[-1].nodeList)); SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0])); ;}
    break;

  case 294:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TableConstructor(); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 295:

    { PE(T_COMMA); ;}
    break;

  case 296:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_TableElements((yyvsp[-3].nodeList), (yyvsp[0].node)); ;}
    break;

  case 297:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_TableElements(NIL_NODELIST, (yyvsp[0].node)); ;}
    break;

  case 298:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_UnindexedValue((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 299:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_UnindexedValue((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 300:

    { MAKE_FUNCTION_EXPR((yyvsp[0].node), (yyval.node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 301:

    { OE(T_TABLE_ELEM); ;}
    break;

  case 302:

    { (yyval.node) = (yyvsp[-1].node); DPTR((yyval.node))->SetLine((yyvsp[0].line)); SET_LOCATION((yyloc), (yylsp[-1]), (yylsp[-1])); ;}
    break;

  case 303:

    { (yyval.node) = (yyvsp[0].node); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 304:

    { (yyval.node) = (yyvsp[0].node); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 305:

    { (yyval.node) = (yyvsp[0].node); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 306:

    { PE2(T_DOT, T_IDENT); ;}
    break;

  case 307:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_DottedIdent((yyvsp[0].constStr)); SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0])); ;}
    break;

  case 308:

    { PE(T_OPINDEX); (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_OperatorIndex((yyvsp[0].constStr)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 309:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 310:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 311:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 312:

    { PE(S_INDEXEXPR); ;}
    break;

  case 313:

    { (yyval.node) = (yyvsp[0].node); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 314:

    { PE(T_COMMA); ;}
    break;

  case 315:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-4].nodeList), (yyvsp[-1].node)); DPTR((yyvsp[-1].node))->SetLine((yyvsp[0].line)); SET_LOCATION((yyloc), (yylsp[-4]), (yylsp[-1])); ;}
    break;

  case 316:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, (yyvsp[-1].node)); DPTR((yyvsp[-1].node))->SetLine((yyvsp[0].line)); SET_LOCATION((yyloc), (yylsp[-1]), (yylsp[-1])); ;}
    break;

  case 317:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TableIndices((yyvsp[0].nodeList)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 318:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TableValues((yyvsp[0].nodeList)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
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

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_IndexedValues((yyvsp[-6].node), (yyvsp[-2].node)); SET_LOCATION((yyval.node), (yylsp[-8]), (yylsp[0])); ;}
    break;

  case 324:

    { (yyval.node) = (yyvsp[-1].node); ;}
    break;

  case 325:

    { PE(T_DOT); OE(T_IDENT_OR_KWDIDENT); ;}
    break;

  case 326:

    { (yyval.node) = (yyvsp[-1].node); PE(T_COLON); ;}
    break;

  case 327:

    { SG(S_EXPR); ;}
    break;

  case 328:

    {
							EG(S_EXPR); PE(S_EXPR);
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_IdentIndexElement((yyvsp[-3].node), (yyvsp[-1].node));
							DPTR((yyval.node))->SetLine((yyvsp[0].line));
							SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[-1]));
						;}
    break;

  case 329:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-3].nodeList), (yyvsp[0].node)); DPTR((yyvsp[0].node))->SetLine((yyvsp[-1].line)); ;}
    break;

  case 330:

    {
								(yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, (yyvsp[0].node)); DPTR((yyvsp[0].node))->SetLine((yyvsp[-1].line));
								SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 331:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 332:

    { MAKE_FUNCTION_EXPR((yyvsp[0].node), (yyval.node)); ;}
    break;

  case 333:

    {
								PE(T_IDENT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Name((yyvsp[0].constStr));
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 334:

    {
								PE(T_KWDIDENT);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Name((yyvsp[0].constStr));
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 335:

    {
								PE(T_ATTRIBUTEID); OE(T_LBC_OR_COLON);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Name((yyvsp[0].constStr) + 1);
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 336:

    { PE(T_ATTRIBUTE); OE(T_IDENT_OR_KWDIDENT); ;}
    break;

  case 337:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 338:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 339:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 340:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_NewAttribute((yyvsp[-2].node), (yyvsp[-1].node), (yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0])); ;}
    break;

  case 341:

    { PE(T_SET);  ;}
    break;

  case 342:

    { SG(S_EXPR); ;}
    break;

  case 343:

    { EG(S_EXPR); PE2(S_EXPR, T_GET); (yyval.node) = (yyvsp[-1].node); DPTR((yyval.node))->SetLine((yyvsp[0].line)); SET_LOCATION((yyloc), (yylsp[-5]), (yylsp[-1])); ;}
    break;

  case 344:

    { SG(S_EXPR); ;}
    break;

  case 345:

    { EG(S_EXPR); PE2(S_EXPR, T_RBC); ;}
    break;

  case 346:

    {  (yyval.node) = (yyvsp[-3].node); DPTR((yyval.node))->SetLine((yyvsp[-2].line)); ;}
    break;

  case 347:

    { SM(S_STMT); PE(T_BREAK);	;}
    break;

  case 348:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BREAK(); SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[-1])); SET_LOCATION((yyloc), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 349:

    { SM(S_STMT); PE(T_CONT);	;}
    break;

  case 350:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_CONTINUE(); SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[-1])); SET_LOCATION((yyloc), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 351:

    { SG(S_EXPR); ;}
    break;

  case 352:

    { EG(S_EXPR); PE2(S_EXPR, T_RPAR); ;}
    break;

  case 353:

    { (yyval.node) = (yyvsp[-2].node); DPTR((yyval.node))->SetLine((yyvsp[-3].line)); ;}
    break;

  case 354:

    { SM(S_IF); PE2(T_IF, T_LPAR);  ;}
    break;

  case 355:

    { EM(S_IF); SM(S_ELSE); PE(T_ELSE); ;}
    break;

  case 356:

    { EM(S_IF);   (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_IfStmt((yyvsp[-1].node), (yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0])); ;}
    break;

  case 357:

    {
								EM(S_ELSE);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_IfElseStmt((yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
								SET_LOCATION((yyval.node), (yylsp[-4]), (yylsp[0]));
							;}
    break;

  case 358:

    { SM(S_WHILE); PE2(T_WHILE, T_LPAR); ;}
    break;

  case 359:

    {
							EM(S_WHILE);
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_WhileStmt((yyvsp[-2].node), (yyvsp[0].node));
							DPTR((yyvsp[0].node))->SetLine((yyvsp[-1].line));
							SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
						;}
    break;

  case 360:

    {
							EM(S_FOR);
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ForStmt((yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[-1].node), (yyvsp[0].node));
							SET_LOCATION((yyval.node), (yylsp[-4]), (yylsp[0]));
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
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ForInit((yyvsp[-1].nodeList)); DPTR((yyval.node))->SetLine((yyvsp[-2].line));
								SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[-1])); SET_LOCATION((yyloc), (yylsp[-1]), (yylsp[0]));
							;}
    break;

  case 364:

    {
								EI(S_ELIST);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ForInit(NIL_NODELIST); DPTR((yyval.node))->SetLine((yyvsp[-1].line));
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 365:

    { PE(S_EXPR); ;}
    break;

  case 366:

    { (yyval.node) = (yyvsp[-2].node); DPTR((yyval.node))->SetLine((yyvsp[-1].line)); SET_LOCATION((yyloc), (yylsp[-2]), (yylsp[0])); ;}
    break;

  case 367:

    { PE(S_ELIST); ;}
    break;

  case 368:

    {
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ForSuffix((yyvsp[-1].nodeList)); DPTR((yyval.node))->SetLine((yyvsp[-2].line));
								SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); SET_LOCATION((yyloc), (yylsp[-1]), (yylsp[0]));
							;}
    break;

  case 369:

    {
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ForSuffix(NIL_NODELIST); DPTR((yyval.node))->SetLine((yyvsp[-1].line));
								SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0]));
							;}
    break;

  case 370:

    { PE(T_RPAR); ;}
    break;

  case 371:

    { SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 372:

    { (yyval.node) = (yyvsp[0].node); DPTR((yyval.node))->SetLine((yyvsp[-1].line)); SET_LOCATION((yyloc), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 373:

    {	PE(S_LVAL);		;}
    break;

  case 374:

    {	PE(T_COMMA);	;}
    break;

  case 375:

    {	(yyval.node) = (yyvsp[-2].node);		;}
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

    {	if ((yyvsp[-7].node))
												(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ForeachStmt((yyvsp[-7].node), (yyvsp[-9].node), (yyvsp[-5].node), (yyvsp[0].node)); 
											else
												(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ForeachStmt((yyvsp[-9].node), NIL_NODE, (yyvsp[-5].node), (yyvsp[0].node)); 
											DPTR((yyval.node))->SetLine((yyvsp[-1].line));
											SET_LOCATION((yyval.node), (yylsp[-13]), (yylsp[0]));
											EM(S_FOREACH);	;}
    break;

  case 384:

    { SM(S_THROW); PE2(T_THROW, S_EXPR); ;}
    break;

  case 385:

    { EM(S_THROW); (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_THROW((yyvsp[-1].node)); SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0])); ;}
    break;

  case 386:

    { PE(S_STMT); (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ExceptionVar((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
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
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Exception((yyvsp[-5].node), (yyvsp[-1].node), (yyvsp[0].node));
							SET_LOCATION((yyval.node), (yylsp[-7]), (yylsp[0]));
						;}
    break;

  case 391:

    { (yyval.count) = (yyvsp[-1].count) + 1; ;}
    break;

  case 392:

    { (yyval.count) = 1; ;}
    break;

  case 393:

    {
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_ESCAPE, (yyvsp[-1].node));
							(yyval.node)->SetAttribute(AST_ATTRIBUTE_CARDINALITY, (yyvsp[-4].count));
							(yyval.node)->SetLine((yyvsp[-2].line));
							SET_LOCATION((yyval.node), (yylsp[-4]), (yylsp[0]));
						;}
    break;

  case 394:

    {
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Name((yyvsp[0].constStr));				SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Lvalue((yyval.node));			SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_PrimaryExpression((yyval.node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0]));
							
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_ESCAPE, (yyval.node));
							(yyval.node)->SetAttribute(AST_ATTRIBUTE_CARDINALITY, (yyvsp[-2].count));
							(yyval.node)->SetLine((yyvsp[-1].line));
							SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0]));
						;}
    break;

  case 395:

    {
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_INLINE, (yyvsp[-1].node));
							(yyval.node)->SetLine((yyvsp[-2].line));
							SET_LOCATION((yyval.node), (yylsp[-4]), (yylsp[0]));
						;}
    break;

  case 396:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-2].nodeList), (yyvsp[0].node)); ;}
    break;

  case 397:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, (yyvsp[0].node)); ;}
    break;

  case 398:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-2].nodeList), (yyvsp[0].node)); ;}
    break;

  case 399:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, (yyvsp[0].node)); ;}
    break;

  case 400:

    { SG(S_ELIST); ;}
    break;

  case 401:

    { EG(S_ELIST); PE2(S_ELIST, T_RBC); ;}
    break;

  case 402:

    {
							Node* indices = ASTCREATOR_BY_CONTEXT.MakeNode_TableIndices((yyvsp[-5].nodeList)); SET_LOCATION(indices, (yylsp[-5]), (yylsp[-5]));
							Node* values = ASTCREATOR_BY_CONTEXT.MakeNode_TableValues((yyvsp[-2].nodeList)); SET_LOCATION(values, (yylsp[-2]), (yylsp[-2]));
							(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_IndexedValues(indices, values); SET_LOCATION((yyval.node), (yylsp[-6]), (yylsp[0]));
						;}
    break;

  case 403:

    { (yyval.node) = (yyvsp[-1].node); ;}
    break;

  case 404:

    { (yyval.node) = (yyvsp[-1].node); PE(T_COLON); ;}
    break;

  case 405:

    { SG(S_EXPR); ;}
    break;

  case 406:

    {
								EG(S_EXPR); PE(S_EXPR);
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_IdentIndexElement((yyvsp[-3].node), (yyvsp[-1].node));
								DPTR((yyval.node))->SetLine((yyvsp[0].line));
								SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0]));
								SET_LOCATION((yyloc), (yylsp[-3]), (yylsp[-1]));
							;}
    break;

  case 407:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 408:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 409:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 410:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 411:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_DottedIdent((yyvsp[0].constStr)); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 412:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 413:

    { PE2(T_BAR, S_EXPR); ;}
    break;

  case 414:

    { PE(T_BAR); ;}
    break;

  case 415:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_LateBoundArgument((yyvsp[-2].node)); SET_LOCATION((yyval.node), (yylsp[-4]), (yylsp[0])); ;}
    break;

  case 416:

    { PE(T_TDOT); (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_TRIPLE_DOT(); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 417:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-2].nodeList), (yyvsp[0].node)); ;}
    break;

  case 418:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-2].nodeList), (yyvsp[0].node)); ;}
    break;

  case 419:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List((yyvsp[-2].nodeList), (yyvsp[0].node)); ;}
    break;

  case 420:

    { (yyval.nodeList) = ASTCREATOR_BY_CONTEXT.MakeNode_List(NIL_NODELIST, (yyvsp[0].node)); ;}
    break;

  case 421:

    { (yyval.nodeList) = (yyvsp[0].nodeList); ;}
    break;

  case 422:

    { (yyval.nodeList) = (yyvsp[0].nodeList); ;}
    break;

  case 423:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt((yyvsp[-1].node)); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 424:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 425:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 426:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_EmptyStmt();	  SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 427:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 428:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 429:

    {
						Node* elist = ASTCREATOR_BY_CONTEXT.MakeNode_ExpressionListStmt((yyvsp[-1].nodeList));
						SET_LOCATION(elist, (yylsp[-1]), (yylsp[-1]));
						(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt(elist);
						SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
					;}
    break;

  case 430:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 431:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionStmt((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 432:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 433:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts((yyvsp[-1].node), (yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 434:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts(NIL_NODE, (yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 435:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 436:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_FunctionStmt((yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 437:

    {
								Node* elist = ASTCREATOR_BY_CONTEXT.MakeNode_ExpressionListStmt((yyvsp[-1].nodeList));
								SET_LOCATION(elist, (yylsp[-1]), (yylsp[-1]));
								(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_BasicStmt(elist);
								SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
							;}
    break;

  case 438:

    { PE(T_RBC); ;}
    break;

  case 439:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Compound((yyvsp[-2].node)); SET_LOCATION((yyval.node), (yylsp[-3]), (yylsp[0])); ;}
    break;

  case 440:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Compound(); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 441:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 442:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts((yyvsp[-1].node), (yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;

  case 443:

    {
						(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts(NIL_NODE, (yyvsp[-1].node));
						(yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts((yyval.node), (yyvsp[0].node));
						SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0]));
					;}
    break;

  case 444:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_FromList(AST_TAG_QUOTED_ELEMENTS, (yyvsp[0].nodeList)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 445:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_Stmts(NIL_NODE, (yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[0]), (yylsp[0])); ;}
    break;

  case 446:

    { (yyval.node) = (yyvsp[0].node); ;}
    break;

  case 447:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_QUASI_QUOTES, (yyvsp[-1].node));	SET_LOCATION((yyval.node), (yylsp[-2]), (yylsp[0])); ;}
    break;

  case 448:

    { (yyval.node) = ASTCREATOR_BY_CONTEXT.MakeNode_ChildExpr(AST_TAG_EXECUTE, (yyvsp[0].node)); SET_LOCATION((yyval.node), (yylsp[-1]), (yylsp[0])); ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */


  yyvsp -= yylen;
  yyssp -= yylen;
  yylsp -= yylen;

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
	      yyerror (&yylloc, lexer, yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (&yylloc, lexer, YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (&yylloc, lexer, YY_("syntax error"));
    }

  yyerror_range[0] = yylloc;

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
	  yydestruct ("Error: discarding", yytoken, &yylval, &yylloc);
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

  yyerror_range[0] = yylsp[1-yylen];
  yylsp -= yylen;
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping", yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though. */
  YYLLOC_DEFAULT (yyloc, yyerror_range - 1, 2);
  *++yylsp = yyloc;

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
  yyerror (&yylloc, lexer, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}




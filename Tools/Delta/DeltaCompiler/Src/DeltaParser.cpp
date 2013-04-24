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
#define yyparse DeltaCompiler_yyparse
#define yylex   DeltaCompiler_yylex
#define yyerror DeltaCompiler_yyerror
#define yylval  DeltaCompiler_yylval
#define yychar  DeltaCompiler_yychar
#define yydebug DeltaCompiler_yydebug
#define yynerrs DeltaCompiler_yynerrs
#define yylloc DeltaCompiler_yylloc

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT = 258,
     ATTRIBUTE_IDENT = 259,
     NUMBER_CONST = 260,
     STRING_CONST = 261,
     FUNCTION = 262,
     RETURN = 263,
     ONEVENT = 264,
     IF = 265,
     ELSE = 266,
     WHILE = 267,
     FOR = 268,
     FOREACH = 269,
     NIL = 270,
     LOCAL = 271,
     AND = 272,
     NOT = 273,
     OR = 274,
     LAMBDA = 275,
     TRY = 276,
     TRAP = 277,
     THROW = 278,
     USING = 279,
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
     ASSIGN = 290,
     ADD_A = 291,
     SUB_A = 292,
     MUL_A = 293,
     DIV_A = 294,
     MOD_A = 295,
     DOUBLE_LB = 296,
     DOUBLE_RB = 297,
     SEMI = 298,
     UMINUS = 299,
     MINUSMINUS = 300,
     LT = 301,
     GT = 302,
     LE = 303,
     GE = 304,
     EQ = 305,
     NE = 306,
     DOT = 307,
     DOUBLE_DOT = 308,
     TRIPLE_DOT = 309,
     DOT_ASSIGN = 310,
     GLOBAL_SCOPE = 311,
     ADD = 312,
     SUB = 313,
     MUL = 314,
     DIV = 315,
     MOD = 316,
     ATTRIBUTE = 317,
     STRINGIFY = 318,
     PLUSPLUS = 319,
     PARENTHESIS = 320,
     SQUARE_BRACKETS = 321,
     CALL = 322,
     CAST = 323,
     DOT_EQUAL = 324,
     DOT_CAST = 325,
     DOT_EQUAL_RHS = 326,
     ADD_POSTFIX = 327,
     SUB_POSTFIX = 328,
     MUL_POSTFIX = 329,
     DIV_POSTFIX = 330,
     MOD_POSTFIX = 331,
     GT_POSTFIX = 332,
     LT_POSTFIX = 333,
     NE_POSTFIX = 334,
     EQ_POSTFIX = 335,
     GE_POSTFIX = 336,
     LE_POSTFIX = 337,
     OPERATOR = 338,
     NEWSELF = 339,
     SET = 340,
     GET = 341,
     LAMBDA_REF = 342
   };
#endif
/* Tokens.  */
#define IDENT 258
#define ATTRIBUTE_IDENT 259
#define NUMBER_CONST 260
#define STRING_CONST 261
#define FUNCTION 262
#define RETURN 263
#define ONEVENT 264
#define IF 265
#define ELSE 266
#define WHILE 267
#define FOR 268
#define FOREACH 269
#define NIL 270
#define LOCAL 271
#define AND 272
#define NOT 273
#define OR 274
#define LAMBDA 275
#define TRY 276
#define TRAP 277
#define THROW 278
#define USING 279
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
#define ASSIGN 290
#define ADD_A 291
#define SUB_A 292
#define MUL_A 293
#define DIV_A 294
#define MOD_A 295
#define DOUBLE_LB 296
#define DOUBLE_RB 297
#define SEMI 298
#define UMINUS 299
#define MINUSMINUS 300
#define LT 301
#define GT 302
#define LE 303
#define GE 304
#define EQ 305
#define NE 306
#define DOT 307
#define DOUBLE_DOT 308
#define TRIPLE_DOT 309
#define DOT_ASSIGN 310
#define GLOBAL_SCOPE 311
#define ADD 312
#define SUB 313
#define MUL 314
#define DIV 315
#define MOD 316
#define ATTRIBUTE 317
#define STRINGIFY 318
#define PLUSPLUS 319
#define PARENTHESIS 320
#define SQUARE_BRACKETS 321
#define CALL 322
#define CAST 323
#define DOT_EQUAL 324
#define DOT_CAST 325
#define DOT_EQUAL_RHS 326
#define ADD_POSTFIX 327
#define SUB_POSTFIX 328
#define MUL_POSTFIX 329
#define DIV_POSTFIX 330
#define MOD_POSTFIX 331
#define GT_POSTFIX 332
#define LT_POSTFIX 333
#define NE_POSTFIX 334
#define EQ_POSTFIX 335
#define GE_POSTFIX 336
#define LE_POSTFIX 337
#define OPERATOR 338
#define NEWSELF 339
#define SET 340
#define GET 341
#define LAMBDA_REF 342




/* Copy the first part of user declarations.  */



// Delta parser, syntax directed parsing file, using Bison parser generator.
// Technical notice: I use a static char* buffer for IDENT, so
// if IDENT is to be taken its char* value when being on the left or
// intermediate part of a rule that has at right another non-terminal
// that includes IDENT too, the last use of IDENT would have overwritten
// all previous, so those have to be saved with a dynamic copy at the
// time the IDENT rule is engaged (see ConstId for an example). To avoid
// memory leaks in case of syntax errors for such temp string copies, 
// late string destruction is adopted.
// ScriptFighter Project.
// A. Savidis, Start October 1999 (original full version).
// Revised January 2005, changing self, putting agruments and
// generalizing table content for any expression type.
// Revised May 2006 for better source line calculation of quads.
// Extended July 2007, adding support for precise syntax error messages.
// Extended September 2008, added support for unparsed forms and 
// selective step-in in case of multiple calls in a single stmt.
// Extended January 2009, added support for object attributes (properties).
// Extended July 2009, added foreach stmt.
// Extended December 2009, added lambda functions.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "DDebug.h"
#include "Symbol.h"
#include "Expr.h"
#include "ParseActions.h"
#include "ParseTableConstruction.h"
#include "Unparsable.h"
#include "DescriptiveParseErrorHandler.h"
#include "LibraryNamespace.h"
#include "CompilerStringHolder.h"

#include "DeltaLexAnalyser.h"
#include "DeltaParser.h"

#define yyFlexLexer DeltaLexAnalyserFlexLexer

// Bison assumes alloca is the memory allocation
// function.
//
#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

extern int DeltaCompiler_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer);

///////////////////////////////////////////////////////////
// Helper macros to get specific components from the lexer
// component directory.

#define DIRECTORY						(lexer.GetDirectory())

#define GET_DESCRIPTIVE_ERROR_HANDLER	DESCRIPTIVE_ERROR_HANDLER_EX(DIRECTORY)
#define GET_PARSEPARMS					PARSEPARMS_EX(DIRECTORY)
#define GET_STRINGHOLDER				STRINGHOLDER_EX(DIRECTORY)
#define GET_DELTASYMBOLS				DELTASYMBOLS_EX(DIRECTORY)
#define GET_TRANSLATOR					TRANSLATOR_EX(DIRECTORY)
#define GET_QUADS						QUADS_EX(DIRECTORY)
#define GET_STMTFACTORY					STMTFACTORY_EX(DIRECTORY)

// Need to undef since its original version uses
// the curr component directory.

#undef	NEW_STMT
#define	NEW_STMT							GET_STMTFACTORY.New()

///////////////////////////////////////////////////////////

static void DeltaCompiler_yyerror (YYLTYPE* yylloc, yyFlexLexer& lexer, const char* unused)
	{ GET_DESCRIPTIVE_ERROR_HANDLER.HandleSyntaxError(); }



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
	const char*				id;
	DeltaNumberValueType	numberConst;
	char*					strConst;
	int						intVal;
	DeltaExpr*				expr;
	DeltaSymbol*			sym;
	TableElements*			table;
	DeltaQuadAddress		quadNo;
	Stmt*					stmt;
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
#define YYFINAL  22
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1087

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  98
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  216
/* YYNRULES -- Number of rules. */
#define YYNRULES  392
/* YYNRULES -- Number of states. */
#define YYNSTATES  576

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   342

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      65,    66,     2,     2,    73,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    72,     2,
       2,     2,     2,    71,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    67,     2,    68,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    69,    97,    70,     2,     2,     2,     2,
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
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     6,     9,    12,    14,    16,    17,
      20,    22,    23,    30,    32,    34,    36,    38,    39,    40,
      45,    47,    50,    52,    53,    55,    57,    59,    60,    65,
      67,    68,    70,    71,    75,    77,    78,    83,    84,    85,
      86,    92,    93,    97,    98,   104,   106,   107,   114,   116,
     117,   118,   119,   122,   123,   127,   131,   135,   137,   139,
     141,   143,   145,   147,   149,   151,   153,   155,   157,   159,
     161,   163,   164,   168,   169,   173,   176,   177,   182,   185,
     188,   190,   192,   196,   199,   204,   206,   208,   210,   212,
     214,   216,   218,   220,   221,   222,   223,   231,   232,   237,
     238,   243,   244,   249,   250,   255,   256,   261,   262,   267,
     268,   273,   274,   279,   280,   285,   286,   291,   292,   297,
     298,   303,   304,   310,   311,   317,   318,   323,   324,   329,
     330,   335,   336,   341,   342,   347,   357,   360,   362,   363,
     366,   369,   370,   374,   377,   378,   382,   383,   387,   388,
     392,   394,   396,   398,   400,   402,   404,   406,   408,   410,
     412,   414,   416,   418,   420,   422,   424,   426,   428,   430,
     432,   434,   436,   438,   440,   442,   444,   446,   448,   450,
     452,   454,   456,   458,   460,   462,   464,   466,   468,   470,
     472,   474,   476,   478,   480,   482,   484,   486,   488,   490,
     492,   494,   496,   498,   500,   502,   504,   506,   508,   510,
     512,   514,   517,   519,   521,   523,   525,   528,   529,   534,
     538,   540,   542,   544,   546,   548,   550,   552,   554,   556,
     558,   559,   564,   566,   567,   572,   573,   575,   576,   581,
     583,   584,   585,   591,   593,   594,   597,   598,   599,   605,
     607,   609,   612,   618,   622,   624,   627,   631,   633,   636,
     638,   639,   643,   644,   648,   650,   652,   654,   657,   660,
     663,   666,   668,   670,   672,   674,   676,   678,   680,   682,
     684,   687,   690,   693,   694,   699,   700,   705,   706,   711,
     712,   717,   719,   720,   723,   727,   730,   731,   736,   738,
     740,   743,   746,   748,   751,   754,   757,   760,   761,   765,
     767,   769,   771,   773,   774,   777,   778,   785,   789,   790,
     791,   792,   793,   803,   805,   806,   810,   811,   818,   824,
     828,   830,   832,   834,   836,   838,   839,   843,   847,   848,
     849,   857,   858,   859,   867,   868,   872,   873,   877,   878,
     879,   886,   887,   889,   896,   898,   900,   902,   907,   916,
     925,   926,   930,   935,   938,   940,   941,   947,   948,   952,
     954,   955,   959,   963,   964,   965,   971,   973,   974,   975,
     976,   977,   990,   991,   996,   997,  1002,  1004,  1006,  1008,
    1009,  1010,  1011
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
      99,     0,    -1,    -1,   102,    -1,   100,   101,    -1,   100,
     143,    -1,   143,    -1,   102,    -1,    -1,   102,   133,    -1,
     133,    -1,    -1,   128,   107,   115,   104,   117,   127,    -1,
       7,    -1,     9,    -1,    32,    -1,    16,    -1,    -1,    -1,
     105,   108,   106,   109,    -1,     3,    -1,    92,   110,    -1,
      62,    -1,    -1,   188,    -1,    52,    -1,    55,    -1,    -1,
     111,    73,   112,     3,    -1,     3,    -1,    -1,    66,    -1,
      -1,    54,   114,    66,    -1,    76,    -1,    -1,    65,   111,
     116,   113,    -1,    -1,    -1,    -1,    69,   118,   102,   119,
      70,    -1,    -1,    69,   120,    70,    -1,    -1,   128,   123,
     115,   122,   124,    -1,    20,    -1,    -1,    69,   125,   127,
     128,   126,    70,    -1,   147,    -1,    -1,    -1,    -1,   130,
      43,    -1,    -1,   132,   211,   129,    -1,   127,   128,   134,
      -1,   127,   128,   135,    -1,   117,    -1,   103,    -1,    43,
      -1,   131,    -1,   138,    -1,   141,    -1,   278,    -1,   283,
      -1,   303,    -1,   282,    -1,   269,    -1,   271,    -1,   311,
      -1,   305,    -1,    -1,    25,   137,   147,    -1,    -1,   139,
     136,   129,    -1,     8,   127,    -1,    -1,   140,   142,   147,
     129,    -1,   140,   129,    -1,    24,   144,    -1,   145,    -1,
     146,    -1,   212,     3,    43,    -1,     3,    43,    -1,    63,
       3,    43,   127,    -1,   153,    -1,   149,    -1,   160,    -1,
     167,    -1,   170,    -1,   176,    -1,   181,    -1,     3,    -1,
      -1,    -1,    -1,    31,   150,   148,   151,    35,   152,   147,
      -1,    -1,   213,    35,   154,   147,    -1,    -1,   213,    36,
     155,   147,    -1,    -1,   213,    38,   156,   147,    -1,    -1,
     213,    37,   157,   147,    -1,    -1,   213,    39,   158,   147,
      -1,    -1,   213,    40,   159,   147,    -1,    -1,   147,    47,
     161,   147,    -1,    -1,   147,    46,   162,   147,    -1,    -1,
     147,    49,   163,   147,    -1,    -1,   147,    48,   164,   147,
      -1,    -1,   147,    50,   165,   147,    -1,    -1,   147,    51,
     166,   147,    -1,    -1,   147,    17,   168,   128,   147,    -1,
      -1,   147,    19,   169,   128,   147,    -1,    -1,   147,    57,
     171,   147,    -1,    -1,   147,    58,   172,   147,    -1,    -1,
     147,    60,   173,   147,    -1,    -1,   147,    59,   174,   147,
      -1,    -1,   147,    61,   175,   147,    -1,    65,   177,   128,
     178,    72,   276,   128,   179,    66,    -1,   147,    71,    -1,
     147,    -1,    -1,   180,   147,    -1,   213,    64,    -1,    -1,
      64,   182,   213,    -1,   213,    45,    -1,    -1,    45,   183,
     213,    -1,    -1,    58,   184,   147,    -1,    -1,    18,   185,
     147,    -1,   186,    -1,   197,    -1,   187,    -1,   121,    -1,
       5,    -1,    15,    -1,    26,    -1,    27,    -1,    57,    -1,
      58,    -1,    59,    -1,    60,    -1,    61,    -1,    47,    -1,
      46,    -1,    51,    -1,    50,    -1,    49,    -1,    48,    -1,
      35,    -1,    81,    -1,    82,    -1,    83,    -1,    84,    -1,
      85,    -1,    86,    -1,    87,    -1,    88,    -1,    89,    -1,
      90,    -1,    91,    -1,    76,    -1,    77,    -1,    10,    -1,
      11,    -1,     7,    -1,     8,    -1,     9,    -1,    12,    -1,
      13,    -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,
      18,    -1,    19,    -1,    20,    -1,    21,    -1,    22,    -1,
      23,    -1,    24,    -1,    25,    -1,    26,    -1,    27,    -1,
      28,    -1,    29,    -1,    30,    -1,    31,    -1,    32,    -1,
      33,    -1,    34,    -1,   190,     6,    -1,     6,    -1,    63,
      -1,     3,    -1,   189,    -1,   191,   192,    -1,    -1,   193,
      52,   194,   192,    -1,   191,   212,     3,    -1,   190,    -1,
     193,    -1,   195,    -1,    33,    -1,    96,    -1,    93,    -1,
     196,    -1,    34,    -1,   213,    -1,   233,    -1,    -1,    65,
     147,   198,    66,    -1,   210,    -1,    -1,    65,   103,   199,
      66,    -1,    -1,   201,    -1,    -1,   201,    73,   202,   206,
      -1,   206,    -1,    -1,    -1,    65,   204,   200,   205,    66,
      -1,    76,    -1,    -1,   207,   147,    -1,    -1,    -1,    97,
     208,   147,   209,    97,    -1,    54,    -1,   103,    -1,   217,
     203,    -1,   211,    73,   127,   128,   147,    -1,   127,   128,
     147,    -1,    56,    -1,     3,    56,    -1,   212,     3,    56,
      -1,     3,    -1,   212,     3,    -1,   260,    -1,    -1,    30,
     214,     3,    -1,    -1,    16,   215,     3,    -1,   225,    -1,
     197,    -1,   197,    -1,   216,    52,    -1,   216,    67,    -1,
     216,    53,    -1,   216,    41,    -1,    55,    -1,    78,    -1,
      79,    -1,    80,    -1,     3,    -1,   189,    -1,   188,    -1,
     196,    -1,   222,    -1,   218,   223,    -1,   216,   224,    -1,
     220,   223,    -1,    -1,   219,   147,   226,    68,    -1,    -1,
     219,   110,   227,    68,    -1,    -1,   221,   147,   228,    42,
      -1,    -1,   221,   110,   229,    42,    -1,    67,    -1,    -1,
     232,    68,    -1,   230,   234,   231,    -1,   230,   231,    -1,
      -1,   234,    73,   235,   239,    -1,   239,    -1,   103,    -1,
     147,   127,    -1,   236,   127,    -1,   128,    -1,   238,   237,
      -1,   238,   248,    -1,   238,   262,    -1,   238,   255,    -1,
      -1,    52,   241,   192,    -1,   188,    -1,   147,    -1,   240,
      -1,   242,    -1,    -1,   245,   243,    -1,    -1,   246,   247,
      73,   128,   244,   127,    -1,   128,   244,   127,    -1,    -1,
      -1,    -1,    -1,    69,   249,   246,   250,    72,   251,   257,
     252,    70,    -1,   260,    -1,    -1,    52,   254,   259,    -1,
      -1,   253,    72,   256,   128,   258,   127,    -1,   257,    73,
     127,   128,   258,    -1,   127,   128,   258,    -1,   147,    -1,
     103,    -1,     3,    -1,   189,    -1,     4,    -1,    -1,    62,
     261,   259,    -1,   260,   263,   266,    -1,    -1,    -1,    69,
     264,    94,   265,   128,   258,   127,    -1,    -1,    -1,    95,
     267,   128,   258,   127,   268,    70,    -1,    -1,   270,    28,
     129,    -1,    -1,   272,    29,   129,    -1,    -1,    -1,    65,
     274,   127,   147,   275,    66,    -1,    -1,    12,    -1,   277,
     128,   273,   127,   128,   133,    -1,    10,    -1,    11,    -1,
     128,    -1,   279,   273,   281,   133,    -1,   279,   273,   281,
     133,   280,   276,   128,   133,    -1,   286,   128,   288,   128,
     294,   276,   128,   133,    -1,    -1,    13,   285,    65,    -1,
     284,   128,   127,   287,    -1,   211,   129,    -1,   129,    -1,
      -1,   289,   128,   147,   127,   129,    -1,    -1,   291,   211,
     292,    -1,   292,    -1,    -1,   293,   127,    66,    -1,   127,
     128,   290,    -1,    -1,    -1,    72,   296,   213,   297,    73,
      -1,    73,    -1,    -1,    -1,    -1,    -1,    14,   299,    65,
     300,   213,   301,   295,   302,   127,   128,   147,    66,    -1,
      -1,   298,   128,   304,   133,    -1,    -1,    23,   306,   147,
     129,    -1,    21,    -1,    22,    -1,     3,    -1,    -1,    -1,
      -1,   307,   133,   312,   308,   310,   309,   313,   133,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   159,   159,   160,   161,   164,   165,   168,   169,   172,
     177,   185,   184,   195,   197,   199,   203,   204,   208,   207,
     213,   215,   217,   220,   223,   224,   225,   229,   228,   232,
     234,   238,   241,   240,   246,   249,   248,   251,   255,   257,
     254,   262,   261,   268,   267,   279,   285,   284,   290,   297,
     298,   299,   299,   301,   301,   306,   307,   308,   309,   310,
     313,   314,   315,   318,   319,   320,   321,   322,   323,   324,
     325,   331,   330,   340,   340,   347,   350,   349,   355,   363,
     366,   367,   370,   372,   376,   383,   384,   385,   386,   387,
     388,   389,   392,   396,   397,   398,   396,   404,   403,   411,
     410,   418,   417,   425,   424,   432,   431,   439,   438,   447,
     446,   453,   452,   459,   458,   465,   464,   471,   470,   477,
     476,   484,   483,   490,   489,   497,   496,   503,   502,   509,
     508,   515,   514,   521,   520,   527,   532,   537,   541,   541,
     546,   551,   550,   557,   562,   561,   569,   568,   574,   573,
     578,   582,   583,   584,   587,   588,   589,   590,   593,   594,
     595,   596,   597,   598,   599,   600,   601,   602,   603,   604,
     605,   606,   607,   608,   609,   610,   611,   612,   613,   614,
     615,   616,   617,   620,   621,   622,   623,   624,   625,   626,
     627,   628,   629,   630,   631,   632,   633,   634,   635,   636,
     637,   638,   639,   640,   641,   642,   643,   644,   645,   646,
     647,   650,   652,   656,   660,   661,   664,   667,   666,   672,
     676,   678,   680,   688,   690,   692,   694,   696,   698,   700,
     704,   703,   708,   712,   711,   721,   722,   728,   727,   731,
     736,   738,   735,   741,   745,   745,   749,   751,   748,   756,
     758,   764,   770,   773,   782,   785,   788,   793,   797,   801,
     805,   804,   810,   809,   814,   818,   822,   826,   830,   834,
     838,   842,   843,   844,   845,   848,   850,   852,   854,   858,
     864,   869,   874,   880,   879,   886,   885,   892,   891,   898,
     897,   907,   912,   912,   917,   923,   930,   930,   934,   938,
     948,   950,   954,   957,   959,   961,   963,   968,   967,   973,
     979,   980,   981,   984,   984,   990,   989,   994,  1000,  1002,
    1004,  1006,   999,  1011,  1014,  1013,  1020,  1019,  1025,  1028,
    1033,  1034,  1040,  1041,  1044,  1048,  1047,  1053,  1058,  1060,
    1057,  1068,  1070,  1067,  1079,  1079,  1080,  1080,  1082,  1083,
    1082,  1087,  1092,  1096,  1105,  1106,  1107,  1109,  1113,  1124,
    1132,  1131,  1137,  1141,  1143,  1147,  1147,  1154,  1154,  1160,
    1164,  1164,  1168,  1174,  1175,  1174,  1177,  1180,  1181,  1182,
    1183,  1180,  1193,  1193,  1204,  1203,  1211,  1212,  1213,  1214,
    1217,  1219,  1216
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT", "ATTRIBUTE_IDENT",
  "NUMBER_CONST", "STRING_CONST", "FUNCTION", "RETURN", "ONEVENT", "IF",
  "ELSE", "WHILE", "FOR", "FOREACH", "NIL", "LOCAL", "AND", "NOT", "OR",
  "LAMBDA", "TRY", "TRAP", "THROW", "USING", "ASSERT", "TRUE", "FALSE",
  "BREAK", "CONTINUE", "STATIC", "CONST", "METHOD", "SELF", "ARGUMENTS",
  "ASSIGN", "ADD_A", "SUB_A", "MUL_A", "DIV_A", "MOD_A", "DOUBLE_LB",
  "DOUBLE_RB", "SEMI", "UMINUS", "MINUSMINUS", "LT", "GT", "LE", "GE",
  "EQ", "NE", "DOT", "DOUBLE_DOT", "TRIPLE_DOT", "DOT_ASSIGN",
  "GLOBAL_SCOPE", "ADD", "SUB", "MUL", "DIV", "MOD", "ATTRIBUTE",
  "STRINGIFY", "PLUSPLUS", "'('", "')'", "'['", "']'", "'{'", "'}'", "'?'",
  "':'", "','", "PARENTHESIS", "SQUARE_BRACKETS", "CALL", "CAST",
  "DOT_EQUAL", "DOT_CAST", "DOT_EQUAL_RHS", "ADD_POSTFIX", "SUB_POSTFIX",
  "MUL_POSTFIX", "DIV_POSTFIX", "MOD_POSTFIX", "GT_POSTFIX", "LT_POSTFIX",
  "NE_POSTFIX", "EQ_POSTFIX", "GE_POSTFIX", "LE_POSTFIX", "OPERATOR",
  "NEWSELF", "SET", "GET", "LAMBDA_REF", "'|'", "$accept", "DeltaCode",
  "UsingDirectives", "OptionalStmts", "Stmts", "Function", "@1",
  "FuncClass", "FuncLinkage", "FunctionPrefix", "@2", "FunctionName",
  "OperatorMethod", "IdentList", "@3", "FormalArgsSuffix", "@4",
  "FormalArgs", "@5", "Compound", "@6", "@7", "@8", "LambdaFunction", "@9",
  "LambdaPrefix", "LambdaCode", "@10", "LambdaStmt", "LN", "M", "Semi",
  "@11", "ExpressionStmt", "@12", "Stmt", "BasicExprStmt",
  "BasicNonExprStmt", "Assertion", "@13", "AssertStmt", "@14",
  "ReturnPrefix", "ReturnStmt", "@15", "UsingDirective",
  "UsingSpecifications", "UsingNamespace", "UsingByteCodeLibrary",
  "Expression", "ConstId", "ConstAssignExpression", "@16", "@17", "@18",
  "AssignExpression", "@19", "@20", "@21", "@22", "@23", "@24",
  "RelationalExpression", "@25", "@26", "@27", "@28", "@29", "@30",
  "BooleanExpression", "@31", "@32", "ArithmeticExpression", "@33", "@34",
  "@35", "@36", "@37", "TernaryExpression", "TernaryCondition",
  "TernarySelection1", "TernarySelection2", "@38", "Term", "@39", "@40",
  "@41", "@42", "Primary", "ConstValue", "OpString", "KwdIdent",
  "StringConst", "Stringify", "StringIdent", "StringifyDottedIdents",
  "@43", "StringifyNamespaceIdent", "StringConstUsed",
  "FunctionAndTableObject", "@44", "@45", "ActualArgumentsList",
  "NonEmptyActualArgumentsList", "@46", "ActualArguments", "@47", "@48",
  "ActualArgument", "@49", "@50", "@51", "FunctionCall", "ExpressionList",
  "NamespacePath", "Lvalue", "@52", "@53", "TableObject",
  "FunctionCallObject", "TableContentDot", "TableContentBracket",
  "TableContentBoundedDot", "TableContentBoundedBracket", "DottedOpString",
  "DotIndex", "SpecialDotIndex", "TableContent", "@54", "@55", "@56",
  "@57", "TablePrefix", "TableSuffix", "@58", "TableConstructor",
  "TableElements", "@59", "FunctionElement", "UnindexedValue", "M_elem",
  "TableElement", "DottedIdent", "@60", "OperatorIndex", "IndexContent",
  "IndexExpression", "@61", "IndexedList", "@62", "IndexedValues", "@63",
  "@64", "@65", "@66", "IdentIndex", "@67", "IdentIndexElement", "@68",
  "ContentList", "ContentExpression", "AttributeIdent", "AttributeId",
  "@69", "NewAttribute", "AttributeSet", "@70", "@71", "AttributeGet",
  "@72", "@73", "BreakStmt", "@74", "ContinueStmt", "@75", "Condition",
  "@76", "@77", "N", "WhilePrefix", "WhileStmt", "IfPrefix", "ElsePrefix",
  "M_If", "IfStmt", "ForStmt", "ForOpening", "@78", "ForPrefix",
  "ForInitList", "ForCondition", "@79", "ForRepeatList", "@80", "ForEnd",
  "@81", "ForSuffix", "ForeachValue", "@82", "@83", "ForeachPrefix", "@84",
  "@85", "@86", "@87", "ForeachStmt", "@88", "ThrowStmt", "@89",
  "TryHeader", "TrapHeader", "ExceptionVar", "TrapJumpOver",
  "ExceptionStmt", "@90", "@91", 0
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
     315,   316,   317,   318,   319,    40,    41,    91,    93,   123,
     125,    63,    58,    44,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short int yyr1[] =
{
       0,    98,    99,    99,    99,   100,   100,   101,   101,   102,
     102,   104,   103,   105,   105,   105,   106,   106,   108,   107,
     109,   109,   109,   109,   110,   110,   110,   112,   111,   111,
     111,   113,   114,   113,   115,   116,   115,   115,   118,   119,
     117,   120,   117,   122,   121,   123,   125,   124,   126,   127,
     128,   130,   129,   132,   131,   133,   133,   133,   133,   133,
     134,   134,   134,   135,   135,   135,   135,   135,   135,   135,
     135,   137,   136,   139,   138,   140,   142,   141,   141,   143,
     144,   144,   145,   145,   146,   147,   147,   147,   147,   147,
     147,   147,   148,   150,   151,   152,   149,   154,   153,   155,
     153,   156,   153,   157,   153,   158,   153,   159,   153,   161,
     160,   162,   160,   163,   160,   164,   160,   165,   160,   166,
     160,   168,   167,   169,   167,   171,   170,   172,   170,   173,
     170,   174,   170,   175,   170,   176,   177,   178,   180,   179,
     181,   182,   181,   181,   183,   181,   184,   181,   185,   181,
     181,   186,   186,   186,   187,   187,   187,   187,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   190,   190,   191,   192,   192,   193,   194,   193,   195,
     196,   196,   196,   197,   197,   197,   197,   197,   197,   197,
     198,   197,   197,   199,   197,   200,   200,   202,   201,   201,
     204,   205,   203,   203,   207,   206,   208,   209,   206,   206,
     206,   210,   211,   211,   212,   212,   212,   213,   213,   213,
     214,   213,   215,   213,   213,   216,   217,   218,   219,   220,
     221,   222,   222,   222,   222,   223,   223,   223,   223,   224,
     225,   225,   225,   226,   225,   227,   225,   228,   225,   229,
     225,   230,   232,   231,   233,   233,   235,   234,   234,   236,
     237,   237,   238,   239,   239,   239,   239,   241,   240,   242,
     243,   243,   243,   245,   244,   247,   246,   246,   249,   250,
     251,   252,   248,   253,   254,   253,   256,   255,   257,   257,
     258,   258,   259,   259,   260,   261,   260,   262,   264,   265,
     263,   267,   268,   266,   270,   269,   272,   271,   274,   275,
     273,   276,   277,   278,   279,   280,   281,   282,   282,   283,
     285,   284,   286,   287,   287,   289,   288,   291,   290,   290,
     293,   292,   294,   296,   297,   295,   295,   299,   300,   301,
     302,   298,   304,   303,   306,   305,   307,   308,   309,   310,
     312,   313,   311
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     2,     2,     1,     1,     0,     2,
       1,     0,     6,     1,     1,     1,     1,     0,     0,     4,
       1,     2,     1,     0,     1,     1,     1,     0,     4,     1,
       0,     1,     0,     3,     1,     0,     4,     0,     0,     0,
       5,     0,     3,     0,     5,     1,     0,     6,     1,     0,
       0,     0,     2,     0,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     0,     3,     2,     0,     4,     2,     2,
       1,     1,     3,     2,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     0,     0,     7,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     5,     0,     5,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     9,     2,     1,     0,     2,
       2,     0,     3,     2,     0,     3,     0,     3,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     2,     0,     4,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     1,     0,     4,     0,     1,     0,     4,     1,
       0,     0,     5,     1,     0,     2,     0,     0,     5,     1,
       1,     2,     5,     3,     1,     2,     3,     1,     2,     1,
       0,     3,     0,     3,     1,     1,     1,     2,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     0,     4,     0,     4,     0,     4,     0,
       4,     1,     0,     2,     3,     2,     0,     4,     1,     1,
       2,     2,     1,     2,     2,     2,     2,     0,     3,     1,
       1,     1,     1,     0,     2,     0,     6,     3,     0,     0,
       0,     0,     9,     1,     0,     3,     0,     6,     5,     3,
       1,     1,     1,     1,     1,     0,     3,     3,     0,     0,
       7,     0,     0,     7,     0,     3,     0,     3,     0,     0,
       6,     0,     1,     6,     1,     1,     1,     4,     8,     8,
       0,     3,     4,     2,     1,     0,     5,     0,     3,     1,
       0,     3,     3,     0,     0,     5,     1,     0,     0,     0,
       0,    12,     0,     4,     0,     4,     1,     1,     1,     0,
       0,     0,     8
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
      49,     0,    59,    38,     0,    49,    49,    58,    57,    50,
       0,    10,     6,     0,   254,     0,    79,    80,    81,     0,
      49,     0,     1,     4,    49,     5,     9,    53,    13,    14,
      15,    18,    37,    83,   255,     0,     0,    49,    42,    49,
     354,   352,   360,   377,   386,   384,    60,    49,    55,    56,
      61,     0,    76,    62,    67,     0,    68,     0,    50,    63,
       0,    66,    64,    50,    50,    50,    65,    70,    49,    69,
      17,    30,    34,    11,    49,    82,   256,     0,    75,     0,
       0,    50,    50,    51,    71,    51,    78,     0,    50,    51,
      51,     0,   348,    50,    49,   365,   382,   390,    16,    23,
      29,    35,     0,    84,    40,   361,   378,   257,   334,   154,
     212,   155,   262,   148,   156,   157,   260,    93,   223,   227,
     144,   146,   335,   213,   141,    50,   291,   225,   224,   153,
       0,    51,    86,    85,    87,    88,    89,    90,    91,   150,
     152,   220,     0,   221,   222,   226,   151,   232,     0,   228,
       0,     0,     0,    50,     0,    50,   264,    50,   229,   259,
      50,    49,    54,    50,    74,    52,    51,   345,   347,    49,
      49,   356,    49,    49,    50,    50,    49,     0,    20,    22,
       0,    19,    27,     0,    49,     0,     0,    50,     0,     0,
       0,    50,     0,     0,   233,     0,   230,    50,    45,    37,
     121,   123,   111,   109,   115,   113,   117,   119,   125,   127,
     131,   129,   133,   385,   211,   214,   185,   186,   187,   183,
     184,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   215,   216,     0,   217,   258,    97,
      99,   103,   101,   105,   107,   143,   140,   270,   267,   269,
     271,   268,   272,   273,   274,   279,   281,   240,   243,   251,
     275,   169,   164,   163,   168,   167,   166,   165,   158,   159,
     160,   161,   162,   181,   182,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   277,   276,   278,   280,
      25,    26,   146,   285,   283,    24,   282,   289,   287,   302,
     295,     0,   292,    50,   298,   253,    50,    72,    77,    50,
      50,   357,   364,    51,   362,    49,    50,   383,   387,   389,
      21,     0,    32,    31,    36,    12,    50,   265,   228,   263,
     149,   261,    92,    94,   145,   147,   332,   333,   336,   142,
       0,   136,     0,    50,    43,    50,    50,    50,    50,    50,
      50,    50,    50,    50,    50,    50,    50,    50,   219,     0,
      50,    50,    50,    50,    50,    50,   244,     0,     0,     0,
       0,   293,   296,   294,   324,   318,   299,    49,    49,   303,
     304,     0,   306,   259,   305,    50,    49,   349,   355,   351,
     363,    50,   351,    49,     0,    28,     0,   230,     0,     0,
     234,   231,   137,     0,     0,    50,    50,   112,   110,   116,
     114,   118,   120,   126,   128,   132,   130,   134,   214,   218,
      98,   100,   104,   102,   106,   108,   249,   246,   250,   241,
     236,   239,    50,   286,   284,   290,   288,    50,     0,    50,
     300,   301,   326,   338,     0,   252,   353,     0,    50,   367,
      50,    51,   388,   391,    33,   373,   376,   380,    95,   351,
      46,    44,   122,   124,    50,     0,   237,   245,   297,   325,
     313,   315,    50,     0,   341,   337,   350,    49,   372,    49,
     369,    49,    49,   366,    49,     0,    49,    50,    50,    49,
     247,   242,   244,    49,    50,     0,     0,    50,   339,    50,
     358,   370,     0,   359,   392,   228,    50,    96,   138,    50,
       0,   238,   317,   307,   310,   309,   311,   312,   314,    50,
     320,   331,   330,    49,    50,    50,   368,   371,     0,    50,
       0,    50,    50,   248,     0,   313,    49,   327,    50,    49,
     375,     0,   135,   139,     0,    48,   308,    49,    50,   321,
      49,   342,   381,    47,   316,    50,    49,     0,   340,     0,
     329,    50,   322,   343,    50,   328
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     4,     5,    23,     6,     7,   102,    31,    99,    32,
      70,   181,   303,   101,   331,   334,   406,    73,   183,     8,
      20,    77,    21,   129,   414,   199,   471,   499,   554,     9,
     130,    86,    87,    46,    47,    11,    48,    49,    85,   163,
      50,    51,    52,    53,    88,    12,    16,    17,    18,   532,
     343,   132,   189,   409,   497,   133,   370,   371,   373,   372,
     374,   375,   134,   358,   357,   360,   359,   361,   362,   135,
     355,   356,   136,   363,   364,   366,   365,   367,   137,   197,
     413,   540,   541,   138,   193,   190,   191,   187,   139,   140,
     305,   244,   141,   142,   245,   143,   369,   144,   145,   146,
     352,   350,   439,   440,   502,   269,   376,   475,   441,   442,
     474,   520,   147,    83,   148,   149,   188,   186,   150,   151,
     152,   153,   154,   155,   265,   299,   266,   156,   378,   377,
     380,   379,   157,   310,   311,   158,   312,   447,   388,   389,
     313,   314,   526,   544,   527,   528,   503,   504,   481,   505,
     390,   449,   506,   546,   567,   391,   448,   392,   482,   559,
     533,   348,   159,   192,   394,   454,   483,   534,   485,   509,
     569,    54,    55,    56,    57,    93,   170,   457,   458,    58,
      59,    60,   399,   172,    61,    62,    63,    79,    64,   324,
     174,   175,   488,   489,   490,   491,   402,   467,   495,   538,
      65,    80,   185,   408,   496,    66,   176,    67,    81,    68,
     329,   463,   404,    69,   177,   494
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -428
static const short int yypact[] =
{
     109,    14,  -428,   -39,    74,   139,   136,  -428,  -428,  -428,
      88,  -428,  -428,   -24,  -428,    78,  -428,  -428,  -428,    89,
     195,    26,  -428,  -428,   187,  -428,  -428,   781,  -428,  -428,
    -428,  -428,   -10,  -428,  -428,    76,    -5,   177,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,    92,    79,  -428,  -428,    98,  -428,   100,  -428,  -428,
      67,  -428,  -428,  -428,  -428,  -428,  -428,  -428,   195,  -428,
     124,   146,  -428,  -428,  -428,  -428,  -428,    81,  -428,    91,
     104,    97,  -428,   107,  -428,  -428,  -428,   111,    97,  -428,
    -428,    67,  -428,  -428,  -428,  -428,  -428,  -428,  -428,    13,
    -428,   110,    96,  -428,  -428,  -428,  -428,   117,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,    97,  -428,  -428,  -428,  -428,
     165,   420,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,   185,   971,   143,  -428,  -428,   460,  -428,   200,   334,
     273,    22,   825,   546,   825,   546,  -428,   130,  -428,  -428,
      97,  -428,  -428,    97,  -428,  -428,   420,  -428,  -428,  -428,
    -428,  -428,   195,    79,  -428,  -428,   195,   184,  -428,  -428,
     882,  -428,  -428,     7,  -428,   207,   204,    97,   211,   212,
     207,    97,  1021,   207,  -428,    47,   358,  -428,  -428,   -10,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,   117,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,   213,  -428,   161,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,   -27,  -428,   420,  -428,  -428,  -428,   420,  -428,
    -428,   150,   148,   734,  -428,   420,  -428,   420,  -428,  -428,
      97,   214,  -428,   107,  -428,  -428,    97,  -428,  -428,  -428,
    -428,   219,  -428,  -428,  -428,  -428,    97,    23,    17,  -428,
    -428,  -428,  -428,  -428,   704,  -428,  -428,  -428,  -428,   704,
     158,  -428,   160,    97,  -428,  -428,  -428,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,    97,   161,  1053,
      97,    97,    97,    97,    97,    97,    40,   163,   164,   186,
     191,  -428,  -428,  -428,  -428,  -428,  -428,   420,  -428,  -428,
    -428,   157,  -428,   -43,  -428,    97,   195,   420,  -428,  -428,
    -428,  -428,  -428,   420,   231,  -428,   169,   420,    62,   209,
    -428,  -428,   420,   167,   173,    97,    97,   820,   820,   820,
     820,   765,   765,   140,   140,  -428,  -428,  -428,  -428,  -428,
     420,   420,   420,   420,   420,   420,  -428,  -428,  -428,  -428,
     172,  -428,    97,  -428,  -428,  -428,  -428,  -428,  1021,  -428,
    -428,  -428,  -428,  -428,   153,   420,  -428,   188,  -428,   189,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,   960,   385,    97,   192,  -428,   420,  -428,  -428,
    -428,   178,  -428,   159,  -428,  -428,  -428,   195,  -428,  -428,
    -428,  -428,   195,  -428,   195,   207,  -428,    97,  -428,  -428,
     420,  -428,    53,  -428,   640,   193,   179,    97,  -428,  -428,
    -428,   107,   194,  -428,  -428,   198,  -428,   420,  -428,  -428,
     168,  -428,  -428,  -428,   420,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,   420,  -428,  -428,    97,  -428,  -428,   202,    97,
     196,    97,    97,  -428,  1053,  -428,  -428,  -428,    97,  -428,
    -428,   602,  -428,   420,   197,   420,  -428,  -428,  -428,   205,
    -428,  -428,  -428,  -428,  -428,    97,  -428,   203,  -428,   221,
    -428,  -428,  -428,  -428,    97,  -428
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -428,  -428,  -428,  -428,    30,  -124,  -428,  -428,  -428,  -428,
    -428,  -428,  -137,  -428,  -428,  -428,  -428,    58,  -428,   166,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,    -3,
       0,   -62,  -428,  -428,  -428,    16,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,   272,  -428,  -428,  -428,   -77,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -144,  -140,  -428,  -428,  -367,  -428,  -428,  -428,   -95,  -160,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -223,  -428,
    -428,  -428,  -428,  -166,     2,  -151,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,   138,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,   -13,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -146,  -428,  -428,  -428,  -428,  -243,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -427,  -143,    -9,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,   215,  -428,  -428,  -389,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -204,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,  -428,
    -428,  -428,  -428,  -428,  -428,  -428
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -380
static const short int yytable[] =
{
      10,   194,   429,    19,   131,    10,    10,   323,   296,    27,
     296,   166,   297,   460,   297,  -159,   178,    13,   307,    33,
      10,   162,    26,   164,    10,   337,   453,   167,   168,  -323,
     337,   -41,    34,   337,   338,    24,    78,    10,    75,   344,
      26,  -159,   349,   330,    82,  -159,  -159,   -50,   196,   -50,
      37,    76,   347,    26,    28,    71,    29,   298,    91,   298,
     -50,   332,   -50,    94,    95,    96,    72,   198,    10,   213,
      14,   103,   -50,   333,    22,   179,   304,    15,   308,    30,
     498,    35,   160,   315,    97,   -50,   317,   267,  -266,  -379,
    -379,   173,    36,   171,   436,    28,    38,    29,   268,  -266,
     107,   108,   109,   110,   318,   180,  -235,   436,   549,    -2,
     340,   322,   111,   112,   345,   113,   -50,    84,   -50,    74,
      30,   560,   -51,   114,   115,   195,    89,   116,   117,    90,
     118,   119,    92,     1,   465,   466,    -3,   437,   570,    -8,
      98,   -50,   120,   -50,   246,   -50,   -50,   575,   -50,   100,
     437,   104,     2,    14,   165,   121,   105,   309,   316,   122,
     123,   124,   125,     1,   126,     3,   319,   320,   -50,   106,
      82,   -50,    10,    34,   325,   326,    10,   556,     3,     2,
     161,   335,     2,   182,   -50,   198,   -50,    -7,   321,   386,
     127,   214,   327,   128,   -50,   247,   -50,   353,  -292,   210,
     211,   212,   -50,   248,   -50,     3,   328,   339,     3,   -50,
     107,   108,   194,   110,   341,   342,   368,    76,   381,   -50,
       2,   382,   405,   112,   410,   398,   411,   -50,   445,   452,
       2,   443,   444,   446,   462,   464,   387,   116,     2,   469,
     118,   119,   470,   397,   468,   476,     3,   -39,   484,   403,
    -319,   530,   438,   508,   486,  -370,     3,   354,   501,   407,
     537,   400,   552,    14,     3,   543,   529,   563,   184,   122,
     123,  -374,   336,   572,   126,   550,   412,    25,   566,   521,
     417,   418,   419,   420,   421,   422,   423,   424,   425,   426,
     427,   573,   306,   430,   431,   432,   433,   434,   435,   383,
     127,   478,   557,   128,   393,   479,   169,   536,   347,     0,
       0,     0,     0,   195,   257,     0,   395,     0,   455,   396,
       0,     0,   401,   511,     0,   258,   259,     0,   260,     0,
       0,     0,     0,     0,     0,   337,   195,     0,   472,   473,
     261,     0,     0,     0,   515,     0,     0,     0,     0,     0,
       0,   262,   263,   264,     0,   415,   416,     0,     0,     0,
     525,     0,     0,     0,     0,   477,     0,     0,     0,   249,
     250,   251,   252,   253,   254,   200,    10,   201,   438,   255,
       0,     0,     0,   531,   450,   451,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    10,   500,   256,   493,
     461,   459,   200,     0,   202,   203,   204,   205,   206,   207,
       0,   531,   456,     0,     0,   208,   209,   210,   211,   212,
     517,     0,     0,     0,   531,     0,     0,   524,     0,   351,
       0,   202,   203,   204,   205,   206,   207,   200,     0,   201,
       0,   531,   208,   209,   210,   211,   212,   309,     0,   480,
     531,     0,     0,     0,     0,     0,     0,     0,   487,     0,
     492,     0,   551,     0,   553,   555,   202,   203,   204,   205,
     206,   207,     0,     0,     0,     0,     0,   208,   209,   210,
     211,   212,   507,     0,     0,     0,    82,    10,   512,     0,
       0,     0,    10,   516,    10,     0,   519,     0,   518,     0,
     522,  -265,    10,   510,     0,     0,     0,   195,   513,   535,
     514,     0,  -265,  -265,     0,  -265,   539,     0,     0,   542,
       0,     0,     0,     0,     0,  -266,     0,  -265,     0,   545,
     547,     0,     0,     0,   548,   195,  -266,     0,  -265,  -265,
    -265,     0,     0,   558,     0,     0,   561,     0,   195,   107,
     108,   109,   110,     0,   564,     0,     0,   568,   565,     0,
       0,   111,   112,   571,   113,   195,     0,     0,     0,     0,
       0,   574,   114,   115,   195,     0,   116,   117,     0,   118,
     119,   271,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   120,   272,   273,   274,   275,   276,   277,   300,     0,
       0,   301,    14,   278,   302,   280,   281,   282,   122,   123,
     124,   125,     0,   126,     0,     0,     0,     0,     0,   200,
       0,   201,   283,   284,     0,     0,     0,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,     0,   127,
       0,     0,   128,   107,   108,   109,   110,     0,   202,   203,
     204,   205,   206,   207,     0,   111,   112,     0,   113,   208,
     209,   210,   211,   212,     0,     0,   114,   115,   562,     0,
     116,   117,     0,   118,   119,   271,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   120,   272,   273,   274,   275,
     276,   277,   523,     0,     0,     0,    14,   278,   302,   280,
     281,   282,   122,   123,   124,   125,     0,   126,     0,     0,
       0,     0,     0,     0,     0,     0,   283,   284,     0,     0,
       0,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   295,     0,   127,     0,     0,   128,   107,   108,   109,
     110,     0,     0,     0,     0,  -228,     0,     0,     0,   111,
     112,     0,   113,     0,     0,     0,  -228,  -228,     0,  -228,
     114,   115,     0,     0,   116,   117,     0,   118,   119,  -228,
       0,  -228,     0,     0,     0,     0,     0,     0,     0,   120,
    -228,     0,  -228,  -228,  -228,     0,   384,     0,     0,    39,
      14,    40,   121,    41,    42,    43,   122,   123,   124,   125,
       0,   126,    44,   385,    45,     0,   -73,     0,     0,  -344,
    -346,   202,   203,   204,   205,  -380,  -380,     0,     0,     0,
       0,     0,   208,   209,   210,   211,   212,   127,   270,     0,
     128,   110,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     271,     0,     0,     0,     0,     0,  -380,  -380,  -380,  -380,
       0,   272,   273,   274,   275,   276,   277,   208,   209,   210,
     211,   212,   278,   279,   280,   281,   282,     0,   123,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   283,   284,     0,     0,     0,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   271,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   272,   273,
     274,   275,   276,   277,   300,     0,     0,   301,     0,   278,
     279,   280,   281,   282,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   283,   284,
       0,     0,     0,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   215,     0,     0,     0,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   202,   203,   204,   205,
     206,   207,     0,     0,     0,     0,     0,   208,   209,   210,
     211,   212,     0,     0,   346,     0,     0,    14,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   428,     0,     0,     0,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243
};

static const short int yycheck[] =
{
       0,   125,   369,     1,    81,     5,     6,   173,   152,     9,
     154,    88,   152,   402,   154,    42,     3,     3,   155,    43,
      20,    83,     6,    85,    24,   185,    69,    89,    90,    72,
     190,    70,    56,   193,   185,     5,    39,    37,    43,   190,
      24,    68,   193,   180,    47,    72,    73,     7,   125,     9,
      20,    56,   192,    37,     7,    65,     9,   152,    58,   154,
       7,    54,     9,    63,    64,    65,    76,    20,    68,   131,
      56,    74,    32,    66,     0,    62,   153,    63,   155,    32,
     469,     3,    82,   160,    68,    32,   163,    65,    65,    72,
      73,    94,     3,    93,    54,     7,    70,     9,    76,    76,
       3,     4,     5,     6,   166,    92,    66,    54,   535,     0,
     187,   173,    15,    16,   191,    18,     7,    25,     9,    43,
      32,   548,    43,    26,    27,   125,    28,    30,    31,    29,
      33,    34,    65,    24,    72,    73,     0,    97,   565,     0,
      16,    32,    45,     7,   142,     9,     7,   574,     9,     3,
      97,    70,    43,    56,    43,    58,    65,   157,   161,    62,
      63,    64,    65,    24,    67,    69,   169,   170,    32,    65,
     173,    32,   172,    56,   174,   175,   176,   544,    69,    43,
      73,   184,    43,    73,     7,    20,     9,     0,   172,   313,
      93,     6,   176,    96,     7,    52,     9,   197,    68,    59,
      60,    61,     7,     3,     9,    69,    22,     3,    69,    32,
       3,     4,   336,     6,     3,     3,     3,    56,    68,    32,
      43,    73,     3,    16,    66,    11,    66,    32,    42,    72,
      43,    68,    68,    42,     3,    66,   313,    30,    43,    72,
      33,    34,    69,   320,    35,    73,    69,    70,    95,   326,
      72,    72,   376,    94,    66,    66,    69,   199,    66,   336,
      66,   323,    66,    56,    69,    97,    73,    70,   102,    62,
      63,    73,    65,    70,    67,    73,   353,     5,    73,   502,
     357,   358,   359,   360,   361,   362,   363,   364,   365,   366,
     367,    70,   154,   370,   371,   372,   373,   374,   375,   312,
      93,   447,   545,    96,   313,   448,    91,   511,   448,    -1,
      -1,    -1,    -1,   313,    41,    -1,   316,    -1,   395,   319,
      -1,    -1,   325,   489,    -1,    52,    53,    -1,    55,    -1,
      -1,    -1,    -1,    -1,    -1,   495,   336,    -1,   415,   416,
      67,    -1,    -1,    -1,   495,    -1,    -1,    -1,    -1,    -1,
      -1,    78,    79,    80,    -1,   355,   356,    -1,    -1,    -1,
     504,    -1,    -1,    -1,    -1,   442,    -1,    -1,    -1,    35,
      36,    37,    38,    39,    40,    17,   376,    19,   502,    45,
      -1,    -1,    -1,   507,   387,   388,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   396,   474,    64,   461,
     403,   401,    17,    -1,    46,    47,    48,    49,    50,    51,
      -1,   535,   396,    -1,    -1,    57,    58,    59,    60,    61,
     497,    -1,    -1,    -1,   548,    -1,    -1,   504,    -1,    71,
      -1,    46,    47,    48,    49,    50,    51,    17,    -1,    19,
      -1,   565,    57,    58,    59,    60,    61,   447,    -1,   449,
     574,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   458,    -1,
     460,    -1,   539,    -1,   541,   542,    46,    47,    48,    49,
      50,    51,    -1,    -1,    -1,    -1,    -1,    57,    58,    59,
      60,    61,   482,    -1,    -1,    -1,   489,   487,   491,    -1,
      -1,    -1,   492,   496,   494,    -1,   499,    -1,   498,    -1,
     503,    41,   502,   487,    -1,    -1,    -1,   507,   492,   509,
     494,    -1,    52,    53,    -1,    55,   516,    -1,    -1,   519,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    -1,   529,
     533,    -1,    -1,    -1,   534,   535,    76,    -1,    78,    79,
      80,    -1,    -1,   546,    -1,    -1,   549,    -1,   548,     3,
       4,     5,     6,    -1,   557,    -1,    -1,   560,   558,    -1,
      -1,    15,    16,   566,    18,   565,    -1,    -1,    -1,    -1,
      -1,   571,    26,    27,   574,    -1,    30,    31,    -1,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      -1,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    -1,    67,    -1,    -1,    -1,    -1,    -1,    17,
      -1,    19,    76,    77,    -1,    -1,    -1,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    -1,    93,
      -1,    -1,    96,     3,     4,     5,     6,    -1,    46,    47,
      48,    49,    50,    51,    -1,    15,    16,    -1,    18,    57,
      58,    59,    60,    61,    -1,    -1,    26,    27,    66,    -1,
      30,    31,    -1,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    -1,    -1,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    -1,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,    -1,
      -1,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    -1,    93,    -1,    -1,    96,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,    15,
      16,    -1,    18,    -1,    -1,    -1,    52,    53,    -1,    55,
      26,    27,    -1,    -1,    30,    31,    -1,    33,    34,    65,
      -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      76,    -1,    78,    79,    80,    -1,    52,    -1,    -1,     8,
      56,    10,    58,    12,    13,    14,    62,    63,    64,    65,
      -1,    67,    21,    69,    23,    -1,    25,    -1,    -1,    28,
      29,    46,    47,    48,    49,    50,    51,    -1,    -1,    -1,
      -1,    -1,    57,    58,    59,    60,    61,    93,     3,    -1,
      96,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      -1,    46,    47,    48,    49,    50,    51,    57,    58,    59,
      60,    61,    57,    58,    59,    60,    61,    -1,    63,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    77,    -1,    -1,    -1,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,
      48,    49,    50,    51,    52,    -1,    -1,    55,    -1,    57,
      58,    59,    60,    61,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,
      -1,    -1,    -1,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,     3,    -1,    -1,    -1,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    46,    47,    48,    49,
      50,    51,    -1,    -1,    -1,    -1,    -1,    57,    58,    59,
      60,    61,    -1,    -1,     3,    -1,    -1,    56,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     3,    -1,    -1,    -1,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
{
       0,    24,    43,    69,    99,   100,   102,   103,   117,   127,
     128,   133,   143,     3,    56,    63,   144,   145,   146,   212,
     118,   120,     0,   101,   102,   143,   133,   128,     7,     9,
      32,   105,   107,    43,    56,     3,     3,   102,    70,     8,
      10,    12,    13,    14,    21,    23,   131,   132,   134,   135,
     138,   139,   140,   141,   269,   270,   271,   272,   277,   278,
     279,   282,   283,   284,   286,   298,   303,   305,   307,   311,
     108,    65,    76,   115,    43,    43,    56,   119,   127,   285,
     299,   306,   127,   211,    25,   136,   129,   130,   142,    28,
      29,   128,    65,   273,   128,   128,   128,   133,    16,   106,
       3,   111,   104,   127,    70,    65,    65,     3,     4,     5,
       6,    15,    16,    18,    26,    27,    30,    31,    33,    34,
      45,    58,    62,    63,    64,    65,    67,    93,    96,   121,
     128,   147,   149,   153,   160,   167,   170,   176,   181,   186,
     187,   190,   191,   193,   195,   196,   197,   210,   212,   213,
     216,   217,   218,   219,   220,   221,   225,   230,   233,   260,
     128,    73,   129,   137,   129,    43,   147,   129,   129,   273,
     274,   128,   281,   127,   288,   289,   304,   312,     3,    62,
      92,   109,    73,   116,   117,   300,   215,   185,   214,   150,
     183,   184,   261,   182,   103,   128,   147,   177,    20,   123,
      17,    19,    46,    47,    48,    49,    50,    51,    57,    58,
      59,    60,    61,   129,     6,     3,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,   189,   192,   212,    52,     3,    35,
      36,    37,    38,    39,    40,    45,    64,    41,    52,    53,
      55,    67,    78,    79,    80,   222,   224,    65,    76,   203,
       3,    35,    46,    47,    48,    49,    50,    51,    57,    58,
      59,    60,    61,    76,    77,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,   188,   189,   196,   223,
      52,    55,    58,   110,   147,   188,   223,   110,   147,   128,
     231,   232,   234,   238,   239,   147,   127,   147,   129,   127,
     127,   133,   129,   211,   287,   128,   128,   133,    22,   308,
     110,   112,    54,    66,   113,   127,    65,   197,   213,     3,
     147,     3,     3,   148,   213,   147,     3,   189,   259,   213,
     199,    71,   198,   128,   115,   168,   169,   162,   161,   164,
     163,   165,   166,   171,   172,   174,   173,   175,     3,   194,
     154,   155,   157,   156,   158,   159,   204,   227,   226,   229,
     228,    68,    73,   231,    52,    69,   103,   147,   236,   237,
     248,   253,   255,   260,   262,   128,   128,   147,    11,   280,
     129,   127,   294,   147,   310,     3,   114,   147,   301,   151,
      66,    66,   147,   178,   122,   128,   128,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,     3,   192,
     147,   147,   147,   147,   147,   147,    54,    97,   103,   200,
     201,   206,   207,    68,    68,    42,    42,   235,   254,   249,
     127,   127,    72,    69,   263,   147,   133,   275,   276,   128,
     276,   127,     3,   309,    66,    72,    73,   295,    35,    72,
      69,   124,   147,   147,   208,   205,    73,   147,   239,   259,
     128,   246,   256,   264,    95,   266,    66,   128,   290,   291,
     292,   293,   128,   129,   313,   296,   302,   152,   276,   125,
     147,    66,   202,   244,   245,   247,   250,   128,    94,   267,
     133,   211,   127,   133,   133,   213,   127,   147,   128,   127,
     209,   206,   127,    52,   147,   188,   240,   242,   243,    73,
      72,   103,   147,   258,   265,   128,   292,    66,   297,   128,
     179,   180,   128,    97,   241,   128,   251,   127,   128,   258,
      73,   147,    66,   147,   126,   147,   192,   244,   127,   257,
     258,   127,    66,    70,   127,   128,    73,   252,   127,   268,
     258,   127,    70,    70,   128,   258
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
        case 3:

    { /* Intentionally empty actions. */ ;}
    break;

  case 9:

    {	UNPARSABLE_GET(Unparse_Stmts((yyvsp[-1].stmt), (yyvsp[0].stmt)));
							GET_DELTASYMBOLS.ResetTemp();
							(yyval.stmt) = GET_TRANSLATOR.Translate_Stmts((yyvsp[-1].stmt), (yyvsp[0].stmt)); 
							UNPARSABLE_SET((yyval.stmt));	;}
    break;

  case 10:

    { (yyval.stmt) = GET_TRANSLATOR.Translate_Stmts((yyvsp[0].stmt)); ;}
    break;

  case 11:

    {	GET_TRANSLATOR.Translate_FunctionHeader((yyvsp[-1].sym)); 
						GET_DELTASYMBOLS.PushAndResetTempCounter(); SG(S_BLOCK); ;}
    break;

  case 12:

    {	EG(S_BLOCK); EM(S_FUNC_((yyvsp[-4].sym))); 
						UNPARSABLE_GET(Unparse_Function((yyvsp[-4].sym), (yyvsp[-1].stmt)));
						if ((yyval.sym) = GET_TRANSLATOR.Translate_Function((yyvsp[-4].sym), (yyvsp[-1].stmt), (yyvsp[-5].quadNo), (yyvsp[0].intVal)))
							DPTR((yyval.sym))->SetSourceCode(unparsed);
					;}
    break;

  case 13:

    { (yyval.intVal) = DELTA_FUNCCLASS_PROGRAMFUNCTION; ;}
    break;

  case 14:

    { (yyval.intVal) = DELTA_FUNCCLASS_ONEVENT; ;}
    break;

  case 15:

    { (yyval.intVal) = DELTA_FUNCCLASS_METHOD; ;}
    break;

  case 16:

    { (yyval.intVal) = DELTA_FUNCTION_NO_EXPORT; ;}
    break;

  case 17:

    { (yyval.intVal) = DELTA_FUNCTION_DEFAULT_EXPORT; ;}
    break;

  case 18:

    { SM(S_FUNC_((yyvsp[0].intVal))); PE2(T_FUNC_((yyvsp[0].intVal)), T_LOCAL_OR_FUNCNAME); ;}
    break;

  case 19:

    { PE(T_LPAR); SG(S_FARGS); (yyval.sym) = GET_TRANSLATOR.Translate_Function((yyvsp[0].id), (yyvsp[-3].intVal), (yyvsp[-1].intVal)); ;}
    break;

  case 20:

    { (yyval.id) = (yyvsp[0].id); ;}
    break;

  case 21:

    { (yyval.id) = (yyvsp[0].id); ;}
    break;

  case 22:

    { (yyval.id) = DELTA_TOSTRING_SYMBOLIC_NAME; ;}
    break;

  case 23:

    { unullify((yyval.id)); ;}
    break;

  case 24:

    { (yyval.id) = (yyvsp[0].id);		;}
    break;

  case 25:

    { (yyval.id) = DELTA_OPERATOR_OBJECT_GET;	;}
    break;

  case 26:

    { (yyval.id) = DELTA_OPERATOR_OBJECT_SET;	;}
    break;

  case 27:

    { PE2(T_COMMA, T_IDENT); ;}
    break;

  case 28:

    { GET_TRANSLATOR.Translate_IdentList((yyvsp[0].id)); ;}
    break;

  case 29:

    { PE(T_IDENT); GET_TRANSLATOR.Translate_IdentList((yyvsp[0].id)); ;}
    break;

  case 31:

    { EG(S_FARGS); PE(T_RPAR);  ;}
    break;

  case 32:

    { EG(S_FARGS); PE2(T_TDOT, T_RPAR); ;}
    break;

  case 33:

    {  GET_TRANSLATOR.Translate_VarArgs(); ;}
    break;

  case 34:

    { EG(S_FARGS); PE(T_RPAR); ;}
    break;

  case 35:

    { OE(T_LPAR_OR_TDOT); ;}
    break;

  case 37:

    { EG(S_FARGS); PE(T_RPAR); ;}
    break;

  case 38:

    { GET_TRANSLATOR.Translate_CompoundBegin(); PE(T_LBC); ;}
    break;

  case 39:

    { PE(T_RBC); ;}
    break;

  case 40:

    {  Unparse_Compound((yyval.stmt) = (yyvsp[-2].stmt)); GET_TRANSLATOR.Translate_CompoundEnd(); ;}
    break;

  case 41:

    { PE(T_LBC); ;}
    break;

  case 42:

    {  PE(T_RBC); Unparse_Compound((yyval.stmt) = NEW_STMT); GET_TRANSLATOR.Translate_EmptyCompound(); ;}
    break;

  case 43:

    {	GET_TRANSLATOR.Translate_FunctionHeader((yyvsp[-1].sym)); 
							GET_DELTASYMBOLS.PushAndResetTempCounter(); 
							PE(T_LBC); ;}
    break;

  case 44:

    {	EM(S_FUNC_((yyvsp[-3].sym))); 
							UNPARSABLE_GET(Unparse_LambdaFunction((yyvsp[-3].sym), (yyvsp[0].stmt)));
							if ((yyval.sym) = GET_TRANSLATOR.Translate_Function((yyvsp[-3].sym), (yyvsp[0].stmt), (yyvsp[-4].quadNo), 0))
								DPTR((yyval.sym))->SetSourceCode(unparsed);
							GET_DELTASYMBOLS.PopAndRestoreTempCounter(); ;}
    break;

  case 45:

    {	SM(S_FUNC_(DELTA_FUNCCLASS_PROGRAMFUNCTION)); PE2(T_LAMBDA, T_LPAR); SG(S_FARGS); 
							(yyval.sym) = GET_TRANSLATOR.Translate_Function((char*) 0, DELTA_FUNCCLASS_PROGRAMFUNCTION); ;}
    break;

  case 46:

    { GET_TRANSLATOR.Translate_CompoundBegin(); PE(S_EXPR); ;}
    break;

  case 47:

    { (yyval.stmt) = (yyvsp[-1].stmt); GET_TRANSLATOR.Translate_CompoundEnd(); GET_QUADS.SetQuadLine((yyvsp[-2].quadNo), (yyvsp[-3].intVal), true); ;}
    break;

  case 48:

    { (yyval.stmt) = GET_TRANSLATOR.Translate_RETURN((yyvsp[0].expr)); Unparse_LambdaStmt((yyval.stmt), (yyvsp[0].expr)); PE(T_RBC); ;}
    break;

  case 49:

    { (yyval.intVal) = GET_PARSEPARMS.GetLine(); ;}
    break;

  case 50:

    { (yyval.quadNo) = GET_QUADS.NextQuadNo(); ;}
    break;

  case 51:

    { PE(T_SEMI); ;}
    break;

  case 53:

    { SM(S_STMT); ;}
    break;

  case 54:

    { (yyval.stmt) =	GET_TRANSLATOR.Translate_ExprListStmt((yyvsp[-1].expr)); Unparse_ExprListStmt((yyval.stmt), (yyvsp[-1].expr)); ;}
    break;

  case 55:

    { (yyval.stmt) = (yyvsp[0].stmt); GET_QUADS.SetQuadLine((yyvsp[-1].quadNo), (yyvsp[-2].intVal), true);  GET_TRANSLATOR.Translate_BasicStmt((yyvsp[-2].intVal)); ;}
    break;

  case 56:

    { (yyval.stmt) = (yyvsp[0].stmt); GET_QUADS.SetQuadLine((yyvsp[-1].quadNo), (yyvsp[-2].intVal));  GET_TRANSLATOR.Translate_BasicStmt((yyvsp[-2].intVal)); ;}
    break;

  case 57:

    { (yyval.stmt) = (yyvsp[0].stmt); ;}
    break;

  case 58:

    { (yyval.stmt) = NEW_STMT; Unparse_FunctionDef((yyval.stmt), (yyvsp[0].sym)); ;}
    break;

  case 59:

    { (yyval.stmt) = NEW_STMT; (yyval.stmt)->SetUnparsed(";"); ;}
    break;

  case 60:

    { EM(S_STMT); (yyval.stmt) = (yyvsp[0].stmt); ;}
    break;

  case 61:

    { EM(S_STMT); (yyval.stmt) = (yyvsp[0].stmt); ;}
    break;

  case 62:

    { (yyval.stmt) = (yyvsp[0].stmt);;}
    break;

  case 63:

    { (yyval.stmt) = (yyvsp[0].stmt);;}
    break;

  case 64:

    { (yyval.stmt) = (yyvsp[0].stmt);;}
    break;

  case 65:

    { (yyval.stmt) = (yyvsp[0].stmt); ;}
    break;

  case 66:

    { (yyval.stmt) = (yyvsp[0].stmt);;}
    break;

  case 67:

    { EM(S_STMT); (yyval.stmt) = GET_TRANSLATOR.Translate_BREAK(); Unparse_BuiltInStmt((yyval.stmt), BREAK); ;}
    break;

  case 68:

    { EM(S_STMT); (yyval.stmt) = GET_TRANSLATOR.Translate_CONTINUE(); Unparse_BuiltInStmt((yyval.stmt), CONTINUE); ;}
    break;

  case 69:

    { (yyval.stmt) = (yyvsp[0].stmt); ;}
    break;

  case 70:

    { (yyval.stmt) = (yyvsp[0].stmt); ;}
    break;

  case 71:

    {	SM(S_ASSRT); SG(S_EXPR); 
						GET_PARSEPARMS.InAssertStmt().enter(); ;}
    break;

  case 72:

    {	EG(S_EXPR); PE(S_EXPR); EM(S_ASSRT); 
						GET_TRANSLATOR.Translate_ASSERT((yyvsp[0].expr)); 
						GET_PARSEPARMS.InAssertStmt().exit(); 
						Unparse_ExprStmt((yyval.stmt) = NEW_STMT, ASSERT, (yyvsp[0].expr)); ;}
    break;

  case 73:

    { SM(S_STMT); PE(S_ASSRT);	;}
    break;

  case 74:

    { (yyval.stmt) = (yyvsp[-1].stmt); ;}
    break;

  case 75:

    { SM(S_RET); ;}
    break;

  case 76:

    { PE2(T_RET, S_EXPR); ;}
    break;

  case 77:

    {	EM(S_RET); 
						(yyval.stmt) = GET_TRANSLATOR.Translate_RETURN((yyvsp[-1].expr)); 
						Unparse_ExprStmt((yyval.stmt), RETURN, (yyvsp[-1].expr)); ;}
    break;

  case 78:

    {	EM(S_RET); 
						(yyval.stmt) = GET_TRANSLATOR.Translate_RETURN(); 
						Unparse_BuiltInStmt((yyval.stmt), RETURN); ;}
    break;

  case 82:

    { GET_TRANSLATOR.Translate_UsingNamespace((yyvsp[-1].id), true); ;}
    break;

  case 83:

    { GET_TRANSLATOR.Translate_UsingNamespace((yyvsp[-1].id), false); ;}
    break;

  case 84:

    { GET_TRANSLATOR.Translate_UsingByteCodeLibrary((yyvsp[-2].id), (yyvsp[0].intVal)); ;}
    break;

  case 85:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 86:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 87:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 88:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 89:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 90:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 91:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 92:

    { (yyval.strConst) = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr((yyvsp[0].id))); ;}
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

    { EM(S_CONST); (yyval.expr) = GET_TRANSLATOR.Translate_ConstAssignExpression((yyvsp[-4].strConst), (yyvsp[0].expr)); ;}
    break;

  case 97:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ASSIGN, S_EXPR); ;}
    break;

  case 98:

    {	EM(S_ASSIGNEXPR); 
								(yyval.expr) = GET_TRANSLATOR.Translate_AssignExpr((yyvsp[-3].expr), (yyvsp[0].expr)); (yyvsp[-3].expr)->SetInitialised(); 
								Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), ASSIGN, (yyvsp[0].expr));	;}
    break;

  case 99:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_ADDA, S_EXPR); ;}
    break;

  case 100:

    {	EM(S_ASSIGNEXPR); 
								(yyval.expr) = GET_TRANSLATOR.Translate_AssignArithExpr((yyvsp[-3].expr), (yyvsp[0].expr), DeltaIC_ADD, "+="); 
								Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), ADD_A, (yyvsp[0].expr));	;}
    break;

  case 101:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MULA, S_EXPR); ;}
    break;

  case 102:

    {	EM(S_ASSIGNEXPR); 
								(yyval.expr) = GET_TRANSLATOR.Translate_AssignArithExpr((yyvsp[-3].expr), (yyvsp[0].expr), DeltaIC_MUL, "*="); 
								Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), MUL_A, (yyvsp[0].expr));	;}
    break;

  case 103:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_SUBA, S_EXPR); ;}
    break;

  case 104:

    {	EM(S_ASSIGNEXPR); 
								(yyval.expr) = GET_TRANSLATOR.Translate_AssignArithExpr((yyvsp[-3].expr), (yyvsp[0].expr), DeltaIC_SUB, "-="); 
								Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), SUB_A, (yyvsp[0].expr));	;}
    break;

  case 105:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_DIVA, S_EXPR); ;}
    break;

  case 106:

    {	EM(S_ASSIGNEXPR); 
								(yyval.expr) = GET_TRANSLATOR.Translate_AssignArithExpr((yyvsp[-3].expr), (yyvsp[0].expr), DeltaIC_DIV, "/="); 
								Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), DIV_A, (yyvsp[0].expr));	;}
    break;

  case 107:

    { SM(S_ASSIGNEXPR); PE3(S_LVAL, T_MODA, S_EXPR); ;}
    break;

  case 108:

    {	EM(S_ASSIGNEXPR); 
								(yyval.expr) = GET_TRANSLATOR.Translate_AssignArithExpr((yyvsp[-3].expr), (yyvsp[0].expr), DeltaIC_MOD, "%="); 
								Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), MOD_A, (yyvsp[0].expr));	;}
    break;

  case 109:

    { SM(S_RELAT); PE3(S_EXPR, T_GT, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 110:

    {	EM(S_RELAT); (yyval.expr) = GET_TRANSLATOR.Translate_RelationalExpr((yyvsp[-3].expr), (yyvsp[0].expr)->AdaptIfBool(), DeltaIC_JGT, ">"); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), GT, (yyvsp[0].expr));	;}
    break;

  case 111:

    { SM(S_RELAT); PE3(S_EXPR, T_LT, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 112:

    {	EM(S_RELAT); (yyval.expr) = GET_TRANSLATOR.Translate_RelationalExpr((yyvsp[-3].expr), (yyvsp[0].expr)->AdaptIfBool(), DeltaIC_JLT, "<"); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), LT, (yyvsp[0].expr));	;}
    break;

  case 113:

    { SM(S_RELAT); PE3(S_EXPR, T_GE, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 114:

    {	EM(S_RELAT); (yyval.expr) = GET_TRANSLATOR.Translate_RelationalExpr((yyvsp[-3].expr), (yyvsp[0].expr)->AdaptIfBool(), DeltaIC_JGE, ">="); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), GE, (yyvsp[0].expr));	;}
    break;

  case 115:

    { SM(S_RELAT); PE3(S_EXPR, T_LE, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 116:

    {	EM(S_RELAT); (yyval.expr) = GET_TRANSLATOR.Translate_RelationalExpr((yyvsp[-3].expr), (yyvsp[0].expr)->AdaptIfBool(), DeltaIC_JLE, "<="); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), LE, (yyvsp[0].expr));	;}
    break;

  case 117:

    { SM(S_RELAT); PE3(S_EXPR, T_EQ, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 118:

    {	EM(S_RELAT); (yyval.expr) = GET_TRANSLATOR.Translate_RelationalExpr((yyvsp[-3].expr), (yyvsp[0].expr)->AdaptIfBool(), DeltaIC_JEQ, "=="); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), EQ, (yyvsp[0].expr));	;}
    break;

  case 119:

    { SM(S_RELAT); PE3(S_EXPR, T_NE, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 120:

    {	EM(S_RELAT); (yyval.expr) = GET_TRANSLATOR.Translate_RelationalExpr((yyvsp[-3].expr), (yyvsp[0].expr)->AdaptIfBool(), DeltaIC_JNE, "!="); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), NE, (yyvsp[0].expr));	;}
    break;

  case 121:

    { SM(S_LOGICAL); PE3(S_EXPR, T_AND, S_EXPR); DNPTR((yyvsp[-1].expr))->AdaptToBool(); ;}
    break;

  case 122:

    {	EM(S_LOGICAL); (yyval.expr) = GET_TRANSLATOR.Translate_ExprANDExpr((yyvsp[-4].expr), (yyvsp[-1].quadNo), (yyvsp[0].expr)); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-4].expr), AND, (yyvsp[0].expr));	;}
    break;

  case 123:

    { SM(S_LOGICAL); PE3(S_EXPR, T_OR, S_EXPR); DNPTR((yyvsp[-1].expr))->AdaptToBool(); ;}
    break;

  case 124:

    {	EM(S_LOGICAL); (yyval.expr) = GET_TRANSLATOR.Translate_ExprORExpr((yyvsp[-4].expr), (yyvsp[-1].quadNo), (yyvsp[0].expr)); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-4].expr), OR, (yyvsp[0].expr));	;}
    break;

  case 125:

    { SM(S_ARITH); PE3(S_EXPR, T_ADD, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 126:

    {	EM(S_ARITH); (yyval.expr) = GET_TRANSLATOR.Translate_ArithmeticExpression((yyvsp[-3].expr), DeltaIC_ADD, (yyvsp[0].expr)->AdaptIfBool(), "+"); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), ADD, (yyvsp[0].expr));	;}
    break;

  case 127:

    { SM(S_ARITH); PE3(S_EXPR, T_SUB, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 128:

    {	EM(S_ARITH); (yyval.expr) = GET_TRANSLATOR.Translate_ArithmeticExpression((yyvsp[-3].expr), DeltaIC_SUB, (yyvsp[0].expr)->AdaptIfBool(), "-"); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), SUB, (yyvsp[0].expr));	;}
    break;

  case 129:

    { SM(S_ARITH); PE3(S_EXPR, T_DIV, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 130:

    {	EM(S_ARITH); (yyval.expr) = GET_TRANSLATOR.Translate_ArithmeticExpression((yyvsp[-3].expr), DeltaIC_DIV, (yyvsp[0].expr)->AdaptIfBool(), "/"); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), DIV, (yyvsp[0].expr));	;}
    break;

  case 131:

    { SM(S_ARITH); PE3(S_EXPR, T_MUL, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 132:

    {	EM(S_ARITH); (yyval.expr) = GET_TRANSLATOR.Translate_ArithmeticExpression((yyvsp[-3].expr), DeltaIC_MUL, (yyvsp[0].expr)->AdaptIfBool(), "*"); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), MUL, (yyvsp[0].expr));	;}
    break;

  case 133:

    { SM(S_ARITH); PE3(S_EXPR, T_MOD, S_EXPR); (yyvsp[-1].expr)->SelfAdaptIfBool(); ;}
    break;

  case 134:

    {	EM(S_ARITH); (yyval.expr) = GET_TRANSLATOR.Translate_ArithmeticExpression((yyvsp[-3].expr), DeltaIC_MOD, (yyvsp[0].expr)->AdaptIfBool(), "%"); 
									Unparse_BinaryOp((yyval.expr), (yyvsp[-3].expr), MOD, (yyvsp[0].expr));	;}
    break;

  case 135:

    {	EM(S_TERNARY); (yyval.expr) = GET_TRANSLATOR.Translate_Ternary((yyvsp[-7].expr), (yyvsp[-5].expr), (yyvsp[-1].expr), (yyvsp[-6].quadNo), (yyvsp[-2].quadNo), (yyvsp[-3].quadNo)); 
							Unparse_TernaryOp((yyval.expr), (yyvsp[-7].expr), (yyvsp[-5].expr), (yyvsp[-1].expr));	;}
    break;

  case 136:

    {	SM(S_TERNARY); PE3(S_EXPR, T_QMARK, S_EXPR); 
							(yyval.expr) = GET_TRANSLATOR.Translate_Condition((yyvsp[-1].expr)); ;}
    break;

  case 137:

    { PE(T_COLON); (yyval.expr) = DNPTR((yyvsp[0].expr))->AdaptIfBool(); ;}
    break;

  case 138:

    { PE(S_EXPR); ;}
    break;

  case 139:

    { PE(T_RPAR); (yyval.expr) = DNPTR((yyvsp[0].expr))->AdaptIfBool(); ;}
    break;

  case 140:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_TermLvalue((yyvsp[-1].expr), true, DeltaIC_ADD); 
					Unparse_UnaryIncDec((yyval.expr), PLUSPLUS, (yyvsp[-1].expr), true);	;}
    break;

  case 141:

    { SM(S_EXPR); PE2(T_PLUSPLUS, S_LVAL); ;}
    break;

  case 142:

    {	EM(S_EXPR); 
					(yyval.expr) = GET_TRANSLATOR.Translate_TermLvalue((yyvsp[0].expr), false, DeltaIC_ADD); 
					Unparse_UnaryIncDec((yyval.expr), PLUSPLUS, (yyvsp[0].expr), false);	;}
    break;

  case 143:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_TermLvalue((yyvsp[-1].expr), true, DeltaIC_SUB); 
					Unparse_UnaryIncDec((yyval.expr), MINUSMINUS, (yyvsp[-1].expr), true);	;}
    break;

  case 144:

    { SM(S_EXPR); PE2(T_MINUSMINUS, S_LVAL); ;}
    break;

  case 145:

    {	EM(S_EXPR); 
					(yyval.expr) = GET_TRANSLATOR.Translate_TermLvalue((yyvsp[0].expr), false, DeltaIC_SUB); 
					Unparse_UnaryIncDec((yyval.expr), MINUSMINUS, (yyvsp[0].expr), false); ;}
    break;

  case 146:

    { SM(S_EXPR); PE2(T_SUB, S_EXPR); ;}
    break;

  case 147:

    { EM(S_EXPR); (yyval.expr) = GET_TRANSLATOR.Translate_UMINUSExpression((yyvsp[0].expr)); Unparse_UnaryOp((yyval.expr), SUB, (yyvsp[0].expr)); ;}
    break;

  case 148:

    { SM(S_EXPR); PE2(T_NOT, S_EXPR); ;}
    break;

  case 149:

    { EM(S_EXPR); (yyval.expr) = GET_TRANSLATOR.Translate_NOTExpression((yyvsp[0].expr)); Unparse_UnaryOp((yyval.expr), NOT, (yyvsp[0].expr)); ;}
    break;

  case 150:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 151:

    { (yyval.expr) = DNPTR((yyvsp[0].expr))->AdaptIfTableContent(); ;}
    break;

  case 152:

    { (yyval.expr) = (yyvsp[0].expr); Unparse_ConstValue((yyval.expr)); ;}
    break;

  case 153:

    { (yyval.expr) = GET_TRANSLATOR.Translate_FunctionExpresssion((yyvsp[0].sym)); ;}
    break;

  case 154:

    { (yyval.expr) = GET_TRANSLATOR.Translate_ConstValue((yyvsp[0].numberConst));	;}
    break;

  case 155:

    { (yyval.expr) = GET_TRANSLATOR.Translate_ConstValue();		;}
    break;

  case 156:

    { (yyval.expr) = GET_TRANSLATOR.Translate_ConstValue(true);	;}
    break;

  case 157:

    { (yyval.expr) = GET_TRANSLATOR.Translate_ConstValue(false);;}
    break;

  case 158:

    { (yyval.id) = "+";		;}
    break;

  case 159:

    { (yyval.id) = "-";		;}
    break;

  case 160:

    { (yyval.id) = "*";		;}
    break;

  case 161:

    { (yyval.id) = "/";		;}
    break;

  case 162:

    { (yyval.id) = "%";		;}
    break;

  case 163:

    { (yyval.id) = ">";		;}
    break;

  case 164:

    { (yyval.id) = "<";		;}
    break;

  case 165:

    { (yyval.id) = "!=";	;}
    break;

  case 166:

    { (yyval.id) = "==";	;}
    break;

  case 167:

    { (yyval.id) = ">=";	;}
    break;

  case 168:

    { (yyval.id) = "<=";	;}
    break;

  case 169:

    { (yyval.id) = "=";		;}
    break;

  case 170:

    { (yyval.id) = "+_";	;}
    break;

  case 171:

    { (yyval.id) = "-_";	;}
    break;

  case 172:

    { (yyval.id) = "*_";	;}
    break;

  case 173:

    { (yyval.id) = "/_";	;}
    break;

  case 174:

    { (yyval.id) = "%_";	;}
    break;

  case 175:

    { (yyval.id) = ">_";	;}
    break;

  case 176:

    { (yyval.id) = "<_";	;}
    break;

  case 177:

    { (yyval.id) = "!=_";	;}
    break;

  case 178:

    { (yyval.id) = "==_";	;}
    break;

  case 179:

    { (yyval.id) = ">=_";	;}
    break;

  case 180:

    { (yyval.id) = "<=_";	;}
    break;

  case 181:

    { (yyval.id) = "()";	;}
    break;

  case 182:

    { (yyval.id) = "=()";	;}
    break;

  case 183:

    { (yyval.id) = "if"; ;}
    break;

  case 184:

    { (yyval.id) = "else"; ;}
    break;

  case 185:

    { (yyval.id) = "function"; ;}
    break;

  case 186:

    { (yyval.id) = "return"; ;}
    break;

  case 187:

    { (yyval.id) = "onevent"; ;}
    break;

  case 188:

    { (yyval.id) = "while"; ;}
    break;

  case 189:

    { (yyval.id) = "for"; ;}
    break;

  case 190:

    { (yyval.id) = "foreach"; ;}
    break;

  case 191:

    { (yyval.id) = "nil"; ;}
    break;

  case 192:

    { (yyval.id) = "local"; ;}
    break;

  case 193:

    { (yyval.id) = "and"; ;}
    break;

  case 194:

    { (yyval.id) = "not"; ;}
    break;

  case 195:

    { (yyval.id) = "or"; ;}
    break;

  case 196:

    { (yyval.id) = "lambda"; ;}
    break;

  case 197:

    { (yyval.id) = "try"; ;}
    break;

  case 198:

    { (yyval.id) = "trap"; ;}
    break;

  case 199:

    { (yyval.id) = "throw"; ;}
    break;

  case 200:

    { (yyval.id) = "using"; ;}
    break;

  case 201:

    { (yyval.id) = "assert"; ;}
    break;

  case 202:

    { (yyval.id) = "true"; ;}
    break;

  case 203:

    { (yyval.id) = "false"; ;}
    break;

  case 204:

    { (yyval.id) = "break"; ;}
    break;

  case 205:

    { (yyval.id) = "continue"; ;}
    break;

  case 206:

    { (yyval.id) = "static"; ;}
    break;

  case 207:

    { (yyval.id) = "const"; ;}
    break;

  case 208:

    { (yyval.id) = "method"; ;}
    break;

  case 209:

    { (yyval.id) = "self"; ;}
    break;

  case 210:

    { (yyval.id) = "arguments"; ;}
    break;

  case 211:

    { (yyval.expr) = GET_TRANSLATOR.Translate_StringConst((yyvsp[0].strConst), (yyvsp[-1].expr)); ;}
    break;

  case 212:

    { (yyval.expr) = GET_TRANSLATOR.Translate_StringConst((yyvsp[0].strConst)); ;}
    break;

  case 213:

    { EI(S_EXPR); PE(T_STRINGIFY); ;}
    break;

  case 214:

    { (yyval.id) = (yyvsp[0].id); ;}
    break;

  case 215:

    { (yyval.id) = (yyvsp[0].id); ;}
    break;

  case 216:

    { PE(T_IDENT); (yyval.expr) = GET_TRANSLATOR.Translate_StringifyDottedIdents((yyvsp[0].id)); ;}
    break;

  case 217:

    { PE2(T_DOT, T_IDENT); ;}
    break;

  case 218:

    { (yyval.expr) = GET_TRANSLATOR.Translate_StringifyDottedIdents((yyvsp[0].id), (yyvsp[-3].expr)); ;}
    break;

  case 219:

    { EG(S_NAMESPACE); PE(T_IDENT); (yyval.expr) = GET_TRANSLATOR.Translate_StringifyNamespaceIdent((yyvsp[0].id)); ;}
    break;

  case 220:

    {  PE(T_STRINGCONST); Unparse_ConstValue((yyval.expr) = (yyvsp[0].expr)); ;}
    break;

  case 221:

    { Unparse_Stringify((yyval.expr) = (yyvsp[0].expr)); ;}
    break;

  case 222:

    { Unparse_Stringify((yyval.expr) = (yyvsp[0].expr)); ;}
    break;

  case 223:

    { EI(S_EXPR); PE(T_SELF); Unparse_BuiltIn((yyval.expr) = GET_TRANSLATOR.Translate_SELF(), SELF); ;}
    break;

  case 224:

    { EI(S_EXPR); PE(T_LAMBDA_REF); Unparse_BuiltIn((yyval.expr) = GET_TRANSLATOR.Translate_LAMBDA_REF(), LAMBDA_REF); ;}
    break;

  case 225:

    { EI(S_EXPR); PE(T_NEWSELF); Unparse_BuiltIn((yyval.expr) = GET_TRANSLATOR.Translate_NEWSELF(), NEWSELF); ;}
    break;

  case 226:

    { EI(S_EXPR); (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 227:

    { EI(S_EXPR); PE(T_ARGUMENTS); (yyval.expr) = GET_TRANSLATOR.Translate_ARGUMENTS(); Unparse_BuiltIn((yyval.expr), ARGUMENTS); ;}
    break;

  case 228:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 229:

    { PE(S_TABLE); (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 230:

    { PE3(T_LPAR, S_EXPR, T_RPAR); ;}
    break;

  case 231:

    { (yyval.expr) = DNPTR((yyvsp[-2].expr))->CheckUninitialised(); Unparse_ParenthesisedExpr((yyval.expr)); ;}
    break;

  case 232:

    { PE(S_CALL); (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 233:

    { SM(S_EXPR); PE3(T_LPAR, S_FUNC_((yyvsp[0].sym)), T_RPAR); ;}
    break;

  case 234:

    {	EM(S_EXPR); (yyval.expr) = GET_TRANSLATOR.Translate_FunctionParenthesisForm((yyvsp[-2].sym)); 
					Unparse_FunctionParenthesisForm((yyval.expr), (yyvsp[-2].sym));	;}
    break;

  case 235:

    { unullify((yyval.expr)); ;}
    break;

  case 236:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 237:

    { PE(T_COMMA); ;}
    break;

  case 238:

    { (yyval.expr) = GET_TRANSLATOR.Translate_ExpressionList((yyvsp[-3].expr), GET_TRANSLATOR.Translate_ExpressionListItem((yyvsp[0].expr))); ;}
    break;

  case 239:

    { (yyval.expr) = GET_TRANSLATOR.Translate_ExpressionList(NIL_EXPR, GET_TRANSLATOR.Translate_ExpressionListItem((yyvsp[0].expr))); ;}
    break;

  case 240:

    { SM(S_CALL); PE2(S_EXPR, T_LPAR); SG(S_AARGS); ;}
    break;

  case 241:

    { PE(T_RPAR); ;}
    break;

  case 242:

    { EG(S_AARGS); EM(S_CALL); (yyval.expr) = (yyvsp[-2].expr); ;}
    break;

  case 243:

    { PE2(T_LPAR, T_RPAR); unullify((yyval.expr)); ;}
    break;

  case 244:

    { PE(S_EXPR); ;}
    break;

  case 245:

    { (yyval.expr) = (yyvsp[0].expr)->AdaptAsArgumentVariable(); ;}
    break;

  case 246:

    { PE2(T_BAR, S_EXPR); ;}
    break;

  case 247:

    { PE(T_BAR); ;}
    break;

  case 248:

    {	(yyval.expr) = (yyvsp[-2].expr)->AdaptAsArgumentVariable(); 
								DNPTR((yyval.expr))->SetLateBound(); 
								Unparse_LateBound((yyval.expr), (yyvsp[-2].expr));	;}
    break;

  case 249:

    { PE(T_TDOT); Unparse_BuiltIn((yyval.expr) = GET_TRANSLATOR.Translate_TRIPLE_DOT(), TRIPLE_DOT); ;}
    break;

  case 250:

    { (yyval.expr) = GET_TRANSLATOR.Translate_FunctionExpresssion((yyvsp[0].sym)); ;}
    break;

  case 251:

    {	UNPARSABLE_GET(Unparse_FunctionCall((yyvsp[-1].expr), (yyvsp[0].expr)));
							(yyval.expr) = GET_TRANSLATOR.Translate_FunctionCall((yyvsp[-1].expr)->CheckUninitialised(), (yyvsp[0].expr)); 
							UNPARSABLE_SET((yyval.expr));		;}
    break;

  case 252:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_ExpressionList((yyvsp[-4].expr), GET_TRANSLATOR.Translate_ExpressionListItem((yyvsp[0].expr))); 
								GET_QUADS.SetQuadLine((yyvsp[-1].quadNo), (yyvsp[-2].intVal));	;}
    break;

  case 253:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_ExpressionList(NIL_EXPR, GET_TRANSLATOR.Translate_ExpressionListItem((yyvsp[0].expr))); 
								GET_QUADS.SetQuadLine((yyvsp[-1].quadNo), (yyvsp[-2].intVal));	;}
    break;

  case 254:

    { EI(S_EXPR); SG(S_NAMESPACE); PE2(T_GLOBAL, T_IDENT); 
						  GET_PARSEPARMS.AppendToNamespacePath(DELTA_LIBRARYNAMESPACE_SEPARATOR); ;}
    break;

  case 255:

    { EI(S_EXPR); SG(S_NAMESPACE); PE3(T_IDENT, T_GLOBAL, T_IDENT); 
						  GET_PARSEPARMS.AppendToNamespacePath((yyvsp[-1].id)); ;}
    break;

  case 256:

    { PE2(T_GLOBAL, T_IDENT); 
						  GET_PARSEPARMS.AppendToNamespacePath((yyvsp[-1].id)); ;}
    break;

  case 257:

    { EI(S_EXPR); PE(T_IDENT); 
						  (yyval.expr) = GET_TRANSLATOR.Translate_Lvalue((yyvsp[0].id)); Unparse_Var((yyval.expr), (yyvsp[0].id)); ;}
    break;

  case 258:

    { EG(S_NAMESPACE); std::string ns;
						  (yyval.expr) = GET_TRANSLATOR.Translate_NamespaceLvalue((yyvsp[0].id), &ns); Unparse_Var((yyval.expr), ns); ;}
    break;

  case 259:

    { (yyval.expr) = GET_TRANSLATOR.Translate_AttrLvalue((yyvsp[0].strConst)); Unparse_Var((yyval.expr), (yyvsp[0].strConst), ATTRIBUTE); ;}
    break;

  case 260:

    { EI(S_EXPR); PE2(T_STATIC, T_IDENT); ;}
    break;

  case 261:

    { (yyval.expr) = GET_TRANSLATOR.Translate_StaticLvalue((yyvsp[0].id)); Unparse_Var((yyval.expr), (yyvsp[0].id), STATIC); ;}
    break;

  case 262:

    { EI(S_EXPR); PE2(T_LOCAL, T_IDENT); ;}
    break;

  case 263:

    { (yyval.expr) = GET_TRANSLATOR.Translate_Lvalue((yyvsp[0].id), 0); Unparse_Var((yyval.expr), (yyvsp[0].id), LOCAL); ;}
    break;

  case 264:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 265:

    { (yyval.expr) = DNPTR((yyvsp[0].expr))->AdaptIfTableContent(); ;}
    break;

  case 266:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 267:

    { PE(T_DOT); SG(S_DOTINDEX); (yyval.expr) = (yyvsp[-1].expr); ;}
    break;

  case 268:

    { PE2(T_LB, S_EXPR); (yyval.expr) = (yyvsp[-1].expr); ;}
    break;

  case 269:

    { PE(T_DDOT); SG(S_DOTINDEX); (yyval.expr) = (yyvsp[-1].expr); ;}
    break;

  case 270:

    { PE2(T_DLB, S_EXPR); (yyval.expr) = (yyvsp[-1].expr); ;}
    break;

  case 271:

    { (yyval.id) = ".=";	;}
    break;

  case 272:

    { (yyval.id) = ".==";	;}
    break;

  case 273:

    { (yyval.id) = ".=()";	;}
    break;

  case 274:

    { (yyval.id) = ".==_";	;}
    break;

  case 275:

    { (yyval.expr) = GET_TRANSLATOR.Translate_StringConst(GET_STRINGHOLDER.StringWithLateDestruction(ucopystr((yyvsp[0].id)))); ;}
    break;

  case 276:

    { (yyval.expr) = GET_TRANSLATOR.Translate_StringConst((yyvsp[0].id)); ;}
    break;

  case 277:

    { (yyval.expr) = GET_TRANSLATOR.Translate_StringConst((yyvsp[0].id)); ;}
    break;

  case 278:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 279:

    { (yyval.expr) = GET_TRANSLATOR.Translate_StringConst((yyvsp[0].id)); ;}
    break;

  case 280:

    {	EG(S_DOTINDEX); PE(S_DOTINDEX); 
							(yyval.expr) = GET_TRANSLATOR.Translate_TableContent((yyvsp[-1].expr)->CheckUninitialised(), (yyvsp[0].expr)); 
							Unparse_TableContentDot((yyval.expr), (yyvsp[-1].expr), (yyvsp[0].expr)->strConst.c_str()); ;}
    break;

  case 281:

    {	EG(S_DOTINDEX); PE(S_DOTINDEX); 
							(yyval.expr) = GET_TRANSLATOR.Translate_TableContent((yyvsp[-1].expr)->CheckUninitialised(), (yyvsp[0].expr)); 
							Unparse_TableContentDot((yyval.expr), (yyvsp[-1].expr), (yyvsp[0].expr)->strConst.c_str()); ;}
    break;

  case 282:

    {	EG(S_DOTINDEX); PE(S_DOTINDEX);
							(yyval.expr) = GET_TRANSLATOR.Translate_BoundedTableContent((yyvsp[-1].expr)->CheckUninitialised(), (yyvsp[0].expr)); 
							Unparse_TableContentDoubleDot((yyval.expr), (yyvsp[-1].expr), (yyvsp[0].expr)->strConst.c_str()); ;}
    break;

  case 283:

    { PE(T_RB); ;}
    break;

  case 284:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_TableContent((yyvsp[-3].expr)->CheckUninitialised(), (yyvsp[-2].expr)->CheckUninitialised()); 
							Unparse_TableContentBracket((yyval.expr), (yyvsp[-3].expr), (yyvsp[-2].expr));	;}
    break;

  case 285:

    { PE(T_RB); ;}
    break;

  case 286:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_TableContent((yyvsp[-3].expr)->CheckUninitialised(), (yyvsp[-2].id)); 
							Unparse_TableContentBracket((yyval.expr), (yyvsp[-3].expr), (yyvsp[-2].id));	;}
    break;

  case 287:

    { PE(T_DRB); ;}
    break;

  case 288:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_BoundedTableContent((yyvsp[-3].expr)->CheckUninitialised(), (yyvsp[-2].expr)->CheckUninitialised()); 
							Unparse_TableContentDoubleBracket((yyval.expr), (yyvsp[-3].expr), (yyvsp[-2].expr));	;}
    break;

  case 289:

    { PE(T_DRB); ;}
    break;

  case 290:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_BoundedTableContent((yyvsp[-3].expr)->CheckUninitialised(), (yyvsp[-2].id)); 
							Unparse_TableContentDoubleBracket((yyval.expr), (yyvsp[-3].expr), (yyvsp[-2].id));	;}
    break;

  case 291:

    {	SM(S_TABLE); PE2(T_LB, S_TABLIST); 
							GET_PARSEPARMS.InTableExpr().enter(); (yyval.expr) = GET_TRANSLATOR.Translate_TablePrefix(); ;}
    break;

  case 292:

    { PE(T_RB); ;}
    break;

  case 293:

    { EM(S_TABLE); ;}
    break;

  case 294:

    {	GET_PARSEPARMS.InTableExpr().exit();
								UNPARSABLE_GET(Unparse_TableConstructor((yyvsp[-1].table))); 
								(yyval.expr) = GET_TRANSLATOR.Translate_TableConstructor((yyvsp[-2].expr), (yyvsp[-1].table)); 
								UNPARSABLE_SET((yyval.expr)); ;}
    break;

  case 295:

    {	GET_PARSEPARMS.InTableExpr().exit();
								UNPARSABLE_GET(Unparse_TableConstructor()); 
								(yyval.expr) = GET_TRANSLATOR.Translate_TableConstructor((yyvsp[-1].expr)); 
								UNPARSABLE_SET((yyval.expr)); ;}
    break;

  case 296:

    { PE(T_COMMA); ;}
    break;

  case 297:

    {	UNPARSABLE_GET(Unparse_TableElements((yyvsp[-3].table), (yyvsp[0].table)));
								(yyval.table) = GET_TRANSLATOR.Translate_TableElements((yyvsp[-3].table), (yyvsp[0].table)); 
								UNPARSABLE_SET((yyval.table)); ;}
    break;

  case 298:

    { (yyval.table) = (yyvsp[0].table); ;}
    break;

  case 299:

    { PE(S_FUNC_((yyvsp[0].sym))); (yyval.expr) = GET_TRANSLATOR.Translate_FunctionExpresssion((yyvsp[0].sym)); ;}
    break;

  case 300:

    { (yyval.table) = GET_TRANSLATOR.Translate_UnindexedValue((yyvsp[-1].expr), (yyvsp[-3].quadNo), (yyvsp[0].intVal)); ;}
    break;

  case 301:

    { (yyval.table) = GET_TRANSLATOR.Translate_UnindexedValue((yyvsp[-1].expr), (yyvsp[-3].quadNo), (yyvsp[0].intVal)); ;}
    break;

  case 302:

    { OE(T_TABLE_ELEM); (yyval.quadNo) = (yyvsp[0].quadNo); ;}
    break;

  case 303:

    { (yyval.table) = (yyvsp[0].table); ;}
    break;

  case 304:

    { Unparse_IndexedValues((yyval.table) = (yyvsp[0].table)); ;}
    break;

  case 305:

    { Unparse_UnindexedValue((yyval.table) = GET_TRANSLATOR.Translate_TableElement((yyvsp[0].expr)), (yyvsp[0].expr)); ;}
    break;

  case 306:

    { (yyval.table) = (yyvsp[0].table); ;}
    break;

  case 307:

    { PE2(T_DOT, T_IDENT); ;}
    break;

  case 308:

    { Unparse_DottedIdent((yyval.expr) = GET_TRANSLATOR.Translate_ConstValue((yyvsp[0].id)), (yyvsp[0].id)); ;}
    break;

  case 309:

    {	PE(T_OPINDEX); 
							(yyval.expr) = GET_TRANSLATOR.Translate_ConstValue((yyvsp[0].id)); 
							Unparse_OperatorIndex((yyval.expr), (yyvsp[0].id));	;}
    break;

  case 310:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 311:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 312:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 313:

    { PE(S_INDEXEXPR); ;}
    break;

  case 314:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 315:

    { PE(T_COMMA); ;}
    break;

  case 316:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_ExpressionList((yyvsp[-5].expr), GET_TRANSLATOR.Translate_ExpressionListItem((yyvsp[-1].expr)->AdaptAsArgumentVariable()));
								GET_QUADS.SetQuadLine((yyvsp[-2].quadNo), (yyvsp[0].intVal));	;}
    break;

  case 317:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_ExpressionList(NIL_EXPR, GET_TRANSLATOR.Translate_ExpressionListItem((yyvsp[-1].expr)->AdaptAsArgumentVariable()));
								GET_QUADS.SetQuadLine((yyvsp[-2].quadNo), (yyvsp[0].intVal));	;}
    break;

  case 318:

    { PE(T_LBC); SG(N_EMPTY); ;}
    break;

  case 319:

    { EG(N_EMPTY); PE2(S_INDICES, T_COLON); ;}
    break;

  case 320:

    { SG(S_ELIST); ;}
    break;

  case 321:

    { EG(S_ELIST); PE2(S_ELIST, T_RBC); ;}
    break;

  case 322:

    { (yyval.table) = GET_TRANSLATOR.Translate_IndexedValues((yyvsp[-6].expr), (yyvsp[-2].expr)); ;}
    break;

  case 323:

    { (yyval.strConst) = (yyvsp[0].strConst); ;}
    break;

  case 324:

    { PE(T_DOT); OE(T_IDENT_OR_KWDIDENT); ;}
    break;

  case 325:

    { (yyval.strConst) = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr((yyvsp[0].id))); ;}
    break;

  case 326:

    { PE(T_COLON); SG(S_EXPR); ;}
    break;

  case 327:

    { EG(S_EXPR); PE(S_EXPR); (yyval.table) = GET_TRANSLATOR.Translate_IdentIndexElement((yyvsp[-5].strConst), (yyvsp[-1].expr), (yyvsp[-2].quadNo), (yyvsp[0].intVal)); ;}
    break;

  case 328:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_ExpressionList((yyvsp[-4].expr), GET_TRANSLATOR.Translate_ExpressionListItem((yyvsp[0].expr)));
								GET_QUADS.SetQuadLine((yyvsp[-1].quadNo), (yyvsp[-2].intVal));	;}
    break;

  case 329:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_ExpressionList(NIL_EXPR, GET_TRANSLATOR.Translate_ExpressionListItem((yyvsp[0].expr)));
								GET_QUADS.SetQuadLine((yyvsp[-1].quadNo), (yyvsp[-2].intVal));	;}
    break;

  case 330:

    { (yyval.expr) = (yyvsp[0].expr); ;}
    break;

  case 331:

    { PE(S_FUNC_((yyvsp[0].sym))); (yyval.expr) = GET_TRANSLATOR.Translate_FunctionExpresssion((yyvsp[0].sym)); ;}
    break;

  case 332:

    { PE(T_IDENT); (yyval.id) = (yyvsp[0].id); ;}
    break;

  case 333:

    { PE(T_KWDIDENT); (yyval.id) = (yyvsp[0].id); ;}
    break;

  case 334:

    {	PE(T_ATTRIBUTEID); OE(T_LBC_OR_COLON); 
								(yyval.strConst) = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr((yyvsp[0].id) + 1)); ;}
    break;

  case 335:

    { PE(T_ATTRIBUTE); OE(T_IDENT_OR_KWDIDENT); ;}
    break;

  case 336:

    { (yyval.strConst) = GET_STRINGHOLDER.StringWithLateDestruction(ucopystr((yyvsp[0].id))); ;}
    break;

  case 337:

    { (yyval.expr) = GET_TRANSLATOR.Translate_NewAttribute((yyvsp[-2].strConst), (yyvsp[-1].expr), (yyvsp[0].expr)); ;}
    break;

  case 338:

    { PE2(T_LBC, T_SET);  ;}
    break;

  case 339:

    { SG(S_EXPR); ;}
    break;

  case 340:

    {	EG(S_EXPR); 
						PE2(S_EXPR, T_GET); 
						(yyval.expr) = GET_TRANSLATOR.Translate_AttributeMethod((yyvsp[-1].expr), (yyvsp[-2].quadNo), (yyvsp[0].intVal)); ;}
    break;

  case 341:

    { SG(S_EXPR); ;}
    break;

  case 342:

    { EG(S_EXPR); PE2(S_EXPR, T_RBC); ;}
    break;

  case 343:

    {	 
						(yyval.expr) = GET_TRANSLATOR.Translate_AttributeMethod((yyvsp[-3].expr), (yyvsp[-4].quadNo), (yyvsp[-2].intVal)); ;}
    break;

  case 344:

    { SM(S_STMT); PE(T_BREAK);	;}
    break;

  case 346:

    { SM(S_STMT); PE(T_CONT);	;}
    break;

  case 348:

    { SG(S_EXPR); ;}
    break;

  case 349:

    { EG(S_EXPR); PE2(S_EXPR, T_RPAR); ;}
    break;

  case 350:

    {  (yyval.expr) = GET_TRANSLATOR.Translate_Condition((yyvsp[-2].expr)); GET_TRANSLATOR.Translate_BasicStmt((yyvsp[-3].intVal));	;}
    break;

  case 351:

    { (yyval.quadNo) = GET_TRANSLATOR.Translate_N(); ;}
    break;

  case 352:

    { SM(S_WHILE); PE2(T_WHILE, T_LPAR); GET_TRANSLATOR.Translate_WhilePrefix(); ;}
    break;

  case 353:

    {	EM(S_WHILE); 
						UNPARSABLE_GET(Unparse_While((yyvsp[-3].expr), (yyvsp[0].stmt)));
						(yyval.stmt) = GET_TRANSLATOR.Translate_WhileStmt((yyvsp[-3].expr), (yyvsp[-4].quadNo), (yyvsp[-1].quadNo), (yyvsp[0].stmt), (yyvsp[-2].intVal)); 
						UNPARSABLE_SET((yyval.stmt)); ;}
    break;

  case 354:

    { SM(S_IF); PE2(T_IF, T_LPAR);  ;}
    break;

  case 355:

    { EM(S_IF); SM(S_ELSE); PE(T_ELSE); GET_TRANSLATOR.Translate_ElseStmtPrefix(); ;}
    break;

  case 356:

    { GET_TRANSLATOR.Translate_IfStmtPrefix(); (yyval.quadNo) = (yyvsp[0].quadNo); ;}
    break;

  case 357:

    {	EM(S_IF);
							GET_TRANSLATOR.Translate_IfStmt((yyvsp[-2].expr), (yyvsp[-1].quadNo));
							Unparse_If((yyval.stmt) = (yyvsp[0].stmt), (yyvsp[-2].expr), (yyvsp[0].stmt)); ;}
    break;

  case 358:

    {	EM(S_ELSE);
							GET_TRANSLATOR.Translate_IfElseStmt((yyvsp[-6].expr), (yyvsp[-5].quadNo), (yyvsp[-2].quadNo), (yyvsp[-1].quadNo));
							UNPARSABLE_GET(Unparse_IfElse((yyvsp[-6].expr), (yyvsp[-4].stmt), (yyvsp[0].stmt)));
							GET_DELTASYMBOLS.ResetTemp();
							(yyval.stmt) = GET_TRANSLATOR.Translate_Stmts((yyvsp[-4].stmt), (yyvsp[0].stmt)); 
							UNPARSABLE_SET((yyval.stmt));	;}
    break;

  case 359:

    {	EM(S_FOR); 
						UNPARSABLE_GET(Unparse_For((yyvsp[-7].stmt), (yyvsp[-5].expr), (yyvsp[-3].stmt), (yyvsp[0].stmt)));
						(yyval.stmt) = GET_TRANSLATOR.Translate_ForStmt((yyvsp[-5].expr), (yyvsp[-6].quadNo), (yyvsp[-4].quadNo), (yyvsp[-1].quadNo), (yyvsp[-2].quadNo), (yyvsp[0].stmt)); 
						UNPARSABLE_SET((yyval.stmt)); ;}
    break;

  case 360:

    { SM(S_FOR); PE2(T_FOR, T_LPAR); ;}
    break;

  case 361:

    { PE(S_ELIST); GET_TRANSLATOR.Translate_ForOpening(); ;}
    break;

  case 362:

    { GET_TRANSLATOR.Translate_ForPrefix((yyvsp[-2].quadNo), (yyvsp[-1].intVal));  (yyval.stmt) = (yyvsp[0].stmt); GET_TRANSLATOR.Translate_BasicStmt((yyvsp[-1].intVal)); ;}
    break;

  case 363:

    { (yyval.stmt) = NEW_STMT; Unparse_ForInitList((yyval.stmt), (yyvsp[-1].expr)); ;}
    break;

  case 364:

    { EI(S_ELIST); (yyval.stmt) = NEW_STMT; Unparse_ForInitList((yyval.stmt)); ;}
    break;

  case 365:

    { PE(S_EXPR); ;}
    break;

  case 366:

    {	(yyval.expr) = GET_TRANSLATOR.Translate_Condition((yyvsp[-2].expr)); 
						GET_QUADS.SetQuadLine((yyvsp[-3].quadNo), (yyvsp[-1].intVal), true); 
						GET_TRANSLATOR.Translate_BasicStmt((yyvsp[-1].intVal)); ;}
    break;

  case 367:

    { PE(S_ELIST); ;}
    break;

  case 368:

    {	(yyval.stmt) = NEW_STMT; 
							UNPARSABLE_GET(Unparse_ExprList((yyvsp[-1].expr)));
							UNPARSABLE_SET((yyval.stmt)); 
							GET_TRANSLATOR.Translate_BasicStmt((yyvsp[0].intVal)); ;}
    break;

  case 369:

    { (yyval.stmt) = NEW_STMT; ;}
    break;

  case 370:

    { PE(T_RPAR); ;}
    break;

  case 371:

    { (yyval.intVal) = (yyvsp[-1].intVal); ;}
    break;

  case 372:

    {  GET_QUADS.SetQuadLine((yyvsp[-1].quadNo), (yyvsp[-2].intVal), true);  (yyval.stmt) = (yyvsp[0].stmt); ;}
    break;

  case 373:

    {	PE(S_LVAL);		;}
    break;

  case 374:

    {	PE(T_COMMA);	;}
    break;

  case 375:

    {	(yyval.expr) = (yyvsp[-2].expr);		;}
    break;

  case 376:

    {	unullify((yyval.expr));	;}
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

    {	UNPARSABLE_GET(Unparse_ForeachPrefix((yyvsp[-7].expr), (yyvsp[-5].expr), (yyvsp[-1].expr))); 
											if ((yyvsp[-5].expr))
												(yyval.stmt) = GET_TRANSLATOR.Translate_ForeachPrefix((yyvsp[-5].expr), (yyvsp[-7].expr), (yyvsp[-1].expr)); 
											else
												(yyval.stmt) = GET_TRANSLATOR.Translate_ForeachPrefix((yyvsp[-7].expr), NIL_EXPR, (yyvsp[-1].expr)); 
											GET_QUADS.SetQuadLine((yyvsp[-2].quadNo), (yyvsp[-3].intVal));
											UNPARSABLE_SET((yyval.stmt));		;}
    break;

  case 382:

    {	PE(S_STMT);		;}
    break;

  case 383:

    {	UNPARSABLE_GET(Unparse_ForeachStmt((yyvsp[-3].stmt), (yyvsp[0].stmt)));
											(yyval.stmt) = GET_TRANSLATOR.Translate_ForeachStmt((yyvsp[-3].stmt), (yyvsp[0].stmt), (yyvsp[-2].quadNo));
											UNPARSABLE_SET((yyval.stmt));	
											EM(S_FOREACH);	;}
    break;

  case 384:

    { SM(S_THROW); PE2(T_THROW, S_EXPR); ;}
    break;

  case 385:

    {	EM(S_THROW); 
						(yyval.stmt) = GET_TRANSLATOR.Translate_THROW((yyvsp[-1].expr)); 
						Unparse_ExprStmt((yyval.stmt), THROW, (yyvsp[-1].expr)); ;}
    break;

  case 386:

    { SM(S_TRY); PE2(T_TRY, S_STMT); (yyval.quadNo) = GET_TRANSLATOR.Translate_TRY(); ;}
    break;

  case 387:

    { PE(T_IDENT); (yyval.quadNo) = GET_TRANSLATOR.Translate_TRAP(); ;}
    break;

  case 388:

    { (yyval.expr) = GET_TRANSLATOR.Translate_Lvalue((yyvsp[0].id)); if ((yyval.expr)) DPTR((yyval.expr))->SetInitialised(); ;}
    break;

  case 389:

    { (yyval.quadNo) = GET_TRANSLATOR.Translate_TrapJumpOver(); ;}
    break;

  case 390:

    { PE(T_TRAP); ;}
    break;

  case 391:

    { GET_TRANSLATOR.Translate_TrapStart((yyvsp[-5].quadNo), (yyvsp[-2].quadNo), (yyvsp[0].expr)); ;}
    break;

  case 392:

    {	EM(S_TRY);
						GET_TRANSLATOR.Translate_TrapEnd((yyvsp[-3].quadNo));
						UNPARSABLE_GET(Unparse_TryTrap((yyvsp[-6].stmt), (yyvsp[0].stmt), (yyvsp[-2].expr)));
						GET_DELTASYMBOLS.ResetTemp();
						(yyval.stmt) = GET_TRANSLATOR.Translate_Stmts((yyvsp[-6].stmt), (yyvsp[0].stmt)); 
						UNPARSABLE_SET((yyval.stmt));		;}
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





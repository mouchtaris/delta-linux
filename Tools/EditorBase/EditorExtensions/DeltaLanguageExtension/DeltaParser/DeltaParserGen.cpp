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



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_FIRST = 258,
     FUNCTION = 259,
     RETURN = 260,
     ASSERT = 261,
     LAMBDA = 262,
     LAMBDA_REF = 263,
     ONEVENT = 264,
     METHOD = 265,
     SELF = 266,
     ARGUMENTS = 267,
     STATIC = 268,
     CONST = 269,
     KWD_IF = 270,
     ELSE = 271,
     WHILE = 272,
     FOR = 273,
     FOREACH = 274,
     NIL = 275,
     AND = 276,
     NOT = 277,
     OR = 278,
     TRUE = 279,
     FALSE = 280,
     BREAK = 281,
     CONTINUE = 282,
     LOCAL = 283,
     TRY = 284,
     TRAP = 285,
     THROW = 286,
     USING = 287,
     ADD = 288,
     SUB = 289,
     MUL = 290,
     DIV = 291,
     MOD = 292,
     GT = 293,
     LT = 294,
     NE = 295,
     EQ = 296,
     GE = 297,
     LE = 298,
     ASSIGN = 299,
     ADD_POSTFIX = 300,
     SUB_POSTFIX = 301,
     MUL_POSTFIX = 302,
     DIV_POSTFIX = 303,
     MOD_POSTFIX = 304,
     GT_POSTFIX = 305,
     LT_POSTFIX = 306,
     NE_POSTFIX = 307,
     EQ_POSTFIX = 308,
     GE_POSTFIX = 309,
     LE_POSTFIX = 310,
     CAST = 311,
     CALL = 312,
     GLOBAL_SCOPE = 313,
     PLUSPLUS = 314,
     MINUSMINUS = 315,
     DOT = 316,
     DOT_ASSIGN = 317,
     DOUBLE_DOT = 318,
     TRIPLE_DOT = 319,
     DOUBLE_LB = 320,
     DOUBLE_RB = 321,
     ADD_A = 322,
     SUB_A = 323,
     MUL_A = 324,
     DIV_A = 325,
     MOD_A = 326,
     SET = 327,
     GET = 328,
     ATTRIBUTE = 329,
     STRINGIFICATION = 330,
     NEWSELF = 331,
     DOT_EQUAL = 332,
     DOT_CAST = 333,
     DOT_EQUAL_RHS = 334,
     NUMBER_CONST = 335,
     STRING_CONST = 336,
     IDENT = 337,
     ATTRIBUTE_IDENT = 338,
     OPERATOR = 339,
     PARENTHESIS = 340,
     SQUARE_BRACKETS = 341,
     COMMENT = 342,
     INCOMPLETE_COMMENT = 343,
     INCOMPLETE_STRING = 344,
     CPP = 345,
     PARSE_STMT = 346,
     PARSE_FUNCTION_CALL_OBJECT = 347,
     PARSE_TABLE_OBJECT = 348,
     PARSE_NAMESPACE = 349,
     TOKEN_LAST = 350,
     UMINUS = 351
   };
#endif
/* Tokens.  */
#define TOKEN_FIRST 258
#define FUNCTION 259
#define RETURN 260
#define ASSERT 261
#define LAMBDA 262
#define LAMBDA_REF 263
#define ONEVENT 264
#define METHOD 265
#define SELF 266
#define ARGUMENTS 267
#define STATIC 268
#define CONST 269
#define KWD_IF 270
#define ELSE 271
#define WHILE 272
#define FOR 273
#define FOREACH 274
#define NIL 275
#define AND 276
#define NOT 277
#define OR 278
#define TRUE 279
#define FALSE 280
#define BREAK 281
#define CONTINUE 282
#define LOCAL 283
#define TRY 284
#define TRAP 285
#define THROW 286
#define USING 287
#define ADD 288
#define SUB 289
#define MUL 290
#define DIV 291
#define MOD 292
#define GT 293
#define LT 294
#define NE 295
#define EQ 296
#define GE 297
#define LE 298
#define ASSIGN 299
#define ADD_POSTFIX 300
#define SUB_POSTFIX 301
#define MUL_POSTFIX 302
#define DIV_POSTFIX 303
#define MOD_POSTFIX 304
#define GT_POSTFIX 305
#define LT_POSTFIX 306
#define NE_POSTFIX 307
#define EQ_POSTFIX 308
#define GE_POSTFIX 309
#define LE_POSTFIX 310
#define CAST 311
#define CALL 312
#define GLOBAL_SCOPE 313
#define PLUSPLUS 314
#define MINUSMINUS 315
#define DOT 316
#define DOT_ASSIGN 317
#define DOUBLE_DOT 318
#define TRIPLE_DOT 319
#define DOUBLE_LB 320
#define DOUBLE_RB 321
#define ADD_A 322
#define SUB_A 323
#define MUL_A 324
#define DIV_A 325
#define MOD_A 326
#define SET 327
#define GET 328
#define ATTRIBUTE 329
#define STRINGIFICATION 330
#define NEWSELF 331
#define DOT_EQUAL 332
#define DOT_CAST 333
#define DOT_EQUAL_RHS 334
#define NUMBER_CONST 335
#define STRING_CONST 336
#define IDENT 337
#define ATTRIBUTE_IDENT 338
#define OPERATOR 339
#define PARENTHESIS 340
#define SQUARE_BRACKETS 341
#define COMMENT 342
#define INCOMPLETE_COMMENT 343
#define INCOMPLETE_STRING 344
#define CPP 345
#define PARSE_STMT 346
#define PARSE_FUNCTION_CALL_OBJECT 347
#define PARSE_TABLE_OBJECT 348
#define PARSE_NAMESPACE 349
#define TOKEN_LAST 350
#define UMINUS 351




/* Copy the first part of user declarations.  */
#line 1 "DeltaParserSpec.y"

/**
 *	DeltaParserSpec.y
 *
 * Delta parser, syntax directed parsing file, using Bison parser generator.
 * Based on Anthony Savidis' (as@ics.forth.gr) Delta Parser
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 *  Changes, addition and extensions by JL and latter by AS.
 *  Extension to support selective parsing of partial code segments, 
 *  Anthony Savidis, July 2010.
 */

#include "Common.h"
#include "Streams.h"
#include "DeltaParserCommon.h"
#include "ProgramDescription.h"
#include "DeltaAST.h"
#include "DeltaASTUtil.h"
#include "DeltaParserGen.hpp"

#include "FlexLexer.h"

////////////////////////////////////////////////////////////////////////

using namespace delta;

extern int yylex (YYSTYPE* yylval, YYLTYPE* yylloc, ParsingCtx& ctx);

////////////////////////////////////////////////////////////////////////

void yyerror (YYLTYPE* yylloc, ParsingCtx& ctx, char* msg)
{
	DBGOUT << msg << " at line: " << yylloc->first_line << ", at pos: (" << yylloc->first_column <<
		"), before: \"" << ctx.GetLexer().YYText() << "\"" << DBGENDL;

	const char* unexpectedEndMsg = msg + strlen(msg) - 4;
	if (!strcmp(unexpectedEndMsg, "$end"))
		return;

	ctx.GetProgramDesc().AddParseError(
			Range(yylloc->first_column, yylloc->last_column), util::std2str(msg)
		);
}

////////////////////////////////////////////////////////////////////////

DeltaASTNode* ExpressionToConstKey (YYLTYPE& pos, DeltaASTNode* node)
{
	if (node) {
		if (node->GetType() == ConstASTNode::Type) {
			ConstASTNode* constNode = static_cast<ConstASTNode*>(node);
			TableConstKeyType type;
			if (constNode->GetValue() == CONST_NUMBER)
				type = CONST_KEY_NUMBER;
			else if (constNode->GetValue() == CONST_STRING)
				type = CONST_KEY_STRING;
			else
				assert(false);

			delete node;
			return CreateNode<TableConstKeyASTNode>(pos, pos, type);
		}
		else if (node->GetType() == StringifiedIdASTNode::Type) {
			delete node;
			return CreateNode<TableConstKeyASTNode>(pos, pos, CONST_KEY_STRINGIFIED_ID);
		}
	}
	return node;
}

//**********************************************************************

#define YYERROR_VERBOSE 1

////////////////////////////////////////////////////////////////////////



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
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
#line 89 "DeltaParserSpec.y"
typedef union YYSTYPE {
	const char*						literalValue;

	delta::DeltaASTNode*			nodeValue;
	delta::StmtsASTNode*			stmtsValue;
	
	delta::ExpressionListASTNode*	exprListValue;
	delta::ArgListASTNode*			argListValue;

	delta::UnaryKwdASTNode*			unaryKwdValue;
	delta::LeafKwdASTNode*			leafKwdValue;

	delta::WhileASTNode*			whileValue;
	delta::ForASTNode*				forValue;
	delta::ForeachASTNode*			foreachValue;
	delta::IfASTNode*				ifValue;
	delta::ElseASTNode*				elseValue;
	delta::CompoundASTNode*			compValue;
	delta::TryASTNode*				tryValue;
	delta::TrapASTNode*				trapValue;

	delta::FunctionASTNode*			funcValue;
	delta::FunctionNameASTNode*		funcNameValue;

	delta::TernaryOpASTNode*		ternaryOpValue;
	delta::PrefixOpASTNode*			prefixOpValue;
	delta::SuffixOpASTNode*			suffixOpValue;
	delta::BinaryOpASTNode*			binaryOpValue;
	delta::UnaryOpASTNode*			unaryOpValue;

	delta::CallASTNode*				callValue;

	delta::StringifiedIdASTNode*	stringifiedValue;
	delta::AttributeASTNode*		attributeValue;
	delta::VariableASTNode*			varValue;
	delta::ConstASTNode*			constValue;
	delta::ArgASTNode*				argValue;

	delta::NewAttributeASTNode*		newAttributeValue;
	delta::TableElemASTNode*		tableElemValue;
	delta::TableElemsASTNode*		tableElemsValue;
	delta::TableIndexListASTNode*	tableIListVlaue;
	delta::TableConstructASTNode*	tableCtorValue;
	delta::UsingASTNode*			usingValue;
} YYSTYPE;
/* Line 196 of yacc.c.  */
#line 403 "DeltaParserGen.cpp"
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
#line 427 "DeltaParserGen.cpp"

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
#define YYFINAL  171
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1741

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  108
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  80
/* YYNRULES -- Number of rules. */
#define YYNRULES  262
/* YYNRULES -- Number of states. */
#define YYNSTATES  400

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   351

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      80,    81,     2,     2,    88,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    87,   106,
       2,     2,     2,    86,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    82,     2,    83,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    84,   107,    85,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     6,    10,    14,    18,    22,    23,
      25,    27,    29,    32,    35,    37,    40,    42,    44,    46,
      48,    50,    51,    53,    56,    58,    59,    61,    63,    65,
      69,    72,    74,    75,    77,    80,    84,    86,    92,    96,
     100,   102,   105,   108,   110,   113,   117,   123,   125,   127,
     130,   133,   136,   138,   140,   142,   144,   146,   148,   150,
     153,   155,   157,   159,   161,   163,   165,   167,   171,   174,
     178,   184,   191,   194,   195,   205,   207,   208,   210,   211,
     215,   217,   225,   229,   232,   235,   239,   243,   247,   251,
     256,   258,   260,   264,   266,   268,   271,   273,   275,   277,
     279,   281,   283,   285,   290,   294,   298,   302,   306,   310,
     314,   318,   322,   326,   330,   334,   338,   342,   346,   350,
     354,   358,   362,   366,   374,   377,   380,   383,   386,   389,
     392,   394,   396,   398,   400,   402,   404,   406,   410,   414,
     416,   418,   420,   422,   424,   426,   428,   430,   432,   435,
     437,   439,   441,   443,   445,   447,   449,   451,   453,   455,
     457,   459,   461,   463,   465,   467,   469,   471,   473,   475,
     477,   479,   481,   483,   485,   487,   489,   491,   493,   495,
     497,   500,   504,   507,   510,   512,   514,   516,   518,   520,
     522,   524,   526,   528,   530,   532,   534,   536,   538,   540,
     542,   544,   546,   548,   550,   552,   554,   556,   558,   560,
     562,   564,   566,   568,   572,   574,   576,   580,   582,   584,
     589,   593,   596,   600,   602,   604,   606,   609,   612,   614,
     616,   618,   620,   622,   624,   626,   628,   630,   632,   636,
     639,   643,   648,   653,   658,   663,   667,   670,   674,   676,
     678,   680,   686,   688,   690,   694,   696,   698,   700,   702,
     705,   707,   711
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     110,     0,    -1,    -1,   111,    -1,   100,   109,   132,    -1,
     101,   109,   162,    -1,   102,   109,   162,    -1,   103,   109,
     149,    -1,    -1,   114,    -1,   112,    -1,   113,    -1,   112,
     132,    -1,   113,   148,    -1,   148,    -1,   114,   132,    -1,
     132,    -1,     4,    -1,     9,    -1,    10,    -1,    28,    -1,
      -1,    91,    -1,    93,   118,    -1,    74,    -1,    -1,   170,
      -1,    61,    -1,    62,    -1,    80,   121,   120,    -1,    80,
     120,    -1,    57,    -1,    -1,    81,    -1,    64,    81,    -1,
     121,    88,    91,    -1,    91,    -1,   115,   116,   117,   119,
     134,    -1,     7,   119,   124,    -1,    84,   153,    85,    -1,
      92,    -1,    74,    91,    -1,    74,   165,    -1,   125,    -1,
      61,   166,    -1,   126,    87,   129,    -1,   125,    84,   130,
     131,    85,    -1,   153,    -1,   122,    -1,    72,   129,    -1,
      73,   129,    -1,   175,   106,    -1,   135,    -1,   136,    -1,
     139,    -1,   143,    -1,   137,    -1,   144,    -1,   134,    -1,
     133,   106,    -1,   146,    -1,   145,    -1,   122,    -1,   106,
      -1,     1,    -1,    26,    -1,    27,    -1,    84,   114,    85,
      -1,    84,    85,    -1,     6,   153,   106,    -1,    17,    80,
     153,    81,   132,    -1,    15,    80,   153,    81,   132,   138,
      -1,    16,   132,    -1,    -1,    18,    80,   140,   106,   153,
     106,   141,    81,   132,    -1,   175,    -1,    -1,   175,    -1,
      -1,    87,   176,    88,    -1,    88,    -1,    19,    80,   176,
     142,   153,    81,   132,    -1,     5,   153,   106,    -1,     5,
     106,    -1,     5,   122,    -1,    31,   153,   106,    -1,    29,
     132,   147,    -1,    30,    91,   132,    -1,    32,   149,   106,
      -1,    32,    75,    91,   106,    -1,   151,    -1,   152,    -1,
     151,    58,    91,    -1,   150,    -1,    91,    -1,    58,   151,
      -1,   155,    -1,   154,    -1,   156,    -1,   157,    -1,   158,
      -1,   159,    -1,   160,    -1,    14,    91,    44,   153,    -1,
     176,    44,   153,    -1,   176,    67,   153,    -1,   176,    69,
     153,    -1,   176,    68,   153,    -1,   176,    70,   153,    -1,
     176,    71,   153,    -1,   153,    38,   153,    -1,   153,    39,
     153,    -1,   153,    42,   153,    -1,   153,    43,   153,    -1,
     153,    41,   153,    -1,   153,    40,   153,    -1,   153,    21,
     153,    -1,   153,    23,   153,    -1,   153,    33,   153,    -1,
     153,    34,   153,    -1,   153,    36,   153,    -1,   153,    35,
     153,    -1,   153,    37,   153,    -1,    80,   153,    86,   153,
      87,   153,    81,    -1,   176,    59,    -1,    59,   176,    -1,
     176,    60,    -1,    60,   176,    -1,    34,   153,    -1,    22,
     153,    -1,   161,    -1,   162,    -1,   163,    -1,   123,    -1,
     176,    -1,   174,    -1,   181,    -1,    80,   122,    81,    -1,
      80,   153,    81,    -1,   169,    -1,    11,    -1,    12,    -1,
       8,    -1,    76,    -1,    89,    -1,    20,    -1,    24,    -1,
      25,    -1,   164,    90,    -1,    90,    -1,    15,    -1,    16,
      -1,     4,    -1,     5,    -1,     9,    -1,    17,    -1,    18,
      -1,    19,    -1,    20,    -1,    28,    -1,    21,    -1,    22,
      -1,    23,    -1,     7,    -1,    29,    -1,    30,    -1,    31,
      -1,    32,    -1,     6,    -1,    24,    -1,    25,    -1,    26,
      -1,    27,    -1,    13,    -1,    14,    -1,    10,    -1,    11,
      -1,    12,    -1,    91,    -1,   165,    -1,    75,   166,    -1,
     167,    61,   166,    -1,    75,   150,    -1,    75,   152,    -1,
     164,    -1,   167,    -1,   168,    -1,    33,    -1,    34,    -1,
      35,    -1,    36,    -1,    37,    -1,    38,    -1,    39,    -1,
      40,    -1,    41,    -1,    42,    -1,    43,    -1,    44,    -1,
      45,    -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,
      50,    -1,    51,    -1,    52,    -1,    53,    -1,    54,    -1,
      55,    -1,    57,    -1,    56,    -1,   162,    -1,   172,    88,
     173,    -1,   173,    -1,   153,    -1,   107,   153,   107,    -1,
      64,    -1,   122,    -1,   171,    80,   172,    81,    -1,   171,
      80,    81,    -1,   171,    57,    -1,   175,    88,   153,    -1,
     153,    -1,   149,    -1,    92,    -1,    13,    91,    -1,    28,
      91,    -1,   180,    -1,   162,    -1,    91,    -1,   165,    -1,
     170,    -1,   169,    -1,    62,    -1,    77,    -1,    78,    -1,
      79,    -1,   177,    61,   178,    -1,   177,   179,    -1,   177,
      63,   178,    -1,   177,    82,   153,    83,    -1,   177,    82,
     118,    83,    -1,   177,    65,   153,    66,    -1,   177,    65,
     118,    66,    -1,    82,   182,    83,    -1,    82,    83,    -1,
     182,    88,   183,    -1,   183,    -1,   153,    -1,   122,    -1,
      84,   187,    87,   184,    85,    -1,   128,    -1,   127,    -1,
     184,    88,   185,    -1,   185,    -1,   153,    -1,   122,    -1,
     153,    -1,    61,   166,    -1,   170,    -1,   187,    88,   186,
      -1,   186,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   268,   268,   277,   279,   280,   281,   282,   285,   286,
     287,   290,   292,   299,   305,   312,   317,   326,   327,   328,
     331,   332,   335,   336,   337,   338,   341,   342,   343,   346,
     353,   363,   364,   367,   368,   372,   377,   383,   392,   400,
     406,   407,   408,   411,   412,   415,   425,   433,   434,   437,
     440,   448,   449,   450,   451,   452,   453,   454,   455,   456,
     457,   458,   459,   460,   461,   467,   468,   471,   472,   475,
     478,   485,   493,   494,   497,   506,   507,   510,   511,   514,
     515,   518,   527,   528,   529,   532,   535,   542,   552,   556,
     562,   563,   566,   570,   571,   574,   580,   581,   582,   583,
     584,   585,   586,   589,   596,   597,   598,   599,   600,   601,
     604,   606,   608,   610,   612,   614,   618,   620,   624,   626,
     628,   630,   632,   636,   644,   645,   646,   647,   648,   649,
     650,   653,   654,   655,   658,   659,   660,   661,   662,   663,
     664,   665,   666,   667,   670,   671,   672,   673,   676,   677,
     680,   681,   682,   683,   684,   685,   686,   687,   688,   689,
     690,   691,   692,   693,   694,   695,   696,   697,   698,   699,
     700,   701,   702,   703,   704,   705,   706,   707,   710,   711,
     714,   715,   718,   719,   722,   723,   724,   727,   728,   729,
     730,   731,   732,   733,   734,   735,   736,   737,   738,   739,
     740,   741,   742,   743,   744,   745,   746,   747,   748,   749,
     750,   751,   757,   760,   765,   771,   772,   773,   774,   777,
     782,   786,   792,   797,   806,   807,   808,   809,   810,   816,
     820,   821,   822,   823,   826,   828,   830,   832,   836,   837,
     838,   839,   841,   843,   845,   849,   850,   853,   858,   864,
     865,   866,   867,   868,   871,   876,   882,   883,   886,   887,
     888,   891,   896
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOKEN_FIRST", "FUNCTION", "RETURN",
  "ASSERT", "LAMBDA", "LAMBDA_REF", "ONEVENT", "METHOD", "SELF",
  "ARGUMENTS", "STATIC", "CONST", "KWD_IF", "ELSE", "WHILE", "FOR",
  "FOREACH", "NIL", "AND", "NOT", "OR", "TRUE", "FALSE", "BREAK",
  "CONTINUE", "LOCAL", "TRY", "TRAP", "THROW", "USING", "ADD", "SUB",
  "MUL", "DIV", "MOD", "GT", "LT", "NE", "EQ", "GE", "LE", "ASSIGN",
  "ADD_POSTFIX", "SUB_POSTFIX", "MUL_POSTFIX", "DIV_POSTFIX",
  "MOD_POSTFIX", "GT_POSTFIX", "LT_POSTFIX", "NE_POSTFIX", "EQ_POSTFIX",
  "GE_POSTFIX", "LE_POSTFIX", "CAST", "CALL", "GLOBAL_SCOPE", "PLUSPLUS",
  "MINUSMINUS", "DOT", "DOT_ASSIGN", "DOUBLE_DOT", "TRIPLE_DOT",
  "DOUBLE_LB", "DOUBLE_RB", "ADD_A", "SUB_A", "MUL_A", "DIV_A", "MOD_A",
  "SET", "GET", "ATTRIBUTE", "STRINGIFICATION", "NEWSELF", "DOT_EQUAL",
  "DOT_CAST", "DOT_EQUAL_RHS", "'('", "')'", "'['", "']'", "'{'", "'}'",
  "'?'", "':'", "','", "NUMBER_CONST", "STRING_CONST", "IDENT",
  "ATTRIBUTE_IDENT", "OPERATOR", "PARENTHESIS", "SQUARE_BRACKETS",
  "COMMENT", "INCOMPLETE_COMMENT", "INCOMPLETE_STRING", "CPP",
  "PARSE_STMT", "PARSE_FUNCTION_CALL_OBJECT", "PARSE_TABLE_OBJECT",
  "PARSE_NAMESPACE", "TOKEN_LAST", "UMINUS", "';'", "'|'", "$accept",
  "NullifyTree", "DeltaCode", "CodeDefs", "UsingDirectivesStmts",
  "UsingDirectives", "Stmts", "FuncClass", "FunctionLinkage",
  "FunctionName", "OperatorMethod", "FormalArgs", "FormalArgsSuffix",
  "IdentList", "Function", "LambdaFunction", "LambdaCode", "AttributeId",
  "IdentIndex", "IdentIndexElement", "NewAttribute", "AttrFunction",
  "AttributeSet", "AttributeGet", "Stmt", "LoopCtrlStmt", "Compound",
  "AssertStmt", "WhileStmt", "IfStmt", "ElseStmt", "ForStmt",
  "ForInitList", "ForRepeatList", "ForeachOpt", "ForeachStmt",
  "ReturnStmt", "ThrowStmt", "TryStmt", "TrapStmt", "UsingNamespace",
  "QualifiedId", "PureQualifiedId", "NonGlobalQualifiedId",
  "GlobalQualifiedId", "Expression", "ConstAssignExpression",
  "AssignExpression", "RelationalExpression", "BooleanExpression",
  "ArithmeticExpression", "TernaryExpression", "Term", "Primary",
  "FunctionAndTableObject", "ConstValueExceptString", "StringConst",
  "KwdIdent", "StringIdent", "StringifyDottedIdents",
  "StringifyNamespaceIdent", "StringConstUsed", "OpString",
  "FunctionCallObject", "ActualArguments", "ActualArgument",
  "FunctionCall", "ExpressionList", "Lvalue", "TableObject", "DotIndex",
  "DottedOpString", "TableContent", "TableConstructor", "TableElements",
  "TableElement", "ContentList", "ContentExpression", "IndexExpression",
  "IndexedList", 0
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
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
      40,    41,    91,    93,   123,   125,    63,    58,    44,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,    59,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   108,   109,   110,   110,   110,   110,   110,   111,   111,
     111,   112,   112,   113,   113,   114,   114,   115,   115,   115,
     116,   116,   117,   117,   117,   117,   118,   118,   118,   119,
     119,   119,   119,   120,   120,   121,   121,   122,   123,   124,
     125,   125,   125,   126,   126,   127,   128,   129,   129,   130,
     131,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   133,   133,   134,   134,   135,
     136,   137,   138,   138,   139,   140,   140,   141,   141,   142,
     142,   143,   144,   144,   144,   145,   146,   147,   148,   148,
     149,   149,   150,   151,   151,   152,   153,   153,   153,   153,
     153,   153,   153,   154,   155,   155,   155,   155,   155,   155,
     156,   156,   156,   156,   156,   156,   157,   157,   158,   158,
     158,   158,   158,   159,   160,   160,   160,   160,   160,   160,
     160,   161,   161,   161,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   163,   163,   163,   163,   164,   164,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   166,   166,
     167,   167,   168,   168,   169,   169,   169,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   171,   172,   172,   173,   173,   173,   173,   174,
     174,   174,   175,   175,   176,   176,   176,   176,   176,   177,
     178,   178,   178,   178,   179,   179,   179,   179,   180,   180,
     180,   180,   180,   180,   180,   181,   181,   182,   182,   183,
     183,   183,   183,   183,   184,   184,   185,   185,   186,   186,
     186,   187,   187
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     3,     3,     3,     3,     0,     1,
       1,     1,     2,     2,     1,     2,     1,     1,     1,     1,
       1,     0,     1,     2,     1,     0,     1,     1,     1,     3,
       2,     1,     0,     1,     2,     3,     1,     5,     3,     3,
       1,     2,     2,     1,     2,     3,     5,     1,     1,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     3,
       5,     6,     2,     0,     9,     1,     0,     1,     0,     3,
       1,     7,     3,     2,     2,     3,     3,     3,     3,     4,
       1,     1,     3,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     4,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     7,     2,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     3,     1,     1,     4,
       3,     2,     3,     1,     1,     1,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     4,     4,     4,     4,     3,     2,     3,     1,     1,
       1,     5,     1,     1,     3,     1,     1,     1,     1,     2,
       1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       0,    64,    17,     0,     0,    32,   142,    18,    19,   140,
     141,     0,     0,     0,     0,     0,     0,   145,     0,   146,
     147,    65,    66,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   143,     0,     0,     0,   144,   149,    94,   225,
       2,     2,     2,     2,    63,     0,     3,     0,    11,     0,
      21,    62,   133,    16,     0,    58,    52,    53,    56,    54,
      55,    57,    61,    60,    14,   224,    93,    90,    91,   223,
      97,    96,    98,    99,   100,   101,   102,   130,   131,   132,
     184,   185,   186,   139,     0,   135,     0,   134,     0,   228,
     136,    83,    84,     0,     0,    31,     0,     0,   226,     0,
       0,     0,    76,     0,   129,   227,     0,     0,     0,     0,
     128,    95,     0,   229,   125,   127,   152,   153,   168,   163,
     154,   175,   176,   177,   173,   174,   150,   151,   155,   156,
     157,   158,   160,   161,   162,   169,   170,   171,   172,   159,
     164,   165,   166,   167,   178,   182,     0,   183,   179,   180,
       0,     0,     0,     0,   246,     0,   225,   250,    43,     0,
     253,   252,   249,     0,   248,    68,     0,     0,     0,     0,
       0,     1,    12,    13,    15,    20,    25,    59,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   148,     0,   221,     0,     0,    51,     0,   124,
     126,     0,     0,     0,     0,     0,     0,   234,     0,     0,
     235,   236,   237,     0,   239,    82,    69,     0,    33,    36,
      30,     0,     0,    38,     0,     0,     0,     0,    75,   134,
       0,    86,    85,     0,    88,     0,   137,   138,     0,   178,
      44,    41,    42,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   211,   210,     0,   258,
     260,   262,     0,     0,     0,   245,     0,    67,     4,   229,
     134,   229,     7,    24,    22,     0,    32,    92,   116,   117,
     118,   119,   121,   120,   122,   110,   111,   115,   114,   112,
     113,   181,   217,   220,     0,   218,   215,     0,   214,   222,
     104,   105,   107,   106,   108,   109,   188,   230,   231,   233,
     232,   238,   240,    27,    28,     0,     0,    26,     0,     0,
      34,     0,    29,     0,   103,     0,     0,     0,     0,    80,
       0,     0,    89,     0,   259,     0,     0,     0,     0,    48,
      45,    47,   247,    23,     0,     0,   219,     0,   244,   243,
     242,   241,    35,    39,    73,    70,     0,   134,     0,    87,
       0,   257,   256,     0,   255,   261,    49,     0,     0,    37,
     216,   213,     0,    71,    78,    79,     0,     0,   251,     0,
      50,    46,    72,     0,    77,    81,   123,   254,     0,    74
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,   167,    45,    46,    47,    48,    49,    50,   176,   286,
     325,    97,   220,   221,    51,    52,   223,   158,   159,   160,
     161,   350,   348,   378,    53,    54,    55,    56,    57,    58,
     383,    59,   227,   393,   340,    60,    61,    62,    63,   231,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,   148,   149,    81,
      82,    83,   327,    84,   307,   308,    85,    86,    87,    88,
     321,   214,    89,    90,   163,   164,   373,   374,   271,   272
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -335
static const short int yypact[] =
{
     387,  -335,  -335,   931,  1583,   -40,  -335,  -335,  -335,  -335,
    -335,   -55,   -49,    -2,     7,    28,    31,  -335,  1583,  -335,
    -335,  -335,  -335,   -33,   903,  1583,   -42,  1583,    29,   248,
     248,  1329,  -335,  1152,  1020,   697,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,   114,  -335,   491,    98,   594,
     107,  -335,  -335,  -335,    30,  -335,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,    79,  -335,  1698,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,   403,  -335,
      48,    78,  -335,  -335,   -29,  -335,   -67,   -15,   226,  -335,
    -335,  -335,  -335,   210,   401,  -335,    24,    59,  -335,   105,
    1583,  1583,  1583,   248,  -335,  -335,   123,   520,    64,    50,
    -335,    79,  1152,    23,   507,   507,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,   101,   102,    79,  -335,  -335,  -335,
      80,   709,  1379,  1412,  -335,  1524,   -61,  -335,    81,    83,
    -335,  -335,  1698,   -51,  -335,  -335,   800,   903,   248,   248,
     -44,  -335,  -335,  -335,  -335,  -335,    38,  -335,    75,  1583,
    1583,  1583,  1583,  1583,  1583,  1583,  1583,  1583,  1583,  1583,
    1583,  1583,  -335,  1379,  -335,    82,  1583,  -335,  1583,  -335,
    -335,  1583,  1583,  1583,  1583,  1583,  1241,  -335,  1241,  1438,
    -335,  -335,  -335,  1438,  -335,  -335,  -335,    86,  -335,  -335,
    -335,    37,  1583,  -335,  1583,  1158,  1284,    62,   103,    -4,
      84,  -335,  -335,    63,  -335,  1589,  -335,  -335,  1583,  -335,
    -335,  -335,  -335,  -335,  1583,  -335,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  1379,  1698,
    -335,  -335,    45,   118,  1152,  -335,  1063,  -335,  -335,    19,
    -335,    20,  -335,  -335,  -335,  1661,   -40,  -335,   415,   493,
      87,    87,  -335,  -335,  -335,   725,   725,   596,   596,   725,
     725,  -335,  -335,  -335,  1583,  -335,  1698,    -7,  -335,  1698,
    1698,  1698,  1698,  1698,  1698,  1698,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,   128,  1183,  -335,   120,  1092,
    -335,   113,  -335,   809,  1698,   903,   903,  1583,   248,  -335,
    1583,   903,  -335,   654,  -335,  1152,  1524,  1152,   132,  -335,
    -335,  1698,  -335,  -335,   125,    27,  -335,   204,  -335,  -335,
    -335,  -335,  -335,  -335,   191,  -335,   623,   134,  1612,  -335,
    1583,  -335,  1698,   -47,  -335,  -335,  -335,  1152,   135,  -335,
    -335,  -335,   903,  -335,  1583,  -335,   903,  1643,  -335,  1152,
    -335,  -335,  -335,   142,   103,  -335,  -335,  -335,   903,  -335
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -335,    85,  -335,  -335,  -335,  -335,   190,  -335,  -335,  -335,
    -206,   -59,     9,  -335,     1,  -335,  -335,  -335,  -335,  -335,
    -335,  -334,  -335,  -335,   -22,  -335,  -120,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,
     188,   -16,   206,    44,   208,    -3,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,   -21,  -335,  -335,  -135,  -147,  -335,
    -335,   -89,  -149,  -335,  -335,  -117,  -335,   -99,   -18,  -335,
      34,  -335,  -335,  -335,  -335,   -19,  -335,  -134,   -92,  -335
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -230
static const short int yytable[] =
{
      93,    94,   106,   228,    92,   240,   270,   328,   113,   113,
     109,   114,   115,   376,    28,   104,    28,    95,   242,    -5,
      -6,   196,   107,   -40,   110,   172,   -40,   174,   194,   198,
     151,   162,   275,   108,   150,   157,    98,   276,   388,   197,
      96,   389,    99,   390,   199,   200,   301,    38,   179,    38,
     180,   195,   201,   202,   203,   204,   205,   320,   105,   320,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   318,   111,   318,   356,   146,  -212,  -212,   100,   353,
    -212,   357,   113,   338,   339,   229,     2,   101,   217,     5,
       6,     7,     8,     9,    10,    11,    12,   225,   226,  -212,
    -212,   217,    17,  -212,    18,   218,    19,    20,   102,   235,
      23,   103,   283,   150,   171,   219,    27,   319,   218,   319,
      38,   344,   183,   184,   185,   331,   168,   169,   170,   284,
      26,   285,   345,   346,   380,   175,   177,   178,   192,   193,
      28,    29,    30,   222,   174,   278,   302,   279,   281,   224,
     280,   280,   269,   230,   282,   233,   234,    31,    32,   -94,
     -93,   236,    33,   303,    34,   273,   287,   330,   337,   342,
     274,    36,    37,    38,    39,   341,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   304,
     347,   196,   306,   309,   358,   310,   305,   270,   311,   312,
     313,   314,   315,   360,   362,   377,   326,   382,     2,    35,
     329,     5,     6,     7,     8,     9,    10,    11,    12,   333,
     391,   334,   385,   398,    17,   166,    18,   354,    19,    20,
     332,   179,    23,   180,   379,   343,   173,   145,    27,   147,
     381,   110,   322,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   375,   397,     6,   352,     0,     9,
      10,    11,    28,    29,    30,     0,     0,     0,   302,     0,
       0,   351,     0,   162,     0,   349,    23,   157,     0,    31,
      32,     0,     0,     0,    33,   394,    34,   206,   207,   208,
       0,   209,     0,    36,    37,    38,    39,     0,     0,     0,
       0,   355,     0,   210,   211,   212,    28,     0,   213,     0,
       0,   304,     0,   364,   365,     0,   215,   113,     0,   369,
     367,     0,     0,    31,    32,     0,     0,     0,   112,     0,
      34,     0,     0,     0,   366,     0,     0,   368,    37,    38,
      39,     0,   372,   269,   351,     0,   371,     0,   349,     0,
       0,     0,     0,     0,   306,     0,     0,     0,   305,     0,
     392,     0,     0,     0,   395,     0,     0,   387,     0,     0,
       0,     0,     0,     0,   351,     0,   399,     0,   349,     0,
       0,     0,     0,     0,     0,     0,   372,    -8,     1,     0,
     371,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,    16,    17,     0,    18,
       0,    19,    20,    21,    22,    23,    24,     0,    25,    26,
       0,    27,   179,     0,   180,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,    28,    29,    30,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,     0,
    -212,     0,    31,    32,  -229,  -229,  -229,    33,  -229,    34,
       0,    35,     0,     0,     0,     0,    36,    37,    38,    39,
    -229,  -229,  -229,  -212,     0,  -229,     0,    40,    41,    42,
      43,   -10,     1,    44,     0,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,   216,    14,    15,
      16,    17,     0,    18,   179,    19,    20,    21,    22,    23,
      24,     0,    25,     0,     0,    27,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,     0,     0,     0,
       0,   179,     0,   180,     0,     0,     0,     0,     0,    28,
      29,    30,     0,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,  -134,     0,    31,    32,  -134,  -134,
    -134,    33,  -134,    34,     0,    35,     0,     0,     0,     0,
      36,    37,    38,    39,  -134,  -134,  -134,  -134,     0,  -134,
       0,     0,     0,     0,    -9,     1,     0,    44,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,    16,    17,     0,    18,     0,    19,    20,
      21,    22,    23,    24,     0,    25,   232,     0,    27,   181,
     182,   183,   184,   185,   186,   187,  -230,  -230,   190,   191,
       0,     0,     0,     0,   179,     0,   180,     0,     0,     0,
       0,     0,    28,    29,    30,     0,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,     0,     0,    31,
      32,     0,     0,     0,    33,   179,    34,   180,    35,     0,
       0,     0,     0,    36,    37,    38,    39,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,     1,     0,
      44,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,    16,    17,     0,    18,
       0,    19,    20,    21,    22,    23,    24,     0,    25,   384,
     179,    27,   180,     0,     0,     0,     0,     0,     0,     0,
       0,   370,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,     0,     0,    28,    29,    30,   181,   182,
     183,   184,   185,  -230,  -230,     0,     0,  -230,  -230,     0,
       0,     0,    31,    32,     0,     0,     0,    33,     0,    34,
       0,    35,   165,     0,     0,     0,    36,    37,    38,    39,
     237,     0,     0,     0,     0,   238,     0,     0,     0,     0,
       0,     1,     0,    44,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,    16,
      17,     0,    18,     0,    19,    20,    21,    22,    23,    24,
     179,    25,   180,     0,    27,     0,     0,     0,     0,     0,
       0,     0,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,     0,     0,     0,     0,     0,    28,    29,
      30,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,     0,     0,     0,
      33,     0,    34,     0,    35,   277,     0,     0,     0,    36,
      37,    38,    39,     0,   363,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     1,     0,    44,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,    16,    17,     0,    18,     0,    19,    20,    21,
      22,    23,    24,     0,    25,     2,     0,    27,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,     0,
       0,    17,     0,    18,     0,    19,    20,     0,     0,    23,
       0,    28,    29,    30,     0,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
       0,     0,     0,    33,     0,    34,     0,    35,     0,    28,
      29,    30,    36,    37,    38,    39,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,    32,     0,    44,
       0,    33,     0,    34,     0,     0,     0,     0,     0,     0,
      36,    37,    38,    39,     2,     0,     0,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,    91,     0,     0,
      17,     0,    18,     0,    19,    20,     0,     0,    23,     0,
       0,     0,     0,     0,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     2,     0,     0,
       5,     6,     7,     8,     9,    10,    11,    12,    28,    29,
      30,   152,     0,    17,     0,    18,     0,    19,    20,     0,
       0,    23,     0,     0,   153,    31,    32,    27,     0,     0,
      33,     0,    34,   154,   155,     0,     0,     0,     0,    36,
      37,    38,   156,   179,     0,   180,     0,     0,     0,     0,
       0,    28,    29,    30,   152,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,     0,   153,    31,    32,
       0,     0,     0,    33,     0,    34,     0,   155,     0,     0,
       0,     0,    36,    37,    38,   156,     2,     0,     0,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,     0,
       0,     0,    17,     0,    18,   361,    19,    20,     0,   179,
      23,   180,     0,     0,     0,     0,    27,     0,     0,     0,
       0,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,     0,     0,   179,     0,   180,     0,     0,     0,
      28,    29,    30,     0,     0,     0,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,    31,    32,     0,
       0,     0,    33,     0,    34,     0,     0,     0,     0,   335,
       0,    36,    37,    38,    39,   116,   117,   118,   119,   359,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   243,   316,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,     0,
       0,     0,     0,     0,     0,   179,     0,   180,     0,     0,
       0,     0,     0,     0,     0,     0,    31,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,     0,     0,
       0,    37,   317,   116,   117,   118,   119,     0,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,     0,     0,     0,   336,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   116,   117,   118,   119,    28,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,     0,     0,     0,     0,   116,   117,   118,   119,
     144,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,     5,     6,     0,     0,     9,
      10,    11,    12,     0,     0,     0,     0,     0,    17,     0,
      18,     0,    19,    20,     0,     0,    23,     0,     0,     0,
     239,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,    28,    29,    30,   323,
     324,     0,     0,   241,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,     0,     0,    33,     0,
      34,     0,     0,     0,     0,     0,     0,    36,    37,    38,
      39,     5,     6,     0,     0,     9,    10,    11,    12,     0,
       0,     0,     0,     0,    17,     0,    18,     0,    19,    20,
       0,     0,    23,     0,     0,     0,     0,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,    28,    29,    30,   268,     0,     0,     0,     0,
       5,     6,     0,     0,     9,    10,    11,    12,     0,    31,
      32,     0,     0,    17,    33,    18,    34,    19,    20,     0,
     179,    23,   180,    36,    37,    38,    39,    27,     0,     0,
       0,     0,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   179,     0,   180,     0,     0,     0,     0,
       0,    28,    29,    30,     0,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,     0,     0,    31,    32,
       0,     0,     0,    33,   179,    34,   180,     0,     0,     0,
     237,     0,    36,    37,    38,    39,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,     0,     0,     0,
       0,     0,     0,   386,   243,   316,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   179,
       0,   180,   323,   324,   396,     0,     0,     0,     0,     0,
       0,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191
};

static const short int yycheck[] =
{
       3,     4,    24,   102,     3,   152,   155,   213,    29,    30,
      26,    29,    30,   347,    58,    18,    58,    57,   153,     0,
       0,    88,    25,    84,    27,    47,    87,    49,    57,    44,
      33,    34,    83,    75,    33,    34,    91,    88,    85,   106,
      80,    88,    91,   377,    59,    60,   193,    91,    21,    91,
      23,    80,    67,    68,    69,    70,    71,   206,    91,   208,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,   206,    28,   208,    81,    31,    57,    57,    80,   285,
      57,    88,   103,    87,    88,   103,     4,    80,    64,     7,
       8,     9,    10,    11,    12,    13,    14,   100,   101,    80,
      80,    64,    20,    80,    22,    81,    24,    25,    80,   112,
      28,    80,    74,   112,     0,    91,    34,   206,    81,   208,
      91,   268,    35,    36,    37,    88,    41,    42,    43,    91,
      32,    93,    87,    88,   107,    28,   106,    58,    90,    61,
      58,    59,    60,    84,   166,   167,    64,   168,   169,    44,
     168,   169,   155,    30,   170,    91,   106,    75,    76,    58,
      58,    81,    80,    81,    82,    84,    91,    81,   106,   106,
      87,    89,    90,    91,    92,    91,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   107,
      72,    88,   195,   196,    66,   198,   195,   346,   201,   202,
     203,   204,   205,    83,    91,    73,   209,    16,     4,    84,
     213,     7,     8,     9,    10,    11,    12,    13,    14,   222,
      85,   224,    88,    81,    20,    35,    22,   286,    24,    25,
     221,    21,    28,    23,   354,   238,    48,    31,    34,    31,
     357,   244,   208,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,   346,   389,     8,   276,    -1,    11,
      12,    13,    58,    59,    60,    -1,    -1,    -1,    64,    -1,
      -1,   274,    -1,   276,    -1,   274,    28,   276,    -1,    75,
      76,    -1,    -1,    -1,    80,   384,    82,    61,    62,    63,
      -1,    65,    -1,    89,    90,    91,    92,    -1,    -1,    -1,
      -1,   304,    -1,    77,    78,    79,    58,    -1,    82,    -1,
      -1,   107,    -1,   335,   336,    -1,   106,   338,    -1,   341,
     338,    -1,    -1,    75,    76,    -1,    -1,    -1,    80,    -1,
      82,    -1,    -1,    -1,   337,    -1,    -1,   340,    90,    91,
      92,    -1,   345,   346,   347,    -1,   345,    -1,   347,    -1,
      -1,    -1,    -1,    -1,   357,    -1,    -1,    -1,   357,    -1,
     382,    -1,    -1,    -1,   386,    -1,    -1,   370,    -1,    -1,
      -1,    -1,    -1,    -1,   377,    -1,   398,    -1,   377,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   389,     0,     1,    -1,
     389,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    -1,    17,    18,    19,    20,    -1,    22,
      -1,    24,    25,    26,    27,    28,    29,    -1,    31,    32,
      -1,    34,    21,    -1,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    58,    59,    60,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    -1,
      57,    -1,    75,    76,    61,    62,    63,    80,    65,    82,
      -1,    84,    -1,    -1,    -1,    -1,    89,    90,    91,    92,
      77,    78,    79,    80,    -1,    82,    -1,   100,   101,   102,
     103,     0,     1,   106,    -1,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,   106,    17,    18,
      19,    20,    -1,    22,    21,    24,    25,    26,    27,    28,
      29,    -1,    31,    -1,    -1,    34,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      -1,    21,    -1,    23,    -1,    -1,    -1,    -1,    -1,    58,
      59,    60,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    57,    -1,    75,    76,    61,    62,
      63,    80,    65,    82,    -1,    84,    -1,    -1,    -1,    -1,
      89,    90,    91,    92,    77,    78,    79,    80,    -1,    82,
      -1,    -1,    -1,    -1,     0,     1,    -1,   106,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      -1,    17,    18,    19,    20,    -1,    22,    -1,    24,    25,
      26,    27,    28,    29,    -1,    31,   106,    -1,    34,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    -1,    21,    -1,    23,    -1,    -1,    -1,
      -1,    -1,    58,    59,    60,    -1,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    75,
      76,    -1,    -1,    -1,    80,    21,    82,    23,    84,    -1,
      -1,    -1,    -1,    89,    90,    91,    92,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,     1,    -1,
     106,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    -1,    17,    18,    19,    20,    -1,    22,
      -1,    24,    25,    26,    27,    28,    29,    -1,    31,   106,
      21,    34,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    87,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    58,    59,    60,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    42,    43,    -1,
      -1,    -1,    75,    76,    -1,    -1,    -1,    80,    -1,    82,
      -1,    84,    85,    -1,    -1,    -1,    89,    90,    91,    92,
      81,    -1,    -1,    -1,    -1,    86,    -1,    -1,    -1,    -1,
      -1,     1,    -1,   106,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    -1,    17,    18,    19,
      20,    -1,    22,    -1,    24,    25,    26,    27,    28,    29,
      21,    31,    23,    -1,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    -1,    -1,    58,    59,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    75,    76,    -1,    -1,    -1,
      80,    -1,    82,    -1,    84,    85,    -1,    -1,    -1,    89,
      90,    91,    92,    -1,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     1,    -1,   106,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    -1,
      17,    18,    19,    20,    -1,    22,    -1,    24,    25,    26,
      27,    28,    29,    -1,    31,     4,    -1,    34,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    20,    -1,    22,    -1,    24,    25,    -1,    -1,    28,
      -1,    58,    59,    60,    -1,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    76,
      -1,    -1,    -1,    80,    -1,    82,    -1,    84,    -1,    58,
      59,    60,    89,    90,    91,    92,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    76,    -1,   106,
      -1,    80,    -1,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    90,    91,    92,     4,    -1,    -1,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,   106,    -1,    -1,
      20,    -1,    22,    -1,    24,    25,    -1,    -1,    28,    -1,
      -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,    -1,    -1,
       7,     8,     9,    10,    11,    12,    13,    14,    58,    59,
      60,    61,    -1,    20,    -1,    22,    -1,    24,    25,    -1,
      -1,    28,    -1,    -1,    74,    75,    76,    34,    -1,    -1,
      80,    -1,    82,    83,    84,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    21,    -1,    23,    -1,    -1,    -1,    -1,
      -1,    58,    59,    60,    61,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    74,    75,    76,
      -1,    -1,    -1,    80,    -1,    82,    -1,    84,    -1,    -1,
      -1,    -1,    89,    90,    91,    92,     4,    -1,    -1,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    20,    -1,    22,    83,    24,    25,    -1,    21,
      28,    23,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    -1,    21,    -1,    23,    -1,    -1,    -1,
      58,    59,    60,    -1,    -1,    -1,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    75,    76,    -1,
      -1,    -1,    80,    -1,    82,    -1,    -1,    -1,    -1,    81,
      -1,    89,    90,    91,    92,     4,     5,     6,     7,    66,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      -1,    -1,    -1,    -1,    -1,    21,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    90,    91,     4,     5,     6,     7,    -1,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    81,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     4,     5,     6,     7,    58,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    -1,     4,     5,     6,     7,
      91,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,     7,     8,    -1,    -1,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    20,    -1,
      22,    -1,    24,    25,    -1,    -1,    28,    -1,    -1,    -1,
      91,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    -1,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    75,    76,    -1,    -1,    -1,    80,    -1,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,
      92,     7,     8,    -1,    -1,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    20,    -1,    22,    -1,    24,    25,
      -1,    -1,    28,    -1,    -1,    -1,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    -1,    -1,    -1,    -1,
       7,     8,    -1,    -1,    11,    12,    13,    14,    -1,    75,
      76,    -1,    -1,    20,    80,    22,    82,    24,    25,    -1,
      21,    28,    23,    89,    90,    91,    92,    34,    -1,    -1,
      -1,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    21,    -1,    23,    -1,    -1,    -1,    -1,
      -1,    58,    59,    60,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    -1,    75,    76,
      -1,    -1,    -1,    80,    21,    82,    23,    -1,    -1,    -1,
      81,    -1,    89,    90,    91,    92,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    81,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    21,
      -1,    23,    61,    62,    81,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    17,    18,    19,    20,    22,    24,
      25,    26,    27,    28,    29,    31,    32,    34,    58,    59,
      60,    75,    76,    80,    82,    84,    89,    90,    91,    92,
     100,   101,   102,   103,   106,   110,   111,   112,   113,   114,
     115,   122,   123,   132,   133,   134,   135,   136,   137,   139,
     143,   144,   145,   146,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   167,   168,   169,   171,   174,   175,   176,   177,   180,
     181,   106,   122,   153,   153,    57,    80,   119,    91,    91,
      80,    80,    80,    80,   153,    91,   132,   153,    75,   149,
     153,   151,    80,   162,   176,   176,     4,     5,     6,     7,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    91,   150,   151,   152,   165,   166,
     122,   153,    61,    74,    83,    84,    92,   122,   125,   126,
     127,   128,   153,   182,   183,    85,   114,   109,   109,   109,
     109,     0,   132,   148,   132,    28,   116,   106,    58,    21,
      23,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    90,    61,    57,    80,    88,   106,    44,    59,
      60,    67,    68,    69,    70,    71,    61,    62,    63,    65,
      77,    78,    79,    82,   179,   106,   106,    64,    81,    91,
     120,   121,    84,   124,    44,   153,   153,   140,   175,   176,
      30,   147,   106,    91,   106,   153,    81,    81,    86,    91,
     166,    91,   165,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    61,   153,
     170,   186,   187,    84,    87,    83,    88,    85,   132,   162,
     176,   162,   149,    74,    91,    93,   117,    91,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   166,    64,    81,   107,   122,   153,   172,   173,   153,
     153,   153,   153,   153,   153,   153,    34,    91,   165,   169,
     170,   178,   178,    61,    62,   118,   153,   170,   118,   153,
      81,    88,   120,   153,   153,    81,    81,   106,    87,    88,
     142,    91,   106,   153,   166,    87,    88,    72,   130,   122,
     129,   153,   183,   118,   119,   153,    81,    88,    66,    66,
      83,    83,    91,    85,   132,   132,   153,   176,   153,   132,
      87,   122,   153,   184,   185,   186,   129,    73,   131,   134,
     107,   173,    16,   138,   106,    88,    81,   153,    85,    88,
     129,    85,   132,   141,   175,   132,    81,   185,    81,   132
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
      yyerror (&yylloc, ctx, YY_("syntax error: cannot back up")); \
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
      case 111: /* "CodeDefs" */
#line 175 "DeltaParserSpec.y"
        { delete (yyvaluep->stmtsValue); };
#line 1817 "DeltaParserGen.cpp"
        break;
      case 114: /* "Stmts" */
#line 175 "DeltaParserSpec.y"
        { delete (yyvaluep->stmtsValue); };
#line 1822 "DeltaParserGen.cpp"
        break;
      case 117: /* "FunctionName" */
#line 185 "DeltaParserSpec.y"
        { delete (yyvaluep->funcNameValue); };
#line 1827 "DeltaParserGen.cpp"
        break;
      case 119: /* "FormalArgs" */
#line 156 "DeltaParserSpec.y"
        { delete (yyvaluep->argListValue); };
#line 1832 "DeltaParserGen.cpp"
        break;
      case 121: /* "IdentList" */
#line 156 "DeltaParserSpec.y"
        { delete (yyvaluep->argListValue); };
#line 1837 "DeltaParserGen.cpp"
        break;
      case 122: /* "Function" */
#line 184 "DeltaParserSpec.y"
        { delete (yyvaluep->funcValue); };
#line 1842 "DeltaParserGen.cpp"
        break;
      case 125: /* "AttributeId" */
#line 152 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 1847 "DeltaParserGen.cpp"
        break;
      case 128: /* "NewAttribute" */
#line 206 "DeltaParserSpec.y"
        { delete (yyvaluep->newAttributeValue); };
#line 1852 "DeltaParserGen.cpp"
        break;
      case 130: /* "AttributeSet" */
#line 176 "DeltaParserSpec.y"
        { delete (yyvaluep->unaryKwdValue); };
#line 1857 "DeltaParserGen.cpp"
        break;
      case 131: /* "AttributeGet" */
#line 176 "DeltaParserSpec.y"
        { delete (yyvaluep->unaryKwdValue); };
#line 1862 "DeltaParserGen.cpp"
        break;
      case 132: /* "Stmt" */
#line 152 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 1867 "DeltaParserGen.cpp"
        break;
      case 133: /* "LoopCtrlStmt" */
#line 177 "DeltaParserSpec.y"
        { delete (yyvaluep->leafKwdValue); };
#line 1872 "DeltaParserGen.cpp"
        break;
      case 134: /* "Compound" */
#line 183 "DeltaParserSpec.y"
        { delete (yyvaluep->compValue); };
#line 1877 "DeltaParserGen.cpp"
        break;
      case 135: /* "AssertStmt" */
#line 176 "DeltaParserSpec.y"
        { delete (yyvaluep->unaryKwdValue); };
#line 1882 "DeltaParserGen.cpp"
        break;
      case 136: /* "WhileStmt" */
#line 178 "DeltaParserSpec.y"
        { delete (yyvaluep->whileValue); };
#line 1887 "DeltaParserGen.cpp"
        break;
      case 137: /* "IfStmt" */
#line 181 "DeltaParserSpec.y"
        { delete (yyvaluep->ifValue); };
#line 1892 "DeltaParserGen.cpp"
        break;
      case 138: /* "ElseStmt" */
#line 182 "DeltaParserSpec.y"
        { delete (yyvaluep->elseValue); };
#line 1897 "DeltaParserGen.cpp"
        break;
      case 139: /* "ForStmt" */
#line 179 "DeltaParserSpec.y"
        { delete (yyvaluep->forValue); };
#line 1902 "DeltaParserGen.cpp"
        break;
      case 140: /* "ForInitList" */
#line 155 "DeltaParserSpec.y"
        { delete (yyvaluep->exprListValue); };
#line 1907 "DeltaParserGen.cpp"
        break;
      case 141: /* "ForRepeatList" */
#line 155 "DeltaParserSpec.y"
        { delete (yyvaluep->exprListValue); };
#line 1912 "DeltaParserGen.cpp"
        break;
      case 142: /* "ForeachOpt" */
#line 152 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 1917 "DeltaParserGen.cpp"
        break;
      case 143: /* "ForeachStmt" */
#line 180 "DeltaParserSpec.y"
        { delete (yyvaluep->foreachValue); };
#line 1922 "DeltaParserGen.cpp"
        break;
      case 144: /* "ReturnStmt" */
#line 176 "DeltaParserSpec.y"
        { delete (yyvaluep->unaryKwdValue); };
#line 1927 "DeltaParserGen.cpp"
        break;
      case 145: /* "ThrowStmt" */
#line 176 "DeltaParserSpec.y"
        { delete (yyvaluep->unaryKwdValue); };
#line 1932 "DeltaParserGen.cpp"
        break;
      case 146: /* "TryStmt" */
#line 187 "DeltaParserSpec.y"
        { delete (yyvaluep->tryValue); };
#line 1937 "DeltaParserGen.cpp"
        break;
      case 147: /* "TrapStmt" */
#line 188 "DeltaParserSpec.y"
        { delete (yyvaluep->trapValue); };
#line 1942 "DeltaParserGen.cpp"
        break;
      case 149: /* "QualifiedId" */
#line 154 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 1947 "DeltaParserGen.cpp"
        break;
      case 150: /* "PureQualifiedId" */
#line 154 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 1952 "DeltaParserGen.cpp"
        break;
      case 151: /* "NonGlobalQualifiedId" */
#line 154 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 1957 "DeltaParserGen.cpp"
        break;
      case 152: /* "GlobalQualifiedId" */
#line 154 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 1962 "DeltaParserGen.cpp"
        break;
      case 153: /* "Expression" */
#line 152 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 1967 "DeltaParserGen.cpp"
        break;
      case 154: /* "ConstAssignExpression" */
#line 196 "DeltaParserSpec.y"
        { delete (yyvaluep->binaryOpValue); };
#line 1972 "DeltaParserGen.cpp"
        break;
      case 155: /* "AssignExpression" */
#line 196 "DeltaParserSpec.y"
        { delete (yyvaluep->binaryOpValue); };
#line 1977 "DeltaParserGen.cpp"
        break;
      case 156: /* "RelationalExpression" */
#line 195 "DeltaParserSpec.y"
        { delete (yyvaluep->binaryOpValue); };
#line 1982 "DeltaParserGen.cpp"
        break;
      case 157: /* "BooleanExpression" */
#line 195 "DeltaParserSpec.y"
        { delete (yyvaluep->binaryOpValue); };
#line 1987 "DeltaParserGen.cpp"
        break;
      case 158: /* "ArithmeticExpression" */
#line 195 "DeltaParserSpec.y"
        { delete (yyvaluep->binaryOpValue); };
#line 1992 "DeltaParserGen.cpp"
        break;
      case 159: /* "TernaryExpression" */
#line 186 "DeltaParserSpec.y"
        { delete (yyvaluep->ternaryOpValue); };
#line 1997 "DeltaParserGen.cpp"
        break;
      case 160: /* "Term" */
#line 152 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2002 "DeltaParserGen.cpp"
        break;
      case 161: /* "Primary" */
#line 152 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2007 "DeltaParserGen.cpp"
        break;
      case 162: /* "FunctionAndTableObject" */
#line 152 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2012 "DeltaParserGen.cpp"
        break;
      case 163: /* "ConstValueExceptString" */
#line 198 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2017 "DeltaParserGen.cpp"
        break;
      case 169: /* "StringConstUsed" */
#line 152 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2022 "DeltaParserGen.cpp"
        break;
      case 171: /* "FunctionCallObject" */
#line 153 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2027 "DeltaParserGen.cpp"
        break;
      case 172: /* "ActualArguments" */
#line 155 "DeltaParserSpec.y"
        { delete (yyvaluep->exprListValue); };
#line 2032 "DeltaParserGen.cpp"
        break;
      case 173: /* "ActualArgument" */
#line 153 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2037 "DeltaParserGen.cpp"
        break;
      case 174: /* "FunctionCall" */
#line 197 "DeltaParserSpec.y"
        { delete (yyvaluep->callValue); };
#line 2042 "DeltaParserGen.cpp"
        break;
      case 175: /* "ExpressionList" */
#line 155 "DeltaParserSpec.y"
        { delete (yyvaluep->exprListValue); };
#line 2047 "DeltaParserGen.cpp"
        break;
      case 176: /* "Lvalue" */
#line 152 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2052 "DeltaParserGen.cpp"
        break;
      case 177: /* "TableObject" */
#line 153 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2057 "DeltaParserGen.cpp"
        break;
      case 178: /* "DotIndex" */
#line 153 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2062 "DeltaParserGen.cpp"
        break;
      case 179: /* "DottedOpString" */
#line 153 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2067 "DeltaParserGen.cpp"
        break;
      case 180: /* "TableContent" */
#line 195 "DeltaParserSpec.y"
        { delete (yyvaluep->binaryOpValue); };
#line 2072 "DeltaParserGen.cpp"
        break;
      case 181: /* "TableConstructor" */
#line 210 "DeltaParserSpec.y"
        { delete (yyvaluep->tableCtorValue); };
#line 2077 "DeltaParserGen.cpp"
        break;
      case 182: /* "TableElements" */
#line 208 "DeltaParserSpec.y"
        { delete (yyvaluep->tableElemsValue); };
#line 2082 "DeltaParserGen.cpp"
        break;
      case 183: /* "TableElement" */
#line 207 "DeltaParserSpec.y"
        { delete (yyvaluep->tableElemValue); };
#line 2087 "DeltaParserGen.cpp"
        break;
      case 186: /* "IndexExpression" */
#line 153 "DeltaParserSpec.y"
        { delete (yyvaluep->nodeValue); };
#line 2092 "DeltaParserGen.cpp"
        break;
      case 187: /* "IndexedList" */
#line 209 "DeltaParserSpec.y"
        { delete (yyvaluep->tableIListVlaue); };
#line 2097 "DeltaParserGen.cpp"
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
int yyparse (delta::ParsingCtx& ctx);
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
yyparse (delta::ParsingCtx& ctx)
#else
int
yyparse (ctx)
    delta::ParsingCtx& ctx;
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
#line 268 "DeltaParserSpec.y"
    { 
					ctx.GetProgramDesc().SetAST(0);	
					ctx.GetProgramDesc().SetStmtAST(0);
					ctx.GetProgramDesc().SetFunctionObjectAST(0);
					ctx.GetProgramDesc().SetTableObjectAST(0);
					ctx.GetProgramDesc().SetNamespaceAST(0);
				;}
    break;

  case 3:
#line 277 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetAST((yyvsp[0].stmtsValue));	;}
    break;

  case 4:
#line 279 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetStmtAST((yyvsp[0].nodeValue));	;}
    break;

  case 5:
#line 280 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetFunctionObjectAST((yyvsp[0].nodeValue)); ;}
    break;

  case 6:
#line 281 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetTableObjectAST((yyvsp[0].nodeValue));	;}
    break;

  case 7:
#line 282 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetNamespaceAST((yyvsp[0].nodeValue));	;}
    break;

  case 8:
#line 285 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = 0; ;}
    break;

  case 9:
#line 286 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = (yyvsp[0].stmtsValue); ;}
    break;

  case 10:
#line 287 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = (yyvsp[0].stmtsValue); ;}
    break;

  case 11:
#line 291 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = (yyvsp[0].stmtsValue); ;}
    break;

  case 12:
#line 293 "DeltaParserSpec.y"
    {	(yyval.stmtsValue) = (yyvsp[-1].stmtsValue); 
									(yyval.stmtsValue)->AppendChild((yyvsp[0].nodeValue)); 
									(yyval.stmtsValue)->GetRange().right = (yylsp[0]).last_column; 
								;}
    break;

  case 13:
#line 300 "DeltaParserSpec.y"
    {  
								(yyval.stmtsValue) = (yyvsp[-1].stmtsValue);
								(yyval.stmtsValue)->AppendChild((yyvsp[0].usingValue));
								(yyval.stmtsValue)->GetRange().right = (yylsp[0]).last_column;
							;}
    break;

  case 14:
#line 306 "DeltaParserSpec.y"
    {
								(yyval.stmtsValue) = CreateNode<StmtsASTNode>((yylsp[0]), (yylsp[0]));
								(yyval.stmtsValue)->AppendChild((yyvsp[0].usingValue));
							;}
    break;

  case 15:
#line 312 "DeltaParserSpec.y"
    {
				(yyval.stmtsValue) = (yyvsp[-1].stmtsValue);
				(yyval.stmtsValue)->AppendChild((yyvsp[0].nodeValue));
				(yyval.stmtsValue)->GetRange().right = (yylsp[0]).last_column;
			;}
    break;

  case 16:
#line 317 "DeltaParserSpec.y"
    {
				(yyval.stmtsValue) = CreateNode<StmtsASTNode>((yylsp[0]), (yylsp[0]));
				(yyval.stmtsValue)->AppendChild((yyvsp[0].nodeValue));
			;}
    break;

  case 17:
#line 326 "DeltaParserSpec.y"
    { (yyval.literalValue) = (yyvsp[0].literalValue); ;}
    break;

  case 18:
#line 327 "DeltaParserSpec.y"
    { (yyval.literalValue) = (yyvsp[0].literalValue); ;}
    break;

  case 19:
#line 328 "DeltaParserSpec.y"
    { (yyval.literalValue) = (yyvsp[0].literalValue); ;}
    break;

  case 20:
#line 331 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[0]), (yylsp[0]), 0, (yyvsp[0].literalValue));  ;}
    break;

  case 21:
#line 332 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = 0;  ;}
    break;

  case 22:
#line 335 "DeltaParserSpec.y"
    { (yyval.funcNameValue) = CreateNode<FunctionNameASTNode>((yylsp[0]), (yylsp[0])); ;}
    break;

  case 23:
#line 336 "DeltaParserSpec.y"
    { (yyval.funcNameValue) = CreateNode<FunctionNameASTNode>((yylsp[-1]), (yylsp[0])); ;}
    break;

  case 24:
#line 337 "DeltaParserSpec.y"
    { (yyval.funcNameValue) = CreateNode<FunctionNameASTNode>((yylsp[0]), (yylsp[0])); ;}
    break;

  case 25:
#line 338 "DeltaParserSpec.y"
    { (yyval.funcNameValue) = 0; ;}
    break;

  case 29:
#line 346 "DeltaParserSpec.y"
    {
					(yyval.argListValue) = (yyvsp[-1].argListValue);
					if ((yyvsp[0].argValue)) {
						(yyval.argListValue)->AppendChild((yyvsp[0].argValue));
						(yyval.argListValue)->GetRange().right = (yyvsp[0].argValue)->GetRange().right;
					}
				;}
    break;

  case 30:
#line 353 "DeltaParserSpec.y"
    {
					if ((yyvsp[0].argValue)) {
						YYLTYPE pos = (yylsp[0]);
						pos.last_column = (yyvsp[0].argValue)->GetRange().right;
						(yyval.argListValue) = CreateNode<ArgListASTNode>((yylsp[0]), pos);
						(yyval.argListValue)->AppendChild((yyvsp[0].argValue));
					}
					else
						(yyval.argListValue) = 0;
				;}
    break;

  case 31:
#line 363 "DeltaParserSpec.y"
    { (yyval.argListValue) = 0; ;}
    break;

  case 32:
#line 364 "DeltaParserSpec.y"
    { (yyval.argListValue) = 0; ;}
    break;

  case 33:
#line 367 "DeltaParserSpec.y"
    { (yyval.argValue) = 0; ;}
    break;

  case 34:
#line 368 "DeltaParserSpec.y"
    { (yyval.argValue) = CreateNode<ArgASTNode>((yylsp[-1]), (yylsp[-1])); ;}
    break;

  case 35:
#line 372 "DeltaParserSpec.y"
    {
					(yyval.argListValue) = (yyvsp[-2].argListValue);
					(yyval.argListValue)->AppendChild(CreateNode<ArgASTNode>((yylsp[0]), (yylsp[0])));
					(yyval.argListValue)->GetRange().right = (yylsp[0]).last_column;
				;}
    break;

  case 36:
#line 377 "DeltaParserSpec.y"
    {
					(yyval.argListValue) = CreateNode<ArgListASTNode>((yylsp[0]), (yylsp[0]));
					(yyval.argListValue)->AppendChild(CreateNode<ArgASTNode>((yylsp[0]), (yylsp[0])));
				;}
    break;

  case 37:
#line 383 "DeltaParserSpec.y"
    {
					(yyval.funcValue) = CreateNode<FunctionASTNode>((yylsp[-4]), (yylsp[0]), (yyvsp[-4].literalValue));
					(yyval.funcValue)->SetChild<0>((yyvsp[-2].funcNameValue));
					(yyval.funcValue)->SetChild<1>((yyvsp[-1].argListValue));
					(yyval.funcValue)->SetChild<2>((yyvsp[0].compValue));
					(yyval.funcValue)->SetChild<3>((yyvsp[-3].unaryKwdValue));
				;}
    break;

  case 38:
#line 392 "DeltaParserSpec.y"
    {
						(yyval.funcValue) = CreateNode<FunctionASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].literalValue));
						(yyval.funcValue)->SetChild<0>(0);		// Anonymous.
						(yyval.funcValue)->SetChild<1>((yyvsp[-1].argListValue));
						(yyval.funcValue)->SetChild<2>((yyvsp[0].nodeValue));
					;}
    break;

  case 39:
#line 400 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[-1].nodeValue); ;}
    break;

  case 40:
#line 406 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<AttributeASTNode>((yylsp[0]), (yylsp[0])); ;}
    break;

  case 41:
#line 407 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<AttributeASTNode>((yylsp[-1]), (yylsp[0])); ;}
    break;

  case 42:
#line 408 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<AttributeASTNode>((yylsp[-1]), (yylsp[0])); ;}
    break;

  case 44:
#line 412 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[-1]), (yylsp[0]), CONST_KEY_DOTID); ;}
    break;

  case 45:
#line 415 "DeltaParserSpec.y"
    {
						(yyval.tableElemValue) =	CreateBinaryNode<TableElemASTNode>(
									(yylsp[-2]), 
									(yylsp[0]), 
									CreateNode<ConstASTNode>((yylsp[-2]), (yylsp[-2]), CONST_STRING), 
									(yyvsp[0].nodeValue)
								);
					;}
    break;

  case 46:
#line 425 "DeltaParserSpec.y"
    {
						(yyval.newAttributeValue) = CreateNode<NewAttributeASTNode>((yylsp[-4]), (yylsp[0]));
						(yyval.newAttributeValue)->SetChild<0>((yyvsp[-4].nodeValue));
						(yyval.newAttributeValue)->SetChild<1>((yyvsp[-2].unaryKwdValue));
						(yyval.newAttributeValue)->SetChild<2>((yyvsp[-1].unaryKwdValue));
					;}
    break;

  case 47:
#line 433 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 48:
#line 434 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].funcValue); ;}
    break;

  case 49:
#line 437 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 50:
#line 440 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 51:
#line 448 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<OtherStmtASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[-1].exprListValue)); ;}
    break;

  case 52:
#line 449 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].unaryKwdValue); ;}
    break;

  case 53:
#line 450 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].whileValue); ;}
    break;

  case 54:
#line 451 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].forValue); ;}
    break;

  case 55:
#line 452 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].foreachValue); ;}
    break;

  case 56:
#line 453 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].ifValue); ;}
    break;

  case 57:
#line 454 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].unaryKwdValue); ;}
    break;

  case 58:
#line 455 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].compValue); ;}
    break;

  case 59:
#line 456 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<OtherStmtASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[-1].leafKwdValue)); ;}
    break;

  case 60:
#line 457 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].tryValue); ;}
    break;

  case 61:
#line 458 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].unaryKwdValue); ;}
    break;

  case 62:
#line 459 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].funcValue); ;}
    break;

  case 63:
#line 460 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<OtherStmtASTNode>((yylsp[0]), (yylsp[0]), 0); ;}
    break;

  case 64:
#line 461 "DeltaParserSpec.y"
    {
			(yyval.nodeValue) = 0;
			ctx.GetProgramDesc().AddNotParsed(Range((yylsp[0]).first_column, (yylsp[0]).last_column));
		;}
    break;

  case 65:
#line 467 "DeltaParserSpec.y"
    { (yyval.leafKwdValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 66:
#line 468 "DeltaParserSpec.y"
    { (yyval.leafKwdValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 67:
#line 471 "DeltaParserSpec.y"
    { (yyval.compValue) = CreateUnaryNode<CompoundASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-1].stmtsValue)); ;}
    break;

  case 68:
#line 472 "DeltaParserSpec.y"
    { (yyval.compValue) = CreateUnaryNode<CompoundASTNode>((yylsp[-1]), (yylsp[0]), 0); ;}
    break;

  case 69:
#line 475 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-1].nodeValue), (yyvsp[-2].literalValue)); ;}
    break;

  case 70:
#line 478 "DeltaParserSpec.y"
    {
					(yyval.whileValue) = CreateNode<WhileASTNode>((yylsp[-4]), (yylsp[0]));
					(yyval.whileValue)->SetChild<0>((yyvsp[-2].nodeValue));
					(yyval.whileValue)->SetChild<1>((yyvsp[0].nodeValue));
				;}
    break;

  case 71:
#line 485 "DeltaParserSpec.y"
    {
				(yyval.ifValue) = CreateNode<IfASTNode>((yylsp[-5]), (yylsp[0]));
				(yyval.ifValue)->SetChild<0>((yyvsp[-3].nodeValue));
				(yyval.ifValue)->SetChild<1>((yyvsp[-1].nodeValue));
				(yyval.ifValue)->SetChild<2>((yyvsp[0].elseValue));
			;}
    break;

  case 72:
#line 493 "DeltaParserSpec.y"
    { (yyval.elseValue) = CreateUnaryNode<ElseASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[0].nodeValue)); ;}
    break;

  case 73:
#line 494 "DeltaParserSpec.y"
    { (yyval.elseValue) = 0; ;}
    break;

  case 74:
#line 497 "DeltaParserSpec.y"
    {
				(yyval.forValue) = CreateNode<ForASTNode>((yylsp[-8]), (yylsp[0]));
				(yyval.forValue)->SetChild<0>((yyvsp[-6].exprListValue));
				(yyval.forValue)->SetChild<1>((yyvsp[-4].nodeValue));
				(yyval.forValue)->SetChild<2>((yyvsp[-2].exprListValue));
				(yyval.forValue)->SetChild<3>((yyvsp[0].nodeValue));
			;}
    break;

  case 75:
#line 506 "DeltaParserSpec.y"
    { (yyval.exprListValue) = (yyvsp[0].exprListValue); ;}
    break;

  case 76:
#line 507 "DeltaParserSpec.y"
    { (yyval.exprListValue) = 0; ;}
    break;

  case 77:
#line 510 "DeltaParserSpec.y"
    { (yyval.exprListValue) = (yyvsp[0].exprListValue); ;}
    break;

  case 78:
#line 511 "DeltaParserSpec.y"
    { (yyval.exprListValue) = 0; ;}
    break;

  case 79:
#line 514 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[-1].nodeValue); ;}
    break;

  case 80:
#line 515 "DeltaParserSpec.y"
    { (yyval.nodeValue) = 0; ;}
    break;

  case 81:
#line 518 "DeltaParserSpec.y"
    {
					(yyval.foreachValue) = CreateNode<ForeachASTNode>((yylsp[-6]), (yylsp[0]));
					(yyval.foreachValue)->SetChild<0>((yyvsp[-4].nodeValue));
					(yyval.foreachValue)->SetChild<1>((yyvsp[-2].nodeValue));
					(yyval.foreachValue)->SetChild<2>((yyvsp[0].nodeValue));
					(yyval.foreachValue)->SetChild<3>((yyvsp[-3].nodeValue));
				;}
    break;

  case 82:
#line 527 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-1].nodeValue), (yyvsp[-2].literalValue)); ;}
    break;

  case 83:
#line 528 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[-1]), (yylsp[0]), 0, (yyvsp[-1].literalValue)); ;}
    break;

  case 84:
#line 529 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[0].funcValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 85:
#line 532 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-1].nodeValue), (yyvsp[-2].literalValue)); ;}
    break;

  case 86:
#line 535 "DeltaParserSpec.y"
    {
				(yyval.tryValue) = CreateNode<TryASTNode>((yylsp[-2]), (yylsp[0]));
				(yyval.tryValue)->SetChild<0>((yyvsp[-1].nodeValue));
				(yyval.tryValue)->SetChild<1>((yyvsp[0].trapValue));
			;}
    break;

  case 87:
#line 542 "DeltaParserSpec.y"
    {
				(yyval.trapValue) = CreateNode<TrapASTNode>((yylsp[-2]), (yylsp[0]));
				(yyval.trapValue)->SetChild<0>(CreateNode<ArgASTNode>((yylsp[-1]), (yylsp[-1])));
				(yyval.trapValue)->SetChild<1>((yyvsp[0].nodeValue));
			;}
    break;

  case 88:
#line 552 "DeltaParserSpec.y"
    {
						(yyval.usingValue) = CreateNode<UsingASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].literalValue));
						(yyval.usingValue)->SetChild<0>((yyvsp[-1].nodeValue));
					;}
    break;

  case 89:
#line 556 "DeltaParserSpec.y"
    {
						(yyval.usingValue) = CreateNode<UsingASTNode>((yylsp[-3]), (yylsp[0]), (yyvsp[-3].literalValue));
						(yyval.usingValue)->SetChild<1>(CreateNode<StringifiedIdASTNode>((yylsp[-2]), (yylsp[-1])));
					;}
    break;

  case 90:
#line 562 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 91:
#line 563 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 92:
#line 567 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), CreateNode<VariableASTNode>((yylsp[0]), (yylsp[0])), (yyvsp[-1].literalValue)); ;}
    break;

  case 93:
#line 570 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 94:
#line 571 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<VariableASTNode>((yylsp[0]), (yylsp[0])); ;}
    break;

  case 95:
#line 574 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 96:
#line 580 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].binaryOpValue); ;}
    break;

  case 97:
#line 581 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].binaryOpValue); ;}
    break;

  case 98:
#line 582 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].binaryOpValue); ;}
    break;

  case 99:
#line 583 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].binaryOpValue); ;}
    break;

  case 100:
#line 584 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].binaryOpValue); ;}
    break;

  case 101:
#line 585 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].ternaryOpValue); ;}
    break;

  case 102:
#line 586 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 103:
#line 589 "DeltaParserSpec.y"
    {
								UnaryKwdASTNode* kwd =
									CreateUnaryNode<UnaryKwdASTNode>((yylsp[-3]), (yylsp[-2]), CreateNode<VariableASTNode>((yylsp[-2]), (yylsp[-2])), (yyvsp[-3].literalValue));
								(yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-3]), (yylsp[0]), kwd, (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); 
							;}
    break;

  case 104:
#line 596 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 105:
#line 597 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 106:
#line 598 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 107:
#line 599 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 108:
#line 600 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 109:
#line 601 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 110:
#line 605 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 111:
#line 607 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 112:
#line 609 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 113:
#line 611 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 114:
#line 613 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 115:
#line 615 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 116:
#line 619 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 117:
#line 621 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 118:
#line 625 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 119:
#line 627 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 120:
#line 629 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 121:
#line 631 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 122:
#line 633 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 123:
#line 636 "DeltaParserSpec.y"
    {
							(yyval.ternaryOpValue) = CreateNode<TernaryOpASTNode>((yylsp[-6]), (yylsp[0]));
							(yyval.ternaryOpValue)->SetChild<0>((yyvsp[-5].nodeValue));
							(yyval.ternaryOpValue)->SetChild<1>((yyvsp[-3].nodeValue));
							(yyval.ternaryOpValue)->SetChild<2>((yyvsp[-1].nodeValue));
						;}
    break;

  case 124:
#line 644 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<SuffixOpASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[-1].nodeValue), (yyvsp[0].literalValue)); ;}
    break;

  case 125:
#line 645 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<PrefixOpASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 126:
#line 646 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<SuffixOpASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[-1].nodeValue), (yyvsp[0].literalValue)); ;}
    break;

  case 127:
#line 647 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<PrefixOpASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 128:
#line 648 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<PrefixOpASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 129:
#line 649 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<PrefixOpASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 130:
#line 650 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 131:
#line 653 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 132:
#line 654 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 133:
#line 655 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].funcValue); ;}
    break;

  case 134:
#line 658 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 135:
#line 659 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].callValue); ;}
    break;

  case 136:
#line 660 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].tableCtorValue); ;}
    break;

  case 137:
#line 661 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-1].funcValue), "()"); ;}
    break;

  case 138:
#line 662 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-1].nodeValue), "()"); ;}
    break;

  case 139:
#line 663 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 140:
#line 664 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 141:
#line 665 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 142:
#line 666 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 143:
#line 667 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 144:
#line 670 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<ConstASTNode>((yylsp[0]), (yylsp[0]), CONST_NUMBER); ;}
    break;

  case 145:
#line 671 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 146:
#line 672 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 147:
#line 673 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 184:
#line 722 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<ConstASTNode>((yylsp[0]), (yylsp[0]), CONST_STRING); ;}
    break;

  case 185:
#line 723 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<StringifiedIdASTNode>((yylsp[0]), (yylsp[0])); ;}
    break;

  case 186:
#line 724 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<StringifiedIdASTNode>((yylsp[0]), (yylsp[0])); ;}
    break;

  case 212:
#line 757 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 213:
#line 760 "DeltaParserSpec.y"
    {
						(yyval.exprListValue) = (yyvsp[-2].exprListValue);
						(yyval.exprListValue)->AppendChild((yyvsp[0].nodeValue));
						(yyval.exprListValue)->GetRange().right = (yylsp[0]).last_column;
					;}
    break;

  case 214:
#line 765 "DeltaParserSpec.y"
    {
						(yyval.exprListValue) = CreateNode<ExpressionListASTNode>((yylsp[0]), (yylsp[0]));
						(yyval.exprListValue)->AppendChild((yyvsp[0].nodeValue));
					;}
    break;

  case 215:
#line 771 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 216:
#line 772 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-1].nodeValue), "||"); ;}
    break;

  case 217:
#line 773 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[0]), (yylsp[0]), (yyvsp[0].literalValue)); ;}
    break;

  case 218:
#line 774 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].funcValue); ;}
    break;

  case 219:
#line 777 "DeltaParserSpec.y"
    {
						(yyval.callValue) = CreateNode<CallASTNode>((yylsp[-3]), (yylsp[0]));
						(yyval.callValue)->SetChild<0>((yyvsp[-3].nodeValue));
						(yyval.callValue)->SetChild<1>((yyvsp[-1].exprListValue));
					;}
    break;

  case 220:
#line 782 "DeltaParserSpec.y"
    {
						(yyval.callValue) = CreateNode<CallASTNode>((yylsp[-2]), (yylsp[0]));
						(yyval.callValue)->SetChild<0>((yyvsp[-2].nodeValue));
					;}
    break;

  case 221:
#line 786 "DeltaParserSpec.y"
    {
						(yyval.callValue) = CreateNode<CallASTNode>((yylsp[-1]), (yylsp[0]));
						(yyval.callValue)->SetChild<0>((yyvsp[-1].nodeValue));
					;}
    break;

  case 222:
#line 792 "DeltaParserSpec.y"
    {
						(yyval.exprListValue) = (yyvsp[-2].exprListValue);
						(yyval.exprListValue)->AppendChild((yyvsp[0].nodeValue));
						(yyval.exprListValue)->GetRange().right = (yylsp[0]).last_column;
					;}
    break;

  case 223:
#line 797 "DeltaParserSpec.y"
    {
						(yyval.exprListValue) = CreateNode<ExpressionListASTNode>((yylsp[0]), (yylsp[0]));
						(yyval.exprListValue)->AppendChild((yyvsp[0].nodeValue));
					;}
    break;

  case 224:
#line 806 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 225:
#line 807 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<AttributeASTNode>((yylsp[0]), (yylsp[0])); ;}
    break;

  case 226:
#line 808 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[-1]), (yylsp[0]), CreateNode<VariableASTNode>((yylsp[0]), (yylsp[0])), (yyvsp[-1].literalValue)); ;}
    break;

  case 227:
#line 809 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[-1]), (yylsp[0]), CreateNode<VariableASTNode>((yylsp[0]), (yylsp[0])), (yyvsp[-1].literalValue)); ;}
    break;

  case 228:
#line 810 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].binaryOpValue); ;}
    break;

  case 229:
#line 816 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 230:
#line 820 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[0]), (yylsp[0]), CONST_KEY_IDENT); ;}
    break;

  case 231:
#line 821 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[0]), (yylsp[0]), CONST_KEY_IDENT); ;}
    break;

  case 232:
#line 822 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[0]), (yylsp[0]), CONST_KEY_STRING); ;}
    break;

  case 233:
#line 823 "DeltaParserSpec.y"
    { (yyval.nodeValue) = ExpressionToConstKey((yylsp[0]), (yyvsp[0].nodeValue)); ;}
    break;

  case 234:
#line 827 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[0]), (yylsp[0]), CONST_KEY_STRING); ++((yyval.nodeValue)->GetRange().left); ;}
    break;

  case 235:
#line 829 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[0]), (yylsp[0]), CONST_KEY_STRING); ++((yyval.nodeValue)->GetRange().left);;}
    break;

  case 236:
#line 831 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[0]), (yylsp[0]), CONST_KEY_STRING); ++((yyval.nodeValue)->GetRange().left);;}
    break;

  case 237:
#line 833 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[0]), (yylsp[0]), CONST_KEY_STRING); ++((yyval.nodeValue)->GetRange().left);;}
    break;

  case 238:
#line 836 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 239:
#line 837 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-1]), (yylsp[0]), (yyvsp[-1].nodeValue), (yyvsp[0].nodeValue), "."); ;}
    break;

  case 240:
#line 838 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-2].nodeValue), (yyvsp[0].nodeValue), (yyvsp[-1].literalValue)); ;}
    break;

  case 241:
#line 840 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-3]), (yylsp[0]), (yyvsp[-3].nodeValue), ExpressionToConstKey((yylsp[-1]), (yyvsp[-1].nodeValue)), "[]"); ;}
    break;

  case 242:
#line 842 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-3]), (yylsp[0]), (yyvsp[-3].nodeValue), CreateNode<LeafKwdASTNode>((yylsp[-1]), (yylsp[-1])), "[]"); ;}
    break;

  case 243:
#line 844 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-3]), (yylsp[0]), (yyvsp[-3].nodeValue), ExpressionToConstKey((yylsp[-1]), (yyvsp[-1].nodeValue)), "[[]]"); ;}
    break;

  case 244:
#line 846 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[-3]), (yylsp[0]), (yyvsp[-3].nodeValue), CreateNode<LeafKwdASTNode>((yylsp[-1]), (yylsp[-1])), "[[]]"); ;}
    break;

  case 245:
#line 849 "DeltaParserSpec.y"
    { (yyval.tableCtorValue) = CreateUnaryNode<TableConstructASTNode>((yylsp[-2]), (yylsp[0]), (yyvsp[-1].tableElemsValue)); ;}
    break;

  case 246:
#line 850 "DeltaParserSpec.y"
    { (yyval.tableCtorValue) = CreateUnaryNode<TableConstructASTNode>((yylsp[-1]), (yylsp[0]), 0); ;}
    break;

  case 247:
#line 853 "DeltaParserSpec.y"
    {
						(yyval.tableElemsValue) = (yyvsp[-2].tableElemsValue);
						(yyval.tableElemsValue)->AppendChild((yyvsp[0].tableElemValue));
						(yyval.tableElemsValue)->GetRange().right = (yylsp[0]).last_column;
					;}
    break;

  case 248:
#line 858 "DeltaParserSpec.y"
    {
						(yyval.tableElemsValue) = CreateNode<TableElemsASTNode>((yylsp[0]), (yylsp[0]));
						(yyval.tableElemsValue)->AppendChild((yyvsp[0].tableElemValue));
					;}
    break;

  case 249:
#line 864 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = CreateBinaryNode<TableElemASTNode>((yylsp[0]), (yylsp[0]), 0, (yyvsp[0].nodeValue));	;}
    break;

  case 250:
#line 865 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = CreateBinaryNode<TableElemASTNode>((yylsp[0]), (yylsp[0]), 0, (yyvsp[0].funcValue));	;}
    break;

  case 251:
#line 866 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = CreateBinaryNode<TableElemASTNode>((yylsp[-4]), (yylsp[0]), (yyvsp[-3].tableIListVlaue), (yyvsp[-1].exprListValue));	;}
    break;

  case 252:
#line 867 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = CreateBinaryNode<TableElemASTNode>((yylsp[0]), (yylsp[0]), 0, (yyvsp[0].newAttributeValue));	;}
    break;

  case 253:
#line 868 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = (yyvsp[0].tableElemValue);;}
    break;

  case 254:
#line 871 "DeltaParserSpec.y"
    {
						(yyval.exprListValue) = (yyvsp[-2].exprListValue);
						(yyval.exprListValue)->AppendChild((yyvsp[0].nodeValue));
						(yyval.exprListValue)->GetRange().right = (yylsp[0]).last_column;
					;}
    break;

  case 255:
#line 876 "DeltaParserSpec.y"
    {
						(yyval.exprListValue) = CreateNode<ExpressionListASTNode>((yylsp[0]), (yylsp[0]));
						(yyval.exprListValue)->AppendChild((yyvsp[0].nodeValue));
					;}
    break;

  case 256:
#line 882 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].nodeValue); ;}
    break;

  case 257:
#line 883 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[0].funcValue); ;}
    break;

  case 258:
#line 886 "DeltaParserSpec.y"
    { (yyval.nodeValue) = ExpressionToConstKey((yylsp[0]), (yyvsp[0].nodeValue)); ;}
    break;

  case 259:
#line 887 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[-1]), (yylsp[0]), CONST_KEY_DOTID); ;}
    break;

  case 260:
#line 888 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[0]), (yylsp[0]), CONST_KEY_OPSTRING); ;}
    break;

  case 261:
#line 891 "DeltaParserSpec.y"
    {
					(yyval.tableIListVlaue) = (yyvsp[-2].tableIListVlaue);
					(yyval.tableIListVlaue)->AppendChild((yyvsp[0].nodeValue));
					(yyval.tableIListVlaue)->GetRange().right = (yylsp[0]).last_column;
				;}
    break;

  case 262:
#line 896 "DeltaParserSpec.y"
    {
					(yyval.tableIListVlaue) = CreateNode<TableIndexListASTNode>((yylsp[0]), (yylsp[0]));
					(yyval.tableIListVlaue)->AppendChild((yyvsp[0].nodeValue));
				;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 3558 "DeltaParserGen.cpp"

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
	      yyerror (&yylloc, ctx, yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (&yylloc, ctx, YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (&yylloc, ctx, YY_("syntax error"));
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
  yyerror (&yylloc, ctx, YY_("memory exhausted"));
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


#line 901 "DeltaParserSpec.y"


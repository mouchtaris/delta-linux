
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



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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
 *  Added metaprogramming support, Yannis Lilis, August 2011.
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

////////////////////////////////////////////////////////////////////////

QuotedElementsASTNode* ExpressionListToQuotedElementList (ExpressionListASTNode* node)
{
	assert(node);
	QuotedElementsASTNode* result = new QuotedElementsASTNode(node->GetRange());
	for (ExpressionListASTNode::Iterator i = node->Begin(); i != node->End(); ++i)
		result->AppendChild((*i)->Clone());
	delete node;
	return result;
}

//**********************************************************************

#define YYERROR_VERBOSE 1

////////////////////////////////////////////////////////////////////////



/* Line 189 of yacc.c  */
#line 167 "DeltaParserGen.cpp"

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
     META_LSHIFT = 335,
     META_RSHIFT = 336,
     META_ESCAPE = 337,
     META_INLINE = 338,
     META_EXECUTE = 339,
     META_RENAME = 340,
     NUMBER_CONST = 341,
     STRING_CONST = 342,
     IDENT = 343,
     ATTRIBUTE_IDENT = 344,
     OPERATOR = 345,
     PARENTHESIS = 346,
     SQUARE_BRACKETS = 347,
     COMMENT = 348,
     INCOMPLETE_COMMENT = 349,
     INCOMPLETE_STRING = 350,
     CPP = 351,
     PARSE_STMT = 352,
     PARSE_FUNCTION_CALL_OBJECT = 353,
     PARSE_TABLE_OBJECT = 354,
     PARSE_NAMESPACE = 355,
     TOKEN_LAST = 356,
     UMINUS = 357
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 102 "DeltaParserSpec.y"

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
	delta::TableIndexListASTNode*	tableIListValue;
	delta::TableConstructASTNode*	tableCtorValue;
	delta::UsingASTNode*			usingValue;
	
	delta::QuotedElementsASTNode*	elementsValue;



/* Line 214 of yacc.c  */
#line 354 "DeltaParserGen.cpp"
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


/* Line 264 of yacc.c  */
#line 379 "DeltaParserGen.cpp"

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
#define YYFINAL  234
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2685

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  114
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  98
/* YYNRULES -- Number of rules.  */
#define YYNRULES  311
/* YYNRULES -- Number of states.  */
#define YYNSTATES  470

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   357

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      86,    87,     2,     2,    94,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    93,   112,
       2,     2,     2,    92,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    88,     2,    89,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    90,   113,    91,     2,     2,     2,     2,
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
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,    10,    14,    18,    22,    24,
      25,    28,    30,    33,    35,    37,    39,    41,    43,    45,
      46,    49,    51,    53,    56,    58,    59,    61,    63,    65,
      69,    72,    74,    75,    77,    80,    84,    86,    89,    91,
      93,    99,   103,   107,   109,   112,   115,   118,   121,   123,
     125,   129,   135,   137,   139,   142,   145,   147,   149,   151,
     153,   156,   158,   160,   162,   164,   166,   168,   171,   173,
     175,   177,   179,   181,   183,   185,   187,   191,   194,   198,
     204,   211,   214,   215,   225,   227,   228,   230,   231,   235,
     237,   245,   249,   252,   255,   259,   263,   267,   271,   276,
     281,   283,   285,   289,   291,   293,   296,   298,   300,   302,
     304,   306,   308,   310,   315,   319,   323,   327,   331,   335,
     339,   343,   347,   351,   355,   359,   363,   367,   371,   375,
     379,   383,   387,   391,   399,   402,   405,   408,   411,   414,
     417,   419,   421,   423,   425,   427,   429,   431,   433,   437,
     441,   443,   445,   447,   449,   451,   453,   455,   457,   460,
     462,   464,   466,   468,   470,   472,   474,   476,   478,   480,
     482,   484,   486,   488,   490,   492,   494,   496,   498,   500,
     502,   504,   506,   508,   510,   512,   514,   516,   518,   520,
     522,   525,   529,   532,   535,   537,   539,   541,   543,   545,
     547,   549,   551,   553,   555,   557,   559,   561,   563,   565,
     567,   569,   571,   573,   575,   577,   579,   581,   583,   585,
     587,   589,   591,   593,   595,   599,   601,   603,   605,   609,
     611,   613,   618,   622,   625,   629,   631,   633,   635,   638,
     641,   643,   645,   647,   649,   651,   653,   655,   657,   659,
     661,   665,   668,   672,   677,   682,   687,   692,   696,   699,
     703,   705,   707,   709,   711,   713,   715,   719,   721,   723,
     725,   727,   729,   731,   735,   737,   743,   746,   748,   753,
     756,   761,   763,   765,   767,   769,   771,   773,   777,   781,
     785,   787,   789,   791,   793,   796,   798,   800,   803,   805,
     807,   809,   813,   816,   818,   820,   823,   826,   828,   830,
     832,   836
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     116,     0,    -1,    -1,   117,    -1,   106,   115,   139,    -1,
     107,   115,   171,    -1,   108,   115,   171,    -1,   109,   115,
     158,    -1,   118,    -1,    -1,   118,   139,    -1,   139,    -1,
      85,    97,    -1,    97,    -1,   200,    -1,     4,    -1,     9,
      -1,    10,    -1,    28,    -1,    -1,    85,    97,    -1,    97,
      -1,   200,    -1,    99,   123,    -1,    74,    -1,    -1,   179,
      -1,    61,    -1,    62,    -1,    86,   126,   125,    -1,    86,
     125,    -1,    57,    -1,    -1,    87,    -1,    64,    87,    -1,
     126,    94,   127,    -1,   127,    -1,    85,    97,    -1,    97,
      -1,   200,    -1,   120,   121,   122,   124,   143,    -1,     7,
     124,   130,    -1,    90,   162,    91,    -1,    98,    -1,    74,
      97,    -1,    74,   200,    -1,    74,   174,    -1,    61,   175,
      -1,   131,    -1,   132,    -1,   133,    93,   136,    -1,   131,
      90,   137,   138,    91,    -1,   162,    -1,   128,    -1,    72,
     136,    -1,    73,   136,    -1,   141,    -1,   140,    -1,   143,
      -1,   128,    -1,   185,   112,    -1,   144,    -1,   145,    -1,
     148,    -1,   152,    -1,   146,    -1,   153,    -1,   142,   112,
      -1,   155,    -1,   154,    -1,   112,    -1,   157,    -1,   211,
      -1,     1,    -1,    26,    -1,    27,    -1,    90,   118,    91,
      -1,    90,    91,    -1,     6,   162,   112,    -1,    17,    86,
     162,    87,   139,    -1,    15,    86,   162,    87,   139,   147,
      -1,    16,   139,    -1,    -1,    18,    86,   149,   112,   162,
     112,   150,    87,   139,    -1,   185,    -1,    -1,   185,    -1,
      -1,    93,   186,    94,    -1,    94,    -1,    19,    86,   186,
     151,   162,    87,   139,    -1,     5,   162,   112,    -1,     5,
     112,    -1,     5,   128,    -1,    31,   162,   112,    -1,    29,
     139,   156,    -1,    30,   119,   139,    -1,    32,   158,   112,
      -1,    32,    75,    97,   112,    -1,    32,    75,   200,   112,
      -1,   160,    -1,   161,    -1,   160,    58,   119,    -1,   159,
      -1,   119,    -1,    58,   160,    -1,   164,    -1,   163,    -1,
     165,    -1,   166,    -1,   167,    -1,   168,    -1,   169,    -1,
      14,   119,    44,   162,    -1,   186,    44,   162,    -1,   186,
      67,   162,    -1,   186,    69,   162,    -1,   186,    68,   162,
      -1,   186,    70,   162,    -1,   186,    71,   162,    -1,   162,
      38,   162,    -1,   162,    39,   162,    -1,   162,    42,   162,
      -1,   162,    43,   162,    -1,   162,    41,   162,    -1,   162,
      40,   162,    -1,   162,    21,   162,    -1,   162,    23,   162,
      -1,   162,    33,   162,    -1,   162,    34,   162,    -1,   162,
      36,   162,    -1,   162,    35,   162,    -1,   162,    37,   162,
      -1,    86,   162,    92,   162,    93,   162,    87,    -1,   186,
      59,    -1,    59,   186,    -1,   186,    60,    -1,    60,   186,
      -1,    34,   162,    -1,    22,   162,    -1,   170,    -1,   210,
      -1,   180,    -1,   172,    -1,   129,    -1,   186,    -1,   184,
      -1,   191,    -1,    86,   128,    87,    -1,    86,   162,    87,
      -1,    11,    -1,    12,    -1,     8,    -1,    76,    -1,    95,
      -1,    20,    -1,    24,    -1,    25,    -1,   173,    96,    -1,
      96,    -1,    15,    -1,    16,    -1,     4,    -1,     5,    -1,
       9,    -1,    17,    -1,    18,    -1,    19,    -1,    20,    -1,
      28,    -1,    21,    -1,    22,    -1,    23,    -1,     7,    -1,
      29,    -1,    30,    -1,    31,    -1,    32,    -1,     6,    -1,
      24,    -1,    25,    -1,    26,    -1,    27,    -1,    13,    -1,
      14,    -1,    10,    -1,    11,    -1,    12,    -1,    97,    -1,
     174,    -1,    75,   175,    -1,   176,    61,   175,    -1,    75,
     159,    -1,    75,   161,    -1,   173,    -1,   176,    -1,   177,
      -1,    33,    -1,    34,    -1,    35,    -1,    36,    -1,    37,
      -1,    38,    -1,    39,    -1,    40,    -1,    41,    -1,    42,
      -1,    43,    -1,    44,    -1,    45,    -1,    46,    -1,    47,
      -1,    48,    -1,    49,    -1,    50,    -1,    51,    -1,    52,
      -1,    53,    -1,    54,    -1,    55,    -1,    57,    -1,    56,
      -1,   171,    -1,   178,    -1,   181,    94,   182,    -1,   182,
      -1,   162,    -1,   183,    -1,   113,   162,   113,    -1,    64,
      -1,   128,    -1,   180,    86,   181,    87,    -1,   180,    86,
      87,    -1,   180,    57,    -1,   185,    94,   162,    -1,   162,
      -1,   158,    -1,   131,    -1,    13,   119,    -1,    28,   119,
      -1,   190,    -1,   171,    -1,    97,    -1,   174,    -1,   179,
      -1,   178,    -1,    62,    -1,    77,    -1,    78,    -1,    79,
      -1,   187,    61,   188,    -1,   187,   189,    -1,   187,    63,
     188,    -1,   187,    88,   162,    89,    -1,   187,    88,   123,
      89,    -1,   187,    65,   162,    66,    -1,   187,    65,   123,
      66,    -1,    88,   192,    89,    -1,    88,    89,    -1,   192,
      94,   193,    -1,   193,    -1,   162,    -1,   128,    -1,   198,
      -1,   135,    -1,   134,    -1,   194,    94,   195,    -1,   195,
      -1,   162,    -1,   128,    -1,   162,    -1,   132,    -1,   179,
      -1,   197,    94,   196,    -1,   196,    -1,    90,   197,    93,
     194,    91,    -1,   199,    82,    -1,    82,    -1,   199,    86,
     162,    87,    -1,   199,    97,    -1,    83,    86,   162,    87,
      -1,   198,    -1,   135,    -1,   134,    -1,   132,    -1,   179,
      -1,   183,    -1,   185,    94,   201,    -1,   202,    94,   201,
      -1,   202,    94,   162,    -1,   201,    -1,   202,    -1,   185,
      -1,   141,    -1,   197,   112,    -1,   143,    -1,   128,    -1,
     205,   204,    -1,   204,    -1,   141,    -1,   140,    -1,    90,
     205,    91,    -1,    90,    91,    -1,   206,    -1,   128,    -1,
     208,   207,    -1,   207,   207,    -1,   203,    -1,   206,    -1,
     208,    -1,    80,   209,    81,    -1,    84,   139,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   293,   293,   302,   304,   305,   306,   307,   310,   311,
     314,   315,   318,   319,   320,   326,   327,   328,   331,   332,
     335,   336,   337,   338,   339,   340,   343,   344,   345,   348,
     355,   365,   366,   369,   370,   374,   375,   378,   379,   380,
     383,   392,   400,   406,   407,   408,   409,   412,   415,   416,
     419,   430,   438,   439,   442,   445,   451,   452,   453,   454,
     457,   460,   461,   462,   463,   464,   465,   466,   467,   468,
     469,   470,   471,   472,   478,   479,   482,   483,   486,   489,
     496,   504,   505,   508,   517,   518,   521,   522,   525,   526,
     529,   538,   539,   540,   543,   546,   553,   563,   567,   571,
     577,   578,   581,   585,   586,   589,   595,   596,   597,   598,
     599,   600,   601,   604,   610,   611,   612,   613,   614,   615,
     618,   620,   622,   624,   626,   628,   632,   634,   638,   640,
     642,   644,   646,   650,   658,   659,   660,   661,   662,   663,
     664,   665,   668,   669,   670,   673,   674,   675,   676,   677,
     678,   679,   680,   681,   684,   685,   686,   687,   690,   691,
     694,   695,   696,   697,   698,   699,   700,   701,   702,   703,
     704,   705,   706,   707,   708,   709,   710,   711,   712,   713,
     714,   715,   716,   717,   718,   719,   720,   721,   724,   725,
     728,   729,   732,   733,   736,   737,   738,   741,   742,   743,
     744,   745,   746,   747,   748,   749,   750,   751,   752,   753,
     754,   755,   756,   757,   758,   759,   760,   761,   762,   763,
     764,   765,   771,   772,   775,   776,   779,   780,   783,   784,
     785,   788,   793,   797,   803,   804,   810,   811,   812,   813,
     814,   820,   824,   825,   826,   827,   830,   832,   834,   836,
     840,   841,   842,   843,   845,   847,   849,   853,   854,   857,
     858,   861,   862,   863,   864,   865,   868,   869,   872,   873,
     876,   877,   878,   881,   882,   885,   891,   892,   895,   896,
     898,   901,   902,   903,   904,   905,   906,   909,   911,   913,
     915,   918,   919,   922,   923,   924,   925,   928,   929,   932,
     933,   934,   935,   938,   939,   942,   943,   949,   950,   951,
     954,   957
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
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
  "DOT_CAST", "DOT_EQUAL_RHS", "META_LSHIFT", "META_RSHIFT", "META_ESCAPE",
  "META_INLINE", "META_EXECUTE", "META_RENAME", "'('", "')'", "'['", "']'",
  "'{'", "'}'", "'?'", "':'", "','", "NUMBER_CONST", "STRING_CONST",
  "IDENT", "ATTRIBUTE_IDENT", "OPERATOR", "PARENTHESIS", "SQUARE_BRACKETS",
  "COMMENT", "INCOMPLETE_COMMENT", "INCOMPLETE_STRING", "CPP",
  "PARSE_STMT", "PARSE_FUNCTION_CALL_OBJECT", "PARSE_TABLE_OBJECT",
  "PARSE_NAMESPACE", "TOKEN_LAST", "UMINUS", "';'", "'|'", "$accept",
  "NullifyTree", "DeltaCode", "CodeDefs", "Stmts", "Ident", "FuncClass",
  "FunctionLinkage", "FunctionName", "OperatorMethod", "FormalArgs",
  "FormalArgsSuffix", "FormalArgList", "FormalArg", "Function",
  "LambdaFunction", "LambdaCode", "AttributeId", "DottedIdent",
  "IdentIndex", "IdentIndexElement", "NewAttribute", "AttrFunction",
  "AttributeSet", "AttributeGet", "Stmt", "ExpressionListStmt",
  "NonExprListCompoundOrFunctionStmt", "LoopCtrlStmt", "Compound",
  "AssertStmt", "WhileStmt", "IfStmt", "ElseStmt", "ForStmt",
  "ForInitList", "ForRepeatList", "ForeachOpt", "ForeachStmt",
  "ReturnStmt", "ThrowStmt", "TryStmt", "TrapStmt", "UsingDirective",
  "QualifiedId", "PureQualifiedId", "NonGlobalQualifiedId",
  "GlobalQualifiedId", "Expression", "ConstAssignExpression",
  "AssignExpression", "RelationalExpression", "BooleanExpression",
  "ArithmeticExpression", "TernaryExpression", "Term", "Primary",
  "FunctionAndTableObject", "ConstValueExceptString", "StringConst",
  "KwdIdent", "StringIdent", "StringifyDottedIdents",
  "StringifyNamespaceIdent", "StringConstUsed", "OpString",
  "FunctionCallObject", "ActualArguments", "ActualArgument",
  "NonExpressionActualArgument", "FunctionCall", "ExpressionList",
  "Lvalue", "TableObject", "DotIndex", "DottedOpString", "TableContent",
  "TableConstructor", "TableElements", "TableElement", "ContentList",
  "ContentExpression", "IndexExpression", "IndexedList", "IndexedValues",
  "MultipleEscapes", "MetaGeneratedCode", "NonExpressionElement",
  "QuotedElementList", "QuotedElements", "SpecialExprListStmt",
  "SpecialExprListStmts", "NonFunctionQuotedStmt", "QuotedStmt",
  "TwoOrMoreQuotedStmts", "QuasiQuoted", "MetaExpression", "MetaStmt", 0
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
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,    40,    41,    91,    93,
     123,   125,    63,    58,    44,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,    59,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   114,   115,   116,   116,   116,   116,   116,   117,   117,
     118,   118,   119,   119,   119,   120,   120,   120,   121,   121,
     122,   122,   122,   122,   122,   122,   123,   123,   123,   124,
     124,   124,   124,   125,   125,   126,   126,   127,   127,   127,
     128,   129,   130,   131,   131,   131,   131,   132,   133,   133,
     134,   135,   136,   136,   137,   138,   139,   139,   139,   139,
     140,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   142,   142,   143,   143,   144,   145,
     146,   147,   147,   148,   149,   149,   150,   150,   151,   151,
     152,   153,   153,   153,   154,   155,   156,   157,   157,   157,
     158,   158,   159,   160,   160,   161,   162,   162,   162,   162,
     162,   162,   162,   163,   164,   164,   164,   164,   164,   164,
     165,   165,   165,   165,   165,   165,   166,   166,   167,   167,
     167,   167,   167,   168,   169,   169,   169,   169,   169,   169,
     169,   169,   170,   170,   170,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   172,   172,   172,   172,   173,   173,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   175,   175,
     176,   176,   177,   177,   178,   178,   178,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   180,   180,   181,   181,   182,   182,   183,   183,
     183,   184,   184,   184,   185,   185,   186,   186,   186,   186,
     186,   187,   188,   188,   188,   188,   189,   189,   189,   189,
     190,   190,   190,   190,   190,   190,   190,   191,   191,   192,
     192,   193,   193,   193,   193,   193,   194,   194,   195,   195,
     196,   196,   196,   197,   197,   198,   199,   199,   200,   200,
     200,   201,   201,   201,   201,   201,   201,   202,   202,   202,
     202,   203,   203,   204,   204,   204,   204,   205,   205,   206,
     206,   206,   206,   207,   207,   208,   208,   209,   209,   209,
     210,   211
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     3,     3,     3,     3,     1,     0,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     0,
       2,     1,     1,     2,     1,     0,     1,     1,     1,     3,
       2,     1,     0,     1,     2,     3,     1,     2,     1,     1,
       5,     3,     3,     1,     2,     2,     2,     2,     1,     1,
       3,     5,     1,     1,     2,     2,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     2,     3,     5,
       6,     2,     0,     9,     1,     0,     1,     0,     3,     1,
       7,     3,     2,     2,     3,     3,     3,     3,     4,     4,
       1,     1,     3,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     4,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     7,     2,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     1,     3,     1,
       1,     4,     3,     2,     3,     1,     1,     1,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     2,     3,     4,     4,     4,     4,     3,     2,     3,
       1,     1,     1,     1,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     3,     1,     5,     2,     1,     4,     2,
       4,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       1,     1,     1,     1,     2,     1,     1,     2,     1,     1,
       1,     3,     2,     1,     1,     2,     2,     1,     1,     1,
       3,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,    73,    15,     0,     0,    32,   152,    16,    17,   150,
     151,     0,     0,     0,     0,     0,     0,   155,     0,   156,
     157,    74,    75,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   153,     0,   277,     0,     0,     0,     0,
       0,     0,   154,   159,    13,    43,     2,     2,     2,     2,
      70,     0,     3,     0,   104,    19,    59,   144,   237,    11,
      57,    56,     0,    58,    61,    62,    65,    63,    64,    66,
      69,    68,    71,   236,   103,   100,   101,   235,   107,   106,
     108,   109,   110,   111,   112,   140,   222,   143,   194,   195,
     196,   223,   142,   146,     0,   145,     0,   240,   147,     0,
      14,   141,    72,    92,    93,     0,     0,    31,     0,     0,
     238,     0,     0,     0,    85,     0,   139,   239,     0,     0,
       0,     0,   138,   105,     0,     0,   135,   137,   162,   163,
     178,   173,   164,   185,   186,   187,   183,   184,   160,   161,
     165,   166,   167,   168,   170,   171,   172,   179,   180,   181,
     182,   169,   174,   175,   176,   177,    44,    46,    45,   188,
     192,     0,   193,   189,   190,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   221,   220,
       0,   229,     0,     0,   304,   237,   284,     0,   283,   282,
     300,   299,   285,   286,   292,   281,   290,   291,   307,   303,
       0,     0,     0,     0,   311,    12,     0,     0,   258,     0,
     262,    49,   265,   264,   261,     0,   260,   263,    77,     0,
       0,     0,     0,     0,     1,    10,    18,    25,    67,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   158,     0,   233,     0,     0,    60,     0,
     134,   136,     0,     0,     0,     0,     0,     0,   246,     0,
       0,   247,   248,   249,     0,   251,   276,     0,   279,    91,
      78,     0,     0,    33,    38,    30,     0,    36,    39,     0,
      41,     0,     0,     0,     0,    84,   145,     0,    95,    94,
       0,     0,    97,     0,   188,    47,   302,   296,   271,   293,
     295,   270,   272,   274,     0,   298,     0,     0,     0,     0,
       0,     0,     0,   304,   303,   306,   305,   310,     0,   148,
     149,     0,     0,   257,     0,    76,     4,   241,   145,   241,
       7,    24,     0,    21,     0,    32,    22,   102,   126,   127,
     128,   129,   131,   130,   132,   120,   121,   125,   124,   122,
     123,   191,   232,   230,   226,     0,   225,   227,   234,   114,
     115,   117,   116,   118,   119,   198,   242,   243,   245,   244,
     250,   252,    27,    28,     0,     0,    26,     0,     0,     0,
      34,    37,     0,    29,     0,   113,     0,     0,     0,     0,
      89,     0,     0,    98,    99,     0,     0,   294,   301,     0,
     297,   228,     0,     0,    53,    50,    52,   287,   289,   288,
     280,     0,   259,    20,    23,     0,   231,     0,   256,   255,
     254,   253,   278,    35,    42,    82,    79,     0,   145,     0,
      96,   269,   268,     0,   267,   273,    54,     0,     0,     0,
      40,   224,     0,    80,    87,    88,     0,   275,     0,    55,
      51,     0,    81,     0,    86,    90,   266,   133,     0,    83
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   230,    51,    52,    53,    54,    55,   237,   345,   384,
     109,   285,   286,   287,    56,    57,   290,    58,   308,   197,
     198,   199,   415,   413,   448,    59,    60,    61,    62,    63,
      64,    65,    66,   453,    67,   294,   463,   401,    68,    69,
      70,    71,   298,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
     163,   164,    89,    90,    91,   312,    92,   365,   366,   203,
      93,    94,    95,    96,   380,   275,    97,    98,   225,   226,
     443,   444,   313,   409,   205,    99,   100,   206,   207,   208,
     315,   316,   324,   210,   211,   212,   101,   102
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -380
static const yytype_int16 yypact[] =
{
     578,  -380,  -380,  1779,   331,   -19,  -380,  -380,  -380,  -380,
    -380,    69,    69,   -52,   -37,    -2,     9,  -380,   331,  -380,
    -380,  -380,  -380,    69,  1342,   331,   -10,   331,    69,  2129,
    2129,  2238,  2149,  -380,   468,  -380,    36,  1342,   -11,  1960,
    1822,  1015,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,
    -380,    71,  -380,   688,  -380,   122,  -380,  -380,  -380,  -380,
    -380,  -380,    52,  -380,  -380,  -380,  -380,  -380,  -380,  -380,
    -380,  -380,  -380,  -380,  -380,    77,  -380,   863,  -380,  -380,
    -380,  -380,  -380,  -380,  -380,  -380,   308,  -380,    72,   109,
    -380,  -380,   -12,  -380,   -15,   222,   370,  -380,  -380,   106,
    -380,  -380,  -380,  -380,  -380,    90,   534,  -380,    56,    85,
    -380,   147,   331,   331,   331,  2129,  -380,  -380,   163,   592,
      17,    84,  -380,    77,  1960,   -12,    83,    83,  -380,  -380,
    -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,
    -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,
    -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,   146,
     150,    77,  -380,  -380,  -380,  -380,   331,  -380,  -380,  -380,
    -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,
    -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,
    2267,  -380,   797,   331,     0,    15,   120,   126,  -380,  -380,
    -380,  -380,  -380,  -380,    -9,  -380,  -380,   131,  -380,   145,
    1451,  1124,   154,   331,  -380,  -380,   164,   144,  -380,  2424,
    -380,  -380,  -380,  -380,   863,    53,  -380,  -380,  -380,  1233,
    1342,  2129,  2129,    19,  -380,  -380,  -380,   278,  -380,    69,
     331,   331,   331,   331,   331,   331,   331,   331,   331,   331,
     331,   331,   331,  -380,  2267,  -380,  1641,   331,  -380,   331,
    -380,  -380,   331,   331,   331,   331,   331,  2055,  -380,  2055,
    2332,  -380,  -380,  -380,  2332,  -380,  -380,   331,  -380,  -380,
    -380,   165,   153,  -380,  -380,  -380,     2,  -380,  -380,   331,
    -380,   331,  1247,  1354,   143,   170,    64,    69,  -380,  -380,
     160,   166,  -380,  1465,  -380,  -380,  -380,  -380,  -380,  -380,
    -380,   863,  -380,  -380,   -43,  -380,   906,    21,   193,  1960,
    1546,  1546,   797,  -380,  -380,  -380,  -380,  -380,  2490,  -380,
    -380,   331,   107,  -380,  1917,  -380,  -380,    26,  -380,    31,
    -380,  -380,   180,  -380,  2623,   -19,  -380,  -380,   716,  1017,
     137,   137,  -380,  -380,  -380,   606,   606,   935,   935,   606,
     606,  -380,  -380,  -380,   863,   -48,  -380,  -380,   863,   863,
     863,   863,   863,   863,   863,  -380,  -380,  -380,  -380,  -380,
    -380,  -380,  -380,  -380,   213,   700,  -380,   194,  1190,  2513,
    -380,  -380,   135,  -380,  1081,   863,  1342,  1342,   331,  2129,
    -380,   331,  1342,  -380,  -380,  1960,  2424,  -380,  -380,    24,
    -380,  -380,  1960,   211,  -380,  -380,   863,  -380,   863,  -380,
    -380,   972,  -380,  -380,  -380,   195,  -380,  1684,  -380,  -380,
    -380,  -380,  -380,  -380,  -380,   271,  -380,   754,   202,  2545,
    -380,  -380,   863,    65,  -380,  -380,  -380,  1960,   208,   331,
    -380,  -380,  1342,  -380,   331,  -380,  1342,  -380,  1960,  -380,
    -380,  2568,  -380,   215,   170,  -380,  -380,  -380,  1342,  -380
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -380,   181,  -380,  -380,   264,    18,  -380,  -380,  -380,  -251,
     -38,    28,  -380,   -83,    13,  -380,  -380,   -26,   -23,  -380,
     -33,   -31,  -379,  -380,  -380,   -18,   -13,   -16,  -380,  -167,
    -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,  -380,
    -380,  -380,  -380,  -380,    -6,   280,    87,   288,    -3,  -380,
    -380,  -380,  -380,  -380,  -380,  -380,  -380,  -125,  -380,  -380,
       1,  -163,  -380,  -380,  -191,     6,   -25,  -380,  -117,  -228,
    -380,   -32,   -17,  -380,    54,  -380,  -380,  -380,  -380,    -4,
    -380,  -137,   -84,  -149,   -30,  -380,   -28,   -99,  -380,  -380,
       8,  -380,   291,    23,  -380,  -380,  -380,  -380
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -310
static const yytype_int16 yytable[] =
{
     105,   106,   204,   158,   125,   125,   118,   222,   195,   223,
     227,   196,   126,   127,   195,   116,   104,   221,   201,   214,
     121,   200,   119,   387,   122,   310,    -5,   305,   367,   110,
     111,    -6,   157,   446,   112,   235,   217,   224,   107,   426,
     202,   117,   240,   314,   241,   255,   427,   194,    28,   113,
     405,   406,   216,   220,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   120,   281,   108,   459,   407,
     332,   234,    35,    36,   256,    38,   378,    28,   378,   257,
     288,  -230,   295,  -222,   114,   320,   215,    44,  -222,   283,
     125,   361,   301,   424,  -230,   115,   392,   258,   296,    35,
      36,    35,    36,   258,    38,   318,   337,   339,   -48,   292,
     293,   240,  -222,   241,   300,   123,    44,  -222,   406,   161,
     281,   303,   213,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   411,   239,   407,   216,    35,    36,
    -145,   282,   333,   283,  -145,  -145,  -145,   334,  -145,   310,
     236,    35,    36,   284,    38,   310,   457,   399,   400,   458,
    -145,  -145,  -145,   122,   238,   240,    44,   241,   253,  -145,
     254,  -145,   244,   245,   246,   289,   309,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   276,   311,
     317,   291,   277,   297,   201,   201,   302,   200,   200,   367,
     405,   406,   279,   278,   -13,   307,   125,   125,  -103,   346,
     328,   235,   336,   -49,   338,   338,   311,    35,    36,   319,
     282,   417,   419,   323,   323,   321,  -308,   340,   231,   232,
     233,   330,   284,   325,   326,   327,   331,   348,   349,   350,
     351,   352,   353,   354,   355,   356,   357,   358,   359,   360,
     391,   329,   390,   364,   368,   398,   369,   347,   450,   370,
     371,   372,   373,   374,   257,   412,   259,   385,   377,   363,
     377,   388,   403,   379,   389,   379,   386,   423,   404,   428,
     386,   260,   261,   430,   447,    41,   394,   452,   395,   262,
     263,   264,   265,   266,   195,   195,   455,   196,   196,   460,
     309,   222,   468,   223,   227,   229,   309,   425,   195,   433,
     451,   221,   160,   311,   393,   402,   416,   368,   418,   311,
     162,   466,   445,   381,   410,   209,   202,   202,   421,   307,
     422,   224,   414,   363,   363,   307,     0,     0,     5,     6,
       0,     0,     9,    10,    11,    12,     0,   220,     0,     0,
     386,    17,   341,    18,     0,    19,    20,     0,     0,    23,
      35,    36,     0,   342,   288,    27,     0,     0,     0,  -241,
    -241,  -241,     0,  -241,   125,   343,     0,   344,   435,   436,
       0,     0,   438,     0,   440,  -241,  -241,  -241,     0,    28,
      29,    30,     0,     0,     0,   437,  -241,     0,   439,     0,
       0,     0,   442,   311,     0,    31,    32,    33,     0,   416,
       0,    34,     0,    35,    36,     0,    38,    39,   441,    40,
       0,     0,   464,     0,   364,   414,    42,    43,    44,    45,
       0,   267,   268,   269,   462,   270,     0,     0,   465,     0,
     363,     0,     0,     0,   416,     0,   461,   271,   272,   273,
     469,     0,     0,     0,     0,   442,     0,     0,   274,     0,
     414,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       0,   441,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,    16,    17,     0,
      18,     0,    19,    20,    21,    22,    23,    24,     0,    25,
      26,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,    28,    29,    30,   190,
       0,     0,   191,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    31,    32,    33,     0,     0,     0,    34,     0,
      35,    36,    37,    38,    39,   240,    40,   241,   192,     0,
       0,     0,     0,    42,    43,    44,    45,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,    -9,     1,
      50,   193,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,    16,    17,     0,
      18,     0,    19,    20,    21,    22,    23,    24,     0,    25,
      26,     0,    27,   240,     0,   241,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,    28,    29,    30,   242,
     243,   244,   245,   246,  -310,  -310,   280,     0,  -310,  -310,
       0,     0,    31,    32,    33,     0,     0,     0,    34,     0,
      35,    36,    37,    38,    39,     0,    40,     0,    41,     0,
       0,     0,     0,    42,    43,    44,    45,     0,     0,     0,
       0,     0,     0,     0,    46,    47,    48,    49,    -8,     1,
      50,     0,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,   299,    14,    15,    16,    17,     0,
      18,     0,    19,    20,    21,    22,    23,    24,     0,    25,
      26,   240,    27,   241,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,     0,     0,    28,    29,    30,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
       0,     0,    31,    32,    33,     0,   429,     0,    34,     0,
      35,    36,    37,    38,    39,   240,    40,   241,    41,     0,
       0,     0,     0,    42,    43,    44,    45,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,     1,     0,
      50,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,    16,    17,     0,    18,
       0,    19,    20,    21,    22,    23,    24,     0,    25,    26,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,    28,    29,    30,   190,     0,
       0,     0,     0,     0,     0,     0,   454,     0,     0,     0,
       0,    31,    32,    33,     0,     0,     0,    34,     0,    35,
      36,    37,    38,    39,   240,    40,   241,    41,   306,     0,
       0,     0,    42,    43,    44,    45,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,     1,     0,    50,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,    16,    17,     0,    18,     0,
      19,    20,    21,    22,    23,    24,     0,    25,    26,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,    28,    29,    30,   190,   242,   243,
     244,   245,   246,   247,   248,  -310,  -310,   251,   252,     0,
      31,    32,    33,     0,     0,     0,    34,     0,    35,    36,
      37,    38,    39,   240,    40,   241,    41,   408,     0,     0,
       0,    42,    43,    44,    45,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,     1,     0,    50,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,    16,    17,     0,    18,   240,    19,
      20,    21,    22,    23,    24,     0,    25,    26,     0,    27,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,     0,     0,     0,     0,   449,     0,     0,     0,     0,
       0,     0,     0,    28,    29,    30,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,    33,     0,     0,     0,    34,     0,    35,    36,    37,
      38,    39,   240,    40,   241,    41,   228,     0,     0,     0,
      42,    43,    44,    45,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,     1,     0,    50,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,    16,    17,     0,    18,     0,    19,    20,
      21,    22,    23,    24,     0,    25,    26,     0,    27,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   434,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    28,    29,    30,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,     0,     0,     0,    34,  -309,    35,    36,    37,    38,
      39,   240,    40,   241,   322,     0,     0,     0,     0,    42,
      43,    44,    45,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,     1,     0,    50,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,    16,    17,     0,    18,     0,    19,    20,    21,
      22,    23,    24,     0,    25,    26,     0,    27,   240,     0,
     241,     0,     0,     0,     0,     0,     0,     0,     0,   431,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,    28,    29,    30,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    32,    33,
       0,     0,     0,    34,     0,    35,    36,    37,    38,    39,
       0,    40,     0,    41,   335,     0,     0,     0,    42,    43,
      44,    45,     0,     0,   396,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     0,    50,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,    16,    17,     0,    18,     0,    19,    20,    21,    22,
      23,    24,     0,    25,    26,   240,    27,   241,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,     0,     0,
      28,    29,    30,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,    32,    33,     0,
       0,     0,    34,     0,    35,    36,    37,    38,    39,     0,
      40,     0,    41,     0,     0,     0,     0,    42,    43,    44,
      45,   397,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     0,    50,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
      16,    17,     0,    18,     0,    19,    20,    21,    22,    23,
      24,     0,    25,    26,     0,    27,   240,     0,   241,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,    28,
      29,    30,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,    33,     0,     0,
       0,    34,     0,    35,    36,    37,    38,    39,     0,    40,
       0,   322,     0,     0,     0,     0,    42,    43,    44,    45,
       2,     0,   330,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,    50,     0,     0,    17,     0,    18,     0,
      19,    20,     0,     0,    23,     0,     0,     0,     0,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,    28,    29,    30,   190,     0,     0,
     191,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      31,    32,    33,     0,     0,     0,    34,     0,    35,    36,
       0,    38,    39,     0,    40,     0,   219,     0,     0,     0,
       0,    42,    43,    44,    45,     2,     0,     0,     5,     6,
       7,     8,     9,    10,    11,    12,     0,     0,     0,   193,
       0,    17,     0,    18,     0,    19,    20,     0,     0,    23,
       0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     0,
       0,     5,     6,     7,     8,     9,    10,    11,    12,    28,
      29,    30,     0,     0,    17,   191,    18,     0,    19,    20,
       0,     0,    23,     0,     0,    31,    32,    33,    27,     0,
       0,    34,     0,    35,    36,     0,    38,    39,   362,    40,
       0,     0,     0,     0,     0,     0,    42,    43,    44,    45,
       0,     0,    28,    29,    30,     0,     0,     0,   191,     0,
       0,     0,     0,     0,   193,     0,     0,     0,    31,    32,
      33,     0,     0,     0,    34,     0,    35,    36,     0,    38,
      39,     0,    40,     0,     0,     0,     0,     0,     0,    42,
      43,    44,    45,     2,     0,     0,     5,     6,     7,     8,
       9,    10,    11,    12,     0,     0,     0,   193,     0,    17,
       0,    18,     0,    19,    20,     0,     0,    23,     0,     0,
       0,     0,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     2,     0,     0,     5,
       6,     7,     8,     9,    10,    11,    12,    28,    29,    30,
       0,     0,    17,     0,    18,     0,    19,    20,     0,     0,
      23,     0,     0,    31,    32,    33,    27,     0,     0,    34,
       0,    35,    36,     0,    38,    39,     0,    40,     0,     0,
       0,     0,     0,     0,    42,    43,    44,    45,     0,     0,
      28,    29,    30,   190,     0,     0,     0,     0,     0,     0,
       0,   103,     0,     0,     0,     0,    31,    32,    33,     0,
       0,     0,    34,     0,    35,    36,     0,    38,    39,     0,
      40,   218,   219,     0,     0,     0,     0,    42,    43,    44,
      45,     2,     0,     0,     5,     6,     7,     8,     9,    10,
      11,    12,     0,     0,     0,     0,     0,    17,     0,    18,
       0,    19,    20,     0,     0,    23,     0,     0,     0,     0,
       0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     2,     0,     0,     5,     6,     7,
       8,     9,    10,    11,    12,    28,    29,    30,   190,     0,
      17,     0,    18,     0,    19,    20,     0,     0,    23,     0,
       0,    31,    32,    33,    27,     0,     0,    34,     0,    35,
      36,     0,    38,    39,     0,    40,     0,   219,     0,     0,
       0,     0,    42,    43,    44,    45,     0,     0,    28,    29,
      30,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,    33,     0,     0,     0,
      34,     0,    35,    36,     0,    38,    39,     0,    40,     0,
       0,     0,     0,     0,     0,    42,    43,    44,    45,   128,
     129,   130,   131,     0,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   165,   375,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      32,     0,     0,     0,     0,     0,     0,     6,     0,     0,
       9,    10,    11,     0,     0,     0,     0,     0,     0,     0,
       0,    43,   376,   128,   129,   130,   131,    23,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,     0,     0,     0,     0,     0,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,     0,    28,     0,     0,
       0,    35,    36,     0,    38,   124,     0,    40,     0,     0,
       0,     0,     0,     0,     0,    43,    44,    45,     0,     0,
       0,    35,    36,     0,    38,     0,     0,     0,     0,     0,
       0,     0,   128,   129,   130,   131,   159,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   128,   129,   130,   131,     0,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   156,     0,     0,     0,     5,
       6,     0,     0,     9,    10,    11,    12,     0,     0,     0,
       0,     0,    17,     0,    18,     0,    19,    20,     0,     0,
      23,     0,     0,     0,   304,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
      28,    29,    30,   382,   383,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,    32,    33,     0,
       0,     0,    34,     0,    35,    36,     0,    38,    39,     0,
      40,     0,     0,     0,     0,     0,     0,    42,    43,    44,
      45,     5,     6,     0,     0,     9,    10,    11,    12,     0,
       0,     0,     0,     0,    17,     0,    18,     0,    19,    20,
       0,     0,    23,     0,     0,     0,     0,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,    28,    29,    30,   190,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,     0,     0,     0,    34,     0,    35,    36,     0,    38,
      39,   240,    40,   241,     0,     0,     0,     0,     0,    42,
      43,    44,    45,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   240,     0,   241,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   240,     0,   241,     0,
       0,     0,     0,     0,     0,     0,     0,   420,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   240,
       0,   241,     0,     0,     0,     0,     0,     0,     0,     0,
     432,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   456,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   467,   165,   375,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,     0,     0,     0,   382,   383
};

static const yytype_int16 yycheck[] =
{
       3,     4,    34,    31,    29,    30,    24,    40,    34,    40,
      40,    34,    29,    30,    40,    18,     3,    40,    34,    37,
      26,    34,    25,   274,    27,   192,     0,   190,   256,    11,
      12,     0,    31,   412,    86,    53,    39,    40,    57,    87,
      34,    23,    21,   192,    23,    57,    94,    34,    58,    86,
      93,    94,    39,    40,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    75,    64,    86,   447,   112,
     219,     0,    82,    83,    86,    85,   267,    58,   269,    94,
     108,    81,   114,    57,    86,    94,    97,    97,    57,    87,
     115,   254,   120,   344,    94,    86,    94,   112,   115,    82,
      83,    82,    83,   112,    85,    90,   231,   232,    93,   112,
     113,    21,    86,    23,    97,    28,    97,    86,    94,    32,
      64,   124,    86,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,   113,    58,   112,   124,    82,    83,
      57,    85,    89,    87,    61,    62,    63,    94,    65,   316,
      28,    82,    83,    97,    85,   322,    91,    93,    94,    94,
      77,    78,    79,   166,   112,    21,    97,    23,    96,    86,
      61,    88,    35,    36,    37,    90,   192,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    82,   192,
     193,    44,    86,    30,   210,   211,   112,   210,   211,   427,
      93,    94,   112,    97,    58,   192,   231,   232,    58,   237,
     213,   229,   230,    93,   231,   232,   219,    82,    83,    93,
      85,   320,   321,   210,   211,    94,    81,   233,    47,    48,
      49,    87,    97,   210,   211,    81,    92,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
      97,    87,    87,   256,   257,   112,   259,   239,   425,   262,
     263,   264,   265,   266,    94,    72,    44,   270,   267,   256,
     269,   274,   112,   267,   277,   269,   270,    97,   112,    66,
     274,    59,    60,    89,    73,    90,   289,    16,   291,    67,
      68,    69,    70,    71,   320,   321,    94,   320,   321,    91,
     316,   334,    87,   334,   334,    41,   322,   345,   334,   392,
     427,   334,    32,   316,   286,   297,   319,   320,   321,   322,
      32,   458,   406,   269,   316,    34,   320,   321,   331,   316,
     334,   334,   319,   320,   321,   322,    -1,    -1,     7,     8,
      -1,    -1,    11,    12,    13,    14,    -1,   334,    -1,    -1,
     344,    20,    74,    22,    -1,    24,    25,    -1,    -1,    28,
      82,    83,    -1,    85,   392,    34,    -1,    -1,    -1,    61,
      62,    63,    -1,    65,   399,    97,    -1,    99,   396,   397,
      -1,    -1,   399,    -1,   402,    77,    78,    79,    -1,    58,
      59,    60,    -1,    -1,    -1,   398,    88,    -1,   401,    -1,
      -1,    -1,   405,   406,    -1,    74,    75,    76,    -1,   412,
      -1,    80,    -1,    82,    83,    -1,    85,    86,   405,    88,
      -1,    -1,   454,    -1,   427,   412,    95,    96,    97,    98,
      -1,    61,    62,    63,   452,    65,    -1,    -1,   456,    -1,
     427,    -1,    -1,    -1,   447,    -1,   449,    77,    78,    79,
     468,    -1,    -1,    -1,    -1,   458,    -1,    -1,    88,    -1,
     447,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      -1,   458,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    -1,    17,    18,    19,    20,    -1,
      22,    -1,    24,    25,    26,    27,    28,    29,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      -1,    -1,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    74,    75,    76,    -1,    -1,    -1,    80,    -1,
      82,    83,    84,    85,    86,    21,    88,    23,    90,    -1,
      -1,    -1,    -1,    95,    96,    97,    98,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,     0,     1,
     112,   113,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    -1,    17,    18,    19,    20,    -1,
      22,    -1,    24,    25,    26,    27,    28,    29,    -1,    31,
      32,    -1,    34,    21,    -1,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    58,    59,    60,    33,
      34,    35,    36,    37,    38,    39,   112,    -1,    42,    43,
      -1,    -1,    74,    75,    76,    -1,    -1,    -1,    80,    -1,
      82,    83,    84,    85,    86,    -1,    88,    -1,    90,    -1,
      -1,    -1,    -1,    95,    96,    97,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   106,   107,   108,   109,     0,     1,
     112,    -1,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,   112,    17,    18,    19,    20,    -1,
      22,    -1,    24,    25,    26,    27,    28,    29,    -1,    31,
      32,    21,    34,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    -1,    58,    59,    60,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    74,    75,    76,    -1,    66,    -1,    80,    -1,
      82,    83,    84,    85,    86,    21,    88,    23,    90,    -1,
      -1,    -1,    -1,    95,    96,    97,    98,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,     1,    -1,
     112,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    -1,    17,    18,    19,    20,    -1,    22,
      -1,    24,    25,    26,    27,    28,    29,    -1,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   112,    -1,    -1,    -1,
      -1,    74,    75,    76,    -1,    -1,    -1,    80,    -1,    82,
      83,    84,    85,    86,    21,    88,    23,    90,    91,    -1,
      -1,    -1,    95,    96,    97,    98,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,     1,    -1,   112,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    -1,    17,    18,    19,    20,    -1,    22,    -1,
      24,    25,    26,    27,    28,    29,    -1,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    -1,
      74,    75,    76,    -1,    -1,    -1,    80,    -1,    82,    83,
      84,    85,    86,    21,    88,    23,    90,    91,    -1,    -1,
      -1,    95,    96,    97,    98,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,     1,    -1,   112,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    -1,    17,    18,    19,    20,    -1,    22,    21,    24,
      25,    26,    27,    28,    29,    -1,    31,    32,    -1,    34,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    58,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,
      75,    76,    -1,    -1,    -1,    80,    -1,    82,    83,    84,
      85,    86,    21,    88,    23,    90,    91,    -1,    -1,    -1,
      95,    96,    97,    98,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,     1,    -1,   112,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      -1,    17,    18,    19,    20,    -1,    22,    -1,    24,    25,
      26,    27,    28,    29,    -1,    31,    32,    -1,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75,
      76,    -1,    -1,    -1,    80,    81,    82,    83,    84,    85,
      86,    21,    88,    23,    90,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,     1,    -1,   112,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    -1,
      17,    18,    19,    20,    -1,    22,    -1,    24,    25,    26,
      27,    28,    29,    -1,    31,    32,    -1,    34,    21,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75,    76,
      -1,    -1,    -1,    80,    -1,    82,    83,    84,    85,    86,
      -1,    88,    -1,    90,    91,    -1,    -1,    -1,    95,    96,
      97,    98,    -1,    -1,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,   112,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    -1,    17,
      18,    19,    20,    -1,    22,    -1,    24,    25,    26,    27,
      28,    29,    -1,    31,    32,    21,    34,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      58,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    74,    75,    76,    -1,
      -1,    -1,    80,    -1,    82,    83,    84,    85,    86,    -1,
      88,    -1,    90,    -1,    -1,    -1,    -1,    95,    96,    97,
      98,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    -1,   112,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    -1,    17,    18,
      19,    20,    -1,    22,    -1,    24,    25,    26,    27,    28,
      29,    -1,    31,    32,    -1,    34,    21,    -1,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    58,
      59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    74,    75,    76,    -1,    -1,
      -1,    80,    -1,    82,    83,    84,    85,    86,    -1,    88,
      -1,    90,    -1,    -1,    -1,    -1,    95,    96,    97,    98,
       4,    -1,    87,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,   112,    -1,    -1,    20,    -1,    22,    -1,
      24,    25,    -1,    -1,    28,    -1,    -1,    -1,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    -1,    -1,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      74,    75,    76,    -1,    -1,    -1,    80,    -1,    82,    83,
      -1,    85,    86,    -1,    88,    -1,    90,    -1,    -1,    -1,
      -1,    95,    96,    97,    98,     4,    -1,    -1,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,   113,
      -1,    20,    -1,    22,    -1,    24,    25,    -1,    -1,    28,
      -1,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,    -1,
      -1,     7,     8,     9,    10,    11,    12,    13,    14,    58,
      59,    60,    -1,    -1,    20,    64,    22,    -1,    24,    25,
      -1,    -1,    28,    -1,    -1,    74,    75,    76,    34,    -1,
      -1,    80,    -1,    82,    83,    -1,    85,    86,    87,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    98,
      -1,    -1,    58,    59,    60,    -1,    -1,    -1,    64,    -1,
      -1,    -1,    -1,    -1,   113,    -1,    -1,    -1,    74,    75,
      76,    -1,    -1,    -1,    80,    -1,    82,    83,    -1,    85,
      86,    -1,    88,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,     4,    -1,    -1,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,   113,    -1,    20,
      -1,    22,    -1,    24,    25,    -1,    -1,    28,    -1,    -1,
      -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     4,    -1,    -1,     7,
       8,     9,    10,    11,    12,    13,    14,    58,    59,    60,
      -1,    -1,    20,    -1,    22,    -1,    24,    25,    -1,    -1,
      28,    -1,    -1,    74,    75,    76,    34,    -1,    -1,    80,
      -1,    82,    83,    -1,    85,    86,    -1,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    98,    -1,    -1,
      58,    59,    60,    61,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,    -1,    -1,    -1,    -1,    74,    75,    76,    -1,
      -1,    -1,    80,    -1,    82,    83,    -1,    85,    86,    -1,
      88,    89,    90,    -1,    -1,    -1,    -1,    95,    96,    97,
      98,     4,    -1,    -1,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    20,    -1,    22,
      -1,    24,    25,    -1,    -1,    28,    -1,    -1,    -1,    -1,
      -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     4,    -1,    -1,     7,     8,     9,
      10,    11,    12,    13,    14,    58,    59,    60,    61,    -1,
      20,    -1,    22,    -1,    24,    25,    -1,    -1,    28,    -1,
      -1,    74,    75,    76,    34,    -1,    -1,    80,    -1,    82,
      83,    -1,    85,    86,    -1,    88,    -1,    90,    -1,    -1,
      -1,    -1,    95,    96,    97,    98,    -1,    -1,    58,    59,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    74,    75,    76,    -1,    -1,    -1,
      80,    -1,    82,    83,    -1,    85,    86,    -1,    88,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    96,    97,    98,     4,
       5,     6,     7,    -1,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      75,    -1,    -1,    -1,    -1,    -1,    -1,     8,    -1,    -1,
      11,    12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    96,    97,     4,     5,     6,     7,    28,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    74,    75,    76,    -1,    58,    -1,    -1,
      -1,    82,    83,    -1,    85,    86,    -1,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    96,    97,    98,    -1,    -1,
      -1,    82,    83,    -1,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     4,     5,     6,     7,    97,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,     4,     5,     6,     7,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,     7,
       8,    -1,    -1,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    20,    -1,    22,    -1,    24,    25,    -1,    -1,
      28,    -1,    -1,    -1,    97,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    74,    75,    76,    -1,
      -1,    -1,    80,    -1,    82,    83,    -1,    85,    86,    -1,
      88,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,
      98,     7,     8,    -1,    -1,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    20,    -1,    22,    -1,    24,    25,
      -1,    -1,    28,    -1,    -1,    -1,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75,
      76,    -1,    -1,    -1,    80,    -1,    82,    83,    -1,    85,
      86,    21,    88,    23,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    21,    -1,    23,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    21,    -1,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    21,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    -1,    -1,    61,    62
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    17,    18,    19,    20,    22,    24,
      25,    26,    27,    28,    29,    31,    32,    34,    58,    59,
      60,    74,    75,    76,    80,    82,    83,    84,    85,    86,
      88,    90,    95,    96,    97,    98,   106,   107,   108,   109,
     112,   116,   117,   118,   119,   120,   128,   129,   131,   139,
     140,   141,   142,   143,   144,   145,   146,   148,   152,   153,
     154,   155,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   176,
     177,   178,   180,   184,   185,   186,   187,   190,   191,   199,
     200,   210,   211,   112,   128,   162,   162,    57,    86,   124,
     119,   119,    86,    86,    86,    86,   162,   119,   139,   162,
      75,   158,   162,   160,    86,   180,   186,   186,     4,     5,
       6,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    97,   174,   200,    97,
     159,   160,   161,   174,   175,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      61,    64,    90,   113,   128,   131,   132,   133,   134,   135,
     140,   141,   179,   183,   185,   198,   201,   202,   203,   206,
     207,   208,   209,    86,   139,    97,   128,   162,    89,    90,
     128,   132,   134,   135,   162,   192,   193,   198,    91,   118,
     115,   115,   115,   115,     0,   139,    28,   121,   112,    58,
      21,    23,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    96,    61,    57,    86,    94,   112,    44,
      59,    60,    67,    68,    69,    70,    71,    61,    62,    63,
      65,    77,    78,    79,    88,   189,    82,    86,    97,   112,
     112,    64,    85,    87,    97,   125,   126,   127,   200,    90,
     130,    44,   162,   162,   149,   185,   186,    30,   156,   112,
      97,   200,   112,   162,    97,   175,    91,   128,   132,   141,
     143,   162,   179,   196,   197,   204,   205,   162,    90,    93,
      94,    94,    90,   128,   206,   207,   207,    81,   162,    87,
      87,    92,   197,    89,    94,    91,   139,   171,   186,   171,
     158,    74,    85,    97,    99,   122,   200,   119,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   175,    87,   128,   162,   181,   182,   183,   162,   162,
     162,   162,   162,   162,   162,    34,    97,   174,   178,   179,
     188,   188,    61,    62,   123,   162,   179,   123,   162,   162,
      87,    97,    94,   125,   162,   162,    87,    87,   112,    93,
      94,   151,   119,   112,   112,    93,    94,   112,    91,   197,
     204,   113,    72,   137,   128,   136,   162,   201,   162,   201,
      87,   162,   193,    97,   123,   124,    87,    94,    66,    66,
      89,    89,    87,   127,    91,   139,   139,   162,   186,   162,
     139,   128,   162,   194,   195,   196,   136,    73,   138,    93,
     143,   182,    16,   147,   112,    94,    87,    91,    94,   136,
      91,   162,   139,   150,   185,   139,   195,    87,    87,   139
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, delta::ParsingCtx& ctx)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, ctx)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    delta::ParsingCtx& ctx;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, delta::ParsingCtx& ctx)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, ctx)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    delta::ParsingCtx& ctx;
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, delta::ParsingCtx& ctx)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, ctx)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    delta::ParsingCtx& ctx;
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, delta::ParsingCtx& ctx)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, ctx)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    delta::ParsingCtx& ctx;
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
      case 117: /* "CodeDefs" */

/* Line 1000 of yacc.c  */
#line 198 "DeltaParserSpec.y"
	{ delete (yyvaluep->stmtsValue); };

/* Line 1000 of yacc.c  */
#line 2239 "DeltaParserGen.cpp"
	break;
      case 118: /* "Stmts" */

/* Line 1000 of yacc.c  */
#line 198 "DeltaParserSpec.y"
	{ delete (yyvaluep->stmtsValue); };

/* Line 1000 of yacc.c  */
#line 2248 "DeltaParserGen.cpp"
	break;
      case 119: /* "Ident" */

/* Line 1000 of yacc.c  */
#line 176 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2257 "DeltaParserGen.cpp"
	break;
      case 121: /* "FunctionLinkage" */

/* Line 1000 of yacc.c  */
#line 199 "DeltaParserSpec.y"
	{ delete (yyvaluep->unaryKwdValue); };

/* Line 1000 of yacc.c  */
#line 2266 "DeltaParserGen.cpp"
	break;
      case 122: /* "FunctionName" */

/* Line 1000 of yacc.c  */
#line 174 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2275 "DeltaParserGen.cpp"
	break;
      case 124: /* "FormalArgs" */

/* Line 1000 of yacc.c  */
#line 180 "DeltaParserSpec.y"
	{ delete (yyvaluep->argListValue); };

/* Line 1000 of yacc.c  */
#line 2284 "DeltaParserGen.cpp"
	break;
      case 125: /* "FormalArgsSuffix" */

/* Line 1000 of yacc.c  */
#line 201 "DeltaParserSpec.y"
	{ delete (yyvaluep->argValue); };

/* Line 1000 of yacc.c  */
#line 2293 "DeltaParserGen.cpp"
	break;
      case 126: /* "FormalArgList" */

/* Line 1000 of yacc.c  */
#line 180 "DeltaParserSpec.y"
	{ delete (yyvaluep->argListValue); };

/* Line 1000 of yacc.c  */
#line 2302 "DeltaParserGen.cpp"
	break;
      case 127: /* "FormalArg" */

/* Line 1000 of yacc.c  */
#line 174 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2311 "DeltaParserGen.cpp"
	break;
      case 128: /* "Function" */

/* Line 1000 of yacc.c  */
#line 209 "DeltaParserSpec.y"
	{ delete (yyvaluep->funcValue); };

/* Line 1000 of yacc.c  */
#line 2320 "DeltaParserGen.cpp"
	break;
      case 129: /* "LambdaFunction" */

/* Line 1000 of yacc.c  */
#line 209 "DeltaParserSpec.y"
	{ delete (yyvaluep->funcValue); };

/* Line 1000 of yacc.c  */
#line 2329 "DeltaParserGen.cpp"
	break;
      case 130: /* "LambdaCode" */

/* Line 1000 of yacc.c  */
#line 172 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2338 "DeltaParserGen.cpp"
	break;
      case 131: /* "AttributeId" */

/* Line 1000 of yacc.c  */
#line 172 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2347 "DeltaParserGen.cpp"
	break;
      case 132: /* "DottedIdent" */

/* Line 1000 of yacc.c  */
#line 174 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2356 "DeltaParserGen.cpp"
	break;
      case 133: /* "IdentIndex" */

/* Line 1000 of yacc.c  */
#line 174 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2365 "DeltaParserGen.cpp"
	break;
      case 134: /* "IdentIndexElement" */

/* Line 1000 of yacc.c  */
#line 231 "DeltaParserSpec.y"
	{ delete (yyvaluep->tableElemValue); };

/* Line 1000 of yacc.c  */
#line 2374 "DeltaParserGen.cpp"
	break;
      case 135: /* "NewAttribute" */

/* Line 1000 of yacc.c  */
#line 230 "DeltaParserSpec.y"
	{ delete (yyvaluep->newAttributeValue); };

/* Line 1000 of yacc.c  */
#line 2383 "DeltaParserGen.cpp"
	break;
      case 136: /* "AttrFunction" */

/* Line 1000 of yacc.c  */
#line 177 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2392 "DeltaParserGen.cpp"
	break;
      case 137: /* "AttributeSet" */

/* Line 1000 of yacc.c  */
#line 199 "DeltaParserSpec.y"
	{ delete (yyvaluep->unaryKwdValue); };

/* Line 1000 of yacc.c  */
#line 2401 "DeltaParserGen.cpp"
	break;
      case 138: /* "AttributeGet" */

/* Line 1000 of yacc.c  */
#line 199 "DeltaParserSpec.y"
	{ delete (yyvaluep->unaryKwdValue); };

/* Line 1000 of yacc.c  */
#line 2410 "DeltaParserGen.cpp"
	break;
      case 139: /* "Stmt" */

/* Line 1000 of yacc.c  */
#line 171 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2419 "DeltaParserGen.cpp"
	break;
      case 140: /* "ExpressionListStmt" */

/* Line 1000 of yacc.c  */
#line 171 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2428 "DeltaParserGen.cpp"
	break;
      case 141: /* "NonExprListCompoundOrFunctionStmt" */

/* Line 1000 of yacc.c  */
#line 171 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2437 "DeltaParserGen.cpp"
	break;
      case 142: /* "LoopCtrlStmt" */

/* Line 1000 of yacc.c  */
#line 202 "DeltaParserSpec.y"
	{ delete (yyvaluep->leafKwdValue); };

/* Line 1000 of yacc.c  */
#line 2446 "DeltaParserGen.cpp"
	break;
      case 143: /* "Compound" */

/* Line 1000 of yacc.c  */
#line 208 "DeltaParserSpec.y"
	{ delete (yyvaluep->compValue); };

/* Line 1000 of yacc.c  */
#line 2455 "DeltaParserGen.cpp"
	break;
      case 144: /* "AssertStmt" */

/* Line 1000 of yacc.c  */
#line 199 "DeltaParserSpec.y"
	{ delete (yyvaluep->unaryKwdValue); };

/* Line 1000 of yacc.c  */
#line 2464 "DeltaParserGen.cpp"
	break;
      case 145: /* "WhileStmt" */

/* Line 1000 of yacc.c  */
#line 203 "DeltaParserSpec.y"
	{ delete (yyvaluep->whileValue); };

/* Line 1000 of yacc.c  */
#line 2473 "DeltaParserGen.cpp"
	break;
      case 146: /* "IfStmt" */

/* Line 1000 of yacc.c  */
#line 206 "DeltaParserSpec.y"
	{ delete (yyvaluep->ifValue); };

/* Line 1000 of yacc.c  */
#line 2482 "DeltaParserGen.cpp"
	break;
      case 147: /* "ElseStmt" */

/* Line 1000 of yacc.c  */
#line 207 "DeltaParserSpec.y"
	{ delete (yyvaluep->elseValue); };

/* Line 1000 of yacc.c  */
#line 2491 "DeltaParserGen.cpp"
	break;
      case 148: /* "ForStmt" */

/* Line 1000 of yacc.c  */
#line 204 "DeltaParserSpec.y"
	{ delete (yyvaluep->forValue); };

/* Line 1000 of yacc.c  */
#line 2500 "DeltaParserGen.cpp"
	break;
      case 149: /* "ForInitList" */

/* Line 1000 of yacc.c  */
#line 178 "DeltaParserSpec.y"
	{ delete (yyvaluep->exprListValue); };

/* Line 1000 of yacc.c  */
#line 2509 "DeltaParserGen.cpp"
	break;
      case 150: /* "ForRepeatList" */

/* Line 1000 of yacc.c  */
#line 178 "DeltaParserSpec.y"
	{ delete (yyvaluep->exprListValue); };

/* Line 1000 of yacc.c  */
#line 2518 "DeltaParserGen.cpp"
	break;
      case 151: /* "ForeachOpt" */

/* Line 1000 of yacc.c  */
#line 177 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2527 "DeltaParserGen.cpp"
	break;
      case 152: /* "ForeachStmt" */

/* Line 1000 of yacc.c  */
#line 205 "DeltaParserSpec.y"
	{ delete (yyvaluep->foreachValue); };

/* Line 1000 of yacc.c  */
#line 2536 "DeltaParserGen.cpp"
	break;
      case 153: /* "ReturnStmt" */

/* Line 1000 of yacc.c  */
#line 199 "DeltaParserSpec.y"
	{ delete (yyvaluep->unaryKwdValue); };

/* Line 1000 of yacc.c  */
#line 2545 "DeltaParserGen.cpp"
	break;
      case 154: /* "ThrowStmt" */

/* Line 1000 of yacc.c  */
#line 199 "DeltaParserSpec.y"
	{ delete (yyvaluep->unaryKwdValue); };

/* Line 1000 of yacc.c  */
#line 2554 "DeltaParserGen.cpp"
	break;
      case 155: /* "TryStmt" */

/* Line 1000 of yacc.c  */
#line 211 "DeltaParserSpec.y"
	{ delete (yyvaluep->tryValue); };

/* Line 1000 of yacc.c  */
#line 2563 "DeltaParserGen.cpp"
	break;
      case 156: /* "TrapStmt" */

/* Line 1000 of yacc.c  */
#line 212 "DeltaParserSpec.y"
	{ delete (yyvaluep->trapValue); };

/* Line 1000 of yacc.c  */
#line 2572 "DeltaParserGen.cpp"
	break;
      case 157: /* "UsingDirective" */

/* Line 1000 of yacc.c  */
#line 200 "DeltaParserSpec.y"
	{ delete (yyvaluep->usingValue); };

/* Line 1000 of yacc.c  */
#line 2581 "DeltaParserGen.cpp"
	break;
      case 158: /* "QualifiedId" */

/* Line 1000 of yacc.c  */
#line 177 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2590 "DeltaParserGen.cpp"
	break;
      case 159: /* "PureQualifiedId" */

/* Line 1000 of yacc.c  */
#line 177 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2599 "DeltaParserGen.cpp"
	break;
      case 160: /* "NonGlobalQualifiedId" */

/* Line 1000 of yacc.c  */
#line 177 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2608 "DeltaParserGen.cpp"
	break;
      case 161: /* "GlobalQualifiedId" */

/* Line 1000 of yacc.c  */
#line 177 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2617 "DeltaParserGen.cpp"
	break;
      case 162: /* "Expression" */

/* Line 1000 of yacc.c  */
#line 171 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2626 "DeltaParserGen.cpp"
	break;
      case 163: /* "ConstAssignExpression" */

/* Line 1000 of yacc.c  */
#line 220 "DeltaParserSpec.y"
	{ delete (yyvaluep->binaryOpValue); };

/* Line 1000 of yacc.c  */
#line 2635 "DeltaParserGen.cpp"
	break;
      case 164: /* "AssignExpression" */

/* Line 1000 of yacc.c  */
#line 220 "DeltaParserSpec.y"
	{ delete (yyvaluep->binaryOpValue); };

/* Line 1000 of yacc.c  */
#line 2644 "DeltaParserGen.cpp"
	break;
      case 165: /* "RelationalExpression" */

/* Line 1000 of yacc.c  */
#line 219 "DeltaParserSpec.y"
	{ delete (yyvaluep->binaryOpValue); };

/* Line 1000 of yacc.c  */
#line 2653 "DeltaParserGen.cpp"
	break;
      case 166: /* "BooleanExpression" */

/* Line 1000 of yacc.c  */
#line 219 "DeltaParserSpec.y"
	{ delete (yyvaluep->binaryOpValue); };

/* Line 1000 of yacc.c  */
#line 2662 "DeltaParserGen.cpp"
	break;
      case 167: /* "ArithmeticExpression" */

/* Line 1000 of yacc.c  */
#line 219 "DeltaParserSpec.y"
	{ delete (yyvaluep->binaryOpValue); };

/* Line 1000 of yacc.c  */
#line 2671 "DeltaParserGen.cpp"
	break;
      case 168: /* "TernaryExpression" */

/* Line 1000 of yacc.c  */
#line 210 "DeltaParserSpec.y"
	{ delete (yyvaluep->ternaryOpValue); };

/* Line 1000 of yacc.c  */
#line 2680 "DeltaParserGen.cpp"
	break;
      case 169: /* "Term" */

/* Line 1000 of yacc.c  */
#line 171 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2689 "DeltaParserGen.cpp"
	break;
      case 170: /* "Primary" */

/* Line 1000 of yacc.c  */
#line 171 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2698 "DeltaParserGen.cpp"
	break;
      case 171: /* "FunctionAndTableObject" */

/* Line 1000 of yacc.c  */
#line 172 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2707 "DeltaParserGen.cpp"
	break;
      case 172: /* "ConstValueExceptString" */

/* Line 1000 of yacc.c  */
#line 222 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2716 "DeltaParserGen.cpp"
	break;
      case 178: /* "StringConstUsed" */

/* Line 1000 of yacc.c  */
#line 172 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2725 "DeltaParserGen.cpp"
	break;
      case 180: /* "FunctionCallObject" */

/* Line 1000 of yacc.c  */
#line 173 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2734 "DeltaParserGen.cpp"
	break;
      case 181: /* "ActualArguments" */

/* Line 1000 of yacc.c  */
#line 178 "DeltaParserSpec.y"
	{ delete (yyvaluep->exprListValue); };

/* Line 1000 of yacc.c  */
#line 2743 "DeltaParserGen.cpp"
	break;
      case 182: /* "ActualArgument" */

/* Line 1000 of yacc.c  */
#line 173 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2752 "DeltaParserGen.cpp"
	break;
      case 183: /* "NonExpressionActualArgument" */

/* Line 1000 of yacc.c  */
#line 173 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2761 "DeltaParserGen.cpp"
	break;
      case 184: /* "FunctionCall" */

/* Line 1000 of yacc.c  */
#line 221 "DeltaParserSpec.y"
	{ delete (yyvaluep->callValue); };

/* Line 1000 of yacc.c  */
#line 2770 "DeltaParserGen.cpp"
	break;
      case 185: /* "ExpressionList" */

/* Line 1000 of yacc.c  */
#line 178 "DeltaParserSpec.y"
	{ delete (yyvaluep->exprListValue); };

/* Line 1000 of yacc.c  */
#line 2779 "DeltaParserGen.cpp"
	break;
      case 186: /* "Lvalue" */

/* Line 1000 of yacc.c  */
#line 171 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2788 "DeltaParserGen.cpp"
	break;
      case 187: /* "TableObject" */

/* Line 1000 of yacc.c  */
#line 173 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2797 "DeltaParserGen.cpp"
	break;
      case 188: /* "DotIndex" */

/* Line 1000 of yacc.c  */
#line 174 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2806 "DeltaParserGen.cpp"
	break;
      case 189: /* "DottedOpString" */

/* Line 1000 of yacc.c  */
#line 174 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2815 "DeltaParserGen.cpp"
	break;
      case 190: /* "TableContent" */

/* Line 1000 of yacc.c  */
#line 219 "DeltaParserSpec.y"
	{ delete (yyvaluep->binaryOpValue); };

/* Line 1000 of yacc.c  */
#line 2824 "DeltaParserGen.cpp"
	break;
      case 191: /* "TableConstructor" */

/* Line 1000 of yacc.c  */
#line 234 "DeltaParserSpec.y"
	{ delete (yyvaluep->tableCtorValue); };

/* Line 1000 of yacc.c  */
#line 2833 "DeltaParserGen.cpp"
	break;
      case 192: /* "TableElements" */

/* Line 1000 of yacc.c  */
#line 232 "DeltaParserSpec.y"
	{ delete (yyvaluep->tableElemsValue); };

/* Line 1000 of yacc.c  */
#line 2842 "DeltaParserGen.cpp"
	break;
      case 193: /* "TableElement" */

/* Line 1000 of yacc.c  */
#line 231 "DeltaParserSpec.y"
	{ delete (yyvaluep->tableElemValue); };

/* Line 1000 of yacc.c  */
#line 2851 "DeltaParserGen.cpp"
	break;
      case 194: /* "ContentList" */

/* Line 1000 of yacc.c  */
#line 178 "DeltaParserSpec.y"
	{ delete (yyvaluep->exprListValue); };

/* Line 1000 of yacc.c  */
#line 2860 "DeltaParserGen.cpp"
	break;
      case 195: /* "ContentExpression" */

/* Line 1000 of yacc.c  */
#line 172 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2869 "DeltaParserGen.cpp"
	break;
      case 196: /* "IndexExpression" */

/* Line 1000 of yacc.c  */
#line 173 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2878 "DeltaParserGen.cpp"
	break;
      case 197: /* "IndexedList" */

/* Line 1000 of yacc.c  */
#line 233 "DeltaParserSpec.y"
	{ delete (yyvaluep->tableIListValue); };

/* Line 1000 of yacc.c  */
#line 2887 "DeltaParserGen.cpp"
	break;
      case 198: /* "IndexedValues" */

/* Line 1000 of yacc.c  */
#line 231 "DeltaParserSpec.y"
	{ delete (yyvaluep->tableElemValue); };

/* Line 1000 of yacc.c  */
#line 2896 "DeltaParserGen.cpp"
	break;
      case 200: /* "MetaGeneratedCode" */

/* Line 1000 of yacc.c  */
#line 176 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2905 "DeltaParserGen.cpp"
	break;
      case 201: /* "NonExpressionElement" */

/* Line 1000 of yacc.c  */
#line 175 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2914 "DeltaParserGen.cpp"
	break;
      case 202: /* "QuotedElementList" */

/* Line 1000 of yacc.c  */
#line 179 "DeltaParserSpec.y"
	{ delete (yyvaluep->elementsValue); };

/* Line 1000 of yacc.c  */
#line 2923 "DeltaParserGen.cpp"
	break;
      case 203: /* "QuotedElements" */

/* Line 1000 of yacc.c  */
#line 179 "DeltaParserSpec.y"
	{ delete (yyvaluep->elementsValue); };

/* Line 1000 of yacc.c  */
#line 2932 "DeltaParserGen.cpp"
	break;
      case 204: /* "SpecialExprListStmt" */

/* Line 1000 of yacc.c  */
#line 175 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2941 "DeltaParserGen.cpp"
	break;
      case 205: /* "SpecialExprListStmts" */

/* Line 1000 of yacc.c  */
#line 198 "DeltaParserSpec.y"
	{ delete (yyvaluep->stmtsValue); };

/* Line 1000 of yacc.c  */
#line 2950 "DeltaParserGen.cpp"
	break;
      case 206: /* "NonFunctionQuotedStmt" */

/* Line 1000 of yacc.c  */
#line 175 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2959 "DeltaParserGen.cpp"
	break;
      case 207: /* "QuotedStmt" */

/* Line 1000 of yacc.c  */
#line 175 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2968 "DeltaParserGen.cpp"
	break;
      case 208: /* "TwoOrMoreQuotedStmts" */

/* Line 1000 of yacc.c  */
#line 198 "DeltaParserSpec.y"
	{ delete (yyvaluep->stmtsValue); };

/* Line 1000 of yacc.c  */
#line 2977 "DeltaParserGen.cpp"
	break;
      case 209: /* "QuasiQuoted" */

/* Line 1000 of yacc.c  */
#line 176 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2986 "DeltaParserGen.cpp"
	break;
      case 210: /* "MetaExpression" */

/* Line 1000 of yacc.c  */
#line 176 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 2995 "DeltaParserGen.cpp"
	break;
      case 211: /* "MetaStmt" */

/* Line 1000 of yacc.c  */
#line 176 "DeltaParserSpec.y"
	{ delete (yyvaluep->nodeValue); };

/* Line 1000 of yacc.c  */
#line 3004 "DeltaParserGen.cpp"
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
int yyparse (delta::ParsingCtx& ctx);
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
yyparse (delta::ParsingCtx& ctx)
#else
int
yyparse (ctx)
    delta::ParsingCtx& ctx;
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

/* Line 1455 of yacc.c  */
#line 293 "DeltaParserSpec.y"
    { 
					ctx.GetProgramDesc().SetAST(0);	
					ctx.GetProgramDesc().SetStmtAST(0);
					ctx.GetProgramDesc().SetFunctionObjectAST(0);
					ctx.GetProgramDesc().SetTableObjectAST(0);
					ctx.GetProgramDesc().SetNamespaceAST(0);
				;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 302 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetAST((yyvsp[(1) - (1)].stmtsValue));	;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 304 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetStmtAST((yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 305 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetFunctionObjectAST((yyvsp[(3) - (3)].nodeValue)); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 306 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetTableObjectAST((yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 307 "DeltaParserSpec.y"
    { ctx.GetProgramDesc().SetNamespaceAST((yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 310 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = (yyvsp[(1) - (1)].stmtsValue);	;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 311 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = 0;	;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 314 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = CreateList<StmtsASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(1) - (2)].stmtsValue), (yyvsp[(2) - (2)].nodeValue)); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 315 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = CreateList<StmtsASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue));	 ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 318 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), CreateNode<VariableASTNode>((yylsp[(2) - (2)]), (yylsp[(2) - (2)])), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 319 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<VariableASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 320 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 326 "DeltaParserSpec.y"
    { (yyval.literalValue) = (yyvsp[(1) - (1)].literalValue); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 327 "DeltaParserSpec.y"
    { (yyval.literalValue) = (yyvsp[(1) - (1)].literalValue); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 328 "DeltaParserSpec.y"
    { (yyval.literalValue) = (yyvsp[(1) - (1)].literalValue); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 331 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].literalValue));  ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 332 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = 0;  ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 335 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), CreateNode<FunctionNameASTNode>((yylsp[(2) - (2)]), (yylsp[(2) - (2)])), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 336 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<FunctionNameASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 337 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 338 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<FunctionNameASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 339 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<FunctionNameASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 340 "DeltaParserSpec.y"
    { (yyval.nodeValue) = 0; ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 348 "DeltaParserSpec.y"
    {
					(yyval.argListValue) = (yyvsp[(2) - (3)].argListValue);
					if ((yyvsp[(3) - (3)].argValue)) {
						(yyval.argListValue)->AppendChild((yyvsp[(3) - (3)].argValue));
						(yyval.argListValue)->GetRange().right = (yyvsp[(3) - (3)].argValue)->GetRange().right;
					}
				;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 355 "DeltaParserSpec.y"
    {
					if ((yyvsp[(2) - (2)].argValue)) {
						YYLTYPE pos = (yylsp[(2) - (2)]);
						pos.last_column = (yyvsp[(2) - (2)].argValue)->GetRange().right;
						(yyval.argListValue) = CreateNode<ArgListASTNode>((yylsp[(2) - (2)]), pos);
						(yyval.argListValue)->AppendChild((yyvsp[(2) - (2)].argValue));
					}
					else
						(yyval.argListValue) = 0;
				;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 365 "DeltaParserSpec.y"
    { (yyval.argListValue) = 0; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 366 "DeltaParserSpec.y"
    { (yyval.argListValue) = 0; ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 369 "DeltaParserSpec.y"
    { (yyval.argValue) = 0; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 370 "DeltaParserSpec.y"
    { (yyval.argValue) = CreateNode<ArgASTNode>((yylsp[(1) - (2)]), (yylsp[(1) - (2)])); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 374 "DeltaParserSpec.y"
    { (yyval.argListValue) = CreateList<ArgListASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].argListValue), (yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 375 "DeltaParserSpec.y"
    { (yyval.argListValue) = CreateList<ArgListASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue));	;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 378 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), CreateNode<ArgASTNode>((yylsp[(2) - (2)]), (yylsp[(2) - (2)])), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 379 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<ArgASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 380 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 383 "DeltaParserSpec.y"
    {
					(yyval.funcValue) = CreateNode<FunctionASTNode>((yylsp[(1) - (5)]), (yylsp[(5) - (5)]), (yyvsp[(1) - (5)].literalValue));
					(yyval.funcValue)->SetChild<0>((yyvsp[(3) - (5)].nodeValue));
					(yyval.funcValue)->SetChild<1>((yyvsp[(4) - (5)].argListValue));
					(yyval.funcValue)->SetChild<2>((yyvsp[(5) - (5)].compValue));
					(yyval.funcValue)->SetChild<3>((yyvsp[(2) - (5)].unaryKwdValue));
				;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 392 "DeltaParserSpec.y"
    {
						(yyval.funcValue) = CreateNode<FunctionASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].literalValue));
						(yyval.funcValue)->SetChild<0>(0);		// Anonymous.
						(yyval.funcValue)->SetChild<1>((yyvsp[(2) - (3)].argListValue));
						(yyval.funcValue)->SetChild<2>((yyvsp[(3) - (3)].nodeValue));
					;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 400 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(2) - (3)].nodeValue); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 406 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<AttributeASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 407 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<AttributeASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 408 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(2) - (2)].nodeValue); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 409 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<AttributeASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 412 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), CONST_KEY_DOTID); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 415 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 416 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 419 "DeltaParserSpec.y"
    {
						(yyval.tableElemValue) =	CreateBinaryNode<TableElemASTNode>(
									(yylsp[(1) - (3)]), 
									(yylsp[(3) - (3)]), 
									CreateNode<ConstASTNode>((yylsp[(1) - (3)]), (yylsp[(1) - (3)]), CONST_STRING), 
									(yyvsp[(3) - (3)].nodeValue)
								);
						delete (yyvsp[(1) - (3)].nodeValue);
					;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 430 "DeltaParserSpec.y"
    {
						(yyval.newAttributeValue) = CreateNode<NewAttributeASTNode>((yylsp[(1) - (5)]), (yylsp[(5) - (5)]));
						(yyval.newAttributeValue)->SetChild<0>((yyvsp[(1) - (5)].nodeValue));
						(yyval.newAttributeValue)->SetChild<1>((yyvsp[(3) - (5)].unaryKwdValue));
						(yyval.newAttributeValue)->SetChild<2>((yyvsp[(4) - (5)].unaryKwdValue));
					;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 438 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 439 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].funcValue); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 442 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 445 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 451 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 452 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 453 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].compValue); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 454 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].funcValue); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 457 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<OtherStmtASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(1) - (2)].exprListValue)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 460 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].unaryKwdValue); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 461 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].whileValue); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 462 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].forValue); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 463 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].foreachValue); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 464 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].ifValue); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 465 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].unaryKwdValue); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 466 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<OtherStmtASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(1) - (2)].leafKwdValue)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 467 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].tryValue); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 468 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].unaryKwdValue); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 469 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<OtherStmtASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 470 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].usingValue); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 471 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 472 "DeltaParserSpec.y"
    {
											(yyval.nodeValue) = 0;
											ctx.GetProgramDesc().AddNotParsed(Range((yylsp[(1) - (1)]).first_column, (yylsp[(1) - (1)]).last_column));
										;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 478 "DeltaParserSpec.y"
    { (yyval.leafKwdValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 479 "DeltaParserSpec.y"
    { (yyval.leafKwdValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 482 "DeltaParserSpec.y"
    { (yyval.compValue) = CreateUnaryNode<CompoundASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].stmtsValue)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 483 "DeltaParserSpec.y"
    { (yyval.compValue) = CreateUnaryNode<CompoundASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), 0);	 ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 486 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].nodeValue), (yyvsp[(1) - (3)].literalValue)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 489 "DeltaParserSpec.y"
    {
					(yyval.whileValue) = CreateNode<WhileASTNode>((yylsp[(1) - (5)]), (yylsp[(5) - (5)]));
					(yyval.whileValue)->SetChild<0>((yyvsp[(3) - (5)].nodeValue));
					(yyval.whileValue)->SetChild<1>((yyvsp[(5) - (5)].nodeValue));
				;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 496 "DeltaParserSpec.y"
    {
				(yyval.ifValue) = CreateNode<IfASTNode>((yylsp[(1) - (6)]), (yylsp[(6) - (6)]));
				(yyval.ifValue)->SetChild<0>((yyvsp[(3) - (6)].nodeValue));
				(yyval.ifValue)->SetChild<1>((yyvsp[(5) - (6)].nodeValue));
				(yyval.ifValue)->SetChild<2>((yyvsp[(6) - (6)].elseValue));
			;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 504 "DeltaParserSpec.y"
    { (yyval.elseValue) = CreateUnaryNode<ElseASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue)); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 505 "DeltaParserSpec.y"
    { (yyval.elseValue) = 0; ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 508 "DeltaParserSpec.y"
    {
				(yyval.forValue) = CreateNode<ForASTNode>((yylsp[(1) - (9)]), (yylsp[(9) - (9)]));
				(yyval.forValue)->SetChild<0>((yyvsp[(3) - (9)].exprListValue));
				(yyval.forValue)->SetChild<1>((yyvsp[(5) - (9)].nodeValue));
				(yyval.forValue)->SetChild<2>((yyvsp[(7) - (9)].exprListValue));
				(yyval.forValue)->SetChild<3>((yyvsp[(9) - (9)].nodeValue));
			;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 517 "DeltaParserSpec.y"
    { (yyval.exprListValue) = (yyvsp[(1) - (1)].exprListValue); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 518 "DeltaParserSpec.y"
    { (yyval.exprListValue) = 0; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 521 "DeltaParserSpec.y"
    { (yyval.exprListValue) = (yyvsp[(1) - (1)].exprListValue); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 522 "DeltaParserSpec.y"
    { (yyval.exprListValue) = 0; ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 525 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(2) - (3)].nodeValue); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 526 "DeltaParserSpec.y"
    { (yyval.nodeValue) = 0; ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 529 "DeltaParserSpec.y"
    {
					(yyval.foreachValue) = CreateNode<ForeachASTNode>((yylsp[(1) - (7)]), (yylsp[(7) - (7)]));
					(yyval.foreachValue)->SetChild<0>((yyvsp[(3) - (7)].nodeValue));
					(yyval.foreachValue)->SetChild<1>((yyvsp[(5) - (7)].nodeValue));
					(yyval.foreachValue)->SetChild<2>((yyvsp[(7) - (7)].nodeValue));
					(yyval.foreachValue)->SetChild<3>((yyvsp[(4) - (7)].nodeValue));
				;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 538 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].nodeValue), (yyvsp[(1) - (3)].literalValue)); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 539 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), 0, (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 540 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].funcValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 543 "DeltaParserSpec.y"
    { (yyval.unaryKwdValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].nodeValue), (yyvsp[(1) - (3)].literalValue)); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 546 "DeltaParserSpec.y"
    {
				(yyval.tryValue) = CreateNode<TryASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
				(yyval.tryValue)->SetChild<0>((yyvsp[(2) - (3)].nodeValue));
				(yyval.tryValue)->SetChild<1>((yyvsp[(3) - (3)].trapValue));
			;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 553 "DeltaParserSpec.y"
    {
				(yyval.trapValue) = CreateNode<TrapASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
				(yyval.trapValue)->SetChild<0>((yyvsp[(2) - (3)].nodeValue));
				(yyval.trapValue)->SetChild<1>((yyvsp[(3) - (3)].nodeValue));
			;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 563 "DeltaParserSpec.y"
    {
						(yyval.usingValue) = CreateNode<UsingASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].literalValue));
						(yyval.usingValue)->SetChild<0>((yyvsp[(2) - (3)].nodeValue));
					;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 567 "DeltaParserSpec.y"
    {
						(yyval.usingValue) = CreateNode<UsingASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]), (yyvsp[(1) - (4)].literalValue));
						(yyval.usingValue)->SetChild<1>(CreateNode<StringifiedIdASTNode>((yylsp[(2) - (4)]), (yylsp[(3) - (4)])));
					;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 571 "DeltaParserSpec.y"
    {
						(yyval.usingValue) = CreateNode<UsingASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]), (yyvsp[(1) - (4)].literalValue));
						(yyval.usingValue)->SetChild<1>((yyvsp[(3) - (4)].nodeValue));
					;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 577 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 578 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 582 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 585 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 586 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 589 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 595 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].binaryOpValue); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 596 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].binaryOpValue); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 597 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].binaryOpValue); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 598 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].binaryOpValue); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 599 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].binaryOpValue); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 600 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].ternaryOpValue); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 601 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 604 "DeltaParserSpec.y"
    {
								UnaryKwdASTNode* kwd = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (4)]), (yylsp[(2) - (4)]), (yyvsp[(2) - (4)].nodeValue), (yyvsp[(1) - (4)].literalValue));
								(yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]), kwd, (yyvsp[(4) - (4)].nodeValue), (yyvsp[(3) - (4)].literalValue)); 
							;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 610 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 611 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 612 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 613 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 614 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 615 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 619 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 621 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 623 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 625 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 627 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 629 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 633 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 635 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 639 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 641 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 643 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 645 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 647 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 650 "DeltaParserSpec.y"
    {
							(yyval.ternaryOpValue) = CreateNode<TernaryOpASTNode>((yylsp[(1) - (7)]), (yylsp[(7) - (7)]));
							(yyval.ternaryOpValue)->SetChild<0>((yyvsp[(2) - (7)].nodeValue));
							(yyval.ternaryOpValue)->SetChild<1>((yyvsp[(4) - (7)].nodeValue));
							(yyval.ternaryOpValue)->SetChild<2>((yyvsp[(6) - (7)].nodeValue));
						;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 658 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<SuffixOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(1) - (2)].nodeValue), (yyvsp[(2) - (2)].literalValue)); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 659 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<PrefixOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 660 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<SuffixOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(1) - (2)].nodeValue), (yyvsp[(2) - (2)].literalValue)); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 661 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<PrefixOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 662 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<PrefixOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 663 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<PrefixOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 664 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 665 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 668 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 669 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 670 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].funcValue); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 673 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 674 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].callValue); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 675 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].tableCtorValue); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 676 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].funcValue), "()"); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 677 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].nodeValue), "()"); ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 678 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 679 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 680 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 681 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 684 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<ConstASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_NUMBER); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 685 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 686 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 687 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 732 "DeltaParserSpec.y"
    { delete (yyvsp[(2) - (2)].nodeValue); ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 733 "DeltaParserSpec.y"
    { delete (yyvsp[(2) - (2)].nodeValue); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 736 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<ConstASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_STRING); ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 737 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<StringifiedIdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 738 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<StringifiedIdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)])); ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 771 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 772 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 775 "DeltaParserSpec.y"
    { (yyval.exprListValue) = CreateList<ExpressionListASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].exprListValue), (yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 776 "DeltaParserSpec.y"
    { (yyval.exprListValue) = CreateList<ExpressionListASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue));	;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 779 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 780 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 783 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].nodeValue), "||"); ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 784 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<LeafKwdASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), (yyvsp[(1) - (1)].literalValue)); ;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 785 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].funcValue); ;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 788 "DeltaParserSpec.y"
    {
						(yyval.callValue) = CreateNode<CallASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]));
						(yyval.callValue)->SetChild<0>((yyvsp[(1) - (4)].nodeValue));
						(yyval.callValue)->SetChild<1>((yyvsp[(3) - (4)].exprListValue));
					;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 793 "DeltaParserSpec.y"
    {
						(yyval.callValue) = CreateNode<CallASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
						(yyval.callValue)->SetChild<0>((yyvsp[(1) - (3)].nodeValue));
					;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 797 "DeltaParserSpec.y"
    {
						(yyval.callValue) = CreateNode<CallASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
						(yyval.callValue)->SetChild<0>((yyvsp[(1) - (2)].nodeValue));
					;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 803 "DeltaParserSpec.y"
    { (yyval.exprListValue) = CreateList<ExpressionListASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].exprListValue), (yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 804 "DeltaParserSpec.y"
    { (yyval.exprListValue) = CreateList<ExpressionListASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue));	;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 810 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 811 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 812 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 813 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryKwdASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 814 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].binaryOpValue); ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 820 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 824 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_KEY_IDENT); ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 825 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_KEY_IDENT); ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 826 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_KEY_STRING); ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 827 "DeltaParserSpec.y"
    { (yyval.nodeValue) = ExpressionToConstKey((yylsp[(1) - (1)]), (yyvsp[(1) - (1)].nodeValue)); ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 831 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_KEY_STRING); ++((yyval.nodeValue)->GetRange().left); ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 833 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_KEY_STRING); ++((yyval.nodeValue)->GetRange().left);;}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 835 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_KEY_STRING); ++((yyval.nodeValue)->GetRange().left);;}
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 837 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_KEY_STRING); ++((yyval.nodeValue)->GetRange().left);;}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 840 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 841 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(1) - (2)].nodeValue), (yyvsp[(2) - (2)].nodeValue), "."); ;}
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 842 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].nodeValue), (yyvsp[(3) - (3)].nodeValue), (yyvsp[(2) - (3)].literalValue)); ;}
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 844 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]), (yyvsp[(1) - (4)].nodeValue), ExpressionToConstKey((yylsp[(3) - (4)]), (yyvsp[(3) - (4)].nodeValue)), "[]"); ;}
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 846 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]), (yyvsp[(1) - (4)].nodeValue), CreateNode<LeafKwdASTNode>((yylsp[(3) - (4)]), (yylsp[(3) - (4)])), "[]"); ;}
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 848 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]), (yyvsp[(1) - (4)].nodeValue), ExpressionToConstKey((yylsp[(3) - (4)]), (yyvsp[(3) - (4)].nodeValue)), "[[]]"); ;}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 850 "DeltaParserSpec.y"
    { (yyval.binaryOpValue) = CreateBinaryNode<BinaryOpASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]), (yyvsp[(1) - (4)].nodeValue), CreateNode<LeafKwdASTNode>((yylsp[(3) - (4)]), (yylsp[(3) - (4)])), "[[]]"); ;}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 853 "DeltaParserSpec.y"
    { (yyval.tableCtorValue) = CreateUnaryNode<TableConstructASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].tableElemsValue)); ;}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 854 "DeltaParserSpec.y"
    { (yyval.tableCtorValue) = CreateUnaryNode<TableConstructASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), 0); ;}
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 857 "DeltaParserSpec.y"
    { (yyval.tableElemsValue) = CreateList<TableElemsASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].tableElemsValue), (yyvsp[(3) - (3)].tableElemValue));	;}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 858 "DeltaParserSpec.y"
    { (yyval.tableElemsValue) = CreateList<TableElemsASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].tableElemValue));	;}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 861 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = CreateBinaryNode<TableElemASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue));	;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 862 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = CreateBinaryNode<TableElemASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].funcValue));	;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 863 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = (yyvsp[(1) - (1)].tableElemValue); ;}
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 864 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = CreateBinaryNode<TableElemASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].newAttributeValue));	;}
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 865 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = (yyvsp[(1) - (1)].tableElemValue);;}
    break;

  case 266:

/* Line 1455 of yacc.c  */
#line 868 "DeltaParserSpec.y"
    { (yyval.exprListValue) = CreateList<ExpressionListASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].exprListValue), (yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 267:

/* Line 1455 of yacc.c  */
#line 869 "DeltaParserSpec.y"
    { (yyval.exprListValue) = CreateList<ExpressionListASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue));	;}
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 872 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 873 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].funcValue); ;}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 876 "DeltaParserSpec.y"
    { (yyval.nodeValue) = ExpressionToConstKey((yylsp[(1) - (1)]), (yyvsp[(1) - (1)].nodeValue)); ;}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 877 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 878 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_KEY_OPSTRING); ;}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 881 "DeltaParserSpec.y"
    { (yyval.tableIListValue) = CreateList<TableIndexListASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].tableIListValue), (yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 882 "DeltaParserSpec.y"
    { (yyval.tableIListValue) = CreateList<TableIndexListASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue));	;}
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 885 "DeltaParserSpec.y"
    { (yyval.tableElemValue) = CreateBinaryNode<TableElemASTNode>((yylsp[(1) - (5)]), (yylsp[(5) - (5)]), (yyvsp[(2) - (5)].tableIListValue), (yyvsp[(4) - (5)].exprListValue));	;}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 891 "DeltaParserSpec.y"
    { (yyval.literalValue) = (yyvsp[(1) - (2)].literalValue); ;}
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 892 "DeltaParserSpec.y"
    { (yyval.literalValue) = (yyvsp[(1) - (1)].literalValue); ;}
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 895 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]), (yyvsp[(3) - (4)].nodeValue), (yyvsp[(1) - (4)].literalValue));	;}
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 897 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), CreateNode<VariableASTNode>((yylsp[(2) - (2)]), (yylsp[(2) - (2)])), (yyvsp[(1) - (2)].literalValue)); ;}
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 898 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (4)]), (yylsp[(4) - (4)]), (yyvsp[(3) - (4)].nodeValue), (yyvsp[(1) - (4)].literalValue)); ;}
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 901 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].tableElemValue); ;}
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 902 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].newAttributeValue); ;}
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 903 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].tableElemValue); ;}
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 904 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 905 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateNode<TableConstKeyASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), CONST_KEY_OPSTRING); ;}
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 906 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 910 "DeltaParserSpec.y"
    { (yyval.elementsValue) = CreateList<QuotedElementsASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), ExpressionListToQuotedElementList((yyvsp[(1) - (3)].exprListValue)), (yyvsp[(3) - (3)].nodeValue)); ;}
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 912 "DeltaParserSpec.y"
    { (yyval.elementsValue) = CreateList<QuotedElementsASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].elementsValue), (yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 914 "DeltaParserSpec.y"
    { (yyval.elementsValue) = CreateList<QuotedElementsASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(1) - (3)].elementsValue), (yyvsp[(3) - (3)].nodeValue));	;}
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 915 "DeltaParserSpec.y"
    { (yyval.elementsValue) = CreateList<QuotedElementsASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue)); ;}
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 918 "DeltaParserSpec.y"
    { (yyval.elementsValue) = (yyvsp[(1) - (1)].elementsValue); ;}
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 919 "DeltaParserSpec.y"
    { (yyval.elementsValue) = ExpressionListToQuotedElementList((yyvsp[(1) - (1)].exprListValue)); ;}
    break;

  case 293:

/* Line 1455 of yacc.c  */
#line 922 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 294:

/* Line 1455 of yacc.c  */
#line 923 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (2)].tableIListValue); ;}
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 924 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].compValue); ;}
    break;

  case 296:

/* Line 1455 of yacc.c  */
#line 925 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].funcValue); ;}
    break;

  case 297:

/* Line 1455 of yacc.c  */
#line 928 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = CreateList<StmtsASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(1) - (2)].stmtsValue), (yyvsp[(2) - (2)].nodeValue)); ;}
    break;

  case 298:

/* Line 1455 of yacc.c  */
#line 929 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = CreateList<StmtsASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue));	 ;}
    break;

  case 299:

/* Line 1455 of yacc.c  */
#line 932 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 300:

/* Line 1455 of yacc.c  */
#line 933 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 301:

/* Line 1455 of yacc.c  */
#line 934 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<CompoundASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].stmtsValue)); ;}
    break;

  case 302:

/* Line 1455 of yacc.c  */
#line 935 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<CompoundASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), 0);	 ;}
    break;

  case 303:

/* Line 1455 of yacc.c  */
#line 938 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].nodeValue); ;}
    break;

  case 304:

/* Line 1455 of yacc.c  */
#line 939 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].funcValue); ;}
    break;

  case 305:

/* Line 1455 of yacc.c  */
#line 942 "DeltaParserSpec.y"
    { (yyval.stmtsValue) = CreateList<StmtsASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(1) - (2)].stmtsValue), (yyvsp[(2) - (2)].nodeValue)); ;}
    break;

  case 306:

/* Line 1455 of yacc.c  */
#line 943 "DeltaParserSpec.y"
    {
								(yyval.stmtsValue) = CreateList<StmtsASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), 0, (yyvsp[(1) - (2)].nodeValue));
								(yyval.stmtsValue)->AppendChild((yyvsp[(2) - (2)].nodeValue));
							;}
    break;

  case 307:

/* Line 1455 of yacc.c  */
#line 949 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].elementsValue); ;}
    break;

  case 308:

/* Line 1455 of yacc.c  */
#line 950 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateList<StmtsASTNode>((yylsp[(1) - (1)]), (yylsp[(1) - (1)]), 0, (yyvsp[(1) - (1)].nodeValue)); ;}
    break;

  case 309:

/* Line 1455 of yacc.c  */
#line 951 "DeltaParserSpec.y"
    { (yyval.nodeValue) = (yyvsp[(1) - (1)].stmtsValue); ;}
    break;

  case 310:

/* Line 1455 of yacc.c  */
#line 954 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (3)]), (yylsp[(3) - (3)]), (yyvsp[(2) - (3)].nodeValue), "<<>>"); ;}
    break;

  case 311:

/* Line 1455 of yacc.c  */
#line 957 "DeltaParserSpec.y"
    { (yyval.nodeValue) = CreateUnaryNode<UnaryOpASTNode>((yylsp[(1) - (2)]), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].nodeValue), (yyvsp[(1) - (2)].literalValue)); ;}
    break;



/* Line 1455 of yacc.c  */
#line 5178 "DeltaParserGen.cpp"
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




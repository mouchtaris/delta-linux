
/*  A Bison parser, made from parsercpy.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	IDENT	258
#define	RESERVEDATTR	259
#define	MONITOR	260
#define	ATTRIBUTE	261
#define	REAL_CONST	262
#define	INT_CONST	263
#define	HEXINT_CONST	264
#define	STRING_CONST	265
#define	STATIC	266
#define	AND	267
#define	NOT	268
#define	OR	269
#define	UMINUS	270
#define	TRUE	271
#define	FALSE	272
#define	FUNCRES	273
#define	NEW	274
#define	SINGLE_RIGHT_SB	275
#define	SINGLE_LEFT_SB	276
#define	FORMAL	277
#define	DELETE	278
#define	NIL	279
#define	LOCAL	280
#define	LT	281
#define	GT	282
#define	LE	283
#define	GE	284
#define	EQ	285
#define	NE	286
#define	DOT	287
#define	DOUBLE_DOT	288
#define	DOUBLE_LEFT_SB	289
#define	DOUBLE_RIGHT_SB	290
#define	PARENTHESIS	291
#define	SQUARE_BRACKETS	292
#define	LAMBDA	293

#line 1 "parsercpy.y"


// Debug expression evaluator.
// Bison parser generator.
// Uses hand-made lexical analyser.
// ScriptFighter Project.
// A. Savidis, Ferbuary 2002 (original version).
//

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "DDebug.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DebugExprParseActions.h"
#include "DebugExprLexAn.h"
#include "DebugEvaluatedExpr.h"

// Bison assumes alloca is the memory allocation
// function.
//
#ifndef	alloca
#define alloca malloc
#endif

// We want the yylval to be passed as a parameter to yylex,
// not being a global variable. This is why we define YYPURE.
//
#define	YYPURE

#define	yyparse	DebugExprParser_yyparse
#define yylex	DebugExprLexAn::GetToken
#define	yydebug	DebugExprParser_yydebug
#define	yyerror	DebugExprParser_yyerror

static void DebugExprParser_yyerror (const char* s) {
	if (!DebugExprLexAn::LastEOFWasForced())
		DebugExprParseActions::Error("%s", s);
}

#define	ACTIONS \
	DebugExprParseActions


#line 48 "parsercpy.y"
typedef union {
	char*					id;					// Variable identifier.
	DeltaNumberValueType	realConst;			// Constant numeric value.
	util_ui32				intConst;			// Constant integer value.
	char*					strConst;			// String constant value.
	DebugEvaluatedExpr*		expr;
	DebugExprList*			exprList;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		104
#define	YYFLAG		-32768
#define	YYNTBASE	48

#define YYTRANSLATE(x) ((unsigned)(x) <= 293 ? yytranslate[x] : 67)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    45,     2,     2,    36,
    37,    43,    41,    46,    42,     2,    44,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    47,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    38,    39,    40
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     8,    10,    12,    14,    16,    20,
    24,    28,    32,    36,    40,    44,    48,    52,    56,    60,
    64,    68,    71,    74,    78,    80,    82,    86,    91,    93,
    95,    97,    99,   101,   103,   105,   107,   109,   111,   113,
   117,   121,   124,   127,   131,   135,   137,   139,   141,   143,
   145,   147,   149,   151,   153,   155,   159,   163,   168,   173,
   177,   182
};

static const short yyrhs[] = {    49,
     0,    60,     0,    65,     0,    66,     0,    50,     0,    51,
     0,    52,     0,    53,     0,    49,    27,    49,     0,    49,
    26,    49,     0,    49,    29,    49,     0,    49,    28,    49,
     0,    49,    30,    49,     0,    49,    31,    49,     0,    49,
    12,    49,     0,    49,    14,    49,     0,    49,    41,    49,
     0,    49,    42,    49,     0,    49,    44,    49,     0,    49,
    43,    49,     0,    49,    45,    49,     0,    42,    49,     0,
    13,    49,     0,    36,    49,    37,     0,    56,     0,    49,
     0,    54,    46,    49,     0,     4,    36,    54,    37,     0,
    61,     0,    57,     0,    55,     0,     7,     0,     8,     0,
     9,     0,    10,     0,    24,     0,    16,     0,    17,     0,
     3,     0,    58,    18,     3,     0,    22,    32,     3,     0,
    25,     3,     0,    18,     3,     0,    11,    32,    58,     0,
    59,    47,    49,     0,    58,     0,    59,     0,    64,     0,
     5,     0,     6,     0,    40,     0,    61,     0,    55,     0,
     3,     0,     6,     0,    62,    32,    63,     0,    62,    33,
    63,     0,    62,    21,    49,    20,     0,    62,    34,    49,
    35,     0,    62,    32,     4,     0,    19,     5,    47,    49,
     0,    23,     5,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    87,    89,    91,    93,    97,    99,   101,   103,   107,   109,
   111,   113,   115,   117,   122,   124,   128,   130,   132,   134,
   136,   140,   142,   144,   146,   150,   152,   156,   160,   162,
   164,   168,   170,   172,   174,   176,   178,   180,   184,   186,
   190,   192,   194,   196,   200,   204,   206,   208,   210,   212,
   214,   218,   219,   222,   223,   226,   228,   231,   233,   236,
   240,   244
};

static const char * const yytname[] = {   "$","error","$undefined.","IDENT",
"RESERVEDATTR","MONITOR","ATTRIBUTE","REAL_CONST","INT_CONST","HEXINT_CONST",
"STRING_CONST","STATIC","AND","NOT","OR","UMINUS","TRUE","FALSE","FUNCRES","NEW",
"SINGLE_RIGHT_SB","SINGLE_LEFT_SB","FORMAL","DELETE","NIL","LOCAL","LT","GT",
"LE","GE","EQ","NE","DOT","DOUBLE_DOT","DOUBLE_LEFT_SB","DOUBLE_RIGHT_SB","'('",
"')'","PARENTHESIS","SQUARE_BRACKETS","LAMBDA","'+'","'-'","'*'","'/'","'%'",
"','","'='","DebugExpression","Expression","RelationalExpression","BooleanExpression",
"ArithmeticExpression","Term","ExpressionList","FunctionCall","Primary","ConstValue",
"Name","LvalueForAssign","Assign","Lvalue","Table","DotIdent","TableContent",
"NewMonitor","DeleteMonitor",""
};
#endif

static const short yyr1[] = {     0,
    48,    48,    48,    48,    49,    49,    49,    49,    50,    50,
    50,    50,    50,    50,    51,    51,    52,    52,    52,    52,
    52,    53,    53,    53,    53,    54,    54,    55,    56,    56,
    56,    57,    57,    57,    57,    57,    57,    57,    58,    58,
    59,    59,    59,    59,    60,    61,    61,    61,    61,    61,
    61,    62,    62,    63,    63,    64,    64,    64,    64,    64,
    65,    66
};

static const short yyr2[] = {     0,
     1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     2,     2,     3,     1,     1,     3,     4,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
     3,     2,     2,     3,     3,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     3,     3,     4,     4,     3,
     4,     2
};

static const short yydefact[] = {     0,
    39,     0,    49,    50,    32,    33,    34,    35,     0,     0,
    37,    38,     0,     0,     0,     0,    36,     0,     0,    51,
     0,     1,     5,     6,     7,     8,    31,    25,    30,    46,
    47,     2,    29,     0,    48,     3,     4,     0,     0,    23,
    47,    43,     0,     0,    62,    42,     0,    22,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    26,     0,    44,
     0,    41,    24,    15,    16,    10,     9,    12,    11,    13,
    14,    17,    18,    20,    19,    21,    40,    45,     0,    54,
    60,    55,    56,    57,     0,    28,     0,    61,    58,    59,
    27,     0,     0,     0
};

static const short yydefgoto[] = {   102,
    22,    23,    24,    25,    26,    69,    27,    28,    29,    30,
    41,    32,    33,    34,    93,    35,    36,    37
};

static const short yypact[] = {    59,
-32768,   -34,-32768,-32768,-32768,-32768,-32768,-32768,   -18,    99,
-32768,-32768,    18,    17,    -5,    24,-32768,    35,    99,-32768,
    99,   156,-32768,-32768,-32768,-32768,    -9,-32768,-32768,    34,
     8,-32768,    58,    64,-32768,-32768,-32768,    99,    53,-32768,
-32768,-32768,    11,    56,-32768,-32768,   -11,-32768,    99,    99,
    99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
    99,    57,    99,    99,     2,     4,    99,   156,   -33,    34,
    99,-32768,-32768,   182,   162,   110,   110,   110,   110,   188,
   188,    -8,    -8,-32768,-32768,-32768,-32768,   156,   116,-32768,
-32768,-32768,-32768,-32768,   136,-32768,    99,   156,-32768,-32768,
   156,    71,    73,-32768
};

static const short yypgoto[] = {-32768,
   -10,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    41,
    74,-32768,-32768,-32768,    20,-32768,-32768,-32768
};


#define	YYLAST		233


static const short yytable[] = {    40,
    49,    38,    50,    96,    90,    91,    90,    92,    47,    92,
    48,   -53,    97,    39,    51,    52,    53,    54,    55,    56,
    42,    43,   -53,   -53,   -53,    73,    44,    68,    45,    57,
    58,    59,    60,    61,    59,    60,    61,    46,    74,    75,
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    62,    88,    89,    63,     1,    95,    71,    72,    87,
    98,     1,     2,     3,     4,     5,     6,     7,     8,     9,
   103,    10,   104,    31,    11,    12,    13,    14,   -52,    70,
    15,    16,    17,    18,    64,    94,   101,     0,     0,   -52,
   -52,   -52,     0,     0,    19,    65,    66,    67,    20,     0,
    21,     1,     2,     3,     4,     5,     6,     7,     8,     9,
     0,    10,     0,     0,    11,    12,    13,     0,     0,     0,
    15,     0,    17,    18,     0,     0,     0,    49,     0,    50,
     0,     0,     0,     0,    19,    99,     0,     0,    20,     0,
    21,    51,    52,    53,    54,    55,    56,    49,     0,    50,
    57,    58,    59,    60,    61,     0,    57,    58,    59,    60,
    61,    51,    52,    53,    54,    55,    56,    49,     0,    50,
   100,     0,     0,    49,     0,     0,    57,    58,    59,    60,
    61,    51,    52,    53,    54,    55,    56,    51,    52,    53,
    54,    55,    56,     0,     0,     0,    57,    58,    59,    60,
    61,     0,    57,    58,    59,    60,    61,    51,    52,    53,
    54,    55,    56,    51,    52,    53,    54,     0,     0,     0,
     0,     0,    57,    58,    59,    60,    61,     0,    57,    58,
    59,    60,    61
};

static const short yycheck[] = {    10,
    12,    36,    14,    37,     3,     4,     3,     6,    19,     6,
    21,    21,    46,    32,    26,    27,    28,    29,    30,    31,
     3,     5,    32,    33,    34,    37,    32,    38,     5,    41,
    42,    43,    44,    45,    43,    44,    45,     3,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    18,    63,    64,    47,     3,    67,    47,     3,     3,
    71,     3,     4,     5,     6,     7,     8,     9,    10,    11,
     0,    13,     0,     0,    16,    17,    18,    19,    21,    39,
    22,    23,    24,    25,    21,    66,    97,    -1,    -1,    32,
    33,    34,    -1,    -1,    36,    32,    33,    34,    40,    -1,
    42,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    -1,    13,    -1,    -1,    16,    17,    18,    -1,    -1,    -1,
    22,    -1,    24,    25,    -1,    -1,    -1,    12,    -1,    14,
    -1,    -1,    -1,    -1,    36,    20,    -1,    -1,    40,    -1,
    42,    26,    27,    28,    29,    30,    31,    12,    -1,    14,
    41,    42,    43,    44,    45,    -1,    41,    42,    43,    44,
    45,    26,    27,    28,    29,    30,    31,    12,    -1,    14,
    35,    -1,    -1,    12,    -1,    -1,    41,    42,    43,    44,
    45,    26,    27,    28,    29,    30,    31,    26,    27,    28,
    29,    30,    31,    -1,    -1,    -1,    41,    42,    43,    44,
    45,    -1,    41,    42,    43,    44,    45,    26,    27,    28,
    29,    30,    31,    26,    27,    28,    29,    -1,    -1,    -1,
    -1,    -1,    41,    42,    43,    44,    45,    -1,    41,    42,
    43,    44,    45
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 88 "parsercpy.y"
{ ACTIONS::SetResultExpression(yyvsp[0].expr); ;
    break;}
case 2:
#line 90 "parsercpy.y"
{ ACTIONS::SetResultExpression(yyvsp[0].expr); ;
    break;}
case 3:
#line 92 "parsercpy.y"
{ ACTIONS::SetResultExpression(yyvsp[0].expr); ;
    break;}
case 4:
#line 94 "parsercpy.y"
{ ACTIONS::SetResultExpression(yyvsp[0].expr); ;
    break;}
case 5:
#line 98 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 6:
#line 100 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 7:
#line 102 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 8:
#line 104 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 9:
#line 108 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_RelationalExpr(yyvsp[-2].expr,yyvsp[0].expr, DebugExprRelOp_GT); ;
    break;}
case 10:
#line 110 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_RelationalExpr(yyvsp[-2].expr,yyvsp[0].expr, DebugExprRelOp_LT); ;
    break;}
case 11:
#line 112 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_RelationalExpr(yyvsp[-2].expr,yyvsp[0].expr, DebugExprRelOp_GE); ;
    break;}
case 12:
#line 114 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_RelationalExpr(yyvsp[-2].expr,yyvsp[0].expr, DebugExprRelOp_LE); ;
    break;}
case 13:
#line 116 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_RelationalExpr(yyvsp[-2].expr,yyvsp[0].expr, DebugExprRelOp_EQ); ;
    break;}
case 14:
#line 118 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_RelationalExpr(yyvsp[-2].expr,yyvsp[0].expr, DebugExprRelOp_NE); ;
    break;}
case 15:
#line 123 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ExprANDExpr(yyvsp[-2].expr, yyvsp[0].expr); ;
    break;}
case 16:
#line 125 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ExprORExpr(yyvsp[-2].expr, yyvsp[0].expr); ;
    break;}
case 17:
#line 129 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ArithmeticExpr(yyvsp[-2].expr, yyvsp[0].expr, DebugExpArithOp_ADD); ;
    break;}
case 18:
#line 131 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ArithmeticExpr(yyvsp[-2].expr, yyvsp[0].expr, DebugExpArithOp_SUB); ;
    break;}
case 19:
#line 133 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ArithmeticExpr(yyvsp[-2].expr, yyvsp[0].expr, DebugExpArithOp_DIV); ;
    break;}
case 20:
#line 135 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ArithmeticExpr(yyvsp[-2].expr, yyvsp[0].expr, DebugExpArithOp_MUL); ;
    break;}
case 21:
#line 137 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ArithmeticExpr(yyvsp[-2].expr, yyvsp[0].expr, DebugExpArithOp_MOD); ;
    break;}
case 22:
#line 141 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_UMINUSExpression(yyvsp[0].expr); ;
    break;}
case 23:
#line 143 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_NOTExpression(yyvsp[0].expr); ;
    break;}
case 24:
#line 145 "parsercpy.y"
{ yyval.expr = yyvsp[-1].expr; ;
    break;}
case 25:
#line 147 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 26:
#line 151 "parsercpy.y"
{ yyval.exprList = ACTIONS::Manage_ExpressionList(yyvsp[0].expr); ;
    break;}
case 27:
#line 153 "parsercpy.y"
{ yyval.exprList = ACTIONS::Manage_ExpressionList(yyvsp[-2].exprList, yyvsp[0].expr); ;
    break;}
case 28:
#line 157 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_UnaryLibraryFuncCall(yyvsp[-3].id, yyvsp[-1].exprList); ;
    break;}
case 29:
#line 161 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 30:
#line 163 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 31:
#line 165 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 32:
#line 169 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ConstValue(yyvsp[0].realConst); ;
    break;}
case 33:
#line 171 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ConstValue((DeltaNumberValueType) yyvsp[0].intConst); ;
    break;}
case 34:
#line 173 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ConstValue((DeltaNumberValueType) yyvsp[0].intConst); ;
    break;}
case 35:
#line 175 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ConstValue(yyvsp[0].strConst); ;
    break;}
case 36:
#line 177 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ConstValue(); ;
    break;}
case 37:
#line 179 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ConstValue(true); ;
    break;}
case 38:
#line 181 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ConstValue(false); ;
    break;}
case 39:
#line 185 "parsercpy.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 40:
#line 187 "parsercpy.y"
{ yyval.id = ACTIONS::Manage_Name(yyvsp[-2].id, yyvsp[0].id); ;
    break;}
case 41:
#line 191 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_FormalLvalue(yyvsp[0].id); ;
    break;}
case 42:
#line 193 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_LocalLvalue(yyvsp[0].id); ;
    break;}
case 43:
#line 195 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_GlobalLvalue(yyvsp[0].id, false); ;
    break;}
case 44:
#line 197 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_GlobalLvalue(yyvsp[0].id, true); ;
    break;}
case 45:
#line 201 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_AssignLvalue(yyvsp[-2].expr, yyvsp[0].expr); ;
    break;}
case 46:
#line 205 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_AnyLvalue(yyvsp[0].id); ;
    break;}
case 47:
#line 207 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 48:
#line 209 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 49:
#line 211 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_MONITOR(yyvsp[0].id); ;
    break;}
case 50:
#line 213 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_ATTRIBUTE(yyvsp[0].id); ;
    break;}
case 51:
#line 215 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_LAMBDA(); ;
    break;}
case 52:
#line 218 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 53:
#line 219 "parsercpy.y"
{ yyval.expr = yyvsp[0].expr; ;
    break;}
case 54:
#line 222 "parsercpy.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 55:
#line 223 "parsercpy.y"
{ yyval.id = ACTIONS::Manage_AttributeIndex(yyvsp[0].id); ;
    break;}
case 56:
#line 227 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_TableContent(yyvsp[-2].expr, yyvsp[0].id, false); ;
    break;}
case 57:
#line 229 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_TableContent(yyvsp[-2].expr, yyvsp[0].id, true); ;
    break;}
case 58:
#line 232 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_TableContent(yyvsp[-3].expr, yyvsp[-1].expr, false); ;
    break;}
case 59:
#line 234 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_TableContent(yyvsp[-3].expr, yyvsp[-1].expr, true); ;
    break;}
case 60:
#line 237 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_TableContent_ReservedAttr(yyvsp[-2].expr, yyvsp[0].id); ;
    break;}
case 61:
#line 241 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_NewMonitor(yyvsp[-2].id, yyvsp[0].expr); ;
    break;}
case 62:
#line 245 "parsercpy.y"
{ yyval.expr = ACTIONS::Manage_DeleteMonitor(yyvsp[0].id); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 248 "parsercpy.y"

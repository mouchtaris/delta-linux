typedef union {
	char*					id;					// Variable identifier.
	DeltaNumberValueType	realConst;			// Constant numeric value.
	util_ui32				intConst;			// Constant integer value.
	char*					strConst;			// String constant value.
	DebugEvaluatedExpr*		expr;
	DebugExprList*			exprList;
} YYSTYPE;
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


extern YYSTYPE yylval;

typedef union {
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
#define	IDENT	258
#define	ATTRIBUTE_IDENT	259
#define	NUMBER_CONST	260
#define	STRING_CONST	261
#define	FUNCTION	262
#define	RETURN	263
#define	ONEVENT	264
#define	IF	265
#define	ELSE	266
#define	WHILE	267
#define	FOR	268
#define	FOREACH	269
#define	NIL	270
#define	LOCAL	271
#define	AND	272
#define	NOT	273
#define	OR	274
#define	LAMBDA	275
#define	TRY	276
#define	TRAP	277
#define	THROW	278
#define	USING	279
#define	ASSERT	280
#define	TRUE	281
#define	FALSE	282
#define	BREAK	283
#define	CONTINUE	284
#define	STATIC	285
#define	CONST	286
#define	METHOD	287
#define	SELF	288
#define	ARGUMENTS	289
#define	ASSIGN	290
#define	ADD_A	291
#define	SUB_A	292
#define	MUL_A	293
#define	DIV_A	294
#define	MOD_A	295
#define	DOUBLE_LB	296
#define	DOUBLE_RB	297
#define	SEMI	298
#define	UMINUS	299
#define	MINUSMINUS	300
#define	LT	301
#define	GT	302
#define	LE	303
#define	GE	304
#define	EQ	305
#define	NE	306
#define	DOT	307
#define	DOUBLE_DOT	308
#define	TRIPLE_DOT	309
#define	DOT_ASSIGN	310
#define	GLOBAL_SCOPE	311
#define	ADD	312
#define	SUB	313
#define	MUL	314
#define	DIV	315
#define	MOD	316
#define	ATTRIBUTE	317
#define	STRINGIFY	318
#define	PLUSPLUS	319
#define	PARENTHESIS	320
#define	SQUARE_BRACKETS	321
#define	CALL	322
#define	CAST	323
#define	DOT_EQUAL	324
#define	DOT_CAST	325
#define	DOT_EQUAL_RHS	326
#define	ADD_POSTFIX	327
#define	SUB_POSTFIX	328
#define	MUL_POSTFIX	329
#define	DIV_POSTFIX	330
#define	MOD_POSTFIX	331
#define	GT_POSTFIX	332
#define	LT_POSTFIX	333
#define	NE_POSTFIX	334
#define	EQ_POSTFIX	335
#define	GE_POSTFIX	336
#define	LE_POSTFIX	337
#define	OPERATOR	338
#define	NEWSELF	339
#define	SET	340
#define	GET	341
#define	LAMBDA_REF	342


extern YYSTYPE yylval;

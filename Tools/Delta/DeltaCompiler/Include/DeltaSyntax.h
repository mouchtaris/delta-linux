typedef union {
	double				numberConst;
	util_ui32			line;
	char*				dynamicStr;
	const char*			constStr;
	AST::IdList*		idList;
	AST::NodeList*		nodeList;
	AST::Node*			node;
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
#define	USING	272
#define	AND	273
#define	NOT	274
#define	OR	275
#define	PLUSPLUS	276
#define	MINUSMINUS	277
#define	LAMBDA	278
#define	TRY	279
#define	TRAP	280
#define	THROW	281
#define	ASSERT	282
#define	TRUE	283
#define	FALSE	284
#define	BREAK	285
#define	CONTINUE	286
#define	STATIC	287
#define	CONST	288
#define	METHOD	289
#define	SELF	290
#define	ARGUMENTS	291
#define	LAMBDA_REF	292
#define	OPERATOR	293
#define	NEWSELF	294
#define	SET	295
#define	GET	296
#define	LT	297
#define	GT	298
#define	LE	299
#define	GE	300
#define	EQ	301
#define	NE	302
#define	DOT	303
#define	DOUBLE_DOT	304
#define	TRIPLE_DOT	305
#define	DOT_ASSIGN	306
#define	DOT_EQUAL	307
#define	DOT_CAST	308
#define	DOT_EQUAL_RHS	309
#define	UMINUS	310
#define	ADD	311
#define	SUB	312
#define	MUL	313
#define	DIV	314
#define	MOD	315
#define	ATTRIBUTE	316
#define	STRINGIFY	317
#define	GLOBAL_SCOPE	318
#define	ASSIGN	319
#define	ADD_A	320
#define	SUB_A	321
#define	MUL_A	322
#define	DIV_A	323
#define	MOD_A	324
#define	DOUBLE_LB	325
#define	DOUBLE_RB	326
#define	CALL	327
#define	CAST	328
#define	SEMI	329
#define	ADD_POSTFIX	330
#define	SUB_POSTFIX	331
#define	MUL_POSTFIX	332
#define	DIV_POSTFIX	333
#define	MOD_POSTFIX	334
#define	GT_POSTFIX	335
#define	LT_POSTFIX	336
#define	NE_POSTFIX	337
#define	EQ_POSTFIX	338
#define	GE_POSTFIX	339
#define	LE_POSTFIX	340
#define	PARENTHESIS	341
#define	SQUARE_BRACKETS	342


extern YYSTYPE yylval;

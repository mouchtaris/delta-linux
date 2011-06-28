typedef union {
	const char*			id;				// Const, internal static string.
	char*				copiedId;		// Copied dynamic, to be deleted by holder.
	double				numberConst;
	char*				strConst;
	bool				boolConst;
	ResourceValue		rcVal;
} YYSTYPE;
#define	IDENT	258
#define	NUMBER_CONST	259
#define	UNSIGNED_CONST	260
#define	STRING_CONST	261
#define	TRUE	262
#define	FALSE	263
#define	UMINUS	264
#define	SINGLETON	265
#define	DIRECTIVE	266


extern YYSTYPE yylval;

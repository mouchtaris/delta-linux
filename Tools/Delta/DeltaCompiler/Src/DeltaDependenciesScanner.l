%{
// Lexical analyser of the Delta language (only for dependencies).
// ScriptFighter Project.
// A. Savidis, October 2010.
//

#include "DDebug.h"
#include "ulexutil.h"
#include "DeltaDependenciesParser.h"
#include "BuildDependencies.h"

#define	YY_SKIP_YYWRAP
#define YY_USE_PROTOS
#define	YY_NEVER_INTERACTIVE 1

// The yylex() must be re-entrant, so it takes
// yylval as a formal parameter.

#define YY_DECL		int DeltaDependencies_yylex (void* yylval)
#define	STACKVAL	((YYSTYPE*) yylval)
#define	yywrap		DeltaDependencies_yywrap

// Lexical analysis helpers.

static void					IgnoreCStyleComments (void);
static void					IgnoreCPPStyleComments (void);
static const std::string	ReadQuotedString (void);

extern int DeltaDependencies_yywrap(void);

%}

ws				[ \t\r\n]
letter			[a-zA-Z]
decdigit		[0-9]
underscore		[_]
hexdigit		[0-9a-fA-F]
ident			{letter}({letter}|{decdigit}|{underscore})*
flag			{ws}+{decdigit}
cpp				#{ws}+{decdigit}+{ws}+\"[^"]+{flag}?

%%

using			return	USING;
"::"			return	DeltaBuildDependencies::InUsing() ? GLOBAL_SCOPE	: OTHER;
":"				return	DeltaBuildDependencies::InUsing() ? COLON			: OTHER;
";"				return	DeltaBuildDependencies::InUsing() ? SEMI			: OTHER;
"#"				return	DeltaBuildDependencies::InUsing() ? STRINGIFY		: OTHER;
{ws}*			{}
{ident}			{
					STACKVAL->id = usaveidstr(yytext); return 
					DeltaBuildDependencies::InUsing() ? IDENT : OTHER;
				}
{cpp}			while (yyinput() != '\n'){} /* Read until end of line. */
\"				{	
					STACKVAL->str = DeltaBuildDependencies::NewString(ReadQuotedString());
					return DeltaBuildDependencies::InUsing() ? STRING_CONST : OTHER;
				}
\/\* 			IgnoreCStyleComments();
\/\/			IgnoreCPPStyleComments();
.				return OTHER;

%%

//------------------------------------------------------------------

int		DeltaDependencies_yywrap (void)			{ return 1; }
void	DeltaDependencies_ResetLex (FILE* fp)	{ yyrestart(fp); }

static char input_wrapper (void)	{ return yyinput(); }
static void unput_wrapper (char c)	{ unput(c); }

static void IgnoreCStyleComments (void)  {
	util_ui32 line;
	uignoreCcomments(input_wrapper, unput_wrapper, &line, &DeltaBuildDependencies::SetError);
}

static void IgnoreCPPStyleComments (void) {
	util_ui32 line;
	uignoreCPPcomments(input_wrapper, &line);
}

static const std::string ReadQuotedString (void) {
	util_ui32 line;
	std::string s;
	ureadquotedstring(s, input_wrapper, &line, &DeltaBuildDependencies::SetError);
	return uextendescapesequences(s.c_str());
}

//------------------------------------------------------------------

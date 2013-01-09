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

#define YY_DECL		int yyFlexLexer::yylex (YYSTYPE* yylval, YYLTYPE* yylloc)
#define BUILDDEPS	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*GetDirectory(), DeltaBuildDependencies)))

%}

ws				[ \t\r\n]
letter			[a-zA-Z]
decdigit		[0-9]
underscore		[_]
hexdigit		[0-9a-fA-F]
ident			{letter}({letter}|{decdigit}|{underscore})*
flag			{ws}+{decdigit}
cpp				#{ws}+{decdigit}+{ws}+\"[^"]+{flag}?

%option c++
%option prefix="DeltaDependencies"
%option outfile="DeltaDependenciesScanner.cpp"
%option verbose
%option noyywrap

%%

using			return	USING;
"::"			return	BUILDDEPS.InUsing() ? GLOBAL_SCOPE	: OTHER;
":"				return	BUILDDEPS.InUsing() ? COLON			: OTHER;
";"				return	BUILDDEPS.InUsing() ? SEMI			: OTHER;
"#"				return	BUILDDEPS.InUsing() ? STRINGIFY		: OTHER;
{ws}*			{}
{ident}			{
					yylval->id = usaveidstr(yytext, &yy_id_buffer); return 
					BUILDDEPS.InUsing() ? IDENT : OTHER;
				}
{cpp}			while (yyinput() != '\n'){} /* Read until end of line. */
\"				{	
					yylval->str = BUILDDEPS.NewString(ReadQuotedString());
					return BUILDDEPS.InUsing() ? STRING_CONST : OTHER;
				}
\/\* 			IgnoreCStyleComments();
\/\/			IgnoreCPPStyleComments();
.				return OTHER;

%%

//------------------------------------------------------------------

int DeltaDependencies_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer)
	{ return lexer.yylex(yylval, yylloc); }

char yyFlexLexer::yyinput_wrapper(void* closure) { return ((yyFlexLexer*) closure)->yyinput(); }

void yyFlexLexer::yyunput_wrapper(char c, void *closure) {
	yyFlexLexer* lexer = (yyFlexLexer*) closure;
	lexer->yyunput(c, lexer->yytext_ptr);
}

void yyFlexLexer::IgnoreCStyleComments (void)  {
	util_ui32 line;	//unused
	std::string error;
	bool result = uignoreCcomments(
		umakecallback(yyinput_wrapper, this),
		umakecallback(yyunput_wrapper, this),
		&line,
		&error
	);
	if (!result)
		BUILDDEPS.SetError(error.c_str());
} 

void yyFlexLexer::IgnoreCPPStyleComments (void)
	{ uignoreCPPcomments(umakecallback(yyinput_wrapper, this)); }

const std::string yyFlexLexer::ReadQuotedString (void) {
	util_ui32 line; //unused
	std::string s;
	std::string error;
	if (!ureadquotedstring(s, umakecallback(yyinput_wrapper, this), &line, &error))
		BUILDDEPS.SetError(error.c_str());
	return uextendescapesequences(s.c_str());
}


//------------------------------------------------------------------

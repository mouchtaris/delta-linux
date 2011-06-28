%{
// Lexical analyser for the ResourceLoader.
// ScriptFighter Project.
// A. Savidis, Start January 2000.
//

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>

#include "DeltaByteCodeTypes.h"
#include "RcParseActions.h"
#include "RcParser.h"
#include "ulexutil.h"
#include "ustrings.h"
#include "RcLoaderErrorMsg.h"
#include "RcLoaderAPI.h"
#include "utypes.h"

using namespace RcLoader;

//////////////////////////////////////////////////////////////

#define	YY_SKIP_YYWRAP
#define YY_USE_PROTOS
#define	YY_NEVER_INTERACTIVE 1

static void* bufferState = (void*) 0;

void RcLoaderInitialiseScannerFromString (const char* str)
	{ bufferState = ResourceLoader_yy_scan_string(str); }

void RcLoaderCleanUpScannerFromString (void) {
	DASSERT(bufferState);
	yy_flush_buffer((YY_BUFFER_STATE) bufferState);
	yy_delete_buffer((YY_BUFFER_STATE) bufferState);
	bufferState = (void*) 0;
}
	
// The yylex() must be reentrant, so it takes yylval as a formal parameter.
//
#define YY_DECL		int ResourceLoader_yylex (void* yylval)
#define	yywrap		ResourceLoader_yywrap
#define	STACKVAL	((YYSTYPE*) yylval)

//////////////////////////////////////////////////////////////
// Lexical analysis helpers.

static void					IgnoreCStyleComments (void);
static void					IgnoreCPPStyleComments (void);
static const std::string	ReadQuotedString (void);

extern int ResourceLoader_yywrap(void);

%}

line		[\n]
ws			[ \t\r]
letter		[a-zA-Z]
decdigit	[0-9]
underscore	[_]
hexdigit	[0-9a-fA-F]
ident		{letter}({letter}|{decdigit}|{underscore})*
real		{decdigit}+"."{decdigit}+
hex			0x{hexdigit}+
flag		{ws}+{decdigit}
cpp			#{ws}+{decdigit}+{ws}+\"[^"]+{flag}?

%%

[Tt][Rr][Uu][Ee]		return	TRUE;
[Ff][Aa][Ll][Ss][Ee]	return	FALSE;
"-"						return	UMINUS;

{ws}*			;
{line}			{ ResourceLoader::NextLine(); }

"!"				return SINGLETON;
"@"				return DIRECTIVE;

{decdigit}+		{	STACKVAL->numberConst=ustrdectodouble(yytext);
					return UNSIGNED_CONST;
				}

{ident}			{	STACKVAL->id = usaveidstr(yytext);
					return IDENT;
				}

{real}			{	ucastassign(STACKVAL->numberConst, ustrfracttodouble(yytext));
					return NUMBER_CONST;
				}

{hex}			{	ucastassign(STACKVAL->numberConst, ustrtohex(yytext + 2));
					return NUMBER_CONST;
				}

{cpp}			{	 ResourceLoader::ParseLineDirective(yytext);
					while (yyinput() != '\n'){} // Read until end of line.
				}

\"				{	STACKVAL->strConst = StringLateDestroy(ucopystr(ReadQuotedString()));
					return STRING_CONST;
				}
				
\/\* 			{	IgnoreCStyleComments(); }

\/\/			{	IgnoreCPPStyleComments(); }

.				return yytext[0];

%%

//------------------------------------------------------------------

int					ResourceLoader_yywrap (void)	{ return 1; }
static char			input_wrapper (void)			{ return yyinput(); }
static void			unput_wrapper (char c)			{ unput(c); }
const std::string	ResourceLoader_GetText (void)	{ return yytext; }

static void IgnoreCStyleComments (void)  {
	util_ui32 line = ResourceLoader::GetLine();
	uignoreCcomments(input_wrapper, unput_wrapper, &line, &ResourceLoaderError);
	ResourceLoader::SetLine(line);
} 

static const std::string ReadQuotedString (void) {
	util_ui32 line = ResourceLoader::GetLine();
	std::string s;
	ureadquotedstring(s, input_wrapper, &line, &ResourceLoaderError);
	ResourceLoader::SetLine(line);
	return uextendescapesequences(s.c_str());
}

static void IgnoreCPPStyleComments (void) {
	util_ui32 line = ResourceLoader::GetLine();
	uignoreCPPcomments(input_wrapper, &line);
	ResourceLoader::SetLine(line);
}

void ResourceLoader_ResetLex (FILE* fp) 
	{ yyrestart(fp); }

//------------------------------------------------------------------

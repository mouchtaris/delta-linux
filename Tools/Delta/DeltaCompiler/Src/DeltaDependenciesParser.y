%{
// Delta parser for dependencies.
// Parses only using directives.
// ScriptFigher Project.
// A. Savidis, October 2010.
//

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <string>

#include "DDebug.h"
#include "BuildDependencies.h"

#include "DeltaDependenciesScanner.h"
#include "DeltaDependenciesParser.h"

#define yyFlexLexer DeltaDependenciesFlexLexer

#ifndef	alloca
#define alloca malloc
#endif

#define YYINCLUDED_STDLIB_H

#define BUILDDEPS	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*lexer.GetDirectory(), DeltaBuildDependencies)))

extern int DeltaDependencies_yylex (YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer);

static void DeltaDependencies_yyerror (YYLTYPE* yylloc, yyFlexLexer& lexer, const char* error)
	{ BUILDDEPS.SetError(error); }
	
%}

%union { const std::string* str; const char* id; }
%start	DeltaCode
%token	<id>		IDENT
%token	<str>		STRING_CONST
%token	USING GLOBAL_SCOPE COLON SEMI STRINGIFY OTHER

%output="DeltaDependenciesParser.cpp"
%name-prefix="DeltaDependencies_yy"
%debug
%defines
%verbose
%pure-parser
%parse-param {yyFlexLexer& lexer}
%lex-param   {YYSTYPE* yylval, YYLTYPE* yylloc, yyFlexLexer& lexer}
%locations
%expect 1
%%

/**************************************************************************/
/* PROGRAM*/

DeltaCode:				/* Empty code allowed. */ |	CodeDefs
						;

CodeDefs:				CodeDefs CodeDef | CodeDef
						;
						
CodeDef:				UsingDirective | Other
						;
						
UsingDirective:			USING 
							{ BUILDDEPS.EnterUsing(); }
						UsingSpecifications
							{ BUILDDEPS.ExitUsing(); }
						;

UsingSpecifications:	UsingNamespace | UsingByteCodeLibrary
						;
				
UsingNamespace:			NamespacePath IDENT SEMI | IDENT SEMI
						;

UsingByteCodeLibrary:	STRINGIFY IDENT SEMI
								{ BUILDDEPS.Handle($2); }
						;
						
Other					: OTHER { return 1; /* stop parsing*/ } | Other OTHER
						;

NamespacePath:				GLOBAL_SCOPE
						|	IDENT GLOBAL_SCOPE
						|	NamespacePath IDENT GLOBAL_SCOPE
						;
										
/**************************************************************************/

%%
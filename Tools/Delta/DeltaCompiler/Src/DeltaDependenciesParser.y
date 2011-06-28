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

#ifndef	alloca
#define alloca malloc
#endif

#ifdef	YYPURE
#define	YYLEX_PARAM		&yylval
#else
#define	YYPURE
#endif

#define	yyparse		DeltaDependencies_yyparse
#define yylex		DeltaDependencies_yylex
#define	yydebug		DeltaDependencies_yydebug
#define	yyerror		DeltaDependencies_yyerror

extern int			DeltaDependencies_yylex (void* yylval);

static void DeltaDependencies_yyerror (const char* error)
	{ DeltaBuildDependencies::SetError(error); }
%}

%union { const std::string* str; const char* id; }
%start	DeltaCode
%token	<id>		IDENT
%token	<str>		STRING_CONST
%token	USING GLOBAL_SCOPE COLON IDENT STRING_CONST SEMI STRINGIFY OTHER

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
							{ DeltaBuildDependencies::EnterUsing(); }
						UsingSpecifications
							{ DeltaBuildDependencies::ExitUsing(); }
						;

UsingSpecifications:	UsingNamespace | UsingByteCodeLibrary
						;
				
UsingNamespace:			NamespacePath IDENT SEMI | IDENT SEMI
						;

UsingByteCodeLibrary:	STRINGIFY IDENT SEMI
								{ DeltaBuildDependencies::Handle($2); }
						;
						
Other					: OTHER { return 1; /* stop parsing*/ } | Other OTHER
						;

NamespacePath:				GLOBAL_SCOPE
						|	IDENT GLOBAL_SCOPE
						|	NamespacePath IDENT GLOBAL_SCOPE
						;
										
/**************************************************************************/

%%
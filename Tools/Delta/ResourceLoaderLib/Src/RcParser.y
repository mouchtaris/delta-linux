%{

// ResourceLoader parser using Bison parser generator.
// ScriptFighter Project.
// A. Savidis, Start January 2000.
// Revised July 2009, refactored the grammar and supported explicit string 
// and numeric indices.
//
#include <string.h>
#include <stdio.h>

#include "DDebug.h"
#include "RcParseActions.h"
#include "ulexutil.h"
#include "ustrings.h"
#include "RcLoaderErrorMsg.h"

using namespace RcLoader;

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

extern int ResourceLoader_yylex (void* yylval);
extern unsigned long ResourceLoader_yylineno;
#define	yyparse	ResourceLoader_yyparse
#define yylex	ResourceLoader_yylex
#define	yydebug	ResourceLoader_yydebug

extern const std::string ResourceLoader_GetText (void);

static void yyerror (const char* s) {
	ResourceLoaderError(
		uconstructstr("%s before token %s", s, ResourceLoader_GetText().c_str()).c_str()
	); 
}

%}

%union {
	const char*			id;				// Const, internal static string.
	char*				copiedId;		// Copied dynamic, to be deleted by holder.
	double				numberConst;
	char*				strConst;
	bool				boolConst;
	ResourceValue		rcVal;
}

%start	ResourceItems
%type	<strConst>		StringConst
%type	<boolConst>		BoolConst
%type	<rcVal>			Literal  ResourceItems  ResourceValues ResourceValue
%type	<rcVal>			IndexedResource UnindexedResource Resource
%type	<copiedId>		SavedId ResourceIndex
%type	<numberConst>	Number

%token	<id>			IDENT
%token	<numberConst>	NUMBER_CONST UNSIGNED_CONST
%token	<strConst>		STRING_CONST
%token	TRUE FALSE UMINUS SINGLETON DIRECTIVE
%token	'[' ']' ',' ':'

%%

ResourceItems:		ResourceItems IndexedResource
						{ Manage_ResourceItems($2); }
				|	IndexedResource
			   			{ Manage_ResourceItems($1);  }
				;

SavedId:		IDENT	{ $$ = StringLateDestroy(ucopystr(usaveidstr($1))); }
				;

ResourceIndex:		SavedId				{ $$ = $1; }
				|	StringConst	':'		{ $$ = $1; }
				;
				
IndexedResource:	ResourceIndex UnindexedResource
						{ Manage_ResourceItem($$, $1, $2); }
				|	UNSIGNED_CONST ':' UnindexedResource 
						{ Manage_NumericIndexResource($$, $1, $3); }
				|	SINGLETON SavedId ResourceValue
						{ ResourceValue pp; Manage_ResourceItem(pp, $2, $3); Manage_SingletonResource($$, pp); }
				|	DIRECTIVE SavedId	
						{ Manage_DirectiveResource($$, $2); }
				;

UnindexedResource:	Literal
						{ $$.Assign($1); }
				|	'[' ResourceValues ']'
						{ Manage_ArrayOfResourceValues($$, $2); }
				|	'[' ']'
						{ Manage_EmptyResources($$); }
				
				;

ResourceValue	:	IndexedResource				{ $$.Assign($1); }
				|	UnindexedResource			{ $$.Assign($1); }
				;
				
Resource:			ResourceValue				{ $$.Assign($1); }
				|	DIRECTIVE UNSIGNED_CONST	{ Manage_SetAutoNumIndexStartValue($$, (util_ui32) $2); }
				;
			
ResourceValues:		ResourceValues ',' Resource
						{ Manage_ResourceValues($$, $1, $3); }
				|	Resource
						{ Manage_ResourceValue($$, $1); }
				;

Number:				NUMBER_CONST		{ $$ = $1; }
				|	UNSIGNED_CONST		{ $$ = $1; }
				;
			
Literal:			StringConst
						{ Manage_Literal($$, $1); }
				|	Number
						{ Manage_Literal($$, $1); }
				|	UMINUS Number
						{ Manage_Literal($$, -$2); }
				|	BoolConst
						{ Manage_Literal($$, $1); }
				;

BoolConst		:	TRUE				{ $$=true; }
				|	FALSE				{ $$=false; }
				;

StringConst:		StringConst STRING_CONST
						{ $$ = Manage_StringConst ($1, $2); }
				|	STRING_CONST
						{ $$ = $1; }
				;
%%
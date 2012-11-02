%{
#define YY_NEVER_INTERACTIVE 1

//Remove automatically generated file warnings
#ifdef  _MSC_VER
#pragma warning(disable:4065)
#endif

#include <stdio.h>
#include <stdlib.h>

#include "ManageActions.h"
#include "util.h"

using namespace dmsl;

#include "ProfileBisonParser.h"	//must be included after ManageActions
	
void yyerror (Profile *profile, PairMap**, const char *msg) {
	extern int Profile_yylineno;
	extern char *Profile_yytext;
	profile->SetError(util::MakeString("Profile: Line %d : %s: before token: %s\n", Profile_yylineno, msg, Profile_yytext));
}

extern int yylex (YYSTYPE* lvalp);

#define ABORT_ON_ERROR(x)	do { if(!x) YYABORT; } while(false)
%}

%output = "Src/ProfileBisonParser.cpp"
%name-prefix = "Profile_yy"
%pure-parser
%debug
%parse-param {Profile *profile}
%parse-param {PairMap **attributeValueMap}
%start logic

%token	'=' '.' ',' '{' '}' DOUBLEDOT PROFILE_ERROR

%left	'.'

%union
{
  int 				intVal;
  bool 				boolVal;
  double 			realVal;
  const char*		stringVal;
  Pair*				pair;
  PairMap*			pairMap;
  ExprValue*		value;
  ExprValueList*	valueList;
  Range*			range;
  RangeList*		rangeList;
}

%token	<intVal> 	INT_VALUE
%token	<realVal>	REAL_VALUE
%token	<stringVal>	STRING_VALUE
%token	<boolVal> 	BOOL_VALUE

%type	<realVal>	number
%type	<stringVal>	attribute
%type	<pair>		decl
%type	<pairMap>	declList
%type	<value>		value
%type	<valueList>	valueList valueListNonEmpty
%type	<range>		range
%type	<rangeList>	rangeList rangeListNonEmpty

%destructor { delete[] $$; } STRING_VALUE attribute

%%

logic				:	declList	{ Manage_logic_declList	(profile, attributeValueMap, $1);	}
					|	/*empty*/	{ Manage_logic_empty	(profile, attributeValueMap);		}
					;

declList			:	declList decl	{ Manage_declList_declList	(profile, &$$, $1, $2);	ABORT_ON_ERROR($$); }
					|	decl			{ Manage_declList_decl		(profile, &$$, $1);		ABORT_ON_ERROR($$); }
					;

decl				:	attribute '=' value	{ Manage_decl(profile, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					;

attribute			:	attribute '.' attribute	{ Manage_attribute_attribute(profile, &$$, $1, $3);	}
					|	STRING_VALUE			{ Manage_attribute_string	(profile, &$$, $1);		}
					;

value				:	BOOL_VALUE			{ Manage_value_bool		(profile, &$$, $1);	ABORT_ON_ERROR($$); }
					|	STRING_VALUE		{ Manage_value_string	(profile, &$$, $1);	ABORT_ON_ERROR($$); }
					|	number				{ Manage_value_number	(profile, &$$, $1);	ABORT_ON_ERROR($$); }
					|	range				{ Manage_value_range	(profile, &$$, $1);	ABORT_ON_ERROR($$); }
					|	'{' valueList '}'	{ Manage_value_valueList(profile, &$$, $2);	ABORT_ON_ERROR($$); }
					|	'[' rangeList ']'	{ Manage_value_rangeList(profile, &$$, $2);	ABORT_ON_ERROR($$); }
					;

number				:	INT_VALUE			{ Manage_number_int		(profile, &$$, $1);	}
					|	REAL_VALUE			{ Manage_number_real	(profile, &$$, $1);	}
					;

range				:	number DOUBLEDOT number { Manage_range(profile, &$$, $1, $3); ABORT_ON_ERROR($$); }
					;

valueList			:	valueListNonEmpty	{ Manage_valueList_valueListNonEmpty(profile, &$$, $1);	ABORT_ON_ERROR($$); }
					|	/*empty*/			{ Manage_valueList_empty			(profile, &$$);		ABORT_ON_ERROR($$); }
					;

valueListNonEmpty	:	valueListNonEmpty ',' value	{ Manage_valueListNonEmpty_valueListNonEmpty(profile, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	value						{ Manage_valueListNonEmpty_value			(profile, &$$, $1);		ABORT_ON_ERROR($$); }
					;

rangeList			:	rangeListNonEmpty	{ Manage_rangeList_rangeListNonEmpty(profile, &$$, $1);	ABORT_ON_ERROR($$); }
					|	/*empty*/			{ Manage_rangeList_empty			(profile, &$$);		ABORT_ON_ERROR($$); }
					;

rangeListNonEmpty	:	rangeListNonEmpty ',' range	{ Manage_rangeListNonEmpty_rangeListNonEmpty(profile, &$$, $1, $3);	ABORT_ON_ERROR($$); }
					|	range						{ Manage_rangeListNonEmpty_range			(profile, &$$, $1);		ABORT_ON_ERROR($$); }
					;
					
%%


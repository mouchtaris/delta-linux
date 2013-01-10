%{
	#include "JsonParseActions.h"
	#include "JsonLoaderErrorMsg.h"
    #include "JsonParser.h"
	#include "DeltaObject.h"
	#include "DeltaValue.h"

	using namespace JsonParserLoaderActions;

	#define yylex JsonLex_yylex
	#define	yyparse	JsonLex_yyparse
	#define yytext JsonLex_yytext

	extern char *yytext;
	extern int yylex();
	extern int yylineno;

	int yyerror (DeltaObject **return_value, bool retainNull, std::string yaccProvidedMessage);

%}

%parse-param {DeltaObject **return_value}
%parse-param {bool retainNull}

%union {  
  std::string*	stringValue;
  double		numberValue;
  DeltaValue*	deltaValue;
  DeltaObject*	deltaObject;
}

%start document

%type <deltaValue> number value
%type <deltaObject> object elements array pair members 

%token <stringValue>	STRING
%token <numberValue> 	INTEGER FLOAT
%token <stringValue> 	EXPINTEGER EXPFLOAT

%token BLOCK_L BLOCK_R BRACKET_L BRACKET_R COMMA
%token DBLDOT TRUE FALSE NIL QUOTE

%%

document:	object 
				{ *return_value = $1; }
			;
		
object:		BLOCK_L BLOCK_R 
				{ $$ = manage_objectEmpty(); }	  
		|	BLOCK_L members BLOCK_R 
				{ $$ = $2;}
		;
	
members:	pair 
				{ $$ = $1;}
		|	pair COMMA members 
				{ $$ = manage_membersPairs($1, $3); }
		;
	 
pair:		STRING DBLDOT value 
				{ $$ = manage_pair($1, $3);	}
		;
	
array:		BRACKET_L BRACKET_R 
				{ $$ = manage_emptyArray();	}
		|	BRACKET_L elements BRACKET_R 
				{ $$ = $2; }
		;
	
elements:	value 
				{ $$ = manage_elementsValue($1); }
		|	value COMMA elements 
				{ $$ = manage_elementsValues($1, $3); }
		;
		
value:		STRING
				{ $$ = manage_valueString($1); }
		|	number 
				{  $$ = $1; }
		|	object 
				{ $$ = manage_valueObject($1); } 
		|	array 
				{ $$ = manage_valueArray($1); }
		|	TRUE 
				{ $$ = manage_valueTrue(); }
		|	FALSE 
				{ $$ = manage_valueFalse(); } 
		|	NIL 
				{ $$ = manage_valueNil(retainNull); }
		;
		
number:		INTEGER 
				{ $$ = manage_numberInteger($1); }  
		|	FLOAT 
				{ $$ = manage_numberFloat($1); }	  
		|	EXPINTEGER 
				{ $$ = manage_numberExpInteger($1); }	      
		|	EXPFLOAT 
				{ $$ = manage_numberExpFloat($1); }
		;
	
%%

int yyerror (DeltaObject **return_value, bool retainNull, std::string yaccProvidedMessage)
{
	JsonParserLoaderErrorMsg::JsonLoaderError("%s before token %s", yaccProvidedMessage.c_str(), JsonLex_yytext);
	return -1;
}


		

	 

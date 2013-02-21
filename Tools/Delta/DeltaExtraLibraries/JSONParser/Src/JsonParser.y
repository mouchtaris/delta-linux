%{

// Json parser for Delta, syntax directed parsing file, 
// using Bison parser generator. I collect all the variables 
// that i reserve memory for better performance 
// and for avoiding memory leaks in case of syntax error
// Giannhs Apostolidhs, january 2013 (original full version).
//

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

int yyerror (DeltaObject **returnValue, bool retainNull, std::string yaccProvidedMessage);

%}

%parse-param {DeltaObject **returnValue}
%parse-param {bool retainNull}

%union {  
  const char*	stringValue;
  double		numberValue;
  DeltaValue*	deltaValue;
  DeltaObject*	deltaObject;
}

%start document

%type <deltaValue> number value
%type <deltaObject> object array 

%token <stringValue>	STRING EXPINTEGER EXPFLOAT
%token <numberValue> 	INTEGER FLOAT

%token BLOCK_L BLOCK_R BRACKET_L BRACKET_R COMMA
%token COLON TRUE FALSE NIL

%%

document:	object 
				{ *returnValue = $1; }
			;
		
object:		BLOCK_L BLOCK_R 
				{ $$ = Manage_ObjectEmpty(); }	  
		|	BLOCK_L
				{ Manage_PushNewObject(); }
			members BLOCK_R 
				{ $$ = Manage_PopObject(); }
		;

members:	pair COMMA members
		|	pair 

		;
	 
pair:		STRING COLON
				{ Manage_PairIndex($1); }
			value 
				{ Manage_PairValue($4);	}
		;
	
array:		BRACKET_L BRACKET_R 
				{ $$ = Manage_EmptyArray();	}
		|	BRACKET_L 
				{ Manage_PushNewArray(); }
			elements BRACKET_R 
				{ $$ = Manage_PopArray(); }
		;

elementsValue:	value 
					{ Manage_ElementsValue($1); }
				;
	
elements:	elementsValue COMMA elements
		|	elementsValue
		;
		
value:		STRING
				{ $$ = Manage_ValueString($1); }
		|	number 
				{  $$ = $1; }
		|	object 
				{ $$ = Manage_ValueObject($1); } 
		|	array 
				{ $$ = Manage_ValueArray($1); }
		|	TRUE 
				{ $$ = Manage_ValueTrue(); }
		|	FALSE 
				{ $$ = Manage_ValueFalse(); } 
		|	NIL 
				{ $$ = Manage_ValueNil(retainNull); }
		;
		
number:		INTEGER 
				{ $$ = Manage_NumberInteger($1); }  
		|	FLOAT 
				{ $$ = Manage_NumberFloat($1); }	  
		|	EXPINTEGER 
				{ $$ = Manage_NumberExpInteger($1); }	      
		|	EXPFLOAT 
				{ $$ = Manage_NumberExpFloat($1); }
		;
	
%%

int yyerror (DeltaObject **returnValue, bool retainNull, std::string yaccProvidedMessage)
{
	JsonParserLoaderErrorMsg::JsonLoaderError("%s before token %s", yaccProvidedMessage.c_str(), JsonLex_yytext);
	return -1;
}


		

	 

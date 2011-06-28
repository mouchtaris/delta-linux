/*
 *	XML_Parser.y -- The bison file used to create the main parsing function yyparse().
 */

%{
	//Includes and function signatures in here
	#include <iostream>
	#include <string>
	#include <list>
	#include "DeltaObject.h"
	#include "DeltaValue.h"
	#include "XmlParseActions.h"
	#include "TagValue.h"
	#include "XmlElement.h"
	#include "XmlParser.h"
	#include "XmlLoaderErrorMsg.h"

	using namespace XmlLoaderActions;

	#define yylex XmlLex_yylex
	#define	yyparse	XmlLex_yyparse
	#define yytext XmlLex_yytext

	extern char *yytext;
	extern int yylex();
	int yyerror (DeltaObject **return_value, std::string yaccProvidedMessage);
%}

%parse-param {DeltaObject **return_value}

%union
{
	std::string*		strValue;
	DeltaValue*		obj;
	DeltaObject*		proto;
	TagValue*			tag;
	XmlElement*			element;
};

%start document

//Keywords 
%token <strValue> NAME ATTVALUE REFERENCE CDSECT CHARDATA ENTITYVALUE
%token LT GT LTSLASH SLASHGT PERCENT LTQMXML QMGT S PI EQ COMMENT
%token DOCTYPE EXTERNALID LBRACKET RBRACKET PEREFERENCE ELEMENTDECL EMPTY ANY
%token MIXED ATTLIST DEFAULTDECL STRINGTYPE TOKENIZEDTYPE NOTATIONTYPE ENUMERATION ENTITY
%token NDATADECL NOTATION PUBLICID VERSIONINFO ENCODINGDECL SDDECL

%type <strValue>	ETag EntityDef PEDef
%type <tag>			STag AttList EmptyElemTag
%type <proto>		content
%type <element>		element

%%

document:		prolog element MiscList									{
																			if ($2 == NULL) {
																				*return_value = (DeltaObject*) 0;
																			} else {
																				DeltaValue index, value;
																				index.FromString("$Name");
																				value.FromString(*($2->GetName()));
																				
																				*return_value = $2->GetContent();
																				DPTR((*return_value))->Set(index, value);

																				DDELETE($2);
																			}
																		}
				;

MiscList:		Misc MiscList											{ }
				|														{ }
				;

prolog:			XmlDeclQM MiscList prologDecl							{ }
				;

XmlDeclQM:		LTQMXML VERSIONINFO EncodingDeclQM SDDeclQM SQM QMGT	{ }
				|														{ }
				;

EncodingDeclQM:	ENCODINGDECL											{ }
				|														{ }
				;

SDDeclQM:		SDDECL													{ }
				|														{ }
				;

prologDecl:		doctypeDecl MiscList									{ }
				|														{ }
				;

doctypeDecl:	DOCTYPE S NAME ExternalIDQM IntSubsetQM GT				{ }

ExternalIDQM:	S EXTERNALID SQM										{ }
				|	SQM													{ }
				;

SQM:			S														{ }
				|														{ }
				;

IntSubsetQM:	LBRACKET IntSubset RBRACKET SQM							{ }
				|														{ }
				;

IntSubset:		IntSubset MarkupDecl									{ }
				|	IntSubset DeclSep									{ }
				|														{ }
				;

MarkupDecl:		ElementDecl												{ }
				|	AttlistDecl											{ }
				|	EntityDecl											{ }
				|	NotationDecl										{ }
				|	PI													{ }
				|	COMMENT												{ }
				;

ElementDecl:	ELEMENTDECL S NAME S ContentSpec SQM GT					{ }
				;

ContentSpec:	EMPTY													{ }
				|	ANY													{ }
				|	MIXED												{ }
				;	/*Missing CHILDREN*/

AttlistDecl:	ATTLIST S NAME AttDefs GT								{ }
				;

AttDefs:		AttDef AttDefs											{ }
				|	SQM													{ }
				;

AttDef:			S NAME S AttType S DEFAULTDECL							{ }
				;

AttType:		STRINGTYPE												{ }
				|	TOKENIZEDTYPE										{ }
				|	EnumeratedType										{ }
				;

EnumeratedType:	NOTATIONTYPE											{ }
				|	ENUMERATION											{ }
				;

EntityDecl:		GEDecl													{ }
				|	PEDecl												{ }
				;

GEDecl:			ENTITY S NAME S EntityDef SQM GT						{ }
				;

EntityDef:		ENTITYVALUE												{ }
				|	EXTERNALID NDATADECL								{ }
				|	EXTERNALID											{ }
				;

PEDecl:			ENTITY S PERCENT S NAME S PEDef SQM GT					{ }
				;

PEDef:			ENTITYVALUE												{ }
				|	EXTERNALID											{ }
				;

NotationDecl:	NOTATION S NAME S EXTERNALID SQM GT						{ }
				|	NOTATION S NAME S PUBLICID SQM GT					{ }
				;

DeclSep:		PEREFERENCE												{ }
				|	S													{ }
				;

Misc:			COMMENT													{ }
				|	PI													{ }
				|	S													{ }
				;

element:		STag content ETag										{
																			$$ = Manage_ElementWithContent($1, $2, $3);
																		}
				|	EmptyElemTag										{
																			$$ = Manage_ElementWithoutContent($1);
																		}
				;

STag:			LT NAME AttList GT										{
																			$$ = Manage_Stag($2, $3);
																		}
				;

AttList:		S NAME EQ ATTVALUE AttList								{
																			$$ = Manage_Attlist($2, $4, $5);
																		}
				| S														{
																			$$ = Manage_AttlistSpaces();
																		}
				|														{
																			$$ = Manage_AttlistNothing();
																		}
				;

ETag:			LTSLASH NAME GT											{
																			$$ = $2;
																		}
				;

content:		content element											{
																			$$ = Manage_ContentElement($1, $2);
																		}
				| content REFERENCE										{
																			$$ = Manage_ContentReference($1, $2);
																		}
				| content CDSECT										{
																			$$ = Manage_ContentCdsect($1, $2);
																		}
				| content PI											{
																			$$ = $1;
																		}
				| content COMMENT										{
																			$$ = $1;
																		}
				| content CHARDATA										{
																			$$ = Manage_ContentChardata($1, $2);
																		}
				| content S												{
																			$$ = $1;
																		}
				|														{
																			$$ = Manage_ContentNothing();
																		}
				| content error											{
																			if ($1 != NULL) {
																				DPTR($1)->DecRefCounter((DeltaValue*) 0);
																			}
																			$$ = (DeltaObject *)0;
																		}
				;

EmptyElemTag:	LT NAME AttList SLASHGT									{
																			$$ = Manage_EmptyElemTag($2, $3);
																		}
				;

%%

/*
 * Prints an error message in case of a syntax error.
 */
int yyerror (DeltaObject **return_value, std::string yaccProvidedMessage)
{
	XmlLoaderError("%s before token %s", yaccProvidedMessage.c_str(), XmlLex_yytext);
	return -1;
}

/*
 * XML_Lex.l -- The lex file used to create the main lexical analyzer
 */
 
%{
	#include <iostream>
	#include <string>
	#include "DeltaObject.h"
	#include "DeltaValue.h"
	#include "TagValue.h"
	#include "XmlElement.h"
	#include "XmlParser.h"
	#include "XmlLoaderAPI.h"
	#include "XmlLoaderErrorMsg.h"

	#define	YY_NEVER_INTERACTIVE 1

	static void* bufferState = (void*) 0;

	void XmlLoaderInitialiseScannerFromString (const char* str) {
		bufferState = yy_scan_string(str);
	}

	void XmlLoaderCleanUpScannerFromString (void) {
		DASSERT(bufferState);
		yy_flush_buffer((YY_BUFFER_STATE) bufferState);
		yy_delete_buffer((YY_BUFFER_STATE) bufferState);
		bufferState = (void*) 0;
	}

	static int finished_prolog = 0;
	static unsigned depth = 0;
	static int check_chardata(char *);
	static char *substring(char *, int, int);
%}
 
/* Flex options */
 
%option noyywrap
%option yylineno

/*Start Conditions*/

%x ELEMENT
%x DECL

/* Flex macros */

Char				[\x1-\xD7FF]|[\xE000-\xFFFD]|[\x10000-\x10FFFF]
RestrictedChar		[\x1-\x8]|[\xB-\xC]|[\xE-\x1F]|[\x7F-\x84]|[\x86-\x9F]
CharWithoutRBracket	[\x1-\x5C]|[\x5E-\xD7FF]
CharWithoutRChevron	[\x1-\x3D]|[\x3F-\xD7FF]

S					(\x20|\x9|\xD|\xA)+
NameStartChar		":"|[A-Z]|"_"|[a-z]
NameChar			{NameStartChar}|"-"|"."|[0-9]
Name				{NameStartChar}{NameChar}*
SystemLiteral		("\""[^\"]*"\"")|("'"[^\']*"'")
PubidChar			\x20|\xD|\xA|[a-zA-Z0-9]|[\-\(\)\+\,\.\/\:\=\?\;\!\*\#\@\$\_\%]
PubidLiteral		("\""({PubidChar}|"'")*"\"")|("'"{PubidChar}*"'")
EntityRef			&{Name};
CharRef				&#[0-9]+;|&#x[0-9a-fA-F]+;
Eq					{S}?"="{S}?
AttValue			("\""({EntityRef}|{CharRef}|[^\"&<])*"\"")|("'"({EntityRef}|{CharRef}|[^'&<])*"'")
Reference			{EntityRef}|{CharRef}
PEReference			%{Name};
EntityValue			("\""([^\%\&\"]|{PEReference}|{EntityRef}|{CharRef})*"\"")|("'"([^\%\&\']|{PEReference}|{EntityRef}|{CharRef})*"'")
CDStart				"<![CDATA["
CData				(({CharWithoutRBracket})|(\x5D{CharWithoutRBracket})|(\x5D\x5D{CharWithoutRChevron}))*
CDEnd				"]]>"
CDSect				{CDStart}{CData}{CDEnd}
PITarget			([Xx])|([A-WY-Za-wy-z:_]{NameChar}*)|([Xx][A-LN-Za-ln-z0-9:_\-.]{NameChar}*)|([Xx][Mm][A-KM-Za-km-z0-9:_\-.]{NameChar}*)|([Xx][Mm][Ll]{NameChar}+)
PIChar				((\x9|\xA|\xD|[\x20-\x3E]|[\x40-\x7F])|(\x3F(\x9|\xA|\xD|[\x20-\x3D]|[\x3F-\x7F])))*
PI					"<?"{PITarget}({S}{PIChar})?"?>"
VersionNum			"1."[0-9]+
VersionInfo			{S}"version"{Eq}("'"{VersionNum}"'"|"\""{VersionNum}"\"")
SDDecl				{S}"standalone"{Eq}(("'"("yes"|"no")"'")|("\""("yes"|"no")"\""))
Mixed				("("{S}?"#PCDATA"({S}?"|"{S}?{Name})*{S}?")*")|("("{S}?"#PCDATA"{S}?")")
StringType			"CDATA"
TokenizedType		"ID"|"IDREF"|"IDREFS"|"ENTITY"|"ENTITIES"|"NMTOKEN"|"NMTOKENS"
NotationType		"NOTATION"{S}"("{S}?{Name}({S}?"|"{S}?{Name})*{S}?")"
Enumeration			"("{S}?{NameChar}+({S}?"|"{S}?{NameChar}+)*{S}?")"
ExternalID			("SYSTEM"{S}{SystemLiteral})|("PUBLIC"{S}{PubidLiteral}{S}{SystemLiteral})
NDataDecl			{S}"NDATA"{S}{Name}
EncName				[A-Za-z]([A-Za-z0-9\.\_\-])*
EncodingDecl		{S}"encoding"{Eq}("\""{EncName}"\""|"'"{EncName}"'")
DefaultDecl			("#REQUIRED")|("#IMPLIED")|(("#FIXED"{S})?{AttValue})
PublicID			"PUBLIC"{S}{PubidLiteral}
CharData			([^<])+
/*Must not contain sub-string ']]>'. Check and if it does throw an error.*/

Comment				<!--([^-]|[-][^-])*-->
/*Two consecutive dashes (--) may not appear anywhere in the text of the comment. */

%%

<*>"<"						{
								finished_prolog = 1;
								++depth;
								BEGIN(INITIAL);
								return LT;
							}
<INITIAL,DECL>">"			{
								if(finished_prolog && depth != 0) {
									BEGIN(ELEMENT);
								} else if (!finished_prolog) {
									--depth;
									if (depth == 0) {
										BEGIN(INITIAL);
									}
								}
								return GT;
							}
<ELEMENT>"</"				{
								--depth;
								BEGIN(INITIAL);
								return LTSLASH;
							}
<INITIAL>"/>"				{
								--depth;
								if(depth != 0) {
									BEGIN(ELEMENT);
								}
								return SLASHGT;
							}
<INITIAL>"="				{ return EQ; }
<DECL>"["					{ return LBRACKET; }
<DECL>"]"					{ return RBRACKET; }
<DECL>"%"					{ return PERCENT; }
"<!DOCTYPE"					{ BEGIN(DECL); ++depth; return DOCTYPE; }
<DECL>"<!ELEMENT"			{ ++depth; return ELEMENTDECL; }
<DECL>"<!ATTLIST"			{ ++depth; return ATTLIST; }
<DECL>"<!ENTITY"			{ ++depth; return ENTITY; }
<DECL>"<!NOTATION"			{ ++depth; return NOTATION; }
"EMPTY"						{ return EMPTY; }
"ANY"						{ return ANY; }
"<?xml"						{ return LTQMXML; }
"?>"						{ return QMGT; }
<*>{S}						{ XmlLoaderAPI::SetLine(yylineno); return S; }
{StringType}				{ return STRINGTYPE; }
{TokenizedType}				{ return TOKENIZEDTYPE; }
<INITIAL,DECL>{Name}		{
								yylval.strValue = DNEWCLASS(std::string, (yytext));
								return NAME;
							}
<*>{Comment}				{ return COMMENT; }
<INITIAL>{AttValue}			{
								yytext = substring(yytext, 1, 1);
								yylval.strValue = DNEWCLASS(std::string, (yytext));
								return ATTVALUE;
							}
{PEReference}				{ return PEREFERENCE; }
<ELEMENT>{CDSect}			{
								yytext = substring(yytext, 9, 3);
								yylval.strValue = DNEWCLASS(std::string, (yytext));
								return CDSECT;
							}
<*>{PI}						{ return PI; }
{VersionInfo}				{ return VERSIONINFO; }
{SDDecl}					{ return SDDECL; }
{Mixed}						{ return MIXED; }
{NotationType}				{ return NOTATIONTYPE; }
{Enumeration}				{ return ENUMERATION; }
<*>{Reference}				{
								yytext = substring(yytext, 1, 1);
								yylval.strValue = DNEWCLASS(std::string, (yytext));//new std::string(yytext);
								return REFERENCE;
							}
{ExternalID}				{ return EXTERNALID; }
{NDataDecl}					{ return NDATADECL; }
{EncodingDecl}				{ return ENCODINGDECL; }
{DefaultDecl}				{ return DEFAULTDECL; }
<DECL>{EntityValue}			{
								yytext = substring(yytext, 1, 1);
								yylval.strValue = DNEWCLASS(std::string, (yytext));
								return ENTITYVALUE;
							}
{PublicID}					{ return PUBLICID; }
<ELEMENT>{CharData}			{
								if (check_chardata(yytext) != 0) {
									yylval.strValue = DNEWCLASS(std::string, (yytext));
									return CHARDATA;
								}
							}

.							{
								XmlLoaderError("Did not recognize token %s", yytext);
								return yytext[0];
							}
%%

int check_chardata(char *value)
{
	std::string str = std::string(value);
	if (str.length() < 3)
		return 1;
	for(unsigned int i = 0; i < str.length() - 2; ++i) {
		if (str.substr(i,3).compare("]]>") == 0) {
			XmlLoaderError("Error! %s has the substring ]]> in position %d\n", str.c_str(), i);
			return 0;
		}
	}
	return 1;
}

char *substring(char *value, int begin, int end)
{
	int length = strlen(value);
	value[length-end] = 0;
	return (value += begin);
}


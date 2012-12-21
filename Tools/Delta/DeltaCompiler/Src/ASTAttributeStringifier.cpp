// AST::AttributeStringifier.cpp
// Attribute stringifier for Delta AST.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "ASTAttributeStringifier.h"
#include "ASTCreationActions.h"
#include "ASTTags.h"
#include "ustrings.h"

///////////////////////////////////////////////////////////

#define	IF_HANDLE_LINE()									\
	if (attr == DELTA_AST_LINE_ATTRIBUTE		||			\
		attr == DELTA_AST_STARTLINE_ATTRIBUTE	||			\
		attr == DELTA_AST_ENDLINE_ATTRIBUTE		||			\
		attr == DELTA_AST_STARTPOS_ATTRIBUTE	||			\
		attr == DELTA_AST_ENDPOS_ATTRIBUTE					\
	) {			\
		if (!data)											\
			*result = "";									\
		else												\
			*result = unum2string(data->GetUInt());			\
	} else if (false)										\

#define	HANDLE_STRING(tag)									\
	if (attr == tag)										\
		*result =  data->GetString();						\
	else													\
		DASSERT(false)

#define	HANDLE_IDLIST(tag, sep)								\
	if (attr == tag)										\
		*result = Stringify(data->GetStringList(), sep);	\
	else													\
		DASSERT(false)

///////////////////////////////////////////////////////////

AST::AttributeStringifier::AttributeStringifier (void) {
	SetHandler(AST_TAG_NAME,						&Handle_Name);
	SetHandler(AST_TAG_FUNCTION,					&Handle_Function);
	SetHandler(AST_TAG_DOTINDEX_IDENT,				&Handle_DotIndexIdent);
	SetHandler(AST_TAG_DOTINDEX_OPSTRING,			&Handle_DotIndexOpString);
	SetHandler(AST_TAG_BRACKETINDEX_OPMETHOD,		&Handle_BracketIndexOperatorMethod);
	SetHandler(AST_TAG_TABLE_DOTTED_IDENT,			&Handle_DottedIdent);
	SetHandler(AST_TAG_TABLE_OPERATOR_INDEX,		&Handle_OperatorIndex);
	SetHandler(AST_TAG_USING_BYTECODE_LIBRARY,		&Handle_UsingByteCodeLibrary);
	SetHandler(AST_TAG_NUM_CONST,					&Handle_NumConst);
	SetHandler(AST_TAG_BOOL_CONST,					&Handle_BoolConst);
	SetHandler(AST_TAG_STRING_CONST,				&Handle_StringConst);
	SetHandler(AST_TAG_STRINGIFY_DOTTED_IDENTS,		&Handle_StringifyDottedIdents);
	SetDefaultHandler(&DefaultHandler);
}

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::DefaultHandler (ATTR_STRINGIFIER_ARGS) 
	{ IF_HANDLE_LINE(); }

///////////////////////////////////////////////////////////

static const std::string Stringify (
		const std::list<std::string>&	l, 
		const std::string&				separator
	) {
	std::string s;
	for (std::list<std::string>::const_iterator i = l.begin(); i != l.end(); ++i) {
		if (i != l.begin() && s != separator)
			s.append(separator);
		s.append(*i);
	}
	return s;
}

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::Handle_Name (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); }

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::Handle_Function (ATTR_STRINGIFIER_ARGS){ 
	IF_HANDLE_LINE();
	else
	if (attr == AST_ATTRIBUTE_CLASS)
		*result = (const char*) data;
	else
		DASSERT(false);
}

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::Handle_DotIndexIdent  (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_VALUE); }

void AST::AttributeStringifier::Handle_DotIndexOpString  (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_VALUE); }

void AST::AttributeStringifier::Handle_BracketIndexOperatorMethod  (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_VALUE); }

void AST::AttributeStringifier::Handle_DottedIdent (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); }

void AST::AttributeStringifier::Handle_OperatorIndex (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_VALUE); }

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::Handle_UsingByteCodeLibrary (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); }

void AST::AttributeStringifier::Handle_StringifyDottedIdents (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_IDLIST(AST_ATTRIBUTE_ITEMS, "."); }

void AST::AttributeStringifier::Handle_StringConst (ATTR_STRINGIFIER_ARGS) {
	IF_HANDLE_LINE();	
	else
	if (attr == AST_ATTRIBUTE_ITEMS)
		*result = std::string("\"") + Stringify(data->GetStringList(), "") + std::string("\"");
	else
		DASSERT(false);
}

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::Handle_NumConst (ATTR_STRINGIFIER_ARGS) {
	IF_HANDLE_LINE();	
	else 
	if (attr == AST_ATTRIBUTE_CONSTVALUE)
		*result = unum2string(DPTR(data)->GetDouble());
	else
		DASSERT(false);
}

void AST::AttributeStringifier::Handle_BoolConst (ATTR_STRINGIFIER_ARGS){
	IF_HANDLE_LINE();	
	else 
	if (attr == AST_ATTRIBUTE_CONSTVALUE) {
		*result = data->GetBool() ? "true" : "false";
	}
	else
		DASSERT(false);
}

///////////////////////////////////////////////////////////

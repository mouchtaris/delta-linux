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

#define	IF_HANDLE_LINE()											\
	if (attr == DELTA_AST_LINE_ATTRIBUTE ||							\
		attr == DELTA_AST_NEWLINE_ATTRIBUTE) {						\
		if (!data)													\
			*result = "";											\
		else														\
			*result = unum2string((util_ui32) data);				\
	} else if (false)												\

#define	HANDLE_STRING(tag)											\
	if (attr == tag)												\
		*result = (const char*) data;								\
	else															\
		DASSERT(false)

#define	HANDLE_IDLIST(tag, sep)										\
	if (attr == tag)												\
		*result = Stringify((AST::IdList*) data, sep);				\
	else															\
		DASSERT(false)

///////////////////////////////////////////////////////////

AST::AttributeStringifier::AttributeStringifier (void) {
	SetHandler(AST_TAG_FUNCTION,					&Handle_Function);
	SetHandler(AST_TAG_LVALUE_IDENT,				&Handle_Lvalue);
	SetHandler(AST_TAG_LVALUE_STATIC_IDENT,			&Handle_StaticLvalue);
	SetHandler(AST_TAG_LVALUE_LOCAL_IDENT,			&Handle_LocalLvalue);
	SetHandler(AST_TAG_LVALUE_ATTRIBUTE,			&Handle_AttrLvalue);
	SetHandler(AST_TAG_LVALUE_NAMESPACE_IDENT,		&Handle_NamespaceLvalue);
	SetHandler(AST_TAG_DOTINDEX_IDENT,				&Handle_DotIndexIdent);
	SetHandler(AST_TAG_DOTINDEX_OPSTRING,			&Handle_DotIndexOpString);
	SetHandler(AST_TAG_BRACKETINDEX_OPMETHOD,		&Handle_BracketIndexOperatorMethod);
	SetHandler(AST_TAG_TABLE_DOTTED_IDENT,			&Handle_DottedIdent);
	SetHandler(AST_TAG_TABLE_OPERATOR_INDEX,		&Handle_OperatorIndex);
	SetHandler(AST_TAG_EXCEPTION_VAR,				&Handle_ExceptionVar);
	SetHandler(AST_TAG_USING_NAMESPACE,				&Handle_UsingNamespace);
	SetHandler(AST_TAG_USING_BYTECODE_LIBRARY,		&Handle_UsingByteCodeLibrary);
	SetHandler(AST_TAG_STRINGIFY_NAMESPACE_IDENT,	&Handle_StringifyNamespaceIdent);
	SetHandler(AST_TAG_CONSTDEF,					&Handle_ConstDefExpression);
	SetHandler(AST_TAG_NUM_CONST,					&Handle_NumConst);
	SetHandler(AST_TAG_BOOL_CONST,					&Handle_BoolConst);
	SetHandler(AST_TAG_STRING_CONST,				&Handle_StringConst);
	SetHandler(AST_TAG_STRINGIFY_DOTTED_IDENTS,		&Handle_StringifyDottedIdents);
	SetHandler(AST_TAG_TABLE_NEW_ATTRIBUTE,			&Handle_NewAttribute);
	SetDefaultHandler(&DefaultHandler);
}

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::DefaultHandler (ATTR_STRINGIFIER_ARGS) 
	{ IF_HANDLE_LINE(); }

///////////////////////////////////////////////////////////

static const std::string Stringify (
		AST::IdList*		idList, 
		const std::string&	separator
	) {
	if (!idList)
		return "";
	std::list<std::string>& l = DPTR(idList)->l;
	std::string s;
	for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i) {
		if (i != l.begin() && s != separator)
			s.append(separator);
		s.append(*i);
	}
	return s;
}

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::Handle_Function (ATTR_STRINGIFIER_ARGS){ 
	IF_HANDLE_LINE();
	else
	if (attr == AST_ATTRIBUTE_CLASS || attr == AST_ATTRIBUTE_NAME)
		*result = (const char*) data;
	else
		DASSERT(false);
}

void AST::AttributeStringifier::Handle_FormalArgs (ATTR_STRINGIFIER_ARGS) {
	IF_HANDLE_LINE();
	else
	HANDLE_IDLIST(AST_ATTRIBUTE_ITEMS, ",");
}

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::Handle_Lvalue (ATTR_STRINGIFIER_ARGS) 
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); } 

void AST::AttributeStringifier::Handle_StaticLvalue (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); } 

void AST::AttributeStringifier::Handle_LocalLvalue (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); } 

void AST::AttributeStringifier::Handle_AttrLvalue (ATTR_STRINGIFIER_ARGS) 
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); } 

void AST::AttributeStringifier::Handle_NamespaceLvalue (ATTR_STRINGIFIER_ARGS) {
	IF_HANDLE_LINE();	
	else 
	if (attr == AST_ATTRIBUTE_NAME)
		*result = (const char*) data;
	else
	HANDLE_IDLIST(AST_ATTRIBUTE_NAMESPACE, "::");
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

void AST::AttributeStringifier::Handle_ExceptionVar (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); }

void AST::AttributeStringifier::Handle_UsingNamespace (ATTR_STRINGIFIER_ARGS){ 
	IF_HANDLE_LINE();	
	else 
	if (attr == AST_ATTRIBUTE_NAME)
		*result = (const char*) data;
	else
		 HANDLE_IDLIST(AST_ATTRIBUTE_NAMESPACE, "::"); 
}

void AST::AttributeStringifier::Handle_UsingByteCodeLibrary (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_VALUE); }

void AST::AttributeStringifier::Handle_StringifyNamespaceIdent (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_IDLIST(AST_ATTRIBUTE_NAMESPACE, "::"); }

void AST::AttributeStringifier::Handle_StringifyDottedIdents (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_IDLIST(AST_ATTRIBUTE_ITEMS, "."); }

void AST::AttributeStringifier::Handle_StringConst (ATTR_STRINGIFIER_ARGS) {
	IF_HANDLE_LINE();	
	else
	if (attr == AST_ATTRIBUTE_ITEMS)
		*result = std::string("\"") + Stringify((AST::IdList*) data, "") + std::string("\"");
	else
		DASSERT(false);
}

void AST::AttributeStringifier::Handle_ConstDefExpression (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); } 

///////////////////////////////////////////////////////////

void AST::AttributeStringifier::Handle_NumConst (ATTR_STRINGIFIER_ARGS) {
	IF_HANDLE_LINE();	
	else 
	if (attr == AST_ATTRIBUTE_CONSTVALUE)
		*result = unum2string(DPTR((AST::NumItem*) data)->Get());
	else
		DASSERT(false);
}

void AST::AttributeStringifier::Handle_BoolConst (ATTR_STRINGIFIER_ARGS){
	IF_HANDLE_LINE();	
	else 
	if (attr == AST_ATTRIBUTE_CONSTVALUE) {
		util_ui32 val = (util_ui32) data;
		DASSERT(val == 1 || val == 0);
		*result = !val ? "false" : "true";
	}
	else
		DASSERT(false);
}

void AST::AttributeStringifier::Handle_NewAttribute (ATTR_STRINGIFIER_ARGS)
	{ IF_HANDLE_LINE();	else HANDLE_STRING(AST_ATTRIBUTE_NAME); } 

///////////////////////////////////////////////////////////

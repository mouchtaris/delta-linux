// DescriptiveParseErrorHandler.cpp
// A method to support descriptive parse erros for S/R parsers
// without relying on implementation-dependent aspects (only reductions).
// ScriptFighter Project.
// A. Savidis, July 2007.
//

#include "DescriptiveParseErrorHandler.h"
#include "DDebug.h"
#include "DeltaCompErrorMsg.h"
#include "ASTTags.h"

/////////////////////////////////////////////////////

util_ui32 S_FUNC_ (const std::string& funcClass) {
	if (funcClass == AST_VALUE_FUNCCLASS_PROGRAMFUNCTION)	return		T_FUNC;
	if (funcClass == AST_VALUE_FUNCCLASS_ONEVENT)			return		T_FUNC;	
	if (funcClass == AST_VALUE_FUNCCLASS_METHOD)			return		T_METHOD;
	return S_FUNC_ERROR;
}

util_ui32 S_DOTINDEX_ (const std::string& tag) {
	if (tag == AST_TAG_DOTINDEX_IDENT)			return	T_IDENT;
	if (tag == AST_TAG_DOTINDEX_OPSTRING)		return	T_OPINDEX;	
	if (tag == AST_TAG_DOTINDEX_STRINGCONST)	return	T_STRINGCONST;	
	DASSERT(false);
	return 0;
}

util_ui32 S_BRACKET_INDEX_ (const std::string& tag) 
	{ return tag == AST_TAG_BRACKETINDEX_OPMETHOD ? T_OPINDEX : S_EXPR; }

/////////////////////////////////////////////////////

void DescriptiveParseErrorHandler::Push (grammar_symbol_t code, symbol_role_t role) {

	DASSERT(IsInitialised());

	if (!DPTR(parseStack)->empty() && DPTR(parseStack)->top().IsAnyExpectedTemp()) {
		DASSERT(role != role_any_expected_temp_symbol);	// Can't push two successive temp symbols.
		parseStack->pop();
	}

	// If nothing has been pushed and the role is not the main symbol, 
	// we set stmt as the default main.
	
	if (DPTR(parseStack)->empty() && role != role_main_symbol) {
		DPTR(parseStack)->push(StackItem(S_STMT, role_main_symbol)); 
		isMainAddedAutomatically = true; 
	}

	DPTR(parseStack)->push(StackItem(code, role));
}

/////////////////////////////////////////////////////

void DescriptiveParseErrorHandler::PopTill (grammar_symbol_t code, symbol_role_t role) {

	DASSERT(IsInitialised() && !DPTR(parseStack)->empty());

	if (role == role_main_symbol || role == role_group_symbol)
		while (!DPTR(parseStack)->top().IsMain() && !DPTR(parseStack)->top().IsGroup()) {
			DPTR(parseStack)->pop();
			DASSERT(!DPTR(parseStack)->empty());
		}

	DASSERT(
		(DPTR(parseStack)->top().symCode == code || code == S_FUNC_ERROR) &&
		DPTR(parseStack)->top().symRole == role
	);
	DPTR(parseStack)->pop();

	if (DPTR(parseStack)->size() == 1 && isMainAddedAutomatically) {
		DASSERT(DPTR(parseStack)->top().IsMain()); 
		DPTR(parseStack)->pop();
		isMainAddedAutomatically = false;
	}
}

/////////////////////////////////////////////////////

void DescriptiveParseErrorHandler::PopIf (grammar_symbol_t code, symbol_role_t role) {

	DASSERT(IsInitialised());

	if (	!DPTR(parseStack)->empty() && 
			DPTR(parseStack)->top().symCode == code && 
			DPTR(parseStack)->top().symRole == role	)

			DPTR(parseStack)->pop();
}

/////////////////////////////////////////////////////

const std::string DescriptiveParseErrorHandler::GetMessage (grammar_symbol_t code) {
	
	DASSERT(IsInitialised());

	std::map<grammar_symbol_t, std::string>::iterator i;
	i = DPTR(symbolMessages)->find(code);
	DASSERT(i != DPTR(symbolMessages)->end());

	return i->second;
}

/////////////////////////////////////////////////////

const std::string DescriptiveParseErrorHandler::GetReport (void) {

	DASSERT(IsInitialised());

	if (DPTR(parseStack)->empty())	// No handling of this syntax error yet.
		return "";

	std::string expected = GetMessage(DPTR(parseStack)->top().symCode);
	DPTR(parseStack)->pop();

	std::string primary = "In ";
	std::string context, inside;

	if (!DPTR(parseStack)->empty()) {

		bool firstTime = true;

		while (!DPTR(parseStack)->top().IsMain()) {
			if (firstTime && DPTR(parseStack)->top().IsGroup())	// Opened group means we are inside it, not after it.
				inside = std::string(" in ") + "'" + GetMessage(DPTR(parseStack)->top().symCode) + "'";
			else
				context = GetMessage(DPTR(parseStack)->top().symCode) + (!context.empty()? " " : "") + context;				
			firstTime = false;
			DPTR(parseStack)->pop();
			DASSERT(!DPTR(parseStack)->empty());
		}

		primary.append(std::string("'") + GetMessage(DPTR(parseStack)->top().symCode) + "'");
		DPTR(parseStack)->pop();
	}	
	else
		primary.append("'global code'");

	if (!context.empty())
		primary.append(std::string(" after ") + "'" + context + "'" + inside);

	primary.append(std::string(" expecting ") + "'" + expected + "'");
	return primary;
}

/////////////////////////////////////////////////////

void DescriptiveParseErrorHandler::HandleSyntaxError (void) {

	DASSERT(IsInitialised());

	std::string report = GetReport();

	if (!report.empty())
		COMPMESSENGER.Error("syntax error before '%s': %s", getTokenText().c_str(), report.c_str());
	else
		COMPMESSENGER.Error("syntax error before '%s'", getTokenText().c_str());
}

/////////////////////////////////////////////////////

void DescriptiveParseErrorHandler::Clear (void) { 

	DASSERT(IsInitialised());

	while (!DPTR(parseStack)->empty())
		DPTR(parseStack)->pop();
}

/////////////////////////////////////////////////////

#define	_MS(id, s) \
			DASSERT(DPTR(symbolMessages)->find(id) == DPTR(symbolMessages)->end()); \
			(*DPTR(symbolMessages))[id] = s

void DescriptiveParseErrorHandler::Initialise (void) {

	DASSERT(!IsInitialised());
	unew(parseStack);
	unew(symbolMessages);
	getTokenText.reset();
	isMainAddedAutomatically = false;

	_MS(S_FUNC,				"function_def");	
	_MS(S_METHOD,			"method_def");	
	_MS(T_FUNC,				"function");
	_MS(T_METHOD,			"method");
	_MS(S_FNAME,			"<func_name>");
	_MS(S_FARGS,			"formal_args");
	_MS(S_BLOCK,			"block");
	_MS(T_LPAR,				"(");	
	_MS(T_RPAR,				")");	
	_MS(T_IDENT,			"ident");
	_MS(T_COMMA,			",");
	_MS(S_ASSRT,			"assert_stmt");
	_MS(S_EXPR,				"expr");
	_MS(S_STMT,				"stmt");
	_MS(T_SEMI,				";");
	_MS(T_BREAK,			"break");
	_MS(T_CONT,				"continue");
	_MS(T_ASSIGN,			"=");
	_MS(S_CONST,			"const");

	_MS(S_ASSIGNEXPR,		"assign_expr");
	_MS(T_ADDA,				"'+='");
	_MS(T_SUBA,				"'-='");
	_MS(T_MODA,				"'%='");
	_MS(T_DIVA,				"'/='");
	_MS(T_MULA,				"'*='");

	_MS(S_RELAT,			"relational_expr");
	_MS(T_GT,				">");
	_MS(T_GE,				">=");
	_MS(T_LT,				"<");
	_MS(T_LE,				"<=");
	_MS(T_EQ,				"==");
	_MS(T_NE,				"!=");

	_MS(S_LOGICAL,			"logical_expr");
	_MS(T_OR,				"or");	
	_MS(T_AND,				"and");
	_MS(T_NOT,				"!");

	_MS(S_ARITH,			"arithmetic_expr");
	_MS(T_ADD,				"+");
	_MS(T_SUB,				"-");
	_MS(T_MOD,				"%");
	_MS(T_DIV,				"/");
	_MS(T_MUL,				"*");

	_MS(S_TERNARY,			"ternary_expr");	
	_MS(T_QMARK,			"?");
	_MS(T_COLON,			":");

	_MS(S_LVAL,				"lvalue");
	_MS(T_PLUSPLUS,			"++");
	_MS(T_MINUSMINUS,		"--");

	_MS(T_BAR,				"|");
	_MS(S_CALL,				"function_call");
	_MS(S_AARGS,			"actual_args");

	_MS(T_STATIC,			"static");
	_MS(T_LOCAL,			"local");
	_MS(T_GLOBAL,			"::");
	_MS(S_NAMESPACE,		"namespace");

	_MS(T_DOT,				".");
	_MS(T_DDOT,				"..");
	_MS(T_TDOT,				"...");

	_MS(T_LB,				"[");
	_MS(T_RB,				"]");
	_MS(T_DLB,				"[[");
	_MS(T_DRB,				"]]");	
	_MS(S_DOTINDEX,			"ident / kwd_ident / operator / string");
	_MS(T_OPINDEX,			"operator");

	_MS(T_SELF,				"self");
	_MS(T_ARGUMENTS,		"arguments");
	_MS(S_TABLE,			"object_constructor");
	_MS(T_LAMBDA,			"lambda");
	_MS(T_LAMBDA_REF,		"@lambda");
	_MS(S_TABLIST,			"object_elements");
	_MS(S_INDEXEXPR,		"index");
	_MS(S_INDICES,			"indices");
	_MS(T_LBC,				"{");
	_MS(T_RBC,				"}");
	_MS(S_ELIST,			"expr_list");
	_MS(N_EMPTY,			"");

	_MS(S_WHILE,			"while_stmt");
	_MS(S_FOR,				"for_stmt");
	_MS(S_FOREACH,			"foreach_stmt");
	_MS(S_IF,				"if_stmt");
	_MS(S_ELSE,				"else_stmt");
	_MS(T_WHILE,			"while");
	_MS(T_FOR,				"for");
	_MS(T_FOREACH,			"foreach");
	_MS(T_IF,				"if");
	_MS(T_ELSE,				"else");
	_MS(T_RET,				"return");
	_MS(S_RET,				"return_stmt");

	_MS(S_TRY,				"try_stmt");	
	_MS(S_THROW,			"throw_stmt");
	_MS(T_TRY,				"try");
	_MS(T_TRAP,				"trap");
	_MS(T_THROW,			"throw");
	_MS(T_STRINGCONST,		"string_const");
	_MS(T_SET,				"set");
	_MS(T_GET,				"get");
	_MS(T_STRINGIFY,		"#");
	_MS(T_NEWSELF,			"@self");
	_MS(T_ATTRIBUTEID,		"attribute_id");
	_MS(T_LPAR_OR_TDOT,		") or ...)");
	_MS(T_LBC_OR_COLON,		"} or :");
	_MS(S_BRACKET_INDEX,	"expr / operator");
	_MS(T_KWDIDENT,			"kwd_ident");
	_MS(T_IDENT_OR_KWDIDENT,"ident / kwd_ident");
	_MS(T_ATTRIBUTE,		"@");
	_MS(T_SEMI_OR_COMMA,	": or ,");
	_MS(T_LOCAL_OR_FUNCNAME,"local / func name");
	_MS(T_TABLE_ELEM,		"expr / func / method / indexed elem / attr");
}

/////////////////////////////////////////////////////

void DescriptiveParseErrorHandler::CleanUp (void) {
	DASSERT(IsInitialised());
	udelete(parseStack);
	udelete(symbolMessages);
}

/////////////////////////////////////////////////////

DescriptiveParseErrorHandler::DescriptiveParseErrorHandler (void) : 
	parseStack					((std::stack<StackItem>*) 0),
	symbolMessages				((SymbolMessageMap*) 0),
	isMainAddedAutomatically	(false)
{}

/////////////////////////////////////////////////////

// Unparse.cpp
// Unparser for Delta syntax elements.
// ScriptFighter Project.
// A. Savidis, October 2009.
//

#include "Unparse.h"
#include "ulexutil.h"
#include "ustrings.h"
#include "ASTNode.h"
#include "ASTTags.h"
#include "LibraryNamespaceHolder.h"
#include "ParseParms.h"

#define	TRANSLATE_SYNTAX_TREE

#ifdef	TRANSLATE_SYNTAX_TREE
#include "DeltaSyntax.h"
#else
#include "Expr.h"
#include "InterCode.h"
#include "DeltaParser.h"
#endif

static std::map<util_ui32, std::string>* tokenStrings = 
	(std::map<util_ui32, std::string>*) 0;

#define	ADD_TOKEN(t,s) \
	(*DPTR(tokenStrings))[t] = s

#define	SCHAR(s)	std::string(s)
#define	SPACE		SCHAR(" ")
#define	NL			SCHAR("\n")
#define	QUOTE(str)	(SCHAR("\"") + str + SCHAR("\""))

//////////////////////////////////////////////////////

void Unparse_SingletonCreate (void) {

	unew(tokenStrings);
	
	ADD_TOKEN(SELF,			"self");
	ADD_TOKEN(LOCAL,		"local ");
	ADD_TOKEN(GLOBAL_SCOPE,	"::");
	ADD_TOKEN(STATIC,		"static ");
	ADD_TOKEN(STRINGIFY,	"#");
	ADD_TOKEN(ATTRIBUTE,	"@");
	ADD_TOKEN(SET,			"@set");
	ADD_TOKEN(GET,			"@get");
	ADD_TOKEN(ARGUMENTS,	"arguments");
	ADD_TOKEN(LAMBDA,		"lambda");
	ADD_TOKEN(LAMBDA_REF,	"@lambda");
	ADD_TOKEN(NEWSELF,		"@self");
	ADD_TOKEN(ASSERT,		"assert");
	ADD_TOKEN(RETURN,		"return");
	ADD_TOKEN(BREAK,		"break");
	ADD_TOKEN(CONTINUE,		"continue");
	ADD_TOKEN(TRY,			"try");
	ADD_TOKEN(TRAP,			"trap");
	ADD_TOKEN(THROW,		"throw");
	ADD_TOKEN(IF,			"if");
	ADD_TOKEN(ELSE,			"else");
	ADD_TOKEN(WHILE,		"while");
	ADD_TOKEN(FOR,			"for");
	ADD_TOKEN(FOREACH,		"foreach");
	ADD_TOKEN(METHOD,		"method");
	ADD_TOKEN(FUNCTION,		"function");
	ADD_TOKEN(ONEVENT,		"onevent");
	ADD_TOKEN(USING,		"using");

	ADD_TOKEN(ASSIGN,		"=");
	ADD_TOKEN(ADD_A,		"+=");
	ADD_TOKEN(MUL_A,		"*=");
	ADD_TOKEN(DIV_A,		"/=");
	ADD_TOKEN(SUB_A,		"-=");
	ADD_TOKEN(MOD_A,		"%=");
	ADD_TOKEN(ADD,			"+");
	ADD_TOKEN(MUL,			"*");
	ADD_TOKEN(DIV,			"/");
	ADD_TOKEN(MOD,			"%");
	ADD_TOKEN(SUB,			"-");
	ADD_TOKEN(UMINUS,		"-");
	ADD_TOKEN(MINUSMINUS,	"--");
	ADD_TOKEN(PLUSPLUS,		"++");

	ADD_TOKEN(GT,			">");
	ADD_TOKEN(LT,			"<");
	ADD_TOKEN(GE,			">=");
	ADD_TOKEN(LE,			"<=");
	ADD_TOKEN(EQ,			"==");
	ADD_TOKEN(NE,			"!=");

	ADD_TOKEN(AND,			"and");
	ADD_TOKEN(OR,			"or");
	ADD_TOKEN(NOT,			"not");
	ADD_TOKEN(TRIPLE_DOT,	"...");

#ifdef	TRANSLATE_SYNTAX_TREE
	ADD_TOKEN(META_LSHIFT,	"<<");
	ADD_TOKEN(META_RSHIFT,	">>");
	ADD_TOKEN(META_ESCAPE,	"~");
	ADD_TOKEN(META_INLINE,	"!");
	ADD_TOKEN(META_EXECUTE,	"&");
	ADD_TOKEN(META_RENAME,	"$");
#endif
}

void Unparse_SingletonDestroy (void)
	{ udelete(tokenStrings); }

const std::string Unparse_Token (util_ui32 t) {
	DASSERT(tokenStrings);
	std::map<util_ui32, std::string>::iterator i = DPTR(tokenStrings)->find(t);
	DASSERT(i != DPTR(tokenStrings)->end());
	return i->second;
}

static const std::string HandleDoubleBrackets(const std::string& text)
	{ return text.empty() || text.front() != '[' && text.back() != ']' ? text : " " + text + " "; }

//////////////////////////////////////////////////////

#define	_CS_	const std::string

_CS_ Unparse_Nil (void)
	{ return "nil"; }

_CS_ Unparse_Number (double num)
	{ return unum2string(num); }

_CS_ Unparse_Bool (bool val)
	{ return val ? "true" : "false"; }

//////////////////////////////////////////////////////

_CS_ Unparse_StringConst (_CS_& s)
	{ return uconstructstr("\"%s\"", uunparseescapesequences(s.c_str()).c_str()); }

_CS_ Unparse_StringConst (const std::list<std::string>& l) {
	std::string s;
	for (std::list<std::string>::const_iterator i = l.begin(); i != l.end(); ++i)
		s.append(*i); 
	return uconstructstr("\"%s\"", uunparseescapesequences(s.c_str()).c_str());
}

_CS_ Unparse_StringifyDottedIdents (const std::list<std::string>& l)  {
	std::string s;
	for (std::list<std::string>::const_iterator i = l.begin(); i != l.end(); ++i) {
		if (!s.empty())
			s.append(Unparse_Token(DOT));
		s.append(*i);
	}
	return Unparse_Token(STRINGIFY) + s;
}

_CS_ Unparse_StringifyNamespaceIdent (const std::list<std::string>& l) 
	{ return  Unparse_Token(STRINGIFY) + DeltaLibraryNamespaceHolder::MakeFullyQualifiedName(l); }

_CS_ Unparse_NamespaceIdent (const std::list<std::string>& l, _CS_& name) {
	std::list<std::string> lc(l);
	lc.push_back(name);
	return DeltaLibraryNamespaceHolder::MakeFullyQualifiedName(lc);
}

_CS_ Unparse_ConstDef (_CS_& name, _CS_& value)
	{ return uconstructstr("const %s = %s", name.c_str(), value.c_str()); }

_CS_ Unparse_Var (_CS_& name, util_ui32 token, _CS_& infix)
	{ return uconstructstr("%s%s%s", Unparse_Token(token).c_str(), infix.c_str(), name.c_str()); }

//////////////////////////////////////////////////////

_CS_ Unparse_BinaryOp (_CS_& left, util_ui32 op, _CS_& right)
	{ return left + SPACE + Unparse_Token(op) + SPACE + right; }

_CS_ Unparse_UnaryOp (util_ui32 op, _CS_& e, _CS_& infix)
	{ return Unparse_Token(op) + infix + e; }

_CS_ Unparse_UnaryOp (_CS_& e, util_ui32 op)
	{ return e +  Unparse_Token(op); }

_CS_ Unparse_UnaryIncDec (util_ui32 op, _CS_& e, bool after)
	{ return after ? Unparse_UnaryOp(e, op) : Unparse_UnaryOp(op, e); }

_CS_ Unparse_TernaryOp (_CS_& cond, _CS_& e1, _CS_& e2)
	{ return SCHAR("( ") + cond + SCHAR(" ? ") + e1 + SCHAR(" : ") + e2 + SCHAR(" )"); }

//////////////////////////////////////////////////////

_CS_ Unparse_ExprList (const std::list<std::string>& l) {
	std::string s;
	for (std::list<std::string>::const_iterator i = l.begin(); i != l.end(); ++i) {
		s.append(*i); 
		if (&*i != &l.back() )
			s.append(SCHAR(", "));
	}
	return s;
}

_CS_ Unparse_LateBound (_CS_& arg)
	{ return SCHAR("|") + arg + SCHAR("|"); }

_CS_ Unparse_ParenthesisedExpr (_CS_& e)
	{ return SCHAR("(") + e + SCHAR(")"); }

//////////////////////////////////////////////////////

_CS_ Unparse_TableIndices (_CS_& indices, _CS_& index)
	{ return indices + SCHAR(", ") + index; }

_CS_ Unparse_IndexedValues (_CS_& indices, _CS_& values)
	{ return SCHAR("{ ") + indices + SCHAR(" : ") + values + SCHAR(" }"); }

_CS_ Unparse_IdentIndexElement (_CS_& index, _CS_& e)
	{ return Unparse_Token(ATTRIBUTE) + index + SCHAR(" : ") + e; }

_CS_ Unparse_DottedIdent (_CS_& id)	
	{ return SCHAR(".") + id; }

_CS_ Unparse_TableElements (_CS_& elems, _CS_& elem)
	{ return elems + SCHAR(", ") + elem; }

_CS_ Unparse_TableElementsPrettyPrint (_CS_& elems, _CS_& elem)
	{ return elems + SCHAR(",\n") + elem; }

_CS_ Unparse_TableConstructor (_CS_& elems)
	{ return SCHAR("[") + HandleDoubleBrackets(elems) + SCHAR("]"); }

_CS_ Unparse_TableConstructorPrettyPrint (_CS_& elems, util_ui16 tabs)
	{ return elems.empty() ? "[]" : SCHAR("[\n") + elems + SCHAR("\n") + utabstops(tabs) + "]"; }

_CS_ Unparse_NewAttributeSet (_CS_& id, _CS_& set, _CS_& get) {
	return	Unparse_Token(ATTRIBUTE) + id + SCHAR("{")	+
			Unparse_Token(SET) + SPACE + set			+
			Unparse_Token(GET) + SPACE + get			+
			SCHAR("}");
}

//////////////////////////////////////////////////////

_CS_ Unparse_TableContentDot (_CS_& t, _CS_& index)
	{ return t + SCHAR(".") + index; }

_CS_ Unparse_TableContentDoubleDot (_CS_& t, _CS_& index)
	{ return t + SCHAR("..") + index; }

_CS_ Unparse_TableContentBracket (_CS_& t, _CS_& index)
	{ return t + SCHAR("[") + HandleDoubleBrackets(index) + SCHAR("]"); }

_CS_ Unparse_TableContentDoubleBracket (_CS_& t, _CS_& index)
	{ return t + SCHAR("[[") + HandleDoubleBrackets(index) + SCHAR("]]"); }

//////////////////////////////////////////////////////

_CS_ Unparse_Compound (_CS_& compound)
	{ return SCHAR("{ ") + compound + SCHAR(" }"); }

_CS_ Unparse_CompoundPrettyPrint (_CS_& compound, util_ui16 tabs)
	{ return  SCHAR("{\n") + compound + SCHAR("\n") + utabstops(tabs) + SCHAR("}"); }

_CS_ Unparse_Stmts (_CS_& stmts, _CS_& stmt)
	{ return stmts + SPACE + stmt; }

_CS_ Unparse_ExprListStmt (_CS_& el)
	{ return el + SCHAR(";"); }

_CS_ Unparse_EmptyStmt (void)
	{ return ";"; }

_CS_ Unparse_ExprStmt (util_ui32 token, _CS_& e, bool addSemi) {
	std::string result = Unparse_Token(token) + SPACE + e;
	if (addSemi)
		result.append(SCHAR(";"));
	return result;
}

_CS_ Unparse_BuiltInStmt (util_ui32 token)
	{ return Unparse_Token(token) + SCHAR(";"); }

//////////////////////////////////////////////////////

_CS_ Unparse_TryTrap (_CS_& trySection, _CS_& trapSection, _CS_& exceptionVar) {
	return	Unparse_Token(TRY)			+ SPACE + 
			trySection					+ SPACE + 
			Unparse_Token(TRAP)			+ SPACE +
			exceptionVar				+ SPACE + 
			trapSection;
}

_CS_ Unparse_TryTrapPrettyPrint (_CS_& trySection, _CS_& trapSection, _CS_& exceptionVar, util_ui16 tabs) {
	return	Unparse_Token(TRY)			+ SPACE + 
			trySection					+ NL + utabstops(tabs - 1) + 
			Unparse_Token(TRAP)			+ SPACE +
			exceptionVar				+ NL + utabstops(tabs) + 
			trapSection;
}

//////////////////////////////////////////////////////

_CS_ Unparse_If (_CS_& cond, _CS_& ifSection)
	{ return Unparse_Token(IF) + SPACE + SCHAR("(") + cond + SCHAR(")") + SPACE + ifSection; }

_CS_ Unparse_IfPrettyPrint (_CS_& cond, _CS_& ifSection, util_ui16 tabs)
	{ return Unparse_Token(IF) + SPACE + SCHAR("(") + cond + SCHAR(")\n") + utabstops(tabs) + ifSection; }

_CS_ Unparse_IfElse (_CS_& cond, _CS_& ifSection, _CS_& elseSection) {
	return	Unparse_Token(IF)				+ SPACE + 
			SCHAR("(") + cond + SCHAR(")")	+ SPACE + 
			ifSection						+ SPACE +
			Unparse_Token(ELSE)				+ SPACE + 
			elseSection;
}

_CS_ Unparse_IfElsePrettyPrint (_CS_& cond, _CS_& ifSection, _CS_& elseSection, util_ui16 tabs) {
	return	Unparse_Token(IF)					+ SPACE + 
			SCHAR("(") + cond + SCHAR(")\n")	+ utabstops(tabs) + 
			ifSection							+ SCHAR("\n") + utabstops(tabs - 1) +
			Unparse_Token(ELSE)					+ SCHAR("\n") + utabstops(tabs) +
			elseSection;
}

//////////////////////////////////////////////////////

_CS_ Unparse_While (_CS_& cond, _CS_& stmt)
	{ return Unparse_Token(WHILE) + SPACE + SCHAR("(") + cond + SCHAR(")") + SPACE + stmt; }

_CS_ Unparse_WhilePrettyPrint (_CS_& cond, _CS_& stmt, util_ui16 tabs)
	{ return Unparse_Token(WHILE) + SPACE + SCHAR("(") + cond + SCHAR(")\n") + utabstops(tabs) + stmt; }

//////////////////////////////////////////////////////

_CS_ Unparse_ForInitList (_CS_& el)
	{ return el + SCHAR(";"); }

_CS_ Unparse_For(_CS_& initSection,_CS_& cond, _CS_& suffixSection, _CS_& stmt) {
	return	Unparse_Token(FOR)						+ SPACE + 
			SCHAR("(") + initSection + SCHAR(";")	+ SPACE +
			cond + SCHAR(";")						+ SPACE +
			suffixSection + SCHAR(")")				+ SPACE +
			stmt;
}

_CS_ Unparse_ForPrettyPrint (_CS_& initSection,_CS_& cond, _CS_& suffixSection, _CS_& stmt, util_ui16 tabs) {
	return	Unparse_Token(FOR)						+ SPACE + 
			SCHAR("(") + initSection + SCHAR(";")	+ SPACE +
			cond + SCHAR(";")						+ SPACE +
			suffixSection + SCHAR(")\n")			+ utabstops(tabs) +
			stmt;
}

//////////////////////////////////////////////////////

_CS_ Unparse_ForeachPrefix (_CS_& var, _CS_& index, _CS_& cont) {
	if (index.empty())
		return Unparse_Token(FOREACH)	+ SPACE + SCHAR("(") + var + SCHAR(",") + SPACE + cont + SCHAR(")");
	else
		return Unparse_Token(FOREACH)	+ SPACE + SCHAR("(") + index + SCHAR(":") + var + SCHAR(",") + SPACE + cont + SCHAR(")");
}

_CS_ Unparse_ForeachStmt (_CS_& foreachPrefix, _CS_& stmt)
	{ return foreachPrefix + stmt; }

_CS_ Unparse_ForeachStmtPrettyPrint (_CS_& foreachPrefix, _CS_& stmt, util_ui16 tabs)
	{ return foreachPrefix +  SCHAR("\n") + utabstops(tabs) + stmt; }

//////////////////////////////////////////////////////

_CS_ Unparse_FunctionPrefix (_CS_& name, util_ui8 funcClass, bool isExported) {
	return	Unparse_Token(
				funcClass == DELTA_FUNCCLASS_METHOD		? METHOD:
				funcClass == DELTA_FUNCCLASS_ONEVENT	? ONEVENT : FUNCTION
			) + 
			(isExported ? std::string("") :  (SPACE + Unparse_Token(LOCAL)))	+
			(!name.empty() ? SPACE : std::string(""))							+ 
			(ParseParms::IsOperator(name) ? "@operator" : "")					+
			name;
}

//***********************

static _CS_ Unparse_FunctionFormals (
		const std::list<std::string>&			l, 
		std::list<std::string>::const_iterator& i
	) {
	if (i == l.end())
		return "";
	else {
		std::string code("(");
		while (i != l.end()) {
			code.append(*i);
			if (++i != l.end() && *i != AST_VALUE_VARARGS_FORMAL_NAME)
				code.append(", ");
		}
		return code + SCHAR(")");
	}
}

//***********************

_CS_ Unparse_FunctionFormals (const std::list<std::string>& l) 
	{ return Unparse_FunctionFormals(l, utempobj(l.begin())); }

//***********************

_CS_ Unparse_FunctionFormals (const std::list<std::string>& l, util_ui8 funcClass) {

	std::list<std::string>::const_iterator i = l.begin();

	if (funcClass == DELTA_FUNCCLASS_METHOD)	// self, arguments
		{ DASSERT(l.size() > 1); ++i; ++i; }
	else 
		{ DASSERT(l.size() > 0); ++i; }			// arguments
	
	return Unparse_FunctionFormals(l, i);
}

//***********************

_CS_ Unparse_Function (_CS_& prefix, _CS_& formals, _CS_& compound)
	{ return prefix + formals + compound; }

_CS_ Unparse_LambdaFunction (_CS_& formals, _CS_& stmt)
	{ return Unparse_Token(LAMBDA) + formals + stmt; }

 _CS_ Unparse_LambdaStmt (_CS_& expr)
	 { return  SCHAR("{ ") + expr +  SCHAR(" }"); }

_CS_ Unparse_FunctionPrettyPrint (_CS_& prefix, _CS_& formals, _CS_& compound, util_ui16 tabs)
	{ return prefix + formals + SCHAR("\n") + utabstops(tabs) + compound; }

_CS_ Unparse_FunctionParenthesisForm (_CS_& func)
	{ return SCHAR("(") + func + SCHAR(")"); }

//***********************

_CS_ Unparse_FunctionCall (_CS_& func, _CS_& args)
	{ return func + SCHAR("(") + args + SCHAR(")"); }

//////////////////////////////////////////////////////

_CS_ Unparse_UsingNamespace (const std::list<std::string>& namespacePath) 
	{ return Unparse_UsingNamespace(DeltaLibraryNamespaceHolder::MakeFullyQualifiedName(namespacePath)); }

_CS_ Unparse_UsingNamespace (_CS_& ns) 
	{ return Unparse_Token(USING) + SPACE + ns + SCHAR(";"); }

_CS_ Unparse_UsingByteCodeLibrary (_CS_& id)
	{ return Unparse_Token(USING) + SPACE + Unparse_Token(STRINGIFY)+ id + SCHAR(";"); }

//////////////////////////////////////////////////////

_CS_ Unparse_Escape (util_ui32 cardinality, _CS_& expr, bool isEscapeIdent) {
	std::string s;
	for(util_ui32 i = 0; i < cardinality; ++i)
		s += Unparse_Token(META_ESCAPE);
	return isEscapeIdent ? s + expr : s + SCHAR("(") + expr + SCHAR(")");
}

_CS_ Unparse_QuotedElements (_CS_& elems, _CS_& elem)
	{ return elems + SCHAR(", ") + elem; }

_CS_ Unparse_QuasiQuotes (_CS_& prefix, _CS_& value, _CS_& suffix)
	{ return Unparse_Token(META_LSHIFT) + prefix + value + suffix + Unparse_Token(META_RSHIFT); }

_CS_ Unparse_Inline (_CS_& expr)
	{ return Unparse_Token(META_INLINE) + SCHAR("(") + expr + SCHAR(")"); }

//////////////////////////////////////////////////////

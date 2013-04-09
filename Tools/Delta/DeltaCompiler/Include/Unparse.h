// Unparse.h
// Unparser for Delta syntax elements.
// ScriptFighter Project.
// A. Savidis, October 2009.
//

#ifndef	UNAPRSE_H
#define	UNPARSE_H

#include "utypes.h"
#include "ustrings.h"
#include <list>
#include <string>

//////////////////////////////////////////////////////

#define	_CS_	const std::string

extern _CS_ Unparse_Nil (void);
extern _CS_ Unparse_Number (double num);
extern _CS_ Unparse_Bool (bool val);
extern _CS_ Unparse_StringConst (_CS_& s);
extern _CS_ Unparse_StringConst (const std::list<std::string>& l);
extern _CS_ Unparse_StringifyDottedIdents (const std::list<std::string>& l);
extern _CS_ Unparse_StringifyNamespaceIdent (const std::list<std::string>& l);
extern _CS_ Unparse_NamespaceIdent (const std::list<std::string>& l, _CS_& name);
extern _CS_ Unparse_ConstDef (_CS_& name, _CS_& value);
extern _CS_ Unparse_Token (util_ui32 token);
extern _CS_ Unparse_Var (_CS_& name, util_ui32 token, _CS_& infix = "");
inline _CS_ Unparse_Var (_CS_& name)				{ return name; }
inline _CS_ Unparse_BuiltIn (util_ui32 token)		{ return Unparse_Token(token); }
extern _CS_ Unparse_BinaryOp (_CS_& left, util_ui32 op, _CS_& right);
extern _CS_ Unparse_UnaryOp (util_ui32 op, _CS_& e, _CS_& infix = "");
extern _CS_ Unparse_UnaryOp (_CS_& e, util_ui32 op);
extern _CS_ Unparse_UnaryIncDec (util_ui32 op, _CS_& e, bool after);
extern _CS_ Unparse_TernaryOp (_CS_& cond, _CS_& e1, _CS_& e2);
extern _CS_ Unparse_ExprList (const std::list<std::string>& l);
extern _CS_ Unparse_LateBound (_CS_& arg);
extern _CS_ Unparse_ParenthesisedExpr (_CS_& e);
extern _CS_ Unparse_TableIndices (_CS_& indices, _CS_& index);
extern _CS_ Unparse_IndexedValues (_CS_& indices, _CS_& values);
inline _CS_ Unparse_UnindexedValue (_CS_& e)	{ return e; }
extern _CS_ Unparse_IdentIndexElement (_CS_& index, _CS_& e);
inline _CS_ Unparse_OperatorIndex (_CS_& op)	{ return op; }
extern _CS_ Unparse_DottedIdent (_CS_& id);
extern _CS_ Unparse_TableElements (_CS_& elems, _CS_& elem);
extern _CS_ Unparse_TableElementsPrettyPrint (_CS_& elems, _CS_& elem);
extern _CS_ Unparse_TableConstructor (_CS_& elems);
extern _CS_ Unparse_TableConstructorPrettyPrint (_CS_& elems, util_ui16 tabs);
extern _CS_ Unparse_TableContentDot (_CS_& t, _CS_& index);
extern _CS_ Unparse_TableContentDoubleDot (_CS_& t, _CS_& index);
extern _CS_ Unparse_TableContentBracket (_CS_& t, _CS_& index);
extern _CS_ Unparse_TableContentDoubleBracket (_CS_& t, _CS_& index);
extern _CS_ Unparse_Stmts (_CS_& stmts, _CS_& stmt);
extern _CS_ Unparse_ExprListStmt (_CS_& el);
extern _CS_ Unparse_EmptyStmt (void);
extern _CS_ Unparse_ExprStmt (util_ui32 token, _CS_& e, bool addSemi = true);
extern _CS_ Unparse_BuiltInStmt (util_ui32 token);
extern _CS_ Unparse_TryTrap (_CS_& trySection, _CS_& trapSection, _CS_& exceptionVar);
extern _CS_ Unparse_TryTrapPrettyPrint (_CS_& trySection, _CS_& trapSection, _CS_& exceptionVar, util_ui16 tabs);
extern _CS_ Unparse_If (_CS_& cond, _CS_& ifSection);
extern _CS_ Unparse_IfPrettyPrint (_CS_& cond, _CS_& ifSection, util_ui16 tabs);
extern _CS_ Unparse_IfElse (_CS_& cond, _CS_& ifSection, _CS_& elseSection);
extern _CS_ Unparse_IfElsePrettyPrint (_CS_& cond, _CS_& ifSection, _CS_& elseSection, util_ui16 tabs);
extern _CS_ Unparse_While (_CS_& cond, _CS_& stmt);
extern _CS_ Unparse_WhilePrettyPrint (_CS_& cond, _CS_& stmt, util_ui16 tabs);
extern _CS_ Unparse_ForInitList (_CS_& el);
extern _CS_ Unparse_For (_CS_& initSection,_CS_& cond, _CS_& suffixSection, _CS_& stmt);
extern _CS_ Unparse_ForPrettyPrint (_CS_& initSection,_CS_& cond, _CS_& suffixSection, _CS_& stmt, util_ui16 tabs);
extern _CS_ Unparse_ForeachPrefix (_CS_&  foreachVar, _CS_& foreachIndex, _CS_&  foreachContainer);
extern _CS_ Unparse_ForeachStmt (_CS_& foreachPrefix, _CS_& stmt);
extern _CS_ Unparse_ForeachStmtPrettyPrint (_CS_& foreachPrefix, _CS_& stmt, util_ui16 tabs);
extern _CS_ Unparse_Compound (_CS_& compound);
extern _CS_ Unparse_CompoundPrettyPrint (_CS_& compound, util_ui16 tabs);
extern _CS_ Unparse_FunctionPrefix (_CS_& name, util_ui8 funcClass, bool isExported);
extern _CS_ Unparse_FunctionFormals (const std::list<std::string>& formals, util_ui8 funcClass);
extern _CS_ Unparse_FunctionFormals (const std::list<std::string>& formals);
extern _CS_ Unparse_Function (_CS_& prefix, _CS_& formals, _CS_& compound);
extern _CS_ Unparse_LambdaFunction (_CS_& formals, _CS_& stmt);
extern _CS_ Unparse_LambdaStmt (_CS_& expr);
extern _CS_ Unparse_FunctionPrettyPrint (_CS_& prefix, _CS_& formals, _CS_& compound, util_ui16 tabs);
inline _CS_ Unparse_FunctionDef (_CS_& func) { return func; }
extern _CS_ Unparse_UsingNamespace (const std::list<std::string>& namespacePath);
extern _CS_ Unparse_UsingNamespace (_CS_& ns);
extern _CS_ Unparse_UsingByteCodeLibrary (_CS_& id);
extern _CS_ Unparse_FunctionCall (_CS_& func, _CS_& args);
extern _CS_ Unparse_FunctionParenthesisForm (_CS_& func);
extern _CS_	Unparse_NewAttributeSet (_CS_& id, _CS_& set, _CS_& get);

extern _CS_	Unparse_Escape (util_ui32 cardinality, _CS_& expr);
extern _CS_ Unparse_QuotedElements (_CS_& elems, _CS_& elem);
extern _CS_ Unparse_QuasiQuotes (_CS_& prefix, _CS_& value, _CS_& suffix);
extern _CS_ Unparse_Inline (_CS_& expr);

#undef	_CS_
extern void Unparse_SingletonCreate (void);
extern void Unparse_SingletonDestroy (void);

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

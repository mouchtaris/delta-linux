// ParseConstants.cpp.
// Translation rules for constants.
// ScriptFighter Project.
// A. Savidis, October 1999 (original 1st version).
// Re-factored last August 2006.
//

#include <string.h>
#include <limits.h>
#include <math.h>

#include "DDebug.h"
#include "ParseActions.h"
#include "DeltaByteCodeTypes.h"
#include "LibraryNamespaceHolder.h"
#include "DeltaCompErrorMsg.h"

///////////////////////////////////////////////////////////////////

void Translator::DELTA_EXPR_EMIT_BOOL_TEST (DeltaExpr* expr, DeltaExpr* result, bool trueTest) {
	QUADS.Emit(trueTest ? DeltaIC_JTRUETEST : DeltaIC_JFALSETEST, expr, NIL_EXPR, NIL_EXPR);
	DPTR(result)->trueList = QUADS.MakeList(QUADS.CurrQuadNo());
	QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	DPTR(result)->falseList = QUADS.MakeList(QUADS.CurrQuadNo());
}

//------------------------------------------------------------------
// CONSTANT EXPRESSIONS.

// The expression is actually the first argument, and the string const the
// second one.

DeltaExpr* Translator::Translate_StringConst (const std::string& str_2nd, DeltaExpr* e_1st){
	if (!e_1st)
		return EXPRFACTORY.MakeConst(str_2nd);
	else {
		DPTR(e_1st)->strConst.append(str_2nd);	// Reuse e_1st
		return e_1st;
	}
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translator::Translate_StringifyDottedIdents (const std::string& id, DeltaExpr* prefix) {
	if (!prefix)
		return EXPRFACTORY.MakeStringifiedConst(id);
	else {
		DASSERT(DPTR(prefix)->IsStringified());
		DPTR(prefix)->strConst.append(std::string(".") + id);	// Reuse prefix.
		return prefix;
	}
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translator::Translate_StringifyNamespaceIdent (const NameList& namespacePath) 
	{ return EXPRFACTORY.MakeStringifiedConst(DELTANAMESPACES.MakeFullyQualifiedName(namespacePath)); }

DeltaExpr* Translator::Translate_StringifyNamespaceIdent (const std::string& id) {
	DeltaExpr* result = EXPRFACTORY.MakeStringifiedConst(
							DELTANAMESPACES.MakeFullyQualifiedName(PARSEPARMS.GetNamespacePath()) + 
							DELTA_LIBRARYNAMESPACE_SEPARATOR +
							id
						);
	PARSEPARMS.ClearNamespacePath();
	return result;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translator::Translate_ConstValue (const std::string& s) {
	DeltaExpr* expr = EXPRFACTORY.New();
	DPTR(expr)->SetString(s);
	return expr;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translator::Translate_ConstValue (DeltaNumberValueType num) {
	DeltaExpr* expr = EXPRFACTORY.New();
	DPTR(expr)->SetNumber(num);
	return expr;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translator::Translate_ConstValue (bool val) 
	{ return EXPRFACTORY.MakeBool(val); }

///////////////////////////////////////////////////////////////////

DeltaExpr* Translator::Translate_ConstValue (void) {
	DeltaExpr* expr = EXPRFACTORY.New();
	DPTR(expr)->SetNil();
	return expr;
}

///////////////////////////////////////////////////////////////////

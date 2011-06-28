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

//------------------------------------------------------------------
// CONSTANT EXPRESSIONS.

// The expression is actually the first argument, and the string const the
// second one.

DeltaExpr* Translate_StringConst (const char* str_2nd, DeltaExpr* e_1st){
	if (!e_1st)
		return DeltaExpr::MakeConst(str_2nd);
	else {
		DPTR(e_1st)->strConst.append(str_2nd);	// Reuse e_1st
		return e_1st;
	}
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translate_StringifyDottedIdents (const char* id, DeltaExpr* prefix) {
	if (!prefix)
		return DeltaExpr::MakeStringifiedConst(id);
	else {
		DASSERT(DPTR(prefix)->IsStringified());
		DPTR(prefix)->strConst.append(std::string(".") + id);	// Reuse prefix.
		return prefix;
	}
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translate_StringifyNamespaceIdent (const NameList& namespacePath) 
	{ return DeltaExpr::MakeStringifiedConst(DELTANAMESPACES.MakeFullyQualifiedName(namespacePath)); }

DeltaExpr* Translate_StringifyNamespaceIdent (const char* id) {
	DeltaExpr* result = DeltaExpr::MakeStringifiedConst(
							DELTANAMESPACES.MakeFullyQualifiedName(ParseParms::GetNamespacePath()) + 
							DELTA_LIBRARYNAMESPACE_SEPARATOR +
							id
						);
	ParseParms::ClearNamespacePath();
	return result;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translate_ConstValue (const std::string& s) {
	DeltaExpr* expr = DNEW(DeltaExpr);
	DPTR(expr)->SetString(s);
	return expr;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translate_ConstValue (DeltaNumberValueType num) {
	DeltaExpr* expr = DNEW(DeltaExpr);
	DPTR(expr)->SetNumber(num);
	return expr;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translate_ConstValue (bool val) 
	{ return DeltaExpr::MakeBool(val); }

///////////////////////////////////////////////////////////////////

DeltaExpr* Translate_ConstValue (void) {
	DeltaExpr* expr = DNEW(DeltaExpr);
	DPTR(expr)->SetNil();
	return expr;
}

///////////////////////////////////////////////////////////////////

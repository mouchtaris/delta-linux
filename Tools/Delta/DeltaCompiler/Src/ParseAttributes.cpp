// ParseAttributes.cpp.
// Translation rules for attributes.
// ScriptFighter Project.
// A. Savidis, Januarty 2009.
//

#include "DDebug.h"
#include "ParseActions.h"
#include "InterCode.h"
#include "DeltaCompErrorMsg.h"
#include "Symbol.h"
#include "Expr.h"
#include "TypeCheck.h"
#include "Unparsable.h"

//------------------------------------------------------------------

DeltaExpr* Translate_AttributeMethod (
		DeltaExpr*			f,
		DeltaQuadAddress	startQuad,
		util_ui32			lastLine
	) {
	NULL_EXPR_CHECK(f);
	DPTR(f)->CheckUninitialised();
	f = DPTR(f)->AdaptIfBool();	// May fail type checking, but needed for completeness.
	if (!TypeCheck_FunctionCall(f))
		return NIL_EXPR;
	QUADS.SetQuadLine(startQuad, lastLine);
	return f;
}

/////////////////////////////////////////////////////////////////////

DeltaExpr* Translate_NewAttribute (
		const char*		id,
		DeltaExpr*		set,
		DeltaExpr*		get
	) {

	NULL_EXPR_PAIR_CHECK(set, get);

	DeltaExpr* attr		= DNEW(DeltaExpr);
	DPTR(attr)->type	= DeltaExprNewAttribute;
	Upnparse_NewAttributeSet(attr, id, set, get);

	QUADS.Emit(
		DeltaIC_OBJNEWATTR,
		set,
		get,
		DeltaExpr::MakeConst(id)
	);

	return attr;
}

//------------------------------------------------------------------

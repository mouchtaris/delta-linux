// ParseActions.cpp.
// Translation schemes for the DeltaCompiler.
// ScriptFighter Project.
// A. Savidis, October 1999 (original 1st version).
//

#include "DDebug.h"
#include "Symbol.h"
#include "ParseActions.h"
#include "InterCode.h"
#include "TypeCheck.h"
#include "DeltaByteCodeTypes.h"
#include "TargetCode.h"
#include "DeltaCompErrorMsg.h"
#include "LocalDataHandling.h"
#include "Optimizer.h"
#include "ufunctors.h"
#include "ulatedestructor.h"
#include "DeltaStdDefs.h"
#include "ASTNode.h"

#define	RETVAL_HIDDEN_VAR	"_retval"

////////////////////////////////////////////////////////////////////

void Translator::MakeReservedGlobalSymbols (void) {

	// Make return value
	DeltaSymbol* sym = DELTASYMBOLS.NewSymbol(RETVAL_HIDDEN_VAR);
	DASSERT(sym->offset == DELTA_RETVALUE_OFFSET);

	retValueExpr					= EXPRFACTORY.New();
	retValueExpr->type				= DeltaExprVar;
	retValueExpr->sym				= sym;

	// Make lambda.
	lambdaValueExpr					= EXPRFACTORY.New();
	DPTR(lambdaValueExpr)->type		= DeltaExprLambda;
	DPTR(lambdaValueExpr)->SetTypeTag(TagMethod);
	DPTR(lambdaValueExpr)->GetTypeInfo().Set(TagMethod);
}

void Translator::Initialise (void) {

	LOCALDATA.Initialise();
	DASSERT(PARSEPARMS.InGlobalScope());
	DASSERT(!PARSEPARMS.CurrScope().value());
}

////////////////////////////////////////////////////////////////////

void Translator::CleanUp (void)
	{ LOCALDATA.CleanUp(); }

////////////////////////////////////////////////////////////////////

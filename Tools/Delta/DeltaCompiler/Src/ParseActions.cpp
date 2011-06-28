// ParseActions.cpp.
// Translation schemes for the DeltaCompiler.
// ScriptFighter Project.
// A. Savidis, October 1999 (original 1st version).
//

#include "DDebug.h"
#include "Symbol.h"
#include "ParseActions.h"
#include "InterCode.h"
#include "ParseParms.h"
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

static DeltaExpr*	retValueExpr		= (DeltaExpr*) 0;
static DeltaExpr*	lambdaValueExpr		= (DeltaExpr*) 0;

DeltaExpr*	DeltaExpr::GetReturnValue (void) 
				{ return retValueExpr; }

DeltaExpr*	DeltaExpr::GetLambdaRef (void) 
				{ return lambdaValueExpr; }

//------------------------------------------------------------------

typedef	udestroyablewrapper<char*, uarrdestructorfunctor<char*> > DestroyableString;

class DeltaCompilerStringDestructorClass : 
	public	ulatedestructionmanager<
				DestroyableString,
				uptrdestructorfunctor<DestroyableString*>
			>{};

class DeltaCompilerStringDestructor : public usingleton<DeltaCompilerStringDestructorClass>{};
USINGLETON_INSTANCE(usingleton<DeltaCompilerStringDestructorClass>)

char* Translate_StringWithLateDestruction (char* s) {
	DeltaCompilerStringDestructor::GetSingleton().add(DNEWCLASS(DestroyableString, (s))); 
	return s;
}

//------------------------------------------------------------------

void ParseActions_SingletonCreate (void) 
	{ DeltaCompilerStringDestructor::SingletonCreate(); }

void ParseActions_SingletonDestroy (void) 
	{ DeltaCompilerStringDestructor::SingletonDestroy(); }

////////////////////////////////////////////////////////////////////

void ParseActions_MakeReservedGlobalSymbols (void) {

	// Make return value
	DeltaSymbol* sym = DNEWCLASS(DeltaSymbol, (RETVAL_HIDDEN_VAR, DeltaSymbol_VarInCurrScope));
	DASSERT(sym->offset == DELTA_RETVALUE_OFFSET);
	DELTASYMBOLS.Install(sym);

	retValueExpr					= DNEW(DeltaExpr);
	retValueExpr->type				= DeltaExprVar;
	retValueExpr->sym				= sym;

	// Make lambda.
	lambdaValueExpr					= DNEW(DeltaExpr);
	DPTR(lambdaValueExpr)->type		= DeltaExprLambda;
	DPTR(lambdaValueExpr)->SetTypeTag(TagMethod);
	DPTR(lambdaValueExpr)->GetTypeInfo().Set(TagMethod);
}

void ParseActions_Initialise (void) {

	LocalDataHandler::Initialise();
	DASSERT(ParseParms::InGlobalScope());
	DASSERT(!ParseParms::CurrScope().value());
}

////////////////////////////////////////////////////////////////////

void ParseActions_CleanUp (void) {
	LocalDataHandler::CleanUp();
	DeltaCompilerStringDestructor::GetSingleton().commit();
}

//------------------------------------------------------------------

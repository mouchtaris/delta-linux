// ParseParms.cpp.
// Parsing parameters for Delta compiler.
// ScriptFigher Project.
// A. Savidis, October 1999.
//

#include "DeltaByteCodeTypes.h"
#include "Symbol.h"
#include "ParseParms.h"
#include "ParseActions.h"
#include "CompilerAPI.h"

////////////////////////////////////////////////////////////////////

DeltaSymbol*				ParseParms::currFunctionStack[DELTA_MAXNESTED_FUNCTIONS];
util_i16					ParseParms::topFunc = -1;
util_ui16					ParseParms::globalDataSize = 0;
util_ui16					ParseParms::globalBlocks = 0;
bool						ParseParms::inFormalArgs = false;
util_ui16					ParseParms::currFuncSerial = DELTA_FUNCSERIAL_OF_GLOBAL_CODE;

NestedState					ParseParms::inLoop;
bool						ParseParms::productionMode = false;

NestedState					ParseParms::inAssertStmt;
NestedState					ParseParms::inTableExpr;
NestedState					ParseParms::currScope(DELTA_GLOBAL_SCOPE);	
std::list<util_ui32>*		ParseParms::unindexedElemOrder	= (std::list<util_ui32>*) 0;
std::list<DeltaExpr*>*		ParseParms::newTableStack		= (std::list<DeltaExpr*>*) 0;
std::list<DeltaSymboList>*	ParseParms::controlFlowInitialisationsStack =  (std::list<DeltaSymboList>*) 0;
std::list<std::string>*		ParseParms::namespacePath		= (std::list<std::string>*) 0;

static const char* operators[] = {
	"+",	
	"-",	
	"*",	
	"/",	
	"%",	
	">",	
	"<",	
	"!=",
	"==",
	">=",
	"<=",
	"=",	
	"()",
	"=()",
	"+_",
	"-_",
	"*_",
	"/_",
	"%_",
	">_",
	"<_",
	"!=_",
	"==_",
	">=_",
	"<=_",
	".",
	".=",
	"..",
	DELTA_TOSTRING_SYMBOLIC_NAME
};

////////////////////////////////////////////////////////////////////

void ParseParms::Initialise (void) {

	unew(unindexedElemOrder); 
	unew(newTableStack);
	unew(controlFlowInitialisationsStack);
	unew(namespacePath);
	DPTR(unindexedElemOrder)->push_back(0);

	currScope			= 0;
	topFunc				= -1;
	globalDataSize		= 0;
	globalBlocks		= 0;
	inFormalArgs		= 0;
	currFuncSerial		= 0;
	productionMode		= false;

	inAssertStmt.reset();
	inTableExpr.reset();
	inLoop.reset();
	currScope.reset(DELTA_GLOBAL_SCOPE);
}

void ParseParms::CleanUp (void) {
	udeleteunlessnull(unindexedElemOrder);
	udeleteunlessnull(newTableStack);
	udeleteunlessnull(controlFlowInitialisationsStack);
	udeleteunlessnull(namespacePath);
}

////////////////////////////////////////////////////////////////////

bool ParseParms::InProductionMode (void) 
	{ return productionMode; }

void ParseParms::SetInProductionMode (bool val) 
	{ productionMode = val; }

////////////////////////////////////////////////////////////////////

util_ui16 ParseParms::CurrLine (void) 
	{ return (util_ui16) DeltaCompiler::GetLine(); }

////////////////////////////////////////////////////////////////////

DeltaSymbol* ParseParms::CurrFunction (void) 
	{ return topFunc == -1 ? NIL_SYMBOL : currFunctionStack[topFunc]; }

bool ParseParms::IsOuterFunction (DeltaSymbol* func) {
	for (util_i16 i = topFunc; i != -1; --i)
		if (currFunctionStack[i] == func)
			return true;
	return false;
}

bool ParseParms::InMethod (void) {
	DeltaSymbol* currFunc = CurrFunction();
	return currFunc && DPTR(currFunc)->funcClass == DELTA_FUNCCLASS_METHOD;
}

bool ParseParms::IsOperator(const std::string& name) 
	{ return ustrpos(operators, uarraysize(operators), name.c_str()) != uarraysize(operators); }

////////////////////////////////////////////////////////////////////

util_ui16 ParseParms::GlobalDataSize (void) 
	{ return globalDataSize; }


void ParseParms::SetGlobalDataSize (util_ui16 n) 
	{ globalDataSize = n; }

util_ui16 ParseParms::IncGlobalBlocks (void) 
	{ return globalBlocks++; }

util_ui16 ParseParms::GetTotalGlobalBlocks (void) 
	{ return globalBlocks; }

////////////////////////////////////////////////////////////////////

bool ParseParms::InFormalArgs (void) 
	{ return inFormalArgs; }


void ParseParms::SetInFormalArgs (bool val) 
	{ inFormalArgs = val; }

////////////////////////////////////////////////////////////////////

util_ui16 ParseParms::CurrFuncSerial (void) {
	return currFuncSerial;
}

util_ui16 ParseParms::InsideFuncSerial (void) {
	return InFunction() ? CurrFunction()->serial + 1 : DELTA_FUNCSERIAL_OF_GLOBAL_CODE;
}

void ParseParms::IncFuncSerial (void) 
	{ ++currFuncSerial; }

////////////////////////////////////////////////////////////////////
// When entering a function, the loop counter is reset.
//
void ParseParms::PushFunction (DeltaSymbol* func) {
	currFunctionStack[++topFunc] = func;
	inLoop.save();
	inAssertStmt.save();
	inTableExpr.save();
	EnteringControlFlowStmt();
}

// Imlpicitly, the previous stored loop value is restored.
//
void ParseParms::PopFunction (void) {
	DASSERT(topFunc > -1);
	--topFunc;
	inLoop.restore();
	inAssertStmt.restore();
	inTableExpr.restore();
	ExitingControlFlowStmt(true, true);
}

bool ParseParms::InFunction (void) 
	{ return topFunc != -1; }

////////////////////////////////////////////////////////////////////

struct ResetInitialised : public std::binary_function<DeltaSymbol*, bool, void> {
	void operator()(DeltaSymbol* s, bool possibly) const
		{ s->ResetInitialised(possibly); }
};

struct SetInitialised : public std::unary_function<DeltaSymbol*, void> {
	void operator()(DeltaSymbol* s) const 
		{ s->SetInitialised(); }
};

struct SetPossiblyUninitialisedToSureUninitialised : public std::unary_function<DeltaSymbol*, void> {
	void operator()(DeltaSymbol* s) const 
		{ if (!s->IsInitialised()) s->SetSureUninitialised(); }
};

//*****************************

void ParseParms::EnteringControlFlowStmt (void) {
	DASSERT(controlFlowInitialisationsStack);
	DPTR(controlFlowInitialisationsStack)->push_front(DeltaSymboList());
}

void ParseParms::ExitingControlFlowStmt (bool pop, bool reset, bool possibly) {
	DASSERT(pop || reset);
	DASSERT(controlFlowInitialisationsStack && !DPTR(controlFlowInitialisationsStack)->empty());
	if (reset) {
		DeltaSymboList& l = DPTR(controlFlowInitialisationsStack)->front();
		std::for_each(
			l.begin(),
			l.end(),
			std::bind2nd(ResetInitialised(), possibly)
		);
	}
	if (pop)
		DPTR(controlFlowInitialisationsStack)->pop_front();
}

//*****************************

void ParseParms::AssumeToBeInitialised (DeltaSymbol* var) {

	DASSERT(controlFlowInitialisationsStack);

	if (!DPTR(controlFlowInitialisationsStack)->empty())	// In control flow stmt.
		if (!DPTR(var)->IsInitialised())					// Initialised in this stmt.
			 DPTR(controlFlowInitialisationsStack)->front().push_back(var);
}

////////////////////////////////////////////////////////////////////

bool ParseParms::InLoop (void) 
	{ return inLoop.value() != 0; }

void ParseParms::EnteringLoop (void) 
	{ inLoop.enter(); EnteringControlFlowStmt(); }

void ParseParms::ExitingLoop (ConditionValue condValue) {
	inLoop.exit(); 
	if (condValue == CondTrue)
		ExitingControlFlowStmt(true, false);	// Remain initialised
	else	// If condition is false we know we uninitialise for sure.
		ExitingControlFlowStmt(true, false, condValue != CondFalse);
}

//*****************************

void ParseParms::EnteringIf (void) 
	{ EnteringControlFlowStmt(); }

void ParseParms::ExitingIf (ConditionValue cond) {
	if (cond == CondTrue)
		ExitingControlFlowStmt(true, false);	// Remain initialised
	else // If condition is false we know we uninitialise for sure.
		ExitingControlFlowStmt(true, true, cond != CondFalse);
}

//*****************************

// The symbols are rest but remain in the list to be rechecked
// when handling the else part.

void ParseParms::ExitingIfFollowedByElse (void) 
	{ ExitingControlFlowStmt(false, true); }	

void ParseParms::EnteringElse (void)
	{ EnteringControlFlowStmt(); }

void ParseParms::ExitingElse (ConditionValue cond) {
	
	DASSERT(controlFlowInitialisationsStack);
	DASSERT(DPTR(controlFlowInitialisationsStack)->size() >= 2); // Both if and else

	DeltaSymboList else_l(DPTR(controlFlowInitialisationsStack)->front());
	DPTR(controlFlowInitialisationsStack)->pop_front();
	
	DeltaSymboList if_l(DPTR(controlFlowInitialisationsStack)->front());
	DPTR(controlFlowInitialisationsStack)->pop_front();

	if (cond == CondCantTell) {	// Uninitialise only those vars not initialised both in if and else parts.
		for (DeltaSymboList::iterator i = else_l.begin(); i != else_l.end(); ++i)
			if (std::find(if_l.begin(), if_l.end(), *i) == if_l.end())	// Not initialised in both
				(*i)->ResetInitialised();
	}
	else
	if (cond == CondTrue) {
		std::for_each(	// Set all symbols initialised in the if part as permanently initialised.
			if_l.begin(),
			if_l.end(),
			SetInitialised()
		);
		std::for_each(	// And set as sure uninitialised those initialised in the else part.
			else_l.begin(),
			else_l.end(),
			std::bind2nd(ResetInitialised(), false)
		);
	}
	else {
		// Here the if part symbols are 'possibly' uninitialised
		// and the else symbols are initialised. We only set the if
		// symbols as 'sure' uninitialised.

		DASSERT(cond == CondFalse);
		std::for_each(	// Set all symbols initialised in the if part as permanently initialised.
			if_l.begin(),
			if_l.end(),
			SetPossiblyUninitialisedToSureUninitialised()
		);
	}
}

////////////////////////////////////////////////////////////////////

void ParseParms::IncUnindexedElementOrder (void)
	{ ++DPTR(unindexedElemOrder)->front(); }

util_ui32 ParseParms::GetUnindexedElementOrder (void)
	{ return DPTR(unindexedElemOrder)->front(); }

void ParseParms::EnteringTableConstructor (DeltaExpr* table) {
	DPTR(unindexedElemOrder)->push_front(0);
	DPTR(newTableStack)->push_front(table);
}

void ParseParms::ExitingTableConstructor (void) {
	DASSERT(!DPTR(unindexedElemOrder)->empty() && !DPTR(newTableStack)->empty());
	DPTR(unindexedElemOrder)->pop_front();
	DPTR(newTableStack)->pop_front();
}

DeltaExpr* ParseParms::GetCurrConstructedTable (void) {
	DASSERT(!DPTR(newTableStack)->empty());
	return DPTR(newTableStack)->front();
}

////////////////////////////////////////////////////////////////////

void ParseParms::AppendToNamespacePath (const std::string& id) 
	{ DPTR(namespacePath)->push_back(id); }

void ParseParms::ClearNamespacePath (void)
	{ DPTR(namespacePath)->clear(); }

const ParseParms::NameList&	ParseParms::GetNamespacePath (void)
	{ return *DPTR(namespacePath); }

////////////////////////////////////////////////////////////////////

// Symbol.cpp.
// Symbol table management for the Delta compiler.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Change on the inactivation style of symbols, March 2009.
//

#include "DDebug.h"
#include "Symbol.h"
#include "ParseParms.h"
#include "ParseActions.h"
#include "DeltaCompErrorMsg.h"
#include "LocalDataHandling.h"
#include "DeltaCompErrorDefs.h"
#include "LibraryNamespaceHolder.h"
#include "uhash.h"
#include "ufunctors.h"

#include <algorithm>

//------------------------------------------------------------------

DeltaSymbol::DeltaSymbol (const std::string& _name, DeltaSymbolType type) {

	new (this) DeltaSymbol;											// Default construction needed to initialise all fields

	name	= _name;
	scope	= ParseParms::CurrScope().value();
	line	= ParseParms::CurrLine();
	myFunc	= ParseParms::CurrFunction();

	if (type == DeltaSymbol_StaticVar)
		isStatic = true;
	else
		isFormal = ParseParms::InFormalArgs();

	if (DELTA_SYMBOL_TYPE_IS_VARIABLE(type)) {
		if (GetMyFunction() && type != DeltaSymbol_StaticVar) {		// Function formal or local
			if (isFormal) {											// Formal argument
				offset				= myFunc->GetTotalFormals();
				blockId				= DELTA_MAIN_BLOCK_VALUE;		// Added in main function block block always
				initialisedCounter	= 1;
				DPTR(GetMyFunction())->AddFormal(name);
			}
			else {													// Block local
				blockId	= LocalDataHandler::GetCurrBlockId();
				LocalDataHandler::OnNewLocalVar(this);
			}
		}
		else {														// Global or static var
			blockId	= LocalDataHandler::GetCurrBlockId();
			if (!isStatic) {
				DASSERT(!GetMyFunction());							// Globals are always outside functions
				LocalDataHandler::OnNewLocalVar(this);
			}
		}
	}
	else
		initialisedCounter = 1;										// All non-vars are considered as initialised
}

/////////////////////////////////////////////////////////////////////

DeltaSymbol::~DeltaSymbol(){ 
	udeleteunlessnull(funcSig); 
	udeleteunlessnull(funcAccess);
	udeleteunlessnull(libFuncSigs); 
	udeleteunlessnull(funcVarInfo); 
	udeleteunlessnull(libVarInfo); 
	udeleteunlessnull(closureInfo); 
}

/////////////////////////////////////////////////////////////////////

void DeltaSymbol::FuncAccess::RecordInnerUse (DeltaExpr* funcExpr, DeltaSymbol* clientFunc) {

	DASSERT(!DPTR(func)->HasClosure() && DPTR(funcExpr)->sym == func);
	DASSERT(DPTR(clientFunc)->IsInnerFunctionOf(func));
	DASSERT(clientFunc == ParseParms::CurrFunction());

	DASSERT(
		std::find_if(
			innerUses.begin(),
			innerUses.end(),
			std::bind2nd(uequal_first<UseInfo>(), funcExpr)
		) == innerUses.end()
	);

	innerUses.push_back(
		UseInfo(funcExpr, clientFunc)
	);
}

	//**************************

void DeltaSymbol::FuncAccess::CreateFunctionVar (void) {
	DASSERT(!funcVar);
	DASSERT(ParseParms::CurrScope().value() == func->GetScope());	// We should be at the same scope as the function.
	funcVar =	DELTASYMBOLS.NewSymbol(
					uconstructstr(
						"%s%s", 
						DELTA_HIDDENCONSTVAR_NAME_PREFIX, 
						func->GetName().c_str()), 
						true
					);
}

/////////////////////////////////////////////////////////////////////

void DeltaSymbol::SetScope (util_ui16 _scope) {
	DASSERT(_scope < ParseParms::CurrScope().value());
	scope = _scope;
}

void DeltaSymbol::CheckUninitialised (void) const {	// Only for non-temporary variables.
	if (!initialisedCounter) {
		DASSERT(!IsTempVar());
		DELTACOMP_WARNING_UNINITIALIZED_USED(GetName().c_str(), possiblyUninitialised);
	}
}

/////////////////////////////////////////////////////////////////////

static const char* funcClassStrings[] = {
	(char*) 0,		//	DELTA_FUNCCLASS_NOTAFUNCTION		
	"function",		//	DELTA_FUNCCLASS_PROGRAMFUNCTION		
	"onevent",		//	DELTA_FUNCCLASS_ONEVENT				
	"method"		//	DELTA_FUNCCLASS_METHOD				
};

const std::string DeltaSymbol::GetFunctionClass (void) const {
	DASSERT(funcClass != DELTA_FUNCCLASS_NOTAFUNCTION);
	DASSERT(funcClass <= DELTA_FUNCCLASS_METHOD);
	return funcClassStrings[(util_ui8) funcClass];
}

/////////////////////////////////////////////////////////////////////

const std::string DeltaSymbol::GetFunctionReadableName (void) const {
	if (IsImportedFunctionVar())
		return GetImportedFuncVar()->GetFunctionReadableName();
	else
	if (funcClass == DELTA_FUNCCLASS_METHOD)
		if (methodName.empty())
			return "method" + GetName();
		else
		if (ParseParms::IsOperator(methodName))
			return "@operator" + methodName;
		else
			return "method_" + methodName;
	else
	if (IsAnonymousFunction())
		return "anonymous" + GetName();
	else
		return GetName();
}

/////////////////////////////////////////////////////////////////////

const std::string DeltaSymbol::GetTypeDescription (void) const {
	return	IsUserDefinedFunction()	?	"user-defined function" :
			IsUserDefinedConst()	?	"user-defined constant"	: 
			IsFormal()				?	"formal argument"		: 
			IsLibraryFunction()		?	"library function"		:	 
			IsLibraryConst()		?	"library const"			: "variable";
}

//------------------------------------------------------------------

DeltaSymbolTable* DeltaSymbolTable::singletonPtr = (DeltaSymbolTable*) 0;

void DeltaSymbolTable::SingletonCreate (void) 
	{ unew(singletonPtr); }

void DeltaSymbolTable::SingletonDestroy (void) 
	{ udelete(singletonPtr); }

DeltaSymbolTable::DeltaSymbolTable (void) {
	tempCount		=
	tempFuncCount	= 0;
	unullify(inactiveHead);
	unullify(inactiveTail);
	for (util_ui16 i = 0; i<DELTA_SYMBOL_TABLE_HASH_SIZE; ++i)
		Table[i] = NIL_SYMBOL;
}

DeltaSymbolTable::~DeltaSymbolTable() 
	{ CleanUp(); }

///////////////////////////////////////////////////////////////////
// Storage is not needed in case functions are defined, since, their
// address, is explicitly taken within a variable.
//
DeltaSymbol* DeltaSymbolTable::NewSymbol (const std::string& name, bool storage) {

	DASSERT(!IsTempVar(name));

	DeltaSymbol* sym = DNEWCLASS(DeltaSymbol, (name, storage ? DeltaSymbol_VarInCurrScope  : DeltaSymbol_NotAVar));
	Install(sym);

	return sym;
}

///////////////////////////////////////////////////////////////////
// This creates static variables. Static variables can
// NEVER be temporary.
//
DeltaSymbol* DeltaSymbolTable::NewStatic (const std::string& name) {

	DASSERT(!IsTempVar(name));

	// The block id for static vars is actually their logical
	// order in the source code, starting from 0. Then, upon code
	// generation, statics are assigned to block 0, while their
	// block id is appended to their name (uniqueness).

	DeltaSymbol* sym = DNEWCLASS(DeltaSymbol, (name, DeltaSymbol_StaticVar));
	DPTR(sym)->blockId = (util_ui16) staticVars.size();
	staticVars.push_back(sym);
	Install(sym);

	return sym;
}

///////////////////////////////////////////////////////////////////

const std::string DeltaSymbolTable::NewTempName (void) 
	{ return uconstructstr("%s%u", DELTA_TEMPVAR_NAME_PREFIX, tempCount++); }

void DeltaSymbolTable::ResetTemp (void) 
	{ tempCount = 0;  }

const std::string DeltaSymbolTable::NewTempFuncName (bool isMethod)
	{ return uconstructstr("%s%u", isMethod ? DELTA_ANONYMOUS_METHOD_NAME_PREFIX : DELTA_ANONYMOUS_FUNC_NAME_PREFIX, tempFuncCount++); }

void DeltaSymbolTable::ResetFuncTemp (void) 
	{ tempFuncCount = 0;  }

///////////////////////////////////////////////////////////////////
// Temp symbols MUST be only be created at current scope.
//
DeltaSymbol* DeltaSymbolTable::NewTemp (bool storage) {

	std::string		name	= NewTempName();
	DeltaSymbol*	tmp		= Lookup(name, ParseParms::CurrScope().value());

	if (!tmp) {
		tmp = DNEWCLASS(DeltaSymbol, (name, storage ? DeltaSymbol_VarInCurrScope  : DeltaSymbol_NotAVar));
		tmp->SetInitialised();	// Temps assumed directly as initialised.
		Install(tmp);
	}

	return tmp;
}


///////////////////////////////////////////////////////////////////

void DeltaSymbolTable::PushAndResetTempCounter (void) {
	tempCounterStack.push(tempCount);
	ResetTemp();
}

void DeltaSymbolTable::PopAndRestoreTempCounter (void) {
	DASSERT(!tempCounterStack.empty());
	tempCount = tempCounterStack.top();
	tempCounterStack.pop();
}

//------------------------------------------------------------------

void DeltaSymbolTable::Install (DeltaSymbol* sym, bool installAtOuterScope) {

	DeltaSymbol* p = Lookup(DPTR(sym)->GetName());
	if (!p)
		p = DELTANAMESPACES.LookupFunctionOrConst(DPTR(sym)->GetName());
	bool error = false;

	if (p) {
		if (DPTR(p)->isUserDefinedFunc && p->IsOfSameScopeAs(sym->scope))
			{ DELTACOMP_ERROR_REDEFINITION_WAS_FUNCTION(p); error = true; }
		else
		if (p->IsLibraryConst())
			{ DELTACOMP_ERROR_REDEFINITION_WAS_LIBCONST(p); error = true; }
		else
		if (p->IsLibraryFunction())
			{ DELTACOMP_ERROR_REDEFINITION_WAS_LIBFUNCTION(p); error = true; }
		else
		if (p->IsOfSameScopeAs(sym->scope))
			{ DELTACOMP_ERROR_REDEFINITION_WAS_VAR(p); error = true; }
	}

	if (!error) {

		DASSERT(installAtOuterScope || DPTR(sym)->GetScope() <= ParseParms::CurrScope().value());

		util_ui32 pos = uhash(sym->GetName(), DELTA_SYMBOL_TABLE_HASH_SIZE);
		DeltaSymbol* bucket = Table[pos];

		DASSERT(
			!bucket || 
			(!installAtOuterScope || DPTR(sym)->IsOfSameOrOuterScopeThan(bucket->GetScope())) ||
			(installAtOuterScope || DPTR(sym)->IsOfSameOrInnerScopeThan(bucket->GetScope()))
		);

		if (!installAtOuterScope || !bucket	|| DPTR(sym)->IsOfSameOrInnerScopeThan(bucket->GetScope())) {
			sym->next	= bucket;
			Table[pos]	= sym;
		}
		else {	// Install at a specific outer (earlier) scope.
			DeltaSymbol* prev = (DeltaSymbol*) 0;
			while (bucket && DPTR(sym)->IsOfOuterScopeThan(bucket->GetScope()))
				prev = bucket, bucket = DPTR(bucket)->next;
			DASSERT(prev && (!bucket || DPTR(bucket)->IsOfSameOrOuterScopeThan(sym->GetScope())));
			sym->next	= bucket;
			prev->next	= sym;
		}

		if (sym->IsGlobal() && !sym->IsStatic())
			globalVars.push_back(sym);
	}

	allSymbols.push_back(sym);
}

//------------------------------------------------------------------
// Here we simply register the closure function var symbol, but never install it
// to the symbol table. This is harmless as closure func vars never get storage
// from the stack while they are explicitly looked inside the closure table.

void DeltaSymbolTable::RegisterClosureFunctionVar (DeltaSymbol* funcVar, DeltaSymbol* clientFunc) {

	DASSERT(clientFunc && DPTR(funcVar)->IsClosureVarAccess());
	DASSERT(!DPTR(funcVar)->next);																// Symbol not linked in the table.
	DASSERT(std::find(allSymbols.begin(), allSymbols.end(), funcVar) == allSymbols.end());		// Symbol not registered.

	DPTR(funcVar)->myFunc	= clientFunc;
	DPTR(funcVar)->scope	= clientFunc->GetScope() + DELTA_FUNCTION_MAINBLOCK_SCOPE_OFFSET;
	DPTR(funcVar)->line		= clientFunc->line;

	allSymbols.push_back(funcVar);
}

//------------------------------------------------------------------

void DeltaSymbolTable::Uninstall (const std::string& name) {

	util_ui32 pos;
	DeltaSymbol* p = Table[pos = uhash(name, DELTA_SYMBOL_TABLE_HASH_SIZE)];
	DASSERT(p);

	if (DPTR(p)->GetName() ==  name) {	// First in bucket list.
		Table[pos] = p->next;
		allSymbols.remove(p);
		DDELETE(p);
	}
	else {
		// Find element before it, and disconnect it from bucket list.

		DeltaSymbol* q;
		for (q = p->next; q && DPTR(q)->GetName()!= name; q = q->next, p = p->next)
			;
		DASSERT(p);

		DPTR(p)->next = DPTR(q)->next;

		allSymbols.remove(q);
		DDELETE(q);
	}
}

//------------------------------------------------------------------

DeltaSymbol* DeltaSymbolTable::Lookup (const std::string& name) {

	for (DeltaSymbol* p = Table[uhash(name, DELTA_SYMBOL_TABLE_HASH_SIZE)]; p; p = p->next)
		if (p->GetName() == name)
			return p;

	return NIL_SYMBOL;
}

//------------------------------------------------------------------

DeltaSymbol* DeltaSymbolTable::Lookup (const std::string& name, util_ui16 scope) {

	// Once we get to an outer scope no way to return to an inner scope.
	
	for (	DeltaSymbol* p = Table[uhash(name, DELTA_SYMBOL_TABLE_HASH_SIZE)]; 
			p && p->IsOfSameOrInnerScopeThan(scope); 
			p = p->next	)
			if (p->GetName() == name && p->IsOfSameScopeAs(scope))
				return p;

	return NIL_SYMBOL;
}

//------------------------------------------------------------------

void DeltaSymbolTable::CleanUp (void) {

	for (util_ui16 i = 0; i < DELTA_SYMBOL_TABLE_HASH_SIZE; ++i)
		unullify(Table[i]);
	unullify(inactiveHead);
	unullify(inactiveTail);

	staticVars.clear();
	globalVars.clear();

	std::for_each(
		allSymbols.begin(),
		allSymbols.end(),
		uptrdestructorfunctor<DeltaSymbol*>()
	);
	allSymbols.clear();

	ResetTemp();
	ResetFuncTemp();
}

//------------------------------------------------------------------

void DeltaSymbolTable::ValidateBucket (DeltaSymbol* bucket) const {
	while (bucket)
		bucket = DPTR(bucket)->next;
}

void DeltaSymbolTable::ValidateAll (void) const {
	
	for (util_ui16 i = 0; i < DELTA_SYMBOL_TABLE_HASH_SIZE; ++i)
		ValidateBucket(Table[i]);

	ValidateBucket(inactiveHead);
	ValidateBucket(inactiveTail);	// Should pass as well.

	for (	std::list<DeltaSymbol*>::const_iterator j = allSymbols.begin(); 
			j != allSymbols.end(); 
			++j		)

			DPTR(*j);
}

//------------------------------------------------------------------
// Once we meet something of smaller scope value (outer scope), it means
// we can never meet an active symbol of larger scope value (inner scope).
//
void DeltaSymbolTable::InactivateSymbols (util_ui16 scope) {

	for (util_ui16 i = 0; i < DELTA_SYMBOL_TABLE_HASH_SIZE; ++i) {

		DeltaSymbol* from = Table[i];
		DASSERT(!from || from->IsOfSameOrOuterScopeThan(scope));	// Nothing alive in inner scope.

		if (from && DPTR(from)->IsOfSameScopeAs(scope)) {

			DeltaSymbol* to = from;

			while (DPTR(to)->next && DPTR(to->next)->IsOfSameScopeAs(scope))
				to = to->next;

			DASSERT(!DPTR(to)->next || DPTR(to->next)->IsOfOuterScopeThan(scope));	// Nothing alive in inner scope.

			Table[i] = to->next;
			AppendInactiveSymbols(from, to);
		}
	}
}

///////////////////////////////////////////////////////////////////

void DeltaSymbolTable::AppendInactiveSymbols (DeltaSymbol* from, DeltaSymbol* to) {
	if (!inactiveHead) {
		DASSERT(!inactiveTail);
		inactiveHead = from;
		inactiveTail = to;
	}
	else {
		DASSERT(inactiveTail);
		DPTR(inactiveTail)->next	= from;
		inactiveTail				= to;
		unullify(DPTR(inactiveTail)->next);
	}
}

///////////////////////////////////////////////////////////////////
// Install an array of names as external functions.
// This array is static. Not to be ptr checked.
//
void DeltaSymbolTable::InstallLibraryFunctions (const std::list<UPTR(const char*)>* externFuncs) {

	DASSERT(externFuncs);

	for (	std::list<UPTR(const char*)>::const_iterator funcs = externFuncs->begin();
			funcs != externFuncs->end();
			++funcs	)
		
			for (UPTR(const char*) func = *funcs; *func; ++func) {
				DeltaSymbol* sym = NewSymbol(*func, false);
				DPTR(sym)->isLibraryFunc = true;
			}
}

///////////////////////////////////////////////////////////////////

void DeltaSymbolTable::ForeachGlobalVar (void (*f)(DeltaSymbol*, void*), void* closure) {
	std::for_each(
		globalVars.begin(),
		globalVars.end(),
		std::bind2nd(std::pointer_to_binary_function<DeltaSymbol*, void*, void>(f), closure)
	);
}

///////////////////////////////////////////////////////////////////

static void ShiftOffsetUnlessReturnValue (DeltaSymbol* var, void* offset) {
	DASSERT(var->IsGlobal());
	if (DPTR(var)->GetOffset() != DELTA_RETVALUE_OFFSET) 
		DPTR(var)->ShiftOffset((util_ui32) offset);
}

void DeltaSymbolTable::ShiftOffsetsOfGlobalVariablesAfterStatics (void) 
	{ ForeachGlobalVar(&ShiftOffsetUnlessReturnValue, (void*) staticVars.size()); }

///////////////////////////////////////////////////////////////////

void DeltaSymbolTable::GiveOffsetsToStaticVariables (util_ui16 totalGlobals) {

	// The static vars go first, after the return value.

	util_ui16 offset = DELTA_RETVALUE_OFFSET + 1;

	std::for_each(
		staticVars.begin(),
		staticVars.end(),
		std::bind2nd(
			GiveStaticVarOffsetFunctor(),
			&offset
		)
	);
}

//------------------------------------------------------------------

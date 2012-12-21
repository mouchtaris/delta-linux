// StagedSymbolTable.cpp
// SymbolTable with staging and pure function information.
// ScriptFighter Project.
// Y. Lilis, May 2011.
//

#include "StagedSymbolTable.h"
#include "ustrings.h"

/////////////////////////////////////////////////////////

bool Symbol::GetSymbolsRecursively(SymbolSet& functions, SymbolSet& symbols) {
	if (!IsCompleted())
		return false;
	else {
		if (functions.find(this) == functions.end()) {
			functions.insert(this);
			for (SymbolSet::const_iterator i = referrers.begin(); i != referrers.end(); ++i)
				symbols.insert(*i);
			for (SymbolSet::const_iterator i = referTo.begin(); i != referTo.end(); ++i)
				if ((*i)->PurenessPending() && !(*i)->GetSymbolsRecursively(functions, symbols))
					return false;
		}

		return true;
	}
}

void Symbol::CheckCyclicDependencies(void) {
	SymbolSet functions, symbols;
	if (GetSymbolsRecursively(functions, symbols)) {
		bool cyclicDependency = true;
		for (SymbolSet::const_iterator i = symbols.begin(); i != symbols.end(); ++i) {
			bool found = false;
			for (SymbolSet::const_iterator j = functions.begin(); j != functions.end(); ++j) {
				SymbolSet& s = (*j)->referTo;
				if (s.find(*i) != s.end()) {
					found = true;
					break;
				}
			}
			if (!found)		//no cyclic dependency
				return;
		}
		for (SymbolSet::iterator i = functions.begin(); i != functions.end(); ++i)
			(*i)->pureness = Pure;	//No need to determine referrers, as they should already be in the update list
	}
}

void Symbol::DetermineReferrerPureness(void)
	{ std::for_each(referrers.begin(), referrers.end(), std::mem_fun(&Symbol::DeterminePureness)); }

//------------------------------------------------------------------

void Symbol::SetPure (bool status) {
	pureness = status ? Pure : Impure;
	if (IsImpure() && !crossStageUsages.empty() && onImpureCrossStageAccess)
		for (UsageList::const_iterator i = crossStageUsages.begin(); i != crossStageUsages.end(); ++i)
			onImpureCrossStageAccess(this, i->first);
	crossStageUsages.clear();
	DetermineReferrerPureness();
}

void Symbol::ReferTo (Symbol* function) {
	DASSERT(IsFunction() && function->IsFunction());
	referTo.insert(function);
	function->referrers.insert(this);
	if (function->IsImpure() && !function->DependsOnlyOnFunctionClosures(this))
		SetPure(false);
}

void Symbol::DeterminePureness(void) {
	if (PurenessPending()) {
		bool pending = false;
		for (SymbolSet::const_iterator i = referTo.begin(); i != referTo.end(); ++i) {
			if ((*i)->IsImpure() && !(*i)->DependsOnlyOnFunctionClosures(this)) {
				SetPure(false);
				return;
			}
			else if ((*i)->PurenessPending())
				pending = true;
		}
		if (pending)
			CheckCyclicDependencies();
		else
			SetPure(true);
	}
}

//------------------------------------------------------------------

void Symbol::AddDependency(DependencyType type, Symbol* symbol)
	{ dependencies.push_back(DependencyPair(type, symbol)); }

bool Symbol::DependsOnlyOnFunctionClosures(Symbol* function) {
	for (DependencyList::const_iterator i = dependencies.begin(); i != dependencies.end(); ++i) {
		DASSERT(i->second || i->first == NonLocal);
		if (i->first != Closure || i->second->MyFunction() != function)
			return false;
	}
	return true;
}

//------------------------------------------------------------------

static const char* typeStrings[] = { "variable", "function", "const" };

const char* Symbol::GetTypeStr (SymbolType type) {
	DASSERT(type >= 0 && (util_ui32) type < uarraysize(typeStrings));
	return typeStrings[type];
}

void Symbol::Print(void) {
	std::string extra;
	if (IsVariable() && myFunction)
		extra = " (defined in " + myFunction->GetName() + ")";
	else if (IsFunction()) {
		extra = std::string(" (") + (IsPure() ? "pure" : IsImpure() ? "impure" : "pending") + ")";
		if (ShouldGlobalize())
			extra += ", shouldGlobalize";
		else if (IsGlobalized())
			extra += ", globalized";
	}

	printf("%s, %s, scope %d, line %d%s\n", name.c_str(), GetTypeStr(type), scope, line, extra.c_str());
}

/////////////////////////////////////////////////////////

void StagedSymbolTable::Hide(util_ui32 stage, util_ui32 scope) {
	SymbolMap& symbols = stages[stage];
	for (SymbolMap::iterator i = symbols.begin(); i != symbols.end(); ++i)
		if (i->second->GetScope() == scope) {
			i->second->SetActive(false);
			if (onHideSymbol)
				onHideSymbol(i->second);
		}
}

//------------------------------------------------------------------

void StagedSymbolTable::EnterStage (void) {
	stageScopeStack.push_back(0);
	PushFunction((Symbol*)0);
}

void StagedSymbolTable::LeaveStage (void) {
	DASSERT(!functions.empty() && functions.back().first > 0 && !functions.back().second);
	PopFunction();
	DASSERT(!stageScopeStack.empty());
	stageScopeStack.pop_back();
}

//------------------------------------------------------------------

void StagedSymbolTable::EnterScope(void) { ++stageScopeStack.back(); }
void StagedSymbolTable::ExitScope (bool shouldHide) {
	if (shouldHide)
		Hide(GetStage(), stageScopeStack.back());
	--stageScopeStack.back();
}

//------------------------------------------------------------------

void StagedSymbolTable::PushFunction (Symbol* function) {
	DASSERT(!function || function->IsFunction());	//allow null entries to specify not in a function
	functions.push_back(StageSymbolPair(GetStage(), function));
}
void StagedSymbolTable::PopFunction	(void){
	DASSERT(!functions.empty());
	functions.pop_back();
}

Symbol* StagedSymbolTable::CurrFunction (void) const
	{ return functions.empty() ? (Symbol*) 0 : functions.back().second; }

Symbol* StagedSymbolTable::PrevFunction (void) const {
	if (functions.size() > 1												&&
		functions.back().second												&&
		functions[functions.size() - 2].first == functions.back().first
	)
		return functions[functions.size() - 2].second;
	else
		return (Symbol*) 0;
}

Symbol* StagedSymbolTable::PreviousStageCurrFunction (void) const {
	if (functions.size() > 1												&&
		!functions.back().second											&&
		functions[functions.size() - 2].first == functions.back().first - 1
	)
		return functions[functions.size() - 2].second;
	else
		return (Symbol*) 0;
}

//------------------------------------------------------------------

const std::string StagedSymbolTable::NewTempFuncName(void) {
	static util_ui32 tempFuncCount = 0;
	return uconstructstr("_f%u", tempFuncCount++);
}

Symbol* StagedSymbolTable::NewSymbol(const std::string& name, SymbolType type, util_ui32 line, TreeNode* node) {
	Symbol* symbol = DNEWCLASS(Symbol, (name, type, GetStage(), GetScope(), line, node));
	if (onImpureCrossStageAccess)
		symbol->SetImpureCrossStageAccessCallback(onImpureCrossStageAccess);
	if (CurrFunction())
		symbol->SetMyFunction(CurrFunction());
	return symbol;
}

void StagedSymbolTable::Install(Symbol* symbol)
	{ stages[GetStage()].insert(std::make_pair(symbol->GetName(), symbol)); }

//------------------------------------------------------------------

Symbol* StagedSymbolTable::Lookup(const std::string& id) {
	std::list<util_ui32> searchList(1, 0);
	util_ui32 stage = GetStage();
	if (stage > 1)
		searchList.push_front(stage - 1);
	if (stage > 0)
		searchList.push_front(stage);

	for (std::list<util_ui32>::const_iterator i = searchList.begin(); i != searchList.end(); ++i)
		if (Symbol *symbol = Lookup(id, *i))
			return symbol;
	return (Symbol *) 0;
}

Symbol* StagedSymbolTable::Lookup(const std::string& id, util_ui32 stage) {
	DASSERT(stage < stageScopeStack.size());
	for (util_ui32 i = stageScopeStack[stage]; i; --i)
		if (Symbol *symbol = Lookup(id, stage, i))
			return symbol;
	return Lookup(id, stage, 0);
}

Symbol* StagedSymbolTable::Lookup(const std::string& id, util_ui32 stage, util_ui32 scope) {
	SymbolMap& symbols = stages[stage];
	IteratorPair ids = symbols.equal_range(id);
	for (SymbolMap::const_iterator i = ids.first; i != ids.second; ++i)
		if (i->second->IsActive() && i->second->GetScope() == scope)
			return i->second;
	return (Symbol *) 0;
}

//------------------------------------------------------------------

void StagedSymbolTable::Print(void) {
	for (StageSymbolMap::iterator i = stages.begin(); i != stages.end(); ++i) {
		printf("STAGE %d\n", i->first);
		SymbolMap& symbols = i->second;
		for (SymbolMap::iterator j = symbols.begin(); j != symbols.end(); ++j)
			j->second->Print();
		printf("\n");
	}
}

//------------------------------------------------------------------

StagedSymbolTable::StagedSymbolTable(void) { stageScopeStack.push_back(0); }

StagedSymbolTable::~StagedSymbolTable() {
	for (StageSymbolMap::iterator i = stages.begin(); i != stages.end(); ++i) {
		SymbolMap& symbols = i->second;
		for (SymbolMap::iterator j = symbols.begin(); j != symbols.end(); ++j)
			DDELETE(j->second);
	}
}

/////////////////////////////////////////////////////////

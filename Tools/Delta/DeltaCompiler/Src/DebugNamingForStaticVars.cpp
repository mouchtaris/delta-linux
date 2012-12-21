// DebugNamingForStaticVars.cpp
// ScriptFigher Project.
// A. Savidis, May 2006.
//

#include "DebugNamingForStaticVars.h"
#include "ustrings.h"

/////////////////////////////////////////////////////////////////
	
DebugNamingForStaticVars::DebugNamingForStaticVars (void) {
	haveProducedNames = false; 
}

DebugNamingForStaticVars::~DebugNamingForStaticVars() {
	staticVars.clear();
	debugNames.clear();
}

/////////////////////////////////////////////////////////////////
// The recursion will produce the name putting the outer function name
// BEFORE the inner name. After a function name an "::" is put.
//
const std::string DebugNamingForStaticVars::FullFunctionPathName (DeltaSymbol* sym) const {
	if (!sym->GetMyFunction())
		return "";
	else
		return	FullFunctionPathName(sym->GetMyFunction()) + 
				DPTR(sym->GetMyFunction())->GetFunctionReadableName() + 
				DELTA_LIBRARYNAMESPACE_SEPARATOR ;
}

const std::string DebugNamingForStaticVars::FullFunctionName (DeltaSymbol* sym) const
	{ return FullFunctionPathName(sym) + sym->GetFunctionReadableName() + DELTA_LIBRARYNAMESPACE_SEPARATOR; }

//*****************************

void DebugNamingForStaticVars::AddStaticVar (DeltaSymbol* sym) {

	std::string func = FullFunctionPathName(sym);
	StaticMap::iterator pos = staticVars.find(func);

	// In case this is the first static var with this qualified name,
	// we make a new entry.
	//
	if (pos == staticVars.end())
		staticVars[func].push_back(StaticEntry(sym, 0));
	else {

		// We iterate in the list, trying to identify also
		// the logical order of the static var.
		//
		std::list<StaticEntry>& vars = pos->second;
		util_ui16 n = 0;

		for (	std::list<StaticEntry>::iterator i= vars.begin();
				i != vars.end();
				++i	)
			
				// When a static var is alone in a scope, it gets 0.
				// Else, it gets the logical successive number starting
				// counting by 1.
				//
				if (DPTR(i->first)->GetName() == DPTR(sym)->GetName()) {
					if (!i->second)
						i->second = 1;
					n = i->second + 1;
				}

		vars.push_back(StaticEntry(sym, n));
	}
}

//*****************************

void DebugNamingForStaticVars::ProduceDebugNamesForStaticVars (void) {

	DASSERT(!haveProducedNames);
	DASSERT(debugNames.empty());

	haveProducedNames = true;

	// We iterate per function, assuming "" also as a null
	// function for globally defined statics. All static variables
	// get the "static." standard prefix.
	//
	for (	StaticMap::iterator i = staticVars.begin();
			i != staticVars.end();
			++i	) {

			std::string func = uconstructstr("%s%s", DELTA_STATICVAR_NAME_PREFIX, i->first.c_str());
			std::list<StaticEntry>& vars = i->second;

			for (	std::list<StaticEntry>::iterator j = vars.begin();
					j != vars.end();
					++j	) {

					// Variables with 0 index are uniquely named in their functions.
					// Else, they are ordered according to presence, so they are given
					// an appearence order suffix in their qualified name.
					//
					std::string name;

					if (!j->second) // First appearing.
						name = j->first->GetName();
					else // All the rest with the same name!
						name = uconstructstr("%s%d", j->first->GetName().c_str(), j->second);

					debugNames[j->first] = func + name;
			}
	}
}

//*****************************

const std::string DebugNamingForStaticVars::GetDebugName (DeltaSymbol* sym) const {

	DASSERT(haveProducedNames);
	DASSERT(!debugNames.empty());

	std::map<DeltaSymbol*, std::string>::const_iterator pos = debugNames.find(sym);
	DASSERT(pos != debugNames.end());

	return pos->second;
}

//*****************************

void DebugNamingForStaticVars::CleanUp (void) {
	haveProducedNames = false;
	staticVars.clear();
	debugNames.clear();
}

/////////////////////////////////////////////////////////////////

// DebugNamingForStaticVars.h
// External debug naming for static variables, so that
// programmers can easily refer to static vars with the debug
// expression evaluator, using unique names.
// ScriptFigher Project.
// A. Savidis, May 2006.
//

#ifndef	DEBUGNAMINGFORSTATICVARS_H
#define	DEBUGNAMINGFORSTATICVARS_H

#include "DeltaByteCodeTypes.h"
#include "Symbol.h"
#include "utypes.h"

#include <list>
#include <map>
#include <functional>
#include <string>

/////////////////////////////////////////////////////////////

class DebugNamingForStaticVars {	// Singleton.

	private:
	static DebugNamingForStaticVars* singletonPtr;

	typedef std::pair<DeltaSymbol*, util_ui16>				StaticEntry;
	typedef std::map<std::string, std::list<StaticEntry> >	StaticMap;

	StaticMap										staticVars;
	std::map<DeltaSymbol*, std::string>				debugNames;
	bool											haveProducedNames;

	//***********************************

	public:
	const std::string			FullFunctionPathName (DeltaSymbol* sym) const;
	const std::string			FullFunctionName (DeltaSymbol* sym) const;
	void						AddStaticVar (DeltaSymbol* sym);
	void						ProduceDebugNamesForStaticVars (void);
	const std::string			GetDebugName (DeltaSymbol* sym) const;

	void						CleanUp (void);

	static void					SingletonCreate (void);								
	static void					SingletonDestroy (void);
	static DebugNamingForStaticVars*	
								GetPtr (void) 
									{ DASSERT(singletonPtr); return singletonPtr; }

	private:
	DFRIENDDESTRUCTOR()
	DebugNamingForStaticVars (void);
	~DebugNamingForStaticVars();
};

/////////////////////////////////////////////////////////////

#define	DEBUGSTATICS GetDebugNamingForStaticVars()

inline DebugNamingForStaticVars& GetDebugNamingForStaticVars (void) {
	DASSERT(DebugNamingForStaticVars::GetPtr());
	return *DebugNamingForStaticVars::GetPtr();
}

#endif	// Do not add stuff beyond this point.

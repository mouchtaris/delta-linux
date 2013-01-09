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

class DebugNamingForStaticVars {

	private:
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

	DebugNamingForStaticVars (void);
	~DebugNamingForStaticVars();
};

/////////////////////////////////////////////////////////////

#define DEBUGSTATICS_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DebugNamingForStaticVars)))

#define DEBUGSTATICS	DEBUGSTATICS_EX(GET_COMPONENT_DIRECTORY())
	
/////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

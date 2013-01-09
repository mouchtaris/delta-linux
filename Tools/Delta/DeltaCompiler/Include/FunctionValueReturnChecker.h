// FunctionValueReturnChecker.h 
// Checking if all control paths of a function return a value.
// ScriptFighter Project.
// A. Savidis, May 2009.
//

#ifndef	FUNCTIONVALUERETURNCHECKER_H
#define	FUNCTIONVALUERETURNCHECKER_H

#include "CompilerComponentDirectory.h"
#include "InterCode.h"
#include <list>
#include <map>

/////////////////////////////////////////////////////////////

class FunctionValueReturnChecker : public ucomponentdirectoryclient {

	private:
	typedef std::list<DeltaQuadAddress>			MarkedList;
	typedef std::map<DeltaQuadAddress, bool>	MarkedMap;
	bool		returnExprFound;
	MarkedList	marked;
	MarkedMap	results;
	
	void				Mark (DeltaQuadAddress i) 
							{ marked.push_back(i); }

	bool				IsResult (DeltaQuadAddress i) const
							{ return results.find(i) != results.end(); }
	bool				IsMarked (DeltaQuadAddress i) const 
							{  return std::find(marked.begin(), marked.end(), i) != marked.end(); }
	
	void				ClearMarked (void) 	
							{ marked.clear();}
	void				ClearResults (void) 	
							{ results.clear(); }

	void				SetReturnExprFound (void) 
							{ returnExprFound = true; }

	void				ResetReturnExprFound (void) 
							{ returnExprFound = false; }

	bool				ReturnExprWasFound (void) const
							{ return returnExprFound; }

	bool				Check (DeltaCodeAddress i);

	/////////////////////////////////////////////////////////////

	public:
	void				CheckFunction (DeltaCodeAddress end);

	FunctionValueReturnChecker (ucomponentdirectory* directory) :
		ucomponentdirectoryclient(directory), returnExprFound(false) {}
	~FunctionValueReturnChecker(){}
};

/////////////////////////////////////////////////////////////

#define RETVALCHECKER_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), FunctionValueReturnChecker)))

#define RETVALCHECKER	RETVALCHECKER_EX(GET_COMPONENT_DIRECTORY())

/////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

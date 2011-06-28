// FunctionValueReturnChecker.h 
// Checking if all control paths of a function return a value.
// ScriptFighter Project.
// A. Savidis, May 2009.
//

#ifndef	FUNCTIONVALUERETURNCHECKER_H
#define	FUNCTIONVALUERETURNCHECKER_H

#include "usingleton.h"
#include "InterCode.h"

#include <list>
#include <stack>

/////////////////////////////////////////////////////////////

class FunctionValueReturnCheckerClass {

	private:
	typedef std::list<DeltaQuadAddress>	MarkedList;
	bool		returnExprFound;
	MarkedList	marked;
	
	void				Mark (DeltaQuadAddress i) 
							{ marked.push_back(i); }

	bool				IsMarked (DeltaQuadAddress i) const 
							{  return std::find(marked.begin(), marked.end(), i) != marked.end(); }
	
	void				ClearMarked (void) 	
							{ marked.clear(); }

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

	FunctionValueReturnCheckerClass (void) : returnExprFound(false){}
	~FunctionValueReturnCheckerClass(){}
};

/////////////////////////////////////////////////////////////

class FunctionValueReturnChecker : public usingleton<FunctionValueReturnCheckerClass>{};

#define	RETVALCHECKER \
			FunctionValueReturnChecker::GetSingleton()

/////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

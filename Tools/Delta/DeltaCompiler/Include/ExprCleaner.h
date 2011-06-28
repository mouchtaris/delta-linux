// AS OF DECEMBER 5, 2006, THIS FILE IS NOT LONGER USED.
// ExprCleaner.hh
// A way to release memory centrally for various items
// dynamically allocated in DeltaCompiler.
// ScriptFighter Project.
// A. Savidis, October 1999.
//


#ifndef	EXPRCLEANER_HH
#define	EXPRCLEANER_HH

#include "DDebug.h"
#include <list>

struct DeltaExpr;

class DeltaExprCleaner {
	
	private:
	static DeltaExprCleaner*	singletonPtr;

	std::list<DeltaExpr*>		exprs;
	bool						destroying;

	public:
	void						Register (DeltaExpr* expr);
	void						Cancel (DeltaExpr* expr);
	void						CleanUp (void);
	void						ValidateAll (void);

	static void					SingletonCreate (void);						
	static void					SingletonDestroy (void);

	static DeltaExprCleaner*	GetPtr (void) 
									{ DASSERT(singletonPtr); return singletonPtr; }
	private:
	DFRIENDDESTRUCTOR()
	DeltaExprCleaner(void): destroying(false) {}
	~DeltaExprCleaner(){}
};

#define	EXPRCLEANER	GetExprCleaner()

inline DeltaExprCleaner& GetExprCleaner (void) {
	DASSERT(DeltaExprCleaner::GetPtr());
	return *DeltaExprCleaner::GetPtr();
}

#endif	// Do not add stuff beyond this point.
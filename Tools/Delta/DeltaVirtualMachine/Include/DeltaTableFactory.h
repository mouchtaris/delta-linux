// DeltaTableFactory.h
// Factory and recycler for delta tables.
// ScriptFighter Project.
// A. Savidis, February 2006.
//

#ifndef	DELTATABLEFACTORY_H
#define	DELTATABLEFACTORY_H

#include "urecyclefactory.h"
#include "usingleton.h"
#include "DeltaTable.h"

////////////////////////////////////////////////////////////
// We always create tables via the factory.

#define	DELTATABLE_FACTORYCLASS								\
		usingleton<											\
			urecyclefactory<								\
				DeltaTable,									\
				uvoidconstructorarguments,					\
				uvoidplacementconstructor<DeltaTable>		\
			>												\
		>

class DVM_CLASS DeltaTableFactory : public DELTATABLE_FACTORYCLASS {
	public:
	static	DeltaTable* New (void)
				{ return GetSingleton().Get(uvoidconstructorarguments()); }
	static void	Free (DeltaTable* obj)
		{ GetSingleton().UnUse(obj); }
	static void	CollectAllUncollected (void);
};

////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
// DeltaArgumentsRecycler.h
// Arguments are delta values created dynamically with each
// push instruction. So, it is critical to ensure the production
// and destruction is very fast. We only use value recyclying 
// factory for arguments.
// ScriptFigher Project.
// A. Savidis, February 2006.
//

#ifndef	DELTAARGUMENTSRECYCLER_H
#define	DELTAARGUMENTSRECYCLER_H

#include "urecyclefactory.h"
#include "usingleton.h"
#include "DeltaValue.h"

////////////////////////////////////////////////////////////

#define	DELTAOBJECT_FACTORYCLASS							\
		usingleton<											\
			urecyclefactory<								\
				DeltaValue,									\
				uvoidconstructorarguments,					\
				uvoidplacementconstructor<DeltaValue>		\
			>												\
		>

class DVM_CLASS DeltaValueFactoryClass : public DELTAOBJECT_FACTORYCLASS {};

class DVM_CLASS DeltaValueFactory : public DeltaValueFactoryClass {	// API actually, no construction.
	public:
	static	DeltaValue* New (void) 
		{ return GetSingleton().Get(uvoidconstructorarguments()); }
	static void Delete (DeltaValue* obj)
		{ GetSingleton().UnUse(obj); }
};

#endif	// Do not add stuff beyond this point.
// CompilerStringHolder.h
// Holder for the compiler dynamic strings.
// ScriptFighter Project.
// Y.Lilis, July 2011.
//

#ifndef COMPILERSTRINGHOLDER_H
#define COMPILERSTRINGHOLDER_H

#include "ulatedestructor.h"
#include "ufunctors.h"

////////////////////////////////////////////////////////////////////////

typedef	udestroyablewrapper<char*, uarrdestructorfunctor<char*> > DestroyableString;

class CompilerStringHolder : 
	public ulatedestructionmanager<
				DestroyableString,
				uptrdestructorfunctor<DestroyableString*>
			>{
	public:
	char* StringWithLateDestruction(char* s) {
		add(DNEWCLASS(DestroyableString, (s))); 
		return s;
	}
};

////////////////////////////////////////////////////////////////////////

#define STRINGHOLDER_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), CompilerStringHolder)))

#define STRINGHOLDER	STRINGHOLDER_EX(COMPONENT_DIRECTORY())

////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

// ulatedestructor.cpp
// Late destruction manager for all sorts of instances.
// ScriptFighter Project.
// A. Savidis, July 2002.
// Update July 2007, adding global late destructor.
//

#include "ulatedestructor.h"

USINGLETON_INSTANCE(usingleton<GlobalLateDestructorClass>)

bool LatelyDestroyable::LateDestruction (void) {
	DASSERT(IsAlive());
	GlobalLateDestructor::GetSingleton().add(this);
	alive = false;
	return NotifyDestruction();
}
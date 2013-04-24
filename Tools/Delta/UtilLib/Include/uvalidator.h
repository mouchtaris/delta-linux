// uvalidator.h
// A generation hash-based pointer validator class.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	UVALIDATOR_H
#define	UVALIDATOR_H

#include "DDebug.h"
#include "utypes.h"
#include "uinstanceholder.h"

#include <algorithm>
#include <set>

//---------------------------------------------------------------

template <class T> class uinstancevalidator : public uinstanceholder<T> {
	public:
	void		validate (const T* inst) const
					{ DASSERT(uinstanceholder<T>::in(inst)); }

	uinstancevalidator(void){}
	~uinstancevalidator() {}
};

/////////////////////////////////////////////////////////////////

#define	UVALIDATOR_HASH_SIZE	211

class UTILLIB_CLASS uvalidator {
	
	private:
	uinstancevalidator<void>	Table[UVALIDATOR_HASH_SIZE];
	util_ui16					Hash (const void* obj) const
									{ return ((util_ui32) obj) % UVALIDATOR_HASH_SIZE; }

	/////////////////////////////////////////////////////////////////

	public:
	const void*	Validate (const void* obj) const
					{ return Table[Hash(obj)].in(obj)? obj : (void*) 0; }

	void		Register (const void* obj)
					{ Table[Hash(obj)].add(obj); }

	void		Cancel (const void* obj)
					{ Table[Hash(obj)].remove(obj); }

	uvalidator(void){}
	~uvalidator(){}
};

template <class T> 
class uvalidatortyped : public uvalidator {};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.




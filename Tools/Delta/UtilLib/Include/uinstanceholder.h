// uinstanceholder.h
// Template class for holdign instances - only for book keeping
// and massive destruction.
// A. Savidis, original version Marrch 2002.
//

#ifndef	UINSTANCEHOLDER_H
#define	UINSTANCEHOLDER_H

#include <set>
#include "utypes.h"

//---------------------------------------------------------------

template <class T> class uinstanceholder {

	protected:
	std::set<const T*> instances;

	public:
	void		add (const T* inst)	
					{ DASSERT(!in(inst)); instances.insert(inst); }

	void		remove (const T* inst) 
					{ instances.erase(inst); }

	bool		in (const T* inst) const
					{ return instances.find(inst) != instances.end(); }

	void		clear (void) { instances.clear(); }

	util_ui32	total (void) const
					{ return instances.size(); }

	const std::set<const T*> 
				get (void) const { return instances; }

	uinstanceholder(void){}
	virtual ~uinstanceholder() { clear(); }
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

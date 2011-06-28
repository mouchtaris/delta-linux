// ulatedestructor.h
// Late destruction manager for all sorts of instances.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	ULATEDESTRUCTOR_H
#define	ULATEDESTRUCTOR_H

#include <list>
#include <algorithm>

#include "DDebug.h"
#include "usingleton.h"

//---------------------------------------------------------------

class UTILLIB_CLASS GlobalLateDestructorClass;

// Late destruction is optional, as it is free to also
// apply instant destruction.
//
class UTILLIB_CLASS LatelyDestroyable {

	DFRIENDDESTRUCTOR()
	friend class GlobalLateDestructorClass;

	protected:
	virtual ~LatelyDestroyable(){}
	bool alive;

	public:
	struct UTILLIB_CLASS DestroyFunctor : public std::unary_function<LatelyDestroyable, void> {
		void operator()(LatelyDestroyable* p) const
			{ p->InstantDestruction(); }
	};

	virtual bool	LateDestruction (void);
	virtual void	InstantDestruction (void)	{ DDELETE(this); }	// Default.
	bool			IsAlive (void) const		{ return alive; }
	virtual bool	NotifyDestruction (void)	{ return true; }

	LatelyDestroyable (void): alive(true) {}
};

//---------------------------------------------------------------

template <class T, class TDestroy = LatelyDestroyable::DestroyFunctor> class ulatedestructionmanager {

	protected:
	bool committing;
	std::list<T*> items;
	
	public:

	/////////////////////////////////////////////////////////////////
	// It is not allowed to request that an item gets destroyed
	// twice.
	//
	void	add (T* item) {
				DASSERT(std::find(items.begin(), items.end(), item) == items.end());
				items.push_back(item);
			}

	void	remove (T* item) {
				if (!committing) {
					typename std::list<T*>::iterator i = std::find(items.begin(), items.end(), item);
					DASSERT(i != items.end());
					items.erase(i);
				}
			}

	void	commit (void) {
				committing = true;
				std::for_each(items.begin(), items.end(), TDestroy());
				clear();
				committing = false;
			}

	void	clear (void) { items.clear(); }

	ulatedestructionmanager(void): committing(false){}
	~ulatedestructionmanager() { clear(); }
};

//---------------------------------------------------------------

class UTILLIB_CLASS GlobalLateDestructorClass	: public ulatedestructionmanager<LatelyDestroyable>{};
class UTILLIB_CLASS GlobalLateDestructor		: public usingleton<GlobalLateDestructorClass>{};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.


// AutoCollection.h
// Hanlding the auto-collection of items not gracefully
// deleted, either upon parse errors or because it is a policy
// not to delete during parsing.
// ScriptFighter Project.
// A. Savidis, December 2006.
// It is a re-factoring of the former expr cleaner, unifying
// with remaining stmt cleaner.
//

#ifndef	AUTOCOLLECTION_H
#define	AUTOCOLLECTION_H

#include "ulatedestructor.h"
#include "ufunctors.h"

/////////////////////////////////////////////////////////////////

class AutoCollectable;

struct AutoCollectableDestructor {
	void operator()(AutoCollectable* item) const;
};

class AutoCollector : public	ulatedestructionmanager<
									AutoCollectable, 
									AutoCollectableDestructor  
								> {
	static AutoCollector* singletonPtr;

	public:
	bool			IsCommitting (void) const { return committing; }
	void			CleanUp (void) { commit(); }
	void			ValidateAll (void);

	static void		SingletonCreate (void);						
	static void		SingletonDestroy (void);

	static AutoCollector*	GetPtr (void) 
								{ DASSERT(singletonPtr); return singletonPtr; }
	private:
	DFRIENDDESTRUCTOR()
	AutoCollector(void){}
	~AutoCollector(){}
};

#define	AUTOCOLLECTOR	GetAutoCollector()

inline AutoCollector& GetAutoCollector (void) {
	DASSERT(AutoCollector::GetPtr());
	return *AutoCollector::GetPtr();
}

/////////////////////////////////////////////////////////////////
// Auto-collectable items should never perform DDELETE of constituent
// collectable members when UnderAutoCollection() returns true.
//
class AutoCollectable  {
	public:
	struct ValidateFunctor : public std::unary_function<AutoCollectable*, void> {
		void operator()(AutoCollectable* p) { p = DPTR(p); } // statement used
	};

	bool			UnderAutoCollection (void) const
						{ return AUTOCOLLECTOR.IsCommitting(); }
	virtual void	Delete (void) // Refine per derived class.
						{ DDELETE(this); }

	AutoCollectable (void)		{ AUTOCOLLECTOR.add(this); }
	virtual ~AutoCollectable()	{  AUTOCOLLECTOR.remove(this);  }
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.


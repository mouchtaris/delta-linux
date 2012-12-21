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
#include "DeltaCompilerDefs.h"

/////////////////////////////////////////////////////////////////

class DCOMPLIB_CLASS AutoCollectable;

struct AutoCollectableDestructor {
	void operator()(AutoCollectable* item) const;
};

class DCOMPLIB_CLASS AutoCollector : public	ulatedestructionmanager<
									AutoCollectable, 
									AutoCollectableDestructor  
								> {
	public:
	bool			IsCommitting (void) const { return committing; }
	void			CleanUp (void) { commit(); }
	void			ValidateAll (void);

	AutoCollector(void){}
	~AutoCollector(){}
};

/////////////////////////////////////////////////////////////////
// Auto-collectable items should never perform DDELETE of constituent
// collectable members when UnderAutoCollection() returns true.
//
class DCOMPLIB_CLASS AutoCollectable  {
	protected:
		AutoCollector* collector;
	public:
	struct ValidateFunctor : public std::unary_function<AutoCollectable*, void> {
		void operator()(AutoCollectable* p) { p = DPTR(p); } // statement used
	};

	void			SetAutoCollector(AutoCollector* c) {
						DASSERT(!collector && c);
						DPTR(collector = c)->add(this);
					}
	bool			UnderAutoCollection (void) const
						{ return DPTR(collector)->IsCommitting(); }
	virtual void	Delete (void) // Refine per derived class.
						{ DDELETE(this); }

	//If no auto-collector provided, it should be explicitly set after construction
	AutoCollectable (AutoCollector* c = (AutoCollector*) 0) :
		collector(c) { if (c) DPTR(collector)->add(this); }
	virtual ~AutoCollectable() { DPTR(collector)->remove(this); }
};

/////////////////////////////////////////////////////////////////
// Use this class to create Auto-collectable items
// that are  aware of their collector.
//
template<class T> class AutoCollectableFactory {
	private:
		AutoCollector* collector;
	public:
	void SetAutoCollector(AutoCollector* c) { collector = c; }

	virtual T* New (void) const {
		T* t = DNEW(T);
		t->SetAutoCollector(DPTR(collector));
		return t;
	}

	AutoCollectableFactory (void) : collector((AutoCollector*) 0) {}
	virtual ~AutoCollectableFactory() {}
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.


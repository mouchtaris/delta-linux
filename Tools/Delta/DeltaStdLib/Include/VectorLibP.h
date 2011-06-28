// VectorLibP.h
// The uvector adapter as a collectable delta container.
// ScriptFighter Project.
// A. Savidis, May 2003.
//
#ifndef	VECTORLIBP_H
#define	VECTORLIBP_H

#include "DeltaCollectableContainer.h"
#include "DeltaValue.h"
#include "uvector.h"
#include "DeltaStdClassNames.h"
#include "IteratorAdapter.h"
#include "ContainerAdapter.h"
#include "STLImportHelpers.h"
#include "DeltaStdDefs.h"

inline const char* VectorNameFun (void) 
	{ return VECTOR_TYPE_STR; }

class DVM_CLASS VectorCollectableContainer : public ContainerAdapter<uvector<DeltaValue>, VectorNameFun> {
	public:
	virtual void	ToValue (DeltaValue* at);
	virtual void	DeleteMemory (void) { DDELETE(this); }
};

/////////////////////////////////////////////////////

template <>
struct ITERATOR_ACCESS_TRAITS< uvector<DeltaValue>::iterator > {
	typedef uvector<DeltaValue>::iterator iterator;
	static const iterator 	Begin (uvector<DeltaValue>& l) 
								{ return l.begin(); }
	static const iterator 	End (uvector<DeltaValue>& l) 
								{ return l.end(); }
	static const iterator 	Begin (uvector<DeltaValue>* l) 
								{ return l->begin(); }
	static const iterator 	End (uvector<DeltaValue>* l) 
								{ return l->end(); }
	static const DeltaValue	Get (const uvector<DeltaValue>::iterator& i) 
								{ return *i; }
	static const DeltaValue	GetBySkippingUserCode (const uvector<DeltaValue>::iterator& i) 
								{ return *i; }
	static bool				Set (uvector<DeltaValue>::iterator& i, const DeltaValue& val) 
								{ i->Assign(val); return true; }
};

/////////////////////////////////////////////////////

class DVM_CLASS VectorIteratorAdapter: public	IteratorAdapter<
													VectorCollectableContainer,
													uvector<DeltaValue>,
													STL_ITERATOR_TYPE_TRAITS< uvector<DeltaValue> >
												>  {
	virtual void	ExtraToStringBeforeValue (DeltaString* at) {
						DPTR(at)->Add(
							uconstructstr("index(%u),", GetIterator().getindex())
						);
					}
	public:
	VectorIteratorAdapter(void){}
	virtual ~VectorIteratorAdapter(){}
};

/////////////////////////////////////////////////////

struct DVM_CLASS VectorNameTraits {
	static const char* 	iter;
	static const char* 	setbegin;
	static const char* 	setend;
	static const char* 	checkend;
	static const char* 	checkbegin;
	static const char* 	equal;
	static const char* 	assign;
	static const char* 	copy;
	static const char* 	fwd;
	static const char* 	bwd;
	static const char* 	getcontainer;
	static const char* 	getcounter;
	static const char* 	setval;
	static const char* 	getval;
	static const char* 	getindex;

	static const char* 	iter2string;
	static const char* 	itertypestring;
	static const char* 	cont2string;
	static const char* 	conttypestring;
	static void			add_methods (DeltaValue* at);
};

/////////////////////////////////////////////////////

typedef	ITERATOR_LIBRARY<
			VectorIteratorAdapter,
			VectorCollectableContainer,
			VectorNameTraits
		> VectorIteratorLibrary;

#endif	// Do not add stuff beyond this point.
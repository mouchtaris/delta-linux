// ListLibP.h
// The STL list adapter as a collectable delta container.
// ScriptFighter Project.
// A. Savidis, May 2003.
//
#ifndef	LISTLIBP_H
#define	LISTLIBP_H

#include "DeltaCollectableContainer.h"
#include "DeltaValue.h"
#include "IteratorAdapter.h"
#include "ContainerAdapter.h"
#include "STLImportHelpers.h"
#include "DeltaStdClassNames.h"
#include "DeltaStdDefs.h"
#include "usafeiterators.h"
#include <list>

/////////////////////////////////////////////////////

inline const char* ListNameFun (void) 
	{ return LIST_TYPE_STR; }

class DVM_CLASS ListCollectableContainer :	public ContainerAdapter<std::list<DeltaValue>, ListNameFun>,
											public usafecontainerbehavior {
	public:
	virtual void DeleteMemory (void) { DDELETE(this); }
	virtual void ToValue (DeltaValue* at);
};

/////////////////////////////////////////////////////

template <>
struct ITERATOR_ACCESS_TRAITS< std::list<DeltaValue>::iterator > {
	static const std::list<DeltaValue>::iterator Begin (std::list<DeltaValue>& l) 
													{ return l.begin(); }
	static const std::list<DeltaValue>::iterator End (std::list<DeltaValue>& l) 
													{ return l.end(); }
	static const std::list<DeltaValue>::iterator Begin (std::list<DeltaValue>* l) 
													{ return l->begin(); }
	static const std::list<DeltaValue>::iterator End (std::list<DeltaValue>* l) 
													{ return l->end(); }
	static const DeltaValue	Get (const std::list<DeltaValue>::iterator& i) 
													{ return *i; }
	static const DeltaValue	GetBySkippingUserCode (const std::list<DeltaValue>::iterator& i) 
													{ return *i; }
	static bool				Set (std::list<DeltaValue>::iterator& i, const DeltaValue& val) 
													{ i->Assign(val); return true; }
};

/////////////////////////////////////////////////////

class DVM_CLASS ListIteratorAdapter : public IteratorAdapter<
											ListCollectableContainer,
											std::list<DeltaValue>,
											STL_ITERATOR_TYPE_TRAITS< std::list<DeltaValue> >
										>,
							public usafeiteratorbehavior {
	DELTA_SAFE_ITERATOR_ADAPTER_IMPLEMENTATION(
		ListIteratorAdapter,
		ListCollectableContainer
	) 
};

/////////////////////////////////////////////////////

struct DVM_CLASS ListNameTraits {
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

	static const char* 	iter2string;
	static const char* 	itertypestring;
	static const char* 	cont2string;
	static const char* 	conttypestring;
	static void			add_methods (DeltaValue* at);
};

/////////////////////////////////////////////////////

typedef	ITERATOR_LIBRARY<
			ListIteratorAdapter,
			ListCollectableContainer,
			ListNameTraits
		> ListIteratorLibrary;

/////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

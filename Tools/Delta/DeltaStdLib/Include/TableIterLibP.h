// TabIterLibP.h
// Table iterator adapters.
// ScriptFighter Project.
// A. Savidis, July 2009.
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

/////////////////////////////////////////////////////

class DVM_CLASS TableContainerAdapter {

	protected:
	DeltaTable* cont;

	public:
	typedef DeltaTable* Original;

	void 				Refer (void)   
							{ cont->IncRefCounter((DeltaValue*) 0); }
	void 				Unrefer (void) 
							{ cont->DecRefCounter((DeltaValue*) 0); DDELETE(this); }
	util_ui32			Size (void) const		
							{ return DPTR(cont)->Total(); }
	util_ui32			GetSerialNo(void) const		
							{ return DPTR(cont)->GetSerialNo(); }

	void				ConvertToString (DeltaString* at, void* closure) 
							{ cont->ConvertToString(at, closure);  }
	void				ConvertToStringBySkippingUserCode (DeltaString* at, void* closure) 
							{ cont->ConvertToStringBySkippingUserCode(at, closure);  }
	void				ToValue (DeltaValue* at) 
							{ at->FromTable(cont); }
	DeltaTable*&		GetNativeContainer (void) 
							{ return cont; }
	DeltaTable*			GetNativeContainerPtr (void) 
							{ return cont; }
	const DeltaTable*	GetNativeContainerPtr (void) const
							{ return cont; }

	TableContainerAdapter (DeltaTable* t) : cont(t) {}
	TableContainerAdapter (void) : cont ((DeltaTable*) 0) {}
	~TableContainerAdapter() {}
};

/////////////////////////////////////////////////////

template <>
struct ITERATOR_ACCESS_TRAITS<DeltaTable::iterator> {

	static const DeltaTable::iterator	Begin (DeltaTable& t)			{ return t.begin(); }
	static const DeltaTable::iterator	End	  (DeltaTable& t)			{ return t.end();	 }
	static const DeltaTable::iterator	Begin (DeltaTable* t)			{ return t->begin(); }
	static const DeltaTable::iterator	End	 (DeltaTable* t)			{ return t->end();	 }

	static const DeltaValue			Get	(DeltaTable::iterator& i)	{
										DeltaValue at;
										i->GetContent(&at);	 
										return at;
									}

	static const DeltaValue			GetBySkippingUserCode (DeltaTable::iterator& i)	{ 
										DeltaValue at;
										i->GetContentBySkippingUserCode(&at);	 
										return at;
									}

	static bool						Set (DeltaTable::iterator& i, const DeltaValue& val) 
										{ return i->SetContent(val);  }
};

/////////////////////////////////////////////////////

class DVM_CLASS TableIteratorAdapter : public IteratorAdapter<
											TableContainerAdapter, 
											DeltaTable,
											STL_ITERATOR_TYPE_TRAITS<DeltaTable> 
										> {

	protected:
	virtual	void			AfterSetContainerActions (TableContainerAdapter* _cont) {
								cont =	DNEWCLASS(
											TableContainerAdapter, 
											(_cont->GetNativeContainer())
										);
							}

	public:
	typedef DeltaTable::iterator Original;

	virtual bool			ShouldIgnoreForward (void) const
								{ return GetIterator().should_ignore_next_fwd(); }

	virtual void			AssumeForwarded (void)
								{ GetIterator().reset_ignore_next_fwd(); }

	void					ConvertValueToString (DeltaString* at) {
								DeltaTable* t = DPTR(cont)->GetNativeContainer();
								if (iter != DPTR(t)->end()) {
									DeltaValue val;
									iter->GetIndex(&val);
									DPTR(at)->Add("index(");	
									val.ConvertToStringBySkippingUserCode(at);

									iter->GetContentBySkippingUserCode(&val);
									DPTR(at)->Add("),value(");
									val.ConvertToStringBySkippingUserCode(at);
									DPTR(at)->Add(")");
								}
								else
									DPTR(at)->Add("at end");
							}

	TableIteratorAdapter (void){}
	virtual ~TableIteratorAdapter(){}
};

/////////////////////////////////////////////////////

struct DVM_CLASS TableIteratorTraits 
	{ typedef DeltaTable::iterator iterator; };

/////////////////////////////////////////////////////

template <> struct GET_CONTAINER_TEMPLATE<TableContainerAdapter> {

	static TableContainerAdapter* adapter;

	TableContainerAdapter* operator()(
			DeltaVirtualMachine*	vm, 
			const char*				func,
			void					(*resetFun)(DeltaVirtualMachine* vm),
			util_ui32				argNo,
			const char*				contTypeStr
		) {

		DeltaValue* argObj;
		GET_TABLE(
			table, 
			func, 
			{ resetFun(vm); return (TableContainerAdapter*) 0;}, 
			argNo
		);		
		udestructor_invocation(adapter);
		return new (adapter) TableContainerAdapter(table);
	}
};

/////////////////////////////////////////////////////

struct DVM_CLASS TableNameTraits {
	static const char* 	iter;
	static const char* 	setbegin;
	static const char* 	setend;
	static const char* 	checkend;
	static const char* 	checkbegin;
	static const char* 	equal;
	static const char* 	assign;
	static const char* 	copy;
	static const char* 	fwd;
	static const char* 	getcounter;
	static const char* 	getcontainer;
	static const char* 	setval;
	static const char* 	getval;
	static const char*	getindex;

	static const char* 	iter2string;
	static const char* 	itertypestring;
	static const char* 	cont2string;
	static const char* 	conttypestring;
	static void			add_methods (DeltaValue* at);
};

/////////////////////////////////////////////////////

typedef	ITERATOR_LIBRARY<
			TableIteratorAdapter,
			TableContainerAdapter,
			TableNameTraits
		> TableIteratorLibrary;

/////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

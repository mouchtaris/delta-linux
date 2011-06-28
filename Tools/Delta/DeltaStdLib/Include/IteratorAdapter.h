// IteratorAdapter.h
// Generic iterator adapter for adapted STL 
// collectable containers.
// ScriptFighter Project.
// A. Savidis, November 2006.
//

#ifndef	ITERATORADAPTER_H
#define	ITERATORADAPTER_H

#include "DeltaExternId.h"
#include "DDebug.h"
#include "usafeiterators.h"

/////////////////////////////////////////////////////

template <typename Tstlcontainer> 
struct STL_ITERATOR_TYPE_TRAITS {
	typedef typename Tstlcontainer::iterator iterator;
};

template <typename Tnativecontainer> 
struct ITERATOR_ACCESS_TRAITS{};

/////////////////////////////////////////////////////

template <class TCadapted, class TCoriginal, class Titertraits> class IteratorAdapterBasic {

	public:
	typedef typename Titertraits::iterator Original;
	
	protected:
	TCadapted*	cont;
	Original	iter;
	util_ui32	iterationCounter;	// Counts how many items iterated.

	virtual	void			BeforeSetContainerActions (TCadapted* _cont){}
	virtual	void			AfterSetContainerActions (TCadapted* _cont){}

	public:
	virtual bool			ShouldIgnoreForward (void) const 
								{ return false; }
	virtual void			AssumeForwarded (void){}

	void					SetContainer (TCadapted* _cont) {
								if (cont != _cont) {
									if (cont) {
										BeforeSetContainerActions(cont);
										DPTR(cont)->Unrefer();
									}
									if ((cont = _cont)) {
										AfterSetContainerActions(cont);
										DPTR(cont)->Refer();
									}
								}
								ResetIterationCounter();
							}

	util_ui32				GetIterationCounter (void) const 
								{ return iterationCounter; }
	void					ResetIterationCounter (void)
								{ iterationCounter = 0; }
	void					SetIterationCounter (util_ui32 n)
								{ iterationCounter = n; }
	void					IncIterationCounter (void)
								{ ++iterationCounter; }
	void					DecIterationCounter (void)
								{ DASSERT(iterationCounter); --iterationCounter; }

	void					Invalidate (void)
								{ SetContainer((TCadapted*) 0); }
	bool					IsValid (void) 
								{ return GetContainerAdapter() && GetNativeContainer();  }
	TCoriginal*				GetNativeContainer (void)
								{ return cont ? cont->GetNativeContainerPtr() : (TCoriginal*) 0; }
	TCoriginal*				GetNativeContainer (void) const
								{ return cont ? cont->GetNativeContainerPtr() : (TCoriginal*) 0; }
	TCadapted*				GetContainerAdapter (void) 
								{ return cont; }
	bool					MatchesContainer (TCadapted* contAdapter)
								{ return GetNativeContainer() == DPTR(contAdapter)->GetNativeContainerPtr(); }
	Original&				GetIterator (void) 
								{ return iter; }
	const Original&			GetIterator (void) const
								{ return iter; }

	void					Assign (IteratorAdapterBasic* from) {
								DASSERT(this != from);
								SetContainer(DPTR(from)->GetContainerAdapter());
								GetIterator() = DPTR(from)->GetIterator();
								SetIterationCounter(DPTR(from)->GetIterationCounter());
							}

	IteratorAdapterBasic (void) : cont((TCadapted*) 0), iterationCounter(0){}
	virtual ~IteratorAdapterBasic() {
		if (cont) 
			DPTR(cont)->Unrefer(); 
	}
};

/////////////////////////////////////////////////////

template <
	class TCadapted, 
	class TCoriginal, 
	class Titertraits
> 
class IteratorAdapter : public IteratorAdapterBasic<TCadapted, TCoriginal, Titertraits>  {

	public:
	typedef typename Titertraits::iterator Original;
	typedef IteratorAdapterBasic<TCadapted, TCoriginal, Titertraits> Base;
	public:
	virtual void			ExtraToStringBeforeValue (DeltaString* at) {}

	void					ConvertValueToString (DeltaString* at) {
								if (Base::iter != ITERATOR_ACCESS_TRAITS<Original>::End(DPTR(Base::cont)->GetNativeContainer())) {
									ExtraToStringBeforeValue(at);
									DPTR(at)->Add("value(");
									if (DeltaExternId::UseConvertToString())
										ITERATOR_ACCESS_TRAITS<Original>::Get(Base::iter).ConvertToString(at);
									else {
										DASSERT(DeltaExternId::UseConvertToStringBySkippingUserCode());
										ITERATOR_ACCESS_TRAITS<Original>::Get(Base::iter).ConvertToStringBySkippingUserCode(at);
									}
									DPTR(at)->Add(")");
								}
								else
									DPTR(at)->Add("at end");			
							}
	IteratorAdapter (void){}
	virtual ~IteratorAdapter(){}
};

/////////////////////////////////////////////////////

#define	DELTA_SAFE_ITERATOR_ADAPTER_IMPLEMENTATION(_iteradapter,_contadapter)						\
	public:																							\
	virtual usafecontainerbehavior*	get_container (void)											\
										{ return GetContainerAdapter(); }							\
	virtual const void*				get_value_address (void)										\
										{ return &*GetIterator(); }									\
	virtual void					set_at_end (void)												\
										{ iter = GetNativeContainer()->end(); }						\
	virtual bool					is_at_end (void) const											\
										{ return iter == GetNativeContainer()->end(); }				\
	virtual void					fwd (void)														\
										{ ++iter; }													\
	virtual bool					ShouldIgnoreForward (void) const								\
										{ return usafeiteratorbehavior::should_ignore_next_fwd(); }	\
	virtual void					AssumeForwarded (void)											\
										{ usafeiteratorbehavior::reset_ignore_next_fwd(); }			\
	virtual void					BeforeSetContainerActions (_contadapter* unused)				\
										{ usafeiteratorbehavior::on_before_set_container(); }		\
	virtual void					AfterSetContainerActions (_contadapter* unused)					\
										{ usafeiteratorbehavior::on_after_set_container(); }		\
	_iteradapter (void)		{}																		\
	virtual ~_iteradapter()	{ usafeiteratorbehavior::clear(); }

/////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

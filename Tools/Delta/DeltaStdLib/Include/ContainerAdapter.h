// ContainerAdapter.h
// Generic container adapter for STL collectable containers.
// ScriptFighter Project.
// A. Savidis, November 2006.
//

#ifndef	CONTAINERADAPTER_H
#define	CONTAINERADAPTER_H

#include "DeltaCollectableContainer.h"
#include "DeltaValue.h"
#include "DeltaExternId.h"
#include "DeltaExternIdFactory.h"
#include "DeltaString.h"
#include "ustrings.h"

/////////////////////////////////////////////////////

template <
	class TCnative, 
	const char* (*NameFunc)(void)
>	
class ContainerAdapter : public CollectableContainer {

	protected:
	TCnative			cont;

	public:
	typedef TCnative	Original;

	void				Refer (void)		{ IncRefCounter((DeltaValue*) 0); }
	void				Unrefer (void)		{ DecRefCounter((DeltaValue*) 0); }
	util_ui32			Size (void)	const	{ return (util_ui32) cont.size(); }

	const char*			GetExtClassString (void) const 
							{ return NameFunc(); }

	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL(CollectableContainer, NameFunc())

	//*******************************

	void	ConvertToStringHandler (
				void (DeltaValue::*	converter)(DeltaString*) const, 
				DeltaString*			at, 
				void*					closure
			) {
			
			ContainerAdapter* adapter = VGET_INST(ContainerAdapter, closure, NameFunc());
			DASSERT(adapter && adapter == this);

			if (GetPrintState().IsInProgress())
				at->Add(uconstructstr("self_%s", NameFunc()));
			else {

				DELTA_CONTAINER_TOSTRING_PROCESSING();

				DPTR(at)->Add(
					uconstructstr(
						"%s(0x%x, size=%d)[", NameFunc(), GetSerialNo(), cont.size()
					)
				);
				
				bool needComma = false;

				for (typename TCnative::iterator i = cont.begin(); i != cont.end(); ++i) {
					if (needComma)
						at->Add(",");
					else
						needComma = true;
					((*i).*converter)(at);
				}

				at->Add("]");
		}
	}

	//*******************************

	virtual void		ConvertToString (DeltaString* at, void* closure) { 
							DASSERT(DeltaExternId::UseConvertToString());  
							ConvertToStringHandler(&DeltaValue::ConvertToString, at, closure);
						}

	virtual void		ConvertToStringBySkippingUserCode (DeltaString* at, void* closure) { 
							DASSERT(DeltaExternId::UseConvertToStringBySkippingUserCode());  
							ConvertToStringHandler(
								&DeltaValue::ConvertToStringBySkippingUserCode, 
								at, 
								closure
							);
						}

	virtual void		ToValue (DeltaValue* at) = 0;

	//*******************************

	TCnative&			GetNativeContainer (void) 
							{ return cont; }

	TCnative*			GetNativeContainerPtr (void) 
							{ return &cont; }
	const TCnative*		GetNativeContainerPtr (void) const
							{ return &cont; }

	virtual void		ExplicitDestructor (void) 
							{ cont.clear(); }

	ContainerAdapter (void){}
	~ContainerAdapter() {}
};

/////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.



// DeltaWxCollectableDestructors.h
// Destructor functions that will be called when the
// reference counter of the collectable reach zero.
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#ifndef DELTA_WX_COLLECTABLE_DESTRUCTORS
#define DELTA_WX_COLLECTABLE_DESTRUCTORS

#include "DeltaWxWrapperToExternIdMap.h"
#include "DeltaWxNativeInstanceToWrapperMap.h"

////////////////////////////////////////////////////////////////

extern void	LetWrapperLive(void* val);

template<class ClassId, class WrapperT>
void		DestroyWrapper(void* val)
{
	util_ui32 wrapperSerialNo	= (util_ui32)val;
	WrapperT* wrapper			= VGET_INST(WrapperT, wrapperSerialNo, ClassId::ID);
	DDELETE(wrapper);
}

template<class ClassId, class WrapperT, class Native>
void		DestroyWrapperAndNativeInstance (void* val)
{
	util_ui32 wrapperSerialNo	= (util_ui32)val;
	WrapperT* wrapper			= VGET_INST(WrapperT, wrapperSerialNo, ClassId::ID);
	Native* nativeInst			= (Native*)DPTR(wrapper)->GetNativeInstance();
	DASSERT(wxWidgets::NativeInstanceToWrapperMapGet().HasKey(nativeInst));
	DASSERT(wxWidgets::NativeInstanceToWrapperMapGet().GetSerialNo(nativeInst) == wrapperSerialNo);
	DDELETE(wrapper);
	delete nativeInst;
}

////////////////////////////////////////////////////////////////

#endif
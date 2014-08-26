#ifndef DELTA_WX_LIBRARY_TEMPLATES_SET_WRAPPER_CHILD
#define DELTA_WX_LIBRARY_TEMPLATES_SET_WRAPPER_CHILD

#include "DeltaWxWindow.h"

// TODO this is a temporary, spaghetti-code solution
// in order to fix a cyclic dependency.
// The library maintainer should be contacted in order
// to fix it properly.
//
// The dependency is among headers
//    DeltaWxWindow.h --> wxWrapperUtilFunctions.h
//      for WX_WRAPPER_DEFINITION   (seems valid)
//    wxWrapperUtilFunctions.h --> DeltaWxLibraryTemplates.h
//      (is probably valid)
//    DeltaWxLibraryTemplates.h --> DeltaWxWindow.h
//      for DeltaWxWindowClassId   (seems INVALID)
//
// This cyclic dependency is not detected in windows compilations
// because of the way microsoft compilers treat templates.
//
// In GCC though, where "identifiers that do not depend on a
// template parameter need to be declared at that point of
// compilation", this cyclic dependency does produce errors.

template<class ChildClassId, class ChildWrapper, class ChildNative>
void			SetWrapperChild (ChildNative* nativeInst)
{
	DASSERT(nativeInst);
	SetDependencyBetweenNativeInstances
		<DeltaWxWindowClassId, DeltaWxWindow, wxWindow,
		ChildClassId, ChildWrapper, ChildNative>
		(nativeInst->GetParent(), nativeInst);
}

#endif

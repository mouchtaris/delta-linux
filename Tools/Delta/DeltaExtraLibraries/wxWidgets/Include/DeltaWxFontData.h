#ifndef DELTAWXFONTDATA_H
#define DELTAWXFONTDATA_H
#include <wx/cmndata.h>
#if wxCHECK_VERSION(2, 9, 0)
#include <wx/fontdata.h>
#endif
#include "uvalidatable.h"
#include "DeltaValue.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxObject.h"

WX_WRAPPER_DEFINITION(FontData, fontdata, Object)

#endif	// Do not add stuff beyond this point.
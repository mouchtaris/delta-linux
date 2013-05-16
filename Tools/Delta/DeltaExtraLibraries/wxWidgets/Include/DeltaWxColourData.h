#ifndef DELTAWXCOLOURDATA_H
#define DELTAWXCOLOURDATA_H
#include <wx/cmndata.h>
#if wxCHECK_VERSION(2, 9, 0)
#include <wx/colourdata.h>
#endif
#include "uvalidatable.h"
#include "DeltaValue.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxObject.h"

WX_WRAPPER_DEFINITION(ColourData, colourdata, Object)

#endif	// Do not add stuff beyond this point.
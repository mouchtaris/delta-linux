// Delta.h
// Basic header for everything you need to use Delta,
// either for embedding or for creating application libraries
// visible inside Delta programs. You will also need to separately
// include the facades to handle initialisation / cleanup.
// Script Fighter Project.
// A. Savidis, October 1999 (original version).
// Latest updates, December 2009.
//

#ifndef	DELTA_H
#define	DELTA_H

#include "DDebug.h"
#include "ustrings.h"
#include "uvalidatable.h"
#include "usingleton.h"
#include "uerrorclass.h"
#include "utypes.h"
#include "udynamiclibloader.h"
#include "ufiles.h"
#include "ubinaryio.h"

#include "DeltaString.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DeltaVMRegistry.h"
#include "DeltaTable.h"
#include "DeltaTableFactory.h"
#include "DeltaObject.h"
#include "DeltaObjectValidator.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "DeltaLibraryObjectCreator.h"
#include "DeltaExceptionHandling.h"
#include "DeltaCollectableContainer.h"
#include "DeltaCollectableSimple.h"
#include "DeltaDebugDynamicActivator.h"
#include "DeltaExternId.h"
#include "DeltaExternIdFactory.h"
#include "DeltaExternIdFieldGetter.h"
#include "GenericReader.h"
#include "GenericWriter.h"

///////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

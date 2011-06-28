// TypeTag.h
// Support for type tags in Delta compilation.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#ifndef	TYPETAG_H
#define	TYPETAG_H

#include "DeltaByteCodeTypes.h"

///////////////////////////////////////////////////////////

#define	DeltaTypeTag	DeltaFunctionReturnBasicType
#define TagAnyValue		DeltaFunctionReturnAnyValue		
#define TagAnyValues	DeltaFunctionReturnAnyValues	
#define TagExternId		DeltaFunctionReturnExternId		
#define TagObject		DeltaFunctionReturnObject		
#define TagBool			DeltaFunctionReturnBool			
#define TagNumber		DeltaFunctionReturnNumber		
#define TagString		DeltaFunctionReturnString		
#define TagFunction		DeltaFunctionReturnFunction		
#define TagMethod		DeltaFunctionReturnMethod		
#define TagLibFunction	DeltaFunctionReturnLibFunction	
#define TagCallable		DeltaFunctionReturnCallable	
#define TagVoid			DeltaFunctionReturnVoid			
#define TagUndefined	DeltaFunctionReturnUndefined	
#define TagNil			DeltaFunctionReturnNil			

#define	TOTAL_TYPE_TAGS	(TagNil + 1)

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.



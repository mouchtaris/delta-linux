// BufferLib.h
// Platform independent buffer I/0 for Delta.
// A. Savidis, March 2009.
//

#ifndef	BUFFERLIB_H
#define	BUFFERLIB_H

/////////////////////////////////////////////////////

#include "DeltaLibraryObjectCreator.h"
#include "DeltaStdLibTemplatesUtils.h"
#include "DeltaStdClassNames.h"
#include "ubinaryio.h"

#define	DLIB_GET_OUTPUTBUFFER \
	DLIB_GET_COLLECTABLESIMPLE_ARG(ob, ubinaryio::OutputBuffer, OUTPUTBUFFER_TYPE_STR)

#define	DLIB_GET_INPUTBUFFER \
	DLIB_GET_COLLECTABLESIMPLE_ARG(ib, ubinaryio::InputBuffer, INPUTBUFFER_TYPE_STR)

#define	DLIB_GET_OUTPUTBUFFER_EX(name) \
	DLIB_GET_COLLECTABLESIMPLE_ARG(name, ubinaryio::OutputBuffer, OUTPUTBUFFER_TYPE_STR)

#define	GETINPUTBUFFER_EX(name) \
	DLIB_GET_COLLECTABLESIMPLE_ARG(name, ubinaryio::InputBuffer, INPUTBUFFER_TYPE_STR)

/////////////////////////////////////////////////////

extern		void 						Install_DeltaInputBuffer_Lib (DeltaObject* mainAPI);
extern		void 						CleanUp_DeltaInputBuffer_Lib (void);
DVM_FUNC	void 						DeltaInputBuffer_Make (DeltaValue* at, ubinaryio::InputBuffer* buffer);
DVM_FUNC	ubinaryio::InputBuffer*		DeltaInputBuffer_Get (DeltaValue& val);

extern		void 						Install_DeltaOutputBuffer_Lib (DeltaObject* mainAPI);
extern		void 						CleanUp_DeltaOutputBuffer_Lib (void);
DVM_FUNC	void						DeltaOutputBuffer_Make (DeltaValue* at, ubinaryio::OutputBuffer* buffer);
DVM_FUNC	ubinaryio::OutputBuffer*	DeltaOutputBuffer_Get (DeltaValue& val);

/////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
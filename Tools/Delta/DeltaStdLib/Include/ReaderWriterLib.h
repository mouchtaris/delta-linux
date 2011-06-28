// ReaderWriterLib.h
// Generic readers / writers for Delta.
// A. Savidis, March 2009.
//

#ifndef	READERWRITERLIB_H
#define	READERWRITERLIB_H

#include "DeltaLibraryObjectCreator.h"
#include "DeltaStdClassNames.h"
#include "DeltaStdLibTemplatesUtils.h"
#include "GenericReader.h"
#include "GenericWriter.h"

// We pack a reader / writer with an object holding the buffer or stream
// from / to which it reads / writes. This is useful for the field getter to
// allow further inspect the buffer or stream via a reader / writer reference.
//
typedef std::pair<GenericReader*, DeltaValue*>	ReaderWrapper;
typedef std::pair<GenericWriter*, DeltaValue*>	WriterWrapper;

#define	DLIB_GET_READER															\
	GenericReader* reader;														\
	unullify(reader);															\
	{																			\
	DLIB_GET_COLLECTABLESIMPLE_ARG(wrapper, ReaderWrapper, READER_TYPE_STR);	\
	reader = DPTR(wrapper)->first;												\
	}

#define	DLIB_GET_WRITER															\
	GenericWriter* writer;														\
	unullify(writer);															\
	{																			\
	DLIB_GET_COLLECTABLESIMPLE_ARG(wrapper, WriterWrapper, WRITER_TYPE_STR);	\
	writer = DPTR(wrapper)->first;												\
	}

extern void Install_DeltaReaderWriter_Lib (DeltaObject* mainAPI);
extern void CleanUp_DeltaReaderWriter_Lib (void);

#endif	// Do not add stuff beyond this point.
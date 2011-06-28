// GenericWriter.h
// GameMaker Project.
// A. Savidis, March 2009.
//

#ifndef	GENERICWRITER_H
#define	GENERICWRITER_H

#include <stdio.h>
#include "utypes.h"
#include "ustrings.h"
#include "uptr.h"
#include "ubinaryio.h"
#include "FileReadersDefs.h"
#include <string>

//------------------------------------------------------

class GenericWriter {
	public:
	virtual void write (const std::string& s) = 0;
	virtual void write (const char* s) = 0;
	virtual void write (util_ui32 b) = 0;
	virtual void write (util_i32 b) = 0;
	virtual void write (util_ui16 b) = 0;
	virtual void write (util_i16 b) = 0;
	virtual void write (util_ui8 b) = 0;
	virtual void write (float f) = 0;
	virtual void write (double f) = 0;
	virtual void write (bool b) = 0;
	virtual void write (const void* b, util_ui32 n) = 0;
	virtual const std::string context (void) const = 0;

	GenericWriter (void){}
	virtual ~GenericWriter(){}
};

//------------------------------------------------------
// Do not export as having only inline members.

class PortableBinFileWriter : public GenericWriter {

	private:
	FILE* fp;

	public:
	virtual void write (const std::string& s)		{ DASSERT(fp); ubinaryio::WriteString(fp, s); }
	virtual void write (const char* s)				{ DASSERT(fp); ubinaryio::WriteString(fp, s); }
	virtual void write (util_ui32 b)				{ DASSERT(fp); ubinaryio::WriteInteger(fp, b); }
	virtual void write (util_i32 b)					{ DASSERT(fp); ubinaryio::WriteInteger(fp, b); }
	virtual void write (util_ui16 b)				{ DASSERT(fp); ubinaryio::WriteInteger(fp, b); }
	virtual void write (util_i16 b)					{ DASSERT(fp); ubinaryio::WriteInteger(fp, b); }
	virtual void write (util_ui8 b)					{ DASSERT(fp); ubinaryio::WriteByte(fp, b); }
	virtual void write (float b)					{ DASSERT(fp); ubinaryio::WriteDouble(fp, b); }
	virtual void write (double b)					{ DASSERT(fp); ubinaryio::WriteDouble(fp, b); }
	virtual void write (bool b)						{ DASSERT(fp); ubinaryio::WriteBool(fp, b); }
	virtual void write (const void* b, util_ui32 n) { DASSERT(fp); ubinaryio::WriteBuffer(fp, b, n); }
	virtual const std::string context (void) const
		{ return uconstructstr("[BIN FILE(0x%x)]", (util_ui32) fp); }

	PortableBinFileWriter (FILE* _fp) : fp(_fp){}
	PortableBinFileWriter (const PortableBinFileWriter& r) : fp(r.fp){}
	~PortableBinFileWriter(){}
};

//------------------------------------------------------
// Do not export as having only inline members.

class PortableBufferWriter : public GenericWriter {
 
	private:
	ubinaryio::OutputBuffer& output;

	public:
	virtual void write (const std::string& s)		{ output.WriteString(s); }
	virtual void write (const char* s)				{ output.WriteString(s); }
	virtual void write (util_ui32 b)				{ output.WriteInteger(b); }
	virtual void write (util_i32 b)					{ output.WriteInteger(b); }
	virtual void write (util_ui16 b)				{ output.WriteInteger(b); }
	virtual void write (util_i16 b)					{ output.WriteInteger(b); }
	virtual void write (util_ui8 b)					{ output.WriteByte(b); }
	virtual void write (float b)					{ output.WriteDouble(b); }
	virtual void write (double b)					{ output.WriteDouble(b); }
	virtual void write (bool b)						{ output.WriteBool(b); }
	virtual void write (const void* b, util_ui32 n) { output.WriteBuffer(b, n); }
	virtual const std::string context (void) const
		{ return "[output buffer]"; }

	PortableBufferWriter (ubinaryio::OutputBuffer& _output) : output(_output){}
	PortableBufferWriter (const PortableBufferWriter& r) : output(r.output){}
	virtual ~PortableBufferWriter(){}
};

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.
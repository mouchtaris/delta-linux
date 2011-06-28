// GenericReader.h
// GameMaker Project.
// A. Savidis, March 2007.
//

#ifndef	GENERICREADER_H
#define	GENERICREADER_H

#include <stdio.h>
#include "utypes.h"
#include "ustrings.h"
#include "uptr.h"
#include "ubinaryio.h"
#include "FileReadersDefs.h"
#include <string>

//------------------------------------------------------

class GenericReader {
	public:
	virtual bool eof (void) = 0;
	virtual bool read (std::string& at, bool quoted) = 0;
	virtual bool read (util_ui32* b) = 0;
	virtual bool read (util_i32* b) = 0;
	virtual bool read (util_ui16* b) = 0;
	virtual bool read (util_i16* b) = 0;
	virtual bool read (util_ui8* b) = 0;
	virtual bool read (float* f) = 0;
	virtual bool read (double* f) = 0;
	virtual bool read (bool* b) = 0;
	virtual bool read (void* b, util_ui32 n) = 0;
	virtual const std::string context (void) const = 0;

	GenericReader (void){}
	virtual ~GenericReader(){}
};

//------------------------------------------------------
// Do not export as having only inline members.

class TextFileReader : public GenericReader {
	private:
	FILE* fp;

	template <class T> bool readint (T* b) {
		util_ui32 val; 
		if (fscanf(fp, "%u", &val) != 1) 
			return false; 
		*b = (T) val;				
		return true;
	}

	public:
	bool eof (void) { return feof(fp); }
	bool read (std::string& at, bool quoted)
			{ return quoted ? ureadquotedstring(fp, at) : ureadstring(fp, at); }
	bool read (util_ui32* b) 
			{ return fscanf(fp, "%u", b) == 1; }
	bool read (util_i32* b)
			{ return fscanf(fp, "%d", b) == 1; }
	bool read (util_ui16* b) { return readint(b); }
	bool read (util_i16* b){ return readint(b); }
	bool read (util_ui8* b) { return readint(b); }
	bool read (float* b) { return fscanf(fp, "%f", b) == 1; }

	bool read (double* b) { 
		float f; 
		bool r = (fscanf(fp, "%f", &f) == 1);
		*b = f;
		return r;
	}

	bool read (bool* b) 
		{ return ureadbool(fp, b); }

	bool read (void* b, util_ui32 n) {
			while (n--) {
				util_ui32 byte;
				if (fscanf(fp, "%u", &byte) != 1)
					return false;
				*((util_ui8*) b) = (util_ui8) byte;
				PINC(b, 1);
			}
			return true;
	}

	const std::string context (void) const
		{ return uconstructstr("[TEXT FILE(0x%x)]", (util_ui32) fp); }

	TextFileReader (FILE* _fp) : fp(_fp){}
	TextFileReader (const TextFileReader& r) : fp(r.fp){}
	~TextFileReader(){}
};

//------------------------------------------------------
// Do not export as having only inline members.

class BinFileReader : public GenericReader {
	private:
	FILE* fp;
	template <class T> bool read_template (T* b)
		{ DASSERT(fp); return fread(b, sizeof(*b), 1, fp) == 1; }

	public:
	bool eof (void) { DASSERT(fp); return feof(fp); }
	bool read (std::string& at, bool quoted)
			{ DASSERT(fp); return uloadstrex(fp, at); }

	bool read (util_ui32* b)	{ return read_template(b); }
	bool read (util_i32* b)		{ return read_template(b); }
	bool read (util_ui16* b)	{ return read_template(b); }
	bool read (util_i16* b)		{ return read_template(b); }
	bool read (util_ui8* b)		{ return read_template(b); }
	bool read (float* b)		{ return read_template(b); }
	bool read (double* b)		{ return read_template(b); }
	bool read (bool* b)			{ return read_template(b); }

	bool read (void* b, util_ui32 n) 
		{ DASSERT(fp); return fread(b, sizeof(util_ui8), n, fp) == n; }

	const std::string context (void) const
		{ DASSERT(fp); return uconstructstr("[BIN FILE(0x%x)]", (util_ui32) fp); }

	BinFileReader (FILE* _fp) : fp(_fp){}
	BinFileReader (const BinFileReader& r) : fp(r.fp){}
	~BinFileReader(){}
};

//------------------------------------------------------
// Do not export as having only inline members.

class PortableBinFileReader : public GenericReader {

	private:
	FILE* fp;
	bool read_float (float* b) { 
		double v; 
		if (!ubinaryio::ReadDouble(fp, &v)) 
			return false;
		else
			{ *b = (float) v; return true; }
	}

	public:
	bool eof (void) 
			{ DASSERT(fp); return feof(fp); }

	bool read (std::string& at, bool quoted)
			{ DASSERT(fp); return ubinaryio::ReadString(fp, at); }

	bool read (util_ui32* b)	{ return ubinaryio::ReadInteger(fp, b); }
	bool read (util_i32* b)		{ return ubinaryio::ReadInteger(fp, b); }
	bool read (util_ui16* b)	{ return ubinaryio::ReadInteger(fp, b); }
	bool read (util_i16* b)		{ return ubinaryio::ReadInteger(fp, b); }
	bool read (util_ui8* b)		{ return ubinaryio::ReadByte(fp, b); }
	bool read (float* b)		{ return read_float(b); }
	bool read (double* b)		{ return ubinaryio::ReadDouble(fp, b); }
	bool read (bool* b)			{ return ubinaryio::ReadBool(fp, b); }

	bool read (void* b, util_ui32 n) 
		{ DASSERT(fp); return ubinaryio::ReadBuffer(fp, b, n); }

	const std::string context (void) const
		{ DASSERT(fp); return uconstructstr("[PORTABLE BIN FILE(0x%x)]", (util_ui32) fp); }

	PortableBinFileReader (FILE* _fp) : fp(_fp){}
	PortableBinFileReader (const PortableBinFileReader& r) : fp(r.fp){}
	~PortableBinFileReader(){}
};

//------------------------------------------------------
// Do not export as having only inline members.

class PortableBufferReader : public GenericReader {

	private:
	ubinaryio::InputBuffer* input;

	bool read_float (float* b) { 
		double v; 
		if (!input->ReadDouble(&v)) 
			return false;
		else
			{ *b = (float) v; return true; }
	}

	public:
	bool eof (void) 
		{ return input->IsEof(); }

	bool read (std::string& at, bool quoted)
			{ return input->ReadString(at); }

	bool read (util_ui32* b)	{ return input->ReadInteger(b); }
	bool read (util_i32* b)		{ return input->ReadInteger(b); }
	bool read (util_ui16* b)	{ return input->ReadInteger(b); }
	bool read (util_i16* b)		{ return input->ReadInteger(b); }
	bool read (util_ui8* b)		{ return input->ReadByte(b); }
	bool read (float* b)		{ return read_float(b); }
	bool read (double* b)		{ return input->ReadDouble(b); }
	bool read (bool* b)			{ return input->ReadBool(b); }

	bool read (void* b, util_ui32 n) 
		{ return input->ReadBuffer(b, n); }

	const std::string context (void) const{
		return	uconstructstr(
					"[input buffer: size %d, remaining %d]", 
					input->GetSize(),
					input->GetRemainingBytes()
				);
	}

	PortableBufferReader (ubinaryio::InputBuffer* b) : input(b){}
	PortableBufferReader (const PortableBufferReader& r) : input(r.input){}
	~PortableBufferReader(){}
};

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.
// BufferFile.h
// Class supporting binary reading from a memory buffer
// as if it was a binary file.
// GameMaker Project.
// A. Savidis, May 2000.
// Refactored February 2008, made independent of resource map.
//

#ifndef	BUFFERFILE_H
#define	BUFFERFILE_H

//------------------------------------------------------

#include "ustrings.h"
#include "utypes.h"
#include "GenericReader.h"
#include "FileReadersDefs.h"

#include <string>

class READERS_CLASS BufferFile {

	protected:
	void*		buffer;		// File buffer.
	util_ui32	size;		// Total size.
	void*		currPtr;	// Current pos.
	util_ui32	currIndex;	// Current byte.

	public:
	virtual bool	bfeof (void) const;
	virtual void	bfreload (void* buffer, util_ui32 size);

	util_ui32		bfread (void* inBuffer, util_i32 itemSize, util_ui32 n);	// Returns #items actually read.
	util_ui32		bfbytesleft (void) const;
	util_ui32		bfsize (void) const;

	// Load a string written in ustorestr() format.
	//
	char*			bfloadstr (void);				// Dynamic copy.
	bool			bfloadstr (std::string& at);	// At a string.
	bool			bfloadstr (char* s);			// At a buffer.

	BufferFile (void* buffer, util_ui32 size);
	virtual ~BufferFile();
};

//------------------------------------------------------

class BufferFileReader : public GenericReader {
	private:
	BufferFile& f;

	public:
	bool eof (void) 
			{ return f.bfeof(); }
	bool read (std::string& at, bool quoted)
			{ return f.bfloadstr(at); }
	bool read (util_ui32* b) 
			{ return f.bfread(b, sizeof(*b), 1); }
	bool read (util_i32* b)
			{ return f.bfread(b, sizeof(*b), 1); }
	bool read (util_ui16* b)
			{ return f.bfread(b, sizeof(*b), 1); }
	bool read (util_i16* b)
			{ return f.bfread(b, sizeof(*b), 1); }
	bool read (util_ui8* b)
			{ return f.bfread(b, sizeof(*b), 1); }
	bool read (float* b)
			{ return f.bfread(b, sizeof(*b), 1); }
	bool read (double* b)
			{ return f.bfread(b, sizeof(*b), 1); }
	bool read (bool* b)
			{ return f.bfread(b, sizeof(*b), 1); }
	bool read (void* b, util_ui32 n)
			{ return f.bfread(b, sizeof(util_ui8), n); }

	const std::string context (void) const
		{ return uconstructstr("[Bytes left %d]", f.bfbytesleft()); }

	BufferFileReader (BufferFile& _f) : f(_f){}
	BufferFileReader (const BufferFileReader& r) : f(r.f){}

	~BufferFileReader(){}
};

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.
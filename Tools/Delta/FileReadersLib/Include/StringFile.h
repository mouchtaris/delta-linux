// StringFile.h
// Emulation of text file reading facilities when instead of a
// FILE* pointer, a null terminated string buffer, containing
// all the text content of a file is used. Reading is not destructive,
// and it does not affect the string buffer passed.
// GameMaker Project.
// A. Savidis, May 2000.
// Refactored February 2008, made independent of resource map.
//

#ifndef	STRINGFILE_H
#define	STRINGFILE_H

#include "GenericReader.h"
#include "ustrings.h"
#include "uptr.h"
#include "FileReadersDefs.h"
#include <string>

//------------------------------------------------------

// Read argument type. This is to be passed to
// sfread.
//
enum sfarg_t {

	sf_char		=0,		// char
	sf_short	=1,		// short
	sf_ushort	=2,		// unsigned short
	sf_int		=3,		// int
	sf_uint		=4,		// unsigned int
	sf_long		=5,		// long
	sf_ulong	=6,		// unsigned long
	sf_string	=7,		// char*, no spaces included.
	sf_float	=8,		// float
	sf_double	=9,		// double
	sf_bool		=10		// boolean
};

#define	STRINGFILE_ARGTYPES	11

//------------------------------------------------------

class READERS_CLASS StringFile {

	protected:

	////////////////////////////////////////////

	char*				buffer;			// The string buffer.
	util_ui32			size;			// The file size (not including the null terminator).
	char*				currPtr;		// The current position in the buffer.
	util_ui32			currIndex;		// The index to the current position.
	util_ui32			line;			// Current line, counts from 1.
	util_ui32			col;			// Current col, counts from 1.

	typedef bool (*ReadFunc) (StringFile* sf, void* arg);
	static ReadFunc readFuncs[STRINGFILE_ARGTYPES];

	static bool read_char (StringFile* sf, void* arg);
	static bool read_string (StringFile* sf, void* arg);

	template <class T> friend T		readoptionalsign (StringFile* sf);
	template <class T> friend bool	readreal (StringFile* sf, void* arg);
	template <class T> friend bool	readsignedint (StringFile* sf, void* arg, bool hasSign);

	void	sfnextchar (void);

	////////////////////////////////////////////

	public:

	virtual bool	sfeof (void) const;							// As in C.
	virtual void	sfreload (char* buffer, util_ui32 size);	// Use new buffer.

	bool			sfread (sfarg_t type, void* arg);			// Reads a single arg. Returns success status.
	char			sfgetc (void);								// As in C.
	char*			sfgets (char* buffer, util_ui32 n);			// As in C.
	bool			sfgets(std::string& s);						// Get any string
	char*			sfgetqstring (char* buffer, util_ui32 n);	// Get a quoted string, up to 'n' chars.
	bool			sfgetqstring (std::string& s);				// Get any quoted string!
	util_ui32		sfcharsleft (void) const;					// How many characters left to EOF.
	util_ui32		sfsize (void) const;						// Size of string file.

	void			sfskipspaces (void);
	util_ui32		sfcurrline (void) const;					// Current line.
	util_ui32		sfcurrcol (void) const;						// Current column.

	StringFile (char* buffer, util_ui32 size);
	virtual ~StringFile();
};

//------------------------------------------------------

class StringFileReader : public GenericReader {
	private:
	StringFile& f;

	public:
	bool eof (void) 
			{ return f.sfeof(); }
	bool read (std::string& at, bool quoted)
			{ return quoted ? f.sfgetqstring(at) : f.sfgets(at); }
	bool read (util_ui32* b) 
			{ return f.sfread(sf_uint, b); }
	bool read (util_i32* b)
			{ return f.sfread(sf_int, b); }
	bool read (util_ui16* b)
			{ return f.sfread(sf_ushort, b); }
	bool read (util_i16* b)
			{ return f.sfread(sf_short, b); }

	bool read (util_ui8* b) {
			util_ui16 i = 0; 
			bool r = f.sfread(sf_ushort, &i);
			*b = (util_ui8) i;
			return r;
	}

	bool read (float* b)
			{ return f.sfread(sf_float, b); }
	bool read (double* b)
			{ return f.sfread(sf_double, b); }
	bool read (bool* b)
			{ return f.sfread(sf_bool, b); }

	bool read (void* b, util_ui32 n) {
			while (n--) {
				util_ui16 byte;
				if (!f.sfread(sf_ushort, &byte))
					return false;
				*((util_ui8*) b) = (util_ui8) byte;
				PINC(b, 1);
			}
			return true;
	}

	const std::string context (void) const
		{ return uconstructstr("[row %d, column %d]", f.sfcurrline(), f.sfcurrcol()); }

	StringFileReader (StringFile& _f) : f(_f){}
	StringFileReader (const StringFileReader& r) : f(r.f){}

	~StringFileReader(){}
};

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.
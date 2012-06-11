// StringFile.cpp
// Implementation of StringFile class.
// GameMaker Project.
// A. Savidis, May 2000.
// Refactored February 2008, made independent of resource map.
//

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "StringFile.h"
#include "ustrings.h"

#define	COIN_TAB_SIZE	8

//------------------------------------------------------

StringFile::StringFile (char* _buffer, util_ui32 _size) {
	line = col = 1;
	sfreload(_buffer, _size);
}
 
StringFile::~StringFile() {}

//------------------------------------------------------
// Reload content from a new buffer.
//
void StringFile::sfreload (char* _buffer, util_ui32 _size) {
	buffer		=
	currPtr		= _buffer;
	size		= _size;
	currIndex	= 0;
}

//------------------------------------------------------
// Simply forwards to the next character,
// updating index, ptr and col. No checking.
//
void StringFile::sfnextchar (void) {

	++currPtr;
	++currIndex;
	++col;
}

//------------------------------------------------------
// Skips spaces until non-space or end of string met.
//
void StringFile::sfskipspaces (util_ui32* lines) {

	if (!currPtr)
		return;

	while (!sfeof() && isspace(*currPtr)) {
		if (*currPtr == '\n') {			// Check new line.
			++line, col = 0;			// Since it will be increased.
			if (lines)
				++*lines;
		}
		else
		if (*currPtr == '\t')			// Check for tab space.
			col += COIN_TAB_SIZE;
		sfnextchar();
	}
}

//------------------------------------------------------

bool StringFile::sfeof (void) const
	{ return !buffer || !*currPtr; }

//------------------------------------------------------
// Reading various items of items from the string file.
// Parsing is done by reading the maximum number of
// characters fulfilling the token type, and then
// returning the value.
//
static bool read_short (StringFile* sf, void* arg);
static bool read_ushort (StringFile* sf, void* arg);
static bool read_int (StringFile* sf, void* arg);
static bool read_uint (StringFile* sf, void* arg);
static bool read_long (StringFile* sf, void* arg);
static bool read_ulong (StringFile* sf, void* arg);
static bool read_float (StringFile* sf, void* arg);
static bool read_double (StringFile* sf, void* arg);
static bool read_bool (StringFile* sf, void* arg);

// The read function dispatch table.
//
StringFile::ReadFunc StringFile::readFuncs[STRINGFILE_ARGTYPES] = {
	StringFile::read_char,
	read_short,
	read_ushort, 
	read_int,
	read_uint, 
	read_long, 
	read_ulong,
	StringFile::read_string,
	read_float, 
	read_double, 
	read_bool
};

////////////////////////////////////////////

bool StringFile::sfread (sfarg_t type, void* arg) {

	if (sfeof())
		return false;
	
	return (*readFuncs[type])(this, arg);
}

////////////////////////////////////////////
// Reading a single char.
//
bool StringFile::read_char (StringFile* sf, void* arg) {

	*((char*) arg) = *sf->currPtr;	// Get curr char.
	if (*sf->currPtr == '\n')		// Check for new line read.
		++(sf->line), sf->col = 0;
	sf->sfnextchar();				// Proceed with next.

	return true;
}

////////////////////////////////////////////

template <class T> T readoptionalsign (StringFile* sf) {
	T sign;
	if (*sf->currPtr == '-') {
		sign = -1;
		sf->sfnextchar();
	}
	else
	if (*sf->currPtr == '+') {
		sign = +1;
		sf->sfnextchar();
	}
	else
		sign = +1;
	return sign;
}

////////////////////////////////////////////
// Template signed integer reading 
// function (int, long).
//
template <class T>
bool readsignedint (StringFile* sf, void* arg, bool hasSign) {

	sf->sfskipspaces();
	if (sf->sfeof())
		return false;

	T sign = +1;
	if (hasSign)
		sign = readoptionalsign<T>(sf);

	// Now start reading digits, until a non-digit, or eof is met.
	// If the first char is not a digit, this is an error.
	//
	if (!isdigit(*sf->currPtr))
		return false;

	T value = 0;
	while (!sf->sfeof() && isdigit(*sf->currPtr)) {
		value = value * 10 + (*sf->currPtr - '0');
		sf->sfnextchar();
	}
	
	*((T*) arg) = sign * value;
	return true;
}

////////////////////////////////////////////

bool read_int (StringFile* sf, void* arg) 
	{ return readsignedint<util_i32>(sf, arg, true); }

bool read_uint (StringFile* sf, void* arg) 
	{ return readsignedint<util_ui32>(sf, arg, false); }

bool read_short (StringFile* sf, void* arg) 
	{ return readsignedint<util_i16>(sf, arg, true); }

bool read_ushort (StringFile* sf, void* arg) 
	{ return readsignedint<util_ui16>(sf, arg, false); }

bool read_long (StringFile* sf, void* arg) 
	{ return readsignedint<long>(sf, arg, true); }

bool read_ulong (StringFile* sf, void* arg) 
	{ return readsignedint<unsigned long>(sf, arg, false); }

////////////////////////////////////////////
// Reading real template function (float, double).
//
template <class T>
bool readreal (StringFile* sf, void* arg) {

	sf->sfskipspaces();
	if (sf->sfeof())
		return false;
	T sign = readoptionalsign<T>(sf);
	
	// Now start reading digits, until a non-digit, or eof is met.
	// If the first char is not a digit or '.', this is an error.
	//
	if (!isdigit(*sf->currPtr) && *sf->currPtr!='.')
		return false;

	T 		value			= 0;
	T 		realFactor		= 10;
	T 		fractFactor		= 1;
	bool	atFractional	= false;

	while (!sf->sfeof() && (isdigit(*sf->currPtr) || *sf->currPtr=='.')) {

		// If starting parsing fractional, and already did that,
		// break parsing.
		//
		if (*sf->currPtr == '.') {
			if (atFractional)
				break;
			else {
				atFractional	= true;
				realFactor		= 1;
				fractFactor		= 10;
			}
        }

		value = value*realFactor + (*sf->currPtr-'0')/fractFactor;
		sf->sfnextchar();

		if (atFractional)
			fractFactor*=10;
	}
	
	*((T*) arg)=sign*value;
	return true;
}

////////////////////////////////////////////

bool read_bool (StringFile* sf, void* arg) {

	std::string boolStr;
	if (!sf->sfgets(boolStr))
		return false;

	ustrupper(boolStr);

	if (boolStr == ubool2conststr(true))
		*((bool*) arg) = true;
	else
	if (boolStr == ubool2conststr(false))
		*((bool*) arg) = false;
	else
		return false;

	return true;
}

////////////////////////////////////////////
// Reads a float. Sign, as well as a fractional part
// are allowed. "12", "-345.0", "+.567" are legal samples.
//
bool read_float (StringFile* sf, void* arg) 
	{ return readreal<float>(sf, arg); }

////////////////////////////////////////////
// Reading a double. Same as float, only types
// differ.
//
bool read_double (StringFile* sf, void* arg) 
	{ return readreal<double>(sf, arg); }

////////////////////////////////////////////

class StringFILETextReader : public SequentialTextFileReader {

	private:
	StringFile*	sf;
	char		lastCharRead;
	
	public:
	char	getnext (void)					{ return sf->sfgetc(); }
	void	skipspaces (util_ui32* lines)	{ sf->sfskipspaces(); }
	bool	iseof (void) const				{ return sf->sfeof(); }

	StringFILETextReader (StringFile* _sf) : sf(_sf), lastCharRead('0') {}
	~StringFILETextReader (){}
};

////////////////////////////////////////////
// Reading a string. The end of string is 
// determined by a space.
//
bool StringFile::read_string (StringFile* sf, void* arg) {
	
	std::string s;
	if (ureadstring(utempobj(StringFILETextReader(sf)),s))
		{ strcpy((char*) arg, s.c_str()); return true; }
	else
		return false;
}

//------------------------------------------------------
// If eof, returns 0.
//
char StringFile::sfgetc (void) {

	if (sfeof())
		return '\0';

	char c = *currPtr;
	sfnextchar();
	return c;
}

//------------------------------------------------------
// Reads until new line, end of string or up to n-1 chars
// are read. The new line becomes part of the string.
//
char* StringFile::sfgets (char* buffer, util_ui32 n) {

	if (sfeof() || n<=0)
		return (char*) 0;

	char* s=buffer;
	while (--n && !sfeof()) {

		*s++ = *currPtr;
		sfnextchar();

		if (*(currPtr-1)=='\n')
			break;
	}
	*s='\0';

	return buffer;
}

//------------------------------------------------------

bool StringFile::sfgets (std::string& s) { 
	return ureadstring(
		utempobj(StringFILETextReader(this)),
		s
	);
}

//------------------------------------------------------

bool StringFile::sfgetqstring (std::string& s) {
	return ureadquotedstring(
		utempobj(StringFILETextReader(this)),
		s
	);
}
//------------------------------------------------------
// Get a quoted string. This should start with "
// and end with ". Any \" is put as part of the string,
// not its end.
//
char* StringFile::sfgetqstring (char* buffer, util_ui32 n) {

	return ureadquotedstring(
		utempobj(StringFILETextReader(this)),
		buffer,
		n
	);	
}

//------------------------------------------------------

util_ui32 StringFile::sfcharsleft (void) const
	{ return size-currIndex; }

util_ui32 StringFile::sfsize (void) const
	{ return size; }

util_ui32 StringFile::sfcurrline (void) const
	{ return line; }

util_ui32 StringFile::sfcurrcol (void) const
	{ return col; }

//------------------------------------------------------
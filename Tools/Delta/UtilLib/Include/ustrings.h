// ustrings.h
// String manipulation utilities.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	USTRINGS_H
#define	USTRINGS_H

#include <cstring>
#include <cstdio>
#include <cstdarg>

#include "DDebug.h"
#include "ufunctors.h"
#include "utypes.h"

#include <string>
#include <list>

//---------------------------------------------------------------
// In case we want to ensure C string is passed to typical
// C library functions.
//
#define	UBOOL_CONST_STRING_TRUE		"TRUE"
#define	UBOOL_CONST_STRING_FALSE	"FALSE"

inline 		 const char*		ucstringarg (const char* s) { return s; }
inline 		 const char*		ucstringarg (const std::string& s) { return s.c_str(); }
inline 		 const char*		ubool2conststr (bool val) { return val ? UBOOL_CONST_STRING_TRUE : UBOOL_CONST_STRING_FALSE; }
UTILLIB_FUNC bool				ureadbool (FILE* fp, bool* val);

/////////////////////////////////////////////////////////////////
// All strings returned as char* are dynamic, delete via DDELARR.
//
UTILLIB_FUNC char*				ucopystr (const std::string& s);
UTILLIB_FUNC char*				uconcatstr (const char* s1, const char* s2);
UTILLIB_FUNC void				usubstitutechars (std::string& s, const char* from, char to);	// Change any 
UTILLIB_FUNC const std::list<std::string>
								utokenizestr (const std::string& s, const char* tokens);
UTILLIB_FUNC char*				uloadstr (FILE* fp);						// binary
UTILLIB_FUNC bool				uloadstrex (FILE* fp, std::string& at);		// binary
UTILLIB_FUNC util_ui32			ustorestrsize (const std::string& s);		// binary
UTILLIB_FUNC void				ustorestr (FILE* fp, const std::string& s);	// binary
UTILLIB_FUNC char*				udeleoln (char* s);	// Returns original pointer.
UTILLIB_FUNC const std::string 	unum2string (double val);	// Only the characters needed!
UTILLIB_FUNC const std::string 	uint2hexstring (util_ui32 val);
UTILLIB_FUNC void				usupresswhitespace (const std::string& inputPath);

inline void						uappend_with_newline (std::string& dest, const std::string& src) {
									if (!src.empty()) {
										if (!dest.empty())
											dest += "\n";
										dest += src;
									}
								}

// Leaves a "..." after the 'n' characters.
UTILLIB_FUNC const std::string	ucopymaxwithsuffix (const std::string& s, util_ui32 n);

struct ustorestrfunctor : public std::binary_function<FILE*, std::string, void> {
	void operator()(FILE* fp, const std::string& s) const { ustorestr(fp, s); }
};

/////////////////////////////////////////////////////////////////
// An sbtraction of the facilities offered by a basic utility library
// to read from a sequential text file.
//
class UTILLIB_CLASS SequentialTextFileReader {
	public:
	virtual char	getnext (void) = 0;
	virtual void	skipspaces (util_ui32* lines = (util_ui32*) 0) = 0;
	virtual bool	iseof (void) const = 0;
	SequentialTextFileReader (void){}
	virtual ~SequentialTextFileReader(){}
};

//****************************

class UTILLIB_CLASS SequentialTextFILEReader : public SequentialTextFileReader {

	private:
	FILE*			fp;
	bool			useLookAhead;
	char			lookAheadChar;

	public:
	virtual char	getnext (void);
	virtual void	skipspaces (util_ui32* lines = (util_ui32*) 0);
	virtual bool	iseof (void) const;

	SequentialTextFILEReader (FILE* _fp);
	virtual ~SequentialTextFILEReader();
};

//****************************

class UTILLIB_CLASS SequentialStringReader : public SequentialTextFileReader {

	private:
	std::string				s;
	std::string::iterator	i;

	public:
	virtual char	getnext (void);
	virtual void	skipspaces (util_ui32* lines = (util_ui32*) 0);
	virtual bool	iseof (void) const;

	SequentialStringReader (const std::string& _s);
	virtual ~SequentialStringReader();
};

//****************************
// The condition is checked after a character is read successfuly
// and added to the buffer. If it is false, the read is stopped.
//

UTILLIB_FUNC bool				ureadstring (
									SequentialTextFileReader&	reader,
									std::string&				result,
									uconditionalfunctor&		condition
								);

inline	bool					ureadstring (
									SequentialTextFileReader&	reader,
									std::string&				result
								) { utrueconditionalfunctor cond; return ureadstring(reader, result, cond); }

UTILLIB_FUNC char*				ureadquotedstring (
									SequentialTextFileReader&	reader, 
									char*						buffer, 
									util_ui16					n
								);
		
UTILLIB_FUNC bool				ureadquotedstring (
									SequentialTextFileReader&	reader, 
									std::string&				result
								);

/////////////////////////////////////////////////////////////////

UTILLIB_FUNC bool				ustrprefix (const char* prefix, const char* s);
UTILLIB_FUNC bool				ustrprefix (const std::string& prefix, const std::string& s);
UTILLIB_FUNC bool				ustrsuffix (const char* suffix, const char* s, bool pureSuffix = true);
UTILLIB_FUNC bool				ustrsuffix (const std::string& suffix, const std::string& s, bool pureSuffix = true);
UTILLIB_FUNC char*				ustrnmaxcpy (char* dest, const char* src, util_ui16 maxLen);
UTILLIB_FUNC void				ustrnmaxcpy (std::string& dest, const char* src, util_ui16 maxLen);
UTILLIB_FUNC char*				umaxstrcat (char* dest, const char* src, util_ui16 maxLen);
UTILLIB_FUNC char*				ucopystr (const char* str, util_ui16 maxLen);
UTILLIB_FUNC char*				ustrreverse (char* src);	// modifies and returns arg
UTILLIB_FUNC std::string&		ustrreverse (std::string& src);
inline const std::string		ustrreverse (const std::string& src)
									{ return ustrreverse(utempobj(std::string(src))); }
inline const std::string		ustrreverse (const char* src)
									{ return ustrreverse(utempobj(std::string(src))); }
UTILLIB_FUNC char*				ustrupper (char* s);		// modifies and returns arg
UTILLIB_FUNC char*				ustrlower (char* s);		// modifies and returns arg
UTILLIB_FUNC std::string&		ustrupper (std::string& s);
UTILLIB_FUNC std::string&		ustrlower (std::string& s);
UTILLIB_FUNC const std::string	ustrupper (const std::string& s);
UTILLIB_FUNC const std::string	ustrlower (const std::string& s);
inline const std::string		ustrupper (const char* s) 
									{ return ustrupper(std::string(s)); }
inline const std::string		ustrlower (const char* s) 
									{ return ustrlower(std::string(s)); }
UTILLIB_FUNC util_ui16			ustrpos (const char** arr, util_ui16 total, const char* s);
UTILLIB_FUNC char*				ureadquotedstring (FILE* fp, char* buffer, util_ui16 maxLen);
UTILLIB_FUNC bool				ureadquotedstring (FILE* fp, std::string& s);
UTILLIB_FUNC char*				ureadstring (FILE* fp, char* buffer, util_ui16 maxLen);
UTILLIB_FUNC bool				ureadstring (FILE* fp, std::string& s);
UTILLIB_FUNC char				uskipspaces (FILE* fp, util_ui32* lines = (util_ui32*) 0);
UTILLIB_FUNC void				utabstops (char* s, util_ui16 total);
UTILLIB_FUNC const std::string	utabstops (util_ui16 total);
UTILLIB_FUNC const std::string	uconstructstr (const char* format,...);
UTILLIB_FUNC void				uvsprintf (std::string& at, const char* format, va_list args);
UTILLIB_FUNC const std::string	ucropstr (const std::string& s, util_ui32 n, const std::string& suffix = "");
UTILLIB_FUNC const std::string	ustrconcat (const ustrlist& l, const std::string& separator = "");

// ARGV (command line arguments).
UTILLIB_FUNC const std::list<std::string> 
								uargstostringlist (const char* args);
UTILLIB_FUNC char**				ustringlisttoargv (const std::list<std::string>& l, int* argc = (int*) 0);
UTILLIB_FUNC void				uclearargv (char**& argv, int argc);

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

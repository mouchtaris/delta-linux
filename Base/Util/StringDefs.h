/**
 *	StringDefs.h
 *
 *	-- Utility Library --
 *
 *	December 2005
 */

#ifndef UTIL_STRINGDEFS_H
#define UTIL_STRINGDEFS_H

#include <string>
#include <iosfwd>

#include "Portability.h"

////////////////////////////////////////////////////////////////////////
// Default string class
//
#ifdef WXWIDGETS

#include <wx/string.h>
#include <wx/intl.h>
typedef wxString String;

#else // ! WXWIDGETS

typedef std::string String;

#ifndef _
#define _(STR) (STR)
#endif

#endif // WXWIDGETS

////////////////////////////////////////////////////////////////////////
// Unix & unicode function compatibility
//
#ifdef WIN32

#define VSNPRINTF 	_vsnprintf
#define SNPRINTF	_snprintf

#ifdef UNICODE

#define STRLEN		wcslen
#define STRCASECMP	wcsicmp
#define STRNCASECMP wcsnicmp
#define STRCMP		wcscmp
#define STRNCMP		wcsncmp

#else

#define STRLEN		strlen
#define STRCASECMP	stricmp
#define STRNCASECMP strnicmp
#define STRCMP		strcmp
#define STRNCMP		strncmp

#endif // UNICODE

#else // !WIN32

#define VSNPRINTF 	vsnprintf
#define SNPRINTF	snprintf

#ifdef UNICODE

#define STRLEN		wcslen
#define STRCASECMP	wcscasecmp
#define STRNCASECMP wcsncasecmp
#define STRCMP		wcscmp
#define STRNCMP		wcsncmp

#else

#define STRLEN		strlen
#define STRCASECMP	strcasecmp
#define STRNCASECMP strncasecmp
#define STRCMP		strcmp
#define STRNCMP		strncmp

#endif // UNICODE

#endif // WIN32

////////////////////////////////////////////////////////////////////////

namespace util {

////////////////////////////////////////////////////////////////////////
// Typedefs
//
typedef String::char_type char_type;

////////////////////////////////////////////////////////////////////////
// Conversions
//
#ifdef WXWIDGETS

#ifdef UNICODE

inline const std::string str2std (const wxString& str) {
	std::string std(str.mb_str(wxConvUTF8));
	if (std.empty() != str.empty())	//conversion failed
		std = std::string(str.mb_str(wxConvISO8859_1));
	if (std.empty() != str.empty())	//conversion failed
		std = std::string(str.mb_str(wxConvLibc));
	return std;
}

inline const wxString std2str (const std::string& std) {
	wxString str(std.c_str(), wxConvUTF8);
	if (str.empty() != std.empty())	//conversion failed
		str = wxString(std.c_str(), wxConvISO8859_1);
	if (str.empty() != std.empty())	//conversion failed
		str = wxString(std.c_str(), wxConvLibc);
	return str;
}

#else // ! UNICODE

inline const std::string str2std (const wxString& str)
	{ return std::string(str.WX_C_STR(), str.size()); }

inline const wxString std2str (const std::string& std)
	{ return wxString(std.WX_C_STR(), std.size()); }

#endif // UNICODE

#else // ! WXWIDGETS

inline const std::string&	str2std (const String& str) { return str; }
inline const String&		std2str (const std::string& std) { return std; }

#endif // WXWIDGETS

////////////////////////////////////////////////////////////////////////

} // namespace util

////////////////////////////////////////////////////////////////////////
// Streams
//
#ifdef WXWIDGETS

inline std::ostream& operator<< (std::ostream& stream, const wxString& str)
{
	stream << util::str2std(str);
	return stream;
}

//**********************************************************************

inline std::istream& operator>> (std::istream& stream, wxString& str)
{
	std::string tmp;
	stream >> tmp;
	str = util::std2str(tmp);
	return stream;
}

#endif // WXWIDGETS

////////////////////////////////////////////////////////////////////////

#endif // UTIL_STRINGDEFS_H

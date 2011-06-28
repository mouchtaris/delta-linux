/*
 * StringHelpers.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef STRINGHELPERS_H
#define STRINGHELPERS_H

#include "wx/wx.h"

#ifdef UNICODE

inline const std::string wx2std (const wxString& str)
{ return std::string(str.mb_str(wxConvUTF8)); }

inline const wxString std2wx (const std::string& std) 
	{ return wxString(std.c_str(), wxConvUTF8); }

#define	FIND_REPLACE_SEARCH_ERROR_TITLE	"Search error"
#define	FIND_REPLACE_SEARCH_MSG_TITLE	"Search message"

#else // ! UNICODE

inline const std::string wx2std (const wxString& str) 
	{ return std::string(str.str(), str.size()); }

inline const wxString std2wx (const std::string& std)
	{ return wxString(std.c_str(), std.size()); }

#endif

#endif	// STRINGHELPERS_H
/**
 *	StringUtils.cpp
 *
 *	-- Utility Library --
 *
 *	December 2005
 */

#include "StringUtils.h"
#include "Algorithms.h"

#include <wx/filename.h>

namespace util {

////////////////////////////////////////////////////////////////////////

void stringrangetokenizer (
			range_type_list&		result,
			const String&			in,
			const char_type* const	delimiters
	)
{
	const String::size_type len = in.length();
	String::size_type i = 0;

	while (i < len) {
		i = in.find_first_not_of(delimiters, i);

		if (i == String::npos)
			return;
		String::size_type j = in.find_first_of(delimiters, i);
		if (j == String::npos) {
			result.push_back(range_type(i, in.length() - i));
			return;
		}
		else
			result.push_back(range_type(i, j - i));
		i = j + 1;
	}
}

////////////////////////////////////////////////////////////////////////

void getwordbounds (
			const String&			input,
			String::size_type		position,
			String::size_type*		begin,
			String::size_type*		end,
			const char_type* const	delimiters
	)
{
	assert(position < input.size());
	assert(begin && end);

	String::size_type first = input.find_last_of(delimiters, position);
	String::size_type last = input.find_first_of(delimiters, position);

	if (first == String::npos)
		first = 0;
	else if (first < last)
		++first;

	if (last == String::npos)
		last = input.size();

	*begin = first;
	*end = last;
}

//**********************************************************************

const String getword (
			const String&			input,
			String::size_type		position,
			const char_type* const	delimiters
	)
{
	String::size_type begin, end;
	getwordbounds(input, position, &begin, &end, delimiters);
	String::size_type len = (end != String::npos) ? end - begin : String::npos;
	return input.substr(begin, len);
}

//**********************************************************************

void getnextwordbounds (
			const String&			input,
			String::size_type		position,
			String::size_type*		begin,
			String::size_type*		end,
			const char_type* const	delimiters
	)
{
	String::size_type pos = input.find_first_of(delimiters, position);
	if (
		pos == String::npos || pos == input.size() ||
		(pos = input.find_first_not_of(delimiters, pos)) == String::npos
	) {
		pos = position;
	}
	getwordbounds(input, pos, begin, end, delimiters);
}

//**********************************************************************

void getprevwordbounds (
			const String&			input,
			String::size_type		position,
			String::size_type*		begin,
			String::size_type*		end,
			const char_type* const	delimiters
	)
{
	String::size_type pos = input.find_last_of(delimiters, position);
	if (
		pos == String::npos || pos == input.size() ||
		(pos = input.find_last_not_of(delimiters, pos)) == String::npos
	) {
		pos = position;
	}
	getwordbounds(input, pos, begin, end, delimiters);
}

////////////////////////////////////////////////////////////////////////

bool strcasecmp (const String& str1, const String& str2)
	{ return !STRCASECMP(str1.c_str(), str2.c_str()); }

////////////////////////////////////////////////////////////////////////

bool filepathsequal (const String& path1, const String& path2)
	{ return wxFileName(path1).SameAs(path2); }

//**********************************************************************

const String normalizeslashes(const String& str, wxChar *from, wxChar *to)
{
	String tmp = str;
	tmp.Replace(from, to);
	return tmp;
}

const String normalizepath (const String& path)
{
	wxFileName filename(path);
	const int normalizationFlags = wxPATH_NORM_ALL & ~wxPATH_NORM_ABSOLUTE;
	filename.Normalize(normalizationFlags);
	String tmp;
	if (filename.IsDir())
		tmp = filename.GetPath();
	else
		tmp = filename.GetFullPath();
	return normalizeslashes(tmp);
}

#ifdef WXWIDGETS
const std::string normalizepath (const std::string& path)
	{ return str2std(normalizepath(std2str(path))); }
#endif

//**********************************************************************

const String quotepath (const String& path, wxChar quote)
	{ return quote + normalizeslashes(path) + quote; }

#ifdef WXWIDGETS
const std::string quotepath (const std::string& path, char quote)
	{ return quote + str2std(normalizeslashes(std2str(path))) + quote; }
#endif

//**********************************************************************

} // namespace util

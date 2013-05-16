/**
 *	StringUtils.h
 *
 *	-- Utility Library --
 *
 *	December 2005
 */

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include "Common.h"
#include "LibraryAPIDecls.h"

namespace util {

////////////////////////////////////////////////////////////////////////

typedef std::pair<String::size_type, String::size_type> range_type;
typedef std::list<range_type>							range_type_list;

////////////////////////////////////////////////////////////////////////

#define UTIL_WORD_DELIMITERS	" \t\n:;.,!?()"
#define UTIL_WHITESPACE			" \t\n"

////////////////////////////////////////////////////////////////////////

template <class TString, class TChar>
const TString strip (
		const TString&			in,
		const typename TChar*	symbols
	)
{
	TString result;
	return strip<TString, TChar>(in, result, symbols);
}

//**********************************************************************

template <class TString, class TChar>
const TString& strip (
		const TString&			in,
		TString&				storage,
		const typename TChar*	symbols
	)
{
	typename TString::size_type startPos = in.find_first_not_of(symbols);
	typename TString::size_type endPos = in.find_last_not_of(symbols);

	if (startPos == 0 && endPos == in.length() - 1)
		return in;
	else if (startPos == String::npos)
		return storage;
	else {
		assert(endPos != String::npos);
		storage = in.substr(startPos, endPos - startPos + 1);
		return storage;
	}
}

////////////////////////////////////////////////////////////////////////

_BASE_API void stringrangetokenizer (
			range_type_list&		container,
			const String&			in,
			const char_type* const	delimiters = _T(UTIL_WHITESPACE)
);

//**********************************************************************

template <typename TContainer, class TString, class TChar>
void stringtokenizer (
			TContainer&			container,
			const TString&		in,
			const TChar* const	delimiters
) {
	const typename TString::size_type len = in.length();
	typename TString::size_type i = 0;

	while (i < len) {
		i = in.find_first_not_of(delimiters, i);

		if (i == TString::npos)
			return;
		typename TString::size_type j = in.find_first_of(delimiters, i);
		if (j == TString::npos) {
			container.push_back(in.substr(i));
			return;
		}
		else
			container.push_back(in.substr(i, j-i));
		i = j + 1;
	}
}

//**********************************************************************

template <typename TContainer, class TString>
const TString stringconcat (const TContainer& container, const TString& delim)
{
	assert(!container.empty());

	typename TContainer::const_iterator iter = container.begin();
	TString result(*iter);
	while (++iter != container.end())
		result += delim + *iter;
	return result;
}

////////////////////////////////////////////////////////////////////////

_BASE_API void getwordbounds (
			const String&			input,
			String::size_type		position,
			String::size_type*		begin,
			String::size_type*		end,
			const char_type* const	delimiters = _T(UTIL_WORD_DELIMITERS)
	);

_BASE_API const String getword (
			const String&			input,
			String::size_type		position,
			const char_type* const	delimiters = _T(UTIL_WORD_DELIMITERS)
	);

//**********************************************************************

_BASE_API void getnextwordbounds (
			const String&			input,
			String::size_type		position,
			String::size_type*		begin,
			String::size_type*		end,
			const char_type* const	delimiters = _T(UTIL_WORD_DELIMITERS)
	);

_BASE_API void getprevwordbounds (
			const String&			input,
			String::size_type		position,
			String::size_type*		begin,
			String::size_type*		end,
			const char_type* const	delimiters = _T(UTIL_WORD_DELIMITERS)
	);

////////////////////////////////////////////////////////////////////////

template <class CharPred>
void getcharacterbounds (const String& input, std::list<range_type>& result, CharPred& pred)
{
	bool insideMatching = false;
	range_type currrange_type;

	for (String::size_type currChar = 0; currChar < input.size(); ++currChar) {

		if (pred(input[currChar])) {

			String::size_type endPos = input.find_first_of(_T(UTIL_WHITESPACE), currChar);
			if (endPos != String::npos)
				endPos = input.find_first_not_of(_T(UTIL_WHITESPACE), endPos);

			if (!insideMatching) {
				currrange_type.first = currChar;
				insideMatching = true;
			}
			else
				currrange_type.second = currChar = endPos;

			if (endPos == String::npos)
				break;
		}
		else if (insideMatching) {
			result.push_back(currrange_type);
			insideMatching = false;
		}
	}
	if (insideMatching)
		result.push_back(range_type(currrange_type.first, input.size()));
}

////////////////////////////////////////////////////////////////////////

_BASE_API bool strcasecmp (const String& str1, const String& str2);

////////////////////////////////////////////////////////////////////////
// Path related functions

_BASE_API bool filepathsequal (const String& path1, const String& path2);

//**********************************************************************

_BASE_API const String normalizeslashes(const String& str, wxChar *from = _T("\\"), wxChar *to = _T("/"));

_BASE_API const String normalizepath (const String& path);

#ifdef WXWIDGETS
_BASE_API const std::string normalizepath (const std::string& path);
#endif

//**********************************************************************

_BASE_API const String quotepath (const String& path, wxChar quote = _T('"'));

#ifdef WXWIDGETS
_BASE_API const std::string quotepath (const std::string& path, char quote = '"');
#endif

////////////////////////////////////////////////////////////////////////

} // namespace util

#endif // STRINGUTILS_H

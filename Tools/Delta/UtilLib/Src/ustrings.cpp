// ustrings.cpp
// Implementation of string utilities.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#include "ustrings.h"
#include "ugeometry.h"
#include "ufiles.h"
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

//------------------------------------------------------------------
// Copy min(maxLen, strlen(src)+1) characters
// on dest (i.e. including the null terminator).
//
UTILLIB_FUNC char* ustrnmaxcpy (char* dest, const char* src, util_ui16 maxLen) {

	DASSERT(maxLen);

	if (!src)
		return dest;
	else
	if (!dest)
		return (char*) 0;
	else {

		char*		p;
		util_ui32	n = maxLen - 1;	// Since we put separately the null terminator.

		for (p = dest; n && *src; ++src, ++p, --n)
			*p = *src;

		DASSERT(!*src || !n);

		*p = '\0';			// Null terminate.

		DASSERT(strlen(dest) < maxLen);

		return dest;
	}
}

//------------------------------------------------------------------

UTILLIB_FUNC void ustrnmaxcpy (std::string& dest, const char* src, util_ui16 maxLen) {

	DASSERT(src && maxLen);
	
	char		add[2] = { '\0', '\0' };
	util_ui32	n = maxLen - 1;

	for (; n && (add[0] = *src); ++src, --n)
		dest.append(add);

	DASSERT(!*src || !n);
	DASSERT(dest.length() < maxLen);
}

UTILLIB_FUNC void usubstitutechars (std::string& s, const char* from, char to) {
	for (std::string::iterator i = s.begin(); i != s.end(); ++i)
		if (strchr(from, *i))
			*i = to;
}

//------------------------------------------------------------------

UTILLIB_FUNC const std::list<std::string> utokenizestr (const std::string& s, const char* tokens) {
	
	std::list<std::string>	result;
	std::string				curr;

	for (std::string::const_iterator i = s.begin(); i!= s.end(); ++i)
		if (strchr(tokens, *i)) {
			if (!curr.empty()) 
				result.push_back(curr); 
			curr.clear();
		}
		else
			curr += *i;

	if (!curr.empty())
		result.push_back(curr);

	return result;
}

//------------------------------------------------------------------

UTILLIB_FUNC char* umaxstrcat (char* dest, const char* src, util_ui16 maxLen) {

	// If length of dest has reached maxLen-1, then strcat is denied.
	//
	util_ui16 len = (util_ui16) strlen(dest);
	if (len == maxLen-1)
		return dest;

	// Else, reach the end of 'dest', over null terminator, and start
	// writing 'src' on it'
	//
	for (dest += len; *src && len < maxLen; *dest++ = *src++, ++len)
		;
	*dest = '\0';

	return dest;
}

//------------------------------------------------------------------
// Makes a string copy up to 'n' characters including
// the null terminator.
//
UTILLIB_FUNC char* ucopystr (const char* s, util_ui16 n) {

	if(!s)
		return (char*) 0;

	util_ui16 len = (util_ui16) strlen(s) + 1;
	if (len > n)
		len = n;

	char* cpy = DNEWARR(char,len);
	DASSERT(cpy);

	memcpy((void*) cpy, (void*) s, len - 1);
	cpy[len-1] = '\0';

	return cpy;
}

//------------------------------------------------------------------

UTILLIB_FUNC char* ucopystr (const std::string& s) {

	util_ui32 len = (util_ui32)  s.length() + 1;
	char* cpy = DNEWARR(char, len);

	memcpy(cpy, s.c_str(), len);
	return cpy;
}

//------------------------------------------------------------------

UTILLIB_FUNC const std::string ucopymaxwithsuffix (const std::string& s, util_ui32 n) {
	if (n > s.length())
		return s;
	else {
		std::string s2;
		s2.append(s, 0, n);

		return s2 + "...";
	}
}

//------------------------------------------------------------------

static const char* hex = "0123456789ABCDEF";

UTILLIB_FUNC const std::string uint2hexstring (util_ui32 n) {

	std::string s;

	while (n) {
		s.insert(s.begin(), hex[n & 15]);	// n % 16
		n >>= 4;							// n /= 16
	}

	return s.empty() ? "0" : s;

}

//------------------------------------------------------------------

UTILLIB_FUNC const std::string unum2string (double val) {

	static char buffer[256];
	sprintf(buffer, "%.15f", val);
	DASSERT(strlen(buffer) < sizeof(buffer));
	std::string full(buffer);
	std::string result;

	std::string::iterator i = full.begin();
	while (*i == '0')	// Ignore all preceeding zeros.
		++i;

	while (i != full.end() && *i != '.') // Add all digits before '.'
		result.insert(result.end(), *i++);

	if (result.empty())	// No non-zero decimal part means we have to add one 0.
		result.append("0");

	if (i != full.end()) {

		DASSERT(*i == '.');
		std::string fract, deferred;

		while (++i != full.end()) {	// We  iterate and add any fractional part ending with non-zero.
			if (*i != '0') {
				fract.append(deferred);
				deferred.clear();
				fract.insert(fract.end(), *i);
			}
			else
				deferred.append("0");
		}

		if (!fract.empty()) // We do have a fractional part.
			result += "." + fract;
	}

	return result;
}

//------------------------------------------------------------------

UTILLIB_FUNC char* uconcatstr (const char* s1, const char* s2) {

	if (!s1 && !s2)
		return (char*) 0;
	else
	if (!s1)
		return ucopystr(s2);
	else
	if (!s2)
		return ucopystr(s1);
	else {
		char* s = DNEWARR(char,strlen(s1) + strlen(s2) + 1);
		DASSERT(s);

		strcpy(s, s1);
		strcat(s, s2);

		return s;
	}
}

//------------------------------------------------------------------

UTILLIB_FUNC char* uloadstr (FILE* fp) {

	DASSERT(fp);

	util_ui32 len;
	if (fread(&len, sizeof(len), 1, fp) != 1)
		return (char*) 0;

	DASSERT(len);
	char* s = DNEWARR(char, len);
	DASSERT(s);
	util_ui32 n = (util_ui32) fread(s, sizeof(char), len, fp);
	if (n != len) {
		DDELARR(s);
		return (char*) 0;
	}
	else
		return s;
}

UTILLIB_FUNC bool uloadstrex (FILE* fp, std::string& at) {

	util_ui32 len;
	if (fread(&len, sizeof(len), 1, fp) != 1)
		return false;
	DASSERT(len);	// Can't be 0 as it stores null terminator.

	char c;
	while (--len)
		if (fread(&c, sizeof(char), 1, fp) != 1)
			return false;
		else
			at.push_back(c);

	len = (util_ui32) fread(&c, sizeof(char), 1, fp); // Null terminator at the end always.
	DASSERT(len == 1 && !c);

	return true;
}

/////////////////////////////////////////////////////////////////
// |UINT(LEN)|CHAR|...|CHAR|, including
// last '\0' character.
//
UTILLIB_FUNC void ustorestr (FILE* fp, const std::string& s) {

	DASSERT(fp);

	util_ui32 len = (util_ui32) s.size() + 1;
	ufwrite(len, fp);
	fwrite(s.c_str(), sizeof(char), len, fp);
}

UTILLIB_FUNC util_ui32 ustorestrsize (const std::string& s)
	{ return (util_ui32) sizeof(util_ui32) + s.length() + 1; }

//------------------------------------------------------------------

UTILLIB_FUNC char* udeleoln (char* s) {

	DASSERT(s);

	if (!*s)
		return s;
	else {

		char* lastPos = s + (strlen(s) - 1);

		if (*lastPos == '\n')
			*lastPos = '\0';
		return s;
	}
}

//------------------------------------------------------------------

UTILLIB_FUNC char* ustrreverse (char* src) {

	if (!src || !*src)
		return src;

	static char text[256];
	util_ui16 len = (util_ui16) strlen(src);
	DASSERT(len  < 256);

	char* s1 = text;
	char* s2 = src + --len;

	while (len--)
		*s1++ = *s2--;
	*s1 = '\0';

	return text;
}

//------------------------------------------------------------------
// Returns true if 'prefix' is a prefix of s.
//
UTILLIB_FUNC bool ustrprefix (const char* prefix, const char* s) {
	
	while (*s && *prefix && *s == *prefix)
		++s, ++prefix;
	
	return !*prefix;
}

UTILLIB_FUNC bool ustrprefix (const std::string& prefix, const std::string& s) 
	{ return ustrprefix(prefix.c_str(), s.c_str()); }

//------------------------------------------------------------------

UTILLIB_FUNC bool ustrsuffix (const char* suffix, const char* s, bool pureSuffix) {

	util_ui32	suffixLen	= strlen(suffix);
	util_ui32	sLen		= strlen(s);

	if (sLen < suffixLen || (pureSuffix && sLen == suffixLen))
		return false;
	
	const char* sPart = s + (sLen - suffixLen); 

	while (suffixLen--) {
		DASSERT(*sPart && *suffix);
		if (*sPart++ != *suffix++)
			return false;
	}

	return true;
}

UTILLIB_FUNC bool ustrsuffix (const std::string& suffix, const std::string& s, bool pureSuffix)
	{ return ustrsuffix(suffix.c_str(), s.c_str(), pureSuffix); }

//------------------------------------------------------------------

UTILLIB_FUNC char* ustrupper (char* s) {

	if (s)
		for (char* p = s; *p; ++p)
			*p = toupper(*p);

	return s;
}

static std::string& ustrupper_priv (std::string& s) {
	for (	std::string::iterator i = s.begin();
			i != s.end();
			++i	)
			*i = toupper(*i);
	return s;
}

UTILLIB_FUNC std::string& ustrupper (std::string& s) 
	{ return ustrupper_priv(s); }

UTILLIB_FUNC const std::string	ustrupper (const std::string& s) {
	std::string u(s);
	ustrupper_priv(u);
	return u;
}

//------------------------------------------------------------------

UTILLIB_FUNC char* ustrlower (char* s) {
	if (s)
		for (char* p = s; *p; ++p)
			*p = tolower(*p);

	return s;
}

static std::string& ustrlower_priv (std::string& s) {
	for (	std::string::iterator i = s.begin();
			i != s.end();
			++i	)
			*i = tolower(*i);
	return s;
}

UTILLIB_FUNC std::string& ustrlower (std::string& s) 
	{ return ustrlower_priv(s); }

UTILLIB_FUNC const std::string ustrlower (const std::string& s) {
		std::string u(s);
	ustrlower_priv(u);
	return u;
}

//------------------------------------------------------------------
// Returns index in 'arr' where 's' is found, else, 'n'.
//
UTILLIB_FUNC util_ui16 ustrpos (const char** arr, util_ui16 n, const char* s) {

	for (util_ui16 i = n; i; --i, ++arr)
		if (!strcmp(*arr, s))
			return n - i;

	return n;
}

//------------------------------------------------------------------

UTILLIB_FUNC char uskipspaces (FILE* fp, util_ui32* lines) {

	char c;

	do {
		c = fgetc(fp);
		if (lines && c == '\n')
			++*lines;
	} while (isspace(c) && c != EOF);

	return c;
}

//------------------------------------------------------------------
// Makes 's' a string with 'n' tab stops.
//
UTILLIB_FUNC void utabstops (char* s, util_ui16 n) {

	DASSERT(s);

	while (n--)
		*s++ = '\t';

	*s = '\0';
}

UTILLIB_FUNC const std::string utabstops (util_ui16 total) {
	std::string s = "";
	char tab[2] = { '\t', '\0' };
	while (total--)
		s.append(tab);
	return s;
}

//------------------------------------------------------------------

UTILLIB_FUNC void uvsprintf (std::string& s, const char* format, va_list args) {

	static char buffer[256]; 
	while (*format) {
		if (*format == '%') {
			switch (*++format) {
				case 'd' 	: sprintf(buffer, "%d", va_arg(args, int));	s += buffer;		break;
				case 'u' 	: sprintf(buffer, "%u", va_arg(args, unsigned)); s += buffer;	break;
				case 'x'	: sprintf(buffer, "%x", va_arg(args, unsigned)); s += buffer;	break;
				case 'f' 	: s += unum2string(va_arg(args, double));						break;
				case 'c' 	: s += (char) va_arg(args, int);								break;
				case 's' 	: s += va_arg(args, char*);										break;
				case '%' 	: s += "%";														break;
				default		: s += std::string("%") + *format;
			}
			++format;
		}
		else
			s += *format++;
	}
}

//------------------------------------------------------------------

UTILLIB_FUNC const std::string uconstructstr (const char* format,...) {

	va_list args;
	va_start(args, format);

	std::string result;
	uvsprintf(result, format, args);
	va_end(args);

	return result;
}

//------------------------------------------------------------------

UTILLIB_FUNC const std::string ucropstr (const std::string& s, util_ui32 n, const std::string& suffix) {
	if (s.length() > n)
		return s.substr(0, n - 1) + suffix;
	else
		return s;
}

//------------------------------------------------------------------

const std::string ustrconcat (const ustrlist& l, const std::string& separator) {
	std::string s;
	for (ustrlist::const_iterator i = l.begin(); i != l.end(); ++i)
		s += (i != l.begin() ? separator : "") + *i;
	return s;
}

//------------------------------------------------------------------

UTILLIB_FUNC bool ureadstring (
		SequentialTextFileReader&	reader, 
		std::string&				result,
		uconditionalfunctor&		condition
	) {

	result = "";
	char add[2] = {'\0', '\0' };

	reader.skipspaces();
	if (reader.iseof())
		return false;

	while (!reader.iseof() && condition())
		if (isspace(add[0] = reader.getnext()))
			break;
		else
			result.append(add);
	return true;
}

/////////////////////////////////////////////////////////////////

UTILLIB_FUNC char* ureadquotedstring (
		SequentialTextFileReader&	reader, 
		char*						buffer, 
		util_ui16					n
	) {
	std::string s;
	if (!ureadquotedstring(reader, s) || s.length() >= n)
		return (char*) 0;
	else
		return strcpy(buffer, s.c_str());
}

//*****************************

UTILLIB_FUNC bool ureadquotedstring (
		SequentialTextFileReader&	reader, 
		std::string&				result
	) {

	result = "";
	char add[2] = {'\0', '\0' };

	reader.skipspaces();
	if (reader.iseof())
		return false;

	char c = reader.getnext();
	if (c != '"')			// Should beging with "
		return false;

	char last = '-';		// Any initial value other but '\\' does the job.
	bool done = false;

	do {
		if (reader.iseof())				// EOF while reading.
			return false;

		c = reader.getnext();
		if (c == '"')					// A quote is met.
			if (last == '\\')	{		// Escape sequence.
				add[0] = last = '"';
				result.erase(result.length() - 1, 1);	// Drop the last '\' tha was added.
				result.append(add);		// It is added to the string.
			}
			else
				done = true;			// Matching ending quote.
		else {
			add[0] = last = c;			
			result.append(add);			// Character normally added.
		}

	} while (!done);
				
	return true;
}

/////////////////////////////////////////////////////////////////

char SequentialTextFILEReader::getnext (void) { 
	if (useLookAhead) {
		useLookAhead = false;
		return lookAheadChar;
	}
	else
		return fgetc(fp);
}

void SequentialTextFILEReader::skipspaces (util_ui32* lines) {
	useLookAhead = true;
	lookAheadChar = uskipspaces(fp, lines);
}

bool SequentialTextFILEReader::iseof (void) const {
	if (useLookAhead)
		return lookAheadChar == EOF;	// If EOF character was read.
	else
		return !!feof(fp);
}

SequentialTextFILEReader::SequentialTextFILEReader (FILE* _fp) : 
	fp				(_fp),
	useLookAhead	(false),
	lookAheadChar	('\0') 
	{}

SequentialTextFILEReader::~SequentialTextFILEReader()
	{}

/////////////////////////////////////////////////////////////////

char SequentialStringReader::getnext (void) 
	{ return i == s.end() ? EOF : *i++; }

void SequentialStringReader::skipspaces (util_ui32* lines) {
	while (i != s.end() && isspace(*i)) {
		if (lines && *i == '\n')
			++*lines;
		++i;
	}
}

bool SequentialStringReader::iseof (void) const 
	{ return i == s.end(); }

SequentialStringReader::SequentialStringReader (const std::string& _s) :  s(_s), i(s.begin()){}
SequentialStringReader::~SequentialStringReader(){}

//****************************

class StringSizeLimitation : public uconditionalfunctor {
	private:
	mutable util_ui16 maxChars;

	public:
	bool operator() (void) const 
		{ return maxChars ? (--maxChars, true) : false; }

	StringSizeLimitation (util_ui16 _maxChars) : maxChars(_maxChars) {}
	~StringSizeLimitation() {}
};

//****************************

UTILLIB_FUNC char* ureadstring (FILE* fp, char* buffer, util_ui16 n) {
	
	std::string	result;
	if (!ureadstring(utempobj(SequentialTextFILEReader(fp)), result, utempobj(StringSizeLimitation(n))))
		return (char*) 0;
	else
		return strcpy(buffer, result.c_str());
}

//------------------------------------------------------------------

UTILLIB_FUNC char* ureadquotedstring (FILE* fp, char* buffer, util_ui16 n) {
	std::string result;
	if (!ureadquotedstring(utempobj(SequentialTextFILEReader(fp)), result))
		return (char*) 0;
	else
		return ustrnmaxcpy(buffer, result.c_str(), n);
}

UTILLIB_FUNC bool ureadquotedstring (FILE* fp, std::string& s) 
	{ return ureadquotedstring(utempobj(SequentialTextFILEReader(fp)), s); }

UTILLIB_FUNC bool ureadstring (FILE* fp, std::string& s) 
	{ return ureadstring(utempobj(SequentialTextFILEReader(fp)), s); }

//------------------------------------------------------------------

UTILLIB_FUNC bool ureadbool (FILE* fp, bool* val) {

	std::string s;
	if (!ureadstring(fp, s))
		return false;

	ustrupper(s);
	
	if (s == ubool2conststr(true))
		*((bool*) val) = true;
	else
	if (s == ubool2conststr(false))
		*((bool*) val) = false;
	else
		return false;

	return true;
}

//------------------------------------------------------------------

UTILLIB_FUNC const std::list<std::string> uargstostringlist (const char* args) {
	
	std::list<std::string> l;
	bool  quoted	= false;
	const char* s	= (char*) 0;

	while (*args)
		if (isspace(*args)) {
			if (!quoted && s) {		// token ending
				l.push_back(std::string(s, args));
				unullify(s);
			}						// if quotedd, space becomes part of token
			++args;
		}
		else
		if (*args == '"') {
			if ((quoted = !quoted))	// opening quote, token starting
				s = args++;
			else {					// closing quote, token ending
				l.push_back(std::string(s, ++args));
				unullify(s);
			}
		}
		else {
			if (!s)					// token starting
				s = args;
			++args;
		}

	if (s)							// token ending by eos
		l.push_back(s);

	return l;
}

//****************************

UTILLIB_FUNC char** ustringlisttoargv (const std::list<std::string>& l, int* argc) {
	if (argc)
		*argc = (int) l.size();
	char**		argv	= DNEWARR(char*, l.size());
	util_ui32	n		= 0;

	for (std::list<std::string>::const_iterator i = l.begin(); i != l.end(); ++i, ++n)
		argv[n] = ucopystr(*i);
	return argv;
}

//****************************

UTILLIB_FUNC void uclearargv (char**& argv, int argc) {
	for (char** p = argv; argc; --argc, ++p)
		DDELARR(*p);
	DDELARR(argv);
	unullify(argv);
}

//------------------------------------------------------------------

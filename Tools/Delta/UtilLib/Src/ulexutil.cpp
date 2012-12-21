// ulexutil.cpp
// Some common functions used for lexical analysis,
// that are shared between DeltaCompiler and ResourceLoader.
// ScriptFigher Project.
// A. Savidis, January 2000.
// Moved in Util package: February 2007.
//

#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "DDebug.h"
#include "utypes.h"
#include "ustrings.h"
#include "ulexutil.h"

//------------------------------------------------------------------

static char escapeChars[]={
	'n',
	'\\',
	't',
	'"',
	'\0'
};

static const char* translationChars[]={
	"\n",
	"\\",
	"\t",
	"\""
};

UTILLIB_FUNC const std::string uextendescapesequences (const char* src) {

	std::string	result;
	while (*src) {	
		if (*src == '\\') {
			const char* pos = strchr(escapeChars, *(src+1));
			if (pos) {
				result.append(translationChars[pos - escapeChars]);
				src += 2;
				continue;
			}
		}
		result += *src++;
	}

	return result;
}

/////////////////////////////////////////////////////////////////

static char parsedChars[]={
	'\n',
	'\\',
	'\t',
	'\"',
	'\0'
};

static const char* unparsedChars[]={
	"\\n",
	"\\\\",
	"\\t",
	"\\\""
};

UTILLIB_FUNC const std::string uunparseescapesequences (const char* src, const char* from, UPTR(const char*) to) {
	std::string	result;
	while (*src) {	
		const char* pos = strchr(from, *src);
		if (pos)
			result.append(to[pos - from]);
		else
			result += *src;
		++src;
	}

	return result;
}

UTILLIB_FUNC const std::string uunparseescapesequences (const char* src) 
	{ return uunparseescapesequences(src, parsedChars, unparsedChars); }

//------------------------------------------------------------------

UTILLIB_FUNC util_ui32 ustrtohex (const char* s) {
	static	util_ui32 hex[]	= { 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
			util_ui32 val	= 0;
	
	while (*s)
		val = 16 * val + (isdigit(*s) ? *s++ - '0' : hex[toupper(*s++) - 'A']);

	return val;
}

//------------------------------------------------------------------

UTILLIB_FUNC double ustrdectodouble (const char* s) {
	double val = 0;

	while (*s)
		val = val * 10 + (*s++ - '0');
	return val;
}


//------------------------------------------------------------------

UTILLIB_FUNC double ustrfracttodouble (const char* s) {

	double	val;
	bool	startFractional;
	double	fractional;

	for (val = fractional = 0.0, startFractional = false; *s; ++s)
		if (*s == '.') {
			startFractional	= true;
			fractional		= 0.1;
		}
		else
		if (startFractional) {
			val = val + ((double) (*s - '0')) * fractional;
			fractional /= 10.0;
		}
		else
			val = val * 10 + (*s - '0');

	return val;
}

//------------------------------------------------------------------

UTILLIB_FUNC const char* usaveidstr (const char* s, std::string* buffer) {
	//TODO make the buffer a required argument
	if (buffer) {
		DASSERT(buffer);
		buffer->assign(s);
		return buffer->c_str();
	}
	else {
		static char id[256];
		DASSERT(strlen(s) < sizeof(id));
		strcpy(id, s);
		return id;
	}
}

//------------------------------------------------------------------

static char input_wrapper (void *func) {
	DASSERT(func);
	return (*(char (*)(void)) func)();
}

static void unput_wrapper (char c, void *func) {
	DASSERT(func);
	(*(void (*)(char)) func)(c);
}

//------------------------------------------------------------------

UTILLIB_FUNC bool uignoreCcomments (
		char			(*input)(void), 
		void			(*unput)(char),
		util_ui32*		line,
		std::string*	error
	){
	return uignoreCcomments(
		umakecallback(&input_wrapper, input),
		umakecallback(&unput_wrapper, unput),
		line,
		error
	);
}

UTILLIB_FUNC bool uignoreCcomments (
		ucallbackwithclosure<char (*)(void*)>		input,
		ucallbackwithclosure<void (*)(char, void*)> unput,
		util_ui32*									line,
		std::string*								error
	){

	util_ui16 stack = 1;
	char nextc, ic;

	while ((ic = input()) != EOF)  {
		switch (ic)  {
     		case '/' : {
				if ((nextc = input()) == '*')
 					++stack;
				else
					unput(nextc);
				break;
			}

     		case '*' : {
				 if ((nextc = input())=='/')
 					if (--stack)
   						continue;
 					else
    					return true;
				 else
 					unput(nextc);
				 break;
			}

			case '\n': ++*line; break;
			default   : ;
        }
	}

	DASSERT(ic == EOF);
	if (error)
		*error = "EOF reached while parsing comments";
	return false;
}

//------------------------------------------------------------------

UTILLIB_FUNC bool ureadquotedstring (
		std::string&	s,
		char			(*input)(void), 
		util_ui32*		line,
		std::string*	error
	) {
	return ureadquotedstring(s, umakecallback(&input_wrapper, input), line, error);
}
UTILLIB_FUNC bool ureadquotedstring (
		std::string&							s,
		ucallbackwithclosure<char (*)(void*)>	input,
		util_ui32*								line,
		std::string*							error
	){

	while (true) {
		char c = input();
		if (c == EOF) {
			if (error)
				*error = "EOF reached while parsing quoted string";
			return false;
		}
		else
		if (c == '"') {
			if (s.empty())
				return true;
			else {
				bool evenNumOfBackSlashes = true;
				for (	std::string::reverse_iterator i = s.rbegin();
						i != s.rend() && *i == '\\';
						++i, evenNumOfBackSlashes = !evenNumOfBackSlashes	)
						;
						
				if (evenNumOfBackSlashes)	// Quote is normal end.
					return true;
				else
					s.push_back(c);			// It is an \"
			}
		}
		else {
			if (c == '\n')
				++*line;
			s.push_back(c);
		}
	}
}

//------------------------------------------------------------------

UTILLIB_FUNC bool uignoreCPPcomments (char (*input)(void))
	{ return uignoreCPPcomments(umakecallback(&input_wrapper, input)); }

UTILLIB_FUNC bool uignoreCPPcomments (ucallbackwithclosure<char (*)(void*)>	input) {
	while (true) {
		char c = input();
		if (c == '\n') 
			return true;
		else
		if (c == EOF)
			return false;
	}
}

//------------------------------------------------------------------

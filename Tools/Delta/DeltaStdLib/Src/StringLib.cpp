// StringLib.cpp.
// String library functions for the DeltaVirtualMachine.
// ScriptFighter Project.
// A. Savidis, December 1999.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#include "DeltaLibraryCreators.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "StringLib.h"
#include "ListLib.h"
#include "DeltaStdLibFuncNames.h"
#include "ustrings.h"

//------------------------------------------------------------------

static void strlen_LibFunc (DeltaVirtualMachine*);
static void strslice_LibFunc (DeltaVirtualMachine*);
static void strsub_LibFunc (DeltaVirtualMachine*);
static void	strinsert_LibFunc (DeltaVirtualMachine*);
static void strlower_LibFunc (DeltaVirtualMachine*);
static void strupper_LibFunc (DeltaVirtualMachine*);
static void strbyte_LibFunc (DeltaVirtualMachine*);
static void strchar_LibFunc (DeltaVirtualMachine*);
static void strbytestr_LibFunc (DeltaVirtualMachine*);
static void strisalpha_LibFunc (DeltaVirtualMachine*);
static void strisalnum_LibFunc (DeltaVirtualMachine*);
static void strisdigit_LibFunc (DeltaVirtualMachine*);
static void strisspace_LibFunc (DeltaVirtualMachine*);
static void strisprint_LibFunc (DeltaVirtualMachine*);
static void strislower_LibFunc (DeltaVirtualMachine*);
static void strisupper_LibFunc (DeltaVirtualMachine*);
static void strisident_LibFunc (DeltaVirtualMachine*);
static void strisprefix_LibFunc (DeltaVirtualMachine*);
static void strrep_LibFunc (DeltaVirtualMachine*);
static void strtonum_LibFunc (DeltaVirtualMachine*);
static void strtokenise_LibFunc (DeltaVirtualMachine*);
static void strsavetofile_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "len",		strlen_LibFunc			},
	{ "slice",		strslice_LibFunc		},
	{ "sub",		strsub_LibFunc			},
	{ "insert",		strinsert_LibFunc		},
	{ "lower",		strlower_LibFunc		},
	{ "upper",		strupper_LibFunc		},
	{ "byte",		strbyte_LibFunc			},
	{ "bytestr",	strbytestr_LibFunc		},
	{ "char",		strchar_LibFunc			},
	{ "isalpha", 	strisalpha_LibFunc 		},
	{ "isalnum", 	strisalnum_LibFunc 		},
	{ "isdigit", 	strisdigit_LibFunc 		},
	{ "isspace", 	strisspace_LibFunc 		},
	{ "isprint", 	strisprint_LibFunc 		},
	{ "islower", 	strislower_LibFunc 		},
	{ "isupper", 	strisupper_LibFunc 		},
	{ "isident",	strisident_LibFunc		},
	{ "isprefix",	strisprefix_LibFunc		},
	{ "rep",		strrep_LibFunc			},
	{ "tonum",		strtonum_LibFunc		},
	{ "tokenise",	strtokenise_LibFunc		},
	{ "savetofile", strsavetofile_LibFunc	}
};

DELTA_LIBRARY_SUBAPI_INSTALLER(DeltaString, "string", DELTA_STDLIB_NAMESPACE_PREFIX "str")

//------------------------------------------------------------------
// strlen(s).
// Get string and return length as a word number.
//
void strlen_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("strlen");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);
	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);

	vm->GetReturnValue().FromNumber((DeltaNumberValueType) str.length());
}

//------------------------------------------------------------------
// strslice(s,l1,r1,l2,r2,...,ln,rn).
// Makes s[l1]+...+s[r1]+...+s[ln]+...+s[rn].
// Conditions: li<=ri, 0<= (li, ri) <= strlen(s)-1.
// Special: if ri is 0, then ri becomes strlen(s)-1.
//
void strslice_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("strslice");
	DeltaAtLeastTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);
	
	util_ui32 len = (util_ui32) str.length();
	std::string slice;
	for (util_ui32	i = 1, n = DPTR(vm)->TotalActualArgs(); i < n; i += 2) {

		GET_POSITIVENUMBER(left, CURR_FUNC, RESET_NIL_RETURNVALUE, i);
		GET_POSITIVENUMBER(right, CURR_FUNC, RESET_NIL_RETURNVALUE, i + 1);

		util_ui32 l	= NUM_WORD(left), r	= NUM_WORD(right);
		if (!r)
			r = len - 1;

		if (l <= r && l < len && r < len)
			slice.append(str.substr(l, (r - l) + 1));
		else {
			DPTR(vm)->PrimaryError(
				"Illegal slice [%d, %d] in 'strslice('%s')",
				l, r,
				str.c_str()
			);
			break;
		}
	}

	if (!DPTR(vm)->HasProducedError())
		DPTR(vm)->GetReturnValue().FromString(slice.c_str());
	else
		DPTR(vm)->GetReturnValue().FromNil();
}

//------------------------------------------------------------------
// String strinsert(s1, i, s2).
// Returns string s1 where s2 is inserted after character at
// position 'i'. If 'i' is equal or larger than len, it is added
// ad the end.
//
static void	strinsert_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("strinsert");
	DeltaTotalArgsCheck(3, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_STRING(s1, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);
	GET_POSITIVENUMBER(i, CURR_FUNC, RESET_NIL_RETURNVALUE, 1);
	GET_STRING(s2, CURR_FUNC, RESET_NIL_RETURNVALUE, 2);
	
	util_ui32 pos = (util_ui32) i;

	if (pos >= s1.length())
		DPTR(vm)->GetReturnValue().FromString(s1 + s2);
	else
		DPTR(vm)->GetReturnValue().FromString(
			s1.substr(0, pos) + s2 + s1.substr(pos + 1, s1.length() - pos)
		);
}

//------------------------------------------------------------------
// strsub(s1,s2).
// Returns index of first occurence of s2 in s1.
// If no matching, returns -1. Result is integer.
//
void strsub_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("strsub");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_ZERO_RETURNVALUE);

	DeltaValue* argObj;
	GET_STRING(s1, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);
	GET_STRING(s2, CURR_FUNC, RESET_ZERO_RETURNVALUE, 1);

	char* s1Val = ucopystr(s1.c_str());
	char* s2Val = ucopystr(s2.c_str());
	char* pos	= strstr(s1Val, s2Val);

	DPTR(vm)->GetReturnValue().FromNumber((DeltaNumberValueType) (pos ? (util_i32) (pos - s1Val) : -1));

	DDELARR(s1Val);
	DDELARR(s2Val);
}

//------------------------------------------------------------------
// String strlower(s).
// Returns a string with all letters to lower case.
//
void strlower_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("strlower");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);

	vm->GetReturnValue().FromString(ustrlower(str));
}

//------------------------------------------------------------------
// String strupper(s).
// Returns a string with all letters to uppercase.
//
void strupper_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("strupper");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);

	vm->GetReturnValue().FromString(ustrupper(str));
}

//------------------------------------------------------------------

#define	GET_STRING_AND_INDEX(reset)					\
	DeltaTotalArgsCheck(2, CURR_FUNC, reset);		\
	DeltaValue* argObj;							\
	GET_STRING(str, CURR_FUNC, reset, 0);			\
	GET_UNSIGNEDINT(index, CURR_FUNC, reset, 1);	\
	if (index >= str.length()) {					\
		DPTR(vm)->PrimaryError(						\
			DEFORMAT_ILLEGAL_STRING_INDEX,			\
			index,									\
			str.length(),							\
			CURR_FUNC								\
		);											\
		reset										\
	}

#define	IS_STRING_OR_NUMER(t)	((t) == DeltaValue_Number || (t) == DeltaValue_String)

#define	GET_ASCII_CODE(reset)																	\
	DeltaTotalArgsCheck(1, CURR_FUNC, reset);													\
	DeltaValue* argObj;																			\
	DeltaGetOverloadedArg(argObj, 0, IS_STRING_OR_NUMER, "String or Number", CURR_FUNC, reset)	\
	util_ui8 index;																				\
	if (argObj->Type() == DeltaValue_String) {													\
		std::string str = argObj->ToString();													\
		if (str.length() != 1) {																\
			DPTR(vm)->PrimaryError(																\
				DEFORMAT_NOT_SINGLE_CHARACTER_STRING,											\
				str.c_str(),																	\
				CURR_FUNC																		\
			);																					\
			reset																				\
		}																						\
		index = (util_ui8) str[0];																\
	}																							\
	else {																						\
		DASSERT(argObj->Type() == DeltaValue_Number);											\
		DeltaNumberValueType num = argObj->ToNumber();											\
		if (num < 0) {																			\
			DPTR(vm)->PrimaryError(																\
				"unsigned expected (negative '%d' passed) in '%s'",								\
				(util_i32) num,																	\
				CURR_FUNC																		\
			);																					\
			reset																				\
		}																						\
		else																					\
			index = (util_ui8) num;																\
	}																							\
	if (!isascii(index)) {																		\
		DPTR(vm)->PrimaryError(																	\
			DEFORMAT_ILLEGAL_ASCII_CODE,														\
			index,																				\
			CURR_FUNC																			\
		);																						\
		reset																					\
	}

////////////////////////////////////////////////////////////////////
// strbyte(s,i).
// Returns the character numeric value at position
// i of string s.
//
void strbyte_LibFunc (DeltaVirtualMachine* vm) { 
	ISSUE_FUNC("strbyte");
	GET_STRING_AND_INDEX(RESET_ZERO_RETURNVALUE);
	vm->GetReturnValue().FromNumber((DeltaNumberValueType) str[index]);
}

////////////////////////////////////////////////////////////////////
// strchar(s,i).
// Returns as string the character value at position
// i of string s.
//
void strchar_LibFunc (DeltaVirtualMachine* vm) { 
	ISSUE_FUNC("strchar");
	GET_STRING_AND_INDEX(RESET_NIL_RETURNVALUE);
	char c[2] = { (char) str[index], '\0' };
	DPTR(vm)->GetReturnValue().FromString(c);
}

////////////////////////////////////////////////////////////////////

#define	STR_IS_TYPE(name, macro)							\
void name##_LibFunc (DeltaVirtualMachine* vm) {				\
	ISSUE_FUNC(#name);										\
	GET_ASCII_CODE(RESET_FALSE_RETURNVALUE);				\
	DPTR(vm)->GetReturnValue().FromBool(macro(index));		\
}

STR_IS_TYPE(strisalpha, isalpha)
STR_IS_TYPE(strisalnum, isalnum)
STR_IS_TYPE(strisdigit, isdigit)
STR_IS_TYPE(strisspace, isspace)
STR_IS_TYPE(strisprint, isprint)
STR_IS_TYPE(strislower, islower)
STR_IS_TYPE(strisupper, isupper)

//****************************
// bool strisident(String(s))

static void strisident_LibFunc (DeltaVirtualMachine* vm) { 
	ISSUE_FUNC("strisident");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);													\
	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_FALSE_RETURNVALUE, 0);
	DPTR(vm)->GetReturnValue().FromBool(DeltaString::IsIdentifier(str));
}

//****************************
// bool strisprefix(String(s), String(prefix))

static void strisprefix_LibFunc (DeltaVirtualMachine* vm) {
	ISSUE_FUNC("strisprefix");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_FALSE_RETURNVALUE);													\
	DeltaValue* argObj;
	GET_STRING(s, CURR_FUNC, RESET_FALSE_RETURNVALUE, 0);
	GET_STRING(prefix, CURR_FUNC, RESET_FALSE_RETURNVALUE, 1);
	DPTR(vm)->GetReturnValue().FromBool(ustrprefix(prefix, s));
}

//------------------------------------------------------------------
// strbytestr(i).
// Returns the string form of a character.
//
void strbytestr_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("strbytestr");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_UNSIGNEDINT(c, CURR_FUNC, RESET_ZERO_RETURNVALUE, 0);
	vm->GetReturnValue().FromString(std::string() += ((char) c));
}

//------------------------------------------------------------------
// strrep(s,n).
// Produces a string s+...+s where s is added n-1
// times to itself.
//
void strrep_LibFunc (DeltaVirtualMachine* vm) { 

	ISSUE_FUNC("strrep");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_STRING(str, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);
	GET_UNSIGNEDINT(n, CURR_FUNC, RESET_NIL_RETURNVALUE, 1);

	std::string repStr = "";
	for (util_ui32 i = 0; i < n; ++i)
		repStr.append(str);
	vm->GetReturnValue().FromString(repStr);
}

//------------------------------------------------------------------
// Number strtonum(s).
// Converts string to number.

DLIB_FUNC_START(strtonum, 1, Zero)
DLIB_ARG(const char*, s)
DLIB_RETVAL_REF.FromNumber(atof(DLIB_ARGVAL(s)));
DLIB_FUNC_END

//------------------------------------------------------------------
// list(String) strtokenise(s, tokens).
// Tokenizes a string 's' to a sequence of strings
// using character tokens from 'tokens'

DLIB_FUNC_START(strtokenise, 2, Nil)
DLIB_ARG(const char*, s)
DLIB_ARG(const char*, tokens)
std::list<std::string> l = utokenizestr(DLIB_ARGVAL(s), DLIB_ARGVAL(tokens));
DeltaList_Make(DLIB_RETVAL_REF);
std::list<DeltaValue>* result = DeltaList_Get(DLIB_RETVAL_REF);
for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i)
	result->push_back(*i);
DLIB_FUNC_END

//------------------------------------------------------------------
// strsavetofile(String(text), String(file))
//
DLIB_FUNC_START(strsavetofile, 2, False)
DLIB_ARG(const char*,	text)
DLIB_ARG(const char*,	file)
FILE* fp = fopen(DLIB_ARGVAL(file), "wt");
DLIB_ERROR_CHECK(!fp, uconstructstr("failed to open '%s' for writing", DLIB_ARGVAL(file)))
fprintf(fp, "%s", DLIB_ARGVAL(text));
fclose(fp);
DLIB_RETVAL_REF.FromBool(true);
DLIB_FUNC_END

//------------------------------------------------------------------


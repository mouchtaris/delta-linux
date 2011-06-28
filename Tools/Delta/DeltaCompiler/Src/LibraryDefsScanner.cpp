// LibraryDefsScanner.cpp
// Scanner for library deifnitions.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "LibraryDefsScanner.h"
#include <ctype.h>
#include "ustrings.h"

///////////////////////////////////////////////////////////

void DeltaLibraryDefsScanner::SkipBlanks (void) {
	while (!IsEof())
		if (!isspace(Input())) 
			{ Unput(); return; }
}

///////////////////////////////////////////////////////////

bool DeltaLibraryDefsScanner::MatchKwd (const std::string& kwd) {
	util_ui32 len = (util_ui32) kwd.length();
	if (len > Remaining()) 
		return false;
	else
	if (input.substr(pos, len) == kwd)
		{ pos += len; return true; }
	else
		return false;
}

///////////////////////////////////////////////////////////

DeltaLibraryDefsScanner::Token DeltaLibraryDefsScanner::GetToken (void) {
	
	if (HasError())
		return Error;

	SkipBlanks();
	if (IsEof())
		return TokenEOF;

	switch (char c = Input()) {

		case	','	:	return	TokenCOMMA;
		case	'('	:	return	TokenLP;
		case	')'	:	return	TokenRP;
		case	'+' :	return	TokenPLUS;
		case	'-' :	return	TokenMINUS;
		case	'{' :	return	TokenLBRACE;
		case	'}' :	return	TokenRBRACE;

		case	':'	:
			if (!MatchKwd(":")) 
				return TokenCOLON;
			else
				return TokenSCOPE;

		case	'.'	: 			
			if (!MatchKwd("..")) {
				error = uconstructstr("'...' expected at [%d]", pos);
				Unput();
				return Error;
			}
			else
				return TokenDOTS;

		case	'@'	: 
			if (!MatchKwd("warn")) {
				error = uconstructstr("'error' string expected after '@' at [%d]", pos);
				Unput();
				return Error;
			}
			else
				return TokenWARN;

		default:
			if (isalpha(c)) {
				id.clear();
				id += c;
				while (!IsEof()) {
					c = Input();
					if (isalnum(c) || c == '_')
						id += c;
					else
						{ Unput(); break; }
				}
				return TokenID;
			}
			else 
			if (c == '"')
				return ScanQuotedString();
			else
			if (isdigit(c) || c == '-')	// Sigh
				return ScanNumber(c);
			else {
				error = uconstructstr("unexpected character '%c' at [%u]", c, pos);
				Unput();
				return Error;
			}
	}
}

///////////////////////////////////////////////////////////

DeltaLibraryDefsScanner::Token DeltaLibraryDefsScanner::ScanQuotedString (void) {
	char c;
	while (!IsEof())
		if ((c = Input()) == '"')
			return TokenQSTRING;
		else 
			quotedString += c;

	error = uconstructstr("end of input at [%u] while reading quoted string '%s'", pos, quotedString.c_str());
	return Error;
}

///////////////////////////////////////////////////////////

DeltaLibraryDefsScanner::Token DeltaLibraryDefsScanner::ScanNumber (char c) {

	DASSERT(isdigit(c) || c == '-');
	std::string dec, fract;
	util_i32	sign = (c == '-' ? -1 : +1);

	if (c != '-')
		dec += c;
	while (!IsEof()) {
		c = Input();
		if (isdigit(c))
			dec += c;
		else
			break;
	}

	if (dec.empty()) {
		error = uconstructstr("expected number after sign '-' at [%u]", pos);
		return Error;
	}

	number = sign * atoi(dec.c_str());

	if (IsEof())
		return TokenNUMBER;
	else
	if (c == '.') {
		
		while (!IsEof()) {
			c = Input();
			if (isdigit(c))
				fract += c;
			else
				{ Unput(); break; }
		}
		
		if (fract.empty()) {
			error = uconstructstr("expected fraction part after '%s.' number at [%u] ", dec.c_str(), pos);
			return Error;
		}

		number += ((double) atoi(fract.c_str())) / ((double) fract.length());
		return TokenNUMBER;
	}
	else
		{ Unput(); return TokenNUMBER; }
}

///////////////////////////////////////////////////////////

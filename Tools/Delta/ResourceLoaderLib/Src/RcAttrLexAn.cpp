// RcAttrLexAn.cpp
// Lexical analyzer for attribute extraction
// from loaded resources.
// ScriptFigher Project.
// A. Savidis, January 2007.
//
#include "RcAttrLexAn.h"
#include "ustrings.h"
#include <ctype.h>

#define	START	0
#define	EOS		8
#define	ERROR	9

//////////////////////////////////////////////////////////

bool RcAttrLexAn::Scan (void) {

	DASSERT(input && token != Error && !EndOfInput());
	char c = *curr++;

	switch (state) {

		case START: {
			if (!c)
				{ curr = (char*) 0; state = EOS; return true; }
			if (c == '[')	{ state = 1; return true; }
			if (c == ']')	{ state = 2; return true; }
			if (c == '.')	{ state = 3; return true; }
			if (isdigit(c)) 
				{ num = c - '0'; state = 4; return false; }
			if (isalpha(c))
				{ ident.append(uconstructstr("%c", c)); state = 6; return false; }
			if (isspace(c))
				return false;	// Consume.
			state = ERROR; return true;
		}

		case 4: {
			if (isdigit(c))
				{  num = num * 10 + c - '0'; return false; }
			state = 5;	// Number recognized.
			--curr;		// Retract.
			return true;
		}

		case 6: {
			if (isalnum(c) || c == '_') // alphanumeric or underscore
				{ ident.append(uconstructstr("%c", c)); return false; }
			state = 7;	// Ident recognized.
			--curr;		// Retract.
			return true;
		}
		
		default: DASSERT(false); return false;
	}
}

//////////////////////////////////////////////////////////

void RcAttrLexAn::Start (const char* inputString) {
	Clear();
	input = curr = ucopystr(inputString);
}

//////////////////////////////////////////////////////////

RcAttrLexAn::TokenType RcAttrLexAn::GetToken (void) {
	Reset();
	while (!Scan())
		;
	return (token = (TokenType) state);
}

//////////////////////////////////////////////////////////

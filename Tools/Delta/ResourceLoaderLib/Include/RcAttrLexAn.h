// RcAttrLexAn.h
// Lexical analuzer for attribute extraction
// from loaded resources.
// ScriptFigher Project.
// A. Savidis, January 2007.
//

#ifndef	RCATTRLEXAN_H
#define	RCATTRLEXAN_H

#include "DDebug.h"
#include "utypes.h"
#include <string>
#include "RcLoaderDefs.h"

//////////////////////////////////////////////////////////////////
// States:
// 0: start, wspace
// 1: [			-> 	accept
// 2: ]			-> 	accept
// 3: .			-> 	accept
// 4: digit 	-> 	continue
// 5: number	-> 	accept
// 6: letter	-> 	continue
// 7: id		->	accept
// 8: EOS
// 9: error
//

//////////////////////////////////////////////////////////////////

class RCLOAD_CLASS RcAttrLexAn {
	public:
	enum TokenType {	// Matching the states.
			LeftBracket		=	1,
			RightBracket	=	2,
			Dot				=	3,
			Number			=	5,
			Ident			=	7,
			EndOfString		=	8,
			Error			=	9
	};

	private:
	TokenType			token;
	util_ui32			num;
	std::string			ident;
	char*				input;
	char*				curr;
	util_ui8			state;

	bool				Scan (void);
	void				Reset (void) 
							{ state = 0; num = 0; ident = ""; }

	//////////////////////////////////////////////////////////////////

	public:
	void				Start (const char* inputString);
	TokenType			GetToken (void);	// Rescans.

	bool				EndOfInput (void) const {
							DASSERT(input); 
							DASSERT(!!curr || token == EndOfString);
							return !curr; 
						}

	const std::string	RemainingInput (void) const 
							{ return curr; }

	util_ui32			GetNumber (void) const
							{ DASSERT(token == Number); return num; }
	const std::string	GetIdent (void) const
							{ DASSERT(token == Ident); return ident; }

	void				Clear (void) { ureset_via_default_ctor(this); }

	RcAttrLexAn (void) :
		token(EndOfString),
		num(0),
		ident(""),
		input((char*) 0),
		curr((char*) 0),
		state(0){}
	~RcAttrLexAn(){ if (input) DDELARR(input); }
};

#endif	// Do not add stuff beyond this point.

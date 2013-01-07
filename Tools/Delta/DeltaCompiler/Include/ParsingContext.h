// ParsingContext.h
// Parsing context for pure parsers.
// ScriptFighter Project.
// Y.Lilis, July 2011.
//

#ifndef PARSINGCONTEXT_H 
#define PARSINGCONTEXT_H

#include "ucomponentdirectory.h"

////////////////////////////////////////////////////////////////////////

template <typename Tlexer>
class ucomponentdirectory_with_lexer : public ucomponentdirectory {
	protected:
	Tlexer&		lexer;

	public:
	Tlexer&		GetLexer (void) { return lexer; }

	ucomponentdirectory_with_lexer (Tlexer& _lexer) : lexer(_lexer) {}
	virtual ~ucomponentdirectory_with_lexer() {}
};

////////////////////////////////////////////////////////////////////////

class FlexLexer;
typedef ucomponentdirectory_with_lexer<FlexLexer> ParsingContext;	// FlexLexerWithComponentDirectory

////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

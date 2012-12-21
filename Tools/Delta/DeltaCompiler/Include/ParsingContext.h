// ParsingContext.h
// Parsing context for pure parsers.
// ScriptFighter Project.
// Y.Lilis, July 2011.
//

#ifndef PARSINGCONTEXT_H 
#define PARSINGCONTEXT_H

#include "ucomponentdirectory.h"

////////////////////////////////////////////////////////////////////////

class FlexLexer;

////////////////////////////////////////////////////////////////////////

class ParsingContext : public ucomponentdirectory {
	private:
	FlexLexer& lexer;

	public:
	FlexLexer& GetLexer (void) { return lexer; }

	ParsingContext (FlexLexer& lexer) : lexer(lexer) {}
	~ParsingContext (void) {}
};

////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

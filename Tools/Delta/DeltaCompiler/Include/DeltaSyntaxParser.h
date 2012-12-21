// DeltaSyntaxParser.h
// Parser class.
// ScriptFighter Project.
// Y.Lilis, July 2011.
//

#ifndef DELTASYNTAXPARSER_H
#define DELTASYNTAXPARSER_H

#include <iostream>
#include "ucallbacks.h"

////////////////////////////////////////////////////////////////////////

class ParsingContext;
class FlexLexer;

////////////////////////////////////////////////////////////////////////

class DeltaSyntaxParser {
	private:
	FlexLexer&		lexer;
	ParsingContext& context;

	typedef int (*ParseFunc)(ParsingContext&);
	ParseFunc parser;

	typedef ucallbackwithclosure<void (*)(bool, void*)> ParseStartCallback;
	ParseStartCallback onParseStarted;

	public:
	bool 				ParseFile (const std::string& file);
	bool 				ParseText (const std::string& text);
	bool 				ParseStream (std::istream& input);

	const std::string	GetTokenText (void) const;
	void				SetParseStartCallback (ParseStartCallback cb) { onParseStarted = cb; }

	DeltaSyntaxParser (FlexLexer& lexer, ParsingContext& context, ParseFunc parser) :
		lexer		(lexer),
		context		(context),
		parser		(parser) {}
	~DeltaSyntaxParser () {}
};

////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

// DeltaSyntaxParser.h
// Parser class.
// ScriptFighter Project.
// Y.Lilis, July 2011.
//

#ifndef DELTASYNTAXPARSER_H
#define DELTASYNTAXPARSER_H

#include <iostream>
#include "ucallbacks.h"
#include "ParsingContext.h"

////////////////////////////////////////////////////////////////////////

class DeltaSyntaxParser {

	public:
	typedef util_i32 (*ParseFunc)(ParsingContext&);
	typedef ucallbackwithclosure<void (*)(bool, void*)> ParseStartCallback;

	private:
	ParsingContext&		context;
	ParseFunc			parser;
	ParseStartCallback	onParseStarted;

	public:
	bool 				ParseFile (const std::string& file);
	bool 				ParseText (const std::string& text);
	bool 				ParseStream (std::istream& input);

	const std::string	GetTokenText (void) const;
	void				SetParseStartCallback (const ParseStartCallback& f) 
							{ onParseStarted = f; }

	DeltaSyntaxParser (ParsingContext& context, ParseFunc parser) :
		context	(context),
		parser	(parser) 
		{}
	~DeltaSyntaxParser() 
		{}
};

////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

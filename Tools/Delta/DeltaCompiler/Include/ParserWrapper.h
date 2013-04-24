// ParserWrapper.h
// Parser class.
// ScriptFighter Project.
// Y.Lilis, July 2011.
//

#ifndef PARSERWRAPPER_H
#define PARSERWRAPPER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "ucallbacks.h"
#include "ulexutil.h"

////////////////////////////////////////////////////////////////////////

template<typename Lexer>
class ParserWrapper {

	public:
	typedef util_i32 (*ParseFunc)(Lexer& lexer);
	typedef ucallbackwithclosure<void (*)(bool, void*)> ParseStartCallback;

	private:
	Lexer&				lexer;
	ParseFunc			parser;
	ParseStartCallback	onParseStarted;

	public:
	bool 				ParseFile (const std::string& file)
							{ return ParseStream(utempobj(std::ifstream(file.c_str(), std::ios::in))); }
	bool 				ParseText (const std::string& text)
							{ return ParseStream(utempobj(std::istringstream(text, std::ios::in))); }
	bool 				ParseStream (std::istream& input) {
							if (input.fail()) {
								if (onParseStarted)
									onParseStarted(false);
								return false;
							}
							else {
								if (onParseStarted)
									onParseStarted(true);
								lexer.switch_streams(&input, 0);
								lexer.set_position(0);
								return !parser(lexer);
							}
						}

	const std::string	GetTokenText (void) const { return lexer.YYText(); }
	void				SetParseStartCallback (const ParseStartCallback& f) 
							{ onParseStarted = f; }

	ParserWrapper (Lexer& _lexer, ParseFunc _parser) :
		lexer	(_lexer),
		parser	(_parser) 
		{}
	~ParserWrapper() 
		{}
};

////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

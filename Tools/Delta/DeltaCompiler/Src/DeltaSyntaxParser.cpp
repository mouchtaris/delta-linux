// DeltaSyntaxParser.cpp
// Parser class.
// ScriptFighter Project.
// Y.Lilis, July 2011.
//

#include "DeltaSyntaxParser.h"
#include "FlexLexer.h"
#include "ParsingContext.h"

#include <fstream>
#include <sstream>

////////////////////////////////////////////////////////////////////////

bool DeltaSyntaxParser::ParseFile (const std::string& file)
	{ return ParseStream(std::ifstream(file.c_str(), std::ios::in)); }

////////////////////////////////////////////////////////////////////////

bool DeltaSyntaxParser::ParseText (const std::string& text)
	{ return ParseStream(std::istringstream(text, std::ios::in)); }

////////////////////////////////////////////////////////////////////////

bool DeltaSyntaxParser::ParseStream (std::istream& input) {
	if (input.fail()) {
		if (onParseStarted)
			onParseStarted(false);
		return false;
	}
	else {
		if (onParseStarted)
			onParseStarted(true);
		FlexLexer& lexer (context.GetLexer());
		lexer.switch_streams(&input, 0);
		lexer.set_position(0);
		lexer.set_context(&context);
		return !parser(context);
	}
}

////////////////////////////////////////////////////////////////////////

const std::string DeltaSyntaxParser::GetTokenText (void) const
	{ return context.GetLexer().YYText(); }

////////////////////////////////////////////////////////////////////////

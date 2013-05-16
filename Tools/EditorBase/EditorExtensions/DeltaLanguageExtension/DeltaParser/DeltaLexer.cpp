/**
 *	DeltaLexer.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaLexer.h"
#include "FlexLexer.h"
#include "DeltaAST.h"
#include "DeltaASTFwdDecls.h"

#include "DeltaParserCommon.h"
#include "DeltaParserGen.hpp"

#include <fstream>
#include <sstream>

extern int yylex (YYSTYPE* yylval, YYLTYPE* yylloc, int* tokenType, yyFlexLexer& lexer);

namespace delta {

////////////////////////////////////////////////////////////////////////
// DeltaLexer class
//
DeltaLexer::DeltaLexer (LexFuncPtr funcPtr, void* closure, uint offset) :
	m_funcPtr(funcPtr), m_closure(closure), m_offset(offset)
{

}

//**********************************************************************

DeltaLexer::~DeltaLexer (void)
{
}

//**********************************************************************

void DeltaLexer::LexFile (const String& uri)
{
	std::ifstream file(OFSTREAM_WXSTRING_INPUT(uri), std::ios::binary | std::ios::in);
	this->lexStream(file);
}

//**********************************************************************

void DeltaLexer::LexText (const std::string& text)
{
	std::istringstream stream(text, std::ios::binary | std::ios::in);
	this->lexStream(stream);
}

//**********************************************************************

void DeltaLexer::LexText (const String& text)
	{ this->LexText(util::str2std(text)); }

//**********************************************************************

void DeltaLexer::LexStream (std::istream& input)
	{ this->lexStream(input); }

//**********************************************************************

void DeltaLexer::lexStream (std::istream& input)
{
	yyFlexLexer lexer(&input, 0);
	lexer.set_position(m_offset);
	assert(m_funcPtr);

	int token, tokenType;
	YYSTYPE yylval;
	YYLTYPE yylloc;
	while ((token = yylex(&yylval, &yylloc, &tokenType, lexer)) != 0) {
		m_funcPtr(
			Range(yylloc.first_column, yylloc.last_column),
			token,
			tokenType,
			m_closure
		);
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace delta

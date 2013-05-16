/**
 *	DeltaParser.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaParser.h"
#include "DeltaParserCommon.h"
#include "DeltaAST.h"
#include "DeltaASTPositionVisitor.h"
#include "ProgramDescription.h"

#include "FlexLexer.h"

#include <fstream>
#include <sstream>

extern int yyparse (delta::ParsingCtx& ctx);

namespace delta {

////////////////////////////////////////////////////////////////////////
// DeltaParser class
//
DeltaParser::DeltaParser (ProgramDescription& desc, uint offset) :
	m_desc(desc), m_offset(offset)
{

}

//**********************************************************************

DeltaParser::~DeltaParser (void)
{
}

//**********************************************************************

bool DeltaParser::ParseFile (const String& uri)
{
	yyFlexLexer lexer;
	ParsingCtx ctx(uri, m_desc, lexer);

	std::ifstream file(OFSTREAM_WXSTRING_INPUT(uri), std::ios::binary | std::ios::in);
	return this->parseStream(ctx, file);
}

//**********************************************************************

bool DeltaParser::ParseText (const std::string& text)
{
	yyFlexLexer lexer;
	ParsingCtx ctx(_T("string"), m_desc, lexer);

	std::istringstream stream(text, std::ios::binary | std::ios::in);
	return this->parseStream(ctx, stream);
}

//**********************************************************************

bool DeltaParser::ParseText (const String& text)
	{ return this->ParseText(util::str2std(text)); }

//**********************************************************************
// Added for i-sense, partial selective parsing (AS).

class DeltaISenseLexer : public yyFlexLexer {
	private:
	std::list<int> pseudoTokens;

	public:
	virtual int	yylex (YYSTYPE* yylval, YYLTYPE* yyloc) {
		if (pseudoTokens.empty())
			return yyFlexLexer::yylex(yylval, yyloc);
		else 
			{ int t = pseudoTokens.front(); pseudoTokens.pop_front(); return t; }
	}

	DeltaISenseLexer (const std::list<int>& tokens) : pseudoTokens(tokens){}
};

//**********************************************************************
// Changed for i-sense, partial selective parsing (AS).

bool DeltaParser::ParseStream (std::istream& input) {
	
	DeltaISenseLexer lexer(GetPseudoTokens());
	ClearPseudoTokens();

	ParsingCtx ctx(_("stream"), m_desc, lexer);
	return this->parseStream(ctx, input);
}

//**********************************************************************

bool DeltaParser::parseStream (ParsingCtx& ctx, std::istream& input)
{
	bool result = false;
	if (!input.bad()) {
		ctx.GetLexer().switch_streams(&input, 0);
		ctx.GetLexer().set_position(m_offset);
		if (!(result = !yyparse(ctx))) {
			ctx.GetProgramDesc().ClearNotParsed();
			const Range range(m_offset, ctx.GetLexer().position());
			ctx.GetProgramDesc().AddNotParsed(range);
		}
	}
	return result;
}

////////////////////////////////////////////////////////////////////////

} // namespace delta

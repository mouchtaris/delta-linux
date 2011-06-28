/**
 *	DeltaParserCommon.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DeltaParserCommon.h"
#include "FlexLexer.h"
#include "DeltaAST.h"

namespace delta {

////////////////////////////////////////////////////////////////////////
// ParsingCtx class
//
ParsingCtx::ParsingCtx (const String& uri, ProgramDescription& desc, yyFlexLexer& lexer) :
	m_uri(uri), m_desc(desc), m_lexer(lexer)
{
}

//**********************************************************************

ParsingCtx::~ParsingCtx (void)
{
}
 
////////////////////////////////////////////////////////////////////////

} // namespace delta

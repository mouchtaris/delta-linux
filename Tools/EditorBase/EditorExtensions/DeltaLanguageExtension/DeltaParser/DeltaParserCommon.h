/**
 *	DeltaParserCommon.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAPARSERCOMMON_H
#define DELTA_DELTAPARSERCOMMON_H

#include "Common.h"

////////////////////////////////////////////////////////////////////////

#undef TRUE
#undef FALSE

#ifdef WIN32
#	define YY_NO_UNISTD_H
#	define YYINCLUDED_STDLIB_H
#endif

#pragma warning(disable:4102)
#pragma warning(disable:4267)
#pragma warning(disable:4065)

class yyFlexLexer;

namespace delta {

////////////////////////////////////////////////////////////////////////

class DeltaASTNode;
class ProgramDescription;

////////////////////////////////////////////////////////////////////////

class ParsingCtx {
public:
	ParsingCtx (const String& uri, ProgramDescription& desc, yyFlexLexer& lexer);
	~ParsingCtx (void);

	const String&		GetURI (void) const { return m_uri; }
	ProgramDescription&	GetProgramDesc (void) { return m_desc; }
	yyFlexLexer&		GetLexer (void) { return m_lexer; }

private:
	const String&		m_uri;
	ProgramDescription&	m_desc;
	yyFlexLexer&		m_lexer;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAPARSERCOMMON_H

/**
 *	DeltaParser.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAPARSER_H
#define DELTA_DELTAPARSER_H

#include "Common.h"

#include <iosfwd>

namespace delta {

////////////////////////////////////////////////////////////////////////

class ParsingCtx;
class ProgramDescription;

////////////////////////////////////////////////////////////////////////

class DeltaParser {
public:
	DeltaParser (ProgramDescription& desc, uint offset = 0);
	~DeltaParser (void);

	void SetOffset (uint offset) { m_offset = offset; }

	bool ParseFile (const String& uri);
	bool ParseText (const std::string& text);
	bool ParseText (const String& text);
	bool ParseStream (std::istream& input);

	// Added for i-sense, partial selective parsing (AS).
	void					AddPseudoToken (int t) 
								{ pseudoTokens.push_back(t); }

private:
	inline bool parseStream (ParsingCtx& ctx, std::istream& input);
		
	// Added for i-sense, partial selective parsing (AS).
	std::list<int> pseudoTokens;

	const std::list<int>&	GetPseudoTokens (void) const
								{ return pseudoTokens; }
	void					ClearPseudoTokens (void)
								{ pseudoTokens.clear(); }

	ProgramDescription& m_desc;
	uint				m_offset;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAPARSER_H

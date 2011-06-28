/**
 *	DeltaLexer.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTALEXER_H
#define DELTA_DELTALEXER_H

#include "Common.h"
#include "DeltaASTDataTypes.h"

#include <iosfwd>

namespace delta {

////////////////////////////////////////////////////////////////////////

typedef void (*LexFuncPtr) (const Range& range, int token, int type, void* closure);

////////////////////////////////////////////////////////////////////////

class DeltaLexer {
public:
	DeltaLexer (LexFuncPtr funcPtr, void* closure = 0, uint offset = 0);
	~DeltaLexer (void);

	void SetOffset (uint offset) { m_offset = offset; }

	void LexFile (const String& path);
	void LexText (const std::string& text);
	void LexText (const String& text);
	void LexStream (std::istream& input);

private:
	inline void lexStream (std::istream& input);

	uint		m_offset;
	LexFuncPtr	m_funcPtr;
	void*		m_closure;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTALEXER_H

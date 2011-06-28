// DebugWatchValueRcEncoder.h
// Resource format encoder for debug watch values.
// ScriptFighter Project.
// Debug expession evaluator.
// A. Savidis, August 2008.
//

#ifndef	DEBUGWATCHVALUERCENCODER_H
#define	DEBUGWATCHVALUERCENCODER_H

#include "DebugWatchValueInfo.h"
#include "DebugWatchValueRcEncodingDefs.h"

//////////////////////////////////////////////////////////////////

class DebugWatchValueRcEncoder {
	public:
	static const std::string	GetFormatId (void) { return RC_ENCODING_ID; }
	static const std::string	Encode (const DebugWatchValueInfo& value);
};

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

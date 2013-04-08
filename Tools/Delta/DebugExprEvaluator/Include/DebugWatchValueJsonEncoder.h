// DebugWatchValueJsonEncoder.h
// Json format encoder for debug watch values.
// Debug expession evaluator.
// Giannhs Apostolidhs, April 2013.
//

#ifndef	DEBUGWATCHVALUEJSONENCODER_H
#define	DEBUGWATCHVALUEJSONENCODER_H

#include "DebugWatchValueInfo.h"
#include "DebugWatchValueJsonEncodingDefs.h"

//////////////////////////////////////////////////////////////////

class DebugWatchValueJsonEncoder {
	public:
	static const std::string	GetFormatId (void) { return JSON_ENCODING_ID; }
	static const std::string	Encode (const DebugWatchValueInfo& value);
};

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

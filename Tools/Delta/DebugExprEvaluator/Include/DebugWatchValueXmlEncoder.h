// DebugWatchValueXmlEncoder.h
// Xml format encoder for debug watch values.
// ScriptFighter Project.
// Debug expession evaluator.
// A. Savidis, August 2008.
//

#ifndef	DEBUGWATCHVALUEXMLENCODER_H
#define	DEBUGWATCHVALUEXMLENCODER_H

#include "DebugWatchValueInfo.h"
#include "DebugWatchValueXmlEncodingDefs.h"

//////////////////////////////////////////////////////////////////

class DebugWatchValueXmlEncoder {
	public:
	static const std::string	GetFormatId (void) { return XML_ENCODING_ID; }
	static const std::string	Encode (const DebugWatchValueInfo& value);
};

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

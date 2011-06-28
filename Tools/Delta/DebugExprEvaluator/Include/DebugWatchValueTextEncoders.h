// DebugWatchValueTextEncoders.h
// Text encoding for debug watcg values.
// ScriptFighter Project.
// Debug expession evaluator.
// A. Savidis, August 2008.
//

#ifndef	DEBUGWATCHVALUETEXTENCODERS_H
#define	DEBUGWATCHVALUETEXTENCODERS_H

#include "DebugWatchValueInfo.h"
#include "usingleton.h"
#include "utypes.h"
#include "DDebug.h"

#include <string>
#include <map>

//////////////////////////////////////////////////////////////////

class DebugWatchValueTextEncoders {
	public:
	typedef const std::string (*Encoder) (const DebugWatchValueInfo& value);

	private:
	typedef std::map<std::string, Encoder> EncoderMap; 
	static EncoderMap* encoders;

	public:
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
	static void					Install (const std::string& formatId, Encoder f);
	static Encoder				Get (const std::string& formatId);
};

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

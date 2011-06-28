// DebugWatchValueTextEncoders.cpp
// Text necoding for debug watcg values.
// ScriptFighter Project.
// Debug expession evaluator.
// A. Savidis, August 2008.
//

#include "DebugWatchValueTextEncoders.h"

DebugWatchValueTextEncoders::EncoderMap* 
	DebugWatchValueTextEncoders::encoders = (EncoderMap*) 0;

/////////////////////////////////////////////////////////////////

void DebugWatchValueTextEncoders::SingletonCreate (void) 
	{ unew(encoders); }

void DebugWatchValueTextEncoders::SingletonDestroy (void)
	{ udelete(encoders); }

/////////////////////////////////////////////////////////////////

void DebugWatchValueTextEncoders::Install (const std::string& formatId, Encoder f) {
	DASSERT(encoders && f && !Get(formatId));
	(*DPTR(encoders))[formatId] = f;
}

DebugWatchValueTextEncoders::Encoder DebugWatchValueTextEncoders::Get (const std::string& formatId) {
	DASSERT(encoders);
	EncoderMap::iterator i = DPTR(encoders)->find(formatId);
	return i != DPTR(encoders)->end() ? i->second : (Encoder) 0;
}

/////////////////////////////////////////////////////////////////

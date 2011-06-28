/**
 *	SoundManager.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef SOUND_SOUNDMANAGER_H
#define SOUND_SOUNDMANAGER_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <map>

namespace sound {

////////////////////////////////////////////////////////////////////////

class _BASE_API Sound {
public:
	Sound (void) {}
	virtual ~Sound (void) {}

	virtual bool Play (bool block = false) = 0;
};

////////////////////////////////////////////////////////////////////////

class _BASE_API SoundManager {
public:
	static void Initialize (void);
	static void CleanUp (void);	

	static Sound& GetSound (const String& sound);

	static void Stop (void);

private:
	DISABLE_CONSTRUCTION(SoundManager);

	typedef std::map<const String, Sound*> SoundMap;

	static SoundMap s_soundMap;
};

////////////////////////////////////////////////////////////////////////

} // namespace sound

#endif // SOUND_SOUNDMANAGER_H

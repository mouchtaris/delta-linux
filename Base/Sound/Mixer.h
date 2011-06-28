/**
 *	Mixer.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef SOUND_MIXER_H
#define SOUND_MIXER_H

#include "Common.h"
#include "LibraryAPIDecls.h"

namespace sound {

////////////////////////////////////////////////////////////////////////

class _BASE_API Mixer {
public:
	enum ChannelType { CHANNEL_MASTER };

	static void Initialize (void);
	static void CleanUp (void);

	static bool SetVolume (int32 volume, ChannelType type = CHANNEL_MASTER);
	static bool GetVolume (int32* volume, ChannelType type = CHANNEL_MASTER);

	static void IncreaseVolume (ChannelType type = CHANNEL_MASTER);
	static void DecreaseVolume (ChannelType type = CHANNEL_MASTER);

private:
	static void changeVolume (int32 offset, ChannelType type);

	DISABLE_CONSTRUCTION(Mixer);
};

////////////////////////////////////////////////////////////////////////

} // namespace sound

#endif // SOUND_MIXER_H

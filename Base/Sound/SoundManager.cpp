/**
 *	SoundManager.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "SoundManager.h"
#include "Adaptors.h"

#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/sound.h>

namespace sound {

////////////////////////////////////////////////////////////////////////
// Null Sound
//
class NullSound : public Sound {
public:
	NullSound (void) {}
	virtual ~NullSound (void) {}

	bool Play (bool PORT_UNUSED_PARAM(block)) { return false; }
};

////////////////////////////////////////////////////////////////////////
// wxSound
//
class WXSound : public Sound {
public:
	WXSound (const String& filename) : m_sound(filename, false) { assert(m_sound.IsOk()); }
	virtual ~WXSound (void) {}

	bool Play (bool block) { return m_sound.Play(block ? wxSOUND_SYNC : wxSOUND_ASYNC); }

private:
	wxSound m_sound;
};

////////////////////////////////////////////////////////////////////////

static wxFileName*	s_soundsDir = 0;
static NullSound	s_nullSound;

////////////////////////////////////////////////////////////////////////
// Sound Manager
//
void SoundManager::Initialize (void)
{
	s_soundsDir = new wxFileName(_T("."));
	s_soundsDir->AppendDir(_T("sounds"));

	if (!s_soundsDir->DirExists()) {

		/*
		wxMessageBox(_(
			"Cannot find the sounds directory\n"
			"No sound will be played during this session"),
			_("Error initializing sound system"), wxOK | wxICON_ERROR
		);
		*/
	}
}

//**********************************************************************

void SoundManager::CleanUp (void)
{
	std::for_each(
		s_soundMap.begin(), s_soundMap.end(),
		util::project2nd<SoundMap::value_type>(util::deleter<Sound>())
	);
	s_soundMap.clear();

	delete s_soundsDir;
	s_soundsDir = 0;
}

//**********************************************************************

Sound& SoundManager::GetSound (const String& sound)
{
	SoundMap::iterator iter = s_soundMap.find(sound);
	if (iter != s_soundMap.end())
		return *iter->second;
	else {
		const String soundFile(sound + _T(".wav"));
		wxFileName fileName(*s_soundsDir);
		fileName.SetName(soundFile);
		if (fileName.IsOk() && fileName.FileExists()) {
			Sound* wxsound = new WXSound(fileName.GetFullPath());
			s_soundMap.insert(SoundMap::value_type(sound, wxsound));
			return *wxsound;
		}
		else
			return s_nullSound;
	}
}

//**********************************************************************

void SoundManager::Stop (void) { wxSound::Stop(); }

//**********************************************************************

SoundManager::SoundMap SoundManager::s_soundMap;

////////////////////////////////////////////////////////////////////////

} // namespace sound

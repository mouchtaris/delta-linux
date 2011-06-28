/**
 *	Mixer.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "Mixer.h"
#include "Streams.h"

#include <windows.h>
#include <mmsystem.h>

namespace sound {

////////////////////////////////////////////////////////////////////////

static UINT			s_nNumMixers;
static HMIXER		s_hMixer = 0;
static MIXERCAPS	s_mxcaps;
static DWORD		s_dwMinimum;
static DWORD		s_dwMaximum;
static DWORD		s_dwVolumeControlID;

static String		s_strDstLineName;
static String		s_strVolumeControlName;

////////////////////////////////////////////////////////////////////////

void Mixer::Initialize (void)
{
	////////////////////////////////////////////////////////////////////
	// Initialize
	//
	s_nNumMixers = ::mixerGetNumDevs();

	s_hMixer = NULL;
	::ZeroMemory(&s_mxcaps, sizeof(MIXERCAPS));

	s_dwMinimum = 0;
	s_dwMaximum = 0;
	s_dwVolumeControlID = 0;

	if (s_nNumMixers != 0) {

		if (
			::mixerOpen(&s_hMixer, 0, 0, NULL, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR ||
			::mixerGetDevCaps((UINT_PTR) s_hMixer, &s_mxcaps, sizeof(MIXERCAPS)) != MMSYSERR_NOERROR
		) {
			s_hMixer = 0;
			DBGOUT << "Mixer initialization FAILED!!!" << DBGENDL;
			return;
		}
	}

	////////////////////////////////////////////////////////////////////
	// Get master volume control
	//

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	if (
		::mixerGetLineInfo(
				reinterpret_cast<HMIXEROBJ>(s_hMixer),
				&mxl,
				MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE
		) == MMSYSERR_NOERROR
	) {
		// get dwControlID
		MIXERCONTROL mxc;
		MIXERLINECONTROLS mxlc;
		mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mxc;
		if (
			::mixerGetLineControls(
				reinterpret_cast<HMIXEROBJ>(s_hMixer),
				&mxlc,
				MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE
			) == MMSYSERR_NOERROR
		) {
			// store dwControlID
			s_strDstLineName = mxl.szName;
			s_strVolumeControlName = mxc.szName;
			s_dwMinimum = mxc.Bounds.dwMinimum;
			s_dwMaximum = mxc.Bounds.dwMaximum;
			s_dwVolumeControlID = mxc.dwControlID;
		}
	}
}

//**********************************************************************

void Mixer::CleanUp (void)
{
	if (s_hMixer != 0) {
		::mixerClose(s_hMixer);
		s_hMixer = 0;
	}
}

//**********************************************************************

bool Mixer::SetVolume (int32 volume, ChannelType PORT_UNUSED_PARAM(type))
{
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { volume };
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = s_dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;

	DBGOUT << "Trying to set volume to value: " <<  volume << DBGENDL;
	return ::mixerSetControlDetails(
					reinterpret_cast<HMIXEROBJ>(s_hMixer),
					&mxcd,
					MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE
		) == MMSYSERR_NOERROR;
}

//**********************************************************************

bool Mixer::GetVolume (int32* volume, ChannelType PORT_UNUSED_PARAM(type))
{
	assert(volume);

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = s_dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (
		::mixerGetControlDetails(
					reinterpret_cast<HMIXEROBJ>(s_hMixer),
					&mxcd,
					MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE
		) != MMSYSERR_NOERROR
	) {
		DBGOUT << "FAILED to get volume value!" << DBGENDL;
		return false;
	}
	*volume = int32(mxcdVolume.dwValue);
	DBGOUT << "Got volume value: " << *volume << DBGENDL;
	return true;
}

//**********************************************************************

void Mixer::IncreaseVolume (ChannelType type) { changeVolume(+3000, type); }
void Mixer::DecreaseVolume (ChannelType type) { changeVolume(-3000, type); }

//**********************************************************************

void Mixer::changeVolume (int32 offset, ChannelType type)
{
	int32 volume;
	GetVolume(&volume, type);
	SetVolume(util::clamp(volume + offset, 0, 68535), type);
}

////////////////////////////////////////////////////////////////////////

} // namespace sound

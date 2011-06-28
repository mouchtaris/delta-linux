/**
 *	EditorBaseLibrary.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "EditorBaseLibrary.h"
#include "EditorMain/EditorModuleLoader.h"

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

#include <wx/clipbrd.h>

#ifndef NO_VLD
#include <vld.h>
#endif

namespace editor {

////////////////////////////////////////////////////////////////////////
// EditorBaseLibrary class
//
void EditorBaseLibrary::Initialize (void)
{
	EditorModuleLoader::Initialize();
}

//**********************************************************************

void EditorBaseLibrary::CleanUp (void)
{
	EditorModuleLoader::CleanUp();
	wxTheClipboard->Clear();
}

////////////////////////////////////////////////////////////////////////

} // namespace editor

////////////////////////////////////////////////////////////////////////
// DLL entry point
//
BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			editor::EditorBaseLibrary::Initialize();
			return true;
		case DLL_PROCESS_DETACH:
			editor::EditorBaseLibrary::CleanUp();
			return true;
	}
	return true;
}

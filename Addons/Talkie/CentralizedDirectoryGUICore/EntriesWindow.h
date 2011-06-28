/**
 *	EntriesWindow.h
 *
 *	-- Talkie Centralized Directory stand-alone Server --
 *
 *	Window displaying the user entries of the directory
 *	in the talkie network. (don't tell anyone)
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef DIRECTORY_GUI_ENTRIES_WINDOW_H_
#define DIRECTORY_GUI_ENTRIES_WINDOW_H_

#include "Common.h"
#include "StringDefs.h"
#include "CommonFwdDecls.h"
#include "UndefineAllWin32Shit.h"

#include <wx/dialog.h>

#include <boost/signal.hpp>

class wxStaticBitmap;
class wxListView;
class wxImageList;
class wxMenu;

class wxListEvent;

namespace talkie
{
	//----------------------------
	//-- class EntriesWindow

	class EntriesWindow :
		public wxDialog
	{
	public:
		///--- constructors / destructor
		EntriesWindow(const String& title);
		~EntriesWindow(void);

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onRightClick(wxListEvent& event);
		void onMenuItemSelected(wxCommandEvent& event);

		void EditSelected(void);
		void RemoveSelected(void);

		void PopulateUserList(void);

		///--- private members
		wxStaticBitmap*	imgLogo;
		wxListView* lstEntries;
		wxImageList* images;
		wxMenu* contextMenu;
	};
} // namespace talkie

#endif	//DIRECTORY_GUI_ENTRIES_WINDOW_H_

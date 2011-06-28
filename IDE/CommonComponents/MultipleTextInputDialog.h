/**
 *	MultipleTextInputDialog.h
 *
 *	-- IDE Common Component --
 *
 *	MultipleTextInputDialog class allows the user to
 *	give multiple text input via a modal dialog.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2008
 */
#ifndef SHELL_MULTIPLE_TEXT_INPUT_DIALOG_H_
#define SHELL_MULTIPLE_TEXT_INPUT_DIALOG_H_

#include "Common.h"

#include "UndefineAllWin32Shit.h"

#include <wx/dialog.h>
#include <wx/textctrl.h>

class wxListView;
class wxTextCtrl;
class wxButton;

namespace ide
{
	//----------------------------
	//-- class MultipleTextInputDialog

	class MultipleTextInputDialog :
		public wxDialog
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		MultipleTextInputDialog(const String& title, const StringList& labels, const StringList& defaultValues = StringList(), wxWindow* parent=0);
		~MultipleTextInputDialog(void);

		///--- public API
		StringList GetInput(void) const;

	private:
		///--- private API
		///--- private members
		typedef std::list<wxTextCtrl *> TextCtrlList;
		TextCtrlList	txtInputList;
	};
}

#endif	//SHELL_MULTIPLE_TEXT_INPUT_DIALOG_H_

/**
 *	TextInputDialog.h
 *
 *	-- IDE Common Component --
 *
 *	TextInputDialog class allows the user to
 *	give text input via a modal dialog.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	August 2007
 */
#ifndef SHELL_TEXT_INPUT_DIALOG_H_
#define SHELL_TEXT_INPUT_DIALOG_H_

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
	//-- class TextInputDialog

	class TextInputDialog :
		public wxDialog
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		TextInputDialog(const String& title, const String& label, const String& defaultValue = String(), wxWindow* parent=0);
		~TextInputDialog(void);

		///--- public API
		String GetInput(void) const	{ return txtInput->GetValue(); }

	private:
		///--- private API
		///--- private members
		wxTextCtrl*		txtInput;
	};
}

#endif	//SHELL_TEXT_INPUT_DIALOG_H_

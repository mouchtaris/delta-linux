/**
 *	IDEDialogs.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_IDEDIALOGS_H
#define IDE_IDEDIALOGS_H

#include "Common.h"
#include "IDECommon.h"

#include "GenericDialogs.h"
#include "UndefineAllWin32Shit.h"

#include <wx/dialog.h>
#include <wx/filedlg.h>

namespace ide
{
	//----------------------------
	//-- class FileDialog

	class _IDE_API FileDialog :
		public wxFileDialog
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		FileDialog(wxWindow* parent, const String& title, const String& defaultDir = String(),
			const String& defaultFile = String(), const String& wildcard = _T("*.*"),
			long style = wxFD_DEFAULT_STYLE)
			:wxFileDialog(parent, title, defaultDir, defaultFile, wildcard, style) {}
		~FileDialog(void) {}

		///--- public API

	private:
		///--- private API

		///--- private members
	};

	//----------------------------
	//-- class FileDialog

	class _IDE_API HostPortDialog :	
		public gui::DialogBase
	{
	public:
		HostPortDialog(
			wxWindow*		parent,
			const String&	title,
			const String&	hostValue = String(),
			int				portValue = 0
		);

	const String	GetHost (void) const;
	int				GetPort (void) const;

	private:
		wxTextCtrl* host;
		wxSpinCtrl* port;
	};

} // namespace ide

#define	IDE_ERROR_DELTA_BYTE_CODE_FILE_NOT_FOUND(_type,_bin,_class)											\
			gui::displayMessage(																			\
				0,																							\
				_("Error"),																					\
				String(_("Delta ")) + _(_type) + _(" '")			+										\
				util::std2str(_class)								+										\
				_("' will not be loaded since byte code file '")	+										\
				util::std2str(_bin)									+										\
				_("' does not exist.\nShould load 'Sparrow.wsp' and rebuild the ") + _(_type) + _("!")		\
			)

#define	IDE_WARNING_DELTA_BYTE_CODE_FILE_NOT_UP_TO_DATE(_type,_bin,_class)									\
			gui::displayMessage(																			\
				0,																							\
				_("Warning"),																				\
				String(_("Delta ")) + _(_type) + _(" '")			+										\
				util::std2str(_class)								+										\
				_("' will be loaded but its byte code file '")		+										\
				util::std2str(_bin)									+										\
				_("' is not up-to-date.\nShould load 'Sparrow.wsp' and rebuild the ") + _(_type) + _("!")	\
			)

#endif // IDE_IDEDIALOGS_H

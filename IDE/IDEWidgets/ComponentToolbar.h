/**
 *	ComponentToolBar.h
 *
 *	-- IDEWidgets --
 *
 *	Component toolbar is a wrapper of wxToolbar
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	May 2007
 */
#ifndef COMPONENT_TOOLBAR_H_
#define COMPONENT_TOOLBAR_H_

#include "Common.h"
#include "IDECommon.h"

#include "ComponentUserCommand.h"
#include "UndefineAllWin32Shit.h"

#include <wx/toolbar.h>
#include <wx/tbarbase.h>

#include <boost/signal.hpp>

#include <functional>

class wxBitmap;

namespace ide
{
	//----------------------------
	//-- class ComponentToolBar

	class _IDE_API ComponentToolBar :
		public wxToolBar
	{
	public:
		///--- type definitions
		typedef UserCommand::Callback		Callback;
		typedef UserCommand::CallbackVec	CallbackVec;
		typedef UserCommand::CallbackVecPtr CallbackVecPtr;

		typedef boost::signal<void (const Callback&)> ToolSelectedSignal;

		///--- constructors / destructor
		ComponentToolBar(void)
			: actions(CallbackVecPtr(new CallbackVec)), canSeparate(false) {}
		ComponentToolBar(wxWindow* parent, wxWindowID id=wxID_ANY)
			: wxToolBar(parent, id, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER),
			actions(CallbackVecPtr(new CallbackVec)), canSeparate(false) {}
		~ComponentToolBar(void) {}

		///--- public API
		wxToolBarToolBase* AddTool(const String& title, const wxBitmap& bitmap, const Callback& action,
			const String& longHelpStr);
		wxToolBarToolBase* AddCheckTool(const String& title, const wxBitmap& bitmap, const Callback& action,
			const String& longHelpStr);
		wxToolBarToolBase* AddSeparator(void);

		void ClearTools(void);

		const Callback& GetToolAction(uint id) const		{ return actions->at(id); }

		///--- Signals
		ToolSelectedSignal sigToolSelected;

	private:
		friend class ToolGenerator;

		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onToolSelected(wxCommandEvent& event);

		///--- private members
		CallbackVecPtr actions;
		bool canSeparate;
	};
}

#endif	//COMPONENT_TOOLBAR

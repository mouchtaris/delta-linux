/**
 *	DeltaQuickWatch.h
 *
 *	-- IDE Quick Watch --
 *
 *	Quick watch view of a Delta expression during debugging.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	August 2008
 */
#ifndef IDE_DELTA_QUICK_WATCH_COMPONENT_H_
#define IDE_DELTA_QUICK_WATCH_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "IDEComponent.h"

#include <wx/dialog.h>

class wxComboBox;

namespace ide
{
	class DeltaQuickWatch : public wxDialog, public IDEComponent
	{
		DECLARE_IDE_COMPONENT(DeltaQuickWatch);

	public:
		///--- type definitions

		///--- constructors / destructor
		DeltaQuickWatch(void);
		~DeltaQuickWatch(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_FUNCTION_(void, ShowDialog, (bool modal),
			_("Show this quick watch dialog"));
		DECLARE_EXPORTED_FUNCTION_(void, CloseDialog, (void),
			_("Show this quick watch dialog"));

		DECLARE_EXPORTED_FUNCTION_(void, SetExpression, (const String& expr),
			_("Set the actual treeview expression of the quick watch"));
		DECLARE_EXPORTED_FUNCTION_(void, SetDisplayedExpression, (const String& expr),
			_("Set the displayed expression in the combobox (no treeview change)"));
		DECLARE_EXPORTED_FUNCTION_(Handle, GetTreeViewWindow, (void),
			_("Get a handle to the treeview window of the quick watch"));
	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- type definitions
		enum ButtonCommands {
			BUTTON_REEVALUATE,
			BUTTON_ADD_WATCH,
			BUTTON_BACK,
			BUTTON_FORWARD,
			COMBO_BOX_EXPRESSIONS,
			TREEVIEW
		};

		///--- private API
		void UpdatePosition	(void);
		void UpdateSize		(void);

		void onReevaluate				(wxCommandEvent& event);
		void onAddWatch					(wxCommandEvent& event);
		void onBack						(wxCommandEvent& event);
		void onForward					(wxCommandEvent& event);
		void onExpressionChange			(wxCommandEvent& event);
		void onExpressionFocusRequested	(wxCommandEvent& event);
		void onTreeviewFocusRequested	(wxCommandEvent& event);
		void onClose					(wxCloseEvent& event);
		void onShow						(wxShowEvent& event);
		void onSize						(wxSizeEvent& event);
		void onMove						(wxMoveEvent& event);

		///--- private members
		static wxArrayString choices;
		static unsigned totalInstances;

		wxComboBox *expressions;
		Component *watch;
		String expression;
		bool shown;
	};
}

#endif	//IDE_DELTA_QUICK_WATCH_COMPONENT_H_

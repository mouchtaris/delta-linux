/**
 *	DeltaQuickWatch.cpp
 *
 *	-- IDE Quick Watch --
 *
 *	Quick watch view of a Delta expression during debugging.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	August 2008
 */
#include "DeltaQuickWatch.h"
#include "Call.h"
#include "ComponentFactory.h"
#include "ComponentFunctionCallerSafe.h"
#include "PropertyUtils.h"

#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>

namespace ide
{
	wxArrayString DeltaQuickWatch::choices;
	unsigned DeltaQuickWatch::totalInstances = 0;

	//-----------------------------------------------------------------------

	BEGIN_EVENT_TABLE(DeltaQuickWatch, wxDialog)
		EVT_BUTTON(BUTTON_REEVALUATE, DeltaQuickWatch::onReevaluate)
		EVT_BUTTON(BUTTON_ADD_WATCH, DeltaQuickWatch::onAddWatch)
		EVT_BUTTON(BUTTON_BACK, DeltaQuickWatch::onBack)
		EVT_BUTTON(BUTTON_FORWARD, DeltaQuickWatch::onForward)
		EVT_TEXT_ENTER(wxID_ANY, DeltaQuickWatch::onExpressionChange)
		EVT_COMBOBOX(wxID_ANY, DeltaQuickWatch::onExpressionChange)
		EVT_MENU(COMBO_BOX_EXPRESSIONS, DeltaQuickWatch::onExpressionFocusRequested)
		EVT_MENU(TREEVIEW, DeltaQuickWatch::onTreeviewFocusRequested)
		EVT_CLOSE(DeltaQuickWatch::onClose)
		EVT_SHOW(DeltaQuickWatch::onShow)
		EVT_SIZE(DeltaQuickWatch::onSize)
		EVT_MOVE(DeltaQuickWatch::onMove)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class DeltaQuickWatch ----------------------------//

	COMPONENT_METADATA(
		DeltaQuickWatch,
		_("Quick Watch"),
		_("Quick watch view of expressions during debugging"),
		_T("Yannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(DeltaQuickWatch, wxDialog, IDEComponent);

	//-----------------------------------------------------------------------

	EXPORTED_SIGNAL(DeltaQuickWatch, QuickWatchBack, (void));
	EXPORTED_SIGNAL(DeltaQuickWatch, QuickWatchForward, (void));
	EXPORTED_SIGNAL(DeltaQuickWatch, QuickWatchReevaluate, (const String& expression));
	EXPORTED_SIGNAL(DeltaQuickWatch, QuickWatchAddWatch, (const String& expression));
	EXPORTED_SIGNAL(DeltaQuickWatch, QuickWatchExpressionChanged, (const String& expression));
	EXPORTED_SIGNAL(DeltaQuickWatch, QuickWatchTreeviewWidthChanged, (uint width));
	EXPORTED_SIGNAL(DeltaQuickWatch, QuickWatchClosed, (void));

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(DeltaQuickWatch, table)
	{
		conf::AggregateProperty* position = new conf::AggregateProperty(_("Window position"), _("initial window position"));
		position->AddProperty("x", new conf::IntProperty(_("x"), wxDefaultPosition.x, _("x position of the window")));
		position->AddProperty("y", new conf::IntProperty(_("y"), wxDefaultPosition.y, _("y position of the window")));
		table.AddProperty("position", position);

		conf::AggregateProperty* size = new conf::AggregateProperty(_("Window size"), _("initial window size"));
		size->AddProperty("width", new conf::IntProperty(_("width"), wxDefaultSize.x, _("width of the window")));
		size->AddProperty("height", new conf::IntProperty(_("height"), wxDefaultSize.y, _("height of the window")));
		table.AddProperty("size", size);
	}

	//-----------------------------------------------------------------------

	DeltaQuickWatch::DeltaQuickWatch(void) : expressions(0), watch(0), shown(false)
	{
		++totalInstances;
	}

	//-----------------------------------------------------------------------

	DeltaQuickWatch::~DeltaQuickWatch(void)
	{
		if (watch) {
			RemoveChild(watch->GetWindow());
			watch->Destroy();
		}
		--totalInstances;
	}

	//-----------------------------------------------------------------------

	wxWindow* DeltaQuickWatch::GenerateWindow(wxWindow* parent)
	{
		const conf::AggregateProperty* position_prop =
			static_cast<const conf::AggregateProperty*>(GetProperty("position"));
		wxPoint position = wxPoint(
			conf::get_prop_value<conf::IntProperty>(position_prop->GetProperty("x")),
			conf::get_prop_value<conf::IntProperty>(position_prop->GetProperty("y"))
		);
		if (totalInstances > 1)	//multiple QuickWatches open in default position
			position = wxDefaultPosition;
		else {
			wxSize screen = ::wxGetDisplaySize();
			if (position.x >= screen.GetWidth() || position.y >= screen.GetHeight())
				position = wxDefaultPosition;
		}

		const conf::AggregateProperty* size_prop =
			static_cast<const conf::AggregateProperty*>(GetProperty("size"));
		wxSize size = wxSize(
			conf::get_prop_value<conf::IntProperty>(size_prop->GetProperty("width")),
			conf::get_prop_value<conf::IntProperty>(size_prop->GetProperty("height"))
		);

		Create(parent, wxID_ANY, _T("Quick Watch"), position, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

		//-- Expression ComboBox
		expressions = new wxComboBox(this, wxID_ANY, String(), wxDefaultPosition, wxSize(400, 20), choices, wxTE_PROCESS_ENTER);

		//-- Static Text
		wxStaticText* expressionText = new wxStaticText(this, wxID_ANY, _T("&Expression:"));
		wxStaticText* valueText = new wxStaticText(this, wxID_ANY, _T("&Value:"));

		//-- Buttons
		wxButton *reevaluateButton = new wxButton(this, BUTTON_REEVALUATE, _("&Reevaluate"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		wxButton *addWatchButton = new wxButton(this, BUTTON_ADD_WATCH, _("Add &Watch"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		wxButton *backButton = new wxButton(this, BUTTON_BACK, _("Back"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		wxButton *forwardButton = new wxButton(this, BUTTON_FORWARD, _("Forward"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		wxButton *closeButton = new wxButton(this, wxID_CANCEL, _("Close"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

		//-- Expression TreeListView
		watch = ComponentFactory::Instance().CreateComponent("ExpressionTreeListView");
		if (!watch)
			return (wxWindow *) 0;	//the QuickWatch won't be functional, but still avoids potential crashes
		wxWindow *watchWindow = watch->GenerateWindow(this);
	
		////////////////////////////////////////////////////////////////////
		// Layout stuff
		//
		wxSizerFlags flags = wxSizerFlags(1).Expand();

		wxBoxSizer* exprSizer = new wxBoxSizer(wxVERTICAL);
		exprSizer->AddSpacer(10);
		exprSizer->Add(expressionText, flags);
		exprSizer->Add(expressions, flags);

		wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
		buttonSizer->AddSpacer(10);
		buttonSizer->Add(reevaluateButton, wxSizerFlags().Align(wxALIGN_RIGHT));
		buttonSizer->AddSpacer(5);
		buttonSizer->Add(addWatchButton, wxSizerFlags().Align(wxALIGN_RIGHT));

		wxBoxSizer* horizontalSizer1 = new wxBoxSizer(wxHORIZONTAL);
		horizontalSizer1->AddSpacer(10);
		horizontalSizer1->Add(exprSizer, wxSizerFlags(1));
		horizontalSizer1->AddSpacer(5);
		horizontalSizer1->Add(buttonSizer);
		horizontalSizer1->AddSpacer(10);

		wxBoxSizer* valueSizer = new wxBoxSizer(wxVERTICAL);
		valueSizer->Add(valueText, wxSizerFlags().Expand());
		valueSizer->AddSpacer(5);
		valueSizer->Add(watchWindow, flags);

		wxBoxSizer* horizontalSizer2 = new wxBoxSizer(wxHORIZONTAL);
		horizontalSizer2->AddSpacer(10);
		horizontalSizer2->Add(valueSizer, flags);
		horizontalSizer2->AddSpacer(10);

		wxBoxSizer* navigationSizer = new wxBoxSizer(wxHORIZONTAL);
		navigationSizer->Add(backButton, wxSizerFlags().Align(wxALIGN_LEFT));
		navigationSizer->AddSpacer(5);
		navigationSizer->Add(forwardButton, wxSizerFlags().Align(wxALIGN_LEFT));

		wxBoxSizer* buttonSizer2 = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer2->Add(navigationSizer, wxSizerFlags(1));
		buttonSizer2->AddSpacer(5);
		buttonSizer2->Add(closeButton, wxSizerFlags().Align(wxALIGN_RIGHT));

		wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
		topSizer->Add(horizontalSizer1, wxSizerFlags().Expand());
		topSizer->Add(horizontalSizer2, flags);
		topSizer->Add(buttonSizer2, wxSizerFlags().Expand().Align(wxALIGN_BOTTOM | wxALIGN_RIGHT).Border(wxUP | wxDOWN | wxRIGHT | wxLEFT, 10));

		this->SetSizer(topSizer);
		//set the min width so that all 3 bottom button fit in, and the height just the same
		int minSize = backButton->GetSize().GetWidth() + forwardButton->GetSize().GetWidth() + 
			closeButton->GetSize().GetWidth() + 38;	//for the spacing and borders
		this->SetMinSize(wxSize(minSize, minSize));
		if (position == wxDefaultPosition)
			this->Centre(wxBOTH | wxCENTRE_ON_SCREEN);

		wxAcceleratorEntry entries[4];
		entries[0].Set(wxACCEL_ALT,	WXK_LEFT,	BUTTON_BACK);
		entries[1].Set(wxACCEL_ALT,	WXK_RIGHT,	BUTTON_FORWARD);
		entries[2].Set(wxACCEL_ALT,	(int) 'E',	COMBO_BOX_EXPRESSIONS);
		entries[3].Set(wxACCEL_ALT,	(int) 'V',	TREEVIEW);
		wxAcceleratorTable accel(4, entries);
		this->SetAcceleratorTable(accel);

		return (wxWindow *) 0;	//this is a popup that should not be added in the shell gui
	}

	//-----------------------------------------------------------------------

	wxWindow* DeltaQuickWatch::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaQuickWatch, void, ShowDialog, (bool modal))
	{
		shown = true;
		if (modal) {
			ShowModal();
			shown = false;
		}
		else
			Show();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaQuickWatch, void, CloseDialog, (void))
	{
		if (shown) {
			EndModal(GetEscapeId());
			shown = false;
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaQuickWatch, void, SetExpression, (const String& expr))
	{
		if (choices.Index(expr) != wxNOT_FOUND)
			choices.Remove(expr);
		if (!expr.empty())
			choices.Insert(expr, 0);
		expressions->Clear();
		expressions->Append(choices);
		expressions->Select(0);
	}

	//-----------------------------------------------------------------------
	
	EXPORTED_FUNCTION(DeltaQuickWatch, void, SetDisplayedExpression, (const String& expr))
	{
		expressions->SetValue(expr);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaQuickWatch, Handle, GetTreeViewWindow, (void))
	{
		return watch ? Handle(watch) : Handle();
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::UpdatePosition(void)
	{
		if (totalInstances == 1) {
			const wxPoint pos = this->GetPosition();
			conf::AggregateProperty* position = (conf::AggregateProperty*)GetProperty("position");
			static_cast<conf::IntProperty*>(position->GetProperty("x"))->SetValue(pos.x);
			static_cast<conf::IntProperty*>(position->GetProperty("y"))->SetValue(pos.y);
			GetClassPropertyTable().SetDirty(true);
		}
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::UpdateSize(void)
	{
		if (totalInstances == 1) {
			const wxSize size = this->GetSize();
			conf::AggregateProperty* size_prop = (conf::AggregateProperty*)GetProperty("size");
			static_cast<conf::IntProperty*>(size_prop->GetProperty("width"))->SetValue(size.GetWidth());
			static_cast<conf::IntProperty*>(size_prop->GetProperty("height"))->SetValue(size.GetHeight());
			GetClassPropertyTable().SetDirty(true);
		}
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onReevaluate(wxCommandEvent& event)
	{
		sigQuickWatchReevaluate(this, expressions->GetValue());
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onAddWatch(wxCommandEvent& event)
	{
		sigQuickWatchAddWatch(this, expressions->GetValue());
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onBack(wxCommandEvent& event)
	{
		sigQuickWatchBack(this);
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onForward(wxCommandEvent& event)
	{
		sigQuickWatchForward(this);
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onExpressionChange(wxCommandEvent& event)
	{
		sigQuickWatchExpressionChanged(this, event.GetString());
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onExpressionFocusRequested(wxCommandEvent& event)
	{
		expressions->SetFocus();
	}
	
	//-----------------------------------------------------------------------
	
	void DeltaQuickWatch::onTreeviewFocusRequested(wxCommandEvent& event)
	{
		if (watch)
			watch->GetWindow()->SetFocus();
	}
	
	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onClose(wxCloseEvent& event)
	{
		sigQuickWatchClosed(this);
		event.Skip();
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onShow(wxShowEvent& event)
	{
#if wxCHECK_VERSION(2,8,11)
		if (!event.IsShown())
#else
		if (!event.GetShow())
#endif
			sigQuickWatchClosed(this);
		else if (watch)
			sigQuickWatchTreeviewWidthChanged(
				this,
				conf::get_prop_value<conf::IntProperty>(
					static_cast<const conf::AggregateProperty*>(GetProperty("size"))->GetProperty("width")
				)
			);
		event.Skip();
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onSize(wxSizeEvent& event)
	{
		UpdatePosition();
		UpdateSize();
		if (watch)
			sigQuickWatchTreeviewWidthChanged(this, watch->GetWindow()->GetSize().GetWidth());
		event.Skip();
	}

	//-----------------------------------------------------------------------

	void DeltaQuickWatch::onMove(wxMoveEvent& event)
	{
		UpdatePosition();
		event.Skip();
	}

	//-----------------------------------------------------------------------
}

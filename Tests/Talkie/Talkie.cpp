#include <wx/wx.h>
#include <wx/aui/aui.h>

class MyFrame : public wxFrame {
public:
	MyFrame(wxWindow* parent) : wxFrame(parent, -1, _("wxAUI Test"),
		wxDefaultPosition, wxSize(800,600), wxDEFAULT_FRAME_STYLE)
	{
		// notify wxAUI which frame to use
		m_mgr.SetManagedWindow(this);

		// create several text controls
		wxTextCtrl* text1 = new wxTextCtrl(this, -1, _("Pane 1 - sample text"),
			wxDefaultPosition, wxSize(200,150), wxNO_BORDER | wxTE_MULTILINE);

		wxTextCtrl* text2 = new wxTextCtrl(this, -1, _("Pane 2 - sample text"),
			wxDefaultPosition, wxSize(200,150), wxNO_BORDER | wxTE_MULTILINE);

		wxTextCtrl* text3 = new wxTextCtrl(this, -1, _("Main content window"),
			wxDefaultPosition, wxSize(200,150), wxNO_BORDER | wxTE_MULTILINE);

		//
		wxAuiNotebook* notebook1 = new wxAuiNotebook(
			this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxAUI_NB_BOTTOM | wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_SPLIT
		);
		wxTextCtrl* ntText1 = new wxTextCtrl(notebook1, -1, _("First tab"),
			wxDefaultPosition, wxSize(200,200), wxNO_BORDER | wxTE_MULTILINE);
		wxTextCtrl* ntText2 = new wxTextCtrl(notebook1, -1, _("Second tab"),
			wxDefaultPosition, wxSize(200,200), wxNO_BORDER | wxTE_MULTILINE);
		wxTextCtrl* ntText3 = new wxTextCtrl(notebook1, -1, _("Third tab"),
			wxDefaultPosition, wxSize(200,200), wxNO_BORDER | wxTE_MULTILINE);
		notebook1->AddPage(ntText1, _("Tab 1"));
		notebook1->AddPage(ntText2, _("Tab 2"));
		notebook1->AddPage(ntText3, _("Tab 3"));

		wxAuiNotebook* notebook2 = new wxAuiNotebook(
			this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxAUI_NB_BOTTOM | wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_SPLIT
		);
		wxTextCtrl* ntText4 = new wxTextCtrl(notebook2, -1, _("First tab"),
			wxDefaultPosition, wxSize(200,200), wxNO_BORDER | wxTE_MULTILINE);
		notebook2->AddPage(ntText4, _("Tab 1 of 2"));
		//

		// add the panes to the manager
		m_mgr.AddPane(text1, wxLEFT, wxT("Pane Number One"));
		m_mgr.AddPane(text2, wxBOTTOM, wxT("Pane Number Two"));
		m_mgr.AddPane(text3, wxCENTER);

		//
		wxAuiPaneInfo info;
		info.DefaultPane()
			.Gripper(true)
			.BestSize(wxSize(200, 200))
			.CaptionVisible(/*false*/);
		m_mgr.AddPane(notebook1, info);
		m_mgr.AddPane(notebook2, info);
		//

		// tell the manager to "commit" all the changes just made
		m_mgr.Update();
	}

	~MyFrame()
	{
		// deinitialize the frame manager
		m_mgr.UnInit();
	}

private:
	wxAuiManager m_mgr;
};

// our normal wxApp-derived class, as usual
class MyApp : public wxApp {
public:

	bool OnInit (void) {
		wxFrame* frame = new MyFrame(NULL);
		SetTopWindow(frame);
		frame->Show();
		return true;
	}
 };
 
DECLARE_APP(MyApp);
IMPLEMENT_APP(MyApp);

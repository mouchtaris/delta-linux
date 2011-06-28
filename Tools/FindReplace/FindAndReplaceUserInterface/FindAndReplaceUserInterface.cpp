#include "FindAndReplaceUserInterface.h"

#include <assert.h>
#include "FindAndReplaceManager.h"
#include "Icons/expression_editor.xpm"

#define BORDER 5

typedef frep::FindAndReplaceManager manager;

#define UPDATE_UI()		comboxFindWhat->SetFocus();														\
						UserInterfaceUpdater::SetComboxFindWhatValue(comboxFindWhat->GetValue());		\
						UserInterfaceUpdater::SetComboxLookInValue(comboxLookIn->GetValue());			\
						UserInterfaceUpdater::SetCheckBoxSearchUpValue(searchUp->GetValue());			\
						UserInterfaceUpdater::SetCheckBoxMatchCaseValue(matchCase->GetValue());			\
						UserInterfaceUpdater::SetCheckBoxWholeWordValue(matchWholeWord->GetValue());	\
						UserInterfaceUpdater::SetCheckBoxRegExprValue(regex->GetValue());				\
						Show(false)



namespace frep {

	// Event table for FindAndReplaceUserInterface
	BEGIN_EVENT_TABLE(FindAndReplaceUserInterface, wxDialog)
		EVT_TEXT		(ID_COMBOBOX_LOOKIN,				FindAndReplaceUserInterface::OnLookIn)
		EVT_TEXT		(ID_COMBOBOX_FIND_WHAT,				FindAndReplaceUserInterface::OnTextCtrlFindWhat)
		EVT_MENU		(ID_REGEX_MENU_ANY_CHAR,			FindAndReplaceUserInterface::OnAnyChar)
		EVT_MENU		(ID_REGEX_MENU_ZERO_OR_MORE,		FindAndReplaceUserInterface::OnZeroOrMore)
		EVT_MENU		(ID_REGEX_MENU_ONE_OR_MORE,			FindAndReplaceUserInterface::OnOneOrMore)
		EVT_MENU		(ID_REGEX_MENU_OR,					FindAndReplaceUserInterface::OnOr)
		EVT_MENU		(ID_REGEX_MENU_TAG,					FindAndReplaceUserInterface::OnTag)
		EVT_MENU		(ID_REGEX_MENU_SPACE_OR_BAR,		FindAndReplaceUserInterface::OnSpaceOrBar)
		EVT_MENU		(ID_REGEX_MENU_DELTA_IDENTIFIER,	FindAndReplaceUserInterface::OnDelteIden)
		EVT_MENU		(ID_REGEX_MENU_QUOTED_STRING,		FindAndReplaceUserInterface::OnQuotedString)
		EVT_MENU		(ID_REGEX_MENU_ESC_SPECIAL_CHAR,	FindAndReplaceUserInterface::OnEsqSpecialCharacter)
		EVT_MENU		(ID_REGEX_MENU_INTEGER,				FindAndReplaceUserInterface::OnInteger)	
		EVT_MENU		(ID_FIND_NEXT,						FindAndReplaceUserInterface::OnFindNext)
		EVT_MENU		(ID_FIND_PREV,						FindAndReplaceUserInterface::OnFindPrev)
		EVT_CLOSE		(									FindAndReplaceUserInterface::OnExit)	
		EVT_MOTION		(									FindAndReplaceUserInterface::OnMouseMove)	
		EVT_BUTTON		(wxID_CANCEL,						FindAndReplaceUserInterface::OnCancel)
		EVT_BUTTON		(ID_REGEX_BUTTON_FIND,				FindAndReplaceUserInterface::OnButtonRegexFindMenu)
		EVT_CHECKBOX	(ID_CHECKBOX_SEARCH_UP,				FindAndReplaceUserInterface::OnCheckBoxSearchUp)
		EVT_CHECKBOX	(ID_CHECKBOX_MATCH_CASE,			FindAndReplaceUserInterface::OnCheckBoxMatchCase)
		EVT_CHECKBOX	(ID_CHECKBOX_MATCH_WHOLE_WORD,		FindAndReplaceUserInterface::OnCheckBoxMatchWholeWord)
	END_EVENT_TABLE()


	bool			FindAndReplaceUserInterface::newSearch		= true;
	bool			FindAndReplaceUserInterface::regExprIsActive	= false;
	wxPoint			FindAndReplaceUserInterface::mousePosotion	= wxPoint(0,0);
	LookIn			FindAndReplaceUserInterface::lookIn			= frep::CURRENT_DOCUMENT;;



	/////////////////////////////////////////////////
	//	Public method implementation

	//-----------------------------------------------------------------------
	
	FindAndReplaceUserInterface::FindAndReplaceUserInterface(	
		wxString title, 
		wxWindow* parent, 
		TextSourceCollectionProducer * pro,
		ApplicationCommunicator * com
	) :	producer(pro),
		communicator(com),
		comboxFindWhat((wxComboBox *)0),
		comboxLookIn((wxComboBox *)0),
		buttonCancel((wxButton *)0),
		buttonFindNext((wxButton *)0),
		regExprFindMenu((wxMenu *)0),
		staticTextLookIn((wxStaticText *)0),
		regExprFindButton((wxButtonBase *)0),
		staticTextFindWhat((wxStaticText *)0),
		regExprFindMenuWindow((wxWindow *)0),
		wxDialog(
			parent, 
			-1, 
			title,	
			wxDefaultPosition, 
			wxDefaultSize, 
			wxSYSTEM_MENU	| 
			wxCAPTION		| 
			wxCLOSE_BOX		|
			wxCLIP_CHILDREN |
			wxFRAME_FLOAT_ON_PARENT
		) {
		whereToLook[frep::CURRENT_DOCUMENT]		= "Current document";
		whereToLook[frep::SELECTION]			= "Selection";
		whereToLook[frep::ALL_OPEN_DOCUMENTS]	= "All opened documents";
		whereToLook[frep::CURRENT_PROJECT]		= "Current project";
		whereToLook[frep::ENTIRE_WORKSPACE]		= "Entire workspace";
		whereToLook[frep::NOWHERE]				= (char*) 0;
	}


	//-----------------------------------------------------------------------
	
	FindAndReplaceUserInterface::~FindAndReplaceUserInterface() {}


	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::OnFindNext (wxCommandEvent & event){
		bool oldValue = manager::settings.searchUp;
		manager::settings.searchUp = false;
		OnButtonFindNext(event);
		manager::settings.searchUp = oldValue;
		return;
	}


	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnFindPrev (wxCommandEvent & event){
		bool oldValue = manager::settings.searchUp;
		manager::settings.searchUp = true;
		OnButtonFindNext(event);
		manager::settings.searchUp = oldValue;
		return;
	}
	

	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::SetNewSearch (void) 
		{ newSearch = true; }
	

	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::OpenDialog (const std::string & selectedText){
		if (!selectedText.empty() && selectedText.compare(wx2std(comboxFindWhat->GetValue())) != 0){ 
			newSearch = true;
			comboxFindWhat->SetValue(std2wx(selectedText));
		}
		comboxFindWhat->SetFocus();
		Show(true);
	}
	

	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::CloseDialog (void)
	{	UPDATE_UI();	}


	/////////////////////////////////////////////////
	//	Protected method implementation
	
	//-----------------------------------------------------------------------
	
	bool FindAndReplaceUserInterface::IsNewSearch(void) {
		if( !newSearch && communicator->HasEditorChanged()){
			if (lookIn == CURRENT_DOCUMENT && communicator->GetEditorPath() != manager::FocusedEditorPath())
				newSearch = true;
			communicator->ChangeEditor(false);
		}
		return newSearch;
	}

	//-----------------------------------------------------------------------
	
	wxBoxSizer *  FindAndReplaceUserInterface::AddLookInSizer(void) {
		lookInSizer		 = new wxBoxSizer( wxVERTICAL );
		staticTextLookIn = new wxStaticText(
									mainPanel, 
									wxID_ANY,
									wxT("Look in:"), 
									wxDefaultPosition, 
									wxDefaultSize, 
									wxALIGN_LEFT
								);

		wxArrayString strings;
		for(int lookIn = CURRENT_DOCUMENT; lookIn != frep::NOWHERE; ++lookIn)
			strings.Add(std2wx(whereToLook[lookIn]));

		comboxLookIn = new wxComboBox(	
								mainPanel, 
								ID_COMBOBOX_LOOKIN, 
								strings[CURRENT_DOCUMENT],
								wxDefaultPosition, 
								wxDefaultSize, 
								strings, 
								wxCB_READONLY
							);

		lookInSizer->Add(staticTextLookIn, 0, wxALL, BORDER);
		lookInSizer->Add(comboxLookIn, 0, wxALIGN_CENTER, BORDER);

		return lookInSizer;
	}

	//-----------------------------------------------------------------------

	wxBoxSizer *  FindAndReplaceUserInterface::AddFindTextSizer(void) {
		findTextSizer			= new wxBoxSizer (wxVERTICAL);
		controlFindTextSizer	= new wxBoxSizer (wxHORIZONTAL);
		

		staticTextFindWhat	= new wxStaticText(	
									mainPanel, 
									wxID_ANY,
									wxT("Find what:"), 
									wxDefaultPosition, 
									wxDefaultSize, 
									wxALIGN_LEFT
								);

		wxArrayString strings;
		comboxFindWhat		= new wxComboBox(	
									mainPanel, 
									ID_COMBOBOX_FIND_WHAT, 
									wxT(""), 
									wxDefaultPosition, 
									wxSize(230,21), 
									strings, 
									wxCB_SORT
								);

		regExprFindButton	= new wxBitmapButton(	
									mainPanel, 
									ID_REGEX_BUTTON_FIND, 
									wxBitmap(expression_editor_xpm), 
									wxDefaultPosition, 
									wxSize(21,21)
								);
		
		regExprFindButton->Disable();
		controlFindTextSizer->Add(comboxFindWhat, 0, wxLEFT, 0);
		controlFindTextSizer->Add(regExprFindButton, 0, wxRIGHT, 0);
		findTextSizer->Add( staticTextFindWhat, 0, wxALL,BORDER);
		findTextSizer->Add( controlFindTextSizer, 0, wxALL, 0);
		return findTextSizer;	
	}

	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::OnCancel(wxCommandEvent& event) 
	{	UPDATE_UI();	}

	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnExit(wxCloseEvent& event) 
	{	UPDATE_UI();	}


	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::DoSearch (bool lookInScripts) {
		if ( comboxFindWhat->FindString(comboxFindWhat->GetValue()) == wxNOT_FOUND)
			comboxFindWhat->Append(comboxFindWhat->GetValue());

		manager::Set( producer->GetTextSourceList(lookIn, lookInScripts) );
		manager::SetFindParam(wx2std(comboxFindWhat->GetValue()), regExprIsActive );		
		
		if (!manager::Find()) {
			if (manager::HasError())
				wxMessageBox( std2wx(std::string(manager::GetError())), wxT(FIND_REPLACE_SEARCH_ERROR_TITLE), wxOK | wxICON_ERROR, this);
			else { 
				wxMessageBox( wxT("The following specified text was not found: ")+comboxFindWhat->GetValue(), 
				wxT(FIND_REPLACE_SEARCH_MSG_TITLE), wxOK | wxICON_ERROR, this);  
			}
		}
	}

	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::OnLookIn(wxCommandEvent& event){
		newSearch		= true;
		lookIn = WhereToLook(wx2std(comboxLookIn->GetValue()));
	}

	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::OnMouseMove (wxMouseEvent& event) 
		{ mousePosotion = event.GetPosition(); }


	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnTextCtrlFindWhat(wxCommandEvent& event)
		{ newSearch	= true; }


	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnCheckBoxSearchUp (wxCommandEvent& event) 
		{ manager::settings.searchUp = event.IsChecked(); }

	//-----------------------------------------------------------------------
	//Match case sensitive word
	void FindAndReplaceUserInterface::OnCheckBoxMatchCase (wxCommandEvent& event){
		newSearch		= true;
		manager::settings.isCaseSensitive = event.IsChecked();
	}

	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::OnCheckBoxMatchWholeWord (wxCommandEvent& event){
		newSearch		= true;
		manager::settings.wholeWord = event.IsChecked();
	}

	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnButtonRegexFindMenu (wxCommandEvent &event){
		
		if (!regExprFindMenuWindow){
			regExprFindMenu			= CreateRegExprFindMenu();
			regExprFindMenuWindow	= new wxWindow(mainPanel, wxID_ANY, mousePosotion);
		}

		regExprFindMenuWindow->PopupMenu(regExprFindMenu);
		regExprFindMenuWindow->SetName(wxT("Find Menu") );
		return;
	}

	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnOr (wxCommandEvent & event)
		{ comboxFindWhat->SetValue(wxT("<") + comboxFindWhat->GetValue() + wxT("|>"));	}

	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnTag (wxCommandEvent & event)
		{ comboxFindWhat->SetValue( wxT("{") + comboxFindWhat->GetValue() + wxT("}"));	}
	
	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnInteger (wxCommandEvent & event)
		{ comboxFindWhat->SetValue(comboxFindWhat->GetValue()+wxT(":z")); }
	
	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnAnyChar (wxCommandEvent & event)
		{ comboxFindWhat->SetValue(comboxFindWhat->GetValue()+wxT(".")); }
	
	
	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnDelteIden (wxCommandEvent & event)
		{ comboxFindWhat->SetValue(comboxFindWhat->GetValue()+wxT(":i")); }

	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnQuotedString (wxCommandEvent & event)
		{ comboxFindWhat->SetValue(comboxFindWhat->GetValue()+wxT(":q")); }

	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnEsqSpecialCharacter (wxCommandEvent & event)
		{ comboxFindWhat->SetValue(comboxFindWhat->GetValue()+wxT("\\")); }

	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnOneOrMore (wxCommandEvent & event)
		{ comboxFindWhat->SetValue(wxT("[") + comboxFindWhat->GetValue() + wxT("]+")); }

	//-----------------------------------------------------------------------
	
	void FindAndReplaceUserInterface::OnZeroOrMore (wxCommandEvent & event)
		{ comboxFindWhat->SetValue(wxT("(") + comboxFindWhat->GetValue() + wxT(")*")); }

	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::OnSpaceOrBar (wxCommandEvent & event)
		{ comboxFindWhat->SetValue(comboxFindWhat->GetValue()+wxT(":w")); }

	//-----------------------------------------------------------------------

	void FindAndReplaceUserInterface::UpdateAllRegExprCheckBoxs(const bool b) 
		{ UserInterfaceUpdater::SetCheckBoxRegExprValue(b); }




	/////////////////////////////////////////////////
	//	private method implementation



	//-----------------------------------------------------------------------
	
	frep::LookIn FindAndReplaceUserInterface::WhereToLook (const std::string & str) {
		if (str == whereToLook[frep::CURRENT_DOCUMENT]) 
			return frep::CURRENT_DOCUMENT;

		else if (str == whereToLook[frep::SELECTION])
			return frep::SELECTION;

		else if (str == whereToLook[frep::ALL_OPEN_DOCUMENTS])
			return frep::ALL_OPEN_DOCUMENTS;

		else if (str == whereToLook[frep::CURRENT_PROJECT])
			return frep::CURRENT_PROJECT;

		else if (str == whereToLook[frep::ENTIRE_WORKSPACE])
			return frep::ENTIRE_WORKSPACE;
		else
			assert(0);
		return frep::NOWHERE;
	}


	//-----------------------------------------------------------------------

	wxMenu * FindAndReplaceUserInterface::CreateRegExprFindMenu (void){
		regExprFindMenu = new wxMenu(wxT(""), wxMENU_TEAROFF);

		regExprFindMenu->Append(ID_REGEX_MENU_ANY_CHAR,			wxT(". Any single character"));
		regExprFindMenu->Append(ID_REGEX_MENU_ONE_OR_MORE,		wxT("[]+ One or more"));
		regExprFindMenu->Append(ID_REGEX_MENU_ZERO_OR_MORE,		wxT("()* Zero or more"));
		regExprFindMenu->AppendSeparator();
		regExprFindMenu->Append(ID_REGEX_MENU_OR,				wxT("<|> Or"));
		regExprFindMenu->Append(ID_REGEX_MENU_TAG,				wxT("{} Tag expression"));
		regExprFindMenu->Append(ID_REGEX_MENU_ESC_SPECIAL_CHAR,	wxT("\\ Escape Special Character"));
		regExprFindMenu->AppendSeparator();
		regExprFindMenu->Append(ID_REGEX_MENU_DELTA_IDENTIFIER,	wxT(":i Delta Identifier"));
		regExprFindMenu->Append(ID_REGEX_MENU_QUOTED_STRING,	wxT(":q Quoted String"));
		regExprFindMenu->Append(ID_REGEX_MENU_SPACE_OR_BAR,		wxT(":w Space ot Tab"));
		regExprFindMenu->Append(ID_REGEX_MENU_INTEGER,			wxT(":z Integer"));

		return regExprFindMenu;
	}
	

	///--------------------		UserInterfaceUpdater	---------------------------///

	bool						UserInterfaceUpdater::singletonCreated = false;
	UserInterfaceUpdater *		UserInterfaceUpdater::singleton = 0;
	std::list<FindAndReplaceUserInterface *>	UserInterfaceUpdater::registry;


	//-----------------------------------------------------------------------

	UserInterfaceUpdater::UserInterfaceUpdater(void){}


	//-----------------------------------------------------------------------

	UserInterfaceUpdater::~UserInterfaceUpdater(void){}

	
	//-----------------------------------------------------------------------

	void UserInterfaceUpdater::SingletonCreate(void) {
		assert(!singleton);
		singleton			= new UserInterfaceUpdater();
		singletonCreated	= true;
	}


	//-----------------------------------------------------------------------
	
	bool UserInterfaceUpdater::SingletonCreated(void) 
		{ return singletonCreated; }


	//-----------------------------------------------------------------------
	
	void UserInterfaceUpdater::SingletonDestroy(void) {
		assert(singleton);
		delete singleton;
		singletonCreated = false;
	}


	//-----------------------------------------------------------------------
	
	UserInterfaceUpdater & UserInterfaceUpdater::SingletonInstance(void) {
		assert(singleton);
		return *singleton;
	}


	//-----------------------------------------------------------------------

	void UserInterfaceUpdater::RegisterUserInterface(FindAndReplaceUserInterface * ds) {
		assert(ds);
		registry.push_back(ds);
	}


#define FOR_ALL_INSTANCES(ITER)										\
	std::list<FindAndReplaceUserInterface *>::iterator ITER = registry.begin();	\
		for(; ITER != registry.end(); ++ITER)

#define SET_ELEMENT_VALUE(ELEMENT, VALUE)							\
	FOR_ALL_INSTANCES(i)											\
		(*i)->ELEMENT->SetValue(VALUE)


	//-----------------------------------------------------------------------

	void UserInterfaceUpdater::SetCheckBoxMatchCaseValue(const bool b)
		{ SET_ELEMENT_VALUE(matchCase, b); }
	

	//-----------------------------------------------------------------------

	void UserInterfaceUpdater::SetCheckBoxWholeWordValue(const bool b)
		{ SET_ELEMENT_VALUE(matchWholeWord, b); }


	//-----------------------------------------------------------------------

	void UserInterfaceUpdater::SetCheckBoxSearchUpValue(const bool b)
		{ SET_ELEMENT_VALUE(searchUp, b); }

	
	//-----------------------------------------------------------------------

	void UserInterfaceUpdater::SetComboxFindWhatValue(const wxString & str)
		{ SET_ELEMENT_VALUE(comboxFindWhat, str); }

	
	//-----------------------------------------------------------------------

	void UserInterfaceUpdater::SetComboxLookInValue(const wxString & str)
		{ SET_ELEMENT_VALUE(comboxLookIn, str); }


	//-----------------------------------------------------------------------

	void UserInterfaceUpdater::SetCheckBoxRegExprValue(const bool b){ 
		FOR_ALL_INSTANCES(i) {							
			(*i)->regex->SetValue(b);
			(*i)->EnableAllRegExprButtons(b);
		}
	}


}
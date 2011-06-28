/*
 * ReplaceDialog.cpp 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */



#include <assert.h>
#include <string>


#include "KeyLogger.h"
#include "ReplaceDialog.h"
#include "StringHelpers.h"
#include "FindAndReplaceManager.h"
#include "Icons/expression_editor.xpm"



#define BORDER 5
#define ACCELERATOR_ENTRY 2



#define CURR_DOC		"Current document"
#define ALL_OPEN_DOC	"All opened documents"
#define CURR_PRO		"Current project"
#define ENT_WORC		"Entire workspace"



namespace frep {

	typedef FindAndReplaceManager manager;


	enum{
		ID_COMBOBOX_REPLACE_WITH				= 100,
		ID_CHECKBOX_REGEX						= 101,
		ID_BUTTON_REGEX_REPLACE					= 102,
		ID_BUTTON_FIND_NEXT						= 103,
		ID_BUTTON_REPLACE						= 104,
		ID_BUTTON_REPLACE_ALL					= 105,
		ID_REGEX_MENU_DELTA_IDENTIFIER			= 106,
		ID_REGEX_MENU_TAGGED_EXPRESSION_1		= 107,
		ID_REGEX_MENU_TAGGED_EXPRESSION_2		= 108,
		ID_REGEX_MENU_TAGGED_EXPRESSION_3		= 109,
		ID_REGEX_MENU_TAGGED_EXPRESSION_4		= 110,
		ID_REGEX_MENU_TAGGED_EXPRESSION_MORE	= 111,
	};


	// Event table for ReplaceDialog
	BEGIN_EVENT_TABLE(ReplaceDialog, FindAndReplaceUserInterface)
		EVT_TEXT		(ID_COMBOBOX_REPLACE_WITH,				ReplaceDialog::OnTextCtrlReplaceWith)
		EVT_CHECKBOX	(ID_CHECKBOX_REGEX,						ReplaceDialog::OnCheckBoxRegex)
		EVT_BUTTON		(ID_BUTTON_REGEX_REPLACE,				ReplaceDialog::OnButtonRegexReplaceMenu)
		EVT_BUTTON		(ID_BUTTON_FIND_NEXT,					ReplaceDialog::OnButtonFindNext)
		EVT_BUTTON		(ID_BUTTON_REPLACE,						ReplaceDialog::OnButtonReplace)
		EVT_BUTTON		(ID_BUTTON_REPLACE_ALL,					ReplaceDialog::OnButtonReplaceAll)
		EVT_MENU		(ID_REGEX_MENU_TAGGED_EXPRESSION_1,		ReplaceDialog::OnTagRep1)
		EVT_MENU		(ID_REGEX_MENU_TAGGED_EXPRESSION_2,		ReplaceDialog::OnTagRep2)
		EVT_MENU		(ID_REGEX_MENU_TAGGED_EXPRESSION_3,		ReplaceDialog::OnTagRep3)
		EVT_MENU		(ID_REGEX_MENU_TAGGED_EXPRESSION_MORE,	ReplaceDialog::OnTagRepMore)
	END_EVENT_TABLE()



	/////////////////////////////////////////////////
	//	Public method implementation

	//-----------------------------------------------------------------------
	
	ReplaceDialog::~ReplaceDialog(void) {
		buttonCancel->Destroy();
		buttonReplaceAll->Destroy();
		buttonReplace->Destroy();
		buttonFindNext->Destroy();
		regex->Destroy();
		searchUp->Destroy();
		matchWholeWord->Destroy();
		matchCase->Destroy();
		staticBox->Destroy();

		comboxLookIn->Destroy();
		staticTextLookIn->Destroy();

		textReplaceWith->Destroy();
		staticTextReplaceWith->Destroy();

		comboxFindWhat->Destroy();
		staticTextFindWhat->Destroy();
		
		if (regExprFindMenuWindow)
			regExprFindMenuWindow->Destroy();

		if (menuReplaceWindow)
			menuReplaceWindow->Destroy();

		regExprFindButton->Destroy();
		regexReplaceButton->Destroy();

		mainPanel->Destroy();
		
		FindAndReplaceManager::CleanUp();
		//KeyLogger::Terminate();
		Destroy();
	} 


	//-----------------------------------------------------------------------

	ReplaceDialog::ReplaceDialog(	
		wxWindow* parent,
		TextSourceCollectionProducer * pro, 
		ApplicationCommunicator * com
	) : menuReplaceWindow((wxWindow *)0),
		regexReplaceMenu( (wxMenu *)0 ),
		FindAndReplaceUserInterface(_("Find and replace"), parent, pro, com)
	{
		//KeyLogger::Init("==========================================koutsop.txt");
		wxImage::AddHandler( new wxXPMHandler );
		FindAndReplaceManager::Initialise();
		mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);


		//create accelerator table events
		wxAcceleratorEntry entries[ACCELERATOR_ENTRY];
		entries[0].Set (wxACCEL_NORMAL,	WXK_F3, ID_FIND_NEXT);
		entries[1].Set (wxACCEL_SHIFT,	WXK_F3, ID_FIND_PREV);
		wxAcceleratorTable accel(ACCELERATOR_ENTRY, entries);
		this->SetAcceleratorTable(accel);


		// Create top-level grid sizer
		gridSizer			= new wxFlexGridSizer(4, 0, 0, 0);
		findTextSizer		= AddFindTextSizer();
		replaceTextSizer	= AddReplaceTextSizer();
		lookInSizer			= AddLookInSizer();
		bottomSizer			= AddBottomSizer();
		
		gridSizer->Add(findTextSizer, 0, wxALIGN_LEFT |wxALL, BORDER);
		gridSizer->Add(replaceTextSizer, 0, wxALIGN_LEFT |wxALL, BORDER);
		gridSizer->Add(lookInSizer, 0, wxALIGN_LEFT |wxALL, BORDER);
		gridSizer->Add(bottomSizer, 0, wxALIGN_LEFT |wxALL, BORDER);
		

		mainPanel->SetSizer(gridSizer);
		gridSizer->Fit(this);
		gridSizer->SetSizeHints(this);
	}




	/////////////////////////////////////////////////
	//	private method implementation


	//-----------------------------------------------------------------------

	wxBoxSizer * ReplaceDialog::AddReplaceTextSizer (void){
		replaceTextSizer		= new wxBoxSizer (wxVERTICAL);
		controlReplaceTextSizer	= new wxBoxSizer (wxHORIZONTAL);
		

		staticTextReplaceWith = new wxStaticText(	
										mainPanel, 
										wxID_ANY,
										wxT("Replace with:"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										wxALIGN_LEFT
									);
		
		wxArrayString strings;
		textReplaceWith = new wxComboBox(	
									mainPanel, 
									ID_COMBOBOX_REPLACE_WITH, 
									wxT(""), 
									wxDefaultPosition, 
									wxSize(230,21), 
									strings, 
									wxCB_SORT
								);

		regexReplaceButton = new wxBitmapButton(
									mainPanel, 
									ID_BUTTON_REGEX_REPLACE, 
									wxBitmap(expression_editor_xpm), 
									wxDefaultPosition, 
									wxSize(21,21)
								);

		regexReplaceButton->Disable();
		controlReplaceTextSizer->Add(textReplaceWith, 0, wxLEFT, 0);
		controlReplaceTextSizer->Add(regexReplaceButton, 0, wxRIGHT, 0);
		findTextSizer->Add(staticTextReplaceWith, 0, wxALL,BORDER);
		findTextSizer->Add(controlReplaceTextSizer, 0, wxALL, 0);
		
		return replaceTextSizer;
	}

	//-----------------------------------------------------------------------
	
	wxBoxSizer * ReplaceDialog::AddBottomSizer (void){
		bottomSizer		= new wxBoxSizer( wxHORIZONTAL );
		staticBox		= new wxStaticBox(mainPanel,wxID_ANY, wxT("Find options"));
		staticSizer		= new wxStaticBoxSizer(staticBox,wxVERTICAL);
		matchCase		= new wxCheckBox(	
								mainPanel, 
								ID_CHECKBOX_MATCH_CASE,
								wxT("Match case"),
								wxDefaultPosition, 
								wxDefaultSize
							);
		
		matchWholeWord	= new wxCheckBox(	
								mainPanel, 
								ID_CHECKBOX_MATCH_WHOLE_WORD, 
								wxT("Math whole word"),
								wxDefaultPosition, 
								wxDefaultSize
							);

		searchUp		= new wxCheckBox(	
								mainPanel, 
								ID_CHECKBOX_SEARCH_UP, 
								wxT("Search up"),
								wxDefaultPosition, 
								wxDefaultSize
							);

		regex			= new wxCheckBox(	
								mainPanel, 
								ID_CHECKBOX_REGEX, 
								wxT("Use regular expressions"),
								wxDefaultPosition, 
								wxDefaultSize
							);

		staticSizer->Add(matchCase, 0, wxALIGN_LEFT |wxALL, BORDER/2);
		staticSizer->Add(matchWholeWord, 0, wxALIGN_LEFT |wxALL, BORDER/2);
		staticSizer->Add(searchUp, 0, wxALIGN_LEFT |wxALL, BORDER/2);
		staticSizer->Add(regex, 0, wxALIGN_LEFT |wxALL, BORDER/2);

		//create the buttons
		buttonSizer			= new wxBoxSizer( wxVERTICAL );
		buttonCancel		= new wxButton(
									mainPanel, 
									wxID_CANCEL, 
									wxT("Cancel"),
									wxDefaultPosition, 
									wxSize(75,17)
								);
		buttonFindNext		= new wxButton(
									mainPanel, 
									ID_BUTTON_FIND_NEXT, 
									wxT("Find next"),
									wxDefaultPosition, 
									wxSize(75,17)
								);
		buttonReplace		= new wxButton(
									mainPanel, 
									ID_BUTTON_REPLACE, 
									wxT("Replace"),
									wxDefaultPosition, 
									wxSize(75,17)
								);
		buttonReplaceAll	= new wxButton(
									mainPanel, 
									ID_BUTTON_REPLACE_ALL, 
									wxT("Replace all"),
									wxDefaultPosition, 
									wxSize(75,17)
								);
		
		this->SetDefaultItem(buttonFindNext);
		buttonReplace->Enable(false);
		buttonReplaceAll->Enable(false);

		buttonSizer->Add(buttonFindNext, 0, wxALL, BORDER/2);
		buttonSizer->Add(buttonReplace, 0, wxALL, BORDER/2);
		buttonSizer->Add(buttonReplaceAll, 0, wxALL, BORDER/2);
		buttonSizer->Add(buttonCancel, 0, wxALL, BORDER/2);

		bottomSizer->Add(staticSizer, 0, wxALL, BORDER);
		bottomSizer->Add(buttonSizer, 0, wxALIGN_CENTER, BORDER);

		return bottomSizer;
	}

	//-----------------------------------------------------------------------
	
	wxMenu * ReplaceDialog::CreateRegExprReplaceMenu (void){
		regexReplaceMenu= new wxMenu(wxT(""), wxMENU_TEAROFF);

		regexReplaceMenu->Append(ID_REGEX_MENU_TAGGED_EXPRESSION_1,		wxT("Tagged Expression 1"));
		regexReplaceMenu->Append(ID_REGEX_MENU_TAGGED_EXPRESSION_2,		wxT("Tagged Expression 2"));
		regexReplaceMenu->Append(ID_REGEX_MENU_TAGGED_EXPRESSION_3,		wxT("Tagged Expression 3"));
		regexReplaceMenu->Append(ID_REGEX_MENU_TAGGED_EXPRESSION_MORE,	wxT("Tagged Expression ..."));
		return regexReplaceMenu;
	}

	//-----------------------------------------------------------------------

	void ReplaceDialog::EnableAllRegExprButtons(const bool b){ 
		regExprFindButton->Enable(b); 
		regexReplaceButton->Enable(b);
	}

	//-----------------------------------------------------------------------
	
	void ReplaceDialog::OnTextCtrlReplaceWith(wxCommandEvent& event){
		newSearch = true; //opote fgrafoume koimeno shmenei oti exoume kai new search na kanoume
		buttonReplace->Disable();
		buttonReplaceAll->Disable();
	}

	//-----------------------------------------------------------------------
	
	void ReplaceDialog::OnCheckBoxRegex (wxCommandEvent& event){
		newSearch = true;
		regExprIsActive = event.IsChecked();
		regExprFindButton->Enable(regExprIsActive);
		regexReplaceButton->Enable(regExprIsActive);
	}

	//-----------------------------------------------------------------------

	void ReplaceDialog::OnButtonFindNext (wxCommandEvent& event){
		
		if ( comboxFindWhat->GetValue().size() == 0 ) { 
			wxMessageBox( 
				wxT("The search pattern is invalid"), 
				wxT(FIND_REPLACE_SEARCH_ERROR_TITLE), 
				wxOK | wxICON_INFORMATION, 
				this
			);
			return;
		}

		if (IsNewSearch())
			DoSearch(false);
		
		if ( manager::HasResults() ){
			newSearch = false;
			manager::SelectText(manager::FindNext());
		}
	}

	//-----------------------------------------------------------------------
	
	void ReplaceDialog::OnButtonReplace (wxCommandEvent& event){
		
		if ( comboxFindWhat->FindString(comboxFindWhat->GetValue()) == wxNOT_FOUND)
			comboxFindWhat->Append(comboxFindWhat->GetValue());

		if ( textReplaceWith->FindString(textReplaceWith->GetValue()) == wxNOT_FOUND)
			textReplaceWith->Append(textReplaceWith->GetValue());

		manager::SetReplaceParam(
			wx2std(comboxFindWhat->GetValue()), 
			wx2std(textReplaceWith->GetValue()),
			regExprIsActive
		);
		
		if (newSearch)
			OnButtonFindNext(event);
	
		else if( !manager::Replace() && manager::HasError())
			wxMessageBox( 
				std2wx(std::string(manager::GetError())), 
				wxT(FIND_REPLACE_SEARCH_ERROR_TITLE), 
				wxOK | wxICON_ERROR, 
				this
			);
		return;
	}

	//-----------------------------------------------------------------------

	void ReplaceDialog::OnButtonReplaceAll (wxCommandEvent& event){
		
		bool undo = false;
		manager::SetReplaceParam(
			wx2std(comboxFindWhat->GetValue()), 
			wx2std(textReplaceWith->GetValue()),
			regExprIsActive
		);
		
		if (newSearch)
			OnButtonFindNext(event);	//Kanoume thn anazhthsh
		if (undo = communicator->CanDoUndoAction())
			communicator->BeginUndoAction();
		if ( !manager::ReplaceAll() && manager::HasError()) 
			wxMessageBox( 
				std2wx(std::string(manager::GetError())), 
				wxT(FIND_REPLACE_SEARCH_ERROR_TITLE), 
				wxOK | wxICON_ERROR, 
				this
			);
		if (undo)
			communicator->EndUndoAction();
		return;
	}

	//-----------------------------------------------------------------------
	
	void ReplaceDialog::OnButtonRegexReplaceMenu (wxCommandEvent &event){
		
		if (!menuReplaceWindow){
			regexReplaceMenu	= CreateRegExprReplaceMenu();
			menuReplaceWindow	= new wxWindow(mainPanel, wxID_ANY, mousePosotion);
		}

		menuReplaceWindow->PopupMenu(regexReplaceMenu);
		menuReplaceWindow->SetName(wxT("Replace Menu"));
		return;
	}

	//-----------------------------------------------------------------------
	
	void ReplaceDialog::OnTagRep1 (wxCommandEvent & event) 
		{ textReplaceWith->SetValue(textReplaceWith->GetValue()+wxT("\\1")); }

	//-----------------------------------------------------------------------
	
	void ReplaceDialog::OnTagRep2 (wxCommandEvent & event) 
		{ textReplaceWith->SetValue(textReplaceWith->GetValue()+wxT("\\2")); }

	//-----------------------------------------------------------------------
	
	void ReplaceDialog::OnTagRep3 (wxCommandEvent & event) 
		{ textReplaceWith->SetValue(textReplaceWith->GetValue()+wxT("\\3")); }

	//-----------------------------------------------------------------------
	
	void ReplaceDialog::OnTagRepMore (wxCommandEvent & event) 
		{ textReplaceWith->SetValue(textReplaceWith->GetValue()+wxT("\\"));	}
} //namespace frep

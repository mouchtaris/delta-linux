/*
 * FindDialog.cpp 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#include <string>
#include <assert.h>
#include <wx/bmpbuttn.h>

#include "KeyLogger.h"
#include "FindDialog.h"
#include "StringHelpers.h"
#include "FindAndReplaceManager.h"
//#include "Icons/expression_editor.xpm"



#define BORDER 5
#define ACCELERATOR_ENTRY 4


namespace frep {

	typedef FindAndReplaceManager manager;

	// Event table for FindDialog
	BEGIN_EVENT_TABLE(FindDialog, FindAndReplaceUserInterface)
		EVT_CHECKBOX	(ID_CHECKBOX_FIND_IN_FILES,	FindDialog::OnCheckBoxFindInFiles)
		EVT_CHECKBOX	(ID_CHECKBOX_REGEX,			FindDialog::OnCheckBoxRegex)
		EVT_BUTTON		(ID_BUTTON_FIND_NEXT,		FindDialog::OnButtonFindNext)
		EVT_BUTTON		(ID_BUTTON_BOOKMARK_ALL,	FindDialog::OnButtonBookmarkAll)
	END_EVENT_TABLE()



	/////////////////////////////////////////////////
	//	Public method implementation


	//-----------------------------------------------------------------------
	FindDialog::~FindDialog(void) {
		buttonCancel->Destroy();
		buttonBookmarkAll->Destroy();
		buttonFindNext->Destroy();
		regex->Destroy();
		searchUp->Destroy();
		matchWholeWord->Destroy();
		matchCase->Destroy();
		findInFiles->Destroy();
		staticBox->Destroy();
		comboxLookIn->Destroy();
		staticTextLookIn->Destroy();
		comboxFindWhat->Destroy();
		staticTextFindWhat->Destroy();
		if (regExprFindMenuWindow)
			regExprFindMenuWindow->Destroy();
		regExprFindButton->Destroy();
		mainPanel->Destroy();

		Destroy();
		FindAndReplaceManager::CleanUp();
	} 

	//-----------------------------------------------------------------------
	
	FindDialog::FindDialog(	
		wxWindow* parent, 
		TextSourceCollectionProducer * pro, 
		ApplicationCommunicator * com
	) : findInScript(false),
		FindAndReplaceUserInterface(_("Find"), parent, pro, com)
	{
		wxImage::AddHandler( new wxXPMHandler );
		FindAndReplaceManager::Initialise();
		mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

		//create accelerator table events
		wxAcceleratorEntry entries[ACCELERATOR_ENTRY];
		entries[0].Set(wxACCEL_NORMAL,	WXK_F3, ID_FIND_NEXT);
		entries[1].Set(wxACCEL_SHIFT,	WXK_F3, ID_FIND_PREV);
		entries[2].Set(wxACCEL_CTRL,	(int) 'H', wxID_ANY);
		entries[3].Set(wxACCEL_CTRL,	(int) 'F', wxID_ANY);
		wxAcceleratorTable accel(ACCELERATOR_ENTRY, entries);
		this->SetAcceleratorTable(accel);


		// Create top-level grid sizer
		gridSizer		= new wxFlexGridSizer(3, 0, 0, 0);
		findTextSizer	= AddFindTextSizer();
		lookInSizer		= AddLookInSizer();
		bottomSizer		= AddBottomSizer();
		
		gridSizer->Add(findTextSizer, 0, wxALIGN_LEFT |wxALL, BORDER);
		gridSizer->Add(lookInSizer, 0, wxALIGN_LEFT |wxALL, BORDER);
		gridSizer->Add(bottomSizer, 0, wxALIGN_LEFT |wxALL, BORDER);
		

		mainPanel->SetSizer(gridSizer);
		gridSizer->Fit(this);
		gridSizer->SetSizeHints(this);
	}



	/////////////////////////////////////////////////
	//	Private method implementation

	//-----------------------------------------------------------------------
	
	wxBoxSizer * FindDialog::AddBottomSizer (void){
		bottomSizer		= new wxBoxSizer( wxHORIZONTAL );
		staticBox		= new wxStaticBox(mainPanel,wxID_ANY, wxT("Find options"));
		staticSizer		= new wxStaticBoxSizer(staticBox,wxVERTICAL);

		findInFiles		= new wxCheckBox(	
								mainPanel, 
								ID_CHECKBOX_FIND_IN_FILES,
								wxT("Find in files"),
								wxDefaultPosition, 
								wxDefaultSize
							);

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
								wxT("Search upwards"),
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

		staticSizer->Add(findInFiles, 0, wxALIGN_LEFT |wxALL, BORDER/2);
		staticSizer->Add(matchCase, 0, wxALIGN_LEFT |wxALL, BORDER/2);
		staticSizer->Add(matchWholeWord, 0, wxALIGN_LEFT |wxALL, BORDER/2);
		staticSizer->Add(searchUp, 0, wxALIGN_LEFT |wxALL, BORDER/2);
		staticSizer->Add(regex, 0, wxALIGN_LEFT |wxALL, BORDER/2);

		//create the buttons
		buttonSizer			= new wxBoxSizer( wxVERTICAL );
		buttonCancel		= new wxButton(mainPanel, wxID_CANCEL, wxT("Cancel"));
		buttonFindNext		= new wxButton(mainPanel, ID_BUTTON_FIND_NEXT, wxT("Find next"));
		buttonBookmarkAll	= new wxButton(mainPanel, ID_BUTTON_BOOKMARK_ALL, wxT("Bookmark all"));

		this->SetDefaultItem(buttonFindNext);
		
		buttonSizer->Add(buttonFindNext, 0, wxALL, BORDER);
		buttonSizer->Add(buttonBookmarkAll, 0, wxALL, BORDER);
		buttonSizer->Add(buttonCancel, 0, wxALL, BORDER);

		bottomSizer->Add(staticSizer, 0, wxALL, BORDER);
		bottomSizer->Add(buttonSizer, 0, wxALIGN_CENTER, BORDER);

		return bottomSizer;
	}

	//-----------------------------------------------------------------------

	void FindDialog::EnableAllRegExprButtons(const bool b)
		{ regExprFindButton->Enable(b); }

	//-----------------------------------------------------------------------	

	void FindDialog::OnCheckBoxFindInFiles (wxCommandEvent& event){
		newSearch = true;

		if ( event.IsChecked() ){
			findInScript = true;
			buttonBookmarkAll->Disable();
		}
		else {
			findInScript = false;
			buttonBookmarkAll->Enable();
		}
		return;
	}

	//-----------------------------------------------------------------------
	
	void FindDialog::OnCheckBoxRegex (wxCommandEvent& event){
		newSearch		= true;
		regExprIsActive = event.IsChecked();
		regExprFindButton->Enable(regExprIsActive);
	}

	//-----------------------------------------------------------------------

	void FindDialog::OnButtonFindNext (wxCommandEvent& event){

		if (!comboxFindWhat->GetValue().size()) { 
			wxMessageBox( 
				wxT("The search pattern is invalid"), 
				wxT(FIND_REPLACE_SEARCH_ERROR_TITLE), 
				wxOK | wxICON_INFORMATION, 
				this
			);
			return;
		}

		if (IsNewSearch())
			DoSearch(findInScript);

		if ( manager::HasResults() ){
			if (findInScript) {
				if (communicator->CanWriteToOutput())
					communicator->WriteToOutput(manager::WriteResultsToOutput());
			}
			else {
				newSearch = false;
				manager::SelectText(manager::FindNext());	//current result to desplay
			}
		}
		return;
	}

	//-----------------------------------------------------------------------
	void FindDialog::OnButtonBookmarkAll(wxCommandEvent& event)
		{ manager::BookmarkAll(); }
} //namespace frep
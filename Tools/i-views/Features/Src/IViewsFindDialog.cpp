#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif


#include "IViewsFindDialog.h"


namespace iviews { 


// Event table for FindAndReplaceUserInterface
BEGIN_EVENT_TABLE(IViewsFindDialog, frep::FindDialog)
	EVT_TEXT (ID_COMBOBOX_LOOKIN, FindAndReplaceUserInterface::OnLookIn)
END_EVENT_TABLE()


//-----------------------------------------------------------------------

IViewsFindDialog::IViewsFindDialog(	
	wxWindow* parent, 
	frep::TextSourceCollectionProducer * pro, 
	frep::ApplicationCommunicator * com
) :	frep::FindDialog(parent, pro, com)
{
	comboxLookIn->Destroy();
	wxArrayString strings;
	strings.Add(wxT("Entire Graph"));
	strings.Add(wxT("Last visited vertex"));
	
	lookIn = frep::ENTIRE_GRAPH;

	comboxLookIn = new wxComboBox(	
							mainPanel, 
							ID_COMBOBOX_LOOKIN, 
							strings[0],
							wxDefaultPosition, 
							wxDefaultSize, 
							strings, 
							wxCB_READONLY
						);
	lookInSizer->Add(comboxLookIn, 0, wxALIGN_CENTER, 5);

	findInFiles->Show(false);
	staticSizer->Remove(findInFiles);
	staticSizer->Layout();

	buttonBookmarkAll->Show(false);
	buttonSizer->Remove(buttonBookmarkAll);
	buttonSizer->Layout();

	gridSizer->Layout();
}

//-----------------------------------------------------------------------

void IViewsFindDialog::OnLookIn (wxCommandEvent& event){
	newSearch = true;

	if (wx2std(comboxLookIn->GetValue()) == "Last visited vertex")
		lookIn = frep::LAST_VISITED_VERTEX;
	else if (wx2std(comboxLookIn->GetValue()) == "Entire Graph")
		lookIn = frep::ENTIRE_GRAPH;
	else
		assert(0);
}

}	//namespace iviews
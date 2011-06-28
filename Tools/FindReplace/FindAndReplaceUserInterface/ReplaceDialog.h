/*
 * ReplaceDialog.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <stdio.h>
#include <string>

#include "StringHelpers.h"
#include "FindAndReplaceUserInterface.h"

#pragma warning(disable: 4996)

namespace frep {

	class ReplaceDialog : public FindAndReplaceUserInterface {
	public:
		~ReplaceDialog (void);
		ReplaceDialog(wxWindow* parent,TextSourceCollectionProducer *, ApplicationCommunicator *);		
		
		void SetButtonReplaceEnable (const bool b) { buttonReplace->Enable(b); }
		void SetButtonReplaceAllEnable (const bool b) { buttonReplaceAll->Enable(b); }


	private:

		//****************************
		//		private variables
		//Sizers
		wxFlexGridSizer *	gridSizer;
		wxBoxSizer *		replaceTextSizer;
		wxBoxSizer *		controlReplaceTextSizer;
		wxBoxSizer *		bottomSizer;
		wxStaticBoxSizer *	staticSizer;
		wxBoxSizer *		buttonSizer;

		//Regex vars replace
		wxWindow *			menuReplaceWindow;
		wxMenu *			regexReplaceMenu;
		wxButtonBase *		regexReplaceButton;

		//top sizer Second
		wxStaticText *		staticTextReplaceWith;
		wxComboBox *		textReplaceWith;
		
		//bottom sizer
		wxButton *			buttonReplace;
		wxButton *			buttonReplaceAll;

		

		//****************************************************
		//		private methods

		wxBoxSizer * AddReplaceTextSizer (void);
		wxBoxSizer * AddBottomSizer (void);	
		wxMenu *	 CreateRegExprReplaceMenu (void);
		
				
		void OnTextCtrlReplaceWith(wxCommandEvent& event);		
		void OnCheckBoxRegex (wxCommandEvent& event);
		void OnButtonReplace (wxCommandEvent& event);
		void OnButtonFindNext (wxCommandEvent& event);
		void OnButtonReplaceAll (wxCommandEvent& event);
		void OnButtonRegexReplaceMenu (wxCommandEvent& event);
		void OnTagRep1 (wxCommandEvent & event);
		void OnTagRep2 (wxCommandEvent & event);
		void OnTagRep3 (wxCommandEvent & event);
		void OnTagRepMore (wxCommandEvent & event);
		
		virtual void EnableAllRegExprButtons(const bool);

		// This class handles events
		DECLARE_EVENT_TABLE()
	}; 

}//namespace frep

#endif	  // REPLACEDIALOG_H
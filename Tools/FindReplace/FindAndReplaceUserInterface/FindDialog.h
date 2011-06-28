/*
 * FindDialog.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <stdio.h>
#include <string>

#include "StringHelpers.h"
#include "FindAndReplaceUserInterface.h"


#pragma warning(disable: 4996)

namespace frep {

	class FindDialog : public FindAndReplaceUserInterface {
	public:
		~FindDialog (void);
		FindDialog (wxWindow*, TextSourceCollectionProducer *, ApplicationCommunicator *);


	protected:
		enum{
			ID_CHECKBOX_FIND_IN_FILES		= 100,
			ID_CHECKBOX_REGEX				= 101,
			ID_BUTTON_FIND_NEXT				= 102,
			ID_BUTTON_BOOKMARK_ALL			= 103,
		};

		//****************************
		//		private variables
		bool				findInScript;
		wxButton *			buttonBookmarkAll;
		wxCheckBox *		findInFiles;
		wxBoxSizer *		bottomSizer;
		wxBoxSizer *		buttonSizer;
		wxFlexGridSizer *	gridSizer;
		wxStaticBoxSizer *	staticSizer;
		

		//****************************************************
		//		private methods
		wxBoxSizer * 	AddBottomSizer (void);	

		//void EventPropagate (wxCommandEvent& event);

		void 			OnCheckBoxFindInFiles (wxCommandEvent& event);
		void 			OnButtonBookmarkAll (wxCommandEvent& event);				
		virtual void 	OnCheckBoxRegex (wxCommandEvent& event);
		virtual void 	OnButtonFindNext (wxCommandEvent& event);
		virtual void	EnableAllRegExprButtons(const bool);
		

		// This class handles events
		DECLARE_EVENT_TABLE()
	}; 


} //namespace frep
#endif		//FINDDIALOG_H
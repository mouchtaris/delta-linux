/*
 * FindAndReplaceUserInterface.h 
 * author: Koutsopoulos Nikolaos
 * 
 * Super class for the find and replace UGI 
 *
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H


#include "StringHelpers.h"
#include "ApplicationCommunicator.h"
#include "TextSourceCollectionProducer.h"



namespace frep {

	class FindAndReplaceUserInterface  : public wxDialog {
	public:
		FindAndReplaceUserInterface(	wxString, 
						wxWindow*, 
						TextSourceCollectionProducer *,
						ApplicationCommunicator *);
		virtual ~FindAndReplaceUserInterface();


		virtual void OnFindNext (wxCommandEvent & event);
		virtual void OnFindPrev (wxCommandEvent & event);
		
		virtual void SetNewSearch (void);
		virtual void OpenDialog (const std::string & selectedText);
		virtual void CloseDialog (void);



	protected:			
		///////////////////////////////////////////////
		//	Protected method API
		static bool				newSearch;
		static bool				regExprIsActive;
		static LookIn			lookIn;
		static wxPoint			mousePosotion;

		enum{
			ID_COMBOBOX_LOOKIN				= 0,
			ID_COMBOBOX_FIND_WHAT			= 1,
			ID_CHECKBOX_SEARCH_UP			= 2,
			ID_CHECKBOX_MATCH_CASE			= 3,
			ID_CHECKBOX_MATCH_WHOLE_WORD	= 4,
			ID_REGEX_BUTTON_FIND			= 5,
			ID_REGEX_MENU_OR				= 6,
			ID_REGEX_MENU_TAG				= 7,
			ID_REGEX_MENU_ANY_CHAR			= 8,
			ID_REGEX_MENU_ZERO_OR_MORE		= 9,
			ID_REGEX_MENU_SPACE_OR_BAR		= 10,
			ID_REGEX_MENU_DELTA_IDENTIFIER	= 11,
			ID_REGEX_MENU_QUOTED_STRING		= 12,
			ID_REGEX_MENU_ESC_SPECIAL_CHAR	= 13,
			ID_REGEX_MENU_INTEGER			= 14,
			ID_REGEX_MENU_ONE_OR_MORE		= 15,
			ID_FIND_NEXT					= 16,
			ID_FIND_PREV					= 17,
		};
		

		const char *		whereToLook[5];
		
		wxMenu *			regExprFindMenu;
		wxWindow *			regExprFindMenuWindow;
		wxButtonBase *		regExprFindButton;

		wxPanel *			mainPanel;
		wxButton *			buttonCancel;
		wxButton *			buttonFindNext;

		wxComboBox *		comboxFindWhat;
		wxComboBox *		comboxLookIn;

		wxStaticBox *		staticBox;
		wxCheckBox *		matchCase;
		wxCheckBox *		matchWholeWord;
		wxCheckBox *		searchUp;
		wxCheckBox *		regex;

		wxStaticText *		staticTextFindWhat;
		wxStaticText *		staticTextLookIn;

		
		wxBoxSizer *		lookInSizer;
		wxBoxSizer *		findTextSizer;
		wxBoxSizer *		controlFindTextSizer;

		ApplicationCommunicator *		communicator;
		TextSourceCollectionProducer *	producer;


		///////////////////////////////////////////////
		//	Protected method API
		virtual bool		 IsNewSearch(void);
		virtual void		 EnableAllRegExprButtons(const bool) = 0;
		virtual void		 UpdateAllRegExprCheckBoxs(const bool);
		virtual wxBoxSizer * AddLookInSizer(void);
		virtual wxBoxSizer * AddFindTextSizer(void);
		
		virtual void DoSearch (bool);
		virtual void OnExit(wxCloseEvent& event);
		virtual void OnCancel(wxCommandEvent& event);
		virtual void OnLookIn (wxCommandEvent& event);
		virtual void OnMouseMove (wxMouseEvent & event);
		virtual void OnTextCtrlFindWhat (wxCommandEvent& event);
		
		virtual void OnCheckBoxRegex (wxCommandEvent& event) = 0;
		virtual void OnCheckBoxSearchUp (wxCommandEvent& event);
		virtual void OnCheckBoxMatchCase (wxCommandEvent& event);
		virtual void OnCheckBoxMatchWholeWord (wxCommandEvent& event);
		
		virtual void OnButtonFindNext (wxCommandEvent& event) = 0;
		virtual void OnButtonRegexFindMenu (wxCommandEvent& event);

		virtual void OnOr (wxCommandEvent & event);
		virtual void OnTag (wxCommandEvent & event);
		virtual void OnInteger (wxCommandEvent & event);
		virtual void OnAnyChar (wxCommandEvent & event);
		virtual void OnOneOrMore (wxCommandEvent & event);
		virtual void OnDelteIden (wxCommandEvent & event);
		virtual void OnZeroOrMore (wxCommandEvent & event);
		virtual void OnSpaceOrBar (wxCommandEvent & event);
		virtual void OnQuotedString(wxCommandEvent & event);
		virtual void OnEsqSpecialCharacter(wxCommandEvent & event);

	private:
		friend class UserInterfaceUpdater;
		///////////////////////////////////////////////
		//	Private method API
		void		Clear(void);
		LookIn		WhereToLook(const std::string & str);
		wxMenu *	CreateRegExprFindMenu (void);

		// This class handles events
		DECLARE_EVENT_TABLE()

	};






	class UserInterfaceUpdater {
	public:
		static void						SingletonCreate(void);
		static bool						SingletonCreated(void);
		static void						SingletonDestroy(void);
		static UserInterfaceUpdater &	SingletonInstance(void);

		static void		RegisterUserInterface(FindAndReplaceUserInterface *);
		static void		SetCheckBoxMatchCaseValue(const bool);
		static void		SetCheckBoxWholeWordValue(const bool);
		static void		SetCheckBoxSearchUpValue(const bool);
		static void		SetCheckBoxRegExprValue(const bool);
		static void		SetComboxFindWhatValue(const wxString &);
		static void		SetComboxLookInValue(const wxString &);

	protected:
		UserInterfaceUpdater(void);
		~UserInterfaceUpdater(void);

		static bool						singletonCreated;
		static UserInterfaceUpdater *	singleton;

	private:
		friend class FindAndReplaceUserInterface;	
		static std::list<FindAndReplaceUserInterface *> registry;
		
	};
}	//namespace frep


#endif //USER_INTERFACE_H
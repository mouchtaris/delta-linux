/*
 * FindAndReplace.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */



#ifndef FINDANDREPLACE_H
#define FINDANDREPLACE_H


#include "Common.h"
#include "FindDialog.h"
#include "ReplaceDialog.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentBuilder.h"
#include "SparrowEditorTextSource.h"
#include "SparrowApplicationCommunicator.h"
#include "SparrowTextSourceCollectionProducer.h"


namespace ide {

	class FindAndReplace : public Component {
	private:
		//********************************************************
		//		private variables
		static frep::FindDialog *						findDialog;
		static frep::ReplaceDialog *					replaceDialog;
		static SparrowApplicationCommunicator *			communi;
		static SparrowTextSourceCollectionProducer *	producer;
		static bool rightDeletion;		//Exoume replace diadikasia. Kai diagrayame to koimeno
		static bool findState;
		static bool replaceState;


		//****************************************************
		//		private functions
		
		static const std::string GetFocusedEditorURI (void);
		static Component* GetFocusedEditor (void);
		static void ToggleDialog(
			frep::FindAndReplaceUserInterface*	active,
			frep::FindAndReplaceUserInterface*	inactive,
			bool*								activeFlag,
			bool*								inactiveFlag
		);

		DECLARE_IDE_COMPONENT(Component);

	public:
		FindAndReplace(void);
		~FindAndReplace(void);

		
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		
		//retType, funcName, argList
		DECLARE_EXPORTED_STATIC_(void, Find, (void),_("Find text"));
		DECLARE_EXPORTED_STATIC_(void, FindNext, (void),_("Find next text"));
		DECLARE_EXPORTED_STATIC_(void, FindPrev, (void),_("Find prev text"));
		DECLARE_EXPORTED_STATIC_(void, Replace, (void),_("Replace text"));


		///--- exported SLOTS
		DECLARE_EXPORTED_STATIC_(void, OnEditTextChanged, 
								(const std::string& classId, int pos, int length, bool added), 
								_(""));
		
		DECLARE_EXPORTED_STATIC_(void, OnEditSelectionChanged, 
								(const std::string& classId, int startPos, int endPos), 
								_(""));

		DECLARE_EXPORTED_STATIC_(void, OnFocusedEditorChanged,
								(const std::string& invokerId, const Handle& focusedEditor),
								_(""));
		DECLARE_EXPORTED_STATIC_(void, OnAllEditorsClosed, (const std::string& invokerId), _(""));		
	};

}	//namespace ide

#endif //FINDANDREPLACE_H
/*
 * FindAndReplace.cpp
 * Component
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include <assert.h>

#include "Call.h"
#include "ComponentFunctionCallerSafe.h"
#include "Streams.h"
#include "FindAndReplaceUserInterface.h"
#include "PropertyUtils.h"
#include "FindAndReplace.h"
#include "ComponentRegistry.h"

#include "FindAndReplaceManager.h"

#include "Icons/find.xpm"
#include "Icons/replace.xpm"

namespace ide {
	bool									FindAndReplace::findState;
	bool									FindAndReplace::replaceState;
	bool									FindAndReplace::rightDeletion;
	frep::FindDialog *						FindAndReplace::findDialog;
	frep::ReplaceDialog *					FindAndReplace::replaceDialog;
	SparrowApplicationCommunicator *		FindAndReplace::communi;
	SparrowTextSourceCollectionProducer *	FindAndReplace::producer;


	//-----------------------------------------------------------------------
	
	COMPONENT_METADATA(
		FindAndReplace,
		_("FindAndReplace"),
		_("Find and replace component"),
		_T("Koutsopoulos Nikolaos <koutsop@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_IDE_COMPONENT(FindAndReplace);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(FindAndReplace, table)
	{}

	//-----------------------------------------------------------------------

	FindAndReplace::FindAndReplace(void){}
	FindAndReplace::~FindAndReplace(){}

	//-----------------------------------------------------------------------

	Component* FindAndReplace::GetFocusedEditor (void)
		{ return ComponentRegistry::Instance().GetComponentEntry("Editor").GetFocusedInstance(); }

	//-----------------------------------------------------------------------

	const std::string FindAndReplace::GetFocusedEditorURI(void) {
		Component* editor = GetFocusedEditor();
		assert(editor);
		return util::str2std(Call<const String& (void)>(s_classId, editor, "GetURI")());
	}

	//-----------------------------------------------------------------------
	
	void FindAndReplace::ToggleDialog(
		frep::FindAndReplaceUserInterface*	active,
		frep::FindAndReplaceUserInterface*	inactive,
		bool*								activeFlag,
		bool*								inactiveFlag
	)
	{
		*activeFlag = true;
		*inactiveFlag = false;
		if (inactive->IsShown())
			inactive->CloseDialog();
		if (!active->IsShown()) {
			std::string text;
			if (Component* editor = GetFocusedEditor())
				text = util::str2std(Call<const String (void)>(s_classId, editor, "GetSelectedText")());
			active->OpenDialog(text);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(FindAndReplace, void, Initialize, (void))
	{
		findState		= false;
		replaceState	= false;
		rightDeletion	= false;
		communi			= new SparrowApplicationCommunicator();
		producer		= new SparrowTextSourceCollectionProducer();
		findDialog		= new frep::FindDialog(wxGetActiveWindow(), producer, communi);
		replaceDialog	= new frep::ReplaceDialog(wxGetActiveWindow(), producer, communi);
		
		frep::UserInterfaceUpdater::SingletonCreate();
		frep::UserInterfaceUpdater::RegisterUserInterface(findDialog);
		frep::UserInterfaceUpdater::RegisterUserInterface(replaceDialog);

		DBGOUT << "============================== FindAndReplace::Initialize();" << DBGENDL;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(FindAndReplace, void, CleanUp, (void))
	{
		assert(communi && producer);
		delete communi;
		delete producer;
		
		//findDialog->Destroy();		//ta antikeimena katastrefontai apo to idio to main frame
		//replaceDialog->Destroy();		//otan kleinoume to applications mas. Den prepei na ta katastrefw egw
		
		frep::UserInterfaceUpdater::SingletonDestroy();
		DBGOUT << "============================== FindAndReplace::CleanUp();" << DBGENDL;
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(FindAndReplace, "find", find_xpm);	
	EXPORTED_CMD_STATIC(FindAndReplace, Find, _("/{1}Edit/{110}--Find and Replace--/Find\tCtrl+F"), MT_MAIN, "find")
	{	
		ToggleDialog(findDialog, replaceDialog, &findState, &replaceState);
	}

	//-----------------------------------------------------------------------
	
	EXPORTED_IMAGE(FindAndReplace, "replace", replace_xpm);	
	EXPORTED_CMD_STATIC(FindAndReplace, Replace, _("/{1}Edit/{110}Find and Replace/Replace\tCtrl+H"), MT_MAIN, "replace")
	{
		ToggleDialog(replaceDialog, findDialog, &replaceState, &findState);
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(FindAndReplace, FindNext, _("/{1}Edit/{110}Find and Replace/Find Next\tF3"), MT_MAIN, "")
	{
		if ( findState )
			findDialog->OnFindNext(wxCommandEvent());
		else 
		if ( replaceState )
			replaceDialog->OnFindNext(wxCommandEvent());
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(FindAndReplace, FindPrev, _("/{1}Edit/{110}Find and Replace/Find Prev\tShift+F3"), MT_MAIN, "")
	{
		if ( findState )
			findDialog->OnFindPrev(wxCommandEvent());
		else 
		if ( replaceState )
			replaceDialog->OnFindPrev(wxCommandEvent());
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(FindAndReplace, void, OnEditTextChanged,
		(const std::string& classId, int pos, int length, bool added), "EditTextChanged")
	{
		const std::string uri = GetFocusedEditorURI();

		if ( frep::FindAndReplaceManager::BlockExists(uri) && findState) 
			findDialog->SetNewSearch();
		else 
		if (replaceState) {
			if (!added)
				if (frep::FindAndReplaceManager::IsRightDeletion(uri, pos, length-1))
					rightDeletion = true;	//Diagrafh khmenou apo to replace
				else 
					replaceDialog->SetNewSearch();
			else
				if (rightDeletion)			//Eisagwgh koimenou apo to replace
					rightDeletion = false;
				else
					replaceDialog->SetNewSearch();
		}
	}

	//-----------------------------------------------------------------------
	
	EXPORTED_SLOT_STATIC(FindAndReplace, void, OnEditSelectionChanged,
		(const std::string& classId, int startPos, int endPos), "EditSelectionChanged")
	{
		const std::string uri = GetFocusedEditorURI();

		if (replaceDialog->IsShown()){
			frep::TextSource::Block * block;
			
			if (block = frep::FindAndReplaceManager::BlockExists(uri, startPos, endPos-1)){
				frep::FindAndReplaceManager::SetCurrentBlock(*block);
				frep::FindAndReplaceManager::SetSelectedBlock(*block);
				replaceDialog->SetButtonReplaceEnable(true);
				replaceDialog->SetButtonReplaceAllEnable(true);
			}
			else{
				replaceDialog->SetButtonReplaceEnable(false);
				replaceDialog->SetButtonReplaceAllEnable(false);
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(FindAndReplace, void, OnFocusedEditorChanged, 
		(const std::string& invokerId, const Handle& focusedEditor), "FocusedEditorChanged")
	{
		if (communi && focusedEditor && !focusedEditor.IsDangling()) {
			communi->ChangeEditor(true);
			std::string path = util::str2std(Call<const String& (void)>(s_classId, focusedEditor.Resolve(), "GetURI")());
			communi->SetEditorPath(path);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(FindAndReplace, void, OnAllEditorsClosed, (const std::string& invokerId), "AllEditorsClosed")
	{
		if (communi)
			communi->ChangeEditor(true);
	}

} // namespace ide
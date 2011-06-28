/*
 * SparrowTextSourceCollectionProducer.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include <assert.h>
#include <algorithm>

#include "Call.h"
#include "Common.h"
#include "Streams.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentRegistry.h"
#include "SparrowFileTextSource.h"
#include "SparrowEditorTextSource.h"
#include "SparrowSelectionTextSource.h"
#include "SparrowTextSourceCollectionProducer.h"



#define CHECK_FOR_PROJECT_MANAGER()	if ( !ComponentRegistry::Instance().GetFocusedInstance("ProjectManager") ) { return; }


namespace ide {
	
	//******************************************************************************************
	//		private functions

	//-----------------------------------------------------------------------
	
	void SparrowTextSourceCollectionProducer::Clear (void) {
		std::for_each(tsList.begin(), tsList.end(), ClearTSListFunctor());
		tsList.clear();
		return;
	}

	//-----------------------------------------------------------------------
	
	bool SparrowTextSourceCollectionProducer::FindIntoTSList (const std::string & path) const {
		for (TSList::const_iterator i = tsList.begin(); i != tsList.end(); i++) 
			if (!(*i)->GetPath().compare(path))
				return true;
		
		return false;
	}
	
	//-----------------------------------------------------------------------
	
	frep::TextSource * SparrowTextSourceCollectionProducer::SelectFileType ( Component* com, 
																			 const std::string uri) 
	{
		assert(com);
		if ( !com->GetName().compare(wxT("Script"))			|| 
			 !com->GetName().compare(wxT("Generic File"))	||
			 !com->GetName().compare(wxT("Text File"))
			) 
		{
			return (new SparrowFileTextSource(uri));
		}
		else {
			assert(!com->GetName().compare(wxT("Editor")));
			if(findOnSelection)
				return (new SparrowSelectionTextSource(com, uri));
			else
				return (new SparrowEditorTextSource(com, uri) );
		}
	}

	//-----------------------------------------------------------------------
	
	HandleList SparrowTextSourceCollectionProducer::GetAllProjects (void) const {
		assert(ComponentRegistry::Instance().GetFocusedInstance("ProjectManager"));

		const Handle& workspace =	Call<const Handle& (void)>
									("FindAndReplace", "ProjectManager", "GetWorkspace")
									();

		HandleList projects		=	Call<HandleList (const String&)>
									("FindAndReplace", workspace, "CollectChildren")
									(util::std2str("Project"));
		return projects;
	}

	//-----------------------------------------------------------------------
	
	Handle SparrowTextSourceCollectionProducer::GetFocusedProject (const HandleList & projects) const {
		HandleList::const_iterator i;
		for (i = projects.begin(); i != projects.end(); ++i)
			if ( (*i)->IsFocused() )
				break;		

		assert(i != projects.end() );	//an den bre8ike focused project tote kati paei polu straba
		return (*i);		
	}

	//-----------------------------------------------------------------------
	
	void SparrowTextSourceCollectionProducer::AddScriptsToTSListFromProject (const Handle project) {
		
		HandleList scripts =	Call<HandleList (const String&)>
								("FindAndReplace", project, "CollectChildren")
								(util::std2str("Script"));

		HandleList genericf =	Call<HandleList (const String&)>
								("FindAndReplace", project, "CollectChildren")
								(util::std2str("GenericFile"));

		HandleList textfile =	Call<HandleList (const String&)>
								("FindAndReplace", project, "CollectChildren")
								(util::std2str("TextFile"));

		scripts.splice(scripts.end(), genericf);
		scripts.splice(scripts.end(), textfile);


		for (HandleList::iterator scr = scripts.begin(); scr != scripts.end(); ++scr) {
			std::string uri = util::str2std( Call<const String& (void)>("FindAndReplace", (*scr), "GetURI")() );
			if (!FindIntoTSList(uri))	//maby one script is alrady inside the tsList, if is an open editor.
				tsList.push_back(SelectFileType((*scr).Resolve(), uri));
		}
		return;
	}

	//-----------------------------------------------------------------------

	void SparrowTextSourceCollectionProducer::CurrentProject (void) {
		CHECK_FOR_PROJECT_MANAGER();

		Handle project = GetFocusedProject(GetAllProjects());
		AllOpenDocuments();	//We takes all the open Editors
		AddScriptsToTSListFromProject( project );
		return;
	}

	//-----------------------------------------------------------------------

	void SparrowTextSourceCollectionProducer::EntireWorkSpace (void) {
		CHECK_FOR_PROJECT_MANAGER();

		HandleList projects = GetAllProjects();

		AllOpenDocuments();	//We takes all the open Editors
		for (HandleList::iterator pro = projects.begin(); pro != projects.end(); ++pro){
			AddScriptsToTSListFromProject((*pro));
		}//end of for

		return;
	}

	//-----------------------------------------------------------------------
	
	void SparrowTextSourceCollectionProducer::AllOpenDocuments (void){
		//pernoume prwta ton focus editor oste na isxiei h sun8ilh pou 8elei sthn arxh ths lista na einai
		//o focus editor
		CurrentDocument(false);
		
		HandleVec vec = Call<const HandleVec& (void)>
						("FindAndReplace", "EditorManager", "GetEditors")
						();
		for (HandleVec::iterator i = vec.begin(); i != vec.end(); ++i ){
			std::string uri = util::str2std( Call<const String& (void)>("FindAndReplace", (*i), "GetURI")() );

			if ( !FindIntoTSList(uri) )
				tsList.push_back(SelectFileType((*i).Resolve(), uri));
		}
		return;
	}
	
	//-----------------------------------------------------------------------
	
	void SparrowTextSourceCollectionProducer::CurrentDocument (const bool _findOnSelection){
		findOnSelection = _findOnSelection;
		const Handle& editor =	Call<const Handle& (void)>
								("FindAndReplace", "EditorManager", "GetFocusedEditor")
								();
		
		std::string uri = util::str2std( Call<const String& (void)>("FindAndReplace", editor, "GetURI")() );
		tsList.push_back(SelectFileType(editor.Resolve(), uri));
		
 		return;		
	}



	//******************************************************************************************
	//		public functions

	//-----------------------------------------------------------------------
	std::list<frep::TextSource *> & SparrowTextSourceCollectionProducer::GetTextSourceList (
										const frep::LookIn lookIn, 
										const bool _findInScript
									) {
		Clear();
		findInScript = _findInScript;
		switch (lookIn){
			case frep::CURRENT_PROJECT:		CurrentProject();		break;
			case frep::SELECTION:			CurrentDocument(true);	break;
			case frep::CURRENT_DOCUMENT:	CurrentDocument(false);	break;
			case frep::ENTIRE_WORKSPACE:	EntireWorkSpace();		break;
			case frep::ALL_OPEN_DOCUMENTS:	AllOpenDocuments(); 	break;
			default: assert(0);
		}
		return tsList;
	}

} //namespace ide

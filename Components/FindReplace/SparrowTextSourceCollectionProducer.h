/*
 * SparrowTextSourceCollectionProducer.h 
 * inherit from TextSourceCollectionProducer
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef IDE_SPARROWTEXTSOURCECOLLECTIONPRODUCER_H
#define IDE_SPARROWTEXTSOURCECOLLECTIONPRODUCER_H


#include <list>
#include <functional>
#include "TextSource.h"
#include "TextSourceCollectionProducer.h"


namespace ide {

	class SparrowTextSourceCollectionProducer : public frep::TextSourceCollectionProducer {
	private:
		//********************************************************
		//		private typedefs
		typedef std::list<frep::TextSource *> TSList;



		//********************************************************
		//		private variables
		TSList	tsList;				//TextSourceList;
		bool	findInScript;
		bool	findOnSelection;



		//****************************************************
		//		private functions


		void Clear (void);
		bool FindIntoTSList (const std::string &) const;
		
		void CurrentDocument(const bool);
		void CurrentProject(void);
		void EntireWorkSpace(void);
		void AllOpenDocuments(void);

		//prepei na exei ginei elegxos gia uparxei ProjectManager
		
		HandleList			GetAllProjects (void) const;
		Handle				GetFocusedProject (const HandleList &) const;
		frep::TextSource *	SelectFileType (Component* , const std::string);
		void AddScriptsToTSListFromProject (const Handle);



		//****************************************************
		//		private functors
		struct ClearTSListFunctor : public std::unary_function<frep::TextSource *, void> {
			void operator() (const frep::TextSource * t) const{
				delete t;
				t = (frep::TextSource *)0;
				return;
			}
		};
		

	public:
		virtual std::list<frep::TextSource *> & GetTextSourceList (const frep::LookIn, const bool);
		
		SparrowTextSourceCollectionProducer(void) : findInScript(false) {}
		~SparrowTextSourceCollectionProducer(void) { Clear(); }
	};

} //namespace ide

#endif	//IDE_SPARROWTEXTSOURCECOLLECTIONPRODUCER_H

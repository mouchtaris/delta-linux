/*
 * FindAndReplaceManager.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef FIND_AND_REPLACE_MANAGER_H
#define FIND_AND_REPLACE_MANAGER_H

#include <list>
#include <vector>
#include <string>
#include <stdlib.h>
#include <functional>

 
#include "TextSource.h"
#include "DFAMatching.h"
#include "StringMatching.h"
#include "RegExStringReplace.h"
#include "BasicStringMatching.h"

#pragma warning( disable: 4267 )

namespace frep {

	class FindAndReplaceManager {
	private:
		//********************************************************
		//		private typedefs
		typedef TextSource::Block				Block;
		typedef std::list<TextSource *>			Files;
		typedef std::vector<TextSource::Block>	Results;


		//********************************************************
		//		variables
		static bool		hasError;
		static char *	errorMsg;
		static Files	stream;

		//Find - Replace param
		static bool				regEx;
		static bool				prevIsSearchUp;
		//static unsigned			currFind;	//Kuklikh anazhthsh
		static std::string		what;
		static std::string		with;
		static std::string		what2;		//auta ta duo ta exome wste na karatame analioth
		static std::string		with2;		//ta arxika str pou pirame (auto boh8aei sta regexpr)
		static Block			deletedBlock;	//to block pou diagrayame;
		static Block			selectedBlock;	//to block pou einai selected
		static Block			currBlock;		// to curr block pou tis anazithshs
		
		//
		static RegExStringReplace	replaceStringCreator;
		static DFAMatching			dfaMatching;
		static BasicStringMatching	strMatching;


		//Results 
		static Results results;



		//****************************************************
		//		private functions
		static void Clear (void);
		static void SetFindSettings (void);
		static void SetError (const char *);
		static bool DoSeartch (StringMatching &);
		static bool RunAlgorithm (StringMatching &, TextSource *);
		static void UpdateResultsPosition (const Block &);
		static bool CreateReplacmentString (const Block &);
		static Block &	GetNext(void);
		static Block &	GetPrev(void);
		

	public:

		static struct FindSettings{
			bool	isCaseSensitive;
			bool	searchUp;
			bool	wholeWord;
		}settings;


		static void Initialise (void);
		static void CleanUp (void);


		//Ta arxeia sthn lista einai me thn shra pou 8a ginete h anazhthsh
		static void		Set(std::list<TextSource *> & _stream)	{ 
			stream.clear(); 
			stream = _stream; 
		}
		static bool		Find (void);
		static bool		Replace (void);
		static bool		ReplaceAll (void);
		static char *	GetError (void) { return errorMsg; }
		static bool		HasError (void) { return hasError; }
		static void		BookmarkAll (void);
		static void		SetFindParam (const std::string, const bool);
		static void		SetReplaceParam (
							const std::string & what,
							const std::string & with, 
							const bool hasRegExpr
						);
		static bool		HasResults (void);
		static bool		IsRightDeletion (const std::string &, const int, const int);
		static bool		IsRightSelection (const std::string &, const int, const int);
		
		
		static Block *	BlockExists (const std::string &);
		static Block *	BlockExists (const std::string &, const int, const int);
		
		static void		SetSelectedBlock ( const Block & b) { selectedBlock = b; }
		static void		SetCurrentBlock (const Block & b) { currBlock = b; } 

		static Block &	FindNext (void);
		static void		SelectText(const Block &);

		static const std::string FocusedEditorPath(void) { return stream.empty() ? "" : stream.front()->GetPath();}
		static const std::string WriteResultsToOutput (void);

		//Delete after testing
		static void	WriteResults( Block& r, FILE *fp);
		static void	DumpResults(FILE *fp);
	};

} // namespace frep 

#endif	//FIND_AND_REPLACE_MANAGER_H


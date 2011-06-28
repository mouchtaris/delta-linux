/*
 * DFAMatching.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef DFAMATCHING_H
#define DFAMATCHING_H


#include "DFA.h"
#include "DFARunner.h"
#include "TextSource.h"
#include "RDParserFind.h"
#include "StringMatching.h"
//#include "LexicalAnalyser.h"
#include "LexicalAnalyserFind.h"

namespace frep {

	//class FindAndReplaceManager;	//forward declaration

	class DFAMatching : public StringMatching {
	private:
		//********************************************************
		//		variables
		DFA *				dfa;
		DFARunner			runner;
		std::string			inputStr;		//Krataei to string pou pirame ka8e fora kai blepei gia alages
		RDParserFind 		parserFind;
		LexicalAnalyserFind	analyser;
		
		
		//****************************************************
		//		private functions
		bool			IsNewAutomaton (const std::string str) const;
		void			CreateAutomaton (RegExpr * expr);
		RegExpr *		CreateRegularExpression (void);
		virtual void	SetError(const std::string);

	public:


		virtual void Clear (void);

		//isCaseSensitive, wholeWord
		virtual void SetFindSetting (bool, bool);

		virtual bool Run (const std::string, TextSource&, TextSource::Block &);
		virtual bool HasError (void) const { return hasError; }
		virtual const std::string GetError (void) const { return errorMsg; }

		DFAMatching(void);
		virtual ~DFAMatching(void);
	};


}	// namespace frep

#endif

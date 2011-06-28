/*
 * RegExStringReplace.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef REGEXSTRINGREPLACE_H
#define REGEXSTRINGREPLACE_H

#include <string>

#include "RDParserReplace.h"
#include "LexicalAnalyserReplace.h"

namespace frep {
	
	class RegExStringReplace {
	private:
		//********************************************************
		//		private variables
		bool					hasError;
		std::string				errorMsg;
		std::string				replaceString;
		RDParserReplace 		parserReplace;
		LexicalAnalyserReplace	analyser;
		


		//****************************************************
		//		private functions
		void Clear (void);
		void SetError (const std::string);
		
	

	public:		
		const std::string GetReplaceString (void) const;
		const std::string GetError (void) const;

		bool CreateReplacmentString (const std::string &);
		bool HasError (void) const;
		

		RegExStringReplace(void) {}
		~RegExStringReplace(void) { Clear(); }
	};

}	//namespace frep

#endif //REGEXSTRINGREPLACE_H

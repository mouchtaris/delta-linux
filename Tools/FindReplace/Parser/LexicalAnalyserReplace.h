/*
 * LexicalAnalyserReplace.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef LEXICAL_ANALYSER_REPLACE_H
#define LEXICAL_ANALYSER_REPLACE_H

#include <stdio.h>

#include <list>
#include <string>
#include <iostream>
#include <functional>
#include "LexicalAnalyser.h"


namespace frep {
	class LexicalAnalyserReplace : public LexicalAnalyser {
	private:
		bool		hasError;
		char *		errorMsg;
		size_t		totalLookAhead;
		TokenList*	tokenList;
		size_t		ptrInputStr;		//Mas dixnei enan enan tous xaraktires apo to str
		std::string	inputStr;


		void SetError(char *msg);

		//**********************
		void Unput (void);
		char Input (void);
		bool IsSTRING (char** val);
		bool IsINT (unsigned int* val);
		bool IsSingleCharToken (char token);
		
			


	public:
		virtual char *		GetError(void) { return errorMsg; }
		virtual bool		HasError(void) { return hasError; }
		virtual bool		Analyse (std::string & str);
		virtual TokenList*	GetTokens (void) const { return tokenList; }
		virtual void		DumpTokens (FILE *fp);
		virtual void		Clear(void);

		LexicalAnalyserReplace(void);
		~LexicalAnalyserReplace(void);
	};//end of class LexicalAnalyserReplace

} //namespace frep {


#endif
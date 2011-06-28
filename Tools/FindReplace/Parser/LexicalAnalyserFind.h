/*
 * LexicalAnalyserFind.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef LEXICAL_ANALYSER_FIND_H
#define LEXICAL_ANALYSER_FIND_H

#include <stdio.h>

#include <list>
#include <string>
#include <iostream>
#include <functional>
#include "LexicalAnalyser.h"



namespace frep {
	
	//-----------------------------------------------------------------------
	class LexicalAnalyserFind : public LexicalAnalyser {
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
		void IgnoreWhiteSpace (void);
		bool IsSingleCharToken (char token);
		bool IsINT (unsigned int* val);
		bool IsSTRING (char** val);
		bool IsWhiteSpace (void);
		bool IsDeltaId (void);
		


	public:
		virtual char *		GetError(void) { return errorMsg; }
		virtual bool		HasError(void) { return hasError; }
		virtual bool		Analyse (std::string & str);
		virtual TokenList*	GetTokens (void) const { return tokenList; }
		virtual void		DumpTokens (FILE *fp);
		virtual void		Clear(void);

		LexicalAnalyserFind(void);
		~LexicalAnalyserFind(void);
	};//end of class LexicalAnalyserFind

} //namespace frep {


#endif
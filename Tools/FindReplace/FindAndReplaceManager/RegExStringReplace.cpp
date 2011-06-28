/*
 * RegExStringReplace.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include <assert.h>
#include "RegExStringReplace.h"



#define FAIL(X)		SetError(X); return false


namespace frep {

	//******************************************************************************************
	//		private functions

	//-----------------------------------------------------------------------
	void RegExStringReplace::Clear (void) {
		analyser.Clear();
		parserReplace.Clear();

		errorMsg = "";
		hasError = false;
		return;
	}



	//-----------------------------------------------------------------------
	void RegExStringReplace::SetError (const std::string msg) {
		hasError = true;
		errorMsg = msg;
		return;
	}
	
	

	//******************************************************************************************
	//		public functions

	//-----------------------------------------------------------------------
	const std::string RegExStringReplace::GetReplaceString (void) const {
		assert(!HasError());
		return replaceString;
	}



	//-----------------------------------------------------------------------
	const std::string RegExStringReplace::GetError (void) const {
		assert(!HasError());
		return errorMsg;
	}


	
	//-----------------------------------------------------------------------
	bool RegExStringReplace::CreateReplacmentString (const std::string & inputStr) {
		RegExpr * e = (RegExpr *)0;
		Clear();

		if( analyser.Analyse(const_cast<std::string &>(inputStr)) ){ 
			if ( !(e = parserReplace.Analyse( analyser.GetTokens() )) ){
				if( parserReplace.HasError() ) { FAIL(parserReplace.GetError()); }
			}
		}//first if
		else { FAIL(analyser.GetError()); }
		
		replaceString = e->Lexeme();
		return true;	
	}



	//-----------------------------------------------------------------------
	bool RegExStringReplace::HasError (void) const {
		return hasError;
	}


}	//namespacefrep
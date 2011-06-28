/*
 * BasicStringMatching.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef BASICSTRINGMATCHING_H
#define BASICSTRINGMATCHING_H

#include "TextSource.h"
#include "StringMatching.h"

namespace frep {

	class BasicStringMatching : public StringMatching {
	private:
		virtual void SetError(const std::string);

	public:
		virtual void Clear (void);
		
		//isCaseSensitive, wholeWord
		virtual void SetFindSetting (bool, bool);

		
		virtual bool Run (const std::string, TextSource&, TextSource::Block &);
		

		//Den mporei na dimiourgi8ei error sto tr3imo;
		virtual bool HasError (void) const { return false; };
		
		//Epistrefoume panta ena keno string
		virtual const std::string GetError (void) const { return ""; };


		BasicStringMatching(void);
		virtual ~BasicStringMatching(void) {}
	};

} // namespace frep


#endif
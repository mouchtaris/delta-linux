/*
 * StringMatching.h
 * Abstract Class
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef STRINGMATCHING_H
#define STRINGMATCHING_H

#include <string>

#include "TextSource.h"

namespace frep {
	

	class StringMatching {
	protected:
		bool		hasError;
		std::string errorMsg;
		virtual void SetError (const std::string) = 0;
	
	public:
		virtual void Clear (void) = 0;
		//isCaseSensitive, wholeWord
		virtual void SetFindSetting (bool, bool) = 0;
		virtual bool Run (const std::string, TextSource&, TextSource::Block &) = 0;
		virtual bool HasError (void) const = 0;
		virtual const std::string GetError (void) const = 0;
		virtual ~StringMatching (void) {}
	};
} // namespace FindReaplce


#endif //STRINGMATCHING_H
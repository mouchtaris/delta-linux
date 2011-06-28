/**
 *	StringUtils.h
 *
 *	-- Utility Library --
 *	Contains utility functions for string management.
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	March 2010.
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H


#include <string>
#include <sstream>
#include <iostream>

namespace utility {
	//-----------------------------------------------------------------------
	template<class T>
	std::string string_cast(const T a){
		std::ostringstream os;
		os<<a;
		return os.str();
	}


	//-----------------------------------------------------------------------
	template<class T>
	std::string ToUpperCopy(const T & input){
		std::string str(string_cast(input));
		for(std::string::iterator i = str.begin(); i != str.end(); ++i)
			if (isalpha(*i)) { *i = toupper(*i); } 
		return str;
	}



	//-----------------------------------------------------------------------
	template<class T>
	std::string ToLowerCopy(const T & input){
		std::string str(string_cast(input));

		for(std::string::iterator i = str.begin(); i != str.end(); ++i)
			if (isalpha(*i)) { *i = tolower(*i); } 
		return str;
	}
}	//namespace unility


#endif //STRING_UTILS_H

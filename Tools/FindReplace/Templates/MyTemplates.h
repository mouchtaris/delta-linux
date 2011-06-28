/*
 * MyTemplates.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef MYTEMPLATES_H
#define MYTEMPLATES_H

#include <map>
#include <set>
#include <string>
#include <sstream>
#include <ctype.h>
#include <iostream>
#include <locale.h>



namespace frep {
	
	
	//-----------------------------------------------------------------------
	template<class T>
	std::string string_cast(const T a){
		std::ostringstream os;
		os<<a;
		return os.str();
	}



	//-----------------------------------------------------------------------
	//	Antigrafei ta periexomena tou prwtou sto deutero.
	template<class Key, class T>
	bool CopyMap (const std::map<Key, T> & first, std::map<Key, T> & second) {
		for ( std::map<Key, T>::const_iterator i = first.begin(); i != first.end(); ++i){
			second[i->first] = i->second;
		}
		return true;
	}



	//-----------------------------------------------------------------------
	//yaxnei na dei an uparxoun toulaxiston ena koino stixeio anamesa sta
	//duo set.
	template<class T>
	bool HasSimilar ( const std::set<T> & set1, const std::set<T> & set2 ){
		for( std::set<T>::const_iterator i = set1.begin() ; i != set1.end(); ++i){
			if ( set2.find( (*i) ) != set2.end() )
				return true;
		}
		return false;
	}


	//-----------------------------------------------------------------------
	//	Check if these two sets are equals
	template<class T>
	bool IsEqual(const std::set<T> & set1, const std::set<T> & set2) {
		if (set1.size() != set2.size())
			return false;

		for(std::set<T>::const_iterator i = set1.begin(); i != set1.end(); ++i)
			if ( set2.find(*i) == set2.end())
				return false;
		return true;
	}

	//-----------------------------------------------------------------------
	//	Check if the second set containing in first set
	template<class T>
	bool Contains (const std::set<T> & set1, const std::set<T> & set2) {
		for(std::set<T>::const_iterator i = set2.begin(); i != set2.end(); ++i)
			if ( set1.find(*i) == set1.end())
				return false;
		return true;	
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

} // namespace frep

#endif
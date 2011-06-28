/*
 * SparrowFileTextSource.h
 * inherit from TextSource
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef FILETEXTSOURCE_H
#define FILETEXTSOURCE_H

#include <map>
#include <vector>
#include <string>
#include "common.h"
#include "Component.h"
#include "TextSource.h"


namespace ide {

	class SparrowFileTextSource : public frep::TextSource {
	private:
		Component *			editor;
		Pos					currPos;	//current coordinates
		unsigned			indexVec;	
		unsigned			sizeOfVec;
		std::vector<char>				vec;		//The contents of file
		std::map<unsigned, Pos>			markPos;	//Marck coordinates if we call Marck()
		std::map<unsigned, std::string>	lineHolder;	//key == line Number, second line text

		void ClearAllContainers(void);		

	public:
		virtual bool 		Open (void);
		virtual bool 		Close (void);
		virtual bool 		IsEof (void) const;
		virtual bool 		Unput (void);
		virtual bool 		Unmark (unsigned);
		virtual void 		UnmarkAll (void);
		virtual bool 		Retract(unsigned);
		virtual char 		GetCurr (void);
		virtual char 		GetNext (void);
		virtual unsigned	Mark (void);
		virtual const char	GetSpecificChar(unsigned) const;
		

		virtual void				SetPath(const std::string & path_) { path = path_; };
		virtual const std::string	GetPath(void) const { return path; };
		virtual const std::string	GetLineText (const int) const;
		

		virtual const Pos	GetPos (void) const { return currPos; }
		virtual const Pos	GetCurrentFilePos (void);// { return currPos; }


		virtual void		Focus (void);
		virtual bool		IsFocused (void);
		virtual void		Bookmark(const unsigned row, const unsigned col);
		virtual void		Select(
								const int startRow, 
								const int startCol, 
								const int endRow, 
								const int endCol 
							);

		virtual std::string	Replace(
								const int startRow, 
								const int startCol, 
								const int endRow, 
								const int endCol, 
								const std::string with
							);


		SparrowFileTextSource(void);
		SparrowFileTextSource(const std::string &);
		~SparrowFileTextSource(void);
	};

}	//namespace ide

#endif

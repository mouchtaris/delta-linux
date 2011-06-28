/*
 * SparrowEditorTextSource.h
 * inherit from TextSource
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef IDE_SPARROWEDITORTEXTSOURCE_H
#define IDE_SPARROWEDITORTEXTSOURCE_H

#include <map>
#include "common.h"
#include "Component.h"
#include "TextSource.h"

namespace ide {
	class SparrowEditorTextSource : public frep::TextSource {
	protected:
		Component *		editor;
		std::string		text;
		unsigned		sizeOfText;
		unsigned		indexPos;

		Pos currPos;				//current coordinates
		Pos currFilePos;
		std::map<int, Pos> markPos;	//Marck coordinates if we call Marck()


	public:
		virtual bool	Open (void);
		virtual bool	Close (void);
		virtual bool	IsEof(void) const;
		virtual char	GetCurr (void);
		virtual char	GetNext (void);
		virtual bool	Unput(void);
		virtual bool	Unmark (unsigned);
		virtual void	UnmarkAll (void);
		virtual void	SetPath(const std::string & path_);
		virtual bool	IsFocused (void);
		virtual void	Focus (void);

		virtual unsigned	Mark (void);
		virtual bool		Retract(unsigned);
		virtual const Pos	GetPos (void) const;
		virtual const Pos	GetCurrentFilePos (void);
		
		//eof an den uparxei auto to index
		virtual const char			GetSpecificChar(unsigned) const;
		virtual const std::string	GetPath(void) const;
		virtual const std::string	GetLineText (const int) const;


		virtual void	Bookmark(const unsigned row, const unsigned col);
		virtual void	Select(const int, const int, const int, const int);
		
		virtual std::string	Replace(const int, const int, const int, const int, const std::string);

		//SparrowEditorTextSource(void);
		SparrowEditorTextSource(Component *, std::string);
		virtual ~SparrowEditorTextSource(void);

	};

} // namespace ide

#endif
/*
 * TextSource.h
 * Abstruct Class
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef TEXTSOURCE_H
#define TEXTSOURCE_H

#include <map>
#include <string>
#include <stdio.h>

namespace frep{

	class TextSource {
	public:
		struct Pos {
			unsigned row;
			unsigned col;

			bool operator == (const Pos & p) const {
				if ((row == p.row) && (col == p.col) )
					return true;
				else 
					return false;
			}
			
			Pos(void) : row(1), col(0) {}
			Pos(unsigned _row, unsigned _col) : row(_row), col(_col) {}
		};
		//********************************************************

		//Edw na rwtisw an eiai 
		struct Block {
			//key == tag numer, string == lexem of tag
			typedef	std::map<unsigned, std::string> BlockTagMap;
			
			Pos				start;
			Pos				end;
			bool			hasTags;
			BlockTagMap		tags;
			std::string		path;
			TextSource *	file;

			void Clear (void) {
				if( hasTags ){ tags.clear(); }
				hasTags = false;
				start	= Pos(1,0);
				end		= Pos(1,0);
				path.clear();
			}

			bool operator == (const Block & b) const {
				if ((start == b.start) && (end == b.end) && (this->path == b.path))
					return true;
				else
					return false;
			}

			Block(void) : hasTags(false) {}
			//Block(const Block & block) { memcpy(this, &block, sizeof(Block)); }
			~Block(void){ 
				Clear(); 
				file = static_cast<TextSource *>(0);
			}
		};
		//********************************************************

		virtual bool				Open (void) = 0;
		virtual bool				Close (void) = 0;
		virtual bool				IsEof(void) const = 0;
		virtual char				GetCurr (void) = 0;
		virtual char				GetNext (void) = 0;
		virtual bool				Unput(void) = 0;
		virtual bool				Unmark (unsigned marked) = 0;
		virtual void				UnmarkAll (void) = 0;
		virtual void				SetPath(const std::string & path_) = 0;
		virtual bool				IsFocused (void) = 0;
		virtual void				Focus (void) = 0;

		virtual unsigned			Mark (void) = 0;
		virtual bool				Retract(unsigned marker) = 0;
		virtual const Pos			GetPos (void) const = 0;
		virtual const Pos			GetCurrentFilePos (void) = 0;
		
		//eof an den uparxei auto to index
		virtual const char			GetSpecificChar(unsigned col) const = 0;
		virtual const std::string	GetPath(void) const = 0;
		virtual const std::string	GetLineText (const int line) const = 0;
		
		virtual void				Bookmark(const unsigned row, const unsigned col) = 0;
		virtual void				Select(
										const int startRow, 
										const int startCol, 
										const int endRow, 
										const int endCol 
									) = 0;

		virtual std::string			Replace(
										const int startRow, 
										const int startCol, 
										const int endRow, 
										const int endCol, 
										const std::string with
									) = 0;

		TextSource() : path("") {}
		virtual ~TextSource(){}

	protected:
		std::string path;
	};
} //frep


#endif //TEXTSOURCE_H
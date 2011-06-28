/*
 * FileTextSource.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@csd.uoc.gr	
 */

#ifndef FILETEXTSOURCE_H
#define FILETEXTSOURCE_H

#include <map>
#include <vector>
#include <string>
#include "TextSource.h"

//using namespace frep;


class FileTextSource : public frep::TextSource {
private:
	bool	 E_O_F;
	unsigned currPos;			//index vec
	Pos position;				//current coordinates
	std::string path;
	std::vector<char> vec;		//The contents of file
	std::map<int, Pos> markPos;	//Marck coordinates if we call Marck()
	

public:
	virtual bool Open (void);
	virtual bool Close (void);
	virtual bool IsEof (void) const;
	virtual bool Unput (void);
	virtual bool Unmark (unsigned);
	virtual void UnmarkAll (void);
	virtual bool Retract(unsigned);
	virtual char GetCurr (void);
	virtual char GetNext (void);
	virtual unsigned	Mark (void);
	virtual const char	GetSpecificChar(unsigned) const;
	virtual const Pos	GetPos (void) const { return position; }
	virtual const Pos	GetCurrentFilePos (void);
	virtual const std::string GetPath(void) const { return path; };
	virtual void SetPath(std::string _path) { path = _path; };
	

	virtual void	Bookmark(const unsigned row, const unsigned col);
	virtual void	Select(const int, const int, const int, const int);
	virtual std::string	Replace(const int, const int, const int, const int, const std::string);


	FileTextSource(void);
	~FileTextSource(void);
};

#endif

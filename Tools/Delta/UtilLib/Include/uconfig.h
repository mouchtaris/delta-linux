// uconfig.h
// Simple config mechanism with key value pairs.
// ScriptFighter Project.
// A. Savidis, July 2014.
//

#ifndef	UCONFIG_H
#define	UCONFIG_H

#include "utypes.h"
#include "ustrings.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <map>

///////////////////////////////////////////////////////////

class UTILLIB_CLASS uconfig {

	private:
	typedef				std::map<std::string, std::string> Values;
	Values				values;
	FILE*				fp;
	char				lookAhead;
	bool				eof;
	util_ui32			line, col;
	std::string			error;

	bool				Restart (const std::string& path);
	bool				ParseItem (std::string& s);
	void				SkipWhitespace (void);
	char				Input (void);
	void				SetError (const std::string& s);

	///////////////////////////////////////////////////////////

	public:
	bool				HasError (void) const
							{ return !error.empty(); }
	const std::string&	GetError (void) const
							{ return error; }

	// key or content is any sequence of non-space 
	// printable characters

	bool				GetBool (const std::string& id) const;
	util_i32			GetInt (const std::string& id) const;
	float				GetFloat (const std::string& id) const;
	const std::string&	GetString (const std::string& id) const;

	bool				Parse (const std::string& path);

	uconfig (void) : 
		fp			((FILE*) 0), 
		lookAhead	(0), 
		eof			(false),
		line		(0),
		col			(0)
		{}
};

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
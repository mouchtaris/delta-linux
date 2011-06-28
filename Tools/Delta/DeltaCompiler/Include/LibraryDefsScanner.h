// LibraryDefsScanner.h
// Scanner for library deifnitions.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#ifndef	LIBRARYDEFSSCANNER_H
#define	LIBRARYDEFSSCANNER_H

#include "DDebug.h"
#include "utypes.h"
#include "ucallbacks.h"
#include "TypeTag.h"
#include <string>
#include <list>
#include <algorithm>

///////////////////////////////////////////////////////////

class DeltaLibraryDefsErrorReporting {
		
	protected:
	bool		hasError;
	std::string error;

	void				Error (const std::string& msg) 
							{ if (!hasError) { error = msg; hasError = true; } }

	public:
	bool				HasError (void) const { return hasError; }
	const std::string&	GetError (void) const { return error; }

	DeltaLibraryDefsErrorReporting (void) : hasError(false){}
	virtual ~DeltaLibraryDefsErrorReporting(){}
};

///////////////////////////////////////////////////////////

class DeltaLibraryDefsScanner : public DeltaLibraryDefsErrorReporting {
	public:
	enum Token { 
		TokenID			= 0, 
		TokenCOLON		= 1, 
		TokenLP			= 2 , 
		TokenRP			= 3, 
		TokenEOF		= 4, 
		TokenWARN		= 5,
		TokenCOMMA		= 6,
		TokenDOTS		= 7,
		TokenPLUS		= 8,
		TokenMINUS		= 9,
		TokenSCOPE		= 10,
		TokenLBRACE		= 11,
		TokenRBRACE		= 12,
		TokenQSTRING	= 13,
		TokenNUMBER		= 14,
		Error			= 15
	};

	private:
	std::string 		input, id;
	util_ui32			pos;
	std::string			quotedString;
	double				number;

	void				Unput (void)					{ DASSERT(pos); --pos; }
	char				Input (void)					{ DASSERT(pos < input.length()); return input[pos++]; }
	util_ui32			Remaining (void) const			{ return (util_ui32) input.length() - pos; }
	bool				MatchKwd (const std::string& kwd);
	Token				ScanQuotedString (void);
	Token				ScanNumber (char c);

	public:
	Token				GetToken (void);
	bool				IsEof (void) const				{ return pos >= input.length(); }
	void				SkipBlanks (void);

	const std::string	GetQuotedString (void) const	{ return quotedString; }
	double				GetNumber (void) const			{ return number; }
	const std::string	GetId (void) const				{ return id;	}
	util_ui32			GetPos (void) const				{ return pos;	}

	void				Start (const std::string& _input) { input = _input; pos = 0; id.clear(); SkipBlanks(); }

	DeltaLibraryDefsScanner (void) : pos(0){}
	~DeltaLibraryDefsScanner(){}
};

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

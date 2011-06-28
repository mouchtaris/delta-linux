// DebugExprLexAn.h
// Header file with the only function exported to
// the parser of debug expressions.
// ScriptFighter Project.
// A. Savidis, February 2002.
//

#ifndef	DEBUGEXPRLEXAN_H
#define	DEBUGEXPRLEXAN_H

#include "utypes.h"

// The Bison stack variable is passed as a pointer.
//
class DebugExprLexAn {

	private:
	static void	SetTokenContent (
					void*			yylval, 
					util_ui16		tokenId,
					char*			tokenStart, 
					char*			tokenEnd
				);

	public:

	static void	Initialise (const char* inputBuffer);
	static void CleanUp (void);

	static int	GetToken (void* yylval);
	static void	ForceEOF (void);
	static bool	LastEOFWasForced (void);
};

#endif	// Do not add stuff beyond this point.

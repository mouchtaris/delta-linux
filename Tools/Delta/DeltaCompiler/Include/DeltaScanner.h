// DeltaScanner.h
// C++ lexer for the Delta Language (AST version)
// ScriptFighter Project.
// Y. Lilis, January 2013.
//

#ifndef DELTASCANNER_H
#define DELTASCANNER_H

#include "ulexutil.h"

#define FLEX_LEXER_PROTECTED_USER_DEFS						\
	void IgnoreCStyleComments (void);						\
	void IgnoreCPPStyleComments (void);						\
	const std::string ReadQuotedString (void);				\
	static char yyinput_wrapper(void* closure);				\
	static void yyunput_wrapper(char c, void *closure);		\
	std::string yy_id_buffer;

UFLEX_LEXER_SUBCLASS_DEF(DeltaScannerFlexLexer, FLEX_LEXER_PROTECTED_USER_DEFS, UEMPTY)

#undef FLEX_LEXER_PROTECTED_USER_DEFS

#endif	// Do not add stuff beyond this point.
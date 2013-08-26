// PointcutScanner.h
// C++ lexer for the AOP pointcuts
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef POINTCUT_SCANNER_H
#define POINTCUT_SCANNER_H

#include "ulexutil.h"

#define FLEX_LEXER_PROTECTED_USER_DEFS						\
	void IgnoreCStyleComments (void);						\
	const std::string ReadQuotedString (void);				\
	static char yyinput_wrapper(void* closure);				\
	static void yyunput_wrapper(char c, void *closure);		\
	std::string yy_id_buffer;

UFLEX_LEXER_SUBCLASS_DEF(PointcutScannerFlexLexer, FLEX_LEXER_PROTECTED_USER_DEFS, UEMPTY)

#undef FLEX_LEXER_PROTECTED_USER_DEFS

#endif	// Do not add stuff beyond this point.
// DeltaScanner.h
// C++ lexer for the Delta Language (AST version)
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef DELTASCANNER_H
#define DELTASCANNER_H

#undef yyFlexLexer
#define yyFlexLexer DeltaScannerFlexLexer
#include "FlexLexer.h"

#endif	// Do not add stuff beyond this point.
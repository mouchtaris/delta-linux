// DeltaDependenciesScanner.h
// C++ lexer for DeltaDependencies.
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef DELTASCANNER_H
#define DELTASCANNER_H

#undef yyFlexLexer
#define yyFlexLexer DeltaDependenciesFlexLexer
#include "FlexLexer.h"

#endif	// Do not add stuff beyond this point.
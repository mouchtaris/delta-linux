// DeltaLexAnalyser.h
// C++ lexer for the Delta Language.
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef DELTALEXANALYSER_H
#define DELTALEXANALYSER_H

#undef yyFlexLexer
#define yyFlexLexer DeltaLexAnalyserFlexLexer
#include "FlexLexer.h"

#endif	// Do not add stuff beyond this point.
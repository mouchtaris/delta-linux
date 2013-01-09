@ECHO OFF
REM lexer and parser generator generator

set flex="flex.exe"
set bison="bison.exe"
set sed="sed.exe"

setlocal

%bison% --no-lines DeltaSyntax.y
%flex% -L DeltaScanner.l
%sed% -e "s/#include <FlexLexer.h>/#include \"DeltaScanner.h\"/g" -e "s/class istream;/#include <iostream>\nusing namespace std;/g" DeltaScanner.cpp > tmpDeltaScanner.cpp
del DeltaScanner.cpp
ren tmpDeltaScanner.cpp DeltaScanner.cpp
move DeltaSyntax.hpp ..\Include\DeltaSyntax.h

%bison% --no-lines DeltaParser.y
%flex% -L DeltaLexAnalyser.l
%sed% -e "s/#include <FlexLexer.h>/#include \"DeltaLexAnalyser.h\"/g" -e "s/class istream;/#include <iostream>\nusing namespace std;/g" DeltaLexAnalyser.cpp > tmpDeltaLexAnalyser.cpp
del DeltaLexAnalyser.cpp
ren tmpDeltaLexAnalyser.cpp DeltaLexAnalyser.cpp
move DeltaParser.hpp ..\Include\DeltaParser.h

%bison% --no-lines DeltaDependenciesParser.y
%flex% -L DeltaDependenciesScanner.l
%sed% -e "s/#include <FlexLexer.h>/#include \"DeltaDependenciesScanner.h\"/g" -e "s/class istream;/#include <iostream>\nusing namespace std;/g" DeltaDependenciesScanner.cpp > tmpDeltaDependenciesScanner.cpp
del DeltaDependenciesScanner.cpp
ren tmpDeltaDependenciesScanner.cpp DeltaDependenciesScanner.cpp
move DeltaDependenciesParser.hpp ..\Include\DeltaDependenciesParser.h

endlocal

pause
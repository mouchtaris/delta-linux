@ECHO OFF
REM lexer and parser generator generator

set flex="C:\Progra~2\GnuWin32\Bin\flex.exe"
set bison="C:\Progra~2\GnuWin32\Bin\bison.exe"
set sed="C:\Progra~2\GnuWin32\Bin\sed.exe"

setlocal

%bison% --no-lines DeltaSyntax.y
%flex% -L DeltaScanner.l
%sed% -e "s/class istream;/#include <iostream>\nusing namespace std;/g" -e "/#include <unistd.h>/d" -e "s/exit( YY_EXIT_FAILURE );/throw std::exception(msg);/g" DeltaScanner.cpp > tmpDeltaScanner.cpp
del DeltaScanner.cpp
ren tmpDeltaScanner.cpp DeltaScanner.cpp
move DeltaSyntax.hpp ..\Include\DeltaSyntax.h

%bison% --no-lines DeltaParser.y
%flex% -L DeltaLexAnalyser.l
%sed% -e "s/class istream;/#include <iostream>\nusing namespace std;/g" -e "/#include <unistd.h>/d" -e "s/exit( YY_EXIT_FAILURE );/throw std::exception(msg);/g" DeltaLexAnalyser.cpp > tmpDeltaLexAnalyser.cpp
del DeltaLexAnalyser.cpp
ren tmpDeltaLexAnalyser.cpp DeltaLexAnalyser.cpp
move DeltaParser.hpp ..\Include\DeltaParser.h

%bison% --no-lines DeltaDependenciesParser.y
%flex% -L DeltaDependenciesScanner.l
%sed% -e "s/class istream;/#include <iostream>\nusing namespace std;/g" -e "/#include <unistd.h>/d" -e "s/exit( YY_EXIT_FAILURE );/throw std::exception(msg);/g" DeltaDependenciesScanner.cpp > tmpDeltaDependenciesScanner.cpp
del DeltaDependenciesScanner.cpp
ren tmpDeltaDependenciesScanner.cpp DeltaDependenciesScanner.cpp
move DeltaDependenciesParser.hpp ..\Include\DeltaDependenciesParser.h

endlocal

pause
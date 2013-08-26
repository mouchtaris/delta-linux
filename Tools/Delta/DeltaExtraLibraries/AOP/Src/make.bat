@ECHO OFF
REM lexer and parser generator generator

set flex="flex.exe"
set bison="bison.exe"
set sed="sed.exe"

setlocal

%bison% --no-lines PointcutSyntax.y
%flex% -L PointcutScanner.l
%sed% -e "s/#include <FlexLexer.h>/#include \"PointcutScanner.h\"/g" -e "s/class istream;/#include <iostream>\nusing namespace std;/g" PointcutScanner.cpp > tmpPointcutScanner.cpp
del PointcutScanner.cpp
ren tmpPointcutScanner.cpp PointcutScanner.cpp
move PointcutSyntax.hpp ..\Include\PointcutSyntax.h

endlocal

pause
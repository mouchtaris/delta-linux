@ECHO OFF
REM lexical analyzer generator

set exe="C:\Progra~1\GnuWin32\Bin\flex.exe"
set sed="C:\Progra~1\GnuWin32\Bin\sed.exe"

setlocal

if not exist %exe% goto notFound
if not exist %sed% goto notFound

del DeltaLexAnalyzerGen.cpp
%exe% -v -oDeltaLexAnalyzerGen.cpp DeltaLexAnalyzerSpec.l
%sed% -e "s/class istream;/#include <iostream>\nusing namespace std;/g" -e "/#include <unistd.h>/d" -e "s/exit( YY_EXIT_FAILURE );/throw std::exception(msg);/g" DeltaLexAnalyzerGen.cpp > tmpDeltaLexAnalyzerGen.cpp
del DeltaLexAnalyzerGen.cpp
ren tmpDeltaLexAnalyzerGen.cpp DeltaLexAnalyzerGen.cpp

goto end

:notFound
echo Cannot create flex scanner

:end

endlocal

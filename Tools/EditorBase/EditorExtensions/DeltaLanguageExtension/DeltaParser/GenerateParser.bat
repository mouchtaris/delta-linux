@ECHO OFF
REM parser generator generator

set exe="C:\Progra~1\GnuWin32\Bin\bison.exe"

setlocal

if not exist %exe% goto notFound

del DeltaParserGen.cpp DeltaParserGen.hpp
%exe% -o DeltaParserGen.cpp -d -v DeltaParserSpec.y

goto end

:notFound
echo %exe% was not found in path!

:end

endlocal

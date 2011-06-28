echo off
copy ..\RcParser.y RcParserCpy.y
\unix\bison -tvd RcParserCpy.y
move RcParserCpy_tab.c ..\RcParser.cpp
move RcParserCpy_tab.h ..\..\Include\RcParser.h
del RcParserCpy.y
move RcParserCpy.output RcParser.Output
pause
echo on

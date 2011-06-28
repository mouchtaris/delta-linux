echo off
copy ..\DebugExprParser.y ParserCpy.y
\unix\bison -tvd ParserCpy.y
move ParserCpy_tab.c ..\DebugExprParser.cpp
move ParserCpy_tab.h ..\..\Include\DebugExprParser.h
del ParserCpy.y
move ParserCpy.output DebugExprParser.Output
echo on
@pause

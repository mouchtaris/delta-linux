echo off
copy ..\JsonParser.y JsonParser.y
bison.exe -tvd ..\JsonParser.y
move JsonParser.tab.c ..\JsonParser.cpp
move JsonParser.tab.h ..\..\Include\JsonParser.h
del JsonParser.y
move JsonParser.output JsonParser.Output
echo on

@pause

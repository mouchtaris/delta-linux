echo off
copy ..\XmlParser.y XmlParser.y
C:\"Program Files (x86)"\GnuWin32\bin\bison -tvd XmlParser.y
move XmlParser.tab.c ..\XmlParser.cpp
move XmlParser.tab.h ..\..\Include\XmlParser.h
del XmlParser.y
move XmlParser.output XmlParser.Output
echo on
@pause

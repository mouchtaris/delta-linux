echo off
C:\GnuWin32\bin\flex -PXmlLex_yy ..\XmlLex.l
copy lex.XmlLex_yy.c XmlLex.cpp
del lex.XmlLex_yy.c
move XmlLex.cpp ..
echo on
@pause

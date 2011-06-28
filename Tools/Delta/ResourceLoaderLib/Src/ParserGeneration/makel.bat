echo off
\unix\flex  -PResourceLoader_yy ..\RcLexAnalyser.l
copy lex.ResourceLoader_yy.c RcLexAnalyser.cpp
del lex.ResourceLoader_yy.c
move RcLexAnalyser.cpp ..
echo on
@pause
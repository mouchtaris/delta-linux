echo off
flex.exe -PJsonLex_yy ..\JsonLex.l
copy lex.JsonLex_yy.c JsonLex.cpp
del lex.JsonLex_yy.c
move JsonLex.cpp ..
echo on
@pause

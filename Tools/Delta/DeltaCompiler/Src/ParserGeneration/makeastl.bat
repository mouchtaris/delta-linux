echo off
\unix\flex -PDeltaSyntax_yy ..\DeltaScanner.l
copy lex.DeltaSyntax_yy.c DeltaScanner.cpp
del lex.DeltaSyntax_yy.c
move DeltaScanner.cpp ..
\unix\flex -PDeltaDependencies_yy ..\DeltaDependenciesScanner.l
copy lex.DeltaDependencies_yy.c DeltaDependenciesScanner.cpp
del lex.DeltaDependencies_yy.c
move DeltaDependenciesScanner.cpp ..
echo on

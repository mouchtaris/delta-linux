echo off
\unix\flex -PDeltaCompiler_yy ..\DeltaLexAnalyser.l
copy lex.DeltaCompiler_yy.c DeltaLexAnalyser.cpp
del lex.DeltaCompiler_yy.c
move DeltaLexAnalyser.cpp ..
\unix\flex -PDeltaDependencies_yy ..\DeltaDependenciesScanner.l
copy lex.DeltaDependencies_yy.c DeltaDependenciesScanner.cpp
del lex.DeltaDependencies_yy.c
move DeltaDependenciesScanner.cpp ..
echo on

echo off
copy ..\DeltaSyntax.y SyntaxCpy.y
\unix\bison -tvd SyntaxCpy.y
move SyntaxCpy_tab.c ..\DeltaSyntax.cpp
move SyntaxCpy_tab.h ..\..\Include\DeltaSyntax.h
del SyntaxCpy.y
move SyntaxCpy.output DeltaSyntax.Output
copy ..\DeltaDependenciesParser.y DependenciesCpy.y
\unix\bison -tvd DependenciesCpy.y
move DependenciesCpy_tab.c ..\DeltaDependenciesParser.cpp
move DependenciesCpy_tab.h ..\..\Include\DeltaDependenciesParser.h
del DependenciesCpy.y
move DependenciesCpy.output DeltaDependenciesParser.Output
echo on
@pause
echo off
copy ..\DeltaParser.y ParserCpy.y
\unix\bison -tvd ParserCpy.y
move ParserCpy_tab.c ..\DeltaParser.cpp
move ParserCpy_tab.h ..\..\Include\DeltaParser.h
del ParserCpy.y
move ParserCpy.output DeltaParser.Output
copy ..\DeltaDependenciesParser.y DependenciesCpy.y
\unix\bison -tvd DependenciesCpy.y
move DependenciesCpy_tab.c ..\DeltaDependenciesParser.cpp
move DependenciesCpy_tab.h ..\..\Include\DeltaDependenciesParser.h
del DependenciesCpy.y
move DependenciesCpy.output DeltaDependenciesParser.Output
echo on
@pause
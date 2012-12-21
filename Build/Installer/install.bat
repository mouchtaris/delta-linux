@echo off
echo Copying Sparrow configuration files...
xcopy %~dp0Config\* %USERPROFILE%\Sparrow\Properties\ /i /Y
call %~dp0..\..\Tools\Delta\delta_environment_var.bat
copy %~dp0..\..\Tools\Delta\DeltaExtraLibraries\XMLParser\lib\release\XMLParser.dll %~dp0..\extension_dlls
copy %~dp0..\..\Tools\Delta\DeltaExtraLibraries\XMLParser\lib\debug\XMLParserD.dll %~dp0..\extension_dlls
echo Done!
pause
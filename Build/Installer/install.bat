@echo off
echo Copying Sparrow configuration files...
xcopy %~dp0Config\* %USERPROFILE%\Sparrow\Properties\ /i /Y
call %~dp0..\..\Tools\Delta\delta_environment_var.bat
echo Done!
pause
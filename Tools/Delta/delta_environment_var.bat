@echo off
echo Registering DELTA environment variable.
path=%~dp0
IF %path:~-1%==\ SET path=%path:~0,-1%
@echo on
setx DELTA %path% /m
@echo off
IF NOT "%VS80COMNTOOLS%" == "" GOTO VS2005
IF NOT "%VS100COMNTOOLS%" == "" GOTO VS2010
@ECHO No compatible Visual Studio version was detected. Expected Visual Studio 2005 or 2010.

:VS2005
call "%VS80COMNTOOLS%\vsvars32.bat"
GOTO END

:VS2010
call "%VS100COMNTOOLS%\vsvars32.bat"
GOTO END

:END

@echo off
set NLM=^


set NL=^^^%NLM%%NLM%^%NLM%%NLM%

set path=%CD%
set quoted_path="%path:\=\\%"

echo Creating registry entry...
echo Windows Registry Editor Version 5.00%NL%%NL%[HKEY_LOCAL_MACHINE\SOFTWARE\Sparrow]%NL%@=%quoted_path%%NL% > registry.reg

echo Registering entry...
registry.reg

echo Removing temporary entry file...
del registry.reg

echo Associating wsp entries with Sparrow
assoc .wsp=Sparrow.wsp
ftype Sparrow.wsp="%path%\Sparrow.exe" "%%1" %%*

echo Done!
pause
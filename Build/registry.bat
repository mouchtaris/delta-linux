@echo off

echo Associating wsp entries with Sparrow
assoc .wsp=Sparrow.wsp
ftype Sparrow.wsp="%~dp0Sparrow.exe" "%%1" %%*

echo Done!
pause
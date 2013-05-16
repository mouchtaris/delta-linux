setx WXWINVER WXWIN28 /m
setx WXWINDIR %WXWIN28% /m
REG DELETE "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v WXWIN28 /f
pause

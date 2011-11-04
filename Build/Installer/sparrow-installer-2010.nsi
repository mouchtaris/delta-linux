; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Sparrow"
!define PRODUCT_VERSION "1.0-rev-163"
!define PRODUCT_WEB_SITE "http://www.ics.forth.gr/hci/files/plang/Delta/Delta.html"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Sparrow.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKCU"

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "x64.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "sparrow.ico"
!define MUI_UNICON "sparrow.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
;!insertmacro MUI_PAGE_LICENSE "License.rtf"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
;!define MUI_FINISHPAGE_RUN "$INSTDIR\Sparrow.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "sparrow-setup.exe"
InstallDir "$PROGRAMFILES\Sparrow"
InstallDirRegKey HKCU "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails hide
ShowUnInstDetails hide

Section "Core" SEC01
  SectionIn RO
  SetOverwrite ifnewer
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer

  ${If} ${RunningX64}
	File "..\bin\x64\msvcp100.dll"
	File "..\bin\x64\msvcr100.dll"
  ${Else}
    File "..\bin\x86\msvcp100.dll"
	File "..\bin\x86\msvcr100.dll"
  ${EndIf}  
  
  File "..\wxbase28u_vc_custom.dll"
  File "..\wxbase28u_xml_vc_custom.dll"
  File "..\wxmsw28u_adv_vc_custom.dll"
  File "..\wxmsw28u_aui_vc_custom.dll"
  File "..\wxmsw28u_core_vc_custom.dll"
  File "..\wxmsw28u_html_vc_custom.dll"
  File "..\wxmsw28u_richtext_vc_custom.dll"
 
  File "..\BaseLibrary.dll"
  File "..\ThirdPartyBaseLibrary.dll"
  File "..\IDECore.dll"

  File "..\sparrow.dmsl"
  File "..\dmsl.ini"

  File "..\EditorLanguages.xml"
  File "..\EditorBaseLibrary.dll"

  File "..\DefaultComponents.xdf"
  File "..\DebuggedComponents.xdf"
  File "..\SparrowDebugged.bat"
  File "..\Sparrow.exe"
 
  CreateDirectory "$SMPROGRAMS\Sparrow"
  CreateDirectory "$APPDATA\Sparrow"
  CreateShortCut "$SMPROGRAMS\Sparrow\Sparrow.lnk" "$INSTDIR\Sparrow.exe"
  CreateShortCut "$DESKTOP\Sparrow.lnk" "$INSTDIR\Sparrow.exe"

  Exec "assoc .wsp=Sparrow.wsp"
  Exec 'ftype Sparrow.wsp="$INSTDIR\Sparrow.exe" "%1" %*'
SectionEnd

Section "Core Components" SEC02
  SectionIn RO
  SetOverwrite ifnewer
  SetOutPath "$INSTDIR\extensions"
  File /r /x "*_d.dll" "..\extensions\*.dll"

  SetOutPath "$INSTDIR\xdf"
  File /r "..\xdf\*.xdf"

  SetOutPath "$INSTDIR\EditorExtensions"
  File /r /x "*_d.dll" "..\EditorExtensions\*.dll"

SectionEnd

Section "Delta Support" SEC03
  SectionIn RO
  SetOverwrite ifnewer

  SetOutPath "$INSTDIR"
  File "..\DeltaCompiler.exe"
  File "..\DeltaVM.exe"
  File "..\DeltaVMDebug.exe"
  File "..\DeltaConsoleDebugger.exe"

  File "..\SparrowLib.dll"

  File "..\BytecodeStructures.dll"
  File "..\CompilerComponents.dll"
  File "..\DebuggerBackend.dll"
  File "..\DebuggerCommon.dll"
  File "..\DebuggerFrontend.dll"
  File "..\ReadersWriters.dll"
  File "..\Sockets.dll"
  File "..\SyntaxTree.dll"
  File "..\UtilAndDefensiveHeap.dll"
  File "..\VirtualMachineComponents.dll"

  SetOutPath "$INSTDIR\scripts"
  File "..\scripts\Sparrow.wsp"
  File "..\scripts\SparrowLibFuncs.txt"

  SetOutPath "$INSTDIR\scripts\bin"
  File "..\scripts\bin\*.dbc"

  SetOutPath "$INSTDIR\scripts\sparrowlib"
  File "..\scripts\sparrowlib\sparrowlib.dproj"
  File "..\scripts\sparrowlib\*.dsc"

  SetOutPath "$INSTDIR\resources"
  File /r "..\resources\*.png"
  File /r "..\resources\*.gif"

  SetOutPath "$INSTDIR\profiles"
  File "..\profiles\profiles.dproj"

  SetOutPath "$INSTDIR\profiles\beginner"
  File "..\profiles\beginner\*.*"

  SetOutPath "$INSTDIR\profiles\simple"
  File "..\profiles\simple\*.*"

  SetOutPath "$INSTDIR\profiles\welcome"
  File "..\profiles\welcome\*.*"

  SetOutPath "$INSTDIR\profiles\Delta Developer"
  File "..\profiles\Delta Developer\*.*"

  SetOutPath "$INSTDIR\profiles\Sparrow Devel"
  File "..\profiles\Sparrow Devel\*.*"

  SetOutPath "$INSTDIR\extension_scripts"
  File "..\extension_scripts\extensions.dproj"
  File "..\extension_scripts\*.dsc"
  File "..\extension_scripts\*.dbc"

  SetOutPath "$INSTDIR\config_scripts"
  File "..\config_scripts\*.dbc"

SectionEnd

Section "Remote Deployment" SEC04
  SetOverwrite ifnewer
  SetOutPath "$INSTDIR\RemoteDeployment\include"
  File "..\..\ExternalInterfaces\IDEDeployment\IDEDeploymentCommon.h"
  File "..\..\ExternalInterfaces\IDEDeployment\RemoteDeploymentAPI.h"
  File "..\..\ExternalInterfaces\IDEDeployment\RemoteValue.h"

  SetOutPath "$INSTDIR\RemoteDeployment\lib"
  File "..\IDEDeployment.lib"
  File "..\IDEDeployment.exp"

  SetOutPath "$INSTDIR\RemoteDeployment\bin"
  File "..\BaseLibrary.dll"
  File "..\IDECore.dll"
  File "..\IDEDeployment.dll"
  File "..\ThirdPartyBaseLibrary.dll"
SectionEnd

Function StrSlash
  Exch $R3 ; $R3 = needle ("\" or "/")
  Exch
  Exch $R1 ; $R1 = String to replacement in (haystack)
  Push $R2 ; Replaced haystack
  Push $R4 ; $R4 = not $R3 ("/" or "\")
  Push $R6
  Push $R7 ; Scratch reg
  StrCpy $R2 ""
  StrLen $R6 $R1
  StrCpy $R4 "\"
  StrCmp $R3 "/" loop
  StrCpy $R4 "/"  
loop:
  StrCpy $R7 $R1 1
  StrCpy $R1 $R1 $R6 1
  StrCmp $R7 $R3 found
  StrCpy $R2 "$R2$R7"
  StrCmp $R1 "" done loop
found:
  StrCpy $R2 "$R2$R4"
  StrCmp $R1 "" done loop
done:
  StrCpy $R3 $R2
  Pop $R7
  Pop $R6
  Pop $R4
  Pop $R2
  Pop $R1
  Exch $R3
FunctionEnd

Section "Extra Delta Libraries" SEC05
  SetOverwrite ifnewer
  SetOutPath "$INSTDIR\DeltaExtraLibraries"
  File /r "..\..\Tools\Delta\Manuals\Examples\ALADIN\ALADIN.dll"
  File /r "..\..\Tools\Delta\Manuals\Examples\ALADIN\ALADINLibFuncs.txt"

  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\DeltaCorbaCommon\Release\DeltaCorbaCommon.dll"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\DeltaCorbaClient\Release\DeltaCorbaClient.dll"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\DeltaCorbaServer\Release\DeltaCorbaServer.dll"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\CorbaLibfuncManual.txt"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\CorbaLibfuncs.txt"

  File /r "..\..\Tools\Delta\DeltaExtraLibraries\XMLParser\lib\release\XMLParser.dll"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\XMLParser\XMLParserLibFuncs.txt"

  File /r "..\..\Tools\Delta\DeltaExtraLibraries\wxWidgets\lib\release\WxWidgets.dll"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\wxWidgets\WxWidgetsLibFuncs.txt"
SectionEnd

Section "Test Workspaces" SEC06
  SetOverwrite ifnewer
  CreateDirectory "$PROFILE\Sparrow\Workspaces"  
  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaTests"
  File /r "..\..\Tools\Delta\Manuals\Examples\*.dsc"
  File /r "..\..\Tools\Delta\Manuals\Examples\*.dproj"
  File /r "..\..\Tools\Delta\Manuals\Examples\*.wsp"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaTests\C\A"
  File /r "..\..\Tools\Delta\Manuals\Examples\C\A\k_outside.dbc"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaTests\ALADIN"
  File /r "..\..\Tools\Delta\Manuals\Examples\ALADIN\*.dproj"
  File /r "..\..\Tools\Delta\Manuals\Examples\ALADIN\*.dsc"
  File /r "..\..\Tools\Delta\Manuals\Examples\ALADIN\*.txt"
  File /r "..\..\Tools\Delta\Manuals\Examples\ALADIN\*.bmp"
  File /r "..\..\Tools\Delta\Manuals\Examples\ALADIN\*.dll"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaWxWidgets"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\wxWidgets\sample\*.wsp"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\wxWidgets\sample\*.dproj"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\wxWidgets\sample\*.dsc"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\wxWidgets\sample\*.bmp"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\XMLParser\lib\release\XMLParser.dll"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\wxWidgets\lib\release\WxWidgets.dll"
  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaWxWidgets\cards"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\wxWidgets\sample\cards\*.png"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaCORBA"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\*.wsp"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\*.bat"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\*.idl"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaCORBA\bank"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\bank\*.dproj"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\bank\*.dsc"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\bank\*.idl"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\bank\*.bat"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaCORBA\deferred"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\deferred\*.dproj"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\deferred\*.dsc"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\deferred\*.idl"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\deferred\*.bat"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaCORBA\echo"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\echo\*.dproj"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\echo\*.dsc"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\echo\*.idl"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\echo\*.bat"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaCORBA\generic"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\generic\*.dproj"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\generic\*.dsc"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\generic\*.idl"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\tests\generic\*.bat"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaCORBA\bin"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\DeltaCorbaCommon\Release\DeltaCorbaCommon.dll"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\DeltaCorbaClient\Release\DeltaCorbaClient.dll"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\CORBAGateway\DeltaCorbaServer\Release\DeltaCorbaServer.dll"

  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaUIGenerator"
  File /r "..\..\Tools\Delta\DeltaTools\DeltaUIGenerator\*.wsp"
  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaUIGenerator\Src"
  File /r "..\..\Tools\Delta\DeltaTools\DeltaUIGenerator\Src\*.dsc"
  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaUIGenerator\Project"
  File /r "..\..\Tools\Delta\DeltaTools\DeltaUIGenerator\Project\*.dproj"
  SetOutPath "$PROFILE\Sparrow\Workspaces\DeltaUIGenerator\dbc"
  File /r "..\..\Tools\Delta\DeltaExtraLibraries\wxWidgets\lib\release\WxWidgets.dll"


  CreateDirectory "$PROFILE\Sparrow\Properties"
  SetOutPath "$PROFILE\Sparrow\Properties"
  File "..\Workspaces\Shell_Config.xml"

  StrCpy $3 "22 serialization::archive 4 0 0 1 0" ; String serialization prefix

  ; Register Recent workspaces
  FileOpen $0 "$PROFILE\Sparrow\Properties\RecentWorkspaces_Config.xml" w
  FileWrite $0 `<?xml version="1.0" encoding="utf-8"?>$\n`
  FileWrite $0 `<PropertyTable>$\n`

  StrCpy $1 "$INSTDIR\scripts\Sparrow.wsp"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "$\t<workspace5>$3 $2 $1</workspace5>$\n"

  StrCpy $1 "$PROFILE\Sparrow\Workspaces\DeltaUIGenerator\UIGenerator.wsp"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "$\t<workspace4>$3 $2 $1</workspace4>$\n"

  StrCpy $1 "$PROFILE\Sparrow\Workspaces\DeltaCORBA\DeltaCorba.wsp"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "$\t<workspace3>$3 $2 $1</workspace3>$\n"

  StrCpy $1 "$PROFILE\Sparrow\Workspaces\DeltaWxWidgets\sample_workspace.wsp"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "$\t<workspace2>$3 $2 $1</workspace2>$\n"

  StrCpy $1 "$PROFILE\Sparrow\Workspaces\DeltaTests\Tests.wsp"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "$\t<workspace1>$3 $2 $1</workspace1>$\n"

  FileWrite $0 `</PropertyTable>`
  FileClose $0

  ; Register Delta Libraries
  FileOpen $0 "$PROFILE\Sparrow\Properties\DeltaVM_Config.xml" w
  FileWrite $0 `<?xml version="1.0" encoding="utf-8"?>$\n`
  FileWrite $0 `<PropertyTable>$\n`
  FileWrite $0 `$\t<vm>$\n$\t$\t<libdefs>$\n`

  FileWrite $0 `$\t$\t$\t<value4>$\n$\t$\t$\t$\t`
  StrCpy $1 "$INSTDIR\DeltaExtraLibraries\XMLParserLibFuncs.txt"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "<path>$3 $2 $1</path>$\n"
  FileWrite $0 `$\t$\t$\t$\t<name>$3 37 22 serialization::archive 4 0 0 3 xml</name>$\n$\t$\t$\t</value4>$\n`

  FileWrite $0 `$\t$\t$\t<value3>$\n$\t$\t$\t$\t`
  StrCpy $1 "$INSTDIR\DeltaExtraLibraries\WxWidgetsLibFuncs.txt"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "<path>$3 $2 $1</path>$\n"
  FileWrite $0 `$\t$\t$\t$\t<name>$3 36 22 serialization::archive 4 0 0 2 wx</name>$\n$\t$\t$\t</value3>$\n`

  FileWrite $0 `$\t$\t$\t<value2>$\n$\t$\t$\t$\t`
  StrCpy $1 "$INSTDIR\scripts\SparrowLibFuncs.txt"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "<path>$3 $2 $1</path>$\n"
  FileWrite $0 `$\t$\t$\t$\t<name>$3 37 22 serialization::archive 4 0 0 3 spw</name>$\n$\t$\t$\t</value2>$\n`

  FileWrite $0 `$\t$\t$\t<value1>$\n$\t$\t$\t$\t`
  StrCpy $1 "$INSTDIR\DeltaExtraLibraries\CorbaLibfuncs.txt"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "<path>$3 $2 $1</path>$\n"
  FileWrite $0 `$\t$\t$\t$\t<name>$3 39 22 serialization::archive 4 0 0 5 corba</name>$\n$\t$\t$\t</value1>$\n`

  FileWrite $0 `$\t$\t$\t<value0>$\n$\t$\t$\t$\t`
  StrCpy $1 "$INSTDIR\DeltaExtraLibraries\ALADINLibFuncs.txt"
  Push $1
  Push "\"
  Call StrSlash
  Pop $1
  StrLen $2 "$1"
  StrCpy $1 "22 serialization::archive 4 0 0 $2 $1"
  StrLen $2 "$1"
  FileWrite $0 "<path>$3 $2 $1</path>$\n"
  FileWrite $0 `$\t$\t$\t$\t<name>$3 40 22 serialization::archive 4 0 0 6 aladin</name>$\n$\t$\t$\t</value0>$\n`

  FileWrite $0 `$\t$\t</libdefs>$\n$\t</vm>$\n$\t<debugger>$\n`
  FileWrite $0 `$\t$\t<tostring_maxlength>22 serialization::archive 4 0 0 1 0 30 22 serialization::archive 4 64</tostring_maxlength>$\n`
  FileWrite $0 `$\t$\t<receive_variables>22 serialization::archive 4 0 0 1 0 29 22 serialization::archive 4 1</receive_variables>$\n`
  FileWrite $0 `$\t$\t<object_graph>$\n`
  FileWrite $0 `$\t$\t$\t<expandProgramFuncs>22 serialization::archive 4 0 0 1 0 29 22 serialization::archive 4 0</expandProgramFuncs>$\n`
  FileWrite $0 `$\t$\t$\t<expandMethodFuncs>22 serialization::archive 4 0 0 1 0 29 22 serialization::archive 4 0</expandMethodFuncs>$\n`
  FileWrite $0 `$\t$\t$\t<expandLibraryFuncs>22 serialization::archive 4 0 0 1 0 29 22 serialization::archive 4 0</expandLibraryFuncs>$\n`
  FileWrite $0 `$\t$\t$\t<expandFuncVMs>22 serialization::archive 4 0 0 1 0 29 22 serialization::archive 4 0</expandFuncVMs>$\n`
  FileWrite $0 `$\t$\t</object_graph>$\n$\t</debugger>$\n</PropertyTable>`

SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\DeltaWebsite.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\Sparrow\Delta Website.lnk" "$INSTDIR\DeltaWebsite.url"
  CreateShortCut "$SMPROGRAMS\Sparrow\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
;  WriteRegStr HKCU "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Sparrow.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Sparrow.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Sparrow core framework"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Basic components"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} "Sparrow Delta support files"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "Remote deployment support"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC05} "Delta Extra Libraries"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC06} "Test Workspaces"
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  RMDir /r "$INSTDIR\config_scripts"
  RMDir /r "$INSTDIR\EditorExtensions"
  RMDir /r "$INSTDIR\extension_scripts"
  RMDir /r "$INSTDIR\extensions"
  RMDir /r "$INSTDIR\profiles"
  RMDir /r "$INSTDIR\resources"
  RMDir /r "$INSTDIR\scripts"
  RMDir /r "$INSTDIR\xdf"
  RMDir /r "$INSTDIR\xdf"
  RMDir /r "$INSTDIR\DeltaExtraLibraries"
  RMDir /r "$INSTDIR\RemoteDeployment"
  RMDir /r "$APPDATA\Sparrow"
  RMDir /r "$PROFILE\Sparrow\Workspaces\DeltaTests"
  RMDir /r "$PROFILE\Sparrow\Workspaces\DeltaWxWidgets"
  RMDir /r "$PROFILE\Sparrow\Workspaces\DeltaCORBA"
  RMDir /r "$PROFILE\Sparrow\Workspaces\DeltaUIGenerator"
  RMDir /r "$PROFILE\Sparrow\Properties"

  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"

  Delete "$SMPROGRAMS\Sparrow\Uninstall.lnk"
  Delete "$SMPROGRAMS\Sparrow\Delta Website.lnk"
  Delete "$DESKTOP\Sparrow.lnk"
  Delete "$SMPROGRAMS\Sparrow\Sparrow.lnk"
  Delete "$SMPROGRAMS\Sparrow\TestDeploymentAPI.lnk"

  RMDir "$SMPROGRAMS\Sparrow\Workspaces"
  RMDir "$SMPROGRAMS\Sparrow"
  RMDir "$PROFILE\Sparrow"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKCU "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

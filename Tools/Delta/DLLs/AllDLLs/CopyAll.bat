echo Copying DLLs...
copy ..\BytecodeStructures\Lib\Debug\BytecodeStructuresD.dll				Debug
copy ..\BytecodeStructures\Lib\Release\BytecodeStructures.dll				Release
copy ..\CompilerComponents\Lib\Debug\CompilerComponentsD.dll				Debug
copy ..\CompilerComponents\Lib\Release\CompilerComponents.dll				Release
copy ..\DebuggerBackend\Lib\Debug\DebuggerBackendD.dll						Debug
copy ..\DebuggerBackend\Lib\Release\DebuggerBackend.dll						Release
copy ..\DebuggerCommon\Lib\Debug\DebuggerCommonD.dll						Debug
copy ..\DebuggerCommon\Lib\Release\DebuggerCommon.dll						Release
copy ..\DebuggerFrontend\Lib\Debug\DebuggerFrontendD.dll					Debug
copy ..\DebuggerFrontend\Lib\Release\DebuggerFrontend.dll					Release
copy ..\ReadersWriters\Lib\Debug\ReadersWritersD.dll						Debug
copy ..\ReadersWriters\Lib\Release\ReadersWriters.dll						Release 
copy ..\SocketLib\Lib\Debug\SocketsD.dll									Debug
copy ..\SocketLib\Lib\Release\Sockets.dll									Release
copy ..\UtilAndDebugLib\Lib\Debug\UtilAndDefensiveHeapD.dll					Debug
copy ..\UtilAndDebugLib\Lib\Release\UtilAndDefensiveHeap.dll				Release
copy ..\VirtualMachineComponents\Lib\Debug\VirtualMachineComponentsD.dll	Debug
copy ..\VirtualMachineComponents\Lib\Release\VirtualMachineComponents.dll	Release
copy ..\SyntaxTree\Lib\Debug\SyntaxTreeD.dll								Debug
copy ..\SyntaxTree\Lib\Release\SyntaxTree.dll								Release
pause
echo Copied!

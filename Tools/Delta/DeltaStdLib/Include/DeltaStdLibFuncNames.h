// DeltaStdLibFuncNames.h
// Delta std library function names. When this header is included,
// a local declaration is made, through which the std lib functions
// can be used. Include this only in one file, where the compilation
// extern function names need to be supplied.
// ScriptFigher Project.
// A. Savidis, June 2001 (original version).
// Inheritance functions added June 2004.
// Delegation functions added February 2009.
// Last entries added March 2009 (dynamic compilation).
//

#ifndef	DELTASTDLIBFUNCNAMES_H
#define	DELTASTDLIBFUNCNAMES_H

#include "utypes.h"
#include "DeltaStdClassNames.h"
#include "DeltaStdDefs.h"

//////////////////////////////////////

static const char* stdLibFuncNames[] = {
	"std_getapi",
	DELTA_STDLIB_NAMESPACE_ENTER

	DELTA_STDLIB_NAMESPACE_TYPE(vm)
	DELTA_STDLIB_NAMESPACE_TYPE(list)
	DELTA_STDLIB_NAMESPACE_TYPE(vector)
	DELTA_STDLIB_NAMESPACE_TYPE(listiter)
	DELTA_STDLIB_NAMESPACE_TYPE(vectoriter)
	DELTA_STDLIB_NAMESPACE_TYPE(tableiter)
	DELTA_STDLIB_NAMESPACE_TYPE(file)
	DELTA_STDLIB_NAMESPACE_TYPE(reader)
	DELTA_STDLIB_NAMESPACE_TYPE(writer)
	DELTA_STDLIB_NAMESPACE_TYPE(outputbuffer)
	DELTA_STDLIB_NAMESPACE_TYPE(inputbuffer)
	DELTA_STDLIB_NAMESPACE_TYPE(socket)
	DELTA_STDLIB_NAMESPACE_TYPE(dll)
	DELTA_STDLIB_NAMESPACE_TYPE(ast)
	DELTA_STDLIB_NAMESPACE_TYPE(astvisitor)

	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_NUMBER,				DELTA_NUMBER_TYPESTRING)
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_STRING,				DELTA_STRING_TYPESTRING)
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_TABLE,				DELTA_TABLE_TYPESTRING)
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_OBJECT,				DELTA_OBJECT_TYPESTRING)
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_PROGRAMFUNC,		DELTA_PROGRAMFUNC_TYPESTRING)
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_LIBRARYFUNC,		DELTA_LIBRARYFUNC_TYPESTRING)
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_BOOL,				DELTA_BOOL_TYPESTRING)
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_EXTERNID,			DELTA_EXTERNID_TYPESTRING)
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_NIL,				DELTA_NIL_TYPESTRING)	
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_METHODFUNC,			DELTA_METHODFUNC_TYPESTRING)
	DELTA_STDLIB_NAMESPACE_CONST(TYPEOF_UNDEFINED,			DELTA_UNDEFINED_TYPESTRING)

	DELTA_STDLIB_NAMESPACE_CONST(ALGORITHMS_SUPERCLASS,		DELTA_STD_ALGORITHMS_SUPERCLASS)
	DELTA_STDLIB_NAMESPACE_CONST(ALGORITHMS_ITERATOR,		DELTA_STD_ALGORITHMS_ITERATOR)
	DELTA_STDLIB_NAMESPACE_CONST(ALGORITHMS_CONTAINER,		DELTA_STD_ALGORITHMS_CONTAINER)

	DELTA_STDLIB_NAMESPACE_CONST(DEPENDENCY_NOT_FOUND,		DELTA_DEPENDENCY_NOT_FOUND)
	DELTA_STDLIB_NAMESPACE_CONST(DEPENDENCY_ONE_FOUND,		DELTA_DEPENDENCY_ONE_FOUND)
	DELTA_STDLIB_NAMESPACE_CONST(DEPENDENCY_MANY_FOUND,		DELTA_DEPENDENCY_MANY_FOUND)

	DELTA_STDLIB_NAMESPACE_CONST(SOCKET_PACKET_ORIGINAL,	DELTA_SOCKET_PACKET_ORIGINAL)
	DELTA_STDLIB_NAMESPACE_CONST(SOCKET_PACKET_ADD_SIZE,	DELTA_SOCKET_PACKET_ADD_SIZE)

	"#group(errors)",
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_GENERAL_RUNTIME_ERROR,					DELTA_GENERAL_RUNTIME_ERROR)					
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_ARITH_OPERATOR_ERROR,					DELTA_ARITH_OPERATOR_ERROR)					
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_RELAT_OPERATOR_ERROR,					DELTA_RELAT_OPERATOR_ERROR)					
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_NOT_CALLABLE_ERROR,						DELTA_NOT_CALLABLE_ERROR)					
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_FORMAL_ARG_WITHOUT_ACTUAL_VALUE_ERROR,	DELTA_FORMAL_ARG_WITHOUT_ACTUAL_VALUE_ERROR)	
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_UNRESOLVED_LIBFUNC_ERROR,				DELTA_UNRESOLVED_LIBFUNC_ERROR)				
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_LIBFUNC_NATIVE_EXCEPTION_ERROR,			DELTA_LIBFUNC_NATIVE_EXCEPTION_ERROR)		
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_UNRESOLVED_GLOBALFUNC_ERROR,				DELTA_UNRESOLVED_GLOBALFUNC_ERROR)			
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_SUICIDAL_VM_ERROR,						DELTA_SUICIDAL_VM_ERROR)						
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_ZOMBI_VM_ERROR,							DELTA_ZOMBI_VM_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_INVALID_SUPPLIED_CLOSURE_ERROR,			DELTA_INVALID_SUPPLIED_CLOSURE_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_NO_SUPPLIED_CLOSURE_ERROR,				DELTA_NO_SUPPLIED_CLOSURE_ERROR)		
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_FUNCTOR_SLOT_MISSING_ERROR,				DELTA_FUNCTOR_SLOT_MISSING_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_STACK_OVERFLOW_ERROR,					DELTA_STACK_OVERFLOW_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_FOREACH_MISSING_METHODS_ERROR,			DELTA_FOREACH_MISSING_METHODS_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_FOREACH_INVALID_ITERATOR_ERROR,			DELTA_FOREACH_INVALID_ITERATOR_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_FOREACH_INVALID_CONTAINER_ERROR,			DELTA_FOREACH_INVALID_CONTAINER_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_VM_NOT_RUN_ERROR,						DELTA_VM_NOT_RUN_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_FUNCTOR_OVERLOADING_DISABLED_ERROR,		DELTA_FUNCTOR_OVERLOADING_DISABLED_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_FUNCTOR_CYCLIC_REFERENCE_ERROR,			DELTA_FUNCTOR_CYCLIC_REFERENCE_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_OPERATOR_OVERLOADING_DISABLED_ERROR,		DELTA_OPERATOR_OVERLOADING_DISABLED_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_OPERATOR_OVERLOADING_ERROR,				DELTA_OPERATOR_OVERLOADING_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_OPERATOR_SLOT_NOT_CALLABLE_ERROR,		DELTA_OPERATOR_SLOT_NOT_CALLABLE_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_ILL_FORMED_TABLE_IN_DYNAMIC_ARGS_ERROR,	DELTA_ILL_FORMED_TABLE_IN_DYNAMIC_ARGS_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_INVALID_VALUE_IN_DYNAMIC_ARGS_ERROR,		DELTA_INVALID_VALUE_IN_DYNAMIC_ARGS_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_EXTERNID_MISSING_USERDATA_ERROR,			DELTA_EXTERNID_MISSING_USERDATA_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_SET_METHOD_SELF_INVALID_VALUE_ERROR,		DELTA_SET_METHOD_SELF_INVALID_VALUE_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_INVALID_METHOD_VALUE_INDEXING,			DELTA_INVALID_METHOD_VALUE_INDEXING)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_SET_METHOD_SELF_NEEDS_CONST_INDEX_ERROR, DELTA_SET_METHOD_SELF_NEEDS_CONST_INDEX_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_INDEXING_INVALID_VALUE_ERROR,			DELTA_INDEXING_INVALID_VALUE_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_OBJECT_ATTRIBUTE_OPERATION_ERROR,		DELTA_OBJECT_ATTRIBUTE_OPERATION_ERROR)
	DELTA_STDLIB_NAMESPACE_CONST(DELTA_USER_RUNTIME_ERROR,						DELTA_USER_RUNTIME_ERROR)

	// MetaCompiler functions
	"#group(meta)",
	"inline"						"(val:Any)												:	Void		",
	"context"						"()														:	ast			"
									"(tag:String)											:	ast			",

	// Common miscellaneous functions.
	"#group(misc)",
	"print"							"(...)													:	Void		",
	"callglobal"					"(f:String,...)											:	Any			",
	"typeof"						"(x:Any)												:	String		",
	"iscallable"					"(x:Any)												:	Bool		",
	"isundefined"					"(x:Any)												:	Bool		",
	"isnil"							"(x:Any)												:	Bool		",
	"undefined"						"()														:	Undefined	",
	"externidtype"					"(x:ExternId)											:	String		",
	"externiduserdata"				"(x:ExternId)											:	Object		",
	"taggedvalue"					"(x:Any, tag:String)									:	Any			",
	"getvaluetag"					"(x:Any)												:	String		",
	"isoverloadableoperator"		"(op:Any)												:	Bool		",
	"equal"							"(x:Any, y:Any)											:	Bool		",
	"currenttime"					"()														:	Number		",
	"getenvironmentvar"				"(id:String)											:	String		",
	"error"							"(msg:String)											:	Void		",
	"tostring"						"(x:Any)												:	String		",
	"iserrortag"					"(x:String)												:	Bool		",

	// Global operator overloading.
	"#group(op)",
	"setarithmeticoperator"			"(op:String, f:Callable)								:	Void		", 
	"setrelationaloperator"			"(op:String, f:Callable)								:	Void		", 
	"setassignmentoperator"			"(f:Callable)											:	Void		", 
	"settypecastingoperator"		"(f:Callable)											:	Void		",
	"getarithmeticoperator"			"(op:String)											:	Callable	", 
	"getrelationaloperator"			"(op:String)											:	Callable	",
	"getassignmentoperator"			"()														:	Callable	",
	"gettypecastingoperator"		"()														:	Callable	",

	// Lib func handling.
	"#group(lib)",
	"libfuncget"					"(id:String)											:	LibraryFunc	",
	"libfuncisbound"				"(f:LibraryFunc)										:	Bool		",
	"libfuncbind"					"(f:LibraryFunc,...)									:	LibraryFunc	",
	"libfuncunbound"				"(f:LibraryFunc)										:	LibraryFunc	",
	"libfuncgetboundargs"			"(f:LibraryFunc)										:	ExternId	",

	// Resource loading.
	"#group(rc)",
	"rcload"						"(path:String)											:	Object		",
	"rcsetpreprocessor"				"(cpp:String, includeFlags:String)						:	Object		",
	"rcparse"						"(text:String)											:	Object		",
	"rcloadgeterror"				"()														:	String		",
	"rcstore"						"(rc:Object, path:String)								:	Bool		",

	// Dynamic library loading.
	"#group(dll)",
	"dllimport"						"(path:String, func:String)								:	dll			",
	"dllimportdeltalib"				"(path:String)											:	dll			",
	"dllimportaddpath"				"(path:String)											:	Void		",
	"dllinvoke"						"(dllObject:dll, func:String)							:	Any			",
	"dllhasfunction"				"(dllObject:dll, func:String)							:	Bool		",
	"dllunimport"					"(dllObject:dll)										:	Void		",
	"dllunimportdeltalib"			"(dllObject:dll)										:	Void		",

	// Objects (tables).
	"#group(tab)",
	"tabget"						"(t:Object,index:-{Nil,Undefined})						:	Any			",
	"tabset"						"(t:Object,index:-{Nil,Undefined},content:Any)			:	Void		",
	"tabnewattribute"				"(t:Object,id:String,set:Callable,get:Callable)			:	Void		",
	"tabredefineattribute"			"(t:Object,id:String,set:Callable,get:Callable)			:	Void		",
	"tabsetattribute"				"(t:Object,id:String,content:Any)						:	Void		",
	"tabgetattribute"				"(t:Object,id:String)									:	Any			",
	"tablength"						"(t:Object)												:	Number		",
	"tabindices"					"(t:Object)												:	Object		",
	"tabcopy"						"(t:Object)												:	Object		",
	"tabclear"						"(t:Object)												:	Void		",
	"tabenableoverloading"			"(t:Object)												:	Void		",
	"tabdisableoverloading"			"(t:Object)												:	Void		",
	"tabisoverloadingenabled"		"(t:Object)												:	Bool		",
	"tabsetmethod"					"(t1:Object,t2:Object,i:Any,j:Any)						:	Void		",
	"tabmethodonme"					"(t:Object,m:MethodFunc)								:	MethodFunc	",
	"tabextend"						"(dest:Object,src:Object)								:	Void		",
	"tabserialise"					"(src:Object)											:	outputbuffer",
	"tabdeserialise"				"(src:inputbuffer)										:	Object		",

	// Subobject tree inheritance.
	"#group(obj)",
	"inherit"						"(derived:+{Object,ExternId}, base:+{Object,ExternId})	:	Void		",
	"inheritredirect"				"(derived:+{Object,ExternId}, base:+{Object,ExternId})	:	Void		",
	"uninherit"						"(derived:+{Object,ExternId}, base:+{Object,ExternId})	:	Void		",
	"isderived"						"(derived:+{Object,ExternId}, base:+{Object,ExternId})	:	Bool		",
	"getbases"						"(obj:+{Object,ExternId})								:	Object		",
	"getbase"						"(obj:+{Object,ExternId}, i:Number)						:	Object		",
	"getderived"					"(obj:+{Object,ExternId})								:	Object		",		
	"getmostderived"				"(obj:+{Object,ExternId})								:	Object		",

	// Delegation inheritance.
	"#group(obj)",
	"delegate"						"(from:+{Object,ExternId}, to:+{Object,ExternId})		:	Void		",
	"undelegate"					"(from:+{Object,ExternId}, to:+{Object,ExternId})		:	Void		",			
	"isdelegate"					"(from:+{Object,ExternId}, to:+{Object,ExternId})		:	Bool		",			
	"isdirectdelegate"				"(from:+{Object,ExternId}, to:+{Object,ExternId})		:	Bool		",	
	"isdelegator"					"(to:+{Object,ExternId}, from:+{Object,ExternId})		:	Bool		",
	"getdelegates"					"(obj:+{Object,ExternId})								:	Object		",		
	"getdelegators"					"(obj:+{Object,ExternId})								:	Object		",		

	// String functions.
	"#group(str)",
	"strlen"						"(s:String)												:	Number		",
	"strslice"						"(s:String,...)											:	String		",
	"strsub"						"(s1:String,s2:String)									:	Number		",
	"strlower"						"(s:String)												:	String		",
	"strupper"						"(s:String)												:	String		",
	"strbyte"						"(s:String,i:Number)									:	Number		",
	"strbytestr"					"(c:Number)												:	String		",
	"strchar"						"(s:String,i:Number)									:	String		",
	"strisalpha"					"(c:Number)												:	Bool		",
	"strisalnum"					"(c:Number)												:	Bool		",
	"strisdigit"					"(c:Number)												:	Bool		",
	"strisspace"					"(c:Number)												:	Bool		",
	"strisprint"					"(c:Number)												:	Bool		",
	"strislower"					"(c:Number)												:	Bool		",
	"strisupper"					"(c:Number)												:	Bool		",
	"strisident"					"(s:String)												:	Bool		",
	"strisprefix"					"(s:String, prefix:String)								:	Bool		",
	"strrep"						"(s:String, n:Number)									:	String		",
	"strtonum"						"(s:String)												:	Number		",
	"strtokenise"					"(s:String, tokens:String)								:	list		",
	"strsavetofile"					"(s:String,path:String)									:	Bool		",

	// Math functions.
	"#group(math)",
	"sqr"							"(x:Number)												:	Number		",
	"sqrt"							"(x:Number)												:	Number		",
	"cos"							"(theta:Number)											:	Number		",
	"sin"							"(theta:Number)											:	Number		",
	"tan"							"(theta:Number)											:	Number		",
	"abs"							"(x:Number)												:	Number		",
	"max"							"(x1:Number,...)										:	Number		",
	"min"							"(x1:Number,...)										:	Number		",
	"random"						"(n:Number)												:	Number		",
	"randomise" 					"()														:	Void		",
	"power"							"(x:Number,exp:Number)									:	Number		",
	"pi"							"()														:	Number		",
	"fractionalpart"				"(x:Number)												:	Number		",
	"integerpart"					"(x:Number)												:	Number		",

	// File I/O.
	"#group(file)",
	"fileopen"						"(path:String,mode:String)								:	file		",
	"fileclose"						"(fp:file)												:	Void		",
	"filewrite"						"(fp:file,...)											:	Void		",
	"fileread"						"(fp:file,type:String)									:	Any			",
	"filegetline"					"(fp:file)												:	String		",
	"fileend"						"(fp:file)												:	Bool		",
	"fileremove"					"(path:String)											:	Void		",
	"filetmpname"					"()														:	String		",
	"fileexecute"					"(cmmd:String)											:	Void		",

	// Binary portable buffers.
	"#group(in)",
	"inputbuffer_new"				"(ob:outputbuffer)										:	inputbuffer	",
	"inputbuffer_set"				"(ib:inputbuffer,ob:outputbuffer)						:	Void		",
	"inputbuffer_rewind"			"(ib:inputbuffer)										:	Void		",
	"inputbuffer_size"				"(ib:inputbuffer)										:	Number		",
	"inputbuffer_remaining"			"(ib:inputbuffer)										:	Number		",
	"inputbuffer_eof"				"(ib:inputbuffer)										:	Bool		",

	"#group(out)",
	"outputbuffer_new"				"()														:	outputbuffer",
	"outputbuffer_append"			"(dest:outputbuffer,src:outputbuffer)					:	Void		",
	"outputbuffer_isempty"			"(ob:outputbuffer)										:	Bool		",
	"outputbuffer_size"				"(ob:outputbuffer)										:	Number		",
	"outputbuffer_totalpackets"		"(ob:outputbuffer)										:	Number		",	
	"outputbuffer_clear"			"(ob:outputbuffer)										:	Void		",
	"outputbuffer_flush"			"(ob:outputbuffer,fp:file)								:	Void		",

	// Binary portable readers / writers.
	"#group(reader)",
	"reader_fromfile"				"(fp:file)												:	reader		",		
	"reader_frominputbuffer"		"(ib:inputbuffer)										:	reader		",
	"reader_read_ui8"				"(r:reader)												:	Number		",
	"reader_read_ui16"				"(r:reader)												:	Number		",			
	"reader_read_i16"				"(r:reader)												:	Number		",			
	"reader_read_ui32"				"(r:reader)												:	Number		",			
	"reader_read_i32"				"(r:reader)												:	Number		",			
	"reader_read_bool8"				"(r:reader)												:	Bool		",			
	"reader_read_real64"			"(r:reader)												:	Number		",		
	"reader_read_string"			"(r:reader)												:	String		",		
	"reader_read_buffer"			"(r:reader,size:Number)									:	inputbuffer	",

	"#group(writer)",
	"writer_fromfile"				"(fp:file)												:	writer		",		
	"writer_fromoutputbuffer"		"(ob:outputbuffer)										:	writer		",
	"writer_write_ui8"				"(w:writer,val:Number)									:	Void		",				
	"writer_write_ui16"				"(w:writer,val:Number)									:	Void		",				
	"writer_write_i16"				"(w:writer,val:Number)									:	Void		",				
	"writer_write_ui32"				"(w:writer,val:Number)									:	Void		",				
	"writer_write_i32"				"(w:writer,val:Number)									:	Void		",				
	"writer_write_bool8"			"(w:writer,val:Bool)									:	Void		",			
	"writer_write_real64"			"(w:writer,val:Number)									:	Void		",			
	"writer_write_string"			"(w:writer,val:String)									:	Void		",			
	"writer_write_buffer"			"(w:writer,ob:outputbuffer)								:	Void		",

	// Portable sockets.
	"#group(socket)",
	"socket_createforclient"		"()														:	socket		",
	"socket_createforserver"		"(port:Number)											:	socket		",
	"socket_destroy"				"(sock:socket)											:	Void		",
	"socket_connecttoserver"		"(sock:socket,addr:String,port:Number,timeOut:Number)	:	Void		",
	"socket_acceptclient"			"(sock:socket,blocking:Bool,timeOut:Number)				:	socket	",
	"socket_isconnectionbroken"		"(sock:socket)											:	Bool		",
	"socket_ismessagepending"		"(sock:socket)											:	Bool		",
	"socket_waitanymessage"			"(sock:socket,timeOut:Number)							:	Void		",
	"socket_receive"				"(sock:socket)											:	inputbuffer	",
	"socket_blockreceive"			"(sock:socket)											:	inputbuffer	",
	"socket_send"					"(sock:socket, ob:outputbuffer)							:	Void		",

	// Shared memory.
	"#group(sh)",
	"shexists"						"(id:String)											:	Bool		",
	"shwrite"						"(id:String,val:Any)									:	Void		",
	"shread"						"(id:String)											:	Any			",
	"shdelete"						"(id:String)											:	Void		",
	"shobject"						"()														:	Object		",

	// Bit operators.
	"#group(bit)",
	"bitand"						"(x:Number,y:Number,...)								:	Number		",
	"bitor"							"(x:Number,y:Number,...)								:	Number		",
	"bitnot"						"(x:Number)												:	Number		",
	"bitxor"						"(x:Number,y:Number,...)								:	Number		",
	"bitshright"					"(x:Number,n:Number)									:	Number		",
	"bitshleft"						"(x:Number,n:Number)									:	Number		",

	// VM management.
	"#group(vm)",
	"vmget"							"(id:String)											:	vm			",
	"vmfuncs"						"(id:String)											:	Object		"
									"(vmInst:vm)											:	Object		",
	"vmcall"						"(id:String,f:String,...)								:	Any			"
									"(vmInst:vm,f:String,...)								:	Any			",
	"vmid"							"(vmInst:vm)											:	String		",
	"vmfuncaddr"					"(id:String,f:String)									:	ProgramFunc	"
									"(vmInst:vm,f:String)									:	ProgramFunc	",
	"vmthis"						"()														:	vm			",
	"vmload"						"(path:String,id:String)								:	vm			",
	"vmloadaddpath"					"(path:String)											:	Void		",
	"vmloadfrominputbuffer"			"(ib:inputbuffer,id:String)								:	vm			",	
	"vmloadfromreader"				"(r:reader,id:String)									:	vm			",				
	"vmrun"							"(vmInst:vm)											:	Void		",
	"vmunload"						"(vmInst:vm)											:	Void		",
	"vmof"							"(f:+{ProgramFunc,MethodFunc})							:	vm			",
	"vmcurrcall"					"()														:	Object		",
	"vmnextcall"					"(callTag:Number)										:	Object		",
	"vmgetstacktrace"				"()														:	String		",
	"vmisvalid"						"(vmInst:vm)											:	Bool		",
	"vmseterrorinvalidatesall"		"(flag:Bool)											:	Void		",
	"vmhaserror"					"(vmInst:vm)											:	Bool		",
	"vmhaserrorsomewhere"			"()														:	Bool		",
	"vmreseterror"					"(vmInst:vm)											:	Void		",
	"vmresetallerrors"				"()														:	Void		",
	"vmgeterrorreport"				"()														:	String		",

	// Dynamic compilation (reflection).
	"#group(comp)",
	"vmparse"						"(dsc:String,onError:Callable)							:	ast			",
	"vmparsestring"					"(code:String,onError:Callable)							:	ast			",
	"vmparsequotedelements"			"(code:String,onError:Callable)							:	ast			",
	"vmtranslate"					"(a:ast,dbc:String,onError:Callable,release:Bool)		:	Bool		",
	"vmtranslatetooutputbuffer"		"(a:ast,onError:Callable,release:Bool)					:	outputbuffer",
	"vmtranslateonwriter"			"(a:ast,w:writer,onError:Callable,release:Bool)			:	Bool		",
	"vmcomp"						"(dsc:String,dbc:String,onError:Callable,release:Bool)	:	Bool		",
	"vmcomptooutputbuffer"			"(dsc:String,onError:Callable,release:Bool)				:	outputbuffer",
	"vmcomponwriter"				"(dsc:String,w:writer,onError:Callable,release:Bool)	:	Bool		",
	"vmcompstring"					"(code:String,dbc:String,onError:Callable,release:Bool)	:	Bool		",
	"vmcompstringtooutputbuffer"	"(code:String,onError:Callable,release:Bool)			:	outputbuffer",
	"vmcompstringonwriter"			"(code:String,w:writer,onError:Callable,release:Bool)	:	Bool		",
	"vmextractbuilddeps"			"(code:String)											:	list		",

	// List manipulation.
	"#group(list)",
	"list_new"						"(...)													:	list		",
	"list_iterator"					"(l:list)												:	listiter	",
	"list_push_back"				"(l:list,val:Any,...)									:	Void		",
	"list_push_front"				"(l:list,val:Any,...)									:	Void		",
	"list_pop_back"					"(l:list)												:	Void		",
	"list_pop_front"				"(l:list)												:	Void		",
	"list_back"						"(l:list)												:	Any			",
	"list_front"					"(l:list)												:	Any			",
	"list_insert"					"(l:list, i:listiter,val:Any)							:	Void		",
	"list_remove"					"(l:list, val:Any)										:	Bool		",
	"list_erase"					"(l:list, i:listiter)									:	Void		",
	"list_clear"					"(l:list)												:	Void		",
	"list_total"					"(l:list)												:	Number		",

	"#group(list)",
	"listiter_new"					"()														:	listiter	",
	"listiter_setbegin"				"(i:listiter,l:list)									:	listiter	",
	"listiter_setend"				"(i:listiter,l:list)									:	listiter	",
	"listiter_checkend"				"(i:listiter,l:list)									:	Bool		",
	"listiter_checkbegin"			"(i:listiter,l:list)									:	Bool		",
	"listiter_equal"				"(i:listiter,j:list)									:	Bool		",
	"listiter_assign"				"(i_lvalue:listiter,j_rvalue:listiter)					:	Void		",
	"listiter_copy"					"(i:listiter)											:	listiter	",
	"listiter_fwd"					"(i:listiter)											:	listiter	",
	"listiter_bwd"					"(i:listiter)											:	listiter	",
	"listiter_getcounter"			"(i:listiter)											:	Number		",
	"listiter_getcontainer"			"(i:listiter)											:	list		",
	"listiter_getval"				"(i:listiter)											:	Any			",
	"listiter_setval"				"(i:listiter,val:Any)									:	Void		",

	// Table iterator.
	"#group(tab)",
	"tableiter_new"					"()														:	tableiter	",
	"tableiter_setbegin"			"(i:tableiter,t:Object)									:	tableiter	",
	"tableiter_setend"				"(i:tableiter,t:Object)									:	tableiter	",
	"tableiter_checkend"			"(i:tableiter,t:Object)									:	Bool		",
	"tableiter_checkbegin"			"(i:tableiter,t:Object)									:	Bool		",
	"tableiter_equal"				"(i:tableiter,j:tableiter)								:	Bool		",
	"tableiter_assign"				"(i_lvalue:tableiter,j_rvalue:tableiter)				:	Void		",
	"tableiter_copy"				"(i:tableiter)											:	tableiter	",
	"tableiter_fwd"					"(i:tableiter)											:	tableiter	",
	"tableiter_getcounter"			"(i:tableiter)											:	Number		",
	"tableiter_getcontainer"		"(i:tableiter)											:	Object		",
	"tableiter_getval"				"(i:tableiter)											:	Any			",
	"tableiter_setval"				"(i:tableiter,val:Any)									:	Void		",
	"tableiter_getindex"			"(i:tableiter)											:	Any			",

	// Vector manipulation.
	"#group(vector)",
	"vector_new"					"(n:Number)												:	vector		", 
	"vector_setval"					"(v:vector,i:Number,x:Any)								:	Void		",
	"vector_getval"					"(v:vector,i:Number)									:	Any			",
	"vector_push_front"				"(v:vector,x:Any,...)									:	Void		",
	"vector_push_back"				"(v:vector,x:Any,...)									:	Void		",
	"vector_pop_front"				"(v:vector)												:	Void		",
	"vector_pop_back"				"(v:vector)												:	Void		",
	"vector_front"					"(v:vector)												:	Any			",
	"vector_back"					"(v:vector)												:	Any			",
	"vector_resize"					"(v:vector, newSize:Number)								:	Void		",
	"vector_total"					"(v:vector)												:	Number		",
	"vector_iterator"				"(v:vector)												:	ExternId	",

	// Vector iterator.
	"#group(vector)",
	"vectoriter_new"				"()														:	vectoriter	",
	"vectoriter_setbegin"			"(i:vectoriter,v:vector)								:	vectoriter	",
	"vectoriter_setend"				"(i:vectoriter,v:vector)								:	vectoriter	",
	"vectoriter_checkend"			"(i:vectoriter,v:vector)								:	Bool		",
	"vectoriter_checkbegin"			"(i:vectoriter,v:vector)								:	Bool		",
	"vectoriter_equal"				"(i:vectoriter,j:vectoriter)							:	Bool		",
	"vectoriter_assign"				"(i_lvalue:vectoriter,j_rvalue:vectoriter)				:	Void		",
	"vectoriter_copy"				"(i:vectoriter)											:	vectoriter	",
	"vectoriter_fwd"				"(i:vectoriter)											:	vectoriter	",
	"vectoriter_bwd"				"(i:vectoriter)											:	vectoriter	",
	"vectoriter_getcounter"			"(i:vectoriter)											:	Number		",
	"vectoriter_getcontainer"		"(i:vectoriter)											:	vector		",
	"vectoriter_getval"				"(i:vectoriter)											:	Any			",
	"vectoriter_setval"				"(i:vectoriter,val:Any)									:	Any			",
	"vectoriter_getindex"			"(i:vectoriter)											:	Number		",

	// List manipulation.
	"#group(list)",
	"list_new"						"(...)													:	list		",
	"list_iterator"					"(l:list)												:	listiter	",
	"list_push_back"				"(l:list,val:Any,...)									:	Void		",
	"list_push_front"				"(l:list,val:Any,...)									:	Void		",
	"list_pop_back"					"(l:list)												:	Void		",
	"list_pop_front"				"(l:list)												:	Void		",
	"list_back"						"(l:list)												:	Any			",
	"list_front"					"(l:list)												:	Any			",
	"list_insert"					"(l:list, i:listiter,val:Any)							:	Void		",
	"list_remove"					"(l:list, val:Any)										:	Bool		",
	"list_erase"					"(l:list, i:listiter)									:	Void		",
	"list_clear"					"(l:list)												:	Void		",
	"list_total"					"(l:list)												:	Number		",

	// AST manipulation.
	"#group(ast)",
	"ast_new"						"(tag:String)											:	ast			"
									"(tag:String, attrs:Object)								:	ast			",
	"ast_get_tag"					"(a:ast)												:	String		",
	"ast_get_child"					"(a:ast, index:+{String, Number})						:	ast			",
	"ast_get_child_index"			"(a:ast, node:ast)										:	Object		",
	"ast_get_children"				"(a:ast)												:	list		",
	"ast_get_total_children"		"(a:ast)												:	Number		",
	"ast_get_parent"				"(a:ast)												:	ast			",
	"ast_is_descendant"				"(a:ast, node:ast)										:	Bool		",
	"ast_push_back"					"(a:ast, node:ast)										:	Void		"
									"(a:ast, node:ast, id:String)							:	Void		",
	"ast_push_front"				"(a:ast, node:ast)										:	Void		"
									"(a:ast, node:ast, id:String)							:	Void		",
	"ast_insert_after"				"(a:ast,after:+{ast,Number,String},node:ast)			:	Void		"
									"(a:ast,after:+{ast,Number,String},node:ast,id:String)	:	Void		",
	"ast_insert_before"				"(a:ast,before:+{ast,Number,String},node:ast)			:	Void		"
									"(a:ast,before:+{ast,Number,String},node:ast,id:String)	:	Void		",
	"ast_pop_back"					"(a:ast)												:	Void		",
	"ast_pop_front"					"(a:ast)												:	Void		",
	"ast_remove"					"(a:ast, indexOrNode:+{ast, Number, String})			:	Void		",
	"ast_remove_from_parent"		"(a:ast)												:	Void		",
	"ast_replace"					"(a:ast, old:ast, new:ast)								:	Void		",
	"ast_get_attribute"				"(a:ast, id:String)										:	Any			",
	"ast_set_attribute"				"(a:ast, id:String, val:Any)							:	Bool		",
	"ast_get_attributes"			"(a:ast)												:	Object		",
	"ast_accept_postorder"			"(a:ast, visitor:astvisitor)							:	Bool		",
	"ast_accept_preorder"			"(a:ast, visitor:astvisitor)							:	Bool		",
	"ast_copy"						"(a:ast)												:	ast			",
	"ast_unparse"					"(a:ast)												:	String		",
	"ast_inject"					"(a:ast, node:+{ast,Number,String,Bool,Nil})			:	ast			",
	"ast_decr_esc_cardinalities"	"(a:ast)												:	Void		",

	// AST traversal.
	"#group(ast)",
	"astvisitor_new"				"()															:	astvisitor	",
	"astvisitor_set_handler"		"(v:astvisitor, tag:String, f:Callable)						:	Void		",
	"astvisitor_set_context_handler""(v:astvisitor,parentTag:String,childId:String,f:Callable)	:	Void		",
	"astvisitor_set_default_handler""(v:astvisitor, f:Callable)									:	Void		",
	"astvisitor_stop"				"(v:astvisitor)												:	Void		",
	"astvisitor_leave"				"(v:astvisitor)												:	Void		",

	// Algorithms
	"#group(algo)",
	"find"							"(elem,cont)											:	Any			"
									"(elem,cont,end_iter)									:	Any			",
	"find_all"						"(elem,cont)											:	list		"
									"(elem,cont,end_iter)									:	list		",
	"find_if"						"(pred,cont)											:	Any			"
									"(pred,cont,end_iter)									:	Any			",
	"find_all_if"					"(pred,cont)											:	list		"
									"(pred,cont,end_iter)									:	list		",
	"apply"							"(action,cont)											:	Void		"
									"(action,cont,end_iter)									:	Void		",
	"remove"						"(elem,cont)											:	Any			"
									"(elem,cont,end_iter)									:	Any			"
									"(elem,cont,end_iter,eraser)							:	Any			",
	"remove_all"					"(elem,cont)											:	Void		"
									"(elem,cont,end_iter)									:	Void		"
									"(elem,cont,end_iter,eraser)							:	Void		",
	"remove_if"						"(pred,cont)											:	Any			"
									"(pred,cont,end_iter)									:	Any			"
									"(pred,cont,end_iter,eraser)							:	Any			",
	"remove_all_if"					"(pred,cont)											:	Void		"
									"(pred,cont,end_iter)									:	Void		"
									"(pred,cont,end_iter,eraser)							:	Void		",

	"@enter", DELTA_STDLIB_LIBS_NAMESPACE,
	"#group(vm)",
	"registercopied"				"(id:String, l:+{String, std::inputbuffer})				:	Void		",
	"registershared"				"(id:String, l:+{String, std::inputbuffer})				:	Void		",
	"unregister"					"(id:String)											:	Void		",
	"isregisteredcopied"			"(id:String)											:	Bool		",
	"isregisteredshared"			"(id:String)											:	Bool		",
	"import"						"(id:String)											:	std::vm		",
	"unimport"						"(vmInst:std::vm)										:	Void		",
	"@exit",
	
	DELTA_STDLIB_NAMESPACE_EXIT
	(const char*) 0
};

//////////////////////////////////////

POSSIBLY_UNUSED_STATIC_FUNCTION(UPTR(const char*) DeltaStdLib_FuncNames (void) )
static UPTR(const char*) DeltaStdLib_FuncNames (void) 
	{ return stdLibFuncNames; }

#endif	// Do not add stuff beyond this point.

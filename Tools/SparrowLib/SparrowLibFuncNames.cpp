/**
 *	SparrowLibFuncNames.cpp
 *
 *	-- Sparrow library functions for Delta --
 *
 *	Required header file defining library function
 *	names to the Delta compiler.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2007
 */
#include "SparrowLibFuncNames.h"

static const char* SparrowLibFuncNames[] = {
	"@enter", SPW_NAMESPACE,
	"print"							"(...)	:	String	",
	"installationdir"				"()		:	String	",

	"loadcomponent"					"(class:String)														:	Bool	",
	"createcomponent"				"(class:String) : Object (class:String, parent:+{Object, String})	:	Object	",
	"destroycomponent"				"(comp:Object)														:	Void	",
	"setparent"						"(comp:Object, parent:+{Object, String})							:	Void	",

	"thiscomponent"					"()														:	Object",
	"basecomponent"					"() : Object (comp:Object)								:	Object",
	"mostbasecomponent"				"() : Object (comp:Object)								:	Object",
	"getcomponent"					"(class:String) : Object (class:String, serial:Number)	:	Object",
	"getcomponents"					"()														:	Object",
	"basecreatewindow"				"(parent:ExternId)										:	ExternId",

	"#group(spwprop)",
	"getclassproperty"				"(class:String, propId:String)				:	Object	",
	"getinstanceproperty"			"(comp:+{Object, String}, propId:String)	:	Object	",
	"getproperty"					"(comp:+{Object, String}, propId:String)	:	Object	",
	"propertytablesetdirty"			"(class:String, dirty:Bool)					:	Bool	",
	"#group(_)",

	"call"							"(comp:+{Object, String}, func:String, ...)	:	Any	",
	"basecall"						"(comp:+{Object, String}, func:String, ...)	:	Any	",
	"setundo"						"(comp:+{Object, String}, func:String, ...)	:	Void",

	"settimer"						"(id:Number, time:Number, continuous:Bool, comp:+{Object, String}, func:String,...)	:	Number	",
	"canceltimer"					"(id:Number)																		:	Void	",

	"registercomponent"				"(class:String) : Void (class:String, baseClass:String)					:	Void	",
	"unregistercomponent"			"(class:String)															:	Void	",
	"setcomponentmetadata"			"(class:String, name:String, desc:String, author:String, version:String):	Void	",

	"#group(spwfunc)",
	"class_impl_static_function"			"(class:String, funcName:String, func:Callable, signature:String) : Void (class:String, funcName:String, func:Callable, signature:String, doc:String) : Void",
	"class_impl_member_function"			"(class:String, funcName:String, func:Callable, signature:String) : Void (class:String, funcName:String, func:Callable, signature:String, doc:String) : Void",
	"class_decl_required_member_function"	"(class:String, funcName:String, signature:String) : Void (class:String, funcName:String, signature:String, doc:String) : Void",
	"class_remove_static_function"			"(class:String, funcName:String) : Void",
	"class_remove_member_function"			"(class:String, funcName:String) : Void",
	"inst_impl_required_member_function"	"(class:String, funcName:String, func:Callable) : Void",
	"inst_impl_dynamic_member_function"		"(class:String, funcName:String, func:Callable, signature:String) : Void (class:String, funcName:String, func:Callable, signature:String, doc:String) : Void",
	"inst_remove_dynamic_member_function"	"(class:String, funcName:String) : Void",

	"#group(spwhandler)",
	"class_impl_static_handler"				"(class:String, signal:String, func:Callable) : Void (class:String, signal:String, func:Callable, doc:String) : Void",
	"class_impl_member_handler"				"(class:String, signal:String, func:Callable) : Void (class:String, signal:String, func:Callable, doc:String) : Void",
	"class_decl_required_member_handler"	"(class:String, signal:String) : Void (class:String, signal:String, doc:String) : Void",
	"class_remove_static_handler"			"(class:String, signal:String) : Void",
	"class_remove_member_handler"			"(class:String, signal:String) : Void",
	"inst_impl_required_member_handler"		"(class:String, signal:String, func:Callable) : Void",
	"inst_impl_dynamic_member_handler"		"(class:String, signal:String, func:Callable) : Void (class:String, signal:String, func:Callable, doc:String) : Void",
	"inst_remove_dynamic_member_handler"	"(class:String, signal:String) : Void",

	"#group(spwcmd)",
	"class_impl_static_command"				"(cmdDesc:Object, command:String, func:Callable) : Void (cmdDesc:Object, command:String, func:Callable, doc:String) : Void",
	"class_impl_member_command"				"(cmdDesc:Object, command:String, func:Callable) : Void (cmdDesc:Object, command:String, func:Callable, doc:String) : Void",
	"class_decl_required_member_command"	"(cmdDesc:Object, command:String) : Void (cmdDesc:Object, command:String, doc:String) : Void",
	"class_remove_static_command"			"(class:String, command:String) : Void",
	"class_remove_member_command"			"(class:String, command:String) : Void",
	"inst_impl_required_member_command"		"(class:String, funcName:String, func:Callable) : Void",
	"inst_impl_dynamic_member_command"		"(cmdDesc:Object, command:String, func:Callable) : Void (cmdDesc:Object, command:String, func:Callable, doc:String) : Void",
	"inst_remove_dynamic_member_command"	"(class:String, command:String) : Void",

	"#group(spwsig)",
	"class_decl_static_signal"	"(class:String, signal:String, arglist:String)	:	Void",
	"class_decl_member_signal"	"(class:String, signal:String, arglist:String)	:	Void",
	"class_signal"				"(signal:String, class:String, ...)				:	Void",
	"inst_signal"				"(signal:String, comp:Object, ...)				:	Void",

	"#group(spwprop)",
	"addclassproperty"			"(class:String, propId:String, prop:Object)				:	Void",
	"removeclassproperty"		"(class:String, propId:String)							:	Void",
	"addinstanceproperty"		"(comp:+{Object, String}, propId:String, prop:Object)	:	Void",
	"removeinstanceproperty"	"(comp:+{Object, String}, propId:String)				:	Void",
	"#group(_)",

	"registerimage"			"(image:String, path:String)	:	Void",
	"unregisterimage"		"(image:String)					:	Void",

	"@exit",
	(char*) 0
};

_SPARROW_LIB_C_API const char** LibraryFuncNames(void) { return SparrowLibFuncNames; }
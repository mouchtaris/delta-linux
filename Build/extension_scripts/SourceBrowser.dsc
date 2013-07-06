//////////////////////////////////////////////////////////////////////////
// SourceBrowser.dsc (2010).
// Circular component of the Sparrow IDE.
// Initial version: Irini Papakwnstantinou (Diploma project).
// Supervision, debugging, revised version and refactoring: Anthony Savidis
//////////////////////////////////////////////////////////////////////////
//
 
using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();
assert spw;

const logging				= false;
const nl 					= "\n";
const classId 				= "SourceBrowser";
sparrowDir 					= spw::installationdir() + "/";
window 						= nil;
treeData 					= [];
ui							= nil;
config						= nil;
editor						= nil;
functionNames 				= [];

classesHandler 				= 
globalVariablesHandler 		= 
functionsHandler 			= 
staticVariablesHandler 		= 
objectConstructorsHandler 	= nil;
functionNodeToData 			= [];	// func node 	-> func data
tableFunctionParents		= [];	// table node	-> func data

//====================================CONFIG========================================//
// Default values.

expandNestedFunctions 		= false;		
expandNamedFunctionsOnly	= true;
expandStaticVars			= false;
expandGlobalVars			= false;
expandObjectCtors			= false;
expandObjectMethods			= false;
expandObjectSlots			= false;

//====================================PROPERTIES====================================//

function SetProperties { 

	function CreateColor(r, g, b) 
				{ return [ @class : "wxColour", @r : r, @g : g, @b : b ]; }
	local blue 		= CreateColor(0, 0, 255);
	local black 	= CreateColor(0, 0, 0);
	local purple 	= CreateColor(160, 32, 240);

	spw::addclassproperty(	
		::classId,
		"globalsHeaderColor",		
		[
			{.class			: "ColorProperty"							},
			{.label			: "Global variables header color"			},
			{.description	: "Set the color of the Globals header"		},
			{.category		: "Header Colors"							},
			{.value			: blue										}
		]		
	);

	spw::addclassproperty(
		::classId,
		"globalsColor",
		[
			{.class			: "ColorProperty"							},
			{.label			: "Global variables color"					},
			{.description	: "Set the color of the Globals"			},
			{.category		: "Colors"									},
			{.value			: black										}
		]

	);

	spw::addclassproperty(	
		::classId,
		"staticsHeaderColor",		
		[
			{.class			: "ColorProperty"								},
			{.label			: "Statics variables header color"				},
			{.description	: "Set the color of the Static variables header"},
			{.category		: "Header Colors"								},
			{.value			: blue											}
		]
		
	);

	spw::addclassproperty(	
		::classId,
		"staticsColor",		
		[
			{.class			: "ColorProperty"								},
			{.label			: "Statics variables color"						},
			{.description	: "Set the color of the Statics variables"		},
			{.category		: "Colors"										},
			{.value			: black											}
		]
		
	);

	spw::addclassproperty(	
		::classId,
		"functionsHeaderColor",		
		[
			{.class			: "ColorProperty"									},
			{.label			: "Functions header color"							},
			{.description	: "Set the color of the Functions header"			},
			{.category		: "Header Colors"									},
			{.value			: blue												}
		]

	);

	spw::addclassproperty(	
		::classId,
		"functionsColor",		
		[
			{.class			: "ColorProperty"									},
			{.label			: "Functions color"									},
			{.description	: "Set the color of the Functions"					},
			{.category		: "Colors"											},
			{.value			: black												}
		]		
	);


	spw::addclassproperty(	
		::classId,
		"classesHeaderColor",		
		[
			{.class			: "ColorProperty"									},
			{.label			: "Classes header color"							},
			{.description	: "Set the color of the Classes header"				},
			{.category		: "Header Colors"									},
			{.value			: blue												}
		]
	);

	spw::addclassproperty(	
		::classId,
		"objectsHeaderColor",		
		[
			{.class			: "ColorProperty"									},
			{.label			: "Object constructors header color"				},
			{.description	: "Set the color of the Object constructors header"	},
			{.category		: "Header Colors"									},
			{.value			: blue												}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"classColor",		
		[
			{.class			: "ColorProperty"									},
			{.label			: "Class entries color"								},
			{.description	: "Set the color of the Class entries"				},
			{.category		: "Colors"											},
			{.value			: blue												}
		]
	);	

	spw::addclassproperty(	
		::classId,
		"objectsColor",		
		[
			{.class			: "ColorProperty"									},
			{.label			: "Object constructors color"						},
			{.description	: "Set the color of the Object constructors"		},
			{.category		: "Colors"											},
			{.value			: purple											}
		]
	);	

	spw::addclassproperty(	
		::classId,
		"objectMethodsColor",		
		[
			{.class			: "ColorProperty"										},
			{.label			: "Object methods color"								},
			{.description	: "Set the color of the Object methods"					},
			{.category		: "Header Colors"										},
			{.value			: blue													}
		]
	);	
	
	spw::addclassproperty(	
		::classId,
		"objectFunctionsColor",		
		[
			{.class			: "ColorProperty"								},
			{.label			: "Object functions color"						},
			{.description	: "Set the color of the Object functions"		},
			{.category		: "Colors"										},
			{.value			: blue											}
		]
	);	
		
	spw::addclassproperty(	
		::classId,
		"objectStrFieldsColor",		
		[
			{.class			: "ColorProperty"										},
			{.label			: "Objects' string indexed fields color"				},
			{.description	: "Set the color of the objects string indexed fields"	},
			{.category		: "Colors"												},
			{.value			: blue													}
		]		
	);	
		
	spw::addclassproperty(	
		::classId,
		"objectNumFieldsColor",		
		[
			{.class			: "ColorProperty"										},
			{.label			: "Objects' number indexed fields color"				},
			{.description	: "Set the color of the objects number indexed fields"	},
			{.category		: "Colors"												},
			{.value			: blue													}
		]		
	);	

	spw::addclassproperty(	
		::classId,
		"objectAttributesColor",		
		[
			{.class			: "ColorProperty"									},
			{.label			: "Objects attributes color"						},
			{.description	: "Set the color of the objects attributes"			},
			{.category		: "Colors"											},
			{.value			: blue												}
		]		
	);	
		
	spw::addclassproperty(	
		::classId,
		"dataColumnWidth",		
		[
			{.class			: "IntProperty"							},
			{.label			: "Width of the Data column"			},
			{.description	: "Set the width of the Data column"	},
			{.category		: "General"								},
			{.value			: 250									}
		]		
	);
		
	spw::addclassproperty(	
		::classId,
		"valueColumnWidth",		
		[
			{.class			: "IntProperty"							},
			{.label			: "Width of the Value column"			},
			{.description	: "Set the width of the Value column"	},
			{.category		: "General"								},
			{.value			: 150									}
		]			
	);
		

	spw::addclassproperty(	
		::classId,
		"maxValueLength",		
		[
			{.class			: "IntProperty"							},
			{.label			: "Maximum length of the values"		},
			{.description	: "Set the maximum length of the expressions presented in the Value column"},
			{.category		: "General"								},
			{.value			: 29									}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"globalIcon",		
		[
			{.class			: "FileProperty"						},
			{.label			: "Globals icon"						},
			{.description	: "Set the global variables icon"		},
			{.category		: "Icons"								},
			{.value			: sparrowDir + "resources/global.png"	}
		]		
	);
		
	spw::addclassproperty(	
		::classId,
		"objectlIcon",		
		[
			{.class			: "FileProperty"						},
			{.label			: "Objects icon"						},
			{.description	: "Set the objects icon"				},
			{.category		: "Icons"								},
			{.value			: sparrowDir + "resources/objectdef.png"}
		]			
	);
		
	spw::addclassproperty(	
		::classId,
		"methodIcon",		
		[
			{.class			: "FileProperty"							},
			{.label			: "Methods icon"							},
			{.description	: "Set the methods icon"					},
			{.category		: "Icons"									},
			{.value			: sparrowDir + "resources/method.png"		}
		]
	);
		
	spw::addclassproperty(	
		::classId,
		"fieldIcon",		
		[
			{.class			: "FileProperty"						},
			{.label			: "Object fields icon"					},
			{.description	: "Set the object fields icon"			},
			{.category		: "Icons"								},
			{.value			: sparrowDir + "resources/field.gif"	}
		]			
	);	
		
	spw::addclassproperty(	
		::classId,
		"staticIcon",		
		[
			{.class			: "FileProperty"						},
			{.label			: "Static variables icon"				},
			{.description	: "Set the static variables icon"		},
			{.category		: "Icons"								},
			{.value			: sparrowDir + "resources/static.gif"	}
		]			
	);		
		
		
	spw::addclassproperty(	
		::classId,
		"attributeIcon",		
		[
			{.class			: "FileProperty"							},
			{.label			: "Attributes icon"							},
			{.description	: "Set the attributes icon"					},
			{.category		: "Icons"									},
			{.value			: sparrowDir + "resources/attribute.gif"	}
		]			
	);		
		
	spw::addclassproperty(	
		::classId,
		"classKeyword",		
		[
			{.class			: "StringListProperty"								},
			{.label			: "Class keyword"									},
			{.description	: "Set the keyword(s) which when used as indices "
							  "their values describe the class of the object constructor." },
			{.category		: "Classes"											},
			{.value			: list_new("classId", "class") 						}																	
		]			
	);
		
	spw::addclassproperty(	
		::classId,
		"expandObjectCtors",		
		[
			{.class			: "BoolProperty"								},
			{.label			: "Object constructors"							},
			{.description	: "Select if object constructors are included"	},
			{.category		: "Expand"										},
			{.value			: false											}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"expandNestedFunctions",		
		[
			{.class			: "BoolProperty"								},
			{.label			: "Nested functions"							},
			{.description	: "Select if nested functions are  included"	},
			{.category		: "Expand"										},
			{.value			: false											}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"expandNamedFunctionsOnly",		
		[
			{.class			: "BoolProperty"								},
			{.label			: "Named functions only"						},
			{.description	: "Select if named functions only are included"	},
			{.category		: "Expand"										},
			{.value			: true											}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"expandStaticVars",		
		[
			{.class			: "BoolProperty"								},
			{.label			: "Static vars"									},
			{.description	: "Select if static vars are included"			},
			{.category		: "Expand"										},
			{.value			: false											}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"expandGlobalVars",		
		[
			{.class			: "BoolProperty"								},
			{.label			: "Global vars"									},
			{.description	: "Select if global vars are included"			},
			{.category		: "Expand"										},
			{.value			: false											}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"expandObjectMethods",		
		[
			{.class			: "BoolProperty"								},
			{.label			: "Object methods"								},
			{.description	: "Select if object methods are included"		},
			{.category		: "Expand"										},
			{.value			: true											}
		]			
	);

	spw::addclassproperty(	
		::classId,
		"expandObjectSlots",		
		[
			{.class			: "BoolProperty"								},
			{.label			: "Object slots"								},
			{.description	: "Select if object slots are included"			},
			{.category		: "Expand"										},
			{.value			: true											}
		]			
	);
}
	
function ConfigDataSingleton {
	if (isundefined(static configData))
		configData = [
			method Refresh { // Get from class properties
				self.globalsHeaderColor 	= spw::getproperty(::classId, "globalsHeaderColor").value;
				self.staticsHeaderColor 	= spw::getproperty(::classId, "staticsHeaderColor").value;
				self.functionsHeaderColor 	= spw::getproperty(::classId, "functionsHeaderColor").value;				
				self.objectsHeaderColor 	= spw::getproperty(::classId, "objectsHeaderColor").value;
				self.globalsColor 			= spw::getproperty(::classId, "globalsColor").value;
				self.staticsColor 			= spw::getproperty(::classId, "staticsColor").value;
				self.functionsColor 		= spw::getproperty(::classId, "functionsColor").value;
				self.objectsColor 			= spw::getproperty(::classId, "objectsColor").value;
				self.objectMethodsColor 	= spw::getproperty(::classId, "objectMethodsColor").value;
				self.objectFunctionsColor 	= spw::getproperty(::classId, "objectFunctionsColor").value;
				self.objectStrFieldsColor 	= spw::getproperty(::classId, "objectStrFieldsColor").value;
				self.objectNumFieldsColor 	= spw::getproperty(::classId, "objectNumFieldsColor").value;
				self.objectAttributesColor 	= spw::getproperty(::classId, "objectAttributesColor").value;
				self.valueColumnWidth 		= spw::getproperty(::classId, "valueColumnWidth").value;				
				self.dataColumnWidth 		= spw::getproperty(::classId, "dataColumnWidth").value;
				self.globalIcon 			= spw::getproperty(::classId, "globalIcon").value;
				self.objectlIcon 			= spw::getproperty(::classId, "objectlIcon").value;
				self.methodIcon 			= spw::getproperty(::classId, "methodIcon").value;
				self.fieldIcon 				= spw::getproperty(::classId, "fieldIcon").value;
				self.staticIcon 			= spw::getproperty(::classId, "staticIcon").value;
				self.attributeIcon 			= spw::getproperty(::classId, "attributeIcon").value;
				self.maxValueLength 		= spw::getproperty(::classId, "maxValueLength").value;
				self.classKeyword 			= spw::getproperty(::classId, "classKeyword").value;
				self.classesHeaderColor 	= spw::getproperty(::classId, "classesHeaderColor").value;
				self.classColor 			= spw::getproperty(::classId, "classColor").value;
				
				::expandObjectCtors			= spw::getproperty(::classId, "expandObjectCtors").value;
				::expandNestedFunctions		= spw::getproperty(::classId, "expandNestedFunctions").value;
				::expandNamedFunctionsOnly	= spw::getproperty(::classId, "expandNamedFunctionsOnly").value;
				::expandStaticVars			= spw::getproperty(::classId, "expandStaticVars").value;
				::expandGlobalVars			= spw::getproperty(::classId, "expandGlobalVars").value;
				::expandObjectMethods		= spw::getproperty(::classId, "expandObjectMethods").value;
				::expandObjectSlots			= spw::getproperty(::classId, "expandObjectSlots").value;
			}			
		];
	return configData;
}

//====================================GENERAL UTILITIES==========================================//

function spw_println(...) 
	{ if (logging) spw::print(..., nl); }

function strnocaselessthan (s1, s2) 
	{ return strlower(s1) < strlower(s2); }

function strisdecimal(s) {
	for (local i = 0, local n = strlen(s); n; ++i, --n)		
		if (not strisdigit(strbyte(s, i)))
			return false;
	return true;
}

//=========================================DATA==================================================//

function OrderedSequenceClass() {
	return [
		@class		: "OrderedSequenceClass",
		@list 		: list_new(),
		method Insert (d) {
			if (not d) 
				return nil;
			for (local i = listiter_new(), local l = self.list, i.setbegin(l); not i.checkend(l); i.fwd())
				if (d.Compare(d, i.getval()))
					{ l.insert(i, d); return self; }
			l.push_back(d);
			return self;
		}
	];
}

//--------------------------------------------------------------------------------------------------------//

function FunctionDataClass (name, args, linenumber) {
	return [
			{ #class			: "FunctionDataClass"		},
			{ #name 			: name						},
			{ #args 			: args						},
			{ #linenumber 		: linenumber				},
			{ #displaydata 		: name + args 				},
			{ #nestedFunctions 	: OrderedSequenceClass()	},
			method Compare(a, b) 
				{ return strnocaselessthan(a.displaydata, b.displaydata); }
		];
}

//--------------------------------------------------------------------------------------------------------//

function VariableDataClass(name, value, linenumber) { 
		return [
			{ #class		: "VariableDataClass"		},
			{ #name 		: name						},
			{ #value		: value						},			
			{ #linenumber 	: linenumber				},
			{ #displaydata	: name 						},
			method Compare(a, b) 
				{ return strnocaselessthan(a.name, b.name); }
		];
}

//--------------------------------------------------------------------------------------------------------//

function NumIndexedFieldDataClass(index, name, linenumber) { 		
		return [
			{ #class		: "NumIndexedFieldDataClass"	},
			{ #index 		: index							},	
			{ #name 		: name							},		
			{ #value		: name							},			
			{ #linenumber 	: linenumber					},
			{ #displaydata	: "[" + index + "]"				},
			method Compare(a, b) 
				{ return a.index < b.index; }		
		];
}

//--------------------------------------------------------------------------------------------------------//

function StrIndexedFieldDataClass(index, name, linenumber) {  	
		return [
			{ #class		: "StrIndexedFieldDataClass"},
			{ #index 		: index						},		
			{ #name 		: name						},		
			{ #value		: name						},			
			{ #linenumber 	: linenumber				},
			{ #displaydata	: index				},
			method Compare(a, b) 
				{ return strnocaselessthan(a.index, b.index); }		
		];
}

//--------------------------------------------------------------------------------------------------------//

function ObjectConstructorsDataClass(name, linenumber, handler) {
	return [
		{ #class		: "ObjectConstructorsDataClass"									},
		{ #name 		: name															},
		{ #linenumber 	: linenumber													},
		{ #handler 		: handler														},
		{ #nestedObjects: OrderedSequenceClass()										},
		{ #displaydata	: spw.components.Editor.GetLineFromPos(linenumber) + ":" + name	},
		
		method Compare (a, b) 
			{ return (a.linenumber < b.linenumber); },
			
		method SetDisplayData (name) 
			{ self.displaydata = spw.components.Editor.GetLineFromPos(self.linenumber) + ":" + name; }
	];
}

//--------------------------------------------------------------------------------------------------------//

function ClassesDataClass(name, linenumber) {
	return [
		{ #class		: "ClassesDataClass" 		},
		{ #name			: name						},
		{ #linenumber 	: linenumber				},
		{ #displaydata	: name						},
		method Compare(a, b) 
			{ return strnocaselessthan(a.name, b.name); }				
	];
}

//============================================UTILITY FUNCTIONS==================================================//

function GetNodeText (a) 
	{ return (a ?  spw.components.Editor.GetTextRange(a.range.left, a.range.right) : nil); }

function GetLineNumber(node) 
	{ return node.range.left;}

/////////////////////////////////////////////////////

function AddHandlerStandardSlots (handler) 
	{ handler.data  = OrderedSequenceClass(); return handler; }

/////////////////////////////////////////////////////

/////////////////////////////////////////////////////

function AddFunctionName (name)
	{ if (name) functionNames[name] = true; }

function IsFunctionName (name)
	{ return ::functionNames[name]; }
	
/////////////////////////////////////////////////////

function IsOutsideFunction (node) {
	while (node = node.parent)
		if (node.type == "FunctionASTNode")
			return false;
	return true;
}		

/////////////////////////////////////////////////////

function IsAnonymousFunction (node) {
	if (node.value == #lambda)
		return true;
	if ((local c = node.children) and (c[0].type == "FunctionNameASTNode"))
		return false;
	return true;
}

/////////////////////////////////////////////////////

function GetFuncNameAndArgsFromNode (node, inObjectCtor) {
	
	local id = "";
	local name = (node.value == "lambda" ? "lambda" : "anonymous " + node.value);
	
	local args = "";
	if (local c = node.children) {
		if ((firstchild = c[0]).type == "FunctionNameASTNode") {
			id = ::GetNodeText(firstchild);
			if (node.value == "function")
				name = id;
			else if (node.value == "method")
				name = " method " + id;
			else {
				assert node.value == "onevent";
				name = " onevent " + id;
			}
		}
		else 
		if (firstchild.type == "ArgListASTNode")
			args = " (" + ::GetNodeText(firstchild) + ")";
			
		if ((local secondchild = c[1]) and secondchild.type == "ArgListASTNode")
			args = " (" + ::GetNodeText(secondchild) + ")";
		if (node.value == "method" and not inObjectCtor)
			name = "orphan " + name;
	}
	
	return [ @name: name, @args: args, @id : id ];
}
 
/////////////////////////////////////////////////////

function GetObjectConstructorNodeFromElementNode (node) {
	function Up (node) {
		if (node.type == "TableConstructASTNode")
			return node;
		else
		if (node.type == "FunctionASTNode" or not node.parent)
			return nil;
		else
			return Up(node.parent);
	}
	return (node.parent ? Up(node.parent) : nil);
}

/////////////////////////////////////////////////////

function UnquoteSlotKey (s) {
	if (local len = strlen(s))
		if ((local c = strchar(s, 0)) == "\"") {
			if (strchar(s, len - 1) == "\"")
				return  strslice(s, 1, len - 2) ;
		}
		else
		if (c == "." or c == "#" or c == "@")
			return strslice (s, 1, len-1);
	return s;
}

//============================================HANDLERS==================================================//

function GetCommonMethods {
	if (isundefined(static methods))
		methods = [
			method GetValue (node){
					const elipses = "...";	
					if (local parent = node.parent)				
						if (parent.type == "BinaryOpASTNode" 	and
							parent.value == "=" 				and 
							parent.children 					and 
							parent.children[1])
							if (strlen(local value = GetNodeText(parent.children[1])) > config.maxValueLength)
								return strslice(value, 0, config.maxValueLength)+ elipses;
							else 
								return value;				
						else 
						if (parent.type == "UnaryKwdASTNode"	and
							parent = parent.parent				and 
							parent.type == "BinaryOpASTNode"	and 
							parent.value == "=" 				and
							parent.children[1])
							if (strlen(local value = GetNodeText(parent.children[1])) > config.maxValueLength) 
								return strslice(value, 0, config.maxValueLength)+ elipses;
							else
								return value;
					return "<initially undef>"; 
				}
		];
	return methods;
}

//--------------------------------------------------------------------------------------------------------//

function GlobalsHandlerClass {	// Global variables (main program).
	return AddHandlerStandardSlots(
		[
			@class			: "GlobalsHandlerClass",
			@vars			: [],
			
			method IsFunctionName (node)
				{ return ::IsFunctionName(::GetNodeText(node)); },
				
			method Operation (node){
				if (not @vars[local name = ::GetNodeText(node)]) {
					@vars[name] = true;
					return VariableDataClass(name, self.GetValue(node), ::GetLineNumber(node));
				}	
				else 
					return nil;
			},
				
			@GetValue : tabmethodonme(@self, ::GetCommonMethods().GetValue)							
		]
	);
}

//--------------------------------------------------------------------------------------------------------//

function StaticVarsHandlerClass {
	
	function GetNameFromNode (node)  {
		assert node;
		local name = spw.components.Editor.GetTextRange(node.range.left, node.range.right);
		return strslice(name, strlen("static "), strlen(name) - 1); 
	}		

	return AddHandlerStandardSlots(
		[
			@class : "StaticVarsHandlerClass",
			
			method Operation(node)
				{ return VariableDataClass(GetNameFromNode(node), self.GetValue(node), ::GetLineNumber(node)); },
				
			@GetValue : tabmethodonme(@self, ::GetCommonMethods().GetValue)
		]
	);
}

//--------------------------------------------------------------------------------------------------------//

function FunctionsHandlerClass {
	
	function IsToplevelFunction (node) {
		assert node.type == "FunctionASTNode";
		while (node = node.parent)
			if (node.type == "FunctionASTNode" or node.type == "TableConstructASTNode")
				return false;
		return true;
	}
	
	function NewFunction (node) {
		local f = ::GetFuncNameAndArgsFromNode(node, false);			
		::AddFunctionName(f.id);	// Register names
		return functionNodeToData[node] = ::FunctionDataClass(f.name, f.args, ::GetLineNumber(node));
	}
		
	local newHandler =  [
		
		@class 	: "FunctionsHandlerClass",
				
		method OperationNestedFunctions (node){ // Only top-level functions are handled this way.
			assert IsToplevelFunction(node);	
			self.IterateNestedFuncs(newFunctionData = NewFunction(node), node, false);
			return newFunctionData;
		},
		
		method OperationToplevelFunctions (node){ // Only top-level functions are handled this way.
			assert IsToplevelFunction(node);	
			return NewFunction(node);
		},
		
		@Operation : (	expandNestedFunctions 			? 
						@self.OperationNestedFunctions 	: 
						@self.OperationToplevelFunctions	),
		
		// This will cause an exhaustive iteration.
		method IterateNestedFuncs (parentFunctionData, node, addNode) {
			
			// Node not yet added, add it explicitly as child of parent.
			if (addNode and parentFunctionData) {
				assert GetObjectConstructorNodeFromElementNode(node).type == "TableConstructASTNode";
				local f = ::GetFuncNameAndArgsFromNode(node, true);
				local funcData = functionNodeToData[node] = ::FunctionDataClass(f.name, f.args, ::GetLineNumber(node));
				parentFunctionData.nestedFunctions.Insert(funcData);
			}
			
			if (not node or not node.children) 
				return;
			
			// When no parent data are supplied addNode must be true.
			if (not parentFunctionData) {
				assert addNode and GetObjectConstructorNodeFromElementNode(node).type == "TableConstructASTNode";
				local f = ::GetFuncNameAndArgsFromNode(node, true);
				parentFunctionData = functionNodeToData[node] = ::FunctionDataClass(f.name, f.args, ::GetLineNumber(node));
				self.data.Insert(parentFunctionData);
			}
			
			// Recursive exhaustive iteration.
			for (local i = 0, local n = node.children.size; i < n; ++i)
				if ((local child = node.children[i]).type == "FunctionASTNode") {					
					parentFunctionData.nestedFunctions.Insert(newFunctionData = NewFunction(child));
					self.IterateNestedFuncs(newFunctionData, child, false);
				}
				else 
				if (child.type == "TableConstructASTNode") {
					tableFunctionParents[child] = parentFunctionData; 
					objectConstructorsHandler.data.Insert(objectConstructorsHandler.Operation(child));
				}
				else
				if (child.type == "UnaryKwdASTNode" and child.value == "static")
					staticVariablesHandler.data.Insert(staticVariablesHandler.Operation(child));
				else
					self.IterateNestedFuncs(parentFunctionData, child, false);
		}
	];
	
	return ::AddHandlerStandardSlots(newHandler);
}

//--------------------------------------------------------------------------------------------------------//

function ElementsIteratorClass (node) {
	
	return [
		{ #class	: "ElementsIteratorClass"		},
		{ #node 	: node							},	
		{ #list 	: list_new()					},

		method CreateElementsList{
			
			local list = self.list;
			if (not self.node.children or not (tableElements = self.node.children[0])) 
				return list;
				
			local autoindex = 0;
			tableElements.processed = true;
			tableElements.parent = self.node;
			
			for (local i = 0, local n = tableElements.children.size; n; ++i, --n) {
				
				(local tableElement = tableElements.children[i]).parent = tableElements;
				local tableElementChildren = tableElement.children;
				
				for (local j = 0, local m = tableElementChildren.size; m; ++j, --m) { //for each of its children (elements)			

					(local elem = tableElementChildren[j]).parent = tableElement;
			
					if (elem.type == "FunctionASTNode" 	and 
						elem.value == "method" 			and 
						elem.children 					and 
						elem.children[0].type == "FunctionNameASTNode") { // method
						list.push_back( 
							[
								@index 		: ::GetNodeText(elem.children[0]),
								@value 		: elem,
								@isnumeric 	: false
							]
						);
					}
					else 
					if (elem.type == "ConstASTNode" and tableElementChildren[j+1]) 
						list.push_back( //str indexed @ case
						[
							@index		: ::UnquoteSlotKey(::GetNodeText(elem)),
							@value		: tableElementChildren[j+1],
							@isnumeric  : ::strisdecimal(GetNodeText(elem))
					
						]
					);							
					else
					if (tableElementChildren.size == 1)
						list.push_back( // auto indexed
							[ 
								@index 		: tostring(autoindex++),
								@value 		: elem,
								@isnumeric 	: true
							]
						);
					else 
					if (elem.type == "TableIndexListASTNode" and tableElementChildren[j+1].type == "ExpressionListASTNode") {
								
							local indexList		 			= tableElementChildren[j];
							local expressionList 			= tableElementChildren[j + 1];	
							local expressionListChildren 	= expressionList.children;
							local expressionIndex			= 0;
							
							for (local indicesIndex = 0, local k = indexList.children.size; k; ++indicesIndex, --k){
								
								if (indicesIndex < expressionListChildren.size)
									expressionIndex = indicesIndex;
								else 
									expressionIndex = expressionListChildren.size - 1;
								try 
									expressionListChildren[expressionIndex].parent = expressionList;
								trap e
									throw e;
								list.push_back( 
									[
										@index		: tostring(::UnquoteSlotKey(::GetNodeText(indexList.children[indicesIndex]))),
										@value 		: expressionListChildren[expressionIndex],
										@isnumeric 	: ::strisdecimal((::GetNodeText(indexList.children[indicesIndex])))
									]
								);
							}					
					}				
				}				
			}			
			return self.list;
		}
	];
}

//--------------------------------------------------------------------------------------------------------//

// Iterates internally within functions or methods.
function HandleFunctionOrMethodSlot (index, value, isnumeric, typeString){
	local funcData = nil;
	if (value.type == "UnaryOpASTNode" and value.children) {
		local f = ::GetFuncNameAndArgsFromNode(value.children[0], true);
		funcData = ::FunctionDataClass(typeString + " " + index, f.args, ::GetLineNumber(value));			
	}
	else {
		local f = ::GetFuncNameAndArgsFromNode(value, true);
		funcData = ::FunctionDataClass(f.name, f.args, ::GetLineNumber(value));					
	}
	
	// Handle nested functions or other items.
	assert funcData and GetObjectConstructorNodeFromElementNode(value).type == "TableConstructASTNode";	
	::functionsHandler.IterateNestedFuncs(tableFunctionParents[value.parent], value, true);
	return funcData;					
}
		
//////////////////////////////////////////////////////////////////

function ObjectMethodsHandlerClass {
	return ::AddHandlerStandardSlots( 
		[			
			@class : "ObjectMethodsHandlerClass",
			method Operation(index, value, isNumeric)
				{ return ::HandleFunctionOrMethodSlot(index, value, isNumeric, "method"); }
		]
	);
		
}

//////////////////////////////////////////////////////////////////

function ObjectFunctionsHandlerClass {
	return ::AddHandlerStandardSlots( 
		[			
			@class : "ObjectFunctionsHandlerClass",
			method Operation(index, value, isNumeric)
				{ return ::HandleFunctionOrMethodSlot(index, value, isNumeric, "function"); }
		]
	);
}

//--------------------------------------------------------------------------------------------------------//
// Applies recursive iteration for definitions appearing in
// slot expressions of object constructors.

function HandleNestedObjectConstructorItems (node) {
	
	local tableNode 	= ::GetObjectConstructorNodeFromElementNode(node);
	local tableParent 	= tableFunctionParents[tableNode];
	
	function Iterate (node, tableParent) {
		if (node.type == "FunctionASTNode")
			::functionsHandler.IterateNestedFuncs(tableParent, node, true);	
		else
		if (node.type == "TableConstructASTNode") {
			if (tableParent)
				::tableFunctionParents[node] = tableParent; 
			objectConstructorsHandler.data.Insert(objectConstructorsHandler.Operation(node));
		}
		else
		if (node.type == "UnaryKwdASTNode" and node.value == "static")
			staticVariablesHandler.data.Insert(staticVariablesHandler.Operation(node));
		else
		if (node.children)
			for (local i = 0, local c = node.children, n = c.size; i < n; ++i)
				c[i].parent = node, Iterate(c[i], tableParent);
	}
	
	Iterate(node, tableParent);
}

//////////////////////////////////////////////////////////////////

function GetSlotName (node) {
	const elipses = "...";
	if (node.type == "FunctionASTNode") {
		local f = ::GetFuncNameAndArgsFromNode(node, true);
		return f.name + f.args;
	}
	else 
	if (node.type == "NewAttributeASTNode")
		return ::GetNodeText(node.children[0]);
	else 
	if (strlen(local name = ::GetNodeText(node)) > config.maxValueLength)
		return strslice(name, 0, config.maxValueLength)+ elipses;
	else
		return name;
}

//////////////////////////////////////////////////////////////////

function ObjectNumIndexedFieldsHandlerClass {
	return ::AddHandlerStandardSlots(
		[			
			@class : "ObjectNumIndexedFieldsHandlerClass",
			method Operation(index, value, isnumeric) {
				::HandleNestedObjectConstructorItems(value);
				return NumIndexedFieldDataClass(strtonum(index), ::GetSlotName(value), ::GetLineNumber(value));
			}
		]
	);
}

//////////////////////////////////////////////////////////////////

function ObjectStrIndexedFieldsHandlerClass {
	return ::AddHandlerStandardSlots(
		[			
			@class : "ObjectStrIndexedFieldsHandlerClass",
			method Operation(index, value, isnumeric) {
				::HandleNestedObjectConstructorItems(value);			
				return StrIndexedFieldDataClass(index, ::GetSlotName(value), ::GetLineNumber(value));
			}
		]
	);
}

/////////////////////////////////////////////////////////////////

function GetObjectElementHandler (node, dispatcher) {
	
	if (node.isnumeric) {
		if (::expandObjectSlots)
			return dispatcher.numIndexedFieldsHandler;
		return nil;
	}
	
	local value = node.value;
	
	if ((local type = node.type) == "FunctionASTNode")
		if (value.value == "function" or value.value == "lambda") {
			if (not ::expandNestedFunctions)
				return nil;
			if (::expandNamedFunctionsOnly and IsAnonymousFunction(node))
				return nil;
			return dispatcher.functionsHandler;
		}
		else {
			 assert value.value == "method"; 
			 if (::expandObjectMethods)
				 return dispatcher.methodsHandler;
			return nil;
		}
	else {
		if (type == "UnaryOpASTNode" 		and 
			value.value == "()" 			and 
			(local c = value.children[0]) 	and 
			c.type == "FunctionASTNode" 	and 
			strisident(node.index))
			
			if (c.value == "method") {
				if (::expandObjectMethods)
					return dispatcher.methodsHandler;
				return nil;
			}
			else
			if (c.value == "function") {
				if (not ::expandNestedFunctions)
					return nil;
				if (::expandNamedFunctionsOnly and IsAnonymousFunction(c))
					return nil;
				return dispatcher.functionsHandler;
			}
			
		if (::expandObjectSlots)
			return dispatcher.strIndexedFieldsHandler;
		return nil;
	}
}

//--------------------------------------------------------------------------------------------------------//

function ClassesHandlerClass {
	
	function GetLineNumber (node)
		{ return node.parent.parent.parent.range.left; }
			
	function ValueIsValid(v) {
		return	strisident(::UnquoteSlotKey(::GetNodeText(v))) and 
				( (v.type == "ConstASTNode" and v.value == "CONST_STRING") or 
				   v.type == "StringifiedIdASTNode" );
	}
	
	function IsOneOfTheClassKeywords(s) {
		foreach (local kwd, config.classKeyword)
			if (s == kwd)
				return true;
		return false;	
	}

	return ::AddHandlerStandardSlots(
		[
			@class : "ClassesHandlerClass",
			
			method Predicate(index, value, isnumeric) 
				{ return IsOneOfTheClassKeywords(index) and ValueIsValid(value); },
			
			method Operation(index, value, isnumeric)
				 { return ClassesDataClass(::UnquoteSlotKey(::GetNodeText(value)), ::GetLineNumber(value)); }
		]
	);
}

//--------------------------------------------------------------------------------------------------------//
// Single constructor.

function ObjectConstructorHandlerClass (classesHandler) {
		
	return [
		@class						: "ObjectConstructorHandlerClass"			,			
		@methodsHandler 			: ObjectMethodsHandlerClass()				,
		@functionsHandler			: ObjectFunctionsHandlerClass()				,
		@numIndexedFieldsHandler	: ObjectNumIndexedFieldsHandlerClass()		,			
		@strIndexedFieldsHandler	: ObjectStrIndexedFieldsHandlerClass()		,	
		@classesHandler				: classesHandler							,
		
		method Handle (node) {			

			foreach (local e, local l = ElementsIteratorClass(node).CreateElementsList()) {
			
				if (local handler =	::GetObjectElementHandler(e, self))
					handler.data.Insert(handler.Operation(e.index, e.value, e.isnumeric));
				
				// Special extra case.					
				if (@classesHandler.Predicate(e.index, e.value, e.isnumeric))
					@classesHandler.data.Insert(
						@classesHandler.Operation(e.index, e.value, e.isnumeric)
					);
			}
		}
	];
}

//--------------------------------------------------------------------------------------------------------//
// All constructors

function ObjectConstructorsHandlerClass (classesHandler) {

	function GetParentFunctionName (node){
		while (node = node.parent)
			if (node.type == "FunctionASTNode")
				if (node.children[0].type == "FunctionNameASTNode")
					return ::GetNodeText(node.children[0]);
				else 
					return "anonymous function";
		assert false;
		return nil;
	}

	function GetNameFromNode (node)  {
		if (local parent = node.parent)
			if ( parent.type == "BinaryOpASTNode")
				return "Object assigned to '" + ::GetNodeText(parent.children[0]) + "'";
			else 
			if (parent.parent and parent.type == "ExpressionListASTNode" and parent.parent.type == "CallASTNode")
				return "Object argument to '" + ::GetNodeText(parent.parent.children[0]) + "()'";
			else 
			if (parent.type  == "UnaryKwdASTNode" and parent.value == "return") 
				return "Object returned by '" + GetParentFunctionName(node) + "()'";
		return "Object";
	}		
		
	local newHandler =  [
		{ #class			: "ObjectConstructorsHandlerClass" 	},	
		{ #classesHandler 	: classesHandler					},
				
		method Operation(node){	
			(local h = ::ObjectConstructorHandlerClass(self.classesHandler)).Handle(node);
			local newObject = ::ObjectConstructorsDataClass(GetNameFromNode(node), ::GetLineNumber(node), h);		
			self.IterateNestedObjects(newObject, node);
			return newObject;
		},
		
		method IterateNestedObjects (newObjectConstr, node) {
			foreach (local e, ElementsIteratorClass(node).CreateElementsList())
				if (e.value.type == "TableConstructASTNode") {
					local evenNewerObject = self.Operation(e.value);
					local openingbracket  = local closingbracket = "'";
					if (e.isnumeric)
						openingbracket = "[", closingbracket = "]";
					evenNewerObject.SetDisplayData(
						"Object as slot with index " 	+ 
						openingbracket 					+ 
						e.index 						+ 
						closingbracket
					);
					newObjectConstr.nestedObjects.Insert(evenNewerObject);					
				}
		}
	];
	
	return ::AddHandlerStandardSlots(newHandler);
}

//======================================VISUALIZERS==================================================//

function VisualizerBasicFunction (handle, visualizerFunc, handler) {
	foreach(local d, handler.data.list)
		treeData[visualizerFunc(handle, d.displaydata, d.value)] = [ @linenumber : d.linenumber];
}

function VisualizerBasic {
	return [
		@class: "VisualizerBasic",
		method Visualize (handle, visualizerFunc, handler) 
			{ VisualizerBasicFunction(handle, visualizerFunc, handler); }
	];
}

//--------------------------------------------------------------------------------------------------------//

function FunctionsVisualizer {
		
	function VisualizeNested (handle, visualizerFunc, nestedFuncs) {
		foreach (local d, nestedFuncs.list) {
			local new_handle = visualizerFunc(handle, d.displaydata, d.value);
			treeData[new_handle] = [ @linenumber : d.linenumber ];	
			VisualizeNested(new_handle, visualizerFunc, d.nestedFunctions);		
		}
	}
	
	return [
		@class : "FunctionsVisualizer",
		method Visualize(handle, visualizerFunc, handler) 
			{ VisualizeNested(handle, visualizerFunc, handler.data); }
	];
}

//--------------------------------------------------------------------------------------------------------//

function ObjectConstructorsVisualizer {
	
	static methodsVisFunc;
	static functionsVisFunc;
	static numIndexedFieldsVisFunc;
	static strIndexedFieldsVisFunc;
	
	function VisualizeNested (handle, visualizerFunc, nestedObjects) {
		
		foreach (local d, nestedObjects.list) {
			
			local new_handle  = visualizerFunc(handle, d.displaydata);
			local new_handler = d.handler;
			treeData[new_handle] = [ @linenumber : d.linenumber ];
			
			VisualizerBasicFunction(new_handle, methodsVisFunc, 			new_handler.methodsHandler);
			VisualizerBasicFunction(new_handle, functionsVisFunc, 			new_handler.functionsHandler);
			VisualizerBasicFunction(new_handle, numIndexedFieldsVisFunc, 	new_handler.numIndexedFieldsHandler);
			VisualizerBasicFunction(new_handle, strIndexedFieldsVisFunc, 	new_handler.strIndexedFieldsHandler);				
			
			VisualizeNested(new_handle, visualizerFunc, d.nestedObjects);		
		}
	}
	
	return [
		
		@class : "ObjectConstructorsVisualizer",					
		
		method SetVisualizationFunctions (
			methodsVis, 
			functionsVis, 
			numIndexedFieldsVis, 
			strIndexedFieldsVis
		) {
			methodsVisFunc 			= methodsVis;
			functionsVisFunc 		= functionsVis;
			numIndexedFieldsVisFunc = numIndexedFieldsVis;
			strIndexedFieldsVisFunc = strIndexedFieldsVis;
		},
		
		method Visualize(handle, visualizerFunc, handler)
			{ VisualizeNested(handle, visualizerFunc, handler.data); }
	];
}

//========================================UI==================================================//

function UIClass{
	return[
		{ #class	: "UIClass" },
		method AppendItem(id, value){
			return window.Append(id, list_new(value));
		},
		method VisualizeGlobalVariable(handle, data, value){
			newid = window.Append(handle, list_new(data, value));
			window.SetTextColor(newid, config.globalsColor);
			window.SetImage(newid, 0, "global_icon");
			return newid;
		},
		method VisualizeClass(handle, data, value){
			newid = self.AppendItem(handle, data);
			window.SetTextColor(newid, config.classColor);
			window.SetImage(newid, 0, "objectdef_icon");
			return newid;
		},
		method VisualizeStaticVariable(handle, data, value){
			newid = window.Append(handle, list_new(data, value));
			window.SetTextColor(newid, config.staticsColor);
			window.SetImage(newid, 0, "static_icon");
			return newid;
		},
		method VisualizeAttribute(handle, data, value){
			newid = self.AppendItem(handle, data);
			window.SetImage(newid, 0, "attribute_icon");
			return newid;
		},
		method VisualizeFunction(handle, data, value){
			newid = self.AppendItem(handle, data);
			window.SetTextColor(newid, config.functionsColor);
			window.SetImage(newid, 0, "method_icon");
			return newid;
		},
		method VisualizeObjectConstror(handle, data, value){
			newid = self.AppendItem(handle, data);
			window.SetTextColor(newid, config.objectsColor);
			window.SetImage(newid, 0, "objectdef_icon");
			return newid;
		},		
		method VisualizeObjMethod(handle, data, value){
			newid = self.AppendItem(handle, data);
			window.SetImage(newid, 0, "method_icon");
			return newid;
		},
		method VisualizeObjFunc(handle, data, value){
			newid = self.AppendItem(handle, data);
			window.SetImage(newid, 0, "method_icon");
			return newid;
		},
		method VisualizeObjNumField(handle, data, value){
			newid = window.Append(handle, list_new(data, value));
			window.SetTextColor(newid, config.objectNumFieldsColor);	
			window.SetImage(newid, 0, "field_icon");	
			return newid;
		},
		method VisualizeObjStrField(handle, data, value){
			newid = window.Append(handle, list_new(data, value));
			window.SetTextColor(newid, config.objectStrFieldsColor);	
			window.SetImage(newid, 0, "field_icon");	
			return newid;
		},
		method AppendRoot(){
			self.root = self.AppendItem(0,"Source Browser");
		},
		method CreateVisualGroups{
			self.globalVarNode = self.AppendItem(self.root, "Global Variables");
			self.staticVarNode = self.AppendItem(self.root, "Static Variables");
			self.functionsNode = self.AppendItem(self.root, "Functions");
			self.classesNode = self.AppendItem(self.root, "Classes");
			self.objConstrNode = self.AppendItem(self.root, "Object Constructors");
			window.SetTextColor(self.globalVarNode, config.globalsHeaderColor);
			window.SetTextColor(self.staticVarNode, config.staticsHeaderColor);
			window.SetTextColor(self.functionsNode, config.functionsHeaderColor);
			window.SetTextColor(self.objConstrNode, config.objectsHeaderColor);
			window.SetTextColor(self.classesNode, config.classesHeaderColor);
		},		
		method ExpandGroups{
			local mainHandles = list_new(self.root, self.globalVarNode, self.staticVarNode,
				self.functionsNode, self.objConstrNode, self.classesNode);
			foreach(h, mainHandles)
				window.Expand(h);
			
			foreach(local objectHandle, window.GetChildren(self.objConstrNode, false))
				window.Expand(objectHandle);
			foreach(h, mainHandles)
				self.RemoveEmptyParent(h);
		},
		method RemoveEmptyParent(handle) {
			if (window.GetTotalChildren(handle, false) == 0) window.Remove(handle);
		},
		method Init(parent){
			local nativeWindow = spw::basecreatewindow(parent);
			::window = spw.decorate(spw::thiscomponent());
			window.SetTitle("Source Browser");
			window.SetRootVisibility(false);						
			window.SetColumns(list_new("Data:"+config.dataColumnWidth, "Value:"+config.valueColumnWidth));
			local images = list_new("global_icon", "objectdef_icon", "method_icon", "field_icon", "static_icon", "attribute_icon");
			window.SetImages(images);
			self.AppendRoot();
			self.CreateVisualGroups();
			return nativeWindow;
		},
		method Clear{
			window.Clear();
			self.AppendRoot();
			self.CreateVisualGroups();
		}
	];
}

//--------------------------------------------------------------------------------------------------------//

function IterateAndDispatch (
			globalVariablesHandler, 
			staticVariablesHandler,
			functionsHandler,
			objectConstructorsHandler
		) {
	const IgnoreHandlerException = 0;
	
	return [
		
		@globalVariablesHandler		:	globalVariablesHandler,
		@staticVariablesHandler		:	staticVariablesHandler,
		@functionsHandler			:	functionsHandler,
		@objectConstructorsHandler	: 	objectConstructorsHandler,
		
		method GetBasicHandler (node, dispatcher) {
			if ((local type = node.type) == "TableConstructASTNode") {
				if (::expandObjectCtors)
					return @objectConstructorsHandler;
				throw IgnoreHandlerException;
			}
			else
			if (type == "FunctionASTNode") {
				if (::expandNamedFunctionsOnly and IsAnonymousFunction(node))
					throw IgnoreHandlerException;
				assert node.value == "function" or node.value == "onevent" or node.value == "lambda" or node.value== "method";
				return @functionsHandler;
			}
			else
			if (type == "UnaryKwdASTNode") {
				if (::expandStaticVars and node.value == "static")
					return @staticVariablesHandler;
				throw IgnoreHandlerException;
			}
			else
			if (type == "VariableASTNode") {
				if (::expandGlobalVars and not dispatcher.globalVariablesHandler.IsFunctionName(node) and ::IsOutsideFunction(node))
					return @globalVariablesHandler;
				throw IgnoreHandlerException;
			}
			else
				return nil;
		},

		method Handle (tree) {
			if (tree and tree.children)
				for (local i = 0, n = tree.children.size; n; ++i, --n) {
					(local node = tree.children[i]).parent = tree;
					try 
						if (local handler =	@GetBasicHandler(node, self))
							handler.data.Insert(handler.Operation(node));
						else
							@Handle(node);	// Try recursively if unhandled.
					trap e
						{ assert e == IgnoreHandlerException; }
				}
		}
	];
}

//---------------------------------------------------------------------------------------------//

function IsDeltaSourceFile (name) {
	return	name								and 								
			(local len = strlen(name)) > 3 		and 
			strslice(name, len - 4, len - 1) == ".dsc";
}

//--------------------------------------------------------------------------------------------------------//

function Visualize (handlers) {
	
	local ts = currenttime();
		
	::ui.Clear();
	::treeData = [];
	local visualizerBasic 		= VisualizerBasic();
	local functionsVisualizer 	= FunctionsVisualizer();
	local objCtorsVisualizer 	= ObjectConstructorsVisualizer();
	
	visualizerBasic.Visualize		(ui.globalVarNode, ui.VisualizeGlobalVariable, 	handlers[0]); 	//globalVariablesHandler
	visualizerBasic.Visualize		(ui.staticVarNode, ui.VisualizeStaticVariable, 	handlers[1] ); 	//staticVariablesHandler
	visualizerBasic.Visualize		(ui.classesNode,   ui.VisualizeClass, 			handlers[2]); 	//classesHandler
	functionsVisualizer.Visualize	(ui.functionsNode, ui.VisualizeFunction, 		handlers[3]); 	//functionsHandler	
	
	objCtorsVisualizer.SetVisualizationFunctions(
		ui.VisualizeObjMethod, 
		ui.VisualizeObjFunc, 
		ui.VisualizeObjNumField, 
		ui.VisualizeObjStrField
	);
	objCtorsVisualizer.Visualize	(ui.objConstrNode, 	ui.VisualizeObjectConstror, handlers[4]); 	//objectConstructorsHandler

	::ui.ExpandGroups();
	
	spw_println("Visualization time = ", currenttime() - ts);	
}

//--------------------------------------------------------------------------------------------------------//

function DataCache {
	if (isundefined(static cache))
		cache = [
			
			@class					: #DataCache,
			@previouslyOpenedfiles 	: [],
			
			method FileIsCachedAndDidntChange (uri, currentText){
				if (self.previouslyOpenedfiles[uri] 		and 
					self.previouslyOpenedfiles[uri].text 	and 
					self.previouslyOpenedfiles[uri].text == currentText) 	
					return true;
				return false;
			},
			
			method SaveInCache (uri, currentText, handlers){
				self.previouslyOpenedfiles[uri] = [
					@text 		: currentText, 
					@handlers 	: handlers
				];
			}		
		];
		
	return cache;
}

//--------------------------------------------------------------------------------------------------------//

function Browse(uri, forced){
		
	local currentText = spw.components.Editor.GetText();
	if (not forced and DataCache().FileIsCachedAndDidntChange(uri, currentText))	// Visualize using existing handlers
		Visualize(DataCache().previouslyOpenedfiles[uri].handlers);
	else {		
		// Instantiate handlers
		::classesHandler 			= ClassesHandlerClass();
		::globalVariablesHandler 	= GlobalsHandlerClass();
		::functionsHandler 			= FunctionsHandlerClass();
		::staticVariablesHandler 	= StaticVarsHandlerClass();
		::objectConstructorsHandler = ObjectConstructorsHandlerClass(classesHandler);

		local ts = local gt = currenttime();
		
		// Get source AST
		if (spw.components.EditorManager.serial == 0)	// ensure visibility of EditorManager
			spw.components.Shell.AddComponent("EditorManager", 6);
		::editor 	 = spw.components.EditorManager.GetFocusedEditor();
		
		// Call deprecated as it is too slow.
		// local ASTree = spw.components.Editor.GetAST();
		
		spw.components.Editor.SharedWriteAST();
		(local ASTree = std::shread("ast")), std::shdelete("ast");
		
		::spw_println("AST extraction time = ", currenttime() - ts);
		ts = currenttime();
		
		if (ASTree) {
					
			IterateAndDispatch(
				globalVariablesHandler,
				staticVariablesHandler,
				functionsHandler,
				objectConstructorsHandler
			).Handle(ASTree);
					
			Visualize(
				[	
					globalVariablesHandler, 
					staticVariablesHandler, 
					classesHandler, 
					functionsHandler, 
					objectConstructorsHandler
				]
			);

			DataCache().SaveInCache(	//create entry and save text and handlers
				uri, 
				currentText, 
				[
					globalVariablesHandler, 
					staticVariablesHandler, 
					classesHandler, 
					functionsHandler, 
					objectConstructorsHandler
				]
			);
		}
		
		spw_println(
			"Iteration, visualization and caching time = ", currenttime() - ts, nl,
			"Total processing time = ", currenttime() - gt, nl
		);
	}
	
}

//--------------------------------------------------------------------------------------------------------//

function RegisterImages {
	spw::registerimage("global_icon",  		config.globalIcon);
	spw::registerimage("objectdef_icon",  	config.objectlIcon);
	spw::registerimage("method_icon",  		config.methodIcon); 
	spw::registerimage("field_icon",  		config.fieldIcon); 
	spw::registerimage("static_icon",  		config.staticIcon); 
	spw::registerimage("attribute_icon",  	config.attributeIcon); 
}

//-----------------------------------------MAIN-----------------------------------------------------------//

onevent  ComponentAppliedChangedProperties(old , changed) {
	if (config)  {
		config.Refresh();
		if ((local em = spw.components.EditorManager) 	and 
			em.serial									and
			(local e = em.GetFocusedEditor())			and
			e.serial)
			Browse(e.GetURI(), true);
	}
}

function Signals {
	if (isundefined(static signals))
		signals = [
			@class : #Signals,
			
			@editorChanged : onevent (invoker, unused) {
				if (config) {
					local focusEditor = spw.components.EditorManager.GetFocusedEditor();
					if (IsDeltaSourceFile(local uri = focusEditor.GetURI()))
						{ ::editor = focusEditor; Browse(uri, false); }
				}
			},
			
			@fileOpened : onevent (fileEditor, uri) {
				if (config and fileEditor.class_id == "Editor" and IsDeltaSourceFile(uri))
					{ ::editor = fileEditor; Browse(uri, false); }
			},
			
			@itemActivated : onevent (id) {
				if (config) {
					if (spw.components.EditorManager.serial == 0)	
						spw.components.Shell.AddComponent("EditorManager", 6);								
					spw.components.Shell.FocusComponent(spw.components.EditorManager);
					if (editor and treeData[id]) {
						spw.components.EditorManager.FocusEditor(editor);
						spw.components.Editor.EnsureLineVisible(treeData[id].linenumber);			
						spw.components.Editor.GotoPos(treeData[id].linenumber);
					}
				}
			}
			
		];
	return signals;
}

//--------------------------------------------------------------------------------------------------------//

function CommandsClass {
	if (isundefined(static commands))
		commands = [
			@Configure	: 	function  { spw.components.Shell.ConfigureComponent(classId); },
			@Refresh 	: 	function {
								local editor 		= spw.components.EditorManager.GetFocusedEditor();
								local uri 	 		= editor.GetURI();
								local currentText 	= spw.components.Editor.GetText();
								if (not DataCache().FileIsCachedAndDidntChange(uri, currentText)) 
									Browse(uri, false);
							}
		];
	return commands;
}

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad {
	spw::registercomponent(classId, "TreeListViewComponent");

	spw::setcomponentmetadata(
		classId, 
		"Source Browser", 
		"A utility for source code browsing",
		"Irene Papakonstantinou, Anthony Savidis", 
		"1.0"
	); 
	spw::registerimage("refresh",  sparrowDir + "resources/refresh.png");

	spw::class_decl_required_member_function(classId, "OnItemActivated", "void (uint serial)");
	spw::class_decl_required_member_handler(classId, "FocusedEditorChanged");
	spw::class_decl_required_member_handler(classId, "FileOpened");
	
	spw::class_decl_required_member_command(
		[
			{.class			: "UserCommandDesc"			},
			{.class_id		: classId					},
			{.function_name	: "ConfigureSourceBrowser"	},
			{.flags			: 1							},
			{.isStatic		: false						}
		],
		"/{2}Configure/Source Browser...",
		"Configure the SourceBrowser"
	);
	
	spw::class_decl_required_member_command(
		[
			{.class			: "UserCommandDesc"		},
			{.class_id		: classId				},
			{.function_name	: "RefreshSourceBrowser"},
			{.flags			: 7						},
			{.image			: "refresh"				},
			{.isStatic		: false					}
		],
		"/{200}Tools/{200}Refresh the Source Browser",
		"Refresh the Source Browser"
	);
	
	SetProperties();
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor {
	spw::inst_impl_required_member_function(classId, "OnItemActivated", Signals().itemActivated);
	spw::inst_impl_required_member_handler(classId, "FocusedEditorChanged", Signals().editorChanged);
	spw::inst_impl_required_member_handler(classId, "FileOpened", Signals().fileOpened);

	spw::inst_impl_required_member_command(classId, "ConfigureSourceBrowser", CommandsClass().Configure);
	spw::inst_impl_required_member_command(classId, "RefreshSourceBrowser", CommandsClass().Refresh);
}

//-----------------------------------------------------------------------

onevent Destructor 
{
	//the component may be destroyed without being removed, so do this anyway
	local shell = spw.components.Shell;
	if (shell.serial != 0 and window)
		shell.RemoveComponent(window);
}

//-----------------------------------------------------------------------

onevent CreateWindow (parent) {
	config = ConfigDataSingleton();
	config.Refresh();
	
	RegisterImages();

	ui = UIClass();
	return ui.Init(parent);
}

//-----------------------------------------------------------------------
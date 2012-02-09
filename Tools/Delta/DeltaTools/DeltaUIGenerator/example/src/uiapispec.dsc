////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sample UIAPISpec and implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;
using corba;
const nl = "\n";

///////////////////////////////////////////////////////////////
//
// Sample function implementations
//

function stringTest (a ,b) {	
	c = a[0];
	a[0] = "#####";
	print(c, " ", b, nl);
	return c+b;
}

function booleanTest (b) {
	print(b[0], nl);
	b[0] = not b[0];
	
	return b[0];
}

function structTest (s) {
	print(s[0].struct_string, " ", s[0].struct_boolean, nl);
	s[0].struct_string = "######";
	s[0].struct_boolean = true;
}

function enumTest (colors) {
	print(colors[0], nl);
	colors[0]="green";
}

function intTest (i, iout) {
	print(i[0], nl);
	i[0] = 696;
	
	print(iout[0]);
}

function listTest (string_list) {
	foreach(s, string_list[0])
		print(s, ", ");
	print(nl);
	local list = list_new();
	list.push_back("hello");
	list.push_back("kitty");
	list.push_back("island adventure");
	string_list[0] = list;
}

function vectorTest (v) {
	for (local i = 0; i < tablength(v[0]); i++)
		print((v[0])[i], ", ");
	print(nl);
	
	v[0] = ["hello", "kitty", "MMO"];
	
	return ["hello", "kitty", "MMO"];
}

function nestedListTest (l) {
	print();
}

function unionTest (u) {
	// unions are string discriminated
	// for example:
	// unionTypeInfo = [ 
	//  @type : "Union",
	//  @userDefClassId : "sample_union",
	//  @members : [ 
	// 	  [ @name : "union_str_element", @typeInfo : stringTypeInfo ],
	//    [ @name : "union_int_element", @typeInfo : intTypeInfo ]
	//  ]
	// ];
	// If unionTypeInfo is incoming data it will be either
	// [ @type : "union_int_element", @value : <a number> ]
	// [ @type : "union_str_element", @value : <a string> ]
	// @type is used for CORBA compatibility.
	
	print(u[0].type, " : ", u[0].value, nl);
	retUnion = [ @type : "union_int_element", @value : 696 ];
	u[0] = retUnion;
}

function arrayTest (a) {
	for (local i = 0; i < tablength(a); i++)
		print(a[i], ", ");
	print(nl);
	
	//for (local i = 0; i < tablength(a[0]); i++)
	//	(a[0])[i] = true;
	
	return ["red", "cyan", "red", "red", "red"];

}

function calc (op1, op2, op) {
	local c = [ 
		{ "+" : (function (op1, op2) { return op1+op2; }) },
		{ "-" : (function (op1, op2) { return op1-op2; }) },
		{ "*" : (function (op1, op2) { return op1*op2; }) }
	];
	
	ret =  c[corba::enum(op)](op1, op2);
	return ret;
}

function book (b) {
	book1 = [
		@Author : "Lewis Carroll",
		@Title : "Alice's Adventures in Wonderland",
		@ISBN : "XXXX-XXXX-XXXXX"
	];
	book2 = [
		@Author : "Arthur Conan Doyle",
		@Title : "The Adventures of Sherlock Holmes",
		@ISBN : "XXXX-XXXX-XXXXX"
	];
	bookList = list_new();
	bookList.push_back(book1);
	bookList.push_back(book2);
	
	return bookList;
}

function nonUITest (in1, in2, in3, in4) {
	return in1 + in2 + in3 + in4;
}

function nonUI (out1, out2, out3, in1, in2) {
	out1[0] = in1 + in2;
	out2[0] = in1 - in2;
	out3[0] = in1 * in2;
	
	return in1 / in2;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// Basic types 
//

//String
stringTypeInfo = [
	@type : "String"
];

//Boolean
booleanTypeInfo = [ 
	@type : "Boolean"
];

//Integer
intTypeInfo = [ 
	@type : "Integer"
];

//Void
voidTypeInfo = [
	@type : "Void"
];

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// Complex types
//

//Struct
structTypeInfo = [
	@type : "Struct",
	@userDefClassId : "sample_struct",
	@members : [
		[ @name : "struct_string", @typeInfo : stringTypeInfo ],
		[ @name : "struct_boolean", @typeInfo : booleanTypeInfo ]
	]
];

//List
listTypeInfo = [
	@type : "List",
	@userDefClassId : "stringList",
	@elementTypeInfo : stringTypeInfo
];

//nested list
nestedListTypeInfo = [ 
	@type : "List",
	@userDefClassId : "ListOfStringLists",
	@elementTypeInfo : listTypeInfo
];

//Vector
vectorTypeInfo = [
	@type : "Vector",
	@userDefClassId : "stringVector",
	@elementTypeInfo : stringTypeInfo
];

//Enum
enumTypeInfo = [ 
	@type : "Enumeration",
	@userDefClassId : "sample_enum",
	@members : [ 
		[ @name : "red" ],
		[ @name : "green" ],
		[ @name : "blue" ],
		[ @name : "cyan" ],
		[ @name : "magenta" ],
		[ @name : "yellow" ]
	]
];

//Union
unionTypeInfo = [ 
	@type : "Union",
	@userDefClassId : "sample_union",
	@members : [ 
		[ @name : "union_str_element", @typeInfo : stringTypeInfo ],
		[ @name : "union_int_element", @typeInfo : intTypeInfo ]
	]
];

//Array
arrayTypeInfo = [
	@type : "Array",
	@userDefClassId : "sample_array",
	@elementTypeInfo : enumTypeInfo,
	@length : 5
];

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// Sample specifications
//

//
// Specification of function stringTest
//

stringTestFuncSpec = [
	@signature : [
		@name : "stringTest",
		@returnValue : [
			@typeInfo : stringTypeInfo,
			@name	: "retval"
		],
		@parameters : [
			[
				@typeInfo : stringTypeInfo,
				@name	: "string1_arg",
				@dataFlowType	: "InOut"
			],
			[
				@typeInfo : stringTypeInfo,
				@name	: "string2_arg",
				@dataFlowType	: "In"
			]
		]
	],
	@func : [
		@ref : stringTest
	]
];

//
// Specification of arrayTest
//

arrayTestFuncSpec = [
	@signature : [
		@name : "arrayTest",
		@returnValue : [
			@typeInfo : arrayTypeInfo,
			@name : "Return array"
		],
		@parameters : [
			[
				@typeInfo : arrayTypeInfo,
				@name : "enum_array",
				@dataFlowType : "In"
			]
		]
	],
	@func : [
		@ref : arrayTest
	]
];

//
// Specification of structTest
//

structTestFuncSpec = [ 
	@signature : [ 
		@name : "structTest",
		@returnValue : [ 
			@name : "none",
			@typeInfo : voidTypeInfo
		],
		@parameters : [
			[
				@typeInfo : structTypeInfo,
				@name : "s",
				@dataFlowType : "InOut"
			]
		]
	],
	@func : [
		@ref : structTest
	]
];

//
// Specification of enumTest
//

enumTestFuncSpec = [ 
	@signature : [ 
		@name : "enumTest",
		@returnValue : [ 
			@name : "none",
			@typeInfo : voidTypeInfo
		],
		@parameters : [
			[
				@typeInfo : enumTypeInfo,
				@name : "colors",
				@dataFlowType : "InOut"
			]
		]
	],
	@func : [
		@ref : enumTest
	]
];

//
// Specification of intTest
//

intTestFuncSpec = [ 
	@signature : [ 
		@name : "intTest",
		@returnValue : [ 
			@name : "none",
			@typeInfo : voidTypeInfo
		],
		@parameters : [
			[
				@typeInfo : intTypeInfo,
				@name : "i",
				@dataFlowType : "InOut"
			],
			[
				@typeInfo : intTypeInfo,
				@name : "i_out",
				@dataFlowType : "Out"
			]
		]
	],
	@func : [
		@ref : intTest
	]
];

//
// Specification of listTest
//

listTestFuncSpec = [ 
	@signature : [ 
		@name : "listTest",
		@returnValue : [ 
			@name : "none",
			@typeInfo : voidTypeInfo
		],
		@parameters : [
			[
				@typeInfo : listTypeInfo,
				@name : "string_list",
				@dataFlowType : "InOut"
			]
		]
	],
	@func : [
		@ref : listTest
	]
];

//
// Specification of vectorTest
//

vectorTestFuncSpec = [ 
	@signature : [ 
		@name : "vectorTest",
		@returnValue : [ 
			@name : "Return vector",
			@typeInfo : vectorTypeInfo
		],
		@parameters : [
			[
				@typeInfo : vectorTypeInfo,
				@name : "string_vector",
				@dataFlowType : "InOut"
			]
		]
	],
	@func : [
		@ref : vectorTest
	]
];

//
// Specification of nestedListTest
//

nestedListTestFuncSpec = [ 
	@signature : [ 
		@name : "nestedListTest",
		@returnValue : [ 
			@name : "none",
			@typeInfo : voidTypeInfo
		],
		@parameters : [
			[
				@typeInfo : nestedListTypeInfo,
				@name : "list_of_string_lists",
				@dataFlowType : "In"
			]
		]
	],
	@func : [
		@ref : nestedListTest
	]
];

//
// Specification of unionTest
//

unionTestFuncSpec = [ 
	@signature : [ 
		@name : "unionTest",
		@returnValue : [ 
			@name : "none",
			@typeInfo : voidTypeInfo
		],
		@parameters : [
			[
				@typeInfo : unionTypeInfo,
				@name : "u",
				@dataFlowType : "InOut"
			]
		]
	],
	@func : [
		@ref : unionTest
	]
];

//
// Specification of booleanTest
//

booleanTestFuncSpec = [ 
	@signature : [ 
		@name : "booleanTest",
		@returnValue : [ 
			@name : "return value",
			@typeInfo : booleanTypeInfo
		],
		@parameters : [
			[
				@typeInfo : booleanTypeInfo,
				@name : "b",
				@dataFlowType : "InOut"
			]
		]
	],
	@func : [
		@ref : booleanTest
	]
];

//
// Specification of calc
//

calcEnum = [ 
	@type : "Enumeration",
	@userDefClassId : "operator",
	@members : [ 
		[ @name : "+" ],
		[ @name : "-" ],
		[ @name : "*" ]
	]
];

calcFuncSpec = [ 
	@signature : [ 
		@name : "calc",
		@returnValue : [ 
			@name : "result",
			@typeInfo : intTypeInfo
		],
		@parameters : [ 
			[ @name : "operand 1", @typeInfo : intTypeInfo, @dataFlowType : "In" ],
			[ @name : "operand 2", @typeInfo : intTypeInfo, @dataFlowType : "In" ],
			[ @name : "operator", @typeInfo : calcEnum, @dataFlowType : "In" ]
		]
	],
	@func : [
		@ref : calc
	]
];

//
// Specification of book
//

bookStruct = [ 
	@type : "Struct",
	@userDefClassId : "Book",
	@members : [ 
		[ @name : "Author", @typeInfo : stringTypeInfo ],
		[ @name : "Title", @typeInfo : stringTypeInfo ],
		[ @name : "ISBN", @typeInfo : stringTypeInfo ]
	]
];

bookList = [ 
	@type : "List",
	@userDefClassId : "Book List",
	@elementTypeInfo : bookStruct
];

bookFuncSpec = [ 
	@signature : [ 
		@name : "Book Search",
		@returnValue : [ 
			@name : "result",
			@typeInfo : bookList
		],
		@parameters : [ 
			[ @name : "Search Parameters", @typeInfo : bookStruct, @dataFlowType : "In" ]
		]
	],
	@func : [
		@ref : book
	]
];

//
// Specification of libfuncTest
//

libfuncTestFuncSpec = [ 
	@signature : [ 
		@name : "print libfunc",
		@returnValue : [ 
			@typeInfo : voidTypeInfo
		],
		@parameters : [ 
			[ @name : "string", @typeInfo : stringTypeInfo, @dataFlowType : "In" ]
		]
	],
	@func : [ 
		@ref : print
	]
];

//
// Specification of nonUITest
//

nonUITestFuncSpec = [
	@signature : [
		@name : "nonUI test",
		@returnValue : [
			@typeInfo : intTypeInfo,
			@name : "return numeric"
		],
		@parameters : [
			[ @name : "a_in1", @typeInfo : intTypeInfo, @dataFlowType : "In" ],
			[ @name : "a_in2", @typeInfo : intTypeInfo, @dataFlowType : "In" ],
			[ @name : "a_in3", @typeInfo : intTypeInfo, @dataFlowType : "In" ],
			[ @name : "a_in4", @typeInfo : intTypeInfo, @dataFlowType : "In" ]
		]
	],
	@func : [
		@ref : nonUITest
	]
];

nonUIFuncSpec = [
	@signature : [
		@name : "nonUI",
		@returnValue : [
			@typeInfo : intTypeInfo,
			@name : "return numeric",
			@mapsTo : "a_in3"
		],
		@parameters : [
			[ @name : "b_out1", @typeInfo : intTypeInfo, @dataFlowType : "Out", @mapsTo : "a_in1" ],
			[ @name : "b_out2", @typeInfo : intTypeInfo, @dataFlowType : "Out", @mapsTo : "a_in2" ],
			[ @name : "b_out3", @typeInfo : intTypeInfo, @dataFlowType : "Out" ],
			[ @name : "b_in1", @typeInfo : intTypeInfo, @dataFlowType : "In" ],
			[ @name : "b_in2", @typeInfo : intTypeInfo, @dataFlowType : "In" ]
		]
	],
	@func : [
		@ref : nonUI,
		@linkedTo : "nonUI test"
	]
];

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// Sample UIAPISpec
//

apiSpec = [
	@uiOperations : [
		stringTestFuncSpec,
		structTestFuncSpec,
		enumTestFuncSpec,
		intTestFuncSpec,
		nonUITestFuncSpec,
		libfuncTestFuncSpec
	],
	@nonUIOperations : [
		nonUIFuncSpec
	]
];

function GetAPISpec {
	return apiSpec;
}

///////////////////////////////////////////////////////////////


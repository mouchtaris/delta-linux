////////////////////////////////////////////////////////////////////////////////////////////////////////
// CORBA metadata to UIAPISpec converter
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;

//
// Constants
//

const nl = "\n";

///////////////////////////////////////////////////////////////
//
// CORBAMetadataUIAPISpecConverter internal functions
//

//
// ConvertModeString
// (String) (mode:String {"in", "out", "inout"})
//

function ConvertModeString (mode) {
	if (mode == nil)
		return "In";
	
	static map = [
			@in : "In",
			@out : "Out",
			@inout: "InOut"
	];
	
	return map[mode];
}

//
// GetTypeStringFromType
// (String) (type:Table or Object or String)
//

function GetTypeStringFromType(type) {
	if (typeof(type) == "Table" or typeof(type) == "Object") {
		if (type.type == "tk_alias")
			return type.content_type.type;
		return type.type;	
	}
	return type;
}

//
// TypeToTypeInfo
// (TypeInfo Object) (type:Object)
//

function TypeToTypeInfo (type) {
	static converters = [
		// Method names match CORBA idl types sans the "tk_" prefix
		method string (type)
			{ return [ @type : "String" ]; },
		method boolean (type)
			{ return [ @type : "Boolean" ]; },
		method double (type)
			{ return [ @type : "Integer" ]; },
		method char (type)
			{ return self.string(type); }, // placeholder
		method float (type)
			{ return self.double(type); }, // placeholder
		method short (type)
			{ return self.double(type); }, // placeholder
		method long (type)
			{ return self.double(type); }, // placeholder
		method octet (type)
			{ return self.double(type); }, // placeholder
		method ushort (type)
			{ return self.double(type); }, // placeholder
		method ulong (type)
			{ return self.double(type); }, // placeholder
		method enum (type) {
			local typeInfo = [
				@type : "Enumeration",
				@userDefClassId : type.name,
				@members : []
			];
			
			local i = 0;
			foreach (m, type.members) {
				print(m, nl);
				typeInfo.members[i++] = [ @name : m.name ];
			}
			return typeInfo;
			
		},
		method struct (type) {
			local typeInfo = [
				@type : "Struct",
				@userDefClassId : type.name,
				@members : []
			];
			
			local i = 0;
			foreach (m, type.members) {
				print(m, nl);
				typeInfo.members[i++] = [ @name : m.name, @typeInfo : TypeToTypeInfo(m.type) ];
			}
			
			return typeInfo;
		},
		method sequence (type) {
			local typeInfo = [
				@type : "Vector",
				@userDefClassId : type.name,
				@elementTypeInfo : TypeToTypeInfo(type.content_type.content_type)
			];
			
			return typeInfo;
		},
		method union (type) {
			local typeInfo = [
				@type : "Union",
				@userDefClassId : type.name,
				@members : []
			];
			
			local i = 0;
			foreach (m, type.members) {
				print(m, typeof(m.label), nl);
				typeInfo.members[i++] = [ @name : m.name, @typeInfo : TypeToTypeInfo(m.type) ];
			}
			
			return typeInfo;
		},
		method array (type) {
			local typeInfo = [
				@type : "Array",
				@userDefClassId : type.name,
				@length : type.content_type.length,
				@elementTypeInfo : TypeToTypeInfo(type.content_type.content_type)
			];
			
			return typeInfo;
		},
		method void (type) {
			return [ @type : "Void" ];
		},
		
		method objref (type) {
			return [
				@type : "Object",
				@userDefClassId : type.name
			];
		}
	];
	
	const corbaTypeStringPrefix = "tk_";
	const corbaTypeStringPrefixLength = 3;
	
	local typeStr = GetTypeStringFromType(type);
	assert(strisprefix(typeStr, corbaTypeStringPrefix));
	
	return converters[strslice(typeStr, corbaTypeStringPrefixLength, 0)](type);
}

//
// ConvertParameters
// (UIAPISpec param Object) (corbaParams:Object)
//

function ConvertParameters (corbaParams) {
	local parameters = [];
	for (local i = 0; i < tablength(corbaParams); i++) {
		parameters[i] = [ 
			@name : corbaParams[i].name,
			@dataFlowType : ConvertModeString(corbaParams[i].mode),
			@typeInfo : TypeToTypeInfo(corbaParams[i].type)
		];
	}
	
	return parameters;
}

//
// ConvertResult
// (UIAPISpec returnValue Object) (result:Object)
//

function ConvertResult (result) {
	local returnValue = [
		@name : "Return Value",
		@typeInfo : TypeToTypeInfo(result)
	];
	
	return returnValue;
}

//
// ConvertOperation
// (UIAPISpec operation Object) (corbaOperation:Object, corbaService:Object)
//

function ConvertOperation (corbaOperation, corbaService) {
	local operation = [
		@signature : [
			@name : corbaOperation.name,
			@returnValue : ConvertResult(corbaOperation.result),
			@parameters : ConvertParameters(corbaOperation.parameters)
		],
		@func : [
			@ref : corbaService[corbaOperation.name]
		]
	];
	
	return operation;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// CORBAMetadataUIAPISpecConverter public functions
//

//
// CreateUIAPISpecFromCORBAService
// (UIAPISpec Object) (corbaService:Object)
//

function CreateUIAPISpecFromCORBAService (corbaService) {
	local corbaOperations = corbaService.metadata.operations;
	local apiSpec = [ @uiOperations : [] ];
	
	local i = 0;
	foreach (op, corbaOperations) {
		if (op.name == "shutdown")
			continue;
		apiSpec.uiOperations[i++] = ConvertOperation(op, corbaService);
	}
	
	return apiSpec;
}

//
// Test function
//

function test {
	testRc = rcload("C:\\delta_workspaces\\corba_lib\\idl_types.rc");
	s = GetTypeStringFromType(testRc.operations.test_simple_types.parameters[0].type);
	print(s, nl);
	s = GetTypeStringFromType(testRc.operations.test_str_list.result);
	print(s, nl);
	s = GetTypeStringFromType(testRc.operations.test_enum.parameters[0].type);
	print(s, nl);
	
	typeInfo = TypeToTypeInfo(testRc.operations.test.parameters[0].type);
	typeInfo = TypeToTypeInfo(testRc.operations.test_enum.parameters[0].type);
	typeInfo = TypeToTypeInfo(testRc.operations.test_str_list.result);
	typeInfo = TypeToTypeInfo(testRc.operations.test_string_union.parameters[0].type);
	print();
}

///////////////////////////////////////////////////////////////

//test();
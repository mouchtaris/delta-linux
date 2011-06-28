////////////////////////////////////////////////////////////////////////////////////////////////////////
// Validator for the UI API Specification
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;

//
// Constants
//

const nl = "\n";


///////////////////////////////////////////////////////////////
//
// Internal functions
//

//
// isStdList
// (boolean) (l:list)
//

function isStdList (l) {
	if (typeof(l) == "ExternId" and externidtype(l) == "std::list")
		return true;
	return false;
}

//
// ValidateTableIsArray 
// (true or String) (t:Object)
//

function ValidateTableIsArray (t) {
	for (local i = 0; i < tablength(t); i++) {
		if (typeof(t[i]) == "Nil") {
			local e = "Table is NOT an array, element at position " + i + " is nil";
			return e;
		}
	}
	return true;
}

//
// ValidateTableHasUniqueNames
// (true or String) (t:Object)
//

function ValidateTableHasUniqueNames (t) {
	for (local i = 0; i < tablength(t); i++) {
		for (local j = i+1; j < tablength(t); j++) {
			if (t[i].name == t[j].name) {
				local e = "Name \"" + t[i].name + 
					"\" collides at positions " + 
					i + " and " + j;
				return e;
			}
		}
	}	
	return true;
}

//
// ValidateName
// (true or String)(name:String)
//

function ValidateName (name) {
	if ((typeof(name) == "Nil"))
		return "name is nil";
	if (typeof(name) != "String")
		return "name is not a String";
		
	return true;
}

//
// ValidateUserDefClassId
// (true or String) (userDefClassId:String)
//

function ValidateUserDefClassId (userDefClassId) {
	if ((typeof(userDefClassId) == "Nil"))
		return "userDefClassId is nil";
	if (typeof(userDefClassId) != "String")
		return "userDefClassId is not a String";
		
	return true;
}

//
// ValidateLength
// (true or String) (length:Number)
//

function ValidateLength (length) {
	if ((typeof(length) == "Nil"))
		return "length is nil";
	if (typeof(length) != "Number")
		return "length is not a Number";
	if (fractionalpart(length) != 0)
		return "length is not an integer";
	if (not (length > 0))
		return "length is not greater than zero";
		
	return true;
}

//
// ValidateDataFlowType
// (true or String) (dataFlowType:String {"In", "InOut", "Out"}) 
//

function ValidateDataFlowType (dataFlowType) {
	static validDataFlowTypes = [ 
		@In : true,
		@InOut : true,
		@Out : true
	];
	
	if (typeof(dataFlowType) == "Nil")
		return "dataFlowType is nil";
	if (typeof(dataFlowType) != "String")
		return "dataFlowType is not a String";
	if (typeof(validDataFlowTypes[dataFlowType]) == "Nil")
		return "Invalid dataFlowType";
		
	return true;
}

//
// ValidateTypeInfo
// (true or list<String>) (typeInfo:UIAPISpec typeInfo Object)
//

function ValidateTypeInfo (typeInfo) {
	local errorList = list_new();
	// Method names map to UIAPISpec types
	static validTypes = [ 
		method String (typeInfo)
			{ return true; },
		method Boolean (typeInfo)
			{ return true; },
		method Integer (typeInfo)
			{ return true; },
		method Object (typeInfo)
			{ return true; },
		method Struct (typeInfo) {
			local errorList = list_new();
			
			// Fatal errors
			if (typeof(typeInfo.members) == "Nil") {
				errorList.push_back("members is nil");
				return errorList;
			}
			else {
				e = ValidateTableIsArray(typeInfo.members);
				if (typeof(e) == "String") {
					errorList.push_back("In members : " + nl + e);
					return erroList;
				}
			}
			
			//Non fatal errors
			e = ValidateUserDefClassId(typeInfo.userDefClassId);
			if (typeof(e) == "String")
				errorList.push_back(e);
				
			for (local i = 0; i < tablength(typeInfo.members); i++) {
				local errorMessage = "";
				
				e = ValidateName(typeInfo.members[i].name);
				if (typeof(e) == "String")
					errorMessage += e + nl;
				if (typeof(typeInfo.members[i].typeInfo) == "Nil")
					errorMessage += "typeInfo is nil" + nl;
				else {
					eList = ValidateTypeInfo(typeInfo.members[i].typeInfo);
					if (isStdList(eList)) {
						errorMessage += "In typeInfo : " + nl;
						foreach (e, eList)
							errorMessage += e + nl;
					}
							
				}
				
				if (errorMessage != "") {
					errorMessage = "In members [" + i + "] : " + nl + errorMessage;
					errorList.push_back(errorMessage);
				}
					
			}
			e = ValidateTableHasUniqueNames(typeInfo.members);
			if (typeof(e) == "String")
				errorList.push_back("In members : " + nl + e);
			
			//"In Struct : " + e;
			if (errorList.total())
				return errorList;
			return true;
		},
		method Union (typeInfo) {
			local errorList = list_new();
			
			// Fatal errors
			if (typeof(typeInfo.members) == "Nil") {
				errorList.push_back("members is nil");
				return errorList;
			}
			else {
				e = ValidateTableIsArray(typeInfo.members);
				if (typeof(e) == "String") {
					errorList.push_back("In members : " + nl + e);
					return erroList;
				}
			}
			
			//Non fatal errors
			e = ValidateUserDefClassId(typeInfo.userDefClassId);
			if (typeof(e) == "String")
				errorList.push_back(e);
				
			for (local i = 0; i < tablength(typeInfo.members); i++) {
				local errorMessage = "";
				
				e = ValidateName(typeInfo.members[i].name);
				if (typeof(e) == "String")
					errorMessage += e + nl;
				if (typeof(typeInfo.members[i].typeInfo) == "Nil")
					errorMessage += "typeInfo is nil" + nl;
				else {
					eList = ValidateTypeInfo(typeInfo.members[i].typeInfo);
					if (isStdList(eList)) {
						errorMessage += "In typeInfo : " + nl;
						foreach (e, eList)
							errorMessage += e + nl;
					}
							
				}
				
				if (errorMessage != "") {
					errorMessage = "In members [" + i + "] : " + nl + errorMessage;
					errorList.push_back(errorMessage);
				}
					
			}
			e = ValidateTableHasUniqueNames(typeInfo.members);
			if (typeof(e) == "String")
				errorList.push_back("In members : " + nl + e);
			
			//"In Union : " + e;
			if (errorList.total())
				return errorList;
			return true;
		},
		method List (typeInfo) {
			local errorList = list_new();
			local e = ValidateUserDefClassId(typeInfo.userDefClassId);
			if (typeof(e) == "String")
				errorList.push_back(e);
					
			eList =	ValidateTypeInfo(typeInfo.elementTypeInfo);
			if (isStdList(eList)) {
				errorMessage = "In element : " + nl;
				foreach (e, eList)
					errorMessage += e + nl;
				errorList.push_back(errorMessage);
			}
			
			//throw "In List : " + e;
			if (errorList.total())
				return errorList;
			return true;
			
		},
		method Vector (typeInfo) {
			local errorList = list_new();
			local e = ValidateUserDefClassId(typeInfo.userDefClassId);
			if (typeof(e) == "String")
				errorList.push_back(e);
					
			eList =	ValidateTypeInfo(typeInfo.elementTypeInfo);
			if (isStdList(eList)) {
				errorMessage = "In element : " + nl;
				foreach (e, eList)
					errorMessage += e + nl;
				errorList.push_back(errorMessage);
			}
			
			//throw "In Vector : " + e;
			if (errorList.total())
				return errorList;
			return true;
			
		},
		method Enumeration 	(typeInfo) { 
			local errorList = list_new();				
			
			// Fatal errors
			e = ValidateTableIsArray(typeInfo.members);
			if (typeof(e) == "String") {
				errorList.push_back("In members : " + nl + e);
				return errorList;
			}
			if (typeof(typeInfo.members) == "Nil") {
				errorList.push_back("members is nil");
				return errorList;
			}
			if (typeof(typeInfo.members) != "Object") {
				errorList.push_back("members is not a table");
				return errorList;
			}
			
			// Non fatal errors
			e = ValidateUserDefClassId(typeInfo.userDefClassId);
			if (typeof(e) == "String")
				errorList.push_back(e);
				
			for (local i = 0; i < tablength(typeInfo.members); i++) {
				e = ValidateName(typeInfo.members[i].name);
				if (typeof(e) == "String")
					errorList.push_back("In members [" + i + "] : " + e);
			}
			
			e = ValidateTableHasUniqueNames(typeInfo.members);
			if (typeof(e) == "String") {
				errorList.push_back("In members : " + e);
			}

			//throw "In Enumeration : " + e;
			if (errorList.total())
				return errorList;
			return true;
			
		},
		method Array (typeInfo) {			
			local errorList = list_new();
			local e = ValidateUserDefClassId(typeInfo.userDefClassId);
			if (typeof(e) == "String")
				errorList.push_back(e);
			local e = ValidateLength(typeInfo.length);
			if (typeof(e) == "String")
				errorList.push_back(e);
					
			eList =	ValidateTypeInfo(typeInfo.elementTypeInfo);
			if (isStdList(eList)) {
				errorMessage = "In elementTypeInfo : " + nl;
				foreach (e, eList)
					errorMessage += e + nl;
				errorList.push_back(errorMessage);
			}
			
			//throw "In Array : " + e;
			if (errorList.total())
				return errorList;
			return true;		}
	];
	
	// Fatal errors
	if ((typeof(typeInfo) == "Nil")) {
		errorList.push_back("typeInfo is nil");
		return errorList;
	}

	if (typeof(typeInfo) != "Object") {
		errorList.push_back("typeInfo is not a Table");
		return errorList;
	}
	if (typeof(validTypes[typeInfo.type]) == "Nil") {
		errorList.push_back("typeInfo is of invalid type");
		return errorList;
	}
		
	return validTypes[typeInfo.type](typeInfo);
}

//
// ValidateMapsTo
// (true or String) (mapsTo:String) 
//

function ValidateMapsTo (mapsTo) {
	if (typeof(mapsTo) == "Nil")
		return true;
	if (typeof(mapsTo) != "String")
		return "mapsTo is not a String";
		
	return true;
}

//
// ValidateLinkedTo
// (true or String) (linkedTo:String) 
//

function ValidateLinkedTo (linkedTo) {
	if (typeof(linkedTo) == "Nil")
		return "linkedTo is nil";
	if (typeof(linkedTo) != "String")
		return "linkedTo is not a String";
		
	return true;
}

//
// ValidateReturnValue 
// (true or String) (returnValueTable:UIAPISpec returnValue Object)
//

function ValidateReturnValue (returnValueTable) {
	local errorMessage = "";
	
	// Fatal errors
	if (typeof(returnValueTable) != "Object")
		return "returnValue is not a table";
	if ((typeof(returnValueTable) == "Nil"))
		return "returnValue is nil";
	
	// Non Fatal errors
	if (returnValueTable.typeInfo.type != "Void") {
		e = ValidateName(returnValueTable.name);
		if (typeof(e) == "String")
			errorMessage += e + nl;
		eList = ValidateTypeInfo(returnValueTable.typeInfo);
		if (isStdList(eList)) {
			errorMessage += "In typeInfo : " + nl;
			foreach (e, eList)
				errorMessage += e + nl;
		}
	}
	if (errorMessage != "")
		return "In returnValue : " + nl + errorMessage;
	return true;
}

//
// ValidateNonUIReturnValue 
// (true or String) (returnValueTable:UIAPISpec returnValue Object)
//

function ValidateNonUIReturnValue (returnValueTable) {
	local errorMessage = ValidateReturnValue(returnValueTable);
	if (errorMessage != true)
		return "In returnValue : " + nl + errorMessage;
	else
		errorMessage = "";
		
	// Non Fatal errors
	if (returnValueTable.typeInfo.type != "Void") {
		e = ValidateMapsTo(returnValueTable.mapsTo);
		if (typeof(e) == "String")
			errorMessage += e + nl;
	}
	
	if (errorMessage != "")
		return "In returnValue : " + nl + errorMessage;
	return true;
}

//
// ValidateParameter
// (true or String) (p:UIAPISpec parameter Object)
//

function ValidateParameter (p) {
	local errorMessage = "";	
	
	if (typeof(e = ValidateDataFlowType(p.dataFlowType)) == "String")
		errorMessage += e + nl;
	if (typeof(e = ValidateName(p.name)) == "String")
		errorMessage += e + nl;
	eList = ValidateTypeInfo(p.typeInfo);
	if (isStdList(eList)) {
		errorMessage += "In typeInfo : " + nl;
		foreach (e, eList)
			errorMessage += e + nl;
	}
			
	if (errorMessage != "")
		return errorMessage;
	return true;

}

//
// ValidateNonUIParameter
// (true or String) (p:UIAPISpec parameter Object)
//

function ValidateNonUIParameter (p) {
	local errorMessage = ValidateParameter(p);
	if (errorMessage == true)
		errorMessage = "";
	
	if (typeof(e = ValidateMapsTo(p.mapsTo)) == "String")
		errorMessage += e + nl;
			
	if (errorMessage != "")
		return errorMessage;
	return true;
}

//
// ValidateParameters
// (true or list<String>) (paramTable:UIAPISpec parameters Object)
//

function ValidateParameters (paramTable) {
	local errorList = list_new();

	// Fatal errors
	if ((typeof(paramTable) == "Nil")) {
		errorList.push_back("parameters is nil");
		return errorList;
	}
	
	if (typeof(paramTable) != "Object") {
		errorList.push_back("parameters is not a table");
		return errorList;
	}
	
	e = ValidateTableIsArray(paramTable);
	if (typeof(e) == "String") {
		errorList.push_back("In parameters : " + e);
		return errorList;
	}
	
	// Non fatal errors
	for (local i = 0; i < tablength(paramTable); i++) {
		if (typeof(paramTable[i]) != "Object") {
			errorList.push_back("At parameters [" + 
			i + 
			"] : value is not a table");
			continue;
		}
		e = ValidateParameter(paramTable[i]);
		if (typeof(e) == "String") {
			errorList.push_back("At parameters [" + 
			i + 
			"] with name \"" + 
			paramTable[i].name +
			"\" : " + nl +
			e);
		}
	}
	e = ValidateTableHasUniqueNames(paramTable);
	if(typeof(e) == "String") {
		errorList.push_back("In parameters : " + e);
	}
	
	if (errorList.total())
		return errorList;
	return true;
	
}

//
// ValidateNonUIParameters
// (true or list<String>) (paramTable:UIAPISpec parameters Object)
//

function ValidateNonUIParameters (paramTable) {
	local errorList = ValidateParameters(paramTable);
	if (isStdList(errorList))
		return errorList;
	else
		errorList = list_new();
	
	// Non fatal errors
	for (local i = 0; i < tablength(paramTable); i++) {
		e = ValidateNonUIParameter(paramTable[i]);
		if (typeof(e) == "String") {
			errorList.push_back(
				"At parameters [" + 
				i + 
				"] with name \"" + 
				paramTable[i].name +
				"\" : " + nl +
				e
			);
		}
	}
	
	if (errorList.total())
		return errorList;
	return true;
	
}

//
// ValidateSignature
// (true or String) (sigTable:UIAPISpec signature Object)
//

function ValidateSignature (sigTable) {
	local errorMessage = "";
	
	// Fatal errors
	if ((typeof(sigTable) == "Nil"))
		return "signature is nil";
	if (typeof(sigTable) != "Object")
		return "signature is not a table";
	
	// Non fatal errors
	e = ValidateName(sigTable.name);
	if (typeof(e) == "String")
		errorMessage += e + nl;
		
	e = ValidateReturnValue(sigTable.returnValue);
	if (typeof(e) == "String")
		errorMessage += e + nl;
		
	eList = ValidateParameters(sigTable.parameters);
	if (isStdList(eList)) {
		foreach (e, eList)
			errorMessage += e;// + nl;
	}
	
	if (errorMessage != "")
		return "In signature : " + nl + errorMessage;
	return true;
}

//
// ValidateNonUISignature
// (true or String) (sigTable:UIAPISpec signature Object)
//

function ValidateNonUISignature (sigTable) {
	local errorMessage = "";
	
	// Fatal errors
	if ((typeof(sigTable) == "Nil"))
		return "signature is nil";
	if (typeof(sigTable) != "Object")
		return "signature is not a table";
	
	// Non fatal errors
	e = ValidateName(sigTable.name);
	if (typeof(e) == "String")
		errorMessage += e + nl;
		
	e = ValidateNonUIReturnValue(sigTable.returnValue);
	if (typeof(e) == "String")
		errorMessage += e + nl;
		
	eList = ValidateNonUIParameters(sigTable.parameters);
	if (isStdList(eList)) {
		foreach (e, eList)
			errorMessage += e;// + nl;
	}
	
	if (errorMessage != "")
		return "In signature : " + nl + errorMessage;
	return true;
}

//
// ValidateRef
// (true or String) (ref:ProgramFunc or LibraryFunc)
//

function ValidateRef (ref) {
	if ((typeof(ref) == "Nil"))
		return "ref is nil";
	if (typeof(ref) != "ProgramFunc" and typeof(ref) != "LibraryFunc")
		return "ref is not of type ProgramFunc or LibraryFunc";
		
	return true;
}

//
// ValidateFunc
// (true or String) (funcTable:UIAPISpec func Object)
//

function ValidateFunc (funcTable) {
	// Fatal errors
	if ((typeof(funcTable) == "Nil"))
		return "func is nil";
	if (typeof(funcTable) != "Object")
		return "func is not a table";
		
	// Non fatal erros
	e = ValidateRef(funcTable.ref);
	if (typeof(e) == "String") {
		return "In func : " + nl + e;
	}
	
	return true;
}

//
// ValidateNonUIFunc
// (true or String) (funcTable:UIAPISpec func Object)
//

function ValidateNonUIFunc (funcTable) {
	local errorMessage = "";
	
	// Fatal errors
	if ((typeof(funcTable) == "Nil"))
		return "func is nil";
	if (typeof(funcTable) != "Object")
		return "func is not a table";
		
	// Non fatal erros
	e = ValidateRef(funcTable.ref);
	if (typeof(e) == "String") {
		errorMessage += e + nl;
	}
	
	e = ValidateLinkedTo(funcTable.linkedTo);
	if (typeof(e) == "String") {
		errorMessage += e + nl;
	}
	
	if (errorMessage != "")
		return "In func: " + nl + errorMessage;
	return true;
}

//
// ValidateOperation
// (true or String) (opTable:UIAPISpec operation Object) 
//

function ValidateOperation (opTable) {
	local errorMessage = "";
	
	e = ValidateSignature(opTable.signature);
	if (typeof(e) == "String") {
		errorMessage += e + nl;
	}
	e = ValidateFunc(opTable.func);
	if (typeof(e) == "String") {
		errorMessage += e + nl;
	}
	
	if (errorMessage != "")
		return errorMessage;
	
	return true;
}

//
// ValidateNonUIOperation
// (true or String) (opTable:UIAPISpec non UI operation Object) 
//

function ValidateNonUIOperation (opTable) {
	local errorMessage = "";
	
	e = ValidateNonUISignature(opTable.signature);
	if (typeof(e) == "String") {
		errorMessage += e + nl;
	}
	e = ValidateNonUIFunc(opTable.func);
	if (typeof(e) == "String") {
		errorMessage += e + nl;
	}
	
	if (errorMessage != "")
		return errorMessage;
	
	return true;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// Public functions
//

//
// ValidateUIAPISpec throws errorList
// (apiSpec:UIAPISpec Object)
//

function ValidateUIAPISpec (apiSpec) {
	local errorList = list_new();
	local uiOperations = apiSpec.uiOperations;
	assert uiOperations;
	local nonUIOperations = apiSpec.nonUIOperations;
	
	local signatureNames = [];
	for (local i = 0; i < tablength(uiOperations); i++)
		signatureNames[i] = [ @name : uiOperations[i].signature.name ];
	if (nonUIOperations)
		for (local i = 0; i < tablength(nonUIOperations); i++)
			signatureNames[tablength(signatureNames)] = 
				[ @name : nonUIOperations[i].signature.name ];
	
	e = ValidateTableHasUniqueNames(signatureNames);
	if (typeof(e) == "String") {
		errorList.push_back(
			"Error in API Specification :" + nl + e
		);
	}
	
	for (local i = 0; i < tablength(uiOperations); i++) {
		e = ValidateOperation(uiOperations[i]);
		if (typeof(e) == "String") {
			errorList.push_back(
				"Error in UI operation " + 
				i + 
				" with name \"" + 
				uiOperations[i].signature.name +
				"\" : " +
				nl +
				e
			);
		}
	}
	
	if (nonUIOperations)
		for (local i = 0; i < tablength(nonUIOperations); i++) {
			e = ValidateNonUIOperation(nonUIOperations[i]);
			if (typeof(e) == "String") {
				errorList.push_back(
					"Error in non UI operation " + 
					i + 
					" with name \"" + 
					nonUIOperations[i].signature.name +
					"\" : " +
					nl +
					e
				);
			}
		}
	
	if (errorList.total())
		throw errorList;
}

///////////////////////////////////////////////////////////////

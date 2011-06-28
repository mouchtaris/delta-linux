////////////////////////////////////////////////////////////////////////////////////////////////////////
// Operation and Property generators
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;
using wx;

//
// Constants
//

const nl = "\n";

//
// VM import
//

MicroUIs = libs::import("MicroUIs");
Properties = libs::import("Properties");
assert(MicroUIs);
assert(Properties);

///////////////////////////////////////////////////////////////

//
// PropertyFromUIAPISpecParamGenerator
// (PropertyFromUIAPISpecParamGenerator Object)
//

function PropertyFromUIAPISpecParamGenerator {
	static generator;
	if (isundefined(generator))
		//Add new datatypes here
		generator = [
			method String (UIAPISpecParamData) {
				return Properties.StringProperty(UIAPISpecParamData.name, "",
					UIAPISpecParamData.dataFlowType);
			},
			method Boolean (UIAPISpecParamData) {
				return Properties.BooleanProperty(UIAPISpecParamData.name,
					UIAPISpecParamData.dataFlowType);
			},
			method Integer (UIAPISpecParamData) {
				return Properties.NumericProperty(UIAPISpecParamData.name, "",
					UIAPISpecParamData.dataFlowType);
			},
			method Struct (UIAPISpecParamData) {
				aggProp = Properties.AggregateProperty(UIAPISpecParamData.name,
					UIAPISpecParamData.typeInfo.userDefClassId,
					UIAPISpecParamData.dataFlowType);
				
				local members = UIAPISpecParamData.typeInfo.members;
				for (local i = 0; i < tablength(members); i++) {
					local paramData = tabcopy(members[i]);
					paramData.dataFlowType = UIAPISpecParamData.dataFlowType;
					local type = paramData.typeInfo.type;
					aggProp.AddProperty(PropertyFromUIAPISpecParamGenerator()[type](paramData));
				}
				
				return aggProp;
			},
			method Union (UIAPISpecParamData) {
				local unionProp = Properties.UnionProperty(UIAPISpecParamData.name,
					UIAPISpecParamData.typeInfo.userDefClassId, UIAPISpecParamData.dataFlowType);
				
				local members = UIAPISpecParamData.typeInfo.members;
				for (local i=0; i < tablength(members); i++) {
					local paramData = tabcopy(members[i]);
					paramData.dataFlowType = UIAPISpecParamData.dataFlowType;
					local type = paramData.typeInfo.type;
					unionProp.AddProperty(PropertyFromUIAPISpecParamGenerator()[type](paramData));
				}
				
				return unionProp;
			},
			method List (UIAPISpecParamData) {
				local type = UIAPISpecParamData.typeInfo.elementTypeInfo.type;
				local paramData = [
					@dataFlowType : UIAPISpecParamData.dataFlowType,
					@name : "list element",
					@typeInfo : tabcopy(UIAPISpecParamData.typeInfo.elementTypeInfo)
				];
				local motherProp = PropertyFromUIAPISpecParamGenerator()[type](paramData);
				
				return Properties.ListProperty(
					UIAPISpecParamData.name,
					UIAPISpecParamData.typeInfo.userDefClassId,
					motherProp
				);
			},
			method Vector (UIAPISpecParamData) {
				local type = UIAPISpecParamData.typeInfo.elementTypeInfo.type;
				local paramData = [
					@dataFlowType : UIAPISpecParamData.dataFlowType,
					@name : "vector element",
					@typeInfo : tabcopy(UIAPISpecParamData.typeInfo.elementTypeInfo)
				];
				local motherProp = PropertyFromUIAPISpecParamGenerator()[type](paramData);
				
				return Properties.VectorProperty(
					UIAPISpecParamData.name,
					UIAPISpecParamData.typeInfo.userDefClassId,
					motherProp
				);
			},
			method Array (UIAPISpecParamData) {
				local type = UIAPISpecParamData.typeInfo.elementTypeInfo.type;
				local paramData = [
					@dataFlowType : UIAPISpecParamData.dataFlowType,
					@name : "array element",
					@typeInfo : tabcopy(UIAPISpecParamData.typeInfo.elementTypeInfo)
				];
				local motherProp = PropertyFromUIAPISpecParamGenerator()[type](paramData);
				
				return Properties.ArrayProperty(
					UIAPISpecParamData.name,
					UIAPISpecParamData.typeInfo.userDefClassId,
					motherProp,
					UIAPISpecParamData.typeInfo.length
				);
			},
			method Enumeration (UIAPISpecParamData) {
				local choices = [];
				local members = UIAPISpecParamData.typeInfo.members;
				for (local i = 0; i < tablength(members); i++)
					choices[tablength(choices)] = members[i].name;
				
				return Properties.EnumeratedProperty(
					UIAPISpecParamData.name,
					UIAPISpecParamData.typeInfo.userDefClassId,
					choices,
					UIAPISpecParamData.dataFlowType
				);
					
			}
		];
		
	return generator;
};

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// PropertyGenerator internal functions
//


//
// GeneratePropertiesFromUIAPISpecParamTable
// (list<Property>) (UIAPISpecParamTable:Object)
//

function GeneratePropertiesFromUIAPISpecParamTable (UIAPISpecParamTable) {
	local propList = list_new();
	
	for (local i = 0; i < tablength(UIAPISpecParamTable); i++) {
		local dataFlowType =  UIAPISpecParamTable[i].dataFlowType;
		local name = UIAPISpecParamTable[i].name;
		local type = UIAPISpecParamTable[i].typeInfo.type;
			
		propList.push_back(PropertyFromUIAPISpecParamGenerator()[type](UIAPISpecParamTable[i]));
		
		print("--", nl);
		print("Param Name: ", name, nl, "Param Type: ", type, nl, "Param dataFlowType: ", dataFlowType, nl);	
		print("--", nl);
	}
	
	return propList;
};

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// PropertyGenerator public functions
//

//
// GenerateOperationsFromUIAPISpec
// (list<OperationProperty>) (specification:UIAPISpec Object)
//

function GenerateOperationsFromUIAPISpec (specification) {
	local operationTable = specification.uiOperations;
	local opList = list_new();
	
	for (local i = 0; i < tablength(operationTable); i++) {
		signature = operationTable[i].signature;

		print("***", nl, "Operation ", signature.name, nl);
		local op = Properties.OperationProperty(signature.name);
			
		// Populate list of argument properties
		local params = signature.parameters;
		local propList = GeneratePropertiesFromUIAPISpecParamTable(params);
		foreach (p, propList)
			op.AddProperty(p);
			
		// Add return property
		if (signature.returnValue.typeInfo.type != "Void") {
			local returnData = tabcopy(signature.returnValue);
			returnData.dataFlowType = "Out";
			local type = signature.returnValue.typeInfo.type;
			local returnProp = PropertyFromUIAPISpecParamGenerator()[type](returnData);
			op.SetReturnProperty(returnProp);
		}
		
		// Add function reference
		op.ref = operationTable[i].func.ref;
		
		opList.push_back(op);
		print("***", nl);
	}
	
	return opList;
};

//
// GenerateNonUIOperationsFromUIAPISpec
// list<NonUIOperationProperty> (specification:UIAPISpec Object)
//

function GenerateNonUIOperationsFromUIAPISpec (specification) {
	local nonUIOperationTable = specification.nonUIOperations;
	local nonUIOpList = list_new();
	
	for (local i = 0; i < tablength(nonUIOperationTable); i++) {
		signature = nonUIOperationTable[i].signature;

		print("***", nl, "Non UI Operation ", signature.name, nl);
		local op = Properties.NonUIOperationProperty(signature.name);
			
		// Populate list of argument properties
		local params = signature.parameters;
		local propList = GeneratePropertiesFromUIAPISpecParamTable(params);
		foreach (p, propList)
			op.AddProperty(p);
			
		// Populate map of property mappings
		for (local j = 0; j < tablength(params); j++) {
			local id = params[j].name;
			
			if (params[j].mapsTo) {
				op.AddPropertyMapping(id, params[j].mapsTo);
				print("Param \" ", id, "\"", "maps to \"", params[j].mapsTo, "\"", nl);
			}
		}
			
		// Add return property
		if (signature.returnValue.typeInfo.type != "Void") {
			local returnData = tabcopy(signature.returnValue);
			returnData.dataFlowType = "Out";
			local type = signature.returnValue.typeInfo.type;
			local returnProp = PropertyFromUIAPISpecParamGenerator()[type](returnData);
			op.SetReturnProperty(returnProp);
			
			if (signature.returnValue.mapsTo) {
				local id = signature.returnValue.name;
				op.AddReturnValueMapping(id, signature.returnValue.mapsTo);
				print("Return Value maps to \"", signature.returnValue.mapsTo, "\"", nl);
			}
		}
		
		// Add function reference
		op.ref = nonUIOperationTable[i].func.ref;
		
		nonUIOpList.push_back(op);
		print("***", nl);
	}
	
	return nonUIOpList;
}

//
// GenerateUIAPIfromUIAPISpec (specification)
// (UIAPI Object) (specification:UIAPISpec Object)
//

function GenerateUIAPIfromUIAPISpec (specification, id) {
	local opList = GenerateOperationsFromUIAPISpec(specification);
	local nonUIOpList = GenerateNonUIOperationsFromUIAPISpec(specification);
	
	local nonUIOpLinksMap = [];
	local it = nonUIOpList.iterator();
	for (
			local i = 0, it.setbegin(nonUIOpList);
			i < tablength(specification.nonUIOperations);
			i++, it.fwd()
		) {
		local linkedId = specification.nonUIOperations[i].func.linkedTo;
		local nonUIOpId = specification.nonUIOperations[i].signature.name;
		nonUIOpLinksMap[linkedId] = it.getval();
	}
	
	local uiAPI = Properties.UIAPI(id);
	foreach (op, opList) {
		local id = op.GetId();
		if (nonUIOpLinksMap[id])
			op.LinkNonUIOperation(nonUIOpLinksMap[id]);
		uiAPI.AddProperty(op);
	}
	return uiAPI;
}

///////////////////////////////////////////////////////////////

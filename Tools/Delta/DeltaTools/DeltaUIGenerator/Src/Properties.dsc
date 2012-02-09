////////////////////////////////////////////////////////////////////////////////////////////////////////
// Property Object contstructors
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;

//
// Constants
//

const nl = "\n";

//
// isStdList
// (void) (l:list)
//

function isStdList (l) {
	if (typeof(l) == "ExternId" and externidtype(l) == "std::list")
		return true;
	return false;
}

///////////////////////////////////////////////////////////////

//
// Property superclass
// (Property Object) (id:String, dataFlowType:String, userDefClassId:String)
//

function Property (id...) { // dataFlowType, userDefClassId
	const dataFlowTypeArgPos = 1;
	const userDefClassIdArgPos = 2;
	
	local dataFlowType = (
		arguments.total() > dataFlowTypeArgPos ? 
		arguments[dataFlowTypeArgPos] : "In"
	);
	local userDefClassId = (
		arguments.total() > userDefClassIdArgPos ? 
		arguments[userDefClassIdArgPos] : "None"
	);
	
	local prop = [
		@id : id,
		@dataFlowType : dataFlowType,
		@userDefClassId : userDefClassId,
		@alternativeDataSources : [],
		method GetDataFlowType
			{ return self.dataFlowType; },
		method GetId
			{ return self.id; },
		method GetUserDefinedClassId
			{ return self.userDefClassId; },
		method GetValue
			{ return self.userData; },
		method SetValue (v)
			{ self.userData = v; },
		method SetDataFlowType (dataFlowType)
			{ self.dataFlowType = dataFlowType; },
		method AddAltDataSource (src_id, src)
			{ self.alternativeDataSources[src_id] = src; },
		method GetAltDataSourceList
			{ return self.alternativeDataSources; },
		// Unimplemented for Property base class
		method GetClassId
			{ },
		method Clone
			{ }
		
		
	];
	
	return prop;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// AggregateProperty inherits from Property
// (AggregateProperty Object) (id:String, userDefClassId:String, dataFlowType:String)
//

function AggregateProperty (id, userDefClassId...) { // dataFlowType
	const dataFlowTypeArgPos = 2;
	local dataFlowType = (arguments.total() > dataFlowTypeArgPos ? arguments[dataFlowTypeArgPos] : "In");
	
	local aggProp = [
		@ordering : list_new(),
		@properties : [],
		method GetClassId
			{ return "Aggregate"; },
		method Clone {
			local clonedProps = list_new();
			foreach (it, self.ordering)
				clonedProps.push_back(self.properties[it].Clone());
			
			local clone = AggregateProperty(self.id, self.userDefClassId, self.dataFlowType);
			
			foreach (x, clonedProps)
				clone.AddProperty(x);
				
			return clone;
		},
		method GetProperties 
			{ return self.properties; },
		method GetUserData 
			{ return self.GetProperties(); },
		method GetOrdering 
			{ return self.ordering; },
		method GetProperty (id) 
			{ return self.properties[id]; },
		method GetFunctionReference
			{ return self.ref; },
		method GetValue {
			local struct = [];
			foreach (it, self.ordering) {
				local id = self.properties[it].GetId();
				struct[id] = self.properties[it].GetValue();
			}
			
			return struct;
			
		},
		method SetValue (structValue) {
			foreach (it, self.ordering) {
				local id = self.properties[it].GetId();
				self.properties[it].SetValue(structValue[id]);
			}
		},
		method AddProperty (p) {
			foreach(local arg, arguments) {
				// dataFlowType gets inherited to the child property
				arg.SetDataFlowType(self.GetDataFlowType());
				self.properties[arg.GetId()] = arg;
				self.ordering.push_back(arg.GetId());
			}
			
			return self;
		},
		method SetDataFlowType (dataFlowType) {
			getbase(self, 0)..SetDataFlowType(dataFlowType);
			// Change the dataFlowType of all children properties
			foreach (it, self.ordering)
				self.properties[it].SetDataFlowType(dataFlowType);
		},
		method iterator {
			local propMap = self.properties;
			return [
				@it : listiter_new(),
				@propMap : propMap, 
				method setbegin (x)		{ @it.setbegin(x.ordering); },
				method setend (x)		{ @it.setend(x.ordering); },
				method checkend (x)  	{ return @it.checkend(x.ordering); },
				method checkbegin(x)	{ return @it.checkbegin(x.ordering); },
				method getval			{ return @propMap[@it.getval()]; },
				method fwd 				{ @it.fwd(); },
				method bwd				{ @it.bwd(); },
				method assign (y)		{ @it.assign(y.it); },
				method equal (y)		{ return @it.equal(y.it); }
			];
		}
	];
	
	local prop = Property(id, dataFlowType, userDefClassId);
	inherit(aggProp, prop);
	
	return aggProp;
	
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// StringProperty inherits from Property
// (StringProperty Object) (id:String, initValue:String, dataFlowType:String)
//

function StringProperty(id, initValue...) { // dataFlowType
	const dataFlowTypeArgPos = 2;
	local dataFlowType = (arguments.total() > dataFlowTypeArgPos ? arguments[dataFlowTypeArgPos] : "In");
	
	const defaultUserDataValue = "";
	
	local strProp = [
		@userData : (initValue?initValue:defaultUserDataValue),
		method GetClassId
			{ return "String"; },
		method Clone {
			local clone = StringProperty(self.GetId(), self.initValue, self.GetDataFlowType());
			
			clone.SetValue(self.GetValue());
				
			return clone;
		}
		
	];

	local prop = Property(id, dataFlowType);
	inherit(strProp, prop);
	
	return strProp;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// BooleanProperty inherits from Property
// (BooleanProperty Object) (id:String, , dataFlowType:String)
//

function BooleanProperty (id...) { // dataFlowType
	const dataFlowTypeArgPos = 1;
	local dataFlowType = (arguments.total() > dataFlowTypeArgPos ? arguments[dataFlowTypeArgPos] : "In");
	
	const defaultUserDataValue = false;
	
	local booleanProp = [
		@userData : defaultUserDataValue, 
		method GetClassId
			{ return "Boolean"; },
		method Clone {
			local clone = BooleanProperty(self.GetId(), self.GetDataFlowType());
			
			clone.SetValue(self.GetValue());
				
			return clone;
		}
	];
	
	local prop = Property(id, dataFlowType);
	inherit(booleanProp, prop);
	
	return booleanProp;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// NumericProperty inherits from Property
// (NumericProperty Object) (id:String, initValue:String, dataFlowType:String)
//

function NumericProperty (id, initValue...) { // dataFlowType
	const dataFlowTypeArgPos = 2;
	local dataFlowType = (arguments.total() > dataFlowTypeArgPos ? arguments[dataFlowTypeArgPos] : "In");
	
	const defaultUserDataValue = ""; 
	
	local numeric_prop = [
		@userData : defaultUserDataValue,
		@initValue : initValue,
		method GetClassId
			{ return "Numeric"; },
		method GetValue {
			if (@userData == "")
				return 0;
			return @userData;
		},
		method Clone {
			local clone = NumericProperty(self.GetId(),
				self.initValue, self.GetDataFlowType());
				
			clone.SetValue(self.GetValue());
			
			return clone;
		}
	];
	
	local prop = Property(id, dataFlowType);
	inherit (numeric_prop, prop);
	
	return numeric_prop;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// EnumeratedProperty inherits from Property
// (EnumeratedProperty Object) 
// (id:String, userDefClassId:String, choices:Table, dataFlowType:String)
//

function EnumeratedProperty (id, userDefClassId, choices...) { // dataFlowType
	const dataFlowTypeArgPos = 3;
	local dataFlowType = (arguments.total() > dataFlowTypeArgPos ? arguments[dataFlowTypeArgPos] : "In");
	
	local enum_prop = [
		@choices : [],
		method GetClassId 
			{ return "Enumerated"; },
		method Add (choiceStr) {
			self.choices[tablength(self.choices)] = choiceStr;
			return self;
		},
		method Clone {
			local clone = EnumeratedProperty(
				self.GetId(), 
				self.GetUserDefinedClassId(),
				tabcopy(self.choices), 
				self.GetDataFlowType()
			);
			
			clone.SetValue(self.GetValue());
				
			return clone;
		}
	];
	
	local prop = Property(id, dataFlowType, userDefClassId);

	if (typeof(choices) == "Object") {
		for (local i = 0; i < tablength(choices); i++)
			enum_prop.choices[i] = choices[i];
	}
	else if (isStdList(choices)) {
		local i = 0;
		foreach (it, choices)
			enum_prop.choices[i++] = it;
	}
	inherit(enum_prop, prop);
	
	enum_prop.SetValue(enum_prop.choices[0]);
	
	return enum_prop;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// UnionProperty inherits from AggregateProperty
// (UnionProperty Object) (id:String, userDefClassId:String, dataFlowType:String)
//

function UnionProperty (id, userDefClassId...) { // dataFlowType
	const dataFlowTypeArgPos = 2;
	local dataFlowType = (arguments.total() > dataFlowTypeArgPos ? 
		arguments[dataFlowTypeArgPos] : "In");
	
	local unionProp = [
		@idxIdMap : [],
		method GetClassId 
			{ return "Union"; },
		method GetValue {
			if (typeof(self.userData) == "Nil") {
				local it = self.ordering.iterator();
				it.setbegin(self.ordering);
				self.SetValue(
					[ 
						@type : it.getval().GetId(),
						@value : it.getval().GetValue()
					]
				);
			}
			return self.userData;
		},
		method SetValue (unionVal) {
			self.userData = unionVal;
			self.properties[unionVal.type].SetValue(unionVal.value);
		},
		method Clone {
			local clonedProps = list_new();
			foreach (it, self.ordering)
				clonedProps.push_back(self.properties[it].Clone());
			
			local clone = UnionProperty(self.id, self.userDefClassId, self.dataFlowType);
			foreach(x, clonedProps)
				clone.AddProperty(x);
				
			clone.SetValue(self.GetValue());
				
			return clone;
		},
		method AddProperty (p) {
			getbase(self, 0)..AddProperty(...);
			foreach(local arg, arguments)
				self.idxIdMap[tablength(self.idxIdMap)] = arg.GetId();
		}
	];
	
	local aggProp = AggregateProperty(id, userDefClassId, dataFlowType);
	inherit(unionProp, aggProp);
	
	return unionProp;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// ListProperty inherits from Property
// (ListProperty Object) (id:String, userDefClassId:String, motherProperty:Property)
//

function ListProperty (id, userDefClassId, motherProperty) {
	local listProp = [
		@pList : list_new(),
		@motherProperty : motherProperty,
		method GetClassId
			{ return "List"; },
		method GetMotherProperty
			{ return self.motherProperty; },
		method IsEmpty 
			{ return @pList.total() == 0; },
		method GetSize 
			{ return @pList.total(); },
		method GetValue {  
			listVal = list_new();
			foreach (p, self)
				listVal.push_back(p.GetValue());
				
			return listVal;
		},
		method SetValue (listVal) {
			@pList.clear();
			
			self.userData = listVal;
			
			foreach (v, listVal) {
				local prop = self.motherProperty.Clone(); 
				prop.SetValue(v);
				self.AddProperty(prop);
			}
		},
		method Clone {
			local clone = ListProperty(self.GetId(), self.GetUserDefinedClassId(),
				self.GetMotherProperty().Clone());
			
			foreach (p, self)
				clone.AddProperty(p.Clone());
				
			return clone;
		},
		method AddProperty (p...) {
			//assert(p.DerivedType() == self.motherProperty.DerivedType());
			foreach(local arg, arguments)
				@pList.push_back(arg);
		},
		method RemoveProperty (it) {
			assert(not it.checkend(self));
		
			@pList.erase(it.it);
			
		},
		method iterator {
			return [
				@it : listiter_new(),
				method setbegin (x)		{ @it.setbegin(x.pList); },
				method setend (x)		{ @it.setend(x.pList); },
				method checkend (x)  	{ return @it.checkend(x.pList); },
				method checkbegin(x)	{ return @it.checkbegin(x.pList); },
				method getval			{ return @it.getval(); },
				method fwd 				{ @it.fwd(); },
				method bwd				{ @it.bwd(); },
				method assign (y)		{ @it.assign(y.it); },
				method equal (y)		{ return @it.equal(y.it); }
			];
		}
	];
	
	local prop = Property(id, motherProperty.GetDataFlowType(),
		userDefClassId);
	
	inherit(listProp, prop);
	
	return listProp;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// VectorProperty
// (VectorProperty Object) (id:String, userDefClassId:String, motherProperty:Property)
//

function VectorProperty (id, userDefClassId, motherProperty) {
	const vecInitSize = 20;
	
	local vecProp = [
		@propVec : vector_new(vecInitSize),
		@totalElements : 0,
		@motherProperty : motherProperty,
		method GetClassId
			{ return "Vector"; },
		method Clone {
			local clone = VectorProperty(self.GetId(), self.GetUserDefinedClassId(),
				self.GetMotherProperty().Clone());
			
			foreach (p, self)
				clone.AddProperty(p.Clone());
				
			return clone;
		},
		method SetValue (tableVal) {
			self.Clear();
			
			for (local i = 0; i < tablength(tableVal); i++) {
				local clone = self.motherProperty.Clone();
				clone.SetValue(tableVal[i]);
				self.AddProperty(clone);
			}
		},
		method GetValue {
			local tableVal = [];
			
			local i = 0;
			foreach (p, self) {
				tableVal[i++] = p.GetValue();
			}
			
			return tableVal;
		},
		method GetMotherProperty
			{ return self.motherProperty; },
		method IsEmpty 
			{ return self.GetTotalElements() == 0; },
		method AddProperty (p) {
			if (@totalElements == @propVec.total())
				@propVec.resize(@propVec.total() + vecInitSize);
			@propVec.setval(@totalElements++, p);
		},
		method RemovePropertyAtIndex (idx) {
			if (idx > -1 and idx < self.GetTotalElements()) {
				for (local i = idx; i < @totalElements - 1; i++)
					@propVec.setval(i, @propVec.getval(i+1));
				@totalElements--;
			}
			if (@totalElements != 0) {
				if (idx < @totalElements)
					return self.GetPropertyAtIndex(idx);
				return self.GetPropertyAtIndex(@totalElements -1);
			}
			return nil;
		},
		method GetPropertyAtIndex (idx) {
			assert(idx > -1 and idx < self.GetTotalElements());
			return @propVec.getval(idx);
		},
		method GetTotalElements
			{ return @totalElements; },
		method Clear ()
			{ @totalElements = 0; },
		method iterator {
				return [
					@it : vectoriter_new(),
					method setbegin (x)	{
						if (x.GetTotalElements() == 0)
							@it.setend(x.propVec);
						@it.setbegin(x.propVec);
					},
					method setend (x)
						{ @it.setend(x.propVec); },
					method checkend (x) {
						if (@it.checkend(x.propVec))
							return true;
						if (@it.getindex() < x.GetTotalElements())
							return false;
						return true;
					},
					method checkbegin (x) { 
						if (x.GetTotalElements() == 0)
							return false;
						return @it.checkbegin(x.propVec); },
					method getval
						{ return @it.getval(); },
					method fwd
						{ @it.fwd(); },
					method bwd 
						{ @it.bwd(); },
					method assign (y)
						{ @it.assign(y.it); },
					method equal (y)
						{ return @it.equal(y.it); }
				];
			}
	];
	
	local prop = Property(id, motherProperty.GetDataFlowType(),
		userDefClassId);

	inherit(vecProp, prop);
	
	return vecProp;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// ArrayProperty
// (ArrayProperty Object)
// (
//	id:String,
//  userDefClassId:String,
//  motherProperty:Property,
//  length:Number
// )
//

function ArrayProperty (id, userDefClassId, motherProperty, length) {
	local arrayProp = [
		@properties : [],
		@propertiesLength : length,
		@motherProperty : motherProperty,
		@curIdx : 0,
		method GetClassId
			{ return "Array"; },
		method GetValue {
			local tableVal = [];
			
			for (local i = 0; i < @propertiesLength; i++) {
				tableVal[i] = self.properties[i].GetValue();
			}
			
			return tableVal;
		},
		method SetValue (tableVal) {
			assert(tablength(tableVal) == @propertiesLength);
			
			for (local i = 0; i < @propertiesLength; i++) {
				self.properties[i].SetValue(tableVal[i]);
			}
			
			self.userData = tableVal;
		},
		method Clone {
			local clone = ArrayProperty(self.GetId(), self.GetUserDefinedClassId(),
				self.GetMotherProperty().Clone(), self.GetLength());
			
			for (local i = 0; i < @propertiesLength; i++) {
				clone.properties[i].SetUserDataFromProperty(self.properties[i]);
			}
			
			return clone;
		},
		method GetMotherProperty
			{ return self.motherProperty; },
		method IsEmpty
			{ return tablength(self.properties) == 0; },
		method GetLength
			{ return self.propertiesLength; },
		method GetPropertyAtIndex (index)
			{ return self.properties[index]; }
	];
	
	for (local i = 0; i < arrayProp.GetLength(); i++)
		arrayProp.properties[i] = motherProperty.Clone();
		
	local prop = Property(id, motherProperty.GetDataFlowType(), userDefClassId);
	inherit(arrayProp, prop);
	
	return arrayProp;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// OperationProperty inherits from AggregateProperty
// (OperationProperty Object) (id:String, parentUIAPIId:String)
//

function OperationProperty (id...) { // parentUIAPIId
	const parentUIAPIIdArgPos = 1;
	local parentUIAPIId = (
		arguments.total() > parentUIAPIIdArgPos ? 
		arguments[parentUIAPIIdArgPos] : "None"
	);

	local opProp = [
		@parentUIAPIId : parentUIAPIId,
		@linkedNonUIOps : list_new(),
		
		method GetClassId
			{ return "Operation"; },
		method GetParentUIAPIId
			{ return @parentUIAPIId; },
		method SetParentUIAPIId (id)
			{ @parentUIAPIId = id; },
		method Clone
			{ /* IMPLEMENTME */ error("Function not implemented"); },
		method GetReturnProperty
			{ return self.returnProperty; },
		method SetReturnProperty (p) 
			{ self.returnProperty = p; },
		method AddProperty (p) {
			foreach(local arg, arguments) {
				self.properties[arg.GetId()] = arg;
				self.ordering.push_back(arg.GetId());
			}
			
			return self;
		},
		method GetValue {
			local op = [];
			
			if (typeof(self.returnProperty) != "Nil")
				op[0] = self.returnProperty.GetValue();
			
			local i = 1;
			foreach (it, self.ordering)
				op[i++] = self.properties[it].GetValue();
			
			return op;
		},
		method SetValue (operationValue) {
			if (typeof(operationValue[0]) != "Nil")
				self.returnProperty.SetValue(operationValue[0]);
			local i = 1;
			foreach (it, self.ordering)
				self.properties[it].SetValue(operationValue[i++]);
		},
		method LinkNonUIOperation (nonUIOperation)
			{ @linkedNonUIOps.push_back(nonUIOperation); },
		method GetLinkedNonUIOperationList
			{ return @linkedNonUIOps; }
		
	];
	
	local aggProp = AggregateProperty(id, "None");
	inherit(opProp, aggProp);
	
	return opProp;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// NonUIOperationProperty inherits from OperationProperty
// (NonUIOperationProperty Object) (id:String, parentUIAPIId:String)
//

function NonUIOperationProperty (id...) { // parentUIAPIId
	const parentUIAPIIdArgPos = 1;
	local parentUIAPIId = (
		arguments.total() > parentUIAPIIdArgPos ? 
		arguments[parentUIAPIIdArgPos] : "None"
	);

	local nonUIOpProp = [
		@propMappings : [],
		@retValMapping : [],
		
		method GetClassId
			{ return "Operation"; },
		method AddPropertyMapping (nonUIPropId, UIPropId)
			{ @propMappings[nonUIPropId] = UIPropId; },
		method AddReturnValueMapping (nonUIPropId, UIPropId)
			{ @retValMapping[nonUIPropId] = UIPropId; },
		method GetPropertyMappings
			{ return @propMappings; },
		method GetReturnValueMapping
			{ return @retValMapping; }
	];
	
	local opProp = OperationProperty(id, parentUIAPIId);
	inherit(nonUIOpProp, opProp);
	
	return nonUIOpProp;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// UIAPI inherits from AggregateProperty
// (UIAPI Object) (id:String)
//

function UIAPI (id) {
	local UIAPI = [
		method GetClassId
			{ return "UIAPI"; },
		method AddProperty (p) {
			foreach(local opProp, arguments) {
				if (opProp.GetClassId() != "Operation")
					error("UIAPI accepts only Operation properties");
				
				opProp.SetParentUIAPIId(self.GetId());
				self.properties[opProp.GetId()] = opProp;
				self.ordering.push_back(opProp.GetId());
			}
			
			return self;
		},
		method SetUIComponent (uiComponent)
			{ self.uiComponent = uiComponent; },
		method GetUIComponent 
			{ return self.uiComponent; }
	];
	
	local aggProp = AggregateProperty(id, "None");
	inherit(UIAPI, aggProp);
	
	return UIAPI;
}

///////////////////////////////////////////////////////////////


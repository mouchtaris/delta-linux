////////////////////////////////////////////////////////////////////////////////////////////////////////
// A Holder for Property selections
////////////////////////////////////////////////////////////////////////////////////////////////////////



using std;

///////////////////////////////////////////////////////////////

//
// ExtractClassId
// (string) (prop:Property)
//

function ExtractClassId (prop) {
	if (prop.GetUserDefinedClassId() != "None")
		return prop.GetUserDefinedClassId();
	return prop.GetClassId();
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// ExtractAllProperties
// (Table) (prop:Property, classId:String)
//

function ExtractAllProperties (prop, classId) {
	local handlers = [
		method Union (unionProp, classId) {
			local id = unionProp.GetValue().type;
			local prop = unionProp.GetProperty(id) ;
			
			if (ExtractClassId(prop) == classId)
				return [ prop ];
			return [];
		},
		method List (listProp, classId) {
			local props = [];
			local motherProp = listProp.GetMotherProperty();
			
			if (ExtractClassId(motherProp) == classId) {
				local i = 0;
				foreach (prop, listProp) {
					props[i++] = prop;
				}
				return props;
			}
			else {
				local i = 0;
				foreach (prop, listProp) {
					innerProps = ExtractAllProperties(prop, classId);
					foreach (p, innerProps)
						props[i++] = p;
				}
				return props;
			}
		},
		method Vector (prop, classId)
			{ return self.List(prop, classId); },
		method Array (arrayProp, classId) { 
			local props = [];
			local motherProp = arrayProp.GetMotherProperty();
			
			if (ExtractClassId(motherProp) == classId) {
				for (local i = 0; i < arrayProp.GetLength(); i++) {
					local prop = arrayProp.GetPropertyAtIndex(i);
					props[i] = prop;
				}
				return props;
			}
			else {
				local j = 0;
				for (local i = 0; i < arrayProp.GetLength(); i++) {
					local prop = arrayProp.GetPropertyAtIndex(i);
					innerProps = ExtractAllProperties(prop, classId);
					foreach(p, innerProps)
						props[j++] = p;
				}
				return props;
			}
		},
		method Aggregate
			{ return []; },
		method Enumerated
			{ return []; }
	];
	if (ExtractClassId(prop) == classId)
		return [ prop ];
			
	if (prop.GetUserDefinedClassId() != "None") {
		local props = [];
		local i = 0;
			
		innerProps = handlers[prop.GetClassId()](prop, classId);
		foreach (p, innerProps)
			props[i++] = p;
		return props;
	}
		
	assert(false);
	return [ prop ];
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// ExtractAllClassIds
// (Table) (prop:Property)
//

function ExtractAllClassIds (prop) {
	local handlers = [
		method Union (unionProp) {
			local classIds = [];
			local i = 0;
			foreach (prop, unionProp.GetProperties()) {
				local propClassIds = ExtractAllClassIds(prop);
				foreach (classId, propClassIds)
					classIds[i++] = classId;
			}
				
			return classIds;
		},
		method List (prop) {
			local classIds = [];
			local i = 0;
			local propClassIds = ExtractAllClassIds(prop.GetMotherProperty());
			foreach (classId, propClassIds)
				classIds[i++] = classId;
			return classIds;
		},
		method Vector (prop)
			{ return self.List(prop); },
		method Array (prop) 
			{ return self.List(prop); },
		method Aggregate
			{ return []; },
		method Enumerated
			{ return []; }
	];
		
	if (prop.GetUserDefinedClassId() != "None") {
		local classIds = [];
		local i = 0;
			
		classIds[i++] = prop.GetUserDefinedClassId();
		propClassIds = handlers[prop.GetClassId()](prop);
		foreach (c, propClassIds)
			classIds[i++] = c;
		return classIds;
	}
	return [ prop.GetClassId() ];
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// GetInstance
// (PropertySelectionHolder singleton Object) ()
//

function GetInstance {
	static propertySelectionHolder;
	
	if (isundefined(propertySelectionHolder)) {
		propertySelectionHolder = [
			@PropertySelectionRegister : [],
			
			method RegisterAvailableClassIds (uiAPI) {
				local availableClassIds = [];
				foreach (opProp, uiAPI) {
					foreach (prop, opProp.GetProperties()) {
						local dft = prop.GetDataFlowType();
						local classId;
						if (dft == "Out" or dft == "InOut") {
							local classIds = ExtractAllClassIds(prop);
							foreach (classId, classIds)
								if (availableClassIds[classId] == nil)
									availableClassIds[classId] = true;
						}
					}
					if (opProp.GetReturnProperty()) {
						local classIds = ExtractAllClassIds(opProp.GetReturnProperty());
						foreach (classId, classIds)
							if (availableClassIds[classId] == nil)
								availableClassIds[classId] = true;
					}
				}
				if (not @PropertySelectionRegister[uiAPI.GetId()])
					@PropertySelectionRegister[uiAPI.GetId()] = [];
				@PropertySelectionRegister[uiAPI.GetId()]["availableClassIds"] = availableClassIds;
			},
			
			method RegisterProperty (uiAPIId, prop) {
				assert(@PropertySelectionRegister[uiAPIId]);
				assert(
					(prop.GetDataFlowType() == "Out") or
					(prop.GetDataFlowType() == "InOut")
				);
				
				if (not @PropertySelectionRegister[uiAPIId]["availableProperties"])
					@PropertySelectionRegister[uiAPIId]["availableProperties"] = [];
				
				
				// A property can be registered for multiple classIds.
				// For example a List Property that contains String Properties
				// will be registered both for its userDefinedClassId type and
				// the String type.
				// Also all properties in the classId-property map need not be of classId
				// ClassId. Take the List Property mentioned above as an example.
					
				local allClassIds = ExtractAllClassIds(prop);
				foreach (classId, allClassIds) {
					if (not @PropertySelectionRegister[uiAPIId]["availableProperties"][classId])
						@PropertySelectionRegister[uiAPIId]["availableProperties"][classId] = [];
					// emulates a set's behavior
					@PropertySelectionRegister[uiAPIId]["availableProperties"][classId][prop] = prop;
					
					self.NotifyLaunchersOfPropertyAvailability(uiAPIId, classId);
				}
			},
			
			method RegisterPropertySelectionLauncher (launcher) {
				local uiAPIId = launcher.GetUIAPIId();
				assert(@PropertySelectionRegister[uiAPIId]);
				
				if (not @PropertySelectionRegister[uiAPIId]["launchers"])
					@PropertySelectionRegister[uiAPIId]["launchers"] = [];
					
				local classId = ExtractClassId(launcher.GetMicroUI().GetProperty());
				if (not @PropertySelectionRegister[uiAPIId]["launchers"][classId])
					@PropertySelectionRegister[uiAPIId]["launchers"][classId] = list_new();
					
				@PropertySelectionRegister[uiAPIId]["launchers"][classId].push_back(launcher);
			},
			
			method NotifyLaunchersOfPropertyAvailability (uiAPIId, classId) {
				local apiLaunchers = @PropertySelectionRegister[uiAPIId]["launchers"];
				if (apiLaunchers and local launchers = apiLaunchers[classId])
					foreach (local launcher, launchers)
						launcher.enable();
				
			},
			
			method GetAvailablePropertyList (uiAPIId, classId) {
				local availableProperties = 
					@PropertySelectionRegister[uiAPIId]["availableProperties"][classId];
				assert(availableProperties);
				local extractedPropertyList = list_new();
				
				foreach (prop, availableProperties) {
					props = ExtractAllProperties(prop, classId);
					foreach (p, props)
						extractedPropertyList.push_back(p);
				}
				
				return extractedPropertyList;
			},
			
			method IsClassIdAvailable (uiAPIId, prop)
				{ return @PropertySelectionRegister[uiAPIId]["availableClassIds"][ExtractClassId(prop)]; }
		];
	}
	
	return propertySelectionHolder;
}

///////////////////////////////////////////////////////////////
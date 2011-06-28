////////////////////////////////////////////////////////////////////////////////////////////////////////
// Top level UI Object constructors
// (Attempt at using the composite pattern)
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;
using wx;

//
// Globals and consts
//

defaultPosition = point_construct(-1, -1);
defaultSize = size_construct(-1, -1);
const defaultBorderWidth = 5;
const noProportion = 0;

//
// VM Import
//

PropertyUIMappingRules = libs::import("PropertyUIMappingRules");
PropertySelectionHolder = libs::import("PropertySelectionHolder");
PropertySelectionUIs = libs::import("PropertySelectionUIs");
assert(PropertyUIMappingRules);
assert(PropertySelectionHolder);
assert(PropertySelectionUIs);


///////////////////////////////////////////////////////////////

//
// TopLevelUI superclass
// (TopLevelUI Object) (id:String)
//

function TopLevelUI (id) {
	local topLevelUI = [
		@id : id,
		method AddTopLevelUI (ui)
			{},
		method GetTopLevelUI (id)
			{},
		method GetId
			{ return @id; }
	];
	
	return topLevelUI;
}

///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////

//
// OperationUI inherits from TopLevelUI
// (OperationUI Object) (wxparent:wxWindow, opProp:OperationProperty)
//

function OperationUI (wxparent, opProp) {
	function Invoke (opProp) {
		local i = 0, args = [];
		foreach (id, opProp.GetOrdering()) {
			local prop = opProp.GetProperty(id);
			local dft = prop.GetDataFlowType();
				
			if (( dft == "Out") or (dft == "InOut")) {
				args[i++] = [ { 0 : prop.GetValue() } ];
			}
			else
				args[i++] = prop.GetValue();
		}
			
		local retVal;
		try { retVal = opProp.GetFunctionReference()(|args|); }
		trap x { print(x); }
			
		local retProp = opProp.GetReturnProperty(); 
		if (typeof(retProp) != "Nil" ) {
			retProp.SetValue(retVal);
			//self.retMicroUI.SetUIValue(retVal);
			//propertySelectionHolder.RegisterProperty(self.opProp.GetParentUIAPIId(), retProp);
		}
		
		local i = 0;
		foreach (id, opProp.GetOrdering()) {
			local prop = opProp.GetProperty(id);
			local dft = prop.GetDataFlowType(); 
			if ((dft == "Out") or (dft == "InOut")) {
				prop.SetValue(args[i][0]);
				//@IdMicroUIMap[id].SetUIValue(args[i][0]);
				//propertySelectionHolder.RegisterProperty(self.opProp.GetParentUIAPIId(), prop);
			}
			i++;
		}
	}
	
	local panel = panel_construct(wxparent);
	
	local topLevelUI = TopLevelUI(opProp.GetId());
	local topLevelUIImpl = [
		@opProp : opProp,
		@microUIOrdering : list_new(),
		@visiblePropertyList : list_new(),
		@IdMicroUIMap : [],
		@topSizer : boxsizer_construct("VERTICAL"),
		
		method AddTopLevelUI (ui)
			{ throw "OperationUI is a leaf TopLevelUI"; },
		method GetTopLevelUI (id)
			{ throw "OperationUI is a leaf TopLevelUI"; },
		method AddMicroUI (mUI) {
			@microUIOrdering.push_back(mUI.GetId());
			@IdMicroUIMap[mUI.GetId()] = mUI;
		},
		method OnInvoke {
			local propertySelectionHolder = PropertySelectionHolder.GetInstance();
			
			//do this only for In(Out) ?
			foreach (mUI, @IdMicroUIMap)
				mUI.UpdateInternalValue();
			
			// Invocation of non UI functions
			
			foreach (nonUIOp, @opProp.GetLinkedNonUIOperationList()) {
				Invoke(nonUIOp);
				foreach (pId, nonUIOp.GetOrdering()) {
					local prop = nonUIOp.GetProperty(pId);
					local dft = prop.GetDataFlowType();
					
					if ((dft == "Out") or (dft == "InOut")) {
						if (mappedPropId = nonUIOp.GetPropertyMappings()[pId]) {
							local mappedProp = @opProp.GetProperty(mappedPropId);
							mappedProp.SetValue(prop.GetValue());
						}
					}
				}
				
				local retProp = nonUIOp.GetReturnProperty();
				if (typeof(retProp) != "nil") {
					if (mappedPropId = nonUIOp.GetReturnValueMapping()[retProp.GetId()]) {
						local mappedProp = @opProp.GetProperty(mappedPropId);
						mappedProp.SetValue(retProp.GetValue());
					}
				}
				
			}
			
			// Invocation of the operation
			Invoke(@opProp);
			
			// Update of micro UI Values and register suitable properties in the property selection holder
			local retProp = @opProp.GetReturnProperty(); 
			if (typeof(retProp) != "Nil" ) {
				self.retMicroUI.SetUIValue(retProp.GetValue());
				propertySelectionHolder.RegisterProperty(@opProp.GetParentUIAPIId(), retProp);
			}
		
			local i = 0;
			foreach (prop, @visiblePropertyList) {
				//local prop = @opProp.GetProperty(id);
				local id = prop.GetId();
				local dft = prop.GetDataFlowType();
				@IdMicroUIMap[id].SetUIValue(prop.GetValue());
				if ((dft == "Out") or (dft == "InOut")) {
					//prop.SetValue(args[i][0]);
					//@IdMicroUIMap[id].SetUIValue(args[i][0]);
					propertySelectionHolder.RegisterProperty(@opProp.GetParentUIAPIId(), prop);
				}
				i++;
			}
			
			/*
			local i = 0, args = [];
			foreach (id, @opProp.GetOrdering()) {
				local prop = @opProp.GetProperty(id);
				local dft = prop.GetDataFlowType();
				
				if (( dft == "Out") or (dft == "InOut")) {
					args[i++] = [ { 0 : prop.GetValue() } ];
				}
				else
					args[i++] = prop.GetValue();
			}
			
			local retVal;
			try { retVal = @opProp.GetFunctionReference()(|args|); }
			trap x { print(x); }
			
			local retProp = @opProp.GetReturnProperty(); 
			if (typeof(retProp) != "Nil" ) {
				retProp.SetValue(retVal);
				self.retMicroUI.SetUIValue(retVal);
				
				propertySelectionHolder.RegisterProperty(self.opProp.GetParentUIAPIId(), retProp);
			}
			
			local i = 0;
			foreach (id, @opProp.GetOrdering()) { //add check for visibility?
				local prop = @opProp.GetProperty(id);
				local dft = prop.GetDataFlowType(); 
				if ((dft == "Out") or (dft == "InOut")) {
					prop.SetValue(args[i][0]);
					@IdMicroUIMap[id].SetUIValue(args[i][0]);
					
					propertySelectionHolder.RegisterProperty(self.opProp.GetParentUIAPIId(), prop);
				}
				i++;
			}
			*/
		}
	];
	
	inherit(topLevelUIImpl, topLevelUI);
	inherit(panel, topLevelUIImpl);
	
	// Visible property list population
	local hiddenPropertyIdSet = [];
	
	foreach (nonUIOp, panel.opProp.GetLinkedNonUIOperationList()) {
		foreach (pId, nonUIOp.GetPropertyMappings()) {
			hiddenPropertyIdSet[pId] = pId;
			print(pId, " will not be included\n");
		}
		foreach (pId, nonUIOp.GetReturnValueMapping()) {
			hiddenPropertyIdSet[pId] = pId;
			print(pId, " will not be included\n");
		}
		
		foreach (pId, nonUIOp.GetOrdering()) {
			local p = nonUIOp.GetProperty(pId);
			local dft = p.GetDataFlowType();
			if ( dft == "In" or dft == "InOut") {
				panel.visiblePropertyList.push_back(p);
				print(pId, " will be included\n");
			}
		}
	}
	
	foreach (pId, panel.opProp.GetOrdering()) {
		if (not hiddenPropertyIdSet[pId])
			panel.visiblePropertyList.push_back(panel.opProp.GetProperty(pId));
	}
	
	
	// Parameter UI construction

	foreach (prop, panel.visiblePropertyList) {
		local propertySelectionHolder = PropertySelectionHolder.GetInstance();
		local labelSizer = boxsizer_construct("HORIZONTAL");
		local st = statictext_construct(
				panel,
				"ID_STATIC",
				prop.GetId()
		);
		f = st.getfont();
		f.setweight("BOLD");
		st.setfont(f);
		
		labelSizer.add(
			st,
			noProportion,
			flags("ALL", "ALIGN_CENTER_VERTICAL"),
			defaultBorderWidth
		);
		
		local microUI = PropertyUIMappingRules.CreateUI(panel, prop);

		if (
				propertySelectionHolder.IsClassIdAvailable(
					opProp.GetParentUIAPIId(),
					prop
				) and
				(prop.GetDataflowType != "Out")
		) {
			labelSizer.add(
			PropertySelectionUIs.PropertySelectionButton(
				panel, 
				opProp.GetParentUIAPIId(),
				microUI
			),
			noProportion,
			flags("ALL", "ALIGN_CENTER_VERTICAL"),
			defaultBorderWidth
			);
		}
		
		panel.topSizer.add(labelSizer, noProportion, "ALL", defaultBorderWidth);
		panel.AddMicroUI(microUI);
		panel.topSizer.add(microUI, noProportion, "ALL", defaultBorderWidth);
	}

	// Return value UI construction
	
	panel.topSizer.add(
		staticline_construct(
			panel, 
			"ID_STATIC",
			defaultPosition,
			defaultSize,
			"LI_HORIZONTAL"
		),
		noProportion,
		flags("GROW", "ALL"),
		defaultBorderWidth
	);
	
	if (opProp.returnProperty != nil) {
		local st = statictext_construct(
			panel,
			"ID_STATIC",
			opProp.returnProperty.GetId()
		);
		f = st.getfont();
		f.setweight("BOLD");
		st.setfont(f);
		
		panel.topSizer.add(
			st,
			noProportion,
			"ALL",
			defaultBorderWidth
		);
		panel.retMicroUI = PropertyUIMappingRules.CreateUI(panel, opProp.returnProperty);
		panel.topSizer.add(panel.retMicroUI, noProportion, "ALL", defaultBorderWidth);
		panel.topSizer.add(
			staticline_construct(
				panel, 
				"ID_STATIC",
				defaultPosition,
				defaultSize,
				"LI_HORIZONTAL"
			),
			noProportion,
			flags("GROW", "ALL"),
			defaultBorderWidth
		);
	}
	
	// Invoke button construction
	local opButton = button_construct(panel, "ID_ANY", opProp.GetId());
	opButton.connect("ID_ANY", "EVT_COMMAND_BUTTON_CLICKED", panel.OnInvoke);
	panel.topSizer.add(
		opButton, 
		noProportion,
		flags("ALIGN_RIGHT", "ALIGN_BOTTOM", "ALL"), 
		defaultBorderWidth
	);
	
	panel.setsizerandfit(panel.topSizer);

	return panel;
}

///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////

//
// TreeBookCompositeUI inherits from TopLevelUI
// (TreeBookCompositeUI Object)
// (
//	wxparent:wxwindow,
//  id:String, 
// 	TopLevelUIList:list<TopLevelUI>
// )
//

function TreeBookCompositeUI (wxparent, id...) { //TopLevelUIList
	const TopLevelUIListArgPos = 2;
	
	local treebook = treebook_construct(wxparent,
			"ID_ANY", defaultPosition, defaultSize);
	
	local topLevelUI = TopLevelUI(id);
	local topLevelUIImpl = [
		@topLevelUIMap : [],
		
		method AddTopLevelUI (ui) {
			@topLevelUIMap[ui.GetId()] = ui;
			self.addpage(ui, ui.GetId());
			self.fit();
		},
		method GetTopLevelUI (id)
			{ return @topLevelUIMap[id]; }
	];
	
	inherit(topLevelUIImpl, topLevelUI);
	inherit(treebook, topLevelUIImpl);
	
	if (arguments.total() > TopLevelUIListArgPos) {
		local TopLevelUIList = arguments[TopLevelUIListArgPos];
		foreach (ui, TopLevelUIList)
			treebook.AddTopLevelUI(ui);
	}
	
	return treebook;
}

///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Micro UI Object constructors
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;
using wx;
using corba;

//
// Globals and constants
//

defaultPosition = point_construct(-1, -1);
defaultSize = size_construct(-1, -1);
const defaultBorderWidth = 5;
const noProportion = 0;

///////////////////////////////////////////////////////////////

//
// MicroUI superclass
// (MicroUI Object) (wxparent:wxWindow, property:Property)
//

function MicroUI (wxparent, property) {
	local microUI = [
		@wxparent : wxparent,
		@property : property,
		method GetId
			{ return @property.GetId(); },
		method GetProperty
			{ return @property; },
		method SetUIValue (v)
			{ },
		method UpdateInternalValue
			{ },
		method Resize //subject to change
			{ }
	];
	
	return microUI;
}

function CompositeMicroUI (wxparent, property) {
	local microUI = MicroUI(wxparent, property);
	
	local compMicroUI = [
		@microUIOrdering : list_new(),
		@IdMicroUIMap : [],
		method AddMicroUI (microUI) {
			@microUIOrdering.push_back(microUI.GetId());
			@IdMicroUIMap[microUI.GetId()] = microUI;
		}
	];
	
	inherit(compMicroUI, microUI);
	
	return compMicroUI;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// MicroUIs for StringProperty
// (MicroUI Object) (wxparent:wxWindow, stringProp:StringProperty)
//

///////////////////////////////////////////////////////////////

function TextCtrl (wxparent, stringProp) {
	local textCtrlDefaultSize = size_construct(150, -1);
	
	local textCtrl = textctrl_construct(wxparent, "ID_ANY", stringProp.GetValue());
	
	local microUI = MicroUI(wxparent, stringProp);
	
	local microUIImpl = [
		method SetUIValue (v)
			{ self.setvalue(v); },
		method UpdateInternalValue
			{ self.property.SetValue(self.getvalue()); }
	];
	
	inherit(microUIImpl, microUI);
	inherit(textCtrl, microUIImpl);
	
	textCtrl.setminsize(textCtrlDefaultSize);
	
	return textCtrl;
}

///////////////////////////////////////////////////////////////

function StaticText (wxparent, stringProp) {
	local staticText = statictext_construct(wxparent, "ID_ANY", stringProp.GetValue());
	
	local microUI = MicroUI(wxparent, stringProp);
	
	local microUIImpl = [
		method SetUIValue (v)
			{ self.setlabel(v); },
		method UpdateInternalValue
			{ self.property.SetValue(self.getlabel()); }
	];
	
	inherit (microUIImpl, microUI);
	inherit(staticText, microUIImpl);
	
	// this is quite useful...
	staticText.setminsize(size_construct(300, -1));
	
	return staticText;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// MicroUIs for BooleanProperty
// (MicroUI Object) (parent:wxWindow, boolProp:BooleanProperty)
//

///////////////////////////////////////////////////////////////

function CheckBox (wxparent, booleanProp) {
	checkBox = checkbox_construct(wxparent, "ID_ANY", booleanProp.GetId());
	
	local microUI = MicroUI(wxparent, booleanProp);
	
	local microUIImpl = [
		method SetUIValue (v)
			{ self.setvalue(v); },
		method UpdateInternalValue
			{ self.property.SetValue(self.getvalue()); }
	];
	
	inherit (microUIImpl, microUI);
	inherit(checkBox, microUIImpl);
	
	return checkBox;
}

///////////////////////////////////////////////////////////////

function BooleanStaticText (wxparent, booleanProp) {
	local staticText = statictext_construct(wxparent, "ID_ANY", "");

	local microUI = MicroUI(wxparent, booleanProp);
	
	local microUIImpl = [
		method SetUIValue (booleanVal)
			{ self.setlabel(tostring(booleanVal)); },
		method UpdateInternalValue {
			local booleanVal = (self.getlabel() == "true"?true:false);
			self.property.SetValue(booleanVal);
		}
	];
	
	inherit (microUIImpl, microUI);
	inherit(staticText, microUIImpl);
	
	// this is quite useful...
	staticText.setminsize(size_construct(300, -1));
	
	return staticText;
}

///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//
// MicroUIs for EnumeratedProperty
// (MicroUI Object) (parent:wxWindow, enumProp:EnumeratedProperty)
//

///////////////////////////////////////////////////////////////

//
// MicroUIWithItems inherits from MicroUI
// superclass for MicroUIs that also inherit from wxControlWithItems
//

function MicroUIWithItems (wxparent, enumProp) {
	local microUI = MicroUI(wxparent, enumProp);
	
	local EnumPropMicroUIImpl = [
		method SetUIValue (strVal)
			{ self.setstringselection(strVal); },
		method UpdateInternalValue {
			self.property.SetValue(corba::enum(self.getstringselection()));
		}
	];
	
	inherit(EnumPropMicroUIImpl, microUI);
	
	return EnumPropMicroUIImpl;
}

///////////////////////////////////////////////////////////////

function RadioBox (wxparent, enumProp) {
	local radiobox = radiobox_construct(
		wxparent,
		"ID_ANY",
		enumProp.GetUserDefinedClassId(),
		defaultPosition,
		defaultSize, 
		enumProp.choices
	); 
	
	local microUIWithItems = MicroUIWithItems(wxparent, enumProp);
	
	inherit(radiobox, microUIWithItems);
	
	return radiobox;
}

///////////////////////////////////////////////////////////////

function Choice (wxparent, enumProp) {
	// TODO : Display userDefClassId somewhere
	local choice = choice_construct(
		wxparent,
		"ID_ANY",
		defaultPosition,
		defaultSize,
		enumProp.choices
	);
	
	local microUIWithItems = MicroUIWithItems(wxparent, enumProp);
	
	inherit(choice, microUIWithItems);
	
	if (tablength(enumProp.choices)) {
		choice.setstringselection(enumProp.choices[0]);
		choice.setminsize(choice.getbestsize());
	}
	
	return choice;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// MicroUIs for NumericProperty
// (wx Object + interface) (parent:wxWindow, numProp:NumericProperty)
//

///////////////////////////////////////////////////////////////

function NumericCtrl (wxparent, numProp) {
	local numericCtrlDefaultSize = size_construct(150, -1);
	local numericCtrl = textctrl_construct(wxparent, "ID_ANY", numProp.initValue);
	
	local microUI = MicroUI(wxparent, numProp);
	
	local microUIImpl = [
		method SetUIValue (numVal)
			{ self.setvalue(tostring(numVal)); },
		method UpdateInternalValue
			{ self.property.SetValue(strtonum(self.getvalue())); },
		method OnChar (parent, e) {
			static hasDot = false;
			local keycode = e.getkeycode();

			if ( strisdigit(keycode) or
				keycode == strbyte(".", 0) or
				keycode == strbyte(",", 0) or
				keycode == 8) { // FIXME: strbyte("\b", 0) does not return number for backspace
				e.skip();
				if ( keycode == strbyte(".", 0) )
					hasDot = true;
			}
			else
				bell();
		}
	];
	
	inherit (microUIImpl, microUI);
	inherit(numericCtrl, microUIImpl);
	
	numericCtrl.connect("ID_ANY", "EVT_KEY_DOWN", numericCtrl.OnChar);
	
	numericCtrl.setminsize(numericCtrlDefaultSize);
	
	return numericCtrl;
}

///////////////////////////////////////////////////////////////

function NumericStaticText (wxparent, numProp) {
	local staticText = statictext_construct(wxparent, "ID_ANY", tostring(numProp.GetValue()));
	
	local microUI = MicroUI(wxparent, numProp);
	
	local microUIImpl = [
		method SetUIValue (numVal)
			{ self.setlabel(tostring(numVal)); },
		method UpdateInternalValue
			{ self.property.SetValue(strtonum(self.getlabel())); }
	];
	
	inherit (microUIImpl, microUI);
	inherit(staticText, microUIImpl);
	
	// this is quite useful...
	staticText.setminsize(size_construct(300, -1));
	
	return staticText;
}

///////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//
// MicroUIs for UnionProperty
// (wx Object + interface) (parent:wxWindow, unionProp:UnionProperty)
//

///////////////////////////////////////////////////////////////

function ChoiceBook (wxparent, unionProp) {
	
	local choicebook = choicebook_construct(wxparent, "ID_ANY");
	
	local compMicroUI = CompositeMicroUI(wxparent, unionProp);
	
	local microUIImpl = [
		@indexIdMap : [],
		@idIndexMap : [],
		@curIdx : 0,
		method SetUIValue (unionVal) {
			self.IdMicroUIMap[unionVal.type].SetUIValue(unionVal.value);
			self.changeselection(@idIndexMap[unionVal.type]);
		},
		method UpdateInternalValue { 
			local id = @indexIdMap[self.getselection()];
			self.IdMicroUIMap[id].UpdateInternalValue();
			local unionVal = [
				@type : id,
				@value : self.IdMicroUIMap[id].GetProperty().GetValue()
			];
			
			self.property.SetValue(corba::union(unionVal.type, unionVal.value));
		},
		method AddMicroUI (microUI) {
			getbase(self, 0)..AddMicroUI(microUI);
			@indexIdMap[@curIdx] = microUI.GetId();
			@idIndexMap[@indexIdMap[@curIdx] ] = @curIdx;
			++@curIdx;
			
			local label = microUI.GetProperty().GetClassId() + " : " + microUI.GetId();
			self.addpage(microUI, label);
			
			local size = self.getchoicectrl().getbestsize();
			size.setheight(-1);
			self.setminsize(size);
		}
	];
	
	inherit(microUIImpl, compMicroUI);
	inherit(choicebook, microUIImpl);
	
	return choicebook;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// MicroUIs for AggregateProperty
// (MicroUI Object) (wxparent:wxWindow, aggregateProp:AggregateProperty)
//

///////////////////////////////////////////////////////////////

function AggregatePanel (wxparent, aggregateProp) {
	
	local panel = panel_construct(wxparent);
	
	local compMicroUI = CompositeMicroUI(wxparent, aggregateProp);
	
	local microUIImpl = [
		@topSizer : staticboxsizer_construct("VERTICAL", panel, aggregateProp.GetUserDefinedClassId()),
		method SetUIValue (structVal) {
			foreach (id, self.microUIOrdering)
				self.IdMicroUIMap[id].SetUIValue(structVal[id]);
		},
		method UpdateInternalValue {
			foreach (id, self.microUIOrdering)
				self.IdMicroUIMap[id].UpdateInternalValue();
		},
		method AddMicroUI (microUI) {
			getbase(self, 0)..AddMicroUI(microUI);

			local staticText = statictext_construct(@topSizer.getstaticbox(),
				"ID_STATIC", microUI.GetId());
				
			@topSizer.add(staticText, noProportion, "ALL", defaultBorderWidth);
			microUI.reparent(@topSizer.getstaticbox());
			@topSizer.add(microUI, noProportion, "ALL", defaultBorderWidth);
			
			self.setsizerandfit(@topSizer);
		}
	];
	
	inherit (microUIImpl, compMicroUI);
	inherit(panel, microUIImpl);
			
	panel.setsizerandfit(panel.topSizer);
	
	return panel;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// MicroUIs for ListProperty
// (MicroUI Object) 
// ( 
//	 wxparent:wxWindow, 
//   listProp:ListProperty,
//   viewUI:MicroUI,
//   previewUI:MicroUI
// )
//

///////////////////////////////////////////////////////////////

function ListPanel (wxparent, listProp, viewUI...) { //previewUI
	const defaultSpace = 10; 
	const previewUIArgPos = 3;
	
	local previewUI = (arguments.total() > previewUIArgPos ? arguments[previewUIArgPos] : []);
	
	local panel = panel_construct(wxparent);
	local topSizer = staticboxsizer_construct("VERTICAL", panel, listProp.GetUserDefinedClassId());
	local viewSizer = boxsizer_construct("HORIZONTAL");
	local viewButtonSizer = boxsizer_construct("VERTICAL");
	local navButtonSizer = boxsizer_construct("HORIZONTAL");
	local viewPanel = panel_construct(panel);
	local buttonPanel = panel_construct(viewPanel);
	local navButtonPanel = panel_construct(buttonPanel);
	local previewSizer = [];
	local previewPanel = [];
	if (listProp.GetDataFlowType() != "Out") {
		previewSizer = boxsizer_construct("HORIZONTAL");
		previewPanel = panel_construct(panel);
	}
	local idxLabel = statictext_construct(buttonPanel, "ID_ANY", "0 / 0");
	
	local microUI = MicroUI(wxparent, listProp);
	
	local microUIImpl = [
		@viewUI : viewUI,
		@previewUI : previewUI,
		@curIt : listProp.iterator(),
		@curIndex : 0,
		@topSizer : topSizer,
		@viewSizer : viewSizer,
		@viewButtonSizer : viewButtonSizer,
		@navButtonSizer : navButtonSizer,
		@viewPanel : viewPanel,
		@buttonPanel : buttonPanel,
		@navButtonPanel : navButtonPanel,
		@previewSizer : previewSizer,
		@previewPanel : previewPanel,
		@idxLabel : idxLabel,
		
		method SetUIValue (listVal) {
			self.GetProperty().SetValue(listVal);
			self.Refresh();
		},
		method UpdateInternalValue
			{ self.Refresh(); }
	];
	
	local panelEventHandlers = [
		method OnNext {
			local listProp = self.GetProperty();
			if (listProp.IsEmpty())
				return;
				
			self.curIt.fwd();
			if (self.curIt.checkend(listProp)) {
				self.curIt.bwd();
				return;
			}
			
			self.curIndex++;
			
			local prop = self.curIt.getval();
			self.viewUI.SetUIValue(prop.GetValue());
			self.UpdateIndexLabel();
		},
		method OnPrev {
			local listProp = self.GetProperty();
			if (listProp.IsEmpty())
				return;
				
			if (self.curIt.checkbegin(listProp))
				return;
			
			self.curIt.bwd();
			self.curIndex--;
			
			local prop = self.curIt.getval();
			self.viewUI.SetUIValue(prop.GetValue());
			self.UpdateIndexLabel();
		},
		method OnCreate {
			local listProp = self.GetProperty();
			local wasEmpty = listProp.IsEmpty();
			local prop = listProp.GetMotherProperty().Clone();
			self.previewUI.UpdateInternalValue();
			prop.SetValue(self.previewUI.GetProperty().GetValue());
			listProp.AddProperty(prop);
			
			if (wasEmpty) {
				self.viewUI.SetUIValue(prop.GetValue());
				self.curIt.setbegin(self.GetProperty());
				self.viewUI.show();
			}
			
			//++self.total;
			self.UpdateIndexLabel();
		},
		method OnDelete {
			local listProp = self.GetProperty();
			if (listProp.IsEmpty())
				return;
				
			local tmpIt = listProp.iterator();
			tmpIt.assign(self.curIt);
		
			self.curIt.fwd();
			if (self.curIt.checkend(listProp)) {
				// iterator points to the last element
				self.curIt.bwd();
				
				if (self.curIt.checkbegin(listProp)) {
					// iterator points to the only element
					listProp.RemoveProperty(self.curIt);
					self.curIt.setend(listProp);
					self.viewUI.hide();
					self.UpdateIndexLabel();
					return;
				}
				self.curIt.bwd();
				self.curIndex--;
			}
		
			listProp.RemoveProperty(tmpIt);
			local prop = self.curIt.getval();
			self.viewUI.SetUIValue(prop.GetValue());
			self.UpdateIndexLabel();
		},
		method UpdateIndexLabel {
			local listProp = self.GetProperty();
			if (listProp.IsEmpty()) {
				self.idxLabel.setlabel("0 / 0");
			}
			else {
				local s = 	tostring(self.curIndex + 1) + 
							" / " 						+ 
							tostring(listProp.GetSize());
				self.idxLabel.setlabel(s);
			}
		},
		method Refresh {
			local listProp = self.GetProperty();
			self.curIt.setbegin(listProp);
			self.curIndex = 0;
			
			if (listProp.IsEmpty()) {
				self.viewUI.hide();
			}
			else {
				local prop = self.curIt.getval();
				self.viewUI.SetUIValue(prop.GetValue());
				if (not self.viewUI.isshown())
					self.viewUI.show();
				self.UpdateIndexLabel();
			}
		}
	];
	
	inherit(microUIImpl, microUI);
	inherit(panel, microUIImpl);
	inherit(panel, panelEventHandlers);
	
	// View panel population
	
	viewUI.reparent(panel.viewPanel);
	viewSizer.add(viewUI);
	viewSizer.add(
		staticline_construct(
			panel.viewPanel,
			"ID_STATIC",
			defaultPosition,
			defaultSize,
			"LI_VERTICAL"
		),
		noProportion,
		flags("GROW", "ALL"),
		defaultBorderWidth
	);
	
	// Navigation button panel population
	
	navButtonSizer.add(
		bitmapbutton_construct(
			navButtonPanel, 
			"ID_BACKWARD",
			artprovider_getbitmap("ART_GO_BACK")
		),
		noProportion,
		"ALL",
		defaultBorderWidth
	);
	navButtonSizer.add(
		bitmapbutton_construct(
			navButtonPanel,
			"ID_FORWARD",
			artprovider_getbitmap("ART_GO_FORWARD")
		),
		noProportion,
		"ALL", 
		defaultBorderWidth
	);
	navButtonPanel.setsizerandfit(navButtonSizer);
	
	viewButtonSizer.add(navButtonPanel, noProportion, "ALIGN_CENTER");
	viewButtonSizer.add(idxLabel, noProportion, "ALIGN_CENTER");
	viewButtonSizer.addspacer(defaultSpace);
	if (panel.GetProperty().GetDataFlowType() != "Out")
		viewButtonSizer.add(
			button_construct(panel.buttonPanel, "ID_DELETE", "Delete"),
			noProportion,
			flags("ALIGN_CENTER", "ALIGN_BOTTOM")
		);
		
	buttonPanel.setsizerandfit(viewButtonSizer);
	
	viewSizer.add(buttonPanel);
	viewPanel.setsizerandfit(viewSizer);
	
	// Preview panel population
	
	if (panel.GetProperty().GetDataFlowType() != "Out") {
		previewUI.reparent(previewPanel);
		previewSizer.add(previewUI);
		previewSizer.add(
			staticline_construct(
				previewPanel,
				"ID_STATIC",
				defaultPosition,
				defaultSize,
				"LI_VERTICAL"
			),
			noProportion,
			flags("GROW", "ALL"),
			defaultBorderWidth
		);
		previewSizer.add(
			button_construct(previewPanel, "ID_NEW", "Create"),
			noProportion,
			"ALIGN_CENTER"
		);
		previewPanel.setsizerandfit(panel.previewSizer);
	}
	
	// Panel population
	
	topSizer.add(viewPanel);
	if (panel.GetProperty().GetDataFlowType() != "Out") {
		topSizer.add(
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
		topSizer.add(previewPanel);
	}
	
	panel.setsizerandfit(topSizer);
	
	if (panel.GetProperty().IsEmpty())
		viewUI.hide();
	
	// Event handler connection
	
	panel.connect("ID_FORWARD", "EVT_COMMAND_BUTTON_CLICKED", panel.OnNext);
	panel.connect("ID_BACKWARD", "EVT_COMMAND_BUTTON_CLICKED", panel.OnPrev);
	if (panel.GetProperty().GetDataFlowType() != "Out") {
		panel.connect("ID_NEW", "EVT_COMMAND_BUTTON_CLICKED", panel.OnCreate);
		panel.connect("ID_DELETE", "EVT_COMMAND_BUTTON_CLICKED", panel.OnDelete);
	}
	
	return panel;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// MicroUIs for VectorProperty
// (MicroUI Object) 
// ( 
//	 wxparent:wxWindow, 
//   listProp:VectorProperty,
//   viewUI:MicroUI,
//   previewUI:MicroUI
// )
//

///////////////////////////////////////////////////////////////

function VectorPanel (wxparent, vecProp, viewUI...) { // previewUI
	local choiceDefaultSize = size_construct(30, -1);
	const defaultSpace = 10; 
	const previewUIArgPos = 3;
	
	local previewUI = (arguments.total() > previewUIArgPos ? arguments[previewUIArgPos] : []);
	
	local panel = panel_construct(wxparent);
	local topSizer = staticboxsizer_construct("VERTICAL", panel, vecProp.GetUserDefinedClassId());
	local viewSizer = boxsizer_construct("HORIZONTAL");
	local viewControlSizer = boxsizer_construct("VERTICAL");
	local viewPanel = panel_construct(panel);
	local viewControlPanel = panel_construct(viewPanel);
	local previewSizer = [];
	local previewPanel = [];
	if (vecProp.GetDataFlowType() != "Out") {
		previewSizer = boxsizer_construct("HORIZONTAL");
		previewPanel = panel_construct(panel);
	}
	local choice = choice_construct(viewControlPanel, "ID_ANY", defaultPosition, defaultSize, []);
	
	local microUI = MicroUI(wxparent, vecProp);
	
	local microUIImpl = [
		@viewUI : viewUI,
		@previewUI : previewUI,
		@curIt : vecProp.iterator(),
		@curChoice : "",
		@topSizer : topSizer,
		@viewSizer : viewSizer,
		@viewControlSizer : viewControlSizer,
		@viewPanel : viewPanel,
		@viewControlPanel : viewControlPanel,
		@previewSizer : previewSizer,
		@previewPanel : previewPanel,
		@choice : choice,
		method SetUIValue (tableVal) {
			self.GetProperty().SetValue(tableVal);
			self.Refresh();
		},
		method UpdateInternalValue 
			{ self.Refresh(); }
	];
	
	local panelEventHandlers = [
		method OnCreate {
			local vecProp = self.GetProperty();
			local wasEmpty = vecProp.IsEmpty();
			local prop = vecProp.GetMotherProperty().Clone();
			self.previewUI.UpdateInternalValue();
			prop.SetValue(self.previewUI.GetProperty().GetValue());
			vecProp.AddProperty(prop);
			
			if (wasEmpty) {
				self.viewUI.SetUIValue(prop.GetValue());
				self.viewUI.show();
			}
			
			self.RefreshChoice();
			
		},
		method OnDelete {
			local vecProp = self.GetProperty();
			if (vecProp.IsEmpty())
				return;
				
			local idx = strtonum(self.choice.getstringselection()); 
			
			curProp = vecProp.RemovePropertyAtIndex(idx);
			if (not (idx < vecProp.GetTotalElements()))
				idx = vecProp.GetTotalElements() - 1;
			if (curProp) {
				self.curChoice = tostring(idx);
				self.viewUI.SetUIValue(curProp.GetValue());
			}
			if (vecProp.IsEmpty())
				self.viewUI.hide();
			
			self.RefreshChoice();
			
		},
		method OnIndexChange {
			local vecProp = self.GetProperty();
			local curProp = vecProp.GetPropertyAtIndex(strtonum(self.choice.getstringselection()));
				
			self.viewUI.SetUIValue(curProp.GetValue());
			
			self.curChoice = self.choice.getstringselection();
		},
		method RefreshChoice {
			local vecProp = self.GetProperty();
			local choices = [];
			for (local i = 0; i < vecProp.GetTotalElements(); i++)
				choices[i] = tostring(i);
			self.choice.clear();
			if (tablength(choices)) {
				self.choice.append(choices);
				if (self.curChoice != "")
					self.choice.setstringselection(self.curChoice);
				else {
					self.curChoice = "0";
					self.choice.setstringselection(self.curChoice);
				}
				self.choice.setminsize(self.choice.getbestsize());
				self.viewControlSizer.layout();

				self.topSizer.layout();
				//TODO there is no buttonPanel
				//self.buttonPanel.fit();

				self.fit();
			}
		},
		method Refresh {
			local vecProp = self.GetProperty();
			if (vecProp.IsEmpty())
				self.viewUI.hide();
			else {
				self.curChoice = "0";
				local curProp = vecProp.GetPropertyAtIndex(0);
				
				self.viewUI.SetUIValue(curProp.GetValue());
				self.viewUI.show();
			}

			self.RefreshChoice();
		}
	];
	
	inherit (microUIImpl, microUI);
	inherit(panel, microUIImpl);
	inherit(panel, panelEventHandlers);
	
	// View panel population
	
	viewUI.reparent(viewPanel);
	viewSizer.add(viewUI);
	viewSizer.add(
		staticline_construct(
			viewPanel,
			"ID_STATIC",
			defaultPosition,
			defaultSize,
			"LI_VERTICAL"
		),
		noProportion,
		flags("GROW", "ALL"), 
		defaultBorderWidth
	);
	
	// View control panel population
	
	choice.setminsize(choiceDefaultSize);
	viewControlSizer.add(choice, noProportion, "ALIGN_CENTER");
	viewControlSizer.addspacer(defaultSpace);
	if (panel.GetProperty().GetDataFlowType() != "Out")
		viewControlSizer.add(
			button_construct(viewControlPanel, "ID_DELETE", "Delete"),
			noProportion, 
			flags("ALIGN_CENTER", "ALIGN_BOTTOM")
		);
	
	viewControlPanel.setsizerandfit(viewControlSizer);
	
	viewSizer.add(viewControlPanel);
	viewPanel.setsizerandfit(viewSizer);
	
	// Preview panel population
	
	if (panel.GetProperty().GetDataFlowType() != "Out") {
		previewUI.reparent(previewPanel);
		previewSizer.add(previewUI);
		previewSizer.add(
			staticline_construct(
				previewPanel, 
				"ID_STATIC", 
				defaultPosition,
				defaultSize,
				"LI_VERTICAL"
			),
			noProportion, 
			flags("GROW", "ALL"), 
			defaultBorderWidth
		);
		previewSizer.add(
			button_construct(previewPanel, "ID_NEW", "Insert"),
			noProportion, 
			"ALIGN_CENTER"
		);
		
		previewPanel.setsizerandfit(previewSizer);
	}
	
	// Panel population
	
	topSizer.add(viewPanel);
	if (panel.GetProperty().GetDataFlowType() != "Out") {
		topSizer.add(
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
		topSizer.add(previewPanel);
	}
	
	panel.setsizerandfit(topSizer);
	
	if (panel.GetProperty().IsEmpty())
		viewUI.hide();
	
	// Event handler connection
	
	panel.connect("ID_ANY", "EVT_COMMAND_CHOICE_SELECTED", panel.OnIndexChange);
	if (panel.GetProperty().GetDataFlowType() != "Out") {
		panel.connect("ID_NEW", "EVT_COMMAND_BUTTON_CLICKED", panel.OnCreate);
		panel.connect("ID_DELETE", "EVT_COMMAND_BUTTON_CLICKED", panel.OnDelete);
	}

	return panel;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// MicroUIs for ArrayProperty
// (MicroUI Object) 
// ( 
//	 wxparent:wxWindow, 
//   listProp:ListProperty,
//   editorUI:MicroUI,
// )
//

///////////////////////////////////////////////////////////////

function ArrayPanel (wxparent, arrayProp, editorUI) {
	local panel = panel_construct(wxparent);
	local topSizer = staticboxsizer_construct("HORIZONTAL", panel, arrayProp.GetUserDefinedClassId());
	local choices = [];
	for (local i = 0; i < arrayProp.GetLength(); i++) {
		choices[i] = tostring(i);
	}
	assert(tablength(choices));
	local choice = choice_construct(panel, "ID_ANY", defaultPosition, defaultSize, choices);
	
	
	local microUI = MicroUI(wxparent, arrayProp);
	
	local microUIImpl = [
		@editorUI : editorUI,
		@curIndex : 0,
		@topSizer : topSizer,
		@choice : choice,
		method SetUIValue (tableVal) {
			self.GetProperty().SetValue(tableVal);
			self.Refresh();
		},
		method UpdateInternalValue { 
			local arrayProp = self.GetProperty();
			editorUI.UpdateInternalValue();
			arrayProp.GetPropertyAtIndex(self.curIndex).SetValue(editorUI.GetProperty().GetValue());
			
			self.Refresh();
		}
	];
	
	local panelEventHandlers = [
		method OnIndexChange {
			local arrayProp = self.GetProperty();
			self.editorUI.UpdateInternalValue();
			arrayProp.GetPropertyAtIndex(self.curIndex).SetValue(editorUI.GetProperty().GetValue());
			
			self.curIndex = strtonum(self.choice.getstringselection());
			self.editorUI.SetUIValue(arrayProp.GetPropertyAtIndex(self.curIndex).GetValue());
		},
		method Refresh {
			local arrayProp = self.GetProperty();
			self.curIndex = 0;
			self.editorUI.SetUIValue(arrayProp.GetPropertyAtIndex(self.curIndex).GetValue());
		}
	];
	
	inherit(microUIImpl, microUI);
	inherit(panel, microUIImpl);
	inherit(panel, panelEventHandlers);
	
	choice.setstringselection(choices[0]);
	choice.setminsize(choice.getbestsize());
	
	// Panel population
	
	editorUI.reparent(panel);
	topSizer.add(panel.editorUI);
	topSizer.add(
		staticline_construct(
			panel, 
			"ID_STATIC", 
			defaultPosition,
			defaultSize,
			"LI_VERTICAL"
		),
		noProportion,
		flags("GROW", "ALL"),
		defaultBorderWidth
	);
	topSizer.add(choice);
	
	panel.setsizerandfit(topSizer);
	
	// Event handler connection
	
	panel.connect("ID_ANY", "EVT_COMMAND_CHOICE_SELECTED", panel.OnIndexChange);
	
	return panel;	
}

///////////////////////////////////////////////////////////////

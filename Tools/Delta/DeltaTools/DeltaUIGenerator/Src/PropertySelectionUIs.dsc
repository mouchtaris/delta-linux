////////////////////////////////////////////////////////////////////////////////////////////////////////
// Property Selection UI Object constructors
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;
using wx;

//
// Globals and constants
//

defaultPosition = point_construct(-1, -1);
defaultSize = size_construct(-1, -1);
const defaultBorderWidth = 5;
const noProportion = 0;

//
// VM Import
//

PropertySelectionHolder = libs::import("PropertySelectionHolder");
PropertyUIMappingRules = libs::import("PropertyUIMappingRules");
Properties = libs::import("Properties");
assert(PropertySelectionHolder);
assert(PropertyUIMappingRules);
assert(Properties);


///////////////////////////////////////////////////////////////

//
// PropertySelectionDialog inherits from WxDialog
// (PropertySelectionDialog Object)
// (
//	wxparent:WxWindow, 
//  microUI:MicroUI Object,
//  propList:std::list<Property>
// )
//

function PropertySelectionDialog (wxparent, microUI, propList) {
	local dataMembers = [
		@microUI : microUI,
		@propList : propList
	];
	
	local dialog = dialog_construct(
		wxparent, 
		"ID_ANY", 
		"Select Value from other source",
		defaultPosition,
		defaultSize, 
		flags("CAPTION", "RESIZE_BORDER", "SYSTEM_MENU")
	);
	
	inherit(dialog, dataMembers);
	
	return dialog;
}

///////////////////////////////////////////////////////////////

//
// ListPanelPropertySelectionDialog inherits from PropertySelectionDialog
// (ListPanelPropertySelectionDialog Object)
// (
//	wxparent:WxWindow, 
//  microUI:MicroUI Object,
//  propList:std::list<Property>
// )
//

function ListPanelPropertySelectionDialog (wxparent, microUI, propList) {
	local dialog = PropertySelectionDialog(wxparent, microUI, propList);
	
	it = propList.iterator();
	it.setbegin(propList);
	local motherProperty = it.getval().Clone();
	motherProperty.SetDataFlowType("Out");
	local listProp = Properties.ListProperty("selections", "Selections", motherProperty);
	foreach (p, propList) {
		listProp.AddProperty(p.Clone());
	}
	
	local listPanel = PropertyUIMappingRules.CreateUI(dialog, listProp);
	listPanel.Refresh();
	
	local topSizer = boxsizer_construct("VERTICAL");
	local buttonSizer = boxsizer_construct("HORIZONTAL");
	
	local listPanelPropertySelectionDialog = [
		@topSizer : topSizer,
		@listPanel : listPanel,
		method OnSelect {
			local prop = listPanel.curIt.getval();
			self.microUI.SetUIValue(prop.GetValue());
			
			self.endmodal("ID_OK");
		}
	];
	
	inherit(dialog, listPanelPropertySelectionDialog);
	
	// Layout construction
	
	topSizer.add(
		listPanel,
		noProportion,
		flags("ALL"), 
		defaultBorderWidth
	);
	
	topSizer.add(
		staticline_construct(
			dialog,
			"ID_STATIC",
			defaultPosition,
			defaultSize,
			"LI_HORIZONTAL"
		),
		noProportion,
		flags("GROW", "ALL"),
		defaultBorderWidth
	);
	
	buttonSizer.add(
		button_construct(
			dialog,
			"ID_CANCEL",
			"Cancel"
		),
		noProportion,
		flags("ALL"),
		defaultBorderWidth
	);
	
	buttonSizer.add(
		button_construct(
			dialog,
			"ID_OK",
			"Select"
		),
		noProportion,
		flags("ALL"),
		defaultBorderWidth
	);
	
	topSizer.add(
		buttonSizer,
		noProportion,
		flags("ALIGN_RIGHT", "ALIGN_BOTTOM", "ALL"),
		defaultBorderWidth
	);
	
	dialog.setsizerandfit(topSizer);
	
	// Event handler connection
	
	dialog.connect("ID_OK", "EVT_COMMAND_BUTTON_CLICKED", dialog.OnSelect);
	
	return dialog;
}

///////////////////////////////////////////////////////////////

//
// PropertySelectionLauncher
// (PropertySelectionLauncher Object) (uiAPIId:String, microUI:MicroUI Object)
//

function PropertySelectionLauncher (uiAPIId, microUI) {
	
	local propertySelectionLauncher = [
		@uiAPIId : uiAPIId,
		@microUI : microUI,
		@classId : PropertySelectionHolder.ExtractClassId(microUI.GetProperty()),
		method GetMicroUI
			{ return @microUI; },
		method GetUIAPIId
			{ return @uiAPIId; }
	];
	
	local propertySelectionHolder = PropertySelectionHolder.GetInstance();
	propertySelectionHolder.RegisterPropertySelectionLauncher(propertySelectionLauncher);
	
	return propertySelectionLauncher;
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

//
// PropertySelectionButton inherits from PropertySelectionLauncher
// (PropertySelectionButton Object) (wxparent:wxWindow, uiAPIId:String, microUI:MicroUI Object)
//

function PropertySelectionButton (wxparent, uiAPIId, microUI) {
	local propertySelectionLauncher = PropertySelectionLauncher(uiAPIId, microUI);
	local propertySelectionButton = button_construct(
		wxparent, 
		"ID_ANY",
		"Select value from other source..."
	);
	local evtHandlers = [
		method OnClick {
			local propertySelectionHolder = PropertySelectionHolder.GetInstance();
			local propList = propertySelectionHolder.GetAvailablePropertyList(
				self.uiAPIId, 
				self.classId
			);
			
			/*
			foreach(p, propList)
				print(p.GetValue(), " ,");
			print("\n");
			*/
			
			local dialog = ListPanelPropertySelectionDialog(wxparent, microUI, propList);
			
			dialog.showmodal();
		}
	];
	
	inherit(propertySelectionButton, propertySelectionLauncher);
	inherit(propertySelectionButton, evtHandlers);
	
	propertySelectionButton.disable();
	
	// Event handler connection
	propertySelectionButton.connect(
		"ID_ANY", 
		"EVT_COMMAND_BUTTON_CLICKED",
		propertySelectionButton.OnClick
	);
	
	return propertySelectionButton;
}

///////////////////////////////////////////////////////////////

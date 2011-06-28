////////////////////////////////////////////////////////////////////////////////////////////////////////
// Property to micro UI mapping rules
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
assert(MicroUIs);

///////////////////////////////////////////////////////////////
//
// Internal data
//

//
// Property to MicroUI map enforcing rules (internal)
//

// Insert rules here
propertyUIRuleMap = [
	method List (parent, listProp) {
		local viewProp = listProp.GetMotherProperty().Clone();
		viewProp.SetDataFlowType("Out");
		local viewUI = self[viewProp.GetClassId()](parent, viewProp);
		
		local ui;
		if (listProp.GetDataFlowType() != "Out") {
			local previewProp = listProp.GetMotherProperty().Clone();
			local previewUI = self[previewProp.GetClassId()](parent, previewProp);
		
			ui = MicroUIs.ListPanel(parent, listProp, viewUI, previewUI);
		}
		else {
			ui = MicroUIs.ListPanel(parent, listProp, viewUI);
		}
		
		return ui;
	},
	method Array (parent, arrayProp) {
		local editorProp = arrayProp.GetMotherProperty().Clone();
		local editorUI = self[editorProp.GetClassId()](parent, editorProp);
		
		return MicroUIs.ArrayPanel(parent, arrayProp, editorUI);
	},
	method Vector (parent, vecProp) {
		local viewProp = vecProp.GetMotherProperty().Clone();
		viewProp.SetDataFlowType("Out");
		local viewUI = self[viewProp.GetClassId()](parent, viewProp);
		
		local ui;
		if (vecProp.GetDataFlowType() != "Out") {
			local previewProp = vecProp.GetMotherProperty().Clone();
			local previewUI = self[previewProp.GetClassId()](parent, previewProp);
		
			ui = MicroUIs.VectorPanel(parent, vecProp, viewUI, previewUI);
		}
		else {
			ui = MicroUIs.VectorPanel(parent, vecProp, viewUI);
		}
		
		return ui;
	},
	method String (parent, stringProp) {
		local ui;
		if (stringProp.GetDataFlowType() == "Out")
			ui = MicroUIs.StaticText(parent, stringProp);
		else
			ui = MicroUIs.TextCtrl(parent, stringProp);
			
		return ui;
	},
	method Enumerated (parent, enumProp) {
		local ui;
		const combobox_threshold = 6;
		
		if (tablength(enumProp.choices) < combobox_threshold)
			ui = MicroUIs.RadioBox(parent, enumProp);
		else
			ui = MicroUIs.Choice(parent, enumProp);
			
		if (enumProp.GetDataFlowType() == "Out")
			ui.disable();
			
		return ui;
	},
	method Union (parent, unionProp) {
		local ui = MicroUIs.ChoiceBook(parent, unionProp);
		foreach (id, unionProp.GetOrdering()) {
			local prop = unionProp.GetProperty(id);
			local microUI = self[prop.GetClassId()](ui, prop);
			ui.AddMicroUI(microUI);
		}
		
		return ui;
	},
	method Numeric (parent, numProp) {
		local ui;
		
		if (numProp.GetDataFlowType() == "Out")
			ui = MicroUIs.NumericStaticText(parent, numProp);
		else
			ui = MicroUIs.NumericCtrl(parent, numProp);
		
		return ui;
	},
	method Boolean (parent, boolProp) {
		local ui;
		
		if (boolProp.GetDataFlowType() == "Out")
			ui = MicroUIs.BooleanStaticText(parent, boolProp);
		else
			ui = MicroUIs.CheckBox(parent, boolProp);
		return ui;
	},
	method Aggregate (parent, aggProp) {
		local ui;
		
		ui = MicroUIs.AggregatePanel(parent, aggProp);
		foreach (id, aggProp.GetOrdering()) {
			local prop = aggProp.GetProperty(id);
			local microUI = self[prop.GetClassId()](ui, prop);
			ui.AddMicroUI(microUI);
		}
		
		return ui;
	}
];

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//
// PropertyMicroUIMappingRules public functions
//

//
// CreateUI
// (MicroUI Object) (parent:wxWindow, property:Property) 
//

function CreateUI (parent, property) {
	return propertyUIRuleMap[property.GetClassId()](parent, property);
}

///////////////////////////////////////////////////////////////

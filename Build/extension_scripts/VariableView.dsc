/**
 *	VariableView.dsc
 *
 *	-- Sparrow IDE Variable View --
 *
 *	Variable list view of Sparrow IDE.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2008
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();
const classId = "VariableView";

window = nil;
base = nil;
mostbase = nil;

const VARIABLE_VALUE_IS_GROUP = "_<VARGROUP>_";

//-------------------------------------------------------//
//---- Properties ---------------------------------------//

function SetProperties {
	function CreateColor(r, g, b) { return [ {.class : "wxColour"}, {.r : r}, {.g : g}, {.b : b} ]; }
	function AddFontProperty(id, label, desc, font, category) {
		spw::addclassproperty(
			classId,
			id,
			[
				{.class			: "FontProperty"},
				{.label			: label			},
				{.description	: desc			},
				{.category		: category		},
				{.value			: font	}
			]		
		);
	}
	function AddColorProperty(id, label, desc, color, category) {
		spw::addclassproperty(
			classId,
			id,
			[
				{.class			: "ColorProperty"	},
				{.label			: label				},
				{.description	: desc				},
				{.category		: category			},
				{.value			: color				}
			]		
		);	
	}
	AddFontProperty(
		"GroupFont",
		"Font for the variable groups",
		"Sets the font for the variable groups",
		[ {.class : "wxFont"}, {.size : 8}, {.weight : "BOLD"} ],
		"Variable Groups"
	);
	AddColorProperty(
		"GroupColor",
		"Color for the variables groups",
		"Sets the color for the variable groups",
		CreateColor(139, 69, 19),					//SADLE BROWN
		"Variable Groups"
	);
	AddColorProperty(
		"DefaultValueColor",
		"Color for the default variables values",
		"Sets the color for the default variable values",
		CreateColor(0, 0, 0),						//BLACK
		"Color encoding"
	);
	AddColorProperty(
		"NewValueColor",
		"Color for the new variables values",
		"Sets the color for the new variable values",
		CreateColor(34, 139, 34),					//FOREST GREEN
		"Color encoding"
	);
	AddColorProperty(
		"ModifiedValueColor",
		"Color for the modified variables values",
		"Sets the color for the modified variable values",
		CreateColor(255, 0, 0),						//RED
		"Color encoding"
	);
	AddColorProperty(
		"InactiveValueColor",
		"Color for the inactive variables",
		"Sets the color for the inactive variables",
		CreateColor(190, 190, 190),					//GRAY
		"Color encoding"
	);
}

function Highlight(id, colorId, fontId) {
	if (colorId != nil)
		window.SetTextColor(id, spw::getclassproperty(classId, colorId).value);
	if (fontId != nil)
		window.SetFont(id, spw::getclassproperty(classId, fontId).value);
}

//-----------------------------------------------------------------------

function AppendItem(id, expr, value) { return window.Append(id, list_new(expr, value)); }

//-----------------------------------------------------------------------

function UpdateVariableView() {
	local vars = spw.components.DeltaVM.GetCurrentContextVariables();
	assert typeof(vars) == TYPEOF_EXTERNID and externidtype(vars) == "std::list";
	foreach(local var, vars) {
		assert typeof(var) == TYPEOF_EXTERNID and externidtype(var) == "std::list";
		local iter = listiter_new(), iter.setbegin(var);
		if (iter.getval() == VARIABLE_VALUE_IS_GROUP) {
			local id = window.Append(window.GetRoot(), list_new("", iter.fwd().getval(), ""));
			Highlight(id, "GroupColor", "GroupFont");
		}
		else {
			local id = window.InsertExpression(callglobal("GetTotalVariables"), iter.getval());
			callglobal("SetLine", id, strtonum(iter.fwd().fwd().getval()));	//shio second value
			if (isundefined(static varStateMapping))
				varStateMapping = ["DefaultValueColor", "NewValueColor", "ModifiedValueColor"];
			Highlight(id, varStateMapping[strtonum(iter.fwd().getval())], nil);	//fourth
		}
	}
}

//-------------------------------------------------------//
//---- Exported API -------------------------------------//

function GetName(id) { assert window and typeof(id) == "Number"; return window.GetText(id, 0); }
function GetValue(id) { assert window and typeof(id) == "Number"; return window.GetText(id, 1); }
function GetLine(id) { assert window and typeof(id) == "Number"; return strtonum(window.GetText(id, 2)); }

function SetName(id, name)
{
	assert window and typeof(id) == "Number" and typeof(name) == "String";
	window.SetText(id, 0, name);
}

function SetValue(id, value)
{
	assert window and typeof(id) == "Number" and typeof(value) == "String";
	window.SetText(id, 1, value);
}

function SetLine(id, line)
{
	assert window and typeof(id) == "Number" and typeof(line) == "Number";
	window.SetText(id, 2, line + "");
}

//-----------------------------------------------------------------------

function GetVariable(id)
{
	if (GetName(id) == "" or window.GetParent(id) != window.GetRoot())
		return "";

	return GetName(id) + ":" + GetValue(id) + ":" + GetLine(id);
}

//-----------------------------------------------------------------------

function GetTotalVariables()
{
	return window.GetTotalChildren(window.GetRoot(), false);
}

//-----------------------------------------------------------------------

function Clear()
{
	base.Clear();
	AppendItem(0, "", "");
}

//-----------------------------------------------------------------------

function Refresh()
{
	Clear();
	UpdateVariableView();
}

//-----------------------------------------------------------------------

function onBreakpointHit(invoker, uri, line) { Refresh(); }

//-----------------------------------------------------------------------

function onStackFrameMoved(classId, index) { Refresh(); }

//-----------------------------------------------------------------------

function onClear(classId) { Clear(); }

//-----------------------------------------------------------------------

function onVariableActivated(invoker, id)
{
	if (invoker.class_id != mostbase.class_id or invoker.serial != mostbase.serial)
		return;

	if (GetName(id) == "" or window.GetParent(id) != window.GetRoot())
		return;

	//goto local definition line
}

//-----------------------------------------------------------------------

function onDebugResumed(invoker)
{
	foreach(local id, window.GetChildren(window.GetRoot(), true))
		if (GetName(id) != "" and window.GetParent(id) == window.GetRoot())
			Highlight(id, "InactiveValueColor", nil);
}

//-----------------------------------------------------------------------


//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId, "ExpressionTreeListView");
	spw::setcomponentmetadata(
		classId, "Variables",
		"List view component for inspection of local variable during debugging",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);

	spw::class_decl_required_member_function(classId, "GetVariable", "String (uint id)",
		"Return the local in the specified index");
	spw::class_decl_required_member_function(classId, "GetName", "String (uint id)",
		"Return the name of the specified local");
	spw::class_decl_required_member_function(classId, "GetValue", "String (uint id)",
		"Return the value of the specified local");
	spw::class_decl_required_member_function(classId, "GetLine", "uint (uint id)",
		"Return the line definition for the specified local");
	spw::class_decl_required_member_function(classId, "GetTotalVariables", "uint (void)",
		"Return number of total locals");

	spw::class_decl_required_member_handler(classId, "BreakpointHit");
	spw::class_decl_required_member_handler(classId, "StackFrameMoved");
	spw::class_decl_required_member_handler(classId, "TreeListItemActivated");
	spw::class_decl_required_member_handler(classId, "DebugResumed");
	
	SetProperties();
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_function(classId, "GetVariable", GetVariable);
	spw::inst_impl_required_member_function(classId, "GetName", GetName);
	spw::inst_impl_required_member_function(classId, "GetValue", GetValue);
	spw::inst_impl_required_member_function(classId, "GetLine", GetLine);
	spw::inst_impl_required_member_function(classId, "GetTotalVariables", GetTotalVariables);

	spw::inst_impl_required_member_handler(classId, "BreakpointHit", onBreakpointHit);
	spw::inst_impl_required_member_handler(classId, "StackFrameMoved", onStackFrameMoved);
	spw::inst_impl_required_member_handler(classId, "TreeListItemActivated", onVariableActivated);
	spw::inst_impl_required_member_handler(classId, "DebugResumed", onDebugResumed);
}

//-----------------------------------------------------------------------

onevent Destructor
{
	//the component may be destroyed without being removed, so do this anyway
	local shell = spw.components.Shell;
	if (shell.serial != 0)
		shell.RemoveComponent(window);
}

//-----------------------------------------------------------------------

onevent GenerateWindow
{
	base = spw.decorate(spw::basecomponent());
	mostbase = spw.decorate(spw::mostbasecomponent());
	window = spw.decorate(spw::thiscomponent());
	window.SetTitle("Variables");
	window.SetColumns(list_new("Name:100", "Value:350", "Line:50"));
	window.SetSingleSelectionMode(false);
	
	local root = AppendItem(0, "", "");
	UpdateVariableView();
}

//-----------------------------------------------------------------------
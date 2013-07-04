/**
 *	RecentReturnValuesView.dsc
 *
 *	-- Sparrow IDE Recent Return Values View --
 *
 *	Recent return value view of Sparrow IDE.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2008
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId = "RecentReturnValuesView";

window	= nil;
base	= nil;

//-----------------------------------------------------------------------

function UpdateRecentFunctionResultView() {}

//-------------------------------------------------------//
//---- Exported API -------------------------------------//

function GetText(index, col)
{
	if (index < 0 or index >= window.GetTotalLines())
		return "";
	return window.GetText(index, col);
}

function SetText(index, col, text)
{
	if (index >= 0 and index < window.GetTotalLines())
		window.SetText(index, col, text);
}

//-----------------------------------------------------------------------

function GetFunction(index) { return GetText(index, 0); }
function GetValue(index) { return GetText(index, 1); }

function SetFunction(index, func) { SetText(index, 0, func); }
function SetValue(index, value) { SetText(index, 1, value); }

//-----------------------------------------------------------------------

function GetRecentReturnValue(index)
{
	if (index < 0 or index > window.GetTotalLines())
		return "";

	return GetFunction(index) + "() returned: '" + GetValue(index) + "'";
}

//-----------------------------------------------------------------------

function GetTotalRecentReturnValues()
{
	return window.GetTotalLines();
}

//-----------------------------------------------------------------------

function Clear()
{
	window.Clear();
}

//-----------------------------------------------------------------------

function onAddFunctionReturnValue(classId, func, value)
{
	window.AppendFields(list_new(func, value));
}

//-----------------------------------------------------------------------

function onClear(classId)
{
	Clear();
}

//-----------------------------------------------------------------------

function onRecentReturnValueActivated(invoker, index)
{
	if (invoker.class_id != base.class_id or invoker.serial != base.serial)
		return;

	if (index < 0 or index > window.GetTotalLines())
		return;

	//TODO: goto function definition line
}

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId, "ListViewComponent");
	spw::setcomponentmetadata(
		classId, "Function Results",
		"List view component for inspection of local variable during debugging",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);

	spw::class_decl_required_member_function(classId, "GetRecentReturnValue", "String (uint index)",
		"Return the return value display for the specified index");
	spw::class_decl_required_member_function(classId, "GetFunction", "String (uint index)",
		"Return the function of the specified recent return value");
	spw::class_decl_required_member_function(classId, "GetValue", "String (uint index)",
		"Return the value of the specified recent return value");
	spw::class_decl_required_member_function(classId, "GetTotalRecentReturnValues", "uint (void)",
		"Return number of total locals");

	spw::class_decl_required_member_handler(classId, "AddFunctionReturnValue");
	spw::class_decl_required_member_handler(classId, "DebugResumed");
	spw::class_decl_required_member_handler(classId, "ListItemActivated");
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_function(classId, "GetRecentReturnValue", GetRecentReturnValue);
	spw::inst_impl_required_member_function(classId, "GetFunction", GetFunction);
	spw::inst_impl_required_member_function(classId, "GetValue", GetValue);
	spw::inst_impl_required_member_function(classId, "GetTotalRecentReturnValues", GetTotalRecentReturnValues);

	spw::inst_impl_required_member_handler(classId, "AddFunctionReturnValue", onAddFunctionReturnValue);
	spw::inst_impl_required_member_handler(classId, "DebugResumed", onClear);
	spw::inst_impl_required_member_handler(classId, "ListItemActivated", onRecentReturnValueActivated);
	
	//This is a GUI component so any other initialization takes place in GenerateWindow
}

//-----------------------------------------------------------------------

onevent Destructor
{
	//the component may be destroyed without being removed, so do this anyway
	local shell = spw.components.Shell;
	if (shell.serial != 0 and window)
		shell.RemoveComponent(window);
}

//-----------------------------------------------------------------------

onevent CreateWindow (parent)
{
	local nativeWindow	= spw::basecreatewindow(parent);
	::base				= spw.decorate(spw::basecomponent());
	::window			= spw.decorate(spw::thiscomponent());

	window.SetTitle("Function Results");
	window.SetColumns(list_new("Function:150", "Return Value:500"));
	UpdateRecentFunctionResultView();
	return nativeWindow;
}

//-----------------------------------------------------------------------
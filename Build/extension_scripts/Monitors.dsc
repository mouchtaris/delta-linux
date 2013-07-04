/**
 *	Monitors.dsc
 *
 *	-- Sparrow Debug Monitors --
 *
 *	Monitoring of variables / expressions during
 *	the debugging of a Delta program.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2008
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId = "Monitors";

window	= nil;
base	= nil;

inBreakpoint = false;

function CreateColor(r, g, b) { return [ {.class : "wxColour"}, {.r : r}, {.g : g}, {.b : b} ]; }
INVALID = CreateColor(190, 190, 190);	//GRAY

//-----------------------------------------------------------------------

function AppendItem(id, expr, value) { return window.Append(id, list_new(expr, value)); }

//-----------------------------------------------------------------------

function CreateMenuUserCommand(func)
{
	return [
		{.class_id : classId},
		{.function_name: func},
		{.isCheck : false},
		{.flags : 2},
		{.image : ""},
		{.class : "UserCommandDesc"}
	];
}

//-------------------------------------------------------//
//---- Exported API -------------------------------------//

function GetTotalMonitors()
{
	return window.GetTotalChildren(window.GetRoot(), false);
}

//-----------------------------------------------------------------------

function GetMonitorName(id) { assert window and typeof(id) == "Number"; return window.GetText(id, 0); }
function GetMonitorValue(id) { assert window and typeof(id) == "Number"; return window.GetText(id, 1); }

function SetMonitorName(id, value)
{
	assert window and typeof(id) == "Number" and typeof(value) == "String";
	window.SetText(id, 0, value);
}

function SetMonitorValue(id, value)
{
	assert window and typeof(id) == "Number" and typeof(value) == "String";
	window.SetText(id, 1, value);
}

//-----------------------------------------------------------------------

function InsertMonitor(index, id, expression)
{
	spw.components.DeltaVM.EvalExpr("$new " + id + " = " + expression);
	SetMonitorName(window.InsertExpression(index, expression), id);
}

//-----------------------------------------------------------------------

function IsValidMonitor(id)
{
	return GetMonitorName(id) != "";	//don't check dummy entries
}

//-----------------------------------------------------------------------

function DeleteSelectedMonitor()
{
	local id = window.GetSingleSelection();
	if (IsValidMonitor(id) and window.GetParent(id) == window.GetRoot())
		window.RemoveExpression(id);
}

//-----------------------------------------------------------------------

function RemoveMonitor(id)
{
	if((local monitor = GetMonitorName(id)) != "")
	{
		spw.components.DeltaVM.EvalExpr("$delete " + monitor);
		window.RemoveExpression(id);
	}
}

//-----------------------------------------------------------------------

function Clear()
{
	//base.Clear();
	spw::basecall(base, "Clear");
	local root = AppendItem(0, "", "");
	AppendItem(root, "", "");
}

//-----------------------------------------------------------------------

function Refresh()
{
	foreach(local monitor, window.GetChildren(window.GetRoot(), false))
		if (IsValidMonitor(monitor))
			window.UpdateExpression(monitor);
}

//-----------------------------------------------------------------------

function onItemActivated(id)
{
	if (window.GetParent(id) == window.GetRoot())
	{
		local list = spw.components.Shell.PromptMultipleInput(
			"Add Monitor",
			list_new("Name", "Expression"),
			list_new()
		);
		assert typeof(list) == TYPEOF_EXTERNID and externidtype(list) == "std::list";
		local total = list.total();
		assert total == 0 or total == 2;
		if (list.total() == 2 and list.front() != "" and list.back() != "")
		{
			if (GetMonitorName(id) == "")
				InsertMonitor(GetTotalMonitors() - 1, "$" + list.front(), list.back());
			else
			{
				local index = window.GetRelativePosition(id);
				window.RemoveExpression(id);
				InsertMonitor(index, "$" + list.front(), list.back());
			}
		}
	}
}

//-----------------------------------------------------------------------

function onDeleteItem(id)
{
	if (GetMonitorName(id) != "" and window.GetParent(id) == window.GetRoot())
		window.RemoveExpression(id);
}

//-----------------------------------------------------------------------

function onBreakpointHit(invoker, uri, line)
{
	Refresh();
	inBreakpoint = true;
}

//-----------------------------------------------------------------------

function onUpdateMonitors(classId) { Refresh(); }

//-----------------------------------------------------------------------

function onClearMonitors(classId) { Clear(); }

//-----------------------------------------------------------------------

function onDebugResumed(invoker)
{
	foreach(local monitor, window.GetChildren(window.GetRoot(), true))
		if (IsValidMonitor(monitor))
			window.SetTextColor(monitor, INVALID);
	inBreakpoint = false;
}

//-----------------------------------------------------------------------

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId, "ExpressionTreeListView");
	spw::setcomponentmetadata(
		classId, "Monitors",
		"Monitoring of variables and expressions",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);

	spw::class_decl_required_member_function(classId, "InserMonitor", "void (uint index, String id, String expression)",
		"Insert a monitor for an expression");
	spw::class_decl_required_member_function(classId, "RemoveMonitor", "void (uint id)",
		"Remove a monitor");
	spw::class_decl_required_member_function(classId, "DeleteSelectedMonitor", "void (void)",
		"Delete the selected monitor");
	spw::class_decl_required_member_function(classId, "Clear", "void (void)",
		"Clear all monitors");
	spw::class_decl_required_member_function(classId, "GetTotalMonitors", "uint (void)",
		"Return number of total monitors");
	spw::class_decl_required_member_function(classId, "GetMonitorName", "String (uint id)",
		"Return the id of the specified monitor");
	spw::class_decl_required_member_function(classId, "GetMonitorValue", "String (uint id)",
		"Return the value of the specified monitor");
	spw::class_decl_required_member_function(classId, "OnItemActivated", "void (uint serial)");
	spw::class_decl_required_member_function(classId, "OnDeleteItem", "void (uint serial)");

	spw::class_decl_required_member_handler(classId, "BreakpointHit");
	spw::class_decl_required_member_handler(classId, "StackFrameMoved");
	spw::class_decl_required_member_handler(classId, "DebugStopped");
	spw::class_decl_required_member_handler(classId, "DebugResumed");
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_function(classId, "InserMonitor", InsertMonitor);
	spw::inst_impl_required_member_function(classId, "RemoveMonitor", RemoveMonitor);
	spw::inst_impl_required_member_function(classId, "DeleteSelectedMonitor", DeleteSelectedMonitor);
	spw::inst_impl_required_member_function(classId, "Clear", Clear);
	spw::inst_impl_required_member_function(classId, "GetTotalMonitors", GetTotalMonitors);
	spw::inst_impl_required_member_function(classId, "GetMonitorName", GetMonitorName);
	spw::inst_impl_required_member_function(classId, "GetMonitorValue", GetMonitorValue);
	spw::inst_impl_required_member_function(classId, "OnItemActivated", onItemActivated);
	spw::inst_impl_required_member_function(classId, "OnDeleteItem", onDeleteItem);

	spw::inst_impl_required_member_handler(classId, "BreakpointHit", onBreakpointHit);
	spw::inst_impl_required_member_handler(classId, "StackFrameMoved", onUpdateMonitors);
	spw::inst_impl_required_member_handler(classId, "DebugStopped", onClearMonitors);
	spw::inst_impl_required_member_handler(classId, "DebugResumed", onDebugResumed);
		
	//This is a GUI component so any other initialization takes place in GenerateWindow
}

//-----------------------------------------------------------------------

onevent Destructor
{
	window.RemoveInstanceCommand("/Delete");
	
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

	window.SetTitle("Monitors");
	window.SetColumns(list_new("Monitor:150", "Value:500"));
	window.SetSingleSelectionMode(false);
	
	window.AddInstanceCommand("/{0}Delete", CreateMenuUserCommand("DeleteSelectedMonitor"));
	
	local root = AppendItem(0, "", "");
	AppendItem(root, "", "");
	return nativeWindow;
}

//-----------------------------------------------------------------------
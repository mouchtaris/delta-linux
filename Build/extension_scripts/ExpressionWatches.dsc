/**
 *	ExpressionWatches.dsc
 *
 *	-- Sparrow Debug Watches --
 *
 *	Watches of variables / expressions during
 *	the debugging of a Delta program using a TreeListView.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2008
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId = "ExpressionWatches";

window = nil;
base = nil;
mostbase = nil;

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

function GetTotalWatches
{
	return window.GetTotalChildren(window.GetRoot(), false);
}

//-----------------------------------------------------------------------

function GetWatchExpression(id) { assert window and typeof(id) == "Number"; return window.GetText(id, 0); }
function GetWatchValue(id) { assert window and typeof(id) == "Number"; return window.GetText(id, 1); }

//-----------------------------------------------------------------------

function SetWatchExpression(id, expr)
{
	assert window and typeof(id) == "Number" and typeof(expr) == "String";
	window.SetText(id, 0, expr);
}

function SetWatchValue(id, value)
{
	assert window and typeof(id) == "Number" and typeof(value) == "String";
	window.SetText(id, 1, value);
}

//-----------------------------------------------------------------------

function AddWatch(expr)
{
	window.InsertExpression(GetTotalWatches() - 1, expr);
}

//-----------------------------------------------------------------------

function AddWatchCmd
{
	if (not inBreakpoint)
		return;
	local expr = "";
	if (spw.components.EditorManager.serial != 0 and
		(local editor = spw.components.EditorManager.GetFocusedEditor()) and
		editor.serial != 0
	)
		expr = editor.GetSelectedText();

	if (expr == "") {
		local result = spw.components.Shell.PromptInput("Add Watch", "Expression:", "");
		if (result.second)
			expr = result.first;
	}
	if (expr != "") {
		AddWatch(expr);
		spw.components.Shell.FocusComponent(window);
	}
}

//-----------------------------------------------------------------------

function IsValidWatch(id)
{
	return GetWatchExpression(id) != "";	//don't check dummy entries
}

//-----------------------------------------------------------------------

function DeleteSelectedWatch
{
	local id = window.GetSingleSelection();
	if (IsValidWatch(id) and window.GetParent(id) == window.GetRoot())
		window.RemoveExpression(id);
}

//-----------------------------------------------------------------------

function Clear
{
	base.Clear();
	local root = AppendItem(0, "", "");
	AppendItem(root, "", "");
}

//-----------------------------------------------------------------------

function Refresh
{
	foreach(local watch, window.GetChildren(window.GetRoot(), false))
		if (IsValidWatch(watch))
			window.UpdateExpression(watch);
}

//-----------------------------------------------------------------------

function onBreakpointHit(invoker, uri, line)
{
	Refresh();
	inBreakpoint = true;
}

//-----------------------------------------------------------------------

function onUpdateWatches(classId) { Refresh(); }

//-----------------------------------------------------------------------

function onClearExpressions(classId) { Clear(); }

//-----------------------------------------------------------------------

function onTreeListItemActivated(invoker, id)
{
	if (invoker.class_id != mostbase.class_id or invoker.serial != mostbase.serial)
		return;

	if (inBreakpoint and window.GetParent(id) == window.GetRoot()) {
		local result = spw.components.Shell.PromptInput(
			"Add Watch",
			"Expression:",
			GetWatchExpression(id)
		);
		if (result.second) {
			local expr = result.first;
			if (GetWatchExpression(id) == "")
				AddWatch(expr);
			else
			{
				local index = window.GetRelativePosition(id);
				window.RemoveExpression(id);
				window.InsertExpression(index, expr);
			}
		}
	}
}

//-----------------------------------------------------------------------

function onDeleteTreeListItem(invoker, id)
{
	if (invoker.class_id != mostbase.class_id or invoker.serial != mostbase.serial)
		return;

	if (GetWatchExpression(id) != "" and window.GetParent(id) == window.GetRoot())
		window.RemoveExpression(id);
}

//-----------------------------------------------------------------------

function onDebugResumed(invoker)
{
	foreach(local watch, window.GetChildren(window.GetRoot(), true))
		if (IsValidWatch(watch))
			window.SetTextColor(watch, INVALID);
	inBreakpoint = false;
}

//-----------------------------------------------------------------------

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId, "ExpressionTreeListView");
	spw::setcomponentmetadata(
		classId, "Watches",
		"Expression Watches",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);

	spw::class_decl_required_member_command(
		[
			{.class			: "UserCommandDesc"	},
			{.class_id		: classId			},
			{.function_name	: "AddWatchCmd"		},
			{.flags			: 3					}
		],
		"/{110}Debug/{130}--Add Watch\tShift+F7",
		"Add a watch variable"
	);

	spw::class_decl_required_member_function(classId, "AddWatch", "void (String expression)",
		"Add a watch for an expression");
	spw::class_decl_required_member_function(classId, "DeleteSelectedWatch", "void (void)",
		"Delete the selected watch");
	spw::class_decl_required_member_function(classId, "Clear", "void (void)",
		"Clear all watches");
	spw::class_decl_required_member_function(classId, "GetTotalWatches", "uint (void)",
		"Return number of total watches");
	spw::class_decl_required_member_function(classId, "GetWatchExpression", "String (uint id)",
		"Return the expression of the specified watch");
	spw::class_decl_required_member_function(classId, "SetWatchExpression", "void (uint id, String expr)",
		"Set the expression of the specified watch");
	spw::class_decl_required_member_function(classId, "GetWatchValue", "String (uint id)",
		"Return the value of the specified watch");
	spw::class_decl_required_member_function(classId, "SetWatchValue", "String (uint id, String value)",
		"Set the value of the specified watch");

	spw::class_decl_required_member_handler(classId, "BreakpointHit");
	spw::class_decl_required_member_handler(classId, "StackFrameMoved");
	spw::class_decl_required_member_handler(classId, "DebugStopped");
	spw::class_decl_required_member_handler(classId, "TreeListItemActivated");
	spw::class_decl_required_member_handler(classId, "DeleteTreeListItem");
	spw::class_decl_required_member_handler(classId, "DebugResumed");
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_command(classId, "AddWatchCmd", AddWatchCmd);

	spw::inst_impl_required_member_function(classId, "AddWatch", AddWatch);
	spw::inst_impl_required_member_function(classId, "DeleteSelectedWatch", DeleteSelectedWatch);
	spw::inst_impl_required_member_function(classId, "Clear", Clear);
	spw::inst_impl_required_member_function(classId, "GetTotalWatches", GetTotalWatches);
	spw::inst_impl_required_member_function(classId, "GetWatchExpression", GetWatchExpression);
	spw::inst_impl_required_member_function(classId, "SetWatchExpression", SetWatchExpression);
	spw::inst_impl_required_member_function(classId, "GetWatchValue", GetWatchValue);
	spw::inst_impl_required_member_function(classId, "SetWatchValue", SetWatchValue);

	spw::inst_impl_required_member_handler(classId, "BreakpointHit", onBreakpointHit);
	spw::inst_impl_required_member_handler(classId, "StackFrameMoved", onUpdateWatches);
	spw::inst_impl_required_member_handler(classId, "DebugStopped", onClearExpressions);
	spw::inst_impl_required_member_handler(classId, "TreeListItemActivated", onTreeListItemActivated);
	spw::inst_impl_required_member_handler(classId, "DeleteTreeListItem", onDeleteTreeListItem);
	spw::inst_impl_required_member_handler(classId, "DebugResumed", onDebugResumed);
		
	//This is a GUI component so any other initialization takes place in GenerateWindow
}

//-----------------------------------------------------------------------

onevent Destructor
{
	window.RemoveInstanceCommand("/Delete");
	
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
	window.SetTitle("Watches");
	window.SetColumns(list_new("Expression:150", "Value:800"));
	window.SetSingleSelectionMode(false);
	
	window.AddInstanceCommand("/{0}Delete", CreateMenuUserCommand("DeleteSelectedWatch"));
		
	local root = AppendItem(0, "", "");
	AppendItem(root, "", "");
}

//-----------------------------------------------------------------------
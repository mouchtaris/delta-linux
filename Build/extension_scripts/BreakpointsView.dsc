/**
 *	BreakpointsView.dsc
 *
 *	-- Sparrow IDE Breakpoints View --
 *
 *	Breakpoint list view of Sparrow IDE.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2008
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId = "BreakpointsView";

window		= nil;
base		= nil;
debugging 	= false;

//-----------------------------------------------------------------------

function UpdateBreakpointView() {
	local breakpoints = spw.components.DeltaVM.GetAllBreakpoints();
	assert typeof(breakpoints) == TYPEOF_EXTERNID and externidtype(breakpoints) == "std::list";
	foreach(local breakpoint, breakpoints) {
		assert typeof(breakpoint) == TYPEOF_EXTERNID and externidtype(breakpoint) == "std::list";
		local iter = listiter_new(), iter.setbegin(breakpoint);
		//skip breakpoint[0] that contains the uri of the breakpoint
		window.AppendFields(list_new(" ", iter.fwd().getval(), iter.fwd().getval(), iter.fwd().getval()));
		if(iter.fwd().getval() == "false")	//enabled
			window.SetImage("breakpoint_disabled", window.GetTotalLines() - 1, 0);
	}
}

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

function GetSourceFile(index) { return GetText(index, 1); }
function GetSourceLine(index) { return strtonum(GetText(index, 2)); }
function GetCondition(index)  { return GetText(index, 3); }
function GetHitCount(index)  { return strtonum(GetText(index, 4)); }

function SetSourceFile(index, file) { SetText(index, 1, file); }
function SetSourceLine(index, line) { SetText(index, 2, line + ""); }
function SetCondition(index, condition)  { SetText(index, 3, condition); }
function SetHitCount(index, count)  { SetText(index, 4, count + ""); }

//-----------------------------------------------------------------------

function ChangeCondition(index, condition)
{
	if (index >= 0 and index < window.GetTotalLines())
		spw.components.DeltaVM.ChangeBreakpointCondition(
			GetSourceFile(index),
			GetSourceLine(index),
			condition
		);
}

//-----------------------------------------------------------------------

function GetBreakpoint(index)
{
	if (index < 0 or index >= window.GetTotalLines())
		return "";

	return
		"source file: " + GetSourceFile(index) +
		", line:" + GetSourceLine(index) +
		", condition: " + GetCondition(index);
}

//-----------------------------------------------------------------------

function GetTotalBreakpoints()
{
	return window.GetTotalLines();
}

//-----------------------------------------------------------------------

function Clear()
{
	spw::basecall(base, "Clear");	//base..Clear();
}

//-----------------------------------------------------------------------

function GetIndex(symbolic, line)
{
	for (local i = GetTotalBreakpoints() - 1; i >= 0; --i)
		if (GetSourceFile(i) == symbolic and GetSourceLine(i) == line)
			break;
	return i;
}

//-----------------------------------------------------------------------

function DeleteBreakpoint(index)
{
	if (index >= 0 and index < window.GetTotalLines())
		spw.components.DeltaVM.RemoveBreakpoint(GetSourceFile(index), GetSourceLine(index));
}

//-----------------------------------------------------------------------

function DeleteSelectedBreakpoint()
{
	DeleteBreakpoint(window.GetSingleSelection());
}

//-----------------------------------------------------------------------

function GotoBreakpointSource(index)
{
	if (index >= 0 and index < window.GetTotalLines())
		spw.components.DeltaVM.GotoSymbolicDocument(GetSourceFile(index), GetSourceLine(index));
}

//-----------------------------------------------------------------------

function GotoSelectedBreakpointSource()
{
	GotoBreakpointSource(window.GetSingleSelection());
}

//-----------------------------------------------------------------------

function ChangeSelectedBreakpointCondition()
{
	local index = window.GetSingleSelection();
	if (index != -1)
	{		
		local previousExpr = GetCondition(index);
		local result = spw.components.Shell.PromptInput(
			"Breakpoint Condition",
			"Condition:",
			previousExpr
		);
		if(result.second and (local expr = result.first) != previousExpr)
			ChangeCondition(index, expr);
	}
}

//-----------------------------------------------------------------------

function onBreakpointActivated(index) { GotoBreakpointSource(index); }

//-----------------------------------------------------------------------

function onDeleteBreakpoint(index) { DeleteBreakpoint(index); }

//-----------------------------------------------------------------------

function onInsertBreakpoint(classId, uri, symbolic, line, condition)
{
	window.AppendFields(list_new(" ", symbolic, line + "", condition, (debugging ? "0" : "")));
	window.SetImage("breakpoint_enabled", window.GetTotalLines() -1, 0);
}

//-----------------------------------------------------------------------

function onRemoveBreakpoint(classId, uri, symbolic, line)
{
	if((local index = GetIndex(symbolic, line)) != -1)
		window.Remove(index);
}

//-----------------------------------------------------------------------

function onEnableBreakpoint(classId, uri, symbolic, line)
{
	if((local index = GetIndex(symbolic, line)) != -1)
		window.SetImage("breakpoint_enabled", index, 0);
}

//-----------------------------------------------------------------------

function onDisableBreakpoint(classId, uri, symbolic, line)
{
	if((local index = GetIndex(symbolic, line)) != -1)
		window.SetImage("breakpoint_disabled", index, 0);
}

//-----------------------------------------------------------------------

function onChangeBreakpointCondition(classId, uri, symbolic, line, condition)
{
	if((local index = GetIndex(symbolic, line)) != -1)
		SetCondition(index, condition);
}

//-----------------------------------------------------------------------

function onChangeBreakpointLine(classId, uri, symbolic, line, newLine)
{
	if((local index = GetIndex(symbolic, line)) != -1)
		SetSourceLine(index, newLine);
}

//-----------------------------------------------------------------------

function onLoadWorkspace(workspace, uri)
{
	Clear();
	UpdateBreakpointView();
}

//-----------------------------------------------------------------------

function onCloseWorkspace(workspace, uri)
{
	Clear();
}

//-----------------------------------------------------------------------

function onDebugStarted(invoker, uri)
{
	debugging = true;
	for(local i = 0; i < GetTotalBreakpoints(); ++i)
		SetHitCount(i, 0);
}

//-----------------------------------------------------------------------

function onDebugStopped(invoker, uri)
{
	for(local i = 0; i < GetTotalBreakpoints(); ++i)
		SetHitCount(i, "");
	debugging = false;
}

//-----------------------------------------------------------------------

function onDebugResumed(invoker)
{
	for(local i = 0; i < GetTotalBreakpoints(); ++i)
		window.SetFontStyleEnabled(i, "bold", false);
}

//-----------------------------------------------------------------------

function onBreakpointHit(invoker, uri, symbolic, line)
{
	local index = GetIndex(symbolic, line);
	if (index != -1)
	{
		SetHitCount(index, GetHitCount(index) + 1);
		window.SetFontStyleEnabled(index, "bold", true);
	}
}

//-----------------------------------------------------------------------

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	local sparrowDir = spw::installationdir() + "/";

	spw::registercomponent(classId, "ListViewComponent");
	spw::setcomponentmetadata(
		classId, "Breakpoints",
		"List view component for inspection of breakpoints placed in the workspace",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);

	spw::class_decl_required_member_function(classId, "GetBreakpoint", "String (uint index)",
		"Return the breakpoint in the specified index");
	spw::class_decl_required_member_function(classId, "GetSourceFile", "String (uint index)",
		"Return the source file of the specified breakpoint");
	spw::class_decl_required_member_function(classId, "GetSourceLine", "uint (uint index)",
		"Return the source line of the specified breakpoint");
	spw::class_decl_required_member_function(classId, "GetCondition", "String (uint index)",
		"Return the condition of the specified breakpoint");
	spw::class_decl_required_member_function(classId, "SetCondition",  "void (uint index, String condition)",
		"Set a condition for the specified breakpoint");
	spw::class_decl_required_member_function(classId, "GetTotalBreakpoints", "uint (void)",
		"Return number of total breakpoints");
	spw::class_decl_required_member_function(classId, "Clear", "void (void)", "Clear all breakpoints");
	spw::class_decl_required_member_function(classId, "DeleteSelectedBreakpoint", "void (void)",
		"Delete the selected breakpoint");
	spw::class_decl_required_member_function(classId, "GotoSelectedBreakpointSource", "void (void)",
		"Go to the selected breakpoint source line");
	spw::class_decl_required_member_function(classId, "ChangeSelectedBreakpointCondition", "void (void)",
		"Change the condition of the selected breakpoint");

	spw::class_decl_required_member_function(classId, "OnItemActivated", "void (uint index)");
	spw::class_decl_required_member_function(classId, "OnDeleteItem", "void (uint index)");

	spw::class_decl_required_member_handler(classId, "BreakpointAdded");
	spw::class_decl_required_member_handler(classId, "BreakpointRemoved");
	spw::class_decl_required_member_handler(classId, "BreakpointEnabled");
	spw::class_decl_required_member_handler(classId, "BreakpointDisabled");
	spw::class_decl_required_member_handler(classId, "BreakpointConditionChanged");
	spw::class_decl_required_member_handler(classId, "BreakpointLineChanged");
	spw::class_decl_required_member_handler(classId, "WorkspaceLoaded");
	spw::class_decl_required_member_handler(classId, "WorkspaceClosed");
	spw::class_decl_required_member_handler(classId, "DebugStarted");
	spw::class_decl_required_member_handler(classId, "DebugStopped");
	spw::class_decl_required_member_handler(classId, "DebugResumed");
	spw::class_decl_required_member_handler(classId, "BreakpointHit");

	spw::registerimage("breakpoint_enabled", sparrowDir + "resources/breakpoint_enabled.png");
	spw::registerimage("breakpoint_disabled", sparrowDir + "resources/breakpoint_disabled.png");
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Initialization------------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_function(classId, "GetBreakpoint", GetBreakpoint);
	spw::inst_impl_required_member_function(classId, "GetSourceFile", GetSourceFile);
	spw::inst_impl_required_member_function(classId, "GetSourceLine", GetSourceLine);
	spw::inst_impl_required_member_function(classId, "GetCondition", GetCondition);
	spw::inst_impl_required_member_function(classId, "SetCondition", ChangeCondition);
	spw::inst_impl_required_member_function(classId, "GetTotalBreakpoints", GetTotalBreakpoints);
	spw::inst_impl_required_member_function(classId, "Clear", Clear);
	spw::inst_impl_required_member_function(classId, "DeleteSelectedBreakpoint", DeleteSelectedBreakpoint);
	spw::inst_impl_required_member_function(classId, "GotoSelectedBreakpointSource", GotoSelectedBreakpointSource);
	spw::inst_impl_required_member_function(classId, "ChangeSelectedBreakpointCondition", ChangeSelectedBreakpointCondition);

	spw::inst_impl_required_member_function(classId, "OnItemActivated", onBreakpointActivated);
	spw::inst_impl_required_member_function(classId, "OnDeleteItem", onDeleteBreakpoint);

	spw::inst_impl_required_member_handler(classId, "BreakpointAdded", onInsertBreakpoint);
	spw::inst_impl_required_member_handler(classId, "BreakpointRemoved", onRemoveBreakpoint);
	spw::inst_impl_required_member_handler(classId, "BreakpointEnabled", onEnableBreakpoint);
	spw::inst_impl_required_member_handler(classId, "BreakpointDisabled", onDisableBreakpoint);
	spw::inst_impl_required_member_handler(classId, "BreakpointConditionChanged", onChangeBreakpointCondition);
	spw::inst_impl_required_member_handler(classId, "BreakpointLineChanged", onChangeBreakpointLine);
	spw::inst_impl_required_member_handler(classId, "WorkspaceLoaded", onLoadWorkspace);
	spw::inst_impl_required_member_handler(classId, "WorkspaceClosed", onCloseWorkspace);
	spw::inst_impl_required_member_handler(classId, "DebugStarted", onDebugStarted);
	spw::inst_impl_required_member_handler(classId, "DebugStopped", onDebugStopped);
	spw::inst_impl_required_member_handler(classId, "DebugResumed", onDebugResumed);
	spw::inst_impl_required_member_handler(classId, "BreakpointHit", onBreakpointHit);
	
	//This is a GUI component so any other initialization takes place in GenerateWindow
}

//-----------------------------------------------------------------------

onevent Destructor
{
	window.RemoveInstanceCommand("/Delete");
	window.RemoveInstanceCommand("/Goto Source File");
	window.RemoveInstanceCommand("/Condition...");

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

	window.SetTitle("Breakpoints");
	window.SetColumns(list_new(" :25", "Source File:300", "Line:50", "Condition:200", "Hit Count:100"));
	window.SetImages(list_new("breakpoint_enabled", "breakpoint_disabled"));
	window.SetSingleSelectionMode(false);
	
	window.AddInstanceCommand("/{0}Delete", CreateMenuUserCommand("DeleteSelectedBreakpoint"));
	window.AddInstanceCommand("/{10}--Goto Source File", CreateMenuUserCommand("GotoSelectedBreakpointSource"));
	window.AddInstanceCommand("/{20}--Condition...", CreateMenuUserCommand("ChangeSelectedBreakpointCondition"));
	
	UpdateBreakpointView();
	return nativeWindow;
}

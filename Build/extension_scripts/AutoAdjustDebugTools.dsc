/**
 *	AutoAdjustDebugTools.dsc
 *
 *	-- Sparrow Auto Adjust Debug Tools --
 *
 *	A utility that automatically (un)deploys debugging tools
 *	according to the needs of the programmer.
 *
 *	Initial Version: Themistoklis Bourdenas <tbourden@doc.ic.ac.uk> December 2007
 *  Revised version: John Lilis
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId = "AutoAdjustDebugTools";

guiComponents = std::list_new(
	[{.component : "ExpressionWatches"},		{.dock : 4}, {.delta : true},	{.debug : true}],
	[{.component : "Monitors"},					{.dock : 4}, {.delta : true},	{.debug : true}],
	[{.component : "RecentReturnValuesView"},	{.dock : 4}, {.delta : true},	{.debug : true}],
	[{.component : "VariableView"},				{.dock : 4}, {.delta : true},	{.debug : true}],
	//TODO: The following components are used during compilation, so they should not be deactivated
	//during staged compilation (when the DebugStarted and DebugStopped signals are triggered).
	//[{.component : "BuildOrder"},				{.dock : 4}, {.delta : false},	{.debug : false}],
	//[{.component : "BuildOrder"},				{.dock : 4}, {.delta : true},	{.debug : false}],
	[{.component : "DeltaCallStackView"},		{.dock : 5}, {.delta : false},	{.debug : true}],
	[{.component : "ErrorList"},				{.dock : 5}, {.delta : false},	{.debug : false}]
);
nonGuiComponents = ["DebugTooltips", "QuickWatch", "ASTView"];

//-------------------------------------------------------//

function IsValidComponent(classId) {
	local comp = spw.components[classId];
	return comp != nil and comp.serial != 0;
}

function AddGuiComponent(entry) {
	if (not entry.shouldSkipAdd and (not entry.delta or IsValidComponent(#DeltaComponentDirectory)))
		entry.instance = spw.components.Shell.AddComponent(entry.component, entry.dock);		
}

function RemoveGuiComponent(entry) {
	local instance = entry.instance;
	if (instance == nil or entry.instance.serial == 0)
		instance = spw.components[entry.component];
	if (instance != nil) {
		local shell = spw.components.Shell;
		if (shell.serial != 0)
			shell.RemoveComponent(instance);
	}
	if (entry.instance != nil)
		entry.instance = nil;
	if (entry.delta and IsValidComponent(#DeltaComponentDirectory))
		spw.components.DeltaComponentDirectory.UnloadComponent(entry.component);
}

//-------------------------------------------------------//
//---- Exported API -------------------------------------//

onevent onDebugStarted(classId, uri)
{
	foreach(local entry, guiComponents) {
		entry.shouldSkipAdd = not entry.debug and spw.components[entry.component].serial == 0;
		((entry.debug ? AddGuiComponent : RemoveGuiComponent))(entry);
	}

	if (IsValidComponent(#DeltaComponentDirectory))
		foreach(local component, nonGuiComponents)
			spw.components.DeltaComponentDirectory.CreateComponent(component);
}

//-----------------------------------------------------------------------

onevent onDebugStopped(classId, uri)
{
	foreach(local entry, guiComponents)
		((entry.debug ? RemoveGuiComponent : AddGuiComponent))(entry);
	
	if (IsValidComponent(#DeltaComponentDirectory))
		foreach(local component, nonGuiComponents)
			spw.components.DeltaComponentDirectory.UnloadComponent(component);
}

//-----------------------------------------------------------------------


//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId);
	spw::setcomponentmetadata(
		classId, "Auto Adjusting of Debugging Tools",
		"A utility that automatically (un)deploys debugging tools according to the needs of the programmer",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);

	spw::class_decl_required_member_handler(classId, "DebugStarted");
	spw::class_decl_required_member_handler(classId, "DebugStopped");
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_handler(classId, "DebugStarted", onDebugStarted);
	spw::inst_impl_required_member_handler(classId, "DebugStopped", onDebugStopped);
}

//-----------------------------------------------------------------------

onevent Destructor {}

//-----------------------------------------------------------------------
/**
 *	RecentWorkspaces.dsc
 *
 *	-- Sparrow IDE Recent Workspaces & Restore --
 *
 *	Store the recently opened workspaces and restore the last one.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	January 2009
 */

using std;
using #sparrowlib;
using #Delta;
spw  = sparrowlib::sparrow();

const classId = "RecentWorkspaces";																			
workspaces = nil;

const totalWorkspaces = 5;

//-----------------------------------------------------------------------

function SetProperties()
{
	for (local i = 0; i < totalWorkspaces; ++i)
		spw::addclassproperty(
			classId,
			"workspace" + (i + 1),
			[
				{.class : "StringProperty"},
				{.label : "workspace" + (i + 1)},
				{.value : ""},
				{.visible : false}
			]
		);
}

//-----------------------------------------------------------------------

function ReplaceSlashes(uri)
{
	local str = uri;
	while ((local index = strsub(str, "/")) != -1)
		str = strslice(str, 0, index - 1) + "\\" + strslice(str, index + 1, 0);
	return str;
}

//-----------------------------------------------------------------------

function RemoveCommand(uri, no)
{
	local cmd = "/File/Recent Workspaces/" + no + " " + ReplaceSlashes(uri);
	spw::class_remove_static_command(classId, cmd);
}

//-----------------------------------------------------------------------

function RemoveWorkspace(uri)
{
	callglobal("RemoveRecentWorkspaces");
	list_remove(workspaces, uri);
	callglobal("AddRecentWorkspaces");
}

//-----------------------------------------------------------------------

function OpenWorkspace(uri) {
	local message = "Workspace '" + uri + "' cannot be opened. Do you want to remove the reference to it from the Recent list?";
	spw.components.DockableComponent.EnsureVisibility("ProjectManager");
	local workspace = spw.components.ProjectManager.GetWorkspace();
	if (workspace.serial == 0 or workspace.GetURI() != uri and workspace.CanDestroy())
		if(	not spw.components.ProjectManager.OpenWorkspace(uri) and
			spw.components.Shell.GetConfirmation("Sparrow", message))
			RemoveWorkspace(uri);
}

//-----------------------------------------------------------------------

function AddRecentWorkspaces()
{
	for (local i = 0, local iter = listiter_new(), listiter_setbegin(iter, workspaces);
		i < totalWorkspaces and not listiter_checkend(iter, workspaces); 
		++i, listiter_fwd(iter)
	)
	{
		local no = i + 1;
		local uri = listiter_getval(iter);
		if (uri == "")
			break;
		else
			spw::class_impl_static_command(
				[
					{.class			: "UserCommandDesc"		},
					{.class_id		: classId				},
					{.function_name	: "RecentWorkspace" + no},
					{.flags			: 7						}
				],
				"/File/{500}--Recent Workspaces--/{" + no + "}" + no + " " + ReplaceSlashes(uri),
				Delta::bind1st(OpenWorkspace, uri),
				"Loads recent workspace " + no
			);
	}
}

//-----------------------------------------------------------------------

function RemoveRecentWorkspaces()
{
	for (local i = 0, local iter = listiter_new(), listiter_setbegin(iter, workspaces);
		i < totalWorkspaces and not listiter_checkend(iter, workspaces); 
		++i, listiter_fwd(iter)
	)
	{
		local uri = listiter_getval(iter);
		if (uri == "")
			break;
		else
			RemoveCommand(uri, i + 1);
	}
}

//-----------------------------------------------------------------------

function storeWorkspace(uri)
{
	RemoveRecentWorkspaces();
	list_remove(workspaces, uri);
	list_push_front(workspaces, uri);	
	AddRecentWorkspaces();
}

//-----------------------------------------------------------------------

function GetLastWorkspace()
{
	return (list_total(workspaces) > 0 ? list_front(workspaces) : "");
}

//-----------------------------------------------------------------------

function RestoreLastWorkspace()
{
	local uri = GetLastWorkspace();
	if (uri != "")
		OpenWorkspace(uri);
		
}

//-----------------------------------------------------------------------

function ComponentDestroyed() {
	local pm = spw.components.ProjectManager;
	if (pm.serial != 0) {
		local workspace = pm.GetWorkspace();
		if (workspace.serial != 0)
			storeWorkspace(workspace.GetURI());
	}
}

//-----------------------------------------------------------------------

function onWorkspaceLoaded(workspace, uri)
{
	storeWorkspace(uri);
}

//-----------------------------------------------------------------------

function onWorkspaceClosed(workspace, uri)
{
	storeWorkspace(uri);
}

//-----------------------------------------------------------------------

function Initialize
{
	local table = [];
	for (local i = 0; i < totalWorkspaces; ++i)
	{
		local prop = spw::getclassproperty(classId, "workspace" + (i + 1));
		if (prop == nil)
			break;
		else
			table[i] = prop.value;
	}
	workspaces = list_new(|table|);
	AddRecentWorkspaces();
}

//-----------------------------------------------------------------------

function CleanUp
{
	for (local i = 0, local iter = listiter_new(), listiter_setbegin(iter, workspaces);
		i < totalWorkspaces and not listiter_checkend(iter, workspaces); 
		++i, listiter_fwd(iter)
	)
		spw::addclassproperty(
			classId,
			"workspace" + (i + 1),
			[
				{.class : "StringProperty"},
				{.label : "workspace" + (i + 1)},
				{.value : listiter_getval(iter)}
			]
		);
	spw::propertytablesetdirty(classId, true);
	RemoveRecentWorkspaces();
	list_clear(workspaces);
	workspaces = nil;
}

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId);
	spw::setcomponentmetadata(
		classId, "Recent Workspaces & Restore",
		"A utility for storing recent workspaces and restoring the last used",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);

	spw::class_impl_static_function(classId, "Initialize", Initialize, "void (void)");
	spw::class_impl_static_function(classId, "CleanUp", CleanUp, "void (void)");
	
	spw::class_impl_static_function(classId, "GetLastWorkspace", GetLastWorkspace,
		"String (void)", "Returns the uri of the last opened workspace");
	spw::class_impl_static_function(classId, "RestoreLastWorkspace", RestoreLastWorkspace,
		"void (void)", "Restores the last opened workspace");

	spw::class_impl_static_handler(classId, "WorkspaceLoaded", onWorkspaceLoaded);
	spw::class_impl_static_handler(classId, "WorkspaceClosed", onWorkspaceClosed);
	
	SetProperties();
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	error(classId + " is a Singleton Class implemented only with static functions.");
}

//-----------------------------------------------------------------------
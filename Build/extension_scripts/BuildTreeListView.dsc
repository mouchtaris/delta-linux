/**
 *	BuildTreeListView.dsc
 *
 *	-- Sparrow IDE Build Tree List View extension --
 *
 *	Extension of the tree list view for build order.
 *
 *	Christos Despotakis <despotak@ics.forth.gr>
 *	July 2010
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId	= "BuildTreeListView";

window			= nil;
base			= nil;

treeData 		= nil;
map				= [];

Root			= nil;


//======================= Properties =======================//

//----------------------- SetProperties -----------------------//

function SetProperties 
{
	
	function CreateColor(r, g, b)
	{
		return [ {.class : "wxColour"}, {.r : r}, {.g : g}, {.b : b} ]; 
	}
	
	function AddFontProperty(id, label, desc, category) 
	{
		local defaultFont = [ {.class : "wxFont"}, {.size : 8} ];
		spw::addclassproperty(
								classId,
								id,
								[
									{.class			: "FontProperty"},
									{.label			: label			},
									{.description	: desc			},
									{.category		: category		},
									{.value			: defaultFont	}
								]		
							);
	}
	
	function AddColorProperty(id, label, desc, color, category)
	{
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
	
	//---------- Default ----------//
	AddColorProperty(
						"Default bg Color",
						"Background Color",
						"The starting color for the output tree main background",
						CreateColor(255, 255, 255),	//White
						"Default"
					);
					
	AddColorProperty(
						"Default fg Color",
						"Foreground Color",
						"The starting color for the output tree main foreground",
						CreateColor(0, 0, 0),	//Black
						"Default"
					);
		
	AddFontProperty	(
						"Default Font", 
						"Font",
						"The output tree default font",
						"Default"
					);
							
	//---------- Error ----------//
						
	AddColorProperty(
						"Errors bg Color",
						"Background Color",
						"The starting color for the output tree error background",
						CreateColor(255, 255, 255),	//White
						"Errors"
					);
					
	AddColorProperty(
						"Errors fg Color",
						"Foreground Color",
						"The starting color for the output tree error foreground",
						CreateColor(255, 0, 0),	//Red
						"Errors"
					);
	
	AddFontProperty	(
						"Errors Font", 
						"Font",
						"The output tree error font",
						"Errors"
					);
					
	//---------- Warnings ----------//
	AddColorProperty(
						"Warnings bg Color",
						"Background Color",
						"The starting color for the output tree warning background",
						CreateColor(255, 255, 255),	//White
						"Warnings"
					);
					
	AddColorProperty(
						"Warnings fg Color",
						"Foreground Color",
						"The starting color for the output tree warning foreground",
						CreateColor(238,118,0),	//Orange
						"Warnings"
					);
	
	AddFontProperty	(
						"Warnings Font", 
						"Font",
						"The output tree warning font",
						"Warnings"
					);
					
	//=====================================
	//-------- Workspaces & Items --------//
	local items = list_new("Workspace", "Project", "AspectProject", "Filter", "Script", "StageSource", "StageResult", "Aspect", "AspectResult");
	foreach(local item, items) {
		local plural = item + "s";
		AddColorProperty(
							plural + " bg Color",
							"Background Color",
							"The starting color for the output tree " + item + " background",
							CreateColor(255, 255, 255),	//White
							plural
						);
						
		AddColorProperty(
							plural + " fg Color",
							"Foreground Color",
							"The starting color for the output tree " + item + " foreground",
							CreateColor(0,0,0),	//Black
							plural
						);
		
		AddFontProperty	(
							plural + " Font", 
							"Font",
							"The output tree " + item + " font",
							plural
						);
	}
}

//----------------------- Highlight -----------------------//

function Highlight(id, colorId, fontId) 
{
	if (colorId != nil)
		window.SetTextColor(id, spw::getclassproperty(classId, colorId).value);
	if (fontId != nil)
		window.SetFont(id, spw::getclassproperty(classId, fontId).value);
}

//----------------------- GetPropertyValue -----------------------//

function GetPropertyValue(id)
{
	return spw::getclassproperty(classId, id).value;
}


//======================= Local Functions =======================//

//----------------------- GetIndex -----------------------//

function GetIndex(list)
{
	local ret = "";
	foreach(local id, list)
		ret += id + ".";
	return ret;
}

//----------------------- GetParent -----------------------//

function GetParent(list)
{
	local ret = "";
	local prevId = "";
	foreach(local id, list) {
		if (prevId != "")
			ret += prevId + ".";
		prevId = id;
	}
	return ret;
}

//----------------------- PassUp -----------------------//

function PassUp(index, field)
{
	assert treeData;
	for(local t = treeData[index]; t.Parent != -1; t = treeData[t.Parent])
		treeData[t.Parent][field]++ ;
}

//----------------------- demical -----------------------//

function demical (number, dems)
{
	local intpart = integerpart(number);
	local fractionpart = fractionalpart(number);
	local demi = power(10, dems);
	
	fractionpart = integerpart(fractionpart * demi);

	return (intpart + "." + fractionpart);
	
}

//----------------------- Update -----------------------//

function Update(index, data)
{
	assert treeData;
	for(local t = treeData[index]; t.Parent != -1; t = treeData[t.Parent]) {
		local comp;
	
		if(t.Childrens)
			comp = t.CompletedChildrens / t.Childrens * 100;
		else
			comp = 0;
		
		if (data)	//a compilation message that will update only an error/warning entry
			window.SetText(t.Key, data.col, tostring(t[data.index]));	
		else		//an initiation/completion event that will only change work percentage
			window.SetText(t.Key, 1, demical(comp, 2) + " %");
	}	
}

//======================= Event Handling =======================//

//----------------------- onItemActivated -----------------------//

function onItemActivated (id)
{
	local index = map[id];
	local FileName = treeData[index].URI;
	local line = treeData[index].Line;
	
	if (not FileName or not line)
		return;
	spw.components.DeltaVM.GotoSymbolicDocument(FileName, line);
}

//----------------------- onWorkStarted -----------------------//

function onWorkStarted(invoker, root, task)
{
	// Set starting time.
	local startedTime = currenttime();
	
	// With every new run, clear the window.
	window.Clear();
	
	// Create the new treeData.
	treeData = [];
	
	// Create table entry.
	treeData = [{"": [
						{"URI":""},
						{"Key":0}, 
						{"Parent":-1}, 
						{"ParentKey":0}, 
						{"Childrens":0}, 
						{"CompletedChildrens":0}, 
						{"Errors":0}, 
						{"Warnings":0},
						{"Started":startedTime},
						{"Finished":0}
					 ]
				}];

	// Create root.
	Root = window.Append(	0,
							list_new(
								task,
								"0 %",
								tostring(treeData[""].Errors),
								tostring(treeData[""].Warnings)
							)
						);
	
	// Highlight
	Highlight(Root, "Default fg Color", nil);
	
	// Enter TreeKey to table.
	treeData[""].Key = Root;
	map[Root] = "";	
}

//----------------------- onWorkCompleted -----------------------//

function onWorkCompleted(invoker, root, task)
{
	assert treeData;
	
	// Set finishing time.
	treeData[""].Finished = currenttime();
	
	// Calculate completion time.
	local compTime = (treeData[""].Finished - treeData[""].Started) / 1000;
	compTime = demical(compTime, 2);
	
	// Update root completion time.
	window.SetText(Root, 4,  compTime + " secs");

	// Update root completion status to "Done".
	window.SetText(Root, 1, "");
	window.SetImage(Root, 1, "done");
}

//----------------------- onWorkCanceled -----------------------//

function onWorkCanceled(invoker, root, task)
{
	assert treeData;
	
	// Set finishing time.
	treeData[""].Finished = currenttime();
	
	// Calculate completion time.
	local compTime = (treeData[""].Finished - treeData[""].Started) / 1000;
	compTime = demical(compTime, 2);
	
	// Update root completion time.
	window.SetText(Root, 4,  compTime + " secs");
	
	// Update root completion status to "Canceled".
	window.SetText(Root, 1, "");
	window.SetImage(Root, 1, "cancel");
	
	//TODO: PASSDOWN CANCEL?
}

//----------------------- onCompilationMessage -----------------------//

function onCompilationMessage(invoker, buildId, type, content, file, line)
{
	if (not treeData)
		return;

	// Create the table index for <workId>
	local buildIdIndex = GetIndex(buildId);
	local index = buildIdIndex + type;
	local parent = buildIdIndex;
	local id = index + "." + file + "." + line; 
	
	// Get invokers's URI.
	//local uri = spw.decorate(invoker).GetURI();
	// Append compilation message to the resource.
	if (type == #Error or type == #Warning or type == #Fail)
	{
		if (not treeData[index])
		{
			treeData[index] = 	[
									{"URI":""},
									{"Key":0}, 
									{"Parent":parent}, 
									{"ParentKey":treeData[parent].Key}, 
									{"Childrens":0}, 
									{"CompletedChildrens":0}, 
									{"Errors":0}, 
									{"Warnings":0},
									{"Started":0},
									{"Finished":0},
									{"Line":0}
								];
			
			treeData[index].Key = window.Append	(
													treeData[index].ParentKey, 
													list_new(type)
												);
												
			map[treeData[index].Key] = index;
						 
		}
		
		if(treeData[parent].Errors > 1 or treeData[parent].Warnings > 1)
		{
			window.SetText(treeData[index].Key, 0, type + "s");
		}
		
		treeData[id] = 	[
							{"URI":file},
							{"Key":0}, 
							{"Parent":index}, 
							{"ParentKey":treeData[index].Key}, 
							{"Childrens":0}, 
							{"CompletedChildrens":0}, 
							{"Errors":0}, 
							{"Warnings":0},
							{"Started":0},
							{"Finished":0},
							{"Line":line}
						];
								
		treeData[id].Key = window.Append(
											treeData[index].Key, 
											list_new("At line " + line + ": " + tostring(content))
										);
									
		map[treeData[id].Key] = id;
		
		Highlight(treeData[id].Key, type + "s fg Color", nil);
		
		treeData[parent][type + "s"]++;
		
		// Pass messages upward.
		PassUp(parent, type + "s");
		
		// Update GUI.
		local updateData = nil;
		if (type == #Error or type == #Warning)
			updateData = [.col : (type == #Error ? 2 : 3), .index : type + "s"];
		Update(parent, updateData);
	}
}

//----------------------- onResourceWorkStarted -----------------------//

function onResourceWorkStarted(invoker, task, workId)
{
	if (not treeData)
		return;
		
	// Set starting time.
	local startedTime = currenttime();
	
	// Get invoker's name.
	local name = spw.decorate(invoker).GetName();
	
	// Get invokers's URI.
	local uri = spw.decorate(invoker).GetURI();
	
	// Create the table index for <workId>
	local index = GetIndex(workId);
	
	// Create the table index for parent.
	local parent = GetParent(workId);
	
	// Create table entry for <workId>.
	local t = treeData[index] = [
		{"URI":uri},
		{"Key":0}, 
		{"Parent":parent}, 
		{"ParentKey":treeData[parent].Key}, 
		{"Childrens":0}, 
		{"CompletedChildrens":0}, 
		{"Errors":0}, 
		{"Warnings":0},
		{"Started":startedTime},
		{"Finished":0}
	];
	
	// Append resource to the tree.
	t.Key = window.Append	(
		t.ParentKey, 
		list_new(invoker.class_id + " : " + name, "0 %", "0", "0")
	);
	
	map[t.Key] = index;
	
	// Highlight.
	Highlight(t.Key, invoker.class_id + "s fg Color", nil);
	
	// Add up childrens.
	PassUp(index, #Childrens);
	
	// Update GUI
	Update(parent, nil);
	
	// Keep the all resource levels of the tree expanded. 
	window.Expand(t.ParentKey);
}

//----------------------- onResourceWorkCompleted -----------------------//

function onResourceWorkCompleted(invoker, task, workId)
{
	if (not treeData)
		return;
		
	// Set finishing time.
	local finishedTime = currenttime();
	
	// Create the table index for <workId>
	local index = GetIndex(workId);
	
	// Create the table index for parent.
	local parent = GetParent(workId);
	
	local t = treeData[index];
	// Add finished time to the table entry.
	t.Finished = finishedTime;
	
	// Calculate completion time.
	local compTime = (t.Finished - t.Started) / 1000;
	compTime = demical(compTime, 2);
	
	// Update completion time.
	window.SetText(t.Key, 4,  compTime + " secs");
	
	// Update completion status to "Done".
	window.SetText(t.Key, 1, "");
	window.SetImage(t.Key, 1, "done");

	// Inform parent.
	PassUp(index, #CompletedChildrens);
	
	// Update GUI
	Update(parent, nil);
}

//----------------------- onWorkspaceClosed -----------------------//

function onWorkspaceClosed (component, uri)
{
	// On exit, clear the window.
	window.Clear();
}

//======================= Component Registration =======================//

onevent ClassLoad
{
	spw::registercomponent(classId, "TreeListViewComponent");
	spw::setcomponentmetadata(
		classId,
		"Build Tree", 
		"Tree view for build order",
		"Christos Despotakis <despotak@ics.forth.gr>", 
		"alpha"
	);
	spw::class_decl_required_member_function(classId, #OnItemActivated, "void (uint serial)");

	spw::class_decl_required_member_handler(classId, #WorkStarted);
	spw::class_decl_required_member_handler(classId, #WorkCompleted);
	spw::class_decl_required_member_handler(classId, #WorkCanceled);
	spw::class_decl_required_member_handler(classId, #CompilationMessage);
	spw::class_decl_required_member_handler(classId, #ResourceWorkStarted);
	spw::class_decl_required_member_handler(classId, #ResourceWorkCompleted);
	spw::class_decl_required_member_handler(classId, #WorkspaceClosed);
							
	spw::class_impl_static_command(
		[
			{.class			: "UserCommandDesc"				},
			{.class_id		: classId						},
			{.function_name	: "ConfigureBuildTreeListView"	},
			{.flags			: 1								}
		],
		"/{2}Configure/Build Tree...",
		(function Configure {spw.components.Shell.ConfigureComponent(classId); }),
		"Configure the Build Tree view"
	);

	local sparrowDir = spw::installationdir();
	spw::registerimage("done", sparrowDir + "/resources/tick.png");
	spw::registerimage("cancel", sparrowDir + "/resources/cancel.png");
	
	SetProperties();
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-----------------------------------------------------------------------

onevent Constructor
{
	spw::inst_impl_required_member_function(classId, #OnItemActivated, onItemActivated);
	
	spw::inst_impl_required_member_handler(classId, #WorkStarted, onWorkStarted);
	spw::inst_impl_required_member_handler(classId, #WorkCompleted, onWorkCompleted);
	spw::inst_impl_required_member_handler(classId, #WorkCanceled, onWorkCanceled);
	spw::inst_impl_required_member_handler(classId, #CompilationMessage, onCompilationMessage);
	spw::inst_impl_required_member_handler(classId, #ResourceWorkStarted, onResourceWorkStarted);
	spw::inst_impl_required_member_handler(classId, #ResourceWorkCompleted, onResourceWorkCompleted);	
	spw::inst_impl_required_member_handler(classId, #WorkspaceClosed, onWorkspaceClosed);

	//This is a GUI component so any other initialization takes place in GenerateWindow
}

//-----------------------------------------------------------------------

onevent Destructor {
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
	
	window.SetTitle("Build Tree");
	
	local columns =	list_new(
						"Name:375", 
						"Completion:65", 
						"Errors:40", 
						"Warnings:60",
						"Completed in...:85"
					);
					
	window.SetColumns(columns);
	
	local images = list_new("done", "cancel");
	window.SetImages(images);
	return nativeWindow;
}

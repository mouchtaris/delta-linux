using #ProjectRAD;
using #ToolBarRAD;
using #ShapeRAD;
using #PatternsEvents;
using #SaveLoad;
using #ConstituentsMouseEvents;
using #idManagement;

/**
 *  Description: Drawing the title of treeview
 *  Arguments:
 *		viewprojToolBar, window of treeview
 *  Return: nope
 */
function drawTreeViewTitle (viewprojToolBar, ev) {
	width = viewprojToolBar.getsize().getwidth();
	//
	local pen = wx::pen_construct(colour = wx::colour_construct(160,160,160), 2);
	viewprojToolBar.dc.setpen(pen);
	pen.destruct();
	colour.destruct();
	local brush = wx::brush_construct(colour = wx::colour_construct(167,167,167));
	viewprojToolBar.dc.setbrush(brush);
	brush.destruct();
	colour.destruct();
	viewprojToolBar.dc.drawrectangle(2, 2, width-4, 18);
	viewprojToolBar.dc.settextforeground(colour = wx::colour_construct(255,255,255));
	colour.destruct();
	viewprojToolBar.dc.drawtext("Project",5,2);
	//
	if (ProjectRAD::GetCurrentProject()==nil) {
		viewprojToolBar.dc.settextforeground(colour = wx::colour_construct(109,109,109));
		colour.destruct();
		viewprojToolBar.dc.drawtext("<No Project Open>", 30, 150);
	}
}

/**
 *  Description: Drawing all raiden when you move the window
 *  Arguments:
 *		frame, main frame which has pined all other windows and data
 *		ev   , event variable
 *  Returns: nope
 */
function onMoveEvent (frame, ev) {
	currProject = ProjectRAD::GetCurrentProject();
	::drawTreeViewTitle (frame.viewprojToolBar,ev);
	ToolBarRAD::drawToolBox();
	// exit in case not project found
	if (std::isundefined(currProject) or std::isnil(currProject))
		return;
	// redraw main page of project
	if (frame.notebook.getpage(frame.notebook.getselection()).getid()==idManagement::Core().MainPageID)
		ShapeRAD::drawPatterns(currProject);
	// redraw current pattern window
	else
	if ((currPattern = currProject.currentPattern)!=nil and (scrolledwindow = currPattern.window)!=nil)
		scrolledwindow.refresh();
}

/**
 *  Description: Drawing all raiden when you resize
 *
 **/
function onResizeEvent (frame, ev) {
	currProject = ProjectRAD::GetCurrentProject();
	::drawTreeViewTitle (frame.viewprojToolBar,ev);
	ToolBarRAD::drawToolBox();
	// exit in case not project found
	if (std::isundefined(currProject) or std::isnil(currProject))
		return;
	if (frame.notebook.getpage(frame.notebook.getselection()).getid()==idManagement::Core().MainPageID)
		ShapeRAD::drawPatterns(currProject);
	else
	if ((currPattern = currProject.currentPattern)!=nil and (scrolledwindow = currPattern.window)!=nil)
		scrolledwindow.refresh();
}

/**
 * Description: The default behaviour is to unsplit the window when this happens, veto this
 *
 **/
function onDClickSplitter (frame, ev) {
	 ev.veto();
}


function onExitRaiden (frame, ev) {
	frame.app.exitmainloop();
	frame.app.exitmainloop();
	frame.app = nil;
}

function onFilterDocument (frame, ev) {
	introduction	=	"Introduction:\n"																								+
						"Its a delta source code, only one file.dsc each filter main function that Raiden call as \nfilter is function"	+
						" Filter, else it is invalid to load it.\n";
	explainInput	=	"Input:\n"																		+
						"As input of this function is a pattern data.\n"								+
						"A Pattern simply store a web of Roles, Operations, Outputs and Requirements.\n\n";
	elements		=	"Elements:\n"																	+
						"> Requirement\n"																+
						"\t1. addrOps,	all ids of Operations that addressed\n"							+
						"\t2. addrRoles,	all ids of Roles that addressed\n"								+
						"> Operation\n"																	+
						"\t1. reqsAddr,	all ids of Requirement that addressed\n"						+
						"\t2. rolesUsed,	all ids of Roles that used this\n"								+
						"\t3. outputs,	all ids of Outputs\n"											+
						"\t4. inputs,		all ids of Inputs\n"											+
						"\t5. roleOffer,	all ids of Roles which offer this **This MUST be only one**\n"	+
						"\t6. parent, 'parent' operation, subOperations, 'sub' operations\n"				+
						"> Role\n"
						"\t1. addrReqs,	all ids of Requirements that addressed\n"		+
						"\t2. opsOffered,	all ids of Operations offered by role\n"	+
						"\t3. opsUsed, 	all ids of Operations used by role\n"			+
						"\t4. outputsUsed,	all ids of Outputs used by role\n"			+
						"\t5. inputsUsed,	all ids of Inputs used by role\n"			+
						"\t6. parent, 'parent' role, subRoles, 'sub' roles\n";
						
	links			=	"Links:\n"														+
						"> Address\n"													+
						"\tRequirement	&&	Operation / Role\n"							+
						"\t.number: reqId	&&	.addrEl with type(Operation/Role) and id\n"	+
						"> Use\n"														+
						"\tOperation	&&	Input  (or Role && Operation / Output)\n"		+
						"\t.element1	&&	.element2\n"
						"\telement includes .type(Operation/Role/Input/Output) and .id\n"	+
						"> Offer\n"														+
						"\tRole	&&	Operation (or Operation	&& Output)\n"		+
						"\t.element1	&&	.element2\n"								+
						"\telement includes .type(Operation/Role/Output) and .id\n"		+
						"> PartOf\n"													+
						"\tRole	&&	Role (or Operation && Operation)\n"		+
						"\t.element	&&	.subelement\n"								+
						"\telement/subelement includes .type(Operation/Role) and .id";
	
	wx::messagebox(introduction + explainInput + elements + links,
					"Help about Filters",
			   wx::flags(wx::OK, wx::ICON_INFORMATION));
}


/**
 *  NOT IMPLEMENTED
 *  Set of functions for configuration Raiden
 **/ 

/**
 *  Description: Changebackground of pages ( main or pattern page )
 *  Arguments:
 *		frame, The window which user want to change background
 *		ev,    
 *  Returns: nope
 **/
function onChangeBackgroundColour (frame, ev) {
	coldlg = wx::colourdialog_construct(frame);
	if (coldlg.showmodal() == wx::flags(wx::ID_OK)) {
		coldata = coldlg.getcolourdata();
	}
}
//
function onChangeSizeOfShapes  (frame, ev) { std::print("NOT IMPLEMENTED\n"); assert(false); }
function onChangeSizeToDefault (frame, ev) { std::print("NOT IMPLEMENTED\n"); assert(false); }
//
function onChangeRoleShape (frame, ev) { std::print("NOT IMPLEMENTED\n"); assert(false); }
function onChangeOperationShape (frame, ev) { std::print("NOT IMPLEMENTED\n"); assert(false); }
function onChangeRequirementShape (frame, ev) { std::print("NOT IMPLEMENTED\n"); assert(false); }
//
function onChangeRequirementColour (frame, ev) { std::print("NOT IMPLEMENTED\n"); assert(false); }
function onChangeOperationColour (frame, ev) { std::print("NOT IMPLEMENTED\n"); assert(false); }
function onChangeRoleColour (frame, ev) { std::print("NOT IMPLEMENTED\n"); assert(false); }
//

//

function propertiesWizard (wizard) {
	page = wx::wizardpagesimple_construct(wizard);
	author     = wx::statictext_construct(page, wx::ID_ANY,"Author Name: ", wx::point_construct(5,200));
	authorCtrl = wx::textctrl_construct(page, 31, "Anonymous", wx::point_construct(80,196), wx::size_construct(200,20), wx::flags(wx::TE_RICH2));
	wizard.author = authorCtrl;
	return page;
}

/**
 *  Description: Complete new project creation
 *  Arguments:
 *  	frame, 
 *  	ev,
 *  Returns: 
 **/
function finishCreateProject (frame, ev) {
	parent = frame.parent;
	title  = frame.fileName.getvalue();
	author = frame.author.getvalue();
	author = ((author!=nil and author!="" and std::strsub(author," ")==-1) ? author:"Anonymous");
	if (title!=nil and title!="" and std::strsub(title," ")==-1) {
		id = parent.info.Project.offset;
		parent.info.Project.offset = id + 1;
		SaveLoad::saveInfo(parent.info);
		project = ProjectRAD::Project(id, title, author, ProjectRAD::ProjProperties(), parent);
		ProjectRAD::SetCurrentProject(project, parent);
		ToolBarRAD::connectToolBar(parent.mainToolBar);
		// connect manage: redraw and currentPattern
		parent.currProject = project;
		parent.connect(12, wx::EVT_COMMAND_NOTEBOOK_PAGE_CHANGED, PatternsEvents::onChangePageNotebook);
		parent.notebook.currProject = project;
		parent.notebook.window = parent;
		parent.notebook.connect(wx::EVT_KEY_DOWN, ConstituentsMouseEvents::onKeyDown);
		// save in rad file
		filepath = frame.location.getvalue() + title;
		std::fileexecute("mkdir "+filepath);
		std::fileexecute("mkdir "+filepath+"\\patterns");
		parent.fileName = filepath+"\\"+title + ".rproj";
		parent.filePath = filepath;
		SaveLoad::onSave(parent,nil);
	}
	else {
		ev.veto();
		ev.skip();
		mesgdlg = wx::messagedialog_construct(frame, "Project must be named!", "Not allowed", 
														wx::flags(wx::ICON_WARNING, wx::OK) );
		mesgdlg.showmodal();
		mesgdlg.destroy();
	}
}

/**
 *  Description: Browse to find the file path that will crete the new project
 *  
 *
 **/
function browseProjectDir (frame, ev) {
	currentPath = frame.wizard.location.getvalue();
	dirdialog = wx::dirdialog_construct(frame, "Project path", currentPath);
	dirdialog.showmodal();
	// fix the path
	newpath = "";
	list = std::strtokenise(dirdialog.getpath(),"\\");
	foreach (cstr, list) newpath += cstr + "\\\\";
	//
	frame.wizard.location.setvalue(newpath);
	dirdialog.destruct();
}
/**
 *  Description: Create the finish page of create project
 *  Arguments:
 *  	wizard, the wizard object of new project rad
 *  Returns: The object finish page of create new project wizard
 **/
function finishWizard (wizard) {
	page = wx::wizardpagesimple_construct(wizard);
	fileName     = wx::statictext_construct(page, wx::ID_ANY,"File Name: ", wx::point_construct(5,200));
	fileNameCtrl = wx::textctrl_construct(page, 31, "", wx::point_construct(60,196), wx::size_construct(350,20), wx::flags(wx::TE_RICH2));
	//
	location     = wx::statictext_construct(page, wx::ID_ANY,"Location: ", wx::point_construct(5,230));
	locationCtrl = wx::textctrl_construct(page, 32, "C:\\Users\\jvalsam\\Desktop\\projects\\", wx::point_construct(60,226), wx::size_construct(350,20), wx::flags(wx::TE_RICH2));
	browse = wx::button_construct(page, 33, "Browse...", wx::point_construct(430, 227), wx::size_construct(70,20), wx::NO_BORDER);
	wizard.fileName = fileNameCtrl;
	wizard.location = locationCtrl;
	// pin wizard data on page
	page.wizard = wizard;
	// browse in which folder to create the new project
	page.connect(33, wx::EVT_COMMAND_BUTTON_CLICKED, ::browseProjectDir);
	wizard.connect(wx::EVT_WIZARD_FINISHED, ::finishCreateProject);
	
	return page;
}

/**
 * Description: Create a new project
 * Arguments:
 * 		frame, main frame of raiden
 * 		ev   , command button event
 * Returns: nope
 **/
function onCreateProject (frame, ev) {
	// create dialog choice name of project
	// maybe create dialog for name project
	// properties of colour etc
	wizard = wx::wizard_construct(frame, wx::ID_ANY, "New Project RAD",
				wx::bitmap_construct("waiztest.bmp", wx::BITMAP_TYPE_BMP), wx::point_construct(-1,-1),
				wx::flags(wx::DEFAULT_DIALOG_STYLE, wx::RESIZE_BORDER));
	local size = wx::size_construct(500,200);
	wizard.setpagesize(size);
	size.destruct();
	wizard.parent = frame;
	page1 = ::propertiesWizard(wizard);
	page2 = ::finishWizard(wizard);
	page1.setnext(page2);
	page2.setprev(page1);
	wizard.runwizard(page1);
}

/**
 * Description: Open a project (*.rproj) that already exists
 * Arguments:
 * 		frame, main frame of raiden
 *		ev   , command button event
 * Returns: nope
 **/
function onOpenProject (frame,ev) {
	filedlg = wx::filedialog_construct(frame, "Open Raiden files", "", "",
	"Raiden files (*.rproj)|*.rproj", wx::flags(wx::FD_OPEN));
	if (filedlg.showmodal() == wx::flags(wx::ID_OK)) {
		fileName = filedlg.getpath();
		filePath = "";
		tmp = std::strtokenise(fileName,"\\");
		// remove file name of the path
		tmp.pop_back();
		//
		index = 0;
		foreach (el, tmp) {
			if (index!=0) filePath += "\\";
			filePath += el;
			++index; }
		frame.fileName = fileName;
		frame.filePath = filePath;
		SaveLoad::onLoad(frame,ev);
	}
}


using #ImageHolder;
using #EditDialogs;

/* Utilities */

function onCancel (frame, ev) { frame.destroy(); }
function onKeyDownDiag (frame, ev) { if (ev.getkeycode()==27) frame.destroy(); }
function onKeyDown (frame, ev) { if (ev.getkeycode()==27) frame.dialog.destroy(); }

// interface of implementation notes on each of elements of project
function onOpen (frame, ev) {}


/* Requirement */

function RequirementProps(req, propsWin, currProject) {
	wxRequirement = [];
	wxRequirement.shortDescText = wx::statictext_construct(propsWin, wx::ID_ANY,"Short\nDescription:",wx::point_construct(15,20));
	wxRequirement.shortDesc = wx::textctrl_construct(propsWin, wx::ID_ANY, req.shortDesc, wx::point_construct(95,18), wx::size_construct(260,30), wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	wxRequirement.statusText = wx::statictext_construct(propsWin, wx::ID_ANY,"Status:", wx::point_construct(15,65));	
	wxRequirement.status 	 = wx::combobox_construct(propsWin, wx::ID_ANY, "Status", wx::point_construct(60,61), wx::size_construct(110,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default status value cases
	//for(value,listValues) statusCombo.append(value); statusCombo.setvalue(req.status);
	wxRequirement.status.append("Approved");
	wxRequirement.status.append("Implemented");
	wxRequirement.status.append("Mandatory");
	wxRequirement.status.append("Proposed");
	wxRequirement.status.append("Validated");
	wxRequirement.status.setvalue(req.status);

	wxRequirement.priorityText  = wx::statictext_construct(propsWin,wx::ID_ANY,"Priority:",wx::point_construct(15,94));
	wxRequirement.priority = wx::combobox_construct(propsWin, wx::ID_ANY, req.priority, wx::point_construct(60,90), wx::size_construct(110,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default priority value cases
	wxRequirement.priority.append("Higher");
	wxRequirement.priority.append("High");
	wxRequirement.priority.append("Medium");
	wxRequirement.priority.append("Low");
	wxRequirement.priority.append("Lower");
	wxRequirement.priority.setvalue(req.priority);

	wxRequirement.typeText = wx::statictext_construct(propsWin,wx::ID_ANY,"Type:",wx::point_construct(200,65));
	wxRequirement.type = wx::combobox_construct(propsWin, wx::ID_ANY, req.type, wx::point_construct(240,61), wx::size_construct(110,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default type value cases
	wxRequirement.type.append("Architectural");
	wxRequirement.type.append("Structural");
	wxRequirement.type.append("Behavioral");
	wxRequirement.type.append("Functional");
	wxRequirement.type.append("Non-functional");
	wxRequirement.type.setvalue(req.type);

	wxRequirement.difficultyText = wx::statictext_construct(propsWin,wx::ID_ANY,"Difficulty:", wx::point_construct(200,94));
	wxRequirement.difficulty = wx::combobox_construct(propsWin, wx::ID_ANY, req.difficulty, wx::point_construct(255,90), wx::size_construct(95,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default type value cases
	wxRequirement.difficulty.append("Higher");
	wxRequirement.difficulty.append("High");
	wxRequirement.difficulty.append("Medium");
	wxRequirement.difficulty.append("Low");
	wxRequirement.difficulty.append("Lower");
	wxRequirement.difficulty.setvalue(req.difficulty);
	
	wxRequirement.authorText = wx::statictext_construct(propsWin,wx::ID_ANY,"Author:",wx::point_construct(15,130));
	wxRequirement.author = wx::combobox_construct(propsWin, wx::ID_ANY, req.author, wx::point_construct(60,126), wx::size_construct(130,20), [], wx::flags(wx::CB_DROPDOWN));
	// default type value cases
	foreach (author, currProject.suggestAuthors)
		wxRequirement.author.append(author);
	
	datetime = req.lastUpdate;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	
	wxRequirement.lastUpdateText = wx::statictext_construct(propsWin,wx::ID_ANY,"Last Update:",wx::point_construct(15,168));
	wxRequirement.lastUpdate 	 = wx::textctrl_construct(propsWin,wx::ID_ANY,datetimeStr,wx::point_construct(80,164),wx::size_construct(80,20),wx::flags(wx::TE_READONLY));

	datetime = req.created;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();

	wxRequirement.createdText = wx::statictext_construct(propsWin,wx::ID_ANY,"Created:",wx::point_construct(180,168));
	wxRequirement.created     = wx::textctrl_construct(propsWin,31,datetimeStr,wx::point_construct(230,164),wx::size_construct(80,20),wx::flags(wx::TE_READONLY));
	
//	wxRequirement.st = wx::radiobox_construct(propsWin, -1, "Description", wx::point_construct(15, 200), wx::size_construct(360, 150));
	wxRequirement.st = wx::staticbox_construct(propsWin, -1, "Description", wx::point_construct(15, 200), wx::size_construct(360, 150));
	wxRequirement.description = wx::textctrl_construct(wxRequirement.st, wx::ID_ANY, req.description, wx::point_construct(15,20),wx::size_construct(330,120),wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	
	return wxRequirement;
}

function RequirementNotes(req, notes, currProject) {
	
	filePath = wx::statictext_construct(notes, wx::ID_ANY, "File Path:", wx::point_construct(35,39));
	notes.tc = wx::textctrl_construct(notes, -1, "", wx::point_construct(85, 35), wx::size_construct(285, 20));
	openID = currProject.connManageId.getID();
	openButton = wx::button_construct(notes, openID, "...", wx::point_construct(372,37), wx::size_construct(20, 16));
	notes.connect(openID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onOpen);
	
	noteText = wx::statictext_construct(notes, wx::ID_ANY,"Note:", wx::point_construct(35,70));
	noteCtrl = wx::textctrl_construct(notes, wx::ID_ANY, "", wx::point_construct(85,70), wx::size_construct(300,50),wx::flags(wx::TE_MULTILINE,wx::TE_RICH));
	rb1 = wx::radiobutton_construct(notes, wx::ID_ANY, "Text", wx::point_construct(285, 125));
	rb1.setvalue(true);
	rb2 = wx::radiobutton_construct(notes, wx::ID_ANY, "Link", wx::point_construct(330, 125));

	viewID = currProject.connManageId.getID();
	viewButton = wx::button_construct(notes, viewID, "View", wx::point_construct(190,155), wx::size_construct(60, 25));
	saveID = currProject.connManageId.getID();
	saveButton = wx::button_construct(notes, saveID, "Save", wx::point_construct(255,155), wx::size_construct(60, 25));
	deleteID = currProject.connManageId.getID();
	deleteButton = wx::button_construct(notes, deleteID, "Delete", wx::point_construct(320,155), wx::size_construct(60, 25));

//	st = wx::radiobox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	st = wx::staticbox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	listctrl = wx::listctrl_construct(st, wx::ID_ANY, wx::point_construct(9,15), wx::size_construct(340,155), wx::LC_LIST);
	k = -1;
/*	items = req.notes;
	foreach (item, items)
		listctrl.insertitem(++k, item);*/
	return [openID, viewID, saveID, deleteID];
}

function Requirement (req, currProject) {
	dialog = wx::dialog_construct(nil, wx::ID_ANY, "Requirement", wx::point_construct(0,0), wx::size_construct(450, 480));
	dialog.centre();
	dialog.show(true);
	panel = wx::panel_construct(dialog, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(450, 480));
	notebook = wx::notebook_construct(panel, wx::ID_ANY,wx::point_construct(22,15),wx::size_construct(400,390));
	notebook.setbackgroundcolour(colour = wx::colour_construct(240,240,240));
	propsWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	propsWin.show(true);
	notesWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	notesWin.show(true);
	wxreq = ::RequirementProps(req, propsWin, currProject);
	notesIds = ::RequirementNotes(req, notesWin, currProject);
	notebook.setimagelist(ImageHolder::images().imageListNB2);
	notebook.addpage(propsWin, "Properties", true, ImageHolder::images().imageListNB2.properties);
	notebook.addpage(notesWin, "Notes", false, ImageHolder::images().imageListNB2.notes);
	local okID = currProject.connManageId.getID();
	okButton = wx::button_construct(panel, okID, "Ok", wx::point_construct(350,415), wx::size_construct(70, 25));
	local cancelID = currProject.connManageId.getID();
	cancelButton = wx::button_construct(panel, cancelID, "Cancel", wx::point_construct(275,415), wx::size_construct(70, 25));
	dialog.req = req;
	dialog.props   = wxreq;
	dialog.currProject = currProject;
	dialog.connect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onRequirementOk);
	dialog.connect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.connect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.dialog = dialog;
	panel.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.dialog = dialog;
	notebook.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.dialog = dialog;
	propsWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.dialog = dialog;
	notesWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.showmodal();
	dialog.disconnect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onRequirementOk);
	dialog.disconnect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.disconnect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.destroy();
	currProject.connManageId.removeIDs([okID, cancelID]);
	currProject.connManageId.removeIDs(notesIds);
	notesIds = nil;
}

function onRequirement (frame, ev) {
	req = frame.constituent;
	currProject = frame.currProject;
	::Requirement(req, currProject);
}


/* Operation */


function OperationProps(operation, propsWin, currProject) {
	wxRequirement = [];
	wxRequirement.shortDescText = wx::statictext_construct(propsWin, wx::ID_ANY,"Short\nDescription:",wx::point_construct(15,20));
	wxRequirement.shortDesc     = wx::textctrl_construct(propsWin, wx::ID_ANY, operation.shortDesc, wx::point_construct(95,18), wx::size_construct(260,30), wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	wxRequirement.statusText    = wx::statictext_construct(propsWin, wx::ID_ANY,"Status:", wx::point_construct(15,65));	
	wxRequirement.status 	    = wx::combobox_construct(propsWin, wx::ID_ANY, "Status", wx::point_construct(60,61), wx::size_construct(110,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default status value cases
	//for(value,listValues) statusCombo.append(value); statusCombo.setvalue(req.status);
	wxRequirement.status.append("Approved");
	wxRequirement.status.append("Implemented");
	wxRequirement.status.append("Mandatory");
	wxRequirement.status.append("Proposed");
	wxRequirement.status.append("Validated");
	wxRequirement.status.setvalue(operation.status);

	wxRequirement.priorityText  = wx::statictext_construct(propsWin,wx::ID_ANY,"Priority:",wx::point_construct(15,94));
	wxRequirement.priority = wx::combobox_construct(propsWin, wx::ID_ANY, operation.priority, wx::point_construct(60,90), wx::size_construct(110,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default priority value cases
	wxRequirement.priority.append("Higher");
	wxRequirement.priority.append("High");
	wxRequirement.priority.append("Medium");
	wxRequirement.priority.append("Low");
	wxRequirement.priority.append("Lower");
	wxRequirement.priority.setvalue(operation.priority);

	wxRequirement.difficultyText = wx::statictext_construct(propsWin,wx::ID_ANY,"Difficulty:", wx::point_construct(200,94));
	wxRequirement.difficulty = wx::combobox_construct(propsWin, wx::ID_ANY, operation.difficulty, wx::point_construct(255,90), wx::size_construct(95,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default type value cases
	wxRequirement.difficulty.append("Higher");
	wxRequirement.difficulty.append("High");
	wxRequirement.difficulty.append("Medium");
	wxRequirement.difficulty.append("Low");
	wxRequirement.difficulty.append("Lower");
	wxRequirement.difficulty.setvalue(operation.difficulty);
	
	wxRequirement.authorText = wx::statictext_construct(propsWin,wx::ID_ANY,"Author:",wx::point_construct(15,130));
	wxRequirement.author = wx::combobox_construct(propsWin, wx::ID_ANY, operation.author, wx::point_construct(60,126), wx::size_construct(130,20), [], wx::flags(wx::CB_DROPDOWN));
	// default type value cases
	foreach (author, currProject.suggestAuthors)
		wxRequirement.author.append(author);
	
	datetime = operation.lastUpdate;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	
	wxRequirement.lastUpdateText = wx::statictext_construct(propsWin,wx::ID_ANY,"Last Update:",wx::point_construct(15,168));
	wxRequirement.lastUpdate 	 = wx::textctrl_construct(propsWin,wx::ID_ANY,datetimeStr,wx::point_construct(80,164),wx::size_construct(80,20),wx::flags(wx::TE_READONLY));

	datetime = operation.created;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();

	wxRequirement.createdText = wx::statictext_construct(propsWin,wx::ID_ANY,"Created:",wx::point_construct(180,168));
	wxRequirement.created     = wx::textctrl_construct(propsWin,31,datetimeStr,wx::point_construct(230,164),wx::size_construct(80,20),wx::flags(wx::TE_READONLY));
	
//	wxRequirement.st = wx::radiobox_construct(propsWin, -1, "Description", wx::point_construct(15, 200), wx::size_construct(360, 150));
	wxRequirement.st = wx::staticbox_construct(propsWin, -1, "Description", wx::point_construct(15, 200), wx::size_construct(360, 150));
	wxRequirement.description = wx::textctrl_construct(wxRequirement.st, wx::ID_ANY, operation.description, wx::point_construct(15,20),wx::size_construct(330,120),wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	
	return wxRequirement;
}

function OperationNotes(operation, notes, currProject) {
	filePath = wx::statictext_construct(notes, wx::ID_ANY, "File Path:", wx::point_construct(35,39));
	notes.tc = wx::textctrl_construct(notes, -1, "", wx::point_construct(85, 35), wx::size_construct(285, 20));
	openID = currProject.connManageId.getID();
	openButton = wx::button_construct(notes, openID, "...", wx::point_construct(372,37), wx::size_construct(20, 16));
	notes.connect(openID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onOpen);
	
	noteText = wx::statictext_construct(notes, wx::ID_ANY,"Note:", wx::point_construct(35,70));
	noteCtrl = wx::textctrl_construct(notes, wx::ID_ANY, "", wx::point_construct(85,70), wx::size_construct(300,50),wx::flags(wx::TE_MULTILINE,wx::TE_RICH));
	rb1 = wx::radiobutton_construct(notes, wx::ID_ANY, "Text", wx::point_construct(285, 125));
	rb1.setvalue(true);
	rb2 = wx::radiobutton_construct(notes, wx::ID_ANY, "Link", wx::point_construct(330, 125));

	viewID = currProject.connManageId.getID();
	viewButton = wx::button_construct(notes, viewID, "View", wx::point_construct(190,155), wx::size_construct(60, 25));
	saveID = currProject.connManageId.getID();
	saveButton = wx::button_construct(notes, saveID, "Save", wx::point_construct(255,155), wx::size_construct(60, 25));
	deleteID = currProject.connManageId.getID();
	deleteButton = wx::button_construct(notes, deleteID, "Delete", wx::point_construct(320,155), wx::size_construct(60, 25));

//	st = wx::radiobox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	st = wx::staticbox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	listctrl = wx::listctrl_construct(st, wx::ID_ANY, wx::point_construct(9,15), wx::size_construct(340,155), wx::LC_LIST);
	k = -1;
/*	items = req.notes;
	foreach (item, items)
		listctrl.insertitem(++k, item);*/
	return [openID, viewID, saveID, deleteID];
}

function Operation (operation, currProject) {
	dialog = wx::dialog_construct(nil, wx::ID_ANY, "Operation", wx::point_construct(0,0), wx::size_construct(450, 480));
	dialog.centre();
	dialog.show(true);
	panel = wx::panel_construct(dialog, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(450, 480));
	notebook = wx::notebook_construct(panel, wx::ID_ANY,wx::point_construct(22,15),wx::size_construct(400,390));
	notebook.setbackgroundcolour(colour = wx::colour_construct(240,240,240));
	propsWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	propsWin.show(true);
	notesWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	notesWin.show(true);
	wxreq = ::OperationProps(operation, propsWin, currProject);
	notesIds = ::OperationNotes(operation, notesWin, currProject);
	notebook.setimagelist(ImageHolder::images().imageListNB2);
	notebook.addpage(propsWin, "Properties", true, ImageHolder::images().imageListNB2.properties);
	notebook.addpage(notesWin, "Notes", false, ImageHolder::images().imageListNB2.notes);
	local okID = currProject.connManageId.getID();
	okButton = wx::button_construct(panel, okID, "Ok", wx::point_construct(350,415), wx::size_construct(70, 25));
	local cancelID = currProject.connManageId.getID();
	cancelButton = wx::button_construct(panel, cancelID, "Cancel", wx::point_construct(275,415), wx::size_construct(70, 25));
	dialog.operation   = operation;
	dialog.props       = wxreq;
	dialog.currProject = currProject;
	dialog.connect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onOperationOk);
	dialog.connect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.connect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.dialog = dialog;
	panel.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.dialog = dialog;
	notebook.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.dialog = dialog;
	propsWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.dialog = dialog;
	notesWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.showmodal();
	dialog.disconnect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onOperationOk);
	dialog.disconnect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.disconnect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.destroy();
	currProject.connManageId.removeIDs([okID, cancelID]);
	currProject.connManageId.removeIDs(notesIds);
	notesIds = nil;
}

function onOperation (frame, ev) {
	operation = frame.constituent;
	currProject = frame.currProject;
	::Operation(operation, currProject);
}


/* Role */


function RoleProps(role, propsWin, currProject) {
	wxRole = [];
	wxRole.shortDescText = wx::statictext_construct(propsWin, wx::ID_ANY,"Short\nDescription:",wx::point_construct(15,20));
	wxRole.shortDesc     = wx::textctrl_construct(propsWin, wx::ID_ANY, role.shortDesc, wx::point_construct(95,18), wx::size_construct(260,30), wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	wxRole.statusText    = wx::statictext_construct(propsWin, wx::ID_ANY,"Status:", wx::point_construct(15,65));	
	wxRole.status 	    = wx::combobox_construct(propsWin, wx::ID_ANY, "Status", wx::point_construct(60,61), wx::size_construct(110,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default status value cases
	//for(value,listValues) statusCombo.append(value); statusCombo.setvalue(req.status);
	wxRole.status.append("Approved");
	wxRole.status.append("Implemented");
	wxRole.status.append("Mandatory");
	wxRole.status.append("Proposed");
	wxRole.status.append("Validated");
	wxRole.status.setvalue(role.status);
	wxRole.priorityText  = wx::statictext_construct(propsWin,wx::ID_ANY,"Priority:",wx::point_construct(15,94));
	wxRole.priority = wx::combobox_construct(propsWin, wx::ID_ANY, role.priority, wx::point_construct(60,90), wx::size_construct(110,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default priority value cases
	wxRole.priority.append("Higher");
	wxRole.priority.append("High");
	wxRole.priority.append("Medium");
	wxRole.priority.append("Low");
	wxRole.priority.append("Lower");
	wxRole.priority.setvalue(role.priority);
	wxRole.difficultyText = wx::statictext_construct(propsWin,wx::ID_ANY,"Difficulty:", wx::point_construct(200,94));
	wxRole.difficulty = wx::combobox_construct(propsWin, wx::ID_ANY, role.difficulty, wx::point_construct(255,90), wx::size_construct(95,20), [], wx::flags(wx::CB_DROPDOWN, wx::CB_READONLY));
	// default type value cases
	wxRole.difficulty.append("Higher");
	wxRole.difficulty.append("High");
	wxRole.difficulty.append("Medium");
	wxRole.difficulty.append("Low");
	wxRole.difficulty.append("Lower");
	wxRole.difficulty.setvalue(role.difficulty);
	wxRole.authorText = wx::statictext_construct(propsWin,wx::ID_ANY,"Author:",wx::point_construct(15,130));
	wxRole.author = wx::combobox_construct(propsWin, wx::ID_ANY, role.author, wx::point_construct(60,126), wx::size_construct(130,20), [], wx::flags(wx::CB_DROPDOWN));
	// default type value cases
	foreach (author, currProject.suggestAuthors)
		wxRole.author.append(author);
	datetime = role.lastUpdate;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	wxRole.lastUpdateText = wx::statictext_construct(propsWin,wx::ID_ANY,"Last Update:",wx::point_construct(15,168));
	wxRole.lastUpdate 	 = wx::textctrl_construct(propsWin,wx::ID_ANY,datetimeStr,wx::point_construct(80,164),wx::size_construct(80,20),wx::flags(wx::TE_READONLY));
	datetime = role.created;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	wxRole.createdText = wx::statictext_construct(propsWin,wx::ID_ANY,"Created:",wx::point_construct(180,168));
	wxRole.created     = wx::textctrl_construct(propsWin,31,datetimeStr,wx::point_construct(230,164),wx::size_construct(80,20),wx::flags(wx::TE_READONLY));
//	wxRole.st = wx::radiobox_construct(propsWin, -1, "Description", wx::point_construct(15, 200), wx::size_construct(360, 150));
	wxRole.st = wx::staticbox_construct(propsWin, -1, "Description", wx::point_construct(15, 200), wx::size_construct(360, 150));
	wxRole.description = wx::textctrl_construct(wxRole.st, wx::ID_ANY, role.description, wx::point_construct(15,20),wx::size_construct(330,120),wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));	
	return wxRole;
}

function RoleNotes(role, notes, currProject) {
	filePath = wx::statictext_construct(notes, wx::ID_ANY, "File Path:", wx::point_construct(35,39));
	notes.tc = wx::textctrl_construct(notes, -1, "", wx::point_construct(85, 35), wx::size_construct(285, 20));
	openID = currProject.connManageId.getID();
	openButton = wx::button_construct(notes, openID, "...", wx::point_construct(372,37), wx::size_construct(20, 16));
	notes.connect(openID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onOpen);
	
	noteText = wx::statictext_construct(notes, wx::ID_ANY,"Note:", wx::point_construct(35,70));
	noteCtrl = wx::textctrl_construct(notes, wx::ID_ANY, "", wx::point_construct(85,70), wx::size_construct(300,50),wx::flags(wx::TE_MULTILINE,wx::TE_RICH));
	rb1 = wx::radiobutton_construct(notes, wx::ID_ANY, "Text", wx::point_construct(285, 125));
	rb1.setvalue(true);
	rb2 = wx::radiobutton_construct(notes, wx::ID_ANY, "Link", wx::point_construct(330, 125));

	viewID = currProject.connManageId.getID();
	viewButton = wx::button_construct(notes, viewID, "View", wx::point_construct(190,155), wx::size_construct(60, 25));
	saveID = currProject.connManageId.getID();
	saveButton = wx::button_construct(notes, saveID, "Save", wx::point_construct(255,155), wx::size_construct(60, 25));
	deleteID = currProject.connManageId.getID();
	deleteButton = wx::button_construct(notes, deleteID, "Delete", wx::point_construct(320,155), wx::size_construct(60, 25));

//	st = wx::radiobox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	st = wx::staticbox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	listctrl = wx::listctrl_construct(st, wx::ID_ANY, wx::point_construct(9,15), wx::size_construct(340,155), wx::LC_LIST);
	k = -1;
/*	items = req.notes;
	foreach (item, items)
		listctrl.insertitem(++k, item);*/
	return [openID, viewID, saveID, deleteID];
}

function Role (role, currProject, frame) {
	dialog = wx::dialog_construct(nil, wx::ID_ANY, "Role", wx::point_construct(0,0), wx::size_construct(450, 480));
	dialog.centre();
	dialog.show(true);
	panel = wx::panel_construct(dialog, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(450, 480));
	notebook = wx::notebook_construct(panel, wx::ID_ANY,wx::point_construct(22,15),wx::size_construct(400,390));
	notebook.setbackgroundcolour(colour = wx::colour_construct(240,240,240));
	propsWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	propsWin.show(true);
	notesWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	notesWin.show(true);
	wxreq = ::RoleProps(role, propsWin, currProject);
	notesIds = ::RoleNotes(role, notesWin, currProject);
	notebook.setimagelist(ImageHolder::images().imageListNB2);
	notebook.addpage(propsWin, "Properties", true, ImageHolder::images().imageListNB2.properties);
	notebook.addpage(notesWin, "Notes", false, ImageHolder::images().imageListNB2.notes);
	local okID = currProject.connManageId.getID();
	okButton = wx::button_construct(panel, okID, "Ok", wx::point_construct(350,415), wx::size_construct(70, 25));
	local cancelID = currProject.connManageId.getID();
	cancelButton = wx::button_construct(panel, cancelID, "Cancel", wx::point_construct(275,415), wx::size_construct(70, 25));
	dialog.role   = role;
	dialog.props  = wxreq;
	dialog.currProject = currProject;
	dialog.frame = frame;
	dialog.connect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onRoleOk);
	dialog.connect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.connect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.dialog = dialog;
	panel.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.dialog = dialog;
	notebook.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.dialog = dialog;
	propsWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.dialog = dialog;
	notesWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.showmodal();
	dialog.disconnect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onRoleOk);
	dialog.disconnect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.disconnect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.destroy();
	currProject.connManageId.removeIDs([okID, cancelID]);
	currProject.connManageId.removeIDs(notesIds);
	notesIds = nil;
}

function onRole (frame, ev) {
	role = frame.constituent;
	currProject = frame.currProject;
	::Role(role, currProject, frame.wxparent);
}

/* Input - Output */



function IoProps(io, propsWin, currProject) {
	wxIo = [];
	wxIo.shortDescText = wx::statictext_construct(propsWin, wx::ID_ANY,"Short\nDescription:",wx::point_construct(15,20));
	wxIo.shortDesc     = wx::textctrl_construct(propsWin, wx::ID_ANY, io.shortDesc, wx::point_construct(95,18), wx::size_construct(260,30), wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	datetime = io.lastUpdate;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	wxIo.lastUpdateText = wx::statictext_construct(propsWin,wx::ID_ANY,"Last Update:",wx::point_construct(15,168));
	wxIo.lastUpdate 	 = wx::textctrl_construct(propsWin,wx::ID_ANY,datetimeStr,wx::point_construct(80,164),wx::size_construct(80,20),wx::flags(wx::TE_READONLY));
	datetime = io.created;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	wxIo.createdText = wx::statictext_construct(propsWin,wx::ID_ANY,"Created:",wx::point_construct(180,168));
	wxIo.created     = wx::textctrl_construct(propsWin,31,datetimeStr,wx::point_construct(230,164),wx::size_construct(80,20),wx::flags(wx::TE_READONLY));
//	wxIo.st = wx::radiobox_construct(propsWin, -1, "Description", wx::point_construct(15, 200), wx::size_construct(360, 150));
	wxIo.st = wx::staticbox_construct(propsWin, -1, "Description", wx::point_construct(15, 200), wx::size_construct(360, 150));
	wxIo.description = wx::textctrl_construct(wxIo.st, wx::ID_ANY, io.description, wx::point_construct(15,20),wx::size_construct(330,120),wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	return wxIo;
}

function IoNotes(io, notes, currProject) {
	filePath = wx::statictext_construct(notes, wx::ID_ANY, "File Path:", wx::point_construct(35,39));
	notes.tc = wx::textctrl_construct(notes, -1, "", wx::point_construct(85, 35), wx::size_construct(285, 20));
	openID = currProject.connManageId.getID();
	openButton = wx::button_construct(notes, openID, "...", wx::point_construct(372,37), wx::size_construct(20, 16));
	notes.connect(openID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onOpen);
	
	noteText = wx::statictext_construct(notes, wx::ID_ANY,"Note:", wx::point_construct(35,70));
	noteCtrl = wx::textctrl_construct(notes, wx::ID_ANY, "", wx::point_construct(85,70), wx::size_construct(300,50),wx::flags(wx::TE_MULTILINE,wx::TE_RICH));
	rb1 = wx::radiobutton_construct(notes, wx::ID_ANY, "Text", wx::point_construct(285, 125));
	rb1.setvalue(true);
	rb2 = wx::radiobutton_construct(notes, wx::ID_ANY, "Link", wx::point_construct(330, 125));

	viewID = currProject.connManageId.getID();
	viewButton = wx::button_construct(notes, viewID, "View", wx::point_construct(190,155), wx::size_construct(60, 25));
	saveID = currProject.connManageId.getID();
	saveButton = wx::button_construct(notes, saveID, "Save", wx::point_construct(255,155), wx::size_construct(60, 25));
	deleteID = currProject.connManageId.getID();
	deleteButton = wx::button_construct(notes, deleteID, "Delete", wx::point_construct(320,155), wx::size_construct(60, 25));

//	st = wx::radiobox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	st = wx::staticbox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	listctrl = wx::listctrl_construct(st, wx::ID_ANY, wx::point_construct(9,15), wx::size_construct(340,155), wx::LC_LIST);
	k = -1;
/*	items = req.notes;
	foreach (item, items)
		listctrl.insertitem(++k, item);*/
	return [openID, viewID, saveID, deleteID];
}

function Io (io, currProject) {
	dialog = wx::dialog_construct(nil, wx::ID_ANY, io.class, wx::point_construct(0,0), wx::size_construct(450, 480));
	dialog.centre();
	dialog.show(true);
	panel = wx::panel_construct(dialog, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(450, 480));
	notebook = wx::notebook_construct(panel, wx::ID_ANY,wx::point_construct(22,15),wx::size_construct(400,390));
	notebook.setbackgroundcolour(colour = wx::colour_construct(240,240,240));
	propsWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	propsWin.show(true);
	notesWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	notesWin.show(true);
	wxreq = ::IoProps(io, propsWin, currProject);
	notesIds = ::IoNotes(io, notesWin, currProject);
	notebook.setimagelist(ImageHolder::images().imageListNB2);
	notebook.addpage(propsWin, "Properties", true, ImageHolder::images().imageListNB2.properties);
	notebook.addpage(notesWin, "Notes", false, ImageHolder::images().imageListNB2.notes);
	local okID = currProject.connManageId.getID();
	okButton = wx::button_construct(panel, okID, "Ok", wx::point_construct(350,415), wx::size_construct(70, 25));
	local cancelID = currProject.connManageId.getID();
	cancelButton = wx::button_construct(panel, cancelID, "Cancel", wx::point_construct(275,415), wx::size_construct(70, 25));
	dialog.io			= io;
	dialog.props		= wxreq;
	dialog.currProject	= currProject;
	dialog.connect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onIoOk);
	dialog.connect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.connect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.dialog = dialog;
	panel.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.dialog = dialog;
	notebook.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.dialog = dialog;
	propsWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.dialog = dialog;
	notesWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.showmodal();
	dialog.disconnect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onIoOk);
	dialog.disconnect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.disconnect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.destroy();
	currProject.connManageId.removeIDs([okID, cancelID]);
	currProject.connManageId.removeIDs(notesIds);
	notesIds = nil;
}

function onIo (frame, ev) {
	io = frame.constituent;
	currProject = frame.currProject;
	::Io(io, currProject/*,"output"*/);
}
function Input 	(io, currProject) { ::Io(io, currProject); }
function Output	(io, currProject) { ::Io(io, currProject); }


/* Relations (Address, Use, Offer, Inherit) */

function AddressProps(address, propsWin, currProject) {}
function AddressNotes(address, notesWin, currProject) {}
function onAddressOk(frame, ev) {}

function UseProps(address, propsWin, currProject) {}
function UseNotes(address, notesWin, currProject) {}
function onUseOk(frame, ev) {}

function OfferProps(address, propsWin, currProject) {}
function OfferNotes(address, notesWin, currProject) {}
function onOfferOk(frame, ev) {}

function InheritProps(address, propsWin, currProject) {}
function InheritNotes(address, notesWin, currProject) {}
function onInheritOk(frame, ev) {}


function Relation (constituent, currProject) {
/*	dialog = wx::dialog_construct(nil, wx::ID_ANY, constituent.class, wx::point_construct(0,0), wx::size_construct(450, 480));
	dialog.centre();
	dialog.show(true);
	panel = wx::panel_construct(dialog, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(450, 480));
	notebook = wx::notebook_construct(panel, wx::ID_ANY,wx::point_construct(22,15),wx::size_construct(400,390));
	notebook.setbackgroundcolour(colour = wx::colour_construct(240,240,240));
	colour.destruct();
	propsWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	propsWin.show(true);
	notesWin = wx::window_construct(panel, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	notesWin.show(true);
	wxreq = AddrFuncs::Dialogs(constituent.class+"Props")(constituent, propsWin, currProject);
	notesIds = AddrFuncs::Dialogs(constituent.class+"Notes")(constituent, notesWin, currProject);
	notebook.setimagelist(ImageHolder::images().imageListNB2);
	notebook.addpage(propsWin, "Properties", true, ImageHolder::images().imageListNB2.properties);
	notebook.addpage(notesWin, "Notes", false, ImageHolder::images().imageListNB2.notes);
	local okID = currProject.connManageId.getID();
	okButton = wx::button_construct(panel, okID, "Ok", wx::point_construct(350,415), wx::size_construct(70, 25));
	local cancelID = currProject.connManageId.getID();
	cancelButton = wx::button_construct(panel, cancelID, "Cancel", wx::point_construct(275,415), wx::size_construct(70, 25));
	dialog[constituent.class]	= constituent;
	dialog.props				= wxreq;
	dialog.currProject			= currProject;
	dialog.connect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, AddrFuncs::Dialogs("on"+constituent.class+"Ok"));
	dialog.connect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.connect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.dialog = dialog;
	panel.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.dialog = dialog;
	notebook.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.dialog = dialog;
	propsWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.dialog = dialog;
	notesWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.showmodal();
	dialog.disconnect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, AddrFuncs::Dialogs("on"+constituent.class+"Ok"));
	dialog.disconnect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.disconnect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.destruct();
	currProject.connManageId.removeIDs([okID, cancelID]);
	currProject.connManageId.removeIDs(notesIds);
	notesIds = nil;*/
}

function onRelation (frame, ev) {
	constituent = frame.constituent;
	currProject = frame.currProject;
	::Relation(constituent, currProject);
}

/* Filter - Pattern */

function FilterProps(filter, propsWin, currProject) {
	shortDescText = wx::statictext_construct(propsWin,wx::ID_ANY,"Short\nDescription:",wx::point_construct(15,20));
	shortDescCtrl = wx::textctrl_construct(propsWin,wx::ID_ANY, filter.name, wx::point_construct(95,18),wx::size_construct(260,30),wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	propsWin.name = shortDescCtrl;
	authorText = wx::statictext_construct(propsWin,wx::ID_ANY, "Author:",wx::point_construct(15,65));
	authorCombo = wx::combobox_construct(propsWin, wx::ID_ANY, filter.author, wx::point_construct(60,61), wx::size_construct(130,20), [], wx::flags(wx::CB_DROPDOWN));
	propsWin.author = authorCombo;
	// default type value cases
	authors = currProject.suggestAuthors;
	foreach (author, authors)
		authorCombo.append(author);
	datetime = filter.lastUpdate;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	lastUpdateText = wx::statictext_construct(propsWin, wx::ID_ANY,"Last Update:",wx::point_construct(15,100));
	lastUdate = wx::textctrl_construct(propsWin, wx::ID_ANY, datetimeStr, wx::point_construct(80,96), wx::size_construct(80,20), wx::flags(wx::TE_READONLY));
	datetime = filter.created;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	createdText = wx::statictext_construct(propsWin,wx::ID_ANY,"Created:",wx::point_construct(180,100));
	createdCtrl = wx::textctrl_construct(propsWin,wx::ID_ANY,datetimeStr,wx::point_construct(230,96),wx::size_construct(80,20), wx::flags(wx::TE_READONLY));
//	st = wx::radiobox_construct(propsWin, wx::ID_ANY, "Description", wx::point_construct(15, 130), wx::size_construct(360, 200));
	st = wx::staticbox_construct(propsWin, wx::ID_ANY, "Description", wx::point_construct(15, 130), wx::size_construct(360, 200));
	textctrl = wx::textctrl_construct(st,wx::ID_ANY, filter.description, wx::point_construct(15,20),wx::size_construct(330,170),wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	propsWin.description = textctrl;
}

function FilterNotes(filter, notes, currProject) {
	filePath = wx::statictext_construct(notes, wx::ID_ANY, "File Path:", wx::point_construct(35,39));
	notes.tc = wx::textctrl_construct(notes, -1, "", wx::point_construct(85, 35), wx::size_construct(285, 20));
	openID = currProject.connManageId.getID();
	openButton = wx::button_construct(notes, openID, "...", wx::point_construct(372,37), wx::size_construct(20, 16));
	notes.connect(openID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onOpen);
	
	noteText = wx::statictext_construct(notes, wx::ID_ANY,"Note:", wx::point_construct(35,70));
	noteCtrl = wx::textctrl_construct(notes, wx::ID_ANY, "", wx::point_construct(85,70), wx::size_construct(300,50),wx::flags(wx::TE_MULTILINE,wx::TE_RICH));
	//noteTypeText = statictext_construct(notes, wx::ID_ANY,"Type:",point_construct(35,130));
	//noteType = combobox_construct(notes, 33, "noteType", point_construct(85,130), size_construct(50,20), ["Text","Link"], flags(wx::CB_READONLY));
	//noteType.setvalue("Text");
	rb1 = wx::radiobutton_construct(notes, wx::ID_ANY, "Text", wx::point_construct(285, 125));
	rb1.setvalue(true);
	rb2 = wx::radiobutton_construct(notes, wx::ID_ANY, "Link", wx::point_construct(330, 125));

	viewID = currProject.connManageId.getID();
	viewButton = wx::button_construct(notes, viewID, "View", wx::point_construct(190,155), wx::size_construct(60, 25));
	saveID = currProject.connManageId.getID();
	saveButton = wx::button_construct(notes, saveID, "Save", wx::point_construct(255,155), wx::size_construct(60, 25));
	deleteID = currProject.connManageId.getID();
	deleteButton = wx::button_construct(notes, deleteID, "Delete", wx::point_construct(320,155), wx::size_construct(60, 25));

//	st = wx::radiobox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	st = wx::staticbox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
//	//listbox = listbox_construct(st, 36, point_construct(9,15), size_construct(340,155), ["choice1","choice2"]);//, flags(wx::LB_SINGLE,wx::LC_LIST));
	listctrl = wx::listctrl_construct(st, wx::ID_ANY, wx::point_construct(9,15), wx::size_construct(340,155), wx::LC_LIST);
	k = -1;
	items = filter.notes;
	foreach (item, items)
		listctrl.insertitem(++k, item);
	return [openID, viewID, saveID, deleteID];
}

function Filter (filter, currProject, frame) {
	dialog = wx::dialog_construct(nil, wx::ID_ANY, "Filter", wx::point_construct(0,0), wx::size_construct(450, 480));
	dialog.centre();
	dialog.show(true);
	panel = wx::panel_construct(dialog, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(450, 480));
	notebook = wx::notebook_construct(panel, wx::ID_ANY,wx::point_construct(22,15),wx::size_construct(400,390));
	notebook.setbackgroundcolour(colour = wx::colour_construct(240,240,240));
	propsWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	propsWin.show(true);
	notesWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	notesWin.show(true);
	::FilterProps(filter, propsWin, currProject);
	notesIds = ::FilterNotes(filter, notesWin, currProject);
	notebook.setimagelist(ImageHolder::images().imageListNB2);
	notebook.addpage(propsWin, "Properties", true, ImageHolder::images().imageListNB2.properties);
	notebook.addpage(notesWin, "Notes", false, ImageHolder::images().imageListNB2.notes);
	local okID = currProject.connManageId.getID();
	okButton = wx::button_construct(panel, okID, "Ok", wx::point_construct(350,415), wx::size_construct(70, 25));
	local cancelID = currProject.connManageId.getID();
	cancelButton   = wx::button_construct(panel, cancelID, "Cancel", wx::point_construct(275,415), wx::size_construct(70, 25));
	dialog.frame   = frame;
	dialog.filter  = filter;
	dialog.props   = propsWin;
	dialog.currProject = currProject;
	dialog.connect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onFilterOk);
	dialog.connect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.connect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.dialog = dialog;
	panel.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.dialog = dialog;
	notebook.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.dialog = dialog;
	propsWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.dialog = dialog;
	notesWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.showmodal();
	dialog.disconnect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onFilterOk);
	dialog.disconnect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.disconnect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.destroy();
	currProject.connManageId.removeIDs([okID, cancelID]);
	currProject.connManageId.removeIDs(notesIds);
	notesIds = nil;
}

function onFilter (frame, ev) {
	wxparent	= frame.frame;
	filter		= frame.currFilter;
	currProject = frame.currProject;
	::Filter(filter, currProject, wxparent);
}


function PatternProps(pattern, propsWin, currProject) {
	shortDescText = wx::statictext_construct(propsWin,wx::ID_ANY,"Short\nDescription:",wx::point_construct(15,20));
	shortDescCtrl = wx::textctrl_construct(propsWin,wx::ID_ANY, pattern.name,wx::point_construct(95,18),wx::size_construct(260,30),wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	propsWin.name = shortDescCtrl;
	authorText = wx::statictext_construct(propsWin,wx::ID_ANY, "Author:",wx::point_construct(15,65));
	authorCombo = wx::combobox_construct(propsWin, wx::ID_ANY, pattern.author, wx::point_construct(60,61), wx::size_construct(130,20), [], wx::flags(wx::CB_DROPDOWN));
	propsWin.author = authorCombo;
	// default type value cases
	authors = currProject.suggestAuthors;
	foreach (author, authors)
		authorCombo.append(author);
	datetime = pattern.lastUpdate;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	lastUpdateText = wx::statictext_construct(propsWin, wx::ID_ANY,"Last Update:",wx::point_construct(15,100));
	lastUdate = wx::textctrl_construct(propsWin, wx::ID_ANY, datetimeStr, wx::point_construct(80,96), wx::size_construct(80,20), wx::flags(wx::TE_READONLY));
	datetime = pattern.created;
	datetimeStr = datetime.getday()+"/"+datetime.getmonth()+"/"+datetime.getyear();
	createdText = wx::statictext_construct(propsWin,wx::ID_ANY,"Created:",wx::point_construct(180,100));
	createdCtrl = wx::textctrl_construct(propsWin,wx::ID_ANY,datetimeStr,wx::point_construct(230,96),wx::size_construct(80,20), wx::flags(wx::TE_READONLY));
//	st = wx::radiobox_construct(propsWin, wx::ID_ANY, "Description", wx::point_construct(15, 130), wx::size_construct(360, 200));
	st = wx::staticbox_construct(propsWin, wx::ID_ANY, "Description", wx::point_construct(15, 130), wx::size_construct(360, 200));
	textctrl = wx::textctrl_construct(st,wx::ID_ANY, pattern.description, wx::point_construct(15,20),wx::size_construct(330,170),wx::flags(wx::TE_RICH2,wx::TE_MULTILINE));
	propsWin.description = textctrl;
}

function PatternNotes(pattern, notes, currProject) {
	filePath = wx::statictext_construct(notes, wx::ID_ANY, "File Path:", wx::point_construct(35,39));
	notes.tc = wx::textctrl_construct(notes, -1, "", wx::point_construct(85, 35), wx::size_construct(285, 20));
	openID = currProject.connManageId.getID();
	openButton = wx::button_construct(notes, openID, "...", wx::point_construct(372,37), wx::size_construct(20, 16));
	notes.connect(openID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onOpen);
	
	noteText = wx::statictext_construct(notes, wx::ID_ANY,"Note:", wx::point_construct(35,70));
	noteCtrl = wx::textctrl_construct(notes, wx::ID_ANY, "", wx::point_construct(85,70), wx::size_construct(300,50),wx::flags(wx::TE_MULTILINE,wx::TE_RICH));
	//noteTypeText = statictext_construct(notes, wx::ID_ANY,"Type:",point_construct(35,130));
	//noteType = combobox_construct(notes, 33, "noteType", point_construct(85,130), size_construct(50,20), ["Text","Link"], flags(wx::CB_READONLY));
	//noteType.setvalue("Text");
	rb1 = wx::radiobutton_construct(notes, wx::ID_ANY, "Text", wx::point_construct(285, 125));
	rb1.setvalue(true);
	rb2 = wx::radiobutton_construct(notes, wx::ID_ANY, "Link", wx::point_construct(330, 125));

	viewID = currProject.connManageId.getID();
	viewButton = wx::button_construct(notes, viewID, "View", wx::point_construct(190,155), wx::size_construct(60, 25));
	saveID = currProject.connManageId.getID();
	saveButton = wx::button_construct(notes, saveID, "Save", wx::point_construct(255,155), wx::size_construct(60, 25));
	deleteID = currProject.connManageId.getID();
	deleteButton = wx::button_construct(notes, deleteID, "Delete", wx::point_construct(320,155), wx::size_construct(60, 25));

//	st = wx::radiobox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
	st = wx::staticbox_construct(notes, -1, "", wx::point_construct(35, 180), wx::size_construct(358, 180));
//	//listbox = listbox_construct(st, 36, point_construct(9,15), size_construct(340,155), ["choice1","choice2"]);//, flags(wx::LB_SINGLE,wx::LC_LIST));
	listctrl = wx::listctrl_construct(st, wx::ID_ANY, wx::point_construct(9,15), wx::size_construct(340,155), wx::LC_LIST);
	k = -1;
	items = pattern.notes;
	foreach (item, items)
		listctrl.insertitem(++k, item);
	return [openID, viewID, saveID, deleteID];
}

function Pattern (pattern, currProject, frame) {
	dialog = wx::dialog_construct(nil, wx::ID_ANY, "Pattern", wx::point_construct(0,0), wx::size_construct(450, 480));
	dialog.centre();
	dialog.show(true);
	panel = wx::panel_construct(dialog, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(450, 480));
	notebook = wx::notebook_construct(panel, wx::ID_ANY,wx::point_construct(22,15),wx::size_construct(400,390));
	notebook.setbackgroundcolour(colour = wx::colour_construct(240,240,240));
	propsWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	propsWin.show(true);
	notesWin = wx::window_construct(notebook, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	notesWin.show(true);
	::PatternProps(pattern, propsWin, currProject);
	notesIds = ::PatternNotes(pattern, notesWin, currProject);
	notebook.setimagelist(ImageHolder::images().imageListNB2);
	notebook.addpage(propsWin, "Properties", true, ImageHolder::images().imageListNB2.properties);
	notebook.addpage(notesWin, "Notes", false, ImageHolder::images().imageListNB2.notes);
	local okID = currProject.connManageId.getID();
	okButton = wx::button_construct(panel, okID, "Ok", wx::point_construct(350,415), wx::size_construct(70, 25));
	local cancelID = currProject.connManageId.getID();
	cancelButton = wx::button_construct(panel, cancelID, "Cancel", wx::point_construct(275,415), wx::size_construct(70, 25));
	dialog.frame   = frame;
	dialog.pattern = pattern;
	dialog.props   = propsWin;
	dialog.currProject = currProject;
	dialog.connect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onPatternOk);
	dialog.connect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.connect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.dialog = dialog;
	panel.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.dialog = dialog;
	notebook.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.dialog = dialog;
	propsWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.dialog = dialog;
	notesWin.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.showmodal();
	dialog.disconnect(okID, wx::EVT_COMMAND_BUTTON_CLICKED, EditDialogs::onPatternOk);
	dialog.disconnect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.disconnect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notebook.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	propsWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	notesWin.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.destroy();
	currProject.connManageId.removeIDs([okID, cancelID]);
	currProject.connManageId.removeIDs(notesIds);
	notesIds = nil;
}

function onPattern (frame, ev) {
	wxparent	= frame.frame;
	pattern		= frame.currPattern;
	currProject = frame.currProject;
	::Pattern(pattern, currProject, wxparent);
}

using #Utilities;
using #ShapeRAD;
using #TreeView;
using #AddrFuncs;

/**
 *  Description: Checks for double name in same type
 *
 **/
function isDuplicate (name, type, project, isChangedName) {
	counter = 0;
	// if found it one time and it is changed before set means duplicate
	if (isChangedName)
		counter=1;
	foreach (tmpreq, project.currentPattern.constituents[type])
		if (tmpreq.shortDesc==name)
			++counter;
	return counter>=2;
}


function onRequirementOk (frame, ev) {
	currProject		= frame.currProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	req         	= frame.req;
	props       	= frame.props;
	shortDesc   	= props.shortDesc.getvalue();
	status      	= props.status.getvalue();
	priority    	= props.priority.getvalue();
	type        	= props.type.getvalue();
	difficulty  	= props.difficulty.getvalue();
	author      	= props.author.getvalue();
	lastUpdate  	= props.lastUpdate.getvalue();
	created	    	= props.created.getvalue();
	description 	= props.description.getvalue();
	//
	if (not (shortDesc!="" and Utilities::strisnotspace(shortDesc) and ::isDuplicate(shortDesc, "Requirement", currProject, shortDesc != req.shortDesc))) {
		req.setRequirement (shortDesc, description, type, status, priority, difficulty, author);
		currProject.addSuggestAuthor(author);
		scrolledwindow.refresh();
		frame.destroy();
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "There is requirement with the same or invalid name", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
	}
}


function onOperationOk (frame, ev) {
	currProject		= frame.currProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	operation   	= frame.operation;
	props       	= frame.props;
	shortDesc   	= props.shortDesc.getvalue();
	status      	= props.status.getvalue();
	priority    	= props.priority.getvalue();
	difficulty  	= props.difficulty.getvalue();
	author      	= props.author.getvalue();
	lastUpdate  	= props.lastUpdate.getvalue();
	created	    	= props.created.getvalue();
	description 	= props.description.getvalue();
	//
	if (not (shortDesc!="" and Utilities::strisnotspace(shortDesc) and ::isDuplicate(shortDesc, "Operation", currProject, shortDesc != operation.shortDesc))) {
		operation.setElement (shortDesc, description, status, priority, difficulty, author);
		currProject.addSuggestAuthor(author);
		scrolledwindow.refresh();
		frame.destroy();
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "There is operation with the same or invalid name", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
	}
}


function onRoleOk (frame, ev) {
	currProject		= frame.currProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	role        	= frame.role;
	props       	= frame.props;
	shortDesc   	= props.shortDesc.getvalue();
	status      	= props.status.getvalue();
	priority    	= props.priority.getvalue();
	difficulty  	= props.difficulty.getvalue();
	author      	= props.author.getvalue();
	lastUpdate  	= props.lastUpdate.getvalue();
	created	    	= props.created.getvalue();
	description 	= props.description.getvalue();
	//
	if (not (shortDesc!="" and Utilities::strisnotspace(shortDesc) and ::isDuplicate(shortDesc, "Role", currProject, shortDesc != role.shortDesc))) {
		role.setElement (shortDesc, description, status, priority, difficulty, author);
		wxparent = frame.frame;
		TreeView::changeChildPattern(role,wxparent);
		currProject.addSuggestAuthor(author);
		scrolledwindow.refresh();
		frame.destroy();
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "There is role with the same or invalid name", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
	}
}


function onIoOk (frame, ev) {
	currProject		= frame.currProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	io          	= frame.io;
	props       	= frame.props;
	shortDesc   	= props.shortDesc.getvalue();
	lastUpdate  	= props.lastUpdate.getvalue();
	created	    	= props.created.getvalue();
	description 	= props.description.getvalue();
	//
	if (not (shortDesc!="" and Utilities::strisnotspace(shortDesc) and ::isDuplicate(shortDesc, io.class, currProject, shortDesc != io.shortDesc))) {
		io.setSElement (shortDesc, description);
		scrolledwindow.refresh();
		frame.destroy();
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "There is "+io.class+" with the same or invalid name", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
	}
}


function isDuplicatePF (type,name,project,isChangedName) {
	counter = 0;
	// if found it one time and it is changed before set means duplicate
	if (isChangedName)
		counter=1;
	foreach (pat, project[type])
		if (pat.name==name)
			++counter;
	return counter>1;
}


function onFilterOk (frame, ev) {
	currProject	= frame.currProject;
	filter      = frame.filter;
	props       = frame.props;
	wxparent   	= frame.frame;
	name        = props.name.getvalue();
	author      = props.author.getvalue();
	description = props.description.getvalue();
	//
	duplicateName = isDuplicatePF (#filters,name,currProject, filter.name != name);
	if (name!="" and Utilities::strisnotspace(name) and (not duplicateName)) {
		filter.name		= name;
		filter.author	= author;
		currProject.addSuggestAuthor(author);
		filter.description	= description;
		filter.lastUpdate	= wx::datetime_now();
		//
		TreeView::changeChildProj(currProject, filter, wxparent);
		frame.destroy();
	}
	else {
		mesgdlg = wx::messagedialog_construct(nil, "There is filter loaded with the same name", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
	}
}

function onPatternOk (frame, ev) {
	currProject 	= frame.currProject;
	pattern     	= frame.pattern;
	scrolledwindow	= pattern.window;
	props       	= frame.props;
	wxparent    	= frame.frame;
	name        	= props.name.getvalue();
	author      	= props.author.getvalue();
	description 	= props.description.getvalue();
	//
	duplicateName = isDuplicatePF (#patterns,name,currProject, pattern.name != name);
	if (name!="" and Utilities::strisnotspace(name) and (not duplicateName)) {
		if (pattern.name!=name) {
			pattern.renamePattern (wxparent, name);
		}
		pattern.author = author;
		currProject.addSuggestAuthor(author);
		pattern.description = description;
		pattern.lastUpdate  = wx::datetime_now();
		//
		TreeView::changeChildProj(currProject, pattern, wxparent);
		if (pattern.window!=nil) {
			notebook = wxparent.notebook;
			for(i=0;i<notebook.getpagecount();++i) {
				currPage = notebook.getpage(i);
				if (currPage.getid()==pattern.window.getid()) {
					notebook.setpagetext(i,pattern.name);
					break; }
			}
		}
		ShapeRAD::drawPatterns(currProject);
		frame.destroy();
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "There is pattern file with the same name", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
	}
}


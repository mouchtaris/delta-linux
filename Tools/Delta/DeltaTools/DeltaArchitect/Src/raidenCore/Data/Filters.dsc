using #ProjectRAD;
using #TreeView;
using #Dialogs;


/**
 *  Description: Compile filter and keep .dbc file in project or keep only source
 *  Arguments:
 *  	frame, object with pinned data (filterpath='src destination')
 *  Returns: true if it is compiled ok, else the errors
 **/
function onLoadFilter (frame, ev) {
	errr = "";
	currProject = frame.currProject;
	// cases of call event wxmenu and maybe abbreviation button
	frame = (std::isnil(frame.frame) ? frame : frame.frame);
	if (currProject==nil)
		return;
	//
	filedlg = wx::filedialog_construct(frame, "Open Delta script filters", "", "",
	"Delta script filters (*.dsc)|*.dsc", wx::flags(wx::FD_OPEN));
	if (filedlg.showmodal() == wx::flags(wx::ID_OK)) {
		fileName = filedlg.getpath();
		tmp = std::strtokenise(fileName,"\\");
		tmp = std::strtokenise(tmp.pop_back(),".");
		//
		scriptName = tmp.front();
		// Pws compile an den ginei include to struct data tou pattern, Thewroume oti tha to pairnei san argument h function pou 8a exei to filter ?
		// make folder if there is not exists
		std::fileexecute("mkdir "+frame.filePath+"\\filters");
		//
		dbc = frame.filePath+"\\filters\\"+scriptName;
		//
		successOnCompile = std::vmcomp(fileName, dbc+".dbc", (function (s){errr += s;}), false);
		if (successOnCompile) {
			filter = currProject.addFilter (scriptName, "", "", fileName, dbc);
			TreeView::addChildProj(currProject,filter,frame);
			Dialogs::Filter(filter,currProject,frame);
		}
		// on not compiled successfully
		else {
			mesgdlg =wx::messagedialog_construct(nil, "Filter canot be loaded:\n\n"+errr, "Not allowed",
			wx::flags(wx::ICON_ERROR, wx::OK));
			mesgdlg.showmodal();
			return;
		}
	}
}

/**
 *  Description: remove filter from project NOT from folder of project (maybe ask question for completely remove, case for load dbc file)
 *
 **/
function onRemoveFilter (frame, ev) {
	wxparent = frame.frame;
	currProject = frame.currProject;
	filter = frame.currFilter;
	name = filter.name;
	TreeView::removeChildProj(currProject, filter, wxparent);
	//
	std::fileexecute("del "+filter.dbc+".dbc");
	std::fileexecute("del "+filter.dbc+".txt");
	//
	currProject.removeFilter(filter.id);
	//
	mesgdlg =wx::messagedialog_construct(nil, "Filter: "+name+" deleted successfully!", "Filter deletion:",
	wx::flags(wx::ICON_INFORMATION, wx::OK));
	mesgdlg.showmodal();
	return;
}

/**
 *  Description: recompile the filter file, if not successfully compilation
 *               tells user that update not successfull
 **/
function onUpdateFilter (frame, ev) {
	errr = "";
	currProject = frame.currProject;
	if (currProject==nil)
		return;
	//
	filter = frame.currFilter;
	//
	path = "";
	tmp = std::strtokenise(filter.dbc,"\\");
	tmp.pop_back();
	foreach (st, tmp)
		path += st + "\\";
	//
	std::fileexecute("mkdir "+path);
	//
	successOnCompile = std::vmcomp(filter.path, filter.dbc+".dbc", (function (s){errr += s;}), false);
	if (not successOnCompile) {
		mesgdlg =wx::messagedialog_construct(nil, "Filter canot be updated:\n\n"+errr, "Not allowed",
		wx::flags(wx::ICON_ERROR, wx::OK));
		mesgdlg.showmodal();
		return;
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "Filter uploaded successfully!", "Filter upload:",
		wx::flags(wx::ICON_INFORMATION, wx::OK));
		mesgdlg.showmodal();
		return;
	}
}

function consoleFilterAnswers (answer, filtername) {
	console = wx::dialog_construct(nil, wx::ID_ANY, "Answers Filter of "+filtername+":", wx::point_construct(0,0), wx::size_construct(600, 300));
	console.centre();
	console.show(true);
	panel = wx::panel_construct(console, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(600, 300));
	panel.show(true);
	anstxt = wx::textctrl_construct(panel, wx::ID_ANY, answer, wx::point_construct(0,0),wx::size_construct(595,275),wx::flags(wx::TE_RICH2,wx::TE_MULTILINE,wx::TE_READONLY));
	anstxt.setbackgroundcolour(colour = wx::colour_construct(0,0,0));
	colour.destruct();
	attr = wx::textattr_construct(colour = wx::colour_construct(255,255,255));
	colour.destruct();
	anstxt.setstyle(0, anstxt.getlastposition(), attr);
	// skaei mallon sta 2 showmodals otan anoigei kai den kleinei h vm na to pw!!!!
	console.show/*modal*/(true);
}

function onAnswersFilter (frame, ev) {
	// decode data
	currProject  = frame.currProject;
	filter	     = frame.currFilter;
	checkboxList = frame.checkboxList;
	patterns     = std::list_new();
	foreach (box, checkboxList) {
		if (box.ischecked()) {
			boxid = box.getid();
			foreach (pat, currProject.patterns)
				if (pat.id==boxid) {
					// a copy of pattern data (constituents)
					patternData = std::tabcopy(pat.constituents);
					patterns.push_back(patternData);
					break;
				}
		}
	}
	//
	if (patterns.total()==0) {
		mesgdlg =wx::messagedialog_construct(nil, "No pattern chosen for run!", "Filter answer:",
		wx::flags(wx::ICON_INFORMATION, wx::OK));
		mesgdlg.showmodal();
		return;
	}
	frame.destroy();
	frame.endmodal("nope");
	// vm load filter path.dbc
	if (std::isundefined(static filterFuncs)) {
		std::libs::registercopied(filter.name,filter.dbc+".dbc");
		filterLib = std::libs::import(filter.name);
		filterFuncs = filterLib.funcs();
	}
	// run filter
	try {
		answer = "";
		foreach (pattern, patterns)
			if (filterFuncs[#Filter]!=nil) {
				answer += ">> Pattern with name "+pattern.name+":\n";
				answer += filterFuncs[#Filter] (pattern) + "\n\n";
			}
			else {
				mesgdlg = wx::messagedialog_construct( nil, "No function named Filter in this filter!", "Not running",
							wx::flags(wx::ICON_WARNING, wx::OK));
				mesgdlg.showmodal();
				return;
			}
		::consoleFilterAnswers(answer, filter.name);
	}
	// in case filter exception
	trap errr {
		mesgdlg = wx::messagedialog_construct( nil, "Run-time error:\n"+errr, "Not allowed",
					wx::flags(wx::ICON_ERROR, wx::OK));
		mesgdlg.showmodal();
		return;
	}
	return;
}

function onCancel (frame, ev) { frame.destroy(); }

/**
 *  Description: 
 *
 **/
function onRunFilter (frame, ev) {
	currProject = frame.currProject;
	filter	    = frame.currFilter;
	dialogID = currProject.connManageId.getID();
	dialog = wx::dialog_construct(nil, dialogID, "Run the filter "+filter.name+":", wx::point_construct(0,0), wx::size_construct(450, 480));
	dialog.centre();
	dialog.show(true);
	panel  = wx::panel_construct(dialog, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(450, 480));
	window = wx::window_construct(panel, wx::ID_ANY, wx::point_construct(), wx::size_construct(400,390));
	window.show(true);
	chf = wx::statictext_construct(window, wx::ID_ANY, "Choose the pattern - patterns:", wx::point_construct(35,39));
	pos = 55;
	checkboxList = std::list_new();
	foreach (p, currProject.patterns) {
		checkbox = wx::checkbox_construct(window, p.id, p.name, wx::point_construct(45,pos));
		checkbox.setvalue(true);
		checkboxList.push_back(checkbox);
		pos += 15;
	}
	local runID = currProject.connManageId.getID();
	okButton = wx::button_construct(panel, runID, "Run", wx::point_construct(350,415), wx::size_construct(70, 25));
	local cancelID = currProject.connManageId.getID();
	cancelButton = wx::button_construct(panel, cancelID, "Cancel", wx::point_construct(275,415), wx::size_construct(70, 25));
	dialog.currFilter   = frame.currFilter;
	dialog.currProject  = frame.currProject;
	dialog.checkboxList = checkboxList;
	//
	dialog.connect(runID   , wx::EVT_COMMAND_BUTTON_CLICKED, ::onAnswersFilter);
	dialog.connect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.showmodal();
	//
	dialog.disconnect(runID   , wx::EVT_COMMAND_BUTTON_CLICKED, ::onAnswersFilter);
	dialog.disconnect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancel);
	dialog.destruct();
	currProject.connManageId.removeIDs([dialogID, runID, cancelID]);
}


/**
 *  Description: view source code of filter
 *
 *  ?? Sub editor of sparrow gia na to anoigoume ??
 *
 **/
function onViewFilter (frame, ev) {
	// prepei o xrhsths na mporei na anoiksei kai na alla3ei to src tou, kai meta na ginetai 3ana load ???????????????????? 
}


//
//
// and if not find the dbc
// file program will delete
//
//

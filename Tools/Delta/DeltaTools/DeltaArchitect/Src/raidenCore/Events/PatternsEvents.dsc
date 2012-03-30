using #ShapeRAD;
using #ImageHolder;
using #idManagement;
using #Scrollbars;
using #AddrFuncs;


/**
 *  Description: user choose to delete the pattern which was in its action area(pattern window)
 *  Arguments:
 *  	frame, wxmenu pined with data(window=parentframe)
 *  	ev   , event command menu selected
 *  Returns: nope
 **/
function onDeletePatternOpened (frame, ev) {
	pframe = frame.window;
	//
	currProject = pframe.currentProject;
	//
	currPattern = currProject.currentPattern;
	// find a point of pattern and put it as currPoint
	rect = currPattern.rect;
	currPoint = wx::point_construct(rect.getx()+1,rect.gety()+1);
	// create obj argument
	object = [];
	object.currProject = currProject;
	object.currPoint   = currPoint;
	object.frame       = frame.wxparent;
	object.currPattern = currPattern;
	ev = [];
	AddrFuncs::ProjectEvent(#onDeletePattern) (object, ev);
}

function onClosePattern (frame, ev) {
	currProject = frame.currProject;
	pattern = currProject.currentPattern;
	i=0;
	notebook = frame.wxparent.notebook;
	while (true) {
		if (std::isnil((currPage = notebook.getpage(i)))) {
			break; }
		else
		if (currPage.getid()==pattern.window.getid()) {
			notebook.setselection(i);
			notebook.removepage(i);
			pattern.window.show(false);
			pattern.window.disconnect(wx::EVT_ENTER_WINDOW, AddrFuncs::ConstituentsMouseEvents(#onEnterWindow));
			pattern.window.disconnect(wx::EVT_LEFT_DOWN,	AddrFuncs::ConstituentsMouseEvents(#onLeftDown));
			pattern.window.disconnect(wx::EVT_LEFT_UP,		AddrFuncs::ConstituentsMouseEvents(#onLeftUp));
			pattern.window.disconnect(wx::EVT_RIGHT_DOWN,	AddrFuncs::ConstituentsMouseEvents(#onRightDown));
			pattern.window.disconnect(wx::EVT_MOTION,		AddrFuncs::ConstituentsMouseEvents(#onMotionEvent));
			//
			pattern.window.disconnect(wx::EVT_SCROLLWIN_PAGEUP,			Scrollbars::onScrollPageUp);
			pattern.window.disconnect(wx::EVT_SCROLLWIN_PAGEDOWN,		Scrollbars::onScrollPageDown);
			pattern.window.disconnect(wx::EVT_SCROLLWIN_LINEUP,			Scrollbars::onScrollLineUp);
			pattern.window.disconnect(wx::EVT_SCROLLWIN_LINEDOWN,		Scrollbars::onScrollLineDown);
			pattern.window.disconnect(wx::EVT_SCROLLWIN_THUMBRELEASE,	Scrollbars::onScrollThumbrelease);
			pattern.window.disconnect(wx::EVT_SCROLLWIN_THUMBTRACK,		Scrollbars::onScrollThumbtrack);
			//
			pattern.window.disconnect(wx::EVT_ERASE_BACKGROUND,			ShapeRAD::onEraseBackground);
			pattern.window.disconnect(wx::EVT_PAINT,					ShapeRAD::onPaint);
			//
			pattern.window.destruct();
			pattern.viewPoint = wx::point_construct(0,0);
			pattern.window = nil;
			notebook.setselection(0);
			ShapeRAD::drawPatterns(currProject);
			break; }
		++i;
	}
}

//

/**
 *  Description: Redraw on change notebook the current page of project (the notebook)
 *  Arguments:
 *  	frame, main frame of raiden with all data pined
 *  	ev   , event command notebook page changed
 *  Returns: nope
 *
 **/
function onChangePageNotebook (frame, ev) {
	currProject = frame.currProject;
	prevWin = frame.notebook.getpage(ev.getoldselection());
	currWin = frame.notebook.getpage(ev.getselection());
	if (currWin.getid()==idManagement::Core().MainPageID) {
		ShapeRAD::drawPatterns(currProject);
	}
	else {
		foreach (pattern, currProject.patterns)
			if (pattern.window!=nil and pattern.window.getid()==currWin.getid()) {
				currProject.currentPattern = pattern;
				scrolledwindow = pattern.window;
				scrolledwindow.refresh();
				break;
			}
	}
}

/**
 *  Description: if user press middle this can close the current window pattern
 *  Arguments:
 *  	frame, notebook which has pined mainframe
 *  	ev   , event middle down
 *  Returns: nope
 **/
function onMiddleDownClosePattern (frame, ev) {
	currWindowId = frame.getcurrentpage().getid();
	local hitselection;
	if (frame.point==nil)
		hitselection = frame.hittest(wx::point_construct(ev.getx(),ev.gety()));
	else
		hitselection = frame.hittest(frame.point);
	// if not main page of project
	if (hitselection != 0) {
		frame.setselection(hitselection);
		currProject = frame.frame.currProject;
		currentPageId = frame.getcurrentpage().getid();
		foreach (pattern, currProject.patterns) {
			if (pattern.window!=nil and currentPageId==pattern.window.getid()) {
				frame.removepage(frame.getselection());
				pattern.window.show(false);
				pattern.window.disconnect(wx::EVT_ENTER_WINDOW, AddrFuncs::ConstituentsMouseEvents(#onEnterWindow));
				pattern.window.disconnect(wx::EVT_LEFT_DOWN , AddrFuncs::ConstituentsMouseEvents(#onLeftDown));
				pattern.window.disconnect(wx::EVT_LEFT_UP   , AddrFuncs::ConstituentsMouseEvents(#onLeftUp));
				pattern.window.disconnect(wx::EVT_RIGHT_DOWN, AddrFuncs::ConstituentsMouseEvents(#onRightDown));
				pattern.window.disconnect(wx::EVT_MOTION    , AddrFuncs::ConstituentsMouseEvents(#onMotionEvent));
				pattern.window.destruct();
				pattern.window = nil;
				// go to start page
				if (currWindowId==currentPageId or currWindowId==idManagement::Core().MainPageID) {
					frame.setselection(0);
					ShapeRAD::drawPatterns(currProject);
				}
				// find the previous page pattern and fix the currentPattern of currProject
				else {
					notebook = frame;
					i=0;
					while (true) {
						if (std::isnil((currPage = notebook.getpage(i)))) {	break; }
						else
						if (currPage.getid()==currWindowId) {
							notebook.setselection(i);
							// fix current pattern
							foreach (currPattern, currProject.patterns)
								if (currPattern.window!=nil and currWindowId==currPattern.window.getid()) {
									currProject.currentPattern = currPattern;
									break; }
							//
							scrolledwindow = currPattern.window;
							scrolledwindow.refresh();
							break;
						}
						++i;
					}
				}
				break;
			}
		}
	}
	// main page of project - case to close project
	else {
		// ask for closing project and then if user answer yes
		// save the project and close it returns in tha start page
	}
}

function onCloseAllPatterns (frame, ev) {
	pframe = frame.window;
	currProject = pframe.currProject;
	currPoint = frame.currPoint;
	notebook = pframe.notebook;
	notebook.frame = pframe;
	notebook.point = wx::point_construct(100,10);
	openpatterns = notebook.getpagecount();
	for (i=1;i<=openpatterns;i++)
		::onMiddleDownClosePattern(notebook, ev);
	notebook.point = nil;
	ShapeRAD::drawPatterns(currProject);
}

/**
 *  Description: create a wxmenu with action in start page or pattern page
 *  Arguments:
 *  	frame, notebook which has pined mainframe
 *  	ev   , 
 *  Returns: nope
 **/
function onNotebookRightDown (frame, ev) {
	notebook = frame;
	currProject = frame.frame.currProject;
	hitselection = notebook.hittest(wx::point_construct(ev.getx(),ev.gety()));
	notebook.setselection(hitselection);
	// if hit selection is main page project
	if (hitselection==0) {
		wxmenu = wx::menu_construct();
		// pin date in wxmenu
		wxmenu.fileName = frame.frame.fileName;
		wxmenu.window = frame.frame;
		// ids of connections
		saveId   = currProject.connManageId.getID();
		closeAId = currProject.connManageId.getID();
		closeId  = currProject.connManageId.getID();
		// create menu
		wxmenu.append(saveId, "Save Current Project");
		wxmenu.append(closeId, "Close Current Project");
		wxmenu.append(closeAId, "Close All Patterns");
		// connections of wxmenu
		wxmenu.connect(saveId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::SaveLoad(#onSave));
		//wxmenu.connect(closeId, wx::EVT_COMMAND_MENU_SELECTED, );
		wxmenu.connect(closeAId, wx::EVT_COMMAND_MENU_SELECTED, ::onCloseAllPatterns);
		//
		frame.popupmenu(wxmenu, ev.getposition());
		// disconnect wxmenu
		wxmenu.disconnect(saveId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::SaveLoad(#onSave));
		//wxmenu.connect(closeId, wx::EVT_COMMAND_MENU_SELECTED, );
		wxmenu.disconnect(closeAId, wx::EVT_COMMAND_MENU_SELECTED, ::onCloseAllPatterns);
		ids = [saveId,closeAId,closeId];
		currProject.connManageId.removeIDs(ids);
	}
	else {//TODO
		wxmenu = wx::menu_construct();
		// pin date in wxmenu
		wxmenu.window = frame.frame;
		// ids of connections
		saveId   = currProject.connManageId.getID();
		closeAId = currProject.connManageId.getID();
		closeId  = currProject.connManageId.getID();
		// create menu
		wxmenu.append(saveId, "Save Current Pattern");
		wxmenu.append(closeId, "Close Current Pattern");
		wxmenu.append(closeAId, "Close All Patterns But This");
		// connections of wxmenu
/*		wxmenu.connect(reqId, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateRequirement);
		wxmenu.connect(opId, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOperation);
		wxmenu.connect(roleId, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateRole);
	*/	
		frame.popupmenu(wxmenu, ev.getposition());
	}
}


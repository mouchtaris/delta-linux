using #ShapeRAD;
using #ImageHolder;
using #Dialogs;
using #ConstituentsMouseEvents;
using #TreeView;
using #SaveLoad;
using #Scrollbars;



/**
 *  Description: Resize current tab pattern action area horizontally
 *
 **/
function onResizeHorizantally (frame, ev) {
	currProject = frame.currProject;
	if (std::isnil(currProject))
		return;
	currPattern = currProject.currentPattern;
	if (std::isnil(currPattern) or std::isnil(currPattern.window))
		return;
	currPattern.virtualsize = currPattern.virtualsize + wx::size_construct(500,0);
	currPattern.window.setvirtualsize(currPattern.virtualsize.getwidth(), currPattern.virtualsize.getheight());
}

/**
 *  Description: Resize current tab pattern action area vertically
 *
 **/
function onResizeVertically (frame, ev) {
	currProject = frame.currProject;
	if (std::isnil(currProject))
		return;
	currPattern = currProject.currentPattern;
	if (std::isnil(currPattern) or std::isnil(currPattern.window))
		return;
	currPattern.virtualsize = currPattern.virtualsize + wx::size_construct(0,500);
	currPattern.window.setvirtualsize(currPattern.virtualsize.getwidth(), currPattern.virtualsize.getheight());
}


/**
 * Description: Create new pattern in current project
 * Arguments:
 * 		frame, wxmenu object with pined data (window=frame"window of mainpageproject", currProject, frame=mainframe"pined all data of windows")
 * 		ev   , event command menu selected
 *  Returns: nope
 **/
function onCreatePattern (frame, ev) {
	parentframe = frame.frame;
	currProject = frame.currProject;
	path = parentframe.filePath + "\\patterns";
	currPoint = frame.currPoint;
	local pattern = currProject.addPattern(path, currPoint);
	// draw main project page which has patterns list view
	ShapeRAD::drawPatterns(currProject);
	TreeView::addChildProj(currProject,pattern,parentframe);
	Dialogs::Pattern(pattern, currProject, parentframe);
	SaveLoad::onSavePattern(pattern,ev);
	parentframe.updateNewPattern = true;
	SaveLoad::onSave(parentframe,ev);
	//TODO TreeView set start page as not saved with name of proj and *
}

/**
 *  Description: Load a pattern if it is valid for Raiden
 *  Arguments:
 * 		frame, wxmenu object with pined data (window=frame"window of mainpageproject", currProject, frame=mainframe"pined all data of windows")
 * 		ev   , event command menu selected
 *  Returns: nope
 **/
function onLoadPattern (frame, ev) {
	parentframe = frame.frame;
	currProject = frame.currProject;
	filedlg = wx::filedialog_construct(parentframe, "Open Raiden Pattern files", "", "",
	"Raiden Pattern files (*.patt)|*.patt", wx::flags(wx::FD_OPEN));
	if (filedlg.showmodal() == wx::flags(wx::ID_OK)) {
		path = filedlg.getpath();
		loadpattern = SaveLoad::loadPattern(path, currProject);
		if (loadpattern!=nil) {
			parentframe.updateNewPattern = true;
			SaveLoad::onSave(parentframe,ev);
			TreeView::addChildProj(currProject,loadpattern,parentframe);
			ShapeRAD::drawPatterns(currProject);
		}
	}
}


/**
 *  Description: Open properties dialog of pattern
 *  Arguments:
 * 		frame, wxmenu object with pined data (currPoint, currProject, frame=mainframe"pined all data of windows", currPattern)
 * 		ev   , event command menu selected
 *  Returns: nope
 **/
function onOpenPattern (frame, ev) {
	parentframe = frame.frame;
	currProject = frame.currProject;
	currPoint = frame.currPoint;
	currPattern = frame.currPattern;
	Dialogs::Pattern(currPattern, currProject, parentframe);
}

/**
 *  Description: Delete a pattern from project
 *  Arguments:
 * 		frame, wxmenu object with pined data (currPoint, currProject, frame=mainframe"pined all data of windows", currPattern)
 * 		ev   , event command menu selected
 *  Returns: nope
 **/
function onDeletePattern (frame, ev) {
	currProject = frame.currProject;
	currPoint   = frame.currPoint;
	wxparent    = frame.frame;
	pattern     = frame.currPattern;
	// if there is window opened
	if (pattern.window!=nil) {
		i=0;
		notebook = wxparent.notebook;
		while (true) {
			if (std::isnil((currPage = notebook.getpage(i)))) {
				break; }
			else
			if (currPage.getid()==pattern.window.getid()) {
				notebook.setselection(i);
				notebook.removepage(i);
				pattern.window.show(false);
				pattern.window.disconnect(wx::EVT_ENTER_WINDOW,		ConstituentsMouseEvents::onEnterWindow);
				pattern.window.disconnect(wx::EVT_LEFT_DOWN,		ConstituentsMouseEvents::onLeftDown);
				pattern.window.disconnect(wx::EVT_LEFT_UP,			ConstituentsMouseEvents::onLeftUp);
				pattern.window.disconnect(wx::EVT_RIGHT_DOWN,		ConstituentsMouseEvents::onRightDown);
				pattern.window.disconnect(wx::EVT_MOTION,			ConstituentsMouseEvents::onMotionEvent);
				
				pattern.window.disconnect(wx::EVT_SCROLLWIN_PAGEUP, 		Scrollbars::onScrollPageUp);
				pattern.window.disconnect(wx::EVT_SCROLLWIN_PAGEDOWN,		Scrollbars::onScrollPageDown);
				pattern.window.disconnect(wx::EVT_SCROLLWIN_LINEUP,			Scrollbars::onScrollLineUp);
				pattern.window.disconnect(wx::EVT_SCROLLWIN_LINEDOWN,		Scrollbars::onScrollLineDown);
				pattern.window.disconnect(wx::EVT_SCROLLWIN_THUMBRELEASE,	Scrollbars::onScrollThumbrelease);
				pattern.window.disconnect(wx::EVT_SCROLLWIN_THUMBTRACK,		Scrollbars::onScrollThumbtrack);
				//
				pattern.window.disconnect(wx::EVT_ERASE_BACKGROUND, ShapeRAD::onEraseBackground);
				pattern.window.disconnect(wx::EVT_PAINT,			ShapeRAD::onPaint);
				
				pattern.window.destruct();
				notebook.setselection(0);
				break; }
			++i;
		}
	}
	TreeView::removeChildProj(currProject, pattern, wxparent);
	currProject.removePattern(pattern.id, wxparent);
	//
	std::fileexecute("del "+pattern.path+"\\"+pattern.name+".patt");
	pattern.constituents.destruct();
	pattern.constituents = nil;
	pattern.created.destruct();
	pattern.lastUpdate.destruct();
	pattern.rect.destruct();
	pattern.notes.clear();
	pattern.notes = nil;
	ShapeRAD::drawPatterns(currProject);
	pattern = nil;
}

/**
 *  Description: Opens (if it is not) the current pattern that is pined in frame
 *  Arguments: 
 *  	frame, [frame:wxparent, currProject, currPattern]
 *  	ev,    unused
 *  Returns: nope
 **/
function onOpenTabPattern (frame, ev) {
	// decode pin data
	wxparent = frame.frame;
	currentProject = frame.currProject;
	pattern = frame.currPattern;
	// if pattern window is not already opened
	if (std::isnil(pattern.window)) {
		winID = currentProject.connManageId.getID();
		pattern.window = wx::scrolledwindow_construct(wxparent.notebook, winID, wx::point_construct(0,0), wx::size_construct(800,800));
		pattern.window.setvirtualsize(2000,2000);
		pattern.virtualsize = wx::size_construct(2000,2000);
//		pattern.window.setscrollbars(20, 20, 100, 100);
		// number of pixels per scroll
		pattern.window.setscrollrate(10, 10);
		pattern.window.scale = 1;
		//
		pattern.window.setbackgroundcolour(colour = wx::colour_construct(212,220,237));
		pattern.window.show(true);
		pattern.window.dc = wx::windowdc_construct(pattern.window);
		local pen = wx::pen_construct(colour);
		pattern.window.dc.setpen(pen);
		pen.destruct();
		local brush = wx::brush_construct(colour);
		pattern.window.dc.setbrush(brush);
		brush.destruct();
		colour.destruct();
		pattern.window.dc.drawrectangle(wx::point_construct(0,0), pattern.window.getsize());
		wxparent.notebook.addpage(pattern.window, pattern.name, true, ImageHolder::images().imageListNB.Pattern);
		currentProject.currentPattern = pattern;
		pattern.window.currentProject = currentProject;
		pattern.window.wxparent = wxparent;
		pattern.window.connect(wx::EVT_ENTER_WINDOW,	ConstituentsMouseEvents::onEnterWindow);
		pattern.window.connect(wx::EVT_LEFT_DCLICK,		ConstituentsMouseEvents::onDClickLeft);
		pattern.window.connect(wx::EVT_LEFT_DOWN,		ConstituentsMouseEvents::onLeftDown);
		pattern.window.connect(wx::EVT_LEFT_UP,			ConstituentsMouseEvents::onLeftUp);
		pattern.window.connect(wx::EVT_RIGHT_DOWN,		ConstituentsMouseEvents::onRightDown);
		pattern.window.connect(wx::EVT_MOTION,			ConstituentsMouseEvents::onMotionEvent);
		// scrollbars of scrolled window
		pattern.window.connect(wx::EVT_SCROLLWIN_PAGEUP, 		Scrollbars::onScrollPageUp);
		pattern.window.connect(wx::EVT_SCROLLWIN_PAGEDOWN,		Scrollbars::onScrollPageDown);
		pattern.window.connect(wx::EVT_SCROLLWIN_LINEUP, 		Scrollbars::onScrollLineUp);
		pattern.window.connect(wx::EVT_SCROLLWIN_LINEDOWN,		Scrollbars::onScrollLineDown);
		pattern.window.connect(wx::EVT_SCROLLWIN_THUMBRELEASE,	Scrollbars::onScrollThumbrelease);
		pattern.window.connect(wx::EVT_SCROLLWIN_THUMBTRACK,	Scrollbars::onScrollThumbtrack);
		//
		pattern.window.connect(wx::EVT_ERASE_BACKGROUND,	ShapeRAD::onEraseBackground);
		pattern.window.connect(wx::EVT_PAINT,				ShapeRAD::onPaint);
		//
		pattern.window.refresh();
	}
	else {
		i=0;
		notebook = wxparent.notebook;
		while (true) {
			if (std::isnil(notebook.getpage(i))) {
				break; }
			else
			if (notebook.getpage(i).getid()==pattern.window.getid()) {
				notebook.setselection(i);
				break; }
			++i;
		}
	}
}

function onExportPatternBMP (frame, ev) {
	wxparent    = frame.frame;
	currPattern = frame.currPattern;
	//
	filedlg = wx::filedialog_construct(wxparent, "Save Raiden Pattern - "+currPattern.name, "", "",
	"BMP files (*.bmp)|*.bmp|JPEG files (*.jpeg)|*.jpeg|PNG files (*.png)|*.png", wx::flags(wx::FD_SAVE));
	if (filedlg.showmodal() == wx::flags(wx::ID_OK)) {
		fileName = filedlg.getpath();
		//
		type = wx::BITMAP_TYPE_BMP;
		if (filedlg.getfilterindex() == 1)
			type = wx::BITMAP_TYPE_JPEG;
		else
		if (filedlg.getfilterindex() == 2)
			type = wx::BITMAP_TYPE_PNG;
		//
		bitmap = ShapeRAD::createBitmapPattern(currPattern);
//		wx::bitmap_savefile(bitmap, fileName, type);
		t = bitmap.converttoimage().savefile(fileName, type);
	}
	
}

/**
 *  Description: Create props of project author etc... under construction
 *
 **/
function onOpenProjectProps (frame, ev) {
}

/**
 *  Description: double click in main page of project, on hit opens pattern dialog
 *  Arguments:
 *  	frame, window of project with pined data = (currProject, frame:mainframe)
 *  	ev, mouse event
 *  Returns: nope
 **/
function onLeftDClick (frame, ev) {
	currPoint   = ev.getposition();
	currProject = frame.currProject;
	if ((currPattern = ShapeRAD::patternHit(currPoint, currProject))!=nil) {
		Dialogs::Pattern(currPattern,currProject,frame.frame);
	}
}


/**
 *  Description: Creates a menu of choices in user at main page
 *  Arguments:
 *  	frame, window of project with pined data = (currProject, frame:mainframe)
 *  	ev, mouse event
 *  Returns: nope
 **/
function onRightUp (frame, ev) {
	currPoint = ev.getposition();
	currProject = frame.currProject;
	wxmenu = wx::menu_construct();
	wxmenu.window = frame;
	wxmenu.currProject = currProject;
	wxmenu.currPoint = currPoint;
	wxmenu.frame = frame.frame;
	// choices if user with right click hitted a pattern
	if ((currPattern = ShapeRAD::patternHit(currPoint, currProject))!=nil) {
		wxmenu.currPattern = currPattern;
		openID = currProject.connManageId.getID();
		wxmenu.append(openID, "Open");
		wxmenu.connect(openID, wx::EVT_COMMAND_MENU_SELECTED, ::onOpenTabPattern);
		deleteID = currProject.connManageId.getID();
		wxmenu.append(deleteID, "Delete");
		wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ::onDeletePattern);
		propertiesID = currProject.connManageId.getID();
		wxmenu.append(propertiesID, "Properties");
		wxmenu.connect(propertiesID, wx::EVT_COMMAND_MENU_SELECTED, ::onOpenPattern);
		frame.popupmenu(wxmenu, currPoint);
		wxmenu.disconnect(openID,   wx::EVT_COMMAND_MENU_SELECTED, ::onOpenTabPattern);
		wxmenu.disconnect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ::onDeletePattern);
		wxmenu.disconnect(propertiesID, wx::EVT_COMMAND_MENU_SELECTED, ::onOpenPattern);
		wxmenu.destruct();
		local ids = [openID, deleteID, propertiesID];
		currProject.connManageId.removeIDs(ids);
		ids = nil;
	}
	// no pattern hitted on main page
	else {
		createID = currProject.connManageId.getID();
		subMenu = wx::menu_construct();
		subMenu.append(createID,"Pattern");
		wxmenu.append(wx::ID_ANY, "Create", subMenu);
		wxmenu.connect(createID, wx::EVT_COMMAND_MENU_SELECTED, ::onCreatePattern);
		propertiesID = currProject.connManageId.getID();
		wxmenu.append(propertiesID, "Properties");
		wxmenu.connect(propertiesID, wx::EVT_COMMAND_MENU_SELECTED, ::onOpenProjectProps);
		frame.popupmenu(wxmenu, currPoint);
		wxmenu.disconnect(createID,   wx::EVT_COMMAND_MENU_SELECTED, ::onCreatePattern);
		wxmenu.disconnect(propertiesID, wx::EVT_COMMAND_MENU_SELECTED, ::onOpenProjectProps);
		wxmenu.destruct();
		// free id mem in IdManagement
		local ids = [createID, propertiesID];
		currProject.connManageId.removeIDs(ids);
		ids = nil;
	}
}

/**
 *  Description: Drag & Drop events in the main page
 **/

// keep the height and width that sub,add from event.point
// to find the currentPoint of shape after the transportation
difSize = [ @w : -1, @h : -1 ];
// pattern which mouse caught
hittedPattern = nil;



function onLeftDownMainPage (frame, ev) {
	currPoint = ev.getposition();
	currProject = frame.currProject;
	//
	pattern = ShapeRAD::patternHit(currPoint, currProject);
	if (pattern != nil) {
		// priority fixing
		patterns = currProject.patterns;
		foreach (pat, patterns)
			if (pat.id==pattern.id) {
				patterns.remove(pat);
				break; }
		patterns.push_front(pattern);
		//
		// keep the dif coordinates that mouse hit the shape
		// in case drag to know the position that shape must be after move
		::difSize.point = wx::point_construct(ev.getx(), ev.gety());
		::difSize.w = ev.getx() - pattern.rect.getx();
		::difSize.h = pattern.rect.gety() - ev.gety();
		::hittedPattern = pattern;
		// fix the mouse view to be in drag mode
		local cursor = wx::cursor_construct(wx::CURSOR_SIZING);
		frame.setcursor(cursor);
		cursor.destruct();
		ShapeRAD::drawPatterns(currProject);
	}
	else {
		::difSize.point = wx::point_construct(-1,-1);
		::difSize.w = -1;
		::difSize.h = -1;
		::hittedPattern = nil;
	}
}

function onLeftUpMainPage (frame, ev) {
	if (::hittedPattern!=nil) {
		::difSize.point = wx::point_construct(-1,-1);
		::difSize.w = -1;
		::difSize.h = -1;
		::hittedPattern = nil;
	}
}

function onMotionEventMainPage (frame, ev) {
	currPoint = ev.getposition();
	currProject = frame.currProject;
	//dragging the mouse
	if (ev.leftisdown()) {
		// mouse caught a pattern previously
		if (::hittedPattern!=nil) {
			assert(::difSize.w!=-1 and difSize.h!=-1);
			local x = ev.getx() - ::difSize.w;
			local y = ev.gety() + ::difSize.h;
			//
			::hittedPattern.setRect(x,y);
			ShapeRAD::drawPatterns(currProject);
		}
		else {
			local cursor = wx::cursor_construct(wx::CURSOR_NONE);
			frame.setcursor(cursor);
			cursor.destruct();
		}
	}
	else {
		if (ShapeRAD::patternHit(currPoint, currProject)!=nil) {
			local cursor = wx::cursor_construct(wx::CURSOR_SIZING);
			frame.setcursor(cursor);
			cursor.destruct();
		}
		else {
			local cursor = wx::cursor_construct(wx::CURSOR_NONE);
			frame.setcursor(cursor);
			cursor.destruct();
		}
	}
}


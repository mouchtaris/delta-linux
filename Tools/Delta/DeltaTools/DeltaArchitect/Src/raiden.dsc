using #ImageHolder;
using #ToolBarRAD;
using #SaveLoad;
using #AddrFuncs;
using #Filters;
using #IdManagement;

dllWX	= std::dllimportdeltalib(wx::DLL);
dllXML	= std::dllimportdeltalib(xml::DLL);


//
app = wx::app_construct();

//
function initializeRAD () {
	wx::initallimagehandlers();

//

	info = SaveLoad::loadInfo();
	frame = wx::frame_construct(nil, wx::ID_ANY, "Raiden", wx::point_construct(0,0), wx::size_construct(info.Dimensions.width, info.Dimensions.height));
	frame.centre();
	iconbitmap = ImageHolder::images().raidenIconBMP;
	mask = wx::mask_construct(iconbitmap, wx::colour_construct("#FF00FF"));
	iconbitmap.setmask(mask);
	icon = ImageHolder::images().raidenIcon;
	icon.copyfrombitmap(iconbitmap);
	frame.seticon(icon);
	// pin info in frame
	frame.info = info;
	
// CREATE MENUS PROGRAM
	fileMenu = wx::menu_construct();
	editMenu = wx::menu_construct();
	viewMenu = wx::menu_construct();
	helpMenu = wx::menu_construct();
	
	fileMenu.append(wx::ID_OPEN, "Open Project\tCtrl+Shift+O", "Open rapid architecture design project. (type file ends \".rproj\")");
	fileMenu.append(wx::ID_NEW, "New Project\tCtrl+Shift+N", "Create new rapid architecture design project");
	fileMenu.append(wx::ID_SAVE, "&Save Project\tCtrl+S", "Save current project");
	fileMenu.append(wx::ID_ADD, "&Load Filter\tCtrl+Shift+F", "Load new filter in project");
	menuitem = wx::menuitem_construct(fileMenu, wx::ID_EXIT, "Exit\tCtrl+Q", "Quit Raiden");
	menuitem.setbitmap(ImageHolder::images().quit);
	fileMenu.append(menuitem);
	
	//
	subView = wx::menu_construct();
	subView.append(0, "priority",	"View architecture by priority", wx::ITEM_RADIO );
	subView.append(1, "status",		"View architecture by status", wx::ITEM_RADIO );
	subView.append(2, "difficulty",	"View architecture by difficulty", wx::ITEM_RADIO );
	subView.append(3, "author",		"View architecture by author", wx::ITEM_RADIO );
	viewMenu.append(wx::ID_ANY, "view", subView);
	viewMenu.append(4, "Background colour", "Change background colour of action area");
	//
	menuitem = wx::menuitem_construct(editMenu, wx::ID_UNDO, "Undo\tCtrl+Z", "Undo user last action");
	menuitem.setbitmap(ImageHolder::images().undo);
	editMenu.append(menuitem);
	menuitem = wx::menuitem_construct(editMenu, wx::ID_REDO, "Redo\tCtrl+Shift+Z", "Redo user last action");
	menuitem.setbitmap(ImageHolder::images().redo);
	editMenu.append(menuitem);
	
	//
	subMenu = wx::menu_construct();
	menuitem = wx::menuitem_construct(subMenu, wx::ID_ZOOM_IN, "Zoom In\tCtrl++", "Zoom in");
	menuitem.setbitmap(ImageHolder::images().zoomin);
	subMenu.append(menuitem);
	menuitem = wx::menuitem_construct(subMenu, wx::ID_ZOOM_OUT , "Zoom Out\tCtrl+-", "Zoom out");
	menuitem.setbitmap(ImageHolder::images().zoomout);
	subMenu.append(menuitem);
	menuitem = wx::menuitem_construct(subMenu, wx::ID_ZOOM_100, "Zoom Fit\tCtrl+0", "Zoom fit");
	menuitem.setbitmap(ImageHolder::images().zoomfit);
	subMenu.append(menuitem);
	editMenu.append(wx::ID_ANY, "Zoom", subMenu);
	//
	subMenu = wx::menu_construct();
	menuitem = wx::menuitem_construct(subMenu, 29, "Resize Horizontally", "Resize Horizontally");
	menuitem.setbitmap(ImageHolder::images().resizehorizontally);
	subMenu.append(menuitem);
	menuitem = wx::menuitem_construct(subMenu, 28, "Resize Vertically", "Resize Vertically");
	menuitem.setbitmap(ImageHolder::images().resizevertically);
	subMenu.append(menuitem);
	editMenu.append(wx::ID_ANY, "Resize", subMenu);
	//
	helpMenu.append(wx::ID_ABOUT, "About", "Description of program");
	menuitem = wx::menuitem_construct(helpMenu, 5, "Filters", "Documentation about filters");
	menuitem.setbitmap(ImageHolder::images().filter);
	helpMenu.append(menuitem);
	menus = [fileMenu, editMenu, viewMenu, helpMenu];
	titles = ["&File", "&Edit", "&View", "&Help"];
	menuBar = wx::menubar_construct(4, menus, titles);
	frame.setmenubar(menuBar);
// CREATE STATUSBAR
	frame.createstatusbar();
// CREATE TOOLBAR
	toolbar = frame.createtoolbar(wx::TB_FLAT);
	toolbar.addseparator();
	toolbar.addtool(wx::ID_EXIT, "Exit"    , ImageHolder::images().quit, "Exit program");
	toolbar.addseparator();
	toolbar.addtool(wx::ID_UNDO, "Undo"    , ImageHolder::images().undo, "Undo");
	toolbar.addtool(wx::ID_REDO, "Redo"    , ImageHolder::images().redo, "Redo");
	toolbar.addseparator();
	toolbar.addtool(wx::ID_NEW , "New"     , ImageHolder::images().newproject , "New project");
	toolbar.addtool(wx::ID_OPEN, "Open"    , ImageHolder::images().openproject, "Open file project");
	toolbar.addseparator();
	toolbar.addtool(wx::ID_SAVE, "Save"    , ImageHolder::images().save   , "Save");
	toolbar.addtool(15, "Save All", ImageHolder::images().saveall, "Save all");
	toolbar.addseparator();
	toolbar.addtool(wx::ID_ZOOM_IN,		"Zoom in",	ImageHolder::images().zoomin,	"Zoom In");
	toolbar.addtool(wx::ID_ZOOM_100, 	"Zoom fit",	ImageHolder::images().zoomfit,	"Zoom Fit");
	toolbar.addtool(wx::ID_ZOOM_OUT, 	"Zoom out",	ImageHolder::images().zoomout,	"Zoom Out");
	toolbar.addseparator();
	toolbar.addtool(29 , "Resize horizontally" , ImageHolder::images().resizehorizontally , "Resize Horizontally");
	toolbar.addtool(28 , "Resize vertically"   , ImageHolder::images().resizevertically , "Resize Vertically");
	toolbar.addseparator();
	//toolbar.addtool(5 , "Filter documentation" , ImageHolder::images().filter, "Filter documentation");
	toolbar.realize();

// CREATE SPLITTER WINDOWS
	mainSpl  = wx::splitterwindow_construct(frame,   -1, wx::point_construct(0,0), wx::size_construct(-1,-1), wx::flags(wx::SP_NO_XP_THEME, wx::SP_3D, wx::SP_LIVE_UPDATE, wx::CLIP_CHILDREN));
	toolSpl1 = wx::splitterwindow_construct(mainSpl, -1, wx::point_construct(0,0), wx::size_construct(-1,-1), wx::flags(wx::SP_NO_XP_THEME, wx::SP_3D, wx::SP_LIVE_UPDATE, wx::CLIP_CHILDREN));
	toolSpl2 = wx::splitterwindow_construct(mainSpl, -1, wx::point_construct(0,0), wx::size_construct(-1,-1), wx::flags(wx::SP_NO_XP_THEME, wx::SP_3D, wx::SP_LIVE_UPDATE, wx::CLIP_CHILDREN));
// CREATE MAIN ACTION AREA
	notebook = wx::notebook_construct(toolSpl1, 12, wx::point_construct(0,0), wx::size_construct(-1,-1), wx::NB_HITTEST_ONICON );
	notebook.setbackgroundcolour(colour = wx::colour_construct("#D0D0D0"));
	// start up window for rad
	startPage = wx::window_construct(notebook, 6, wx::point_construct(0,0));
	startPage.setbackgroundcolour(colour = wx::colour_construct(212,220,237));
	bitmap = ImageHolder::images().buttonNewProj;
	mask = wx::mask_construct(bitmap, wx::colour_construct("#FF00FF"));
	bitmap.setmask(mask);
	buttonNewProj = wx::bitmapbutton_construct(startPage, 7, bitmap, wx::point_construct(100,300), wx::size_construct(228,100));
	bitmapsel = ImageHolder::images().buttonNewProjSelected;
	mask = wx::mask_construct(bitmapsel, wx::colour_construct("#FF00FF"));
	bitmapsel.setmask(mask);
	buttonNewProj.setbitmaphover(bitmapsel);
	bitmapOpen = ImageHolder::images().buttonOpenProj;
	mask = wx::mask_construct(bitmapOpen, wx::colour_construct("#FF00FF"));
	bitmapOpen.setmask(mask);
	buttonOpenProj = wx::bitmapbutton_construct(startPage, 8, bitmapOpen, wx::point_construct(100,410), wx::size_construct(228,100));
	bitmapOpensel = ImageHolder::images().buttonOpenProjSelected;
	mask = wx::mask_construct(bitmapOpensel, wx::colour_construct("#FF00FF"));
	bitmapOpensel.setmask(mask);
	buttonOpenProj.setbitmaphover(bitmapOpensel);
	mainToolBar = wx::window_construct(toolSpl1, 9, wx::point_construct(0,0));
	viewprojToolBar = wx::scrolledwindow_construct(toolSpl2, 10, wx::point_construct(0,0));
	wx::scrolledwindow_enablescrolling(viewprojToolBar,true,true);
	viewprojToolBar.setbackgroundcolour(colour = wx::colour_construct(255,255,255));
	filterToolBar = wx::window_construct(toolSpl2, 11, wx::point_construct(0,0));
	
	frame.show(true);
	frame.maximize(true);
	mainSpl.show(true);toolSpl1.show(true);toolSpl2.show(true);
	mainToolBar.show(true);
	viewprojToolBar.show(true);
	filterToolBar.show(true);
	viewprojToolBar.show(true);
	startPage.show(true);
	
	notebook.addpage(startPage, "Start Page");
	mainSpl.splitvertically(toolSpl2,toolSpl1,200);
	toolSpl2.splithorizontally(viewprojToolBar, filterToolBar);
	toolSpl1.splitvertically(notebook,mainToolBar,-200);
	
	mainToolBar = ToolBarRAD::loadMainToolBar(mainToolBar);
	
// Start Page
	startPage.dc = wx::windowdc_construct(startPage);

// Tree view in the begining
	viewprojToolBar.dc = wx::windowdc_construct(viewprojToolBar);
	
//  all windows data pin in frame
	frame.mainToolBar        = mainToolBar;
	frame.filterToolBar      = filterToolBar;
	frame.viewprojToolBar    = viewprojToolBar;
	frame.startPage          = startPage;
	frame.notebook           = notebook;
	frame.splitters          = [];
	frame.splitters.mainSpl	 = mainSpl;
	frame.splitters.toolSpl1 = toolSpl1;
	frame.splitters.toolSpl2 = toolSpl2;
// pin app
	frame.app = ::app;
// VETO for splitters events	
	mainSpl.connect (wx::EVT_SPLITTER_DCLICK, AddrFuncs::RadEvents(#onDClickSplitter));
	toolSpl1.connect(wx::EVT_SPLITTER_DCLICK, AddrFuncs::RadEvents(#onDClickSplitter));
	toolSpl2.connect(wx::EVT_SPLITTER_DCLICK, AddrFuncs::RadEvents(#onDClickSplitter));
// FIRST EVENT CONNECTIONS
	// move / resize the frame
	frame.connect(wx::EVT_MOVE, AddrFuncs::RadEvents(#onMoveEvent));
	frame.connect(wx::EVT_SIZE, AddrFuncs::RadEvents(#onResizeEvent));
	// move / resize the treeview window
	frame.viewprojToolBar.connect(wx::EVT_MOVE, AddrFuncs::RadEvents(#drawTreeViewTitle));
	frame.viewprojToolBar.connect(wx::EVT_SIZE, AddrFuncs::RadEvents(#drawTreeViewTitle));
	// menu events
	frame.connect(wx::ID_OPEN, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::RadEvents(#onOpenProject));
	frame.connect(wx::ID_NEW, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::RadEvents(#onCreateProject));
	frame.connect(wx::ID_ADD, wx::EVT_COMMAND_MENU_SELECTED, Filters::onLoadFilter);
	frame.connect(4, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::RadEvents(#onChangeBackgroundColour));
	frame.connect(wx::ID_SAVE, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::SaveLoad(#onSave));
	frame.connect(15, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::SaveLoad(#onSave));
	// create new project
	frame.connect(7, wx::EVT_COMMAND_BUTTON_CLICKED, AddrFuncs::RadEvents(#onCreateProject));
	// open new project
	frame.connect(8, wx::EVT_COMMAND_BUTTON_CLICKED, AddrFuncs::RadEvents(#onOpenProject));
	// undo / redo
	frame.connect(wx::ID_UNDO, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::UndoRedo(#onUndo));
	frame.connect(wx::ID_REDO, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::UndoRedo(#onRedo));
	// zoom in out
	frame.connect(wx::ID_ZOOM_IN , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Zoom(#onZoomIn));
	frame.connect(wx::ID_ZOOM_OUT, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Zoom(#onZoonOut));
	frame.connect(wx::ID_ZOOM_100, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Zoom(#onNormalizeZoom));
	// resize area of load project current pattern
	frame.connect(29 , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onResizeHorizantally));
	frame.connect(28 , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onResizeVertically));
	//
	frame.connect(5 , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::RadEvents(#onFilterDocument));
	//
	frame.connect(wx::ID_EXIT, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::RadEvents(#onExitRaiden));
	//
	notebook.frame = frame;
	notebook.connect(wx::EVT_MIDDLE_DOWN, AddrFuncs::PatternsEvents(#onMiddleDownClosePattern));
	notebook.connect(wx::EVT_RIGHT_DOWN, AddrFuncs::PatternsEvents(#onNotebookRightDown));
}

app.oninitadd(initializeRAD);
app.start();

std::dllunimportdeltalib(dllWX);
std::dllunimportdeltalib(dllXML);

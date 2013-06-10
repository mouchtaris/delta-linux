using #ImageHolder;
using #ShapeRAD;
using #AddrFuncs;


function onActivatedItem (frame, ev) {
	currProject = frame.currProject;
	treeitemid = ev.getitem();
	treerootid = frame.root;
	
	if (treeitemid.equal(treerootid)) {
		//nope
	}
	else
	if (treeitemid.equal(currProject.treeitemid)) {
		frame.notebook.setselection(0);
	}
	else {
		foreach (filter, currProject.filters)
			if (treeitemid.equal(filter.treeitemid)) {
				AddrFuncs::Dialogs(filter.class) (filter, currProject, frame);
			}
		foreach (pattern, currProject.patterns)
			if (treeitemid.equal(pattern.treeitemid)) {
				wxparent = [];
				wxparent.frame = frame;
				wxparent.currProject = currProject;
				wxparent.currPattern = pattern;
				AddrFuncs::ProjectEvent(#onOpenTabPattern)(wxparent, ev);
				return;
			}
		foreach (pattern, currProject.patterns)
			foreach (role, pattern.constituents.Role)
				if (treeitemid.equal(role.treeitemid)) {
					wxparent = [];
					wxparent.frame = frame;
					wxparent.currProject = currProject;
					wxparent.currPattern = pattern;
					AddrFuncs::ProjectEvent(#onOpenTabPattern)(wxparent, ev);
					foreach (item, pattern.constituents.selectedItems)
						pattern.constituents[item.type][item.id].selected = false;
					pattern.constituents.selectedItems.clear();
					role.selected = true;
					pattern.constituents.selectedItems.push_back([@type : role.class, @id : role.id]);
					if (pattern.window!=nil)
						pattern.window.refresh();
					AddrFuncs::Dialogs(role.class)(role,currProject,frame);
					return;
				}
	}
}

function onBeginEdit (frame, ev) {
	currProject = frame.currProject;
	treeitemid = ev.getitem();
	treerootid = frame.root;
	if (treeitemid.equal(treerootid) or treeitemid.equal(currProject.treeitemid))
		ev.veto();
}

function onEditLabel (frame, ev) {
	currProject = frame.currProject;
	treeitemid = ev.getitem();
	label = ev.getlabel();
	treerootid = frame.root;
	treectrl = frame.treectrl;
	if (label=="" or std::strsub(label," ")!=-1) {
		ev.veto();
		return;
	}
	foreach (pattern, currProject.patterns)
		if (treeitemid.equal(pattern.treeitemid)) {
			foreach (p, currProject.patterns)
				if (p.name==label) {
					ev.veto();
					return;
				}
			pattern.renamePattern (frame, label);
			//
			if (pattern.window!=nil) {
				notebook = frame.notebook;
				for(i=0;i<notebook.getpagecount();++i) {
					currPage = notebook.getpage(i);
					if (currPage.getid()==pattern.window.getid()) {
						notebook.setpagetext(i,pattern.name);
						break; }
				}
			}
			ShapeRAD::drawPatterns(currProject);
			return;
		}
	foreach (pattern, currProject.patterns)
		foreach (role, pattern.constituents.Role)
			if (treeitemid.equal(role.treeitemid)) {
				foreach (r, pattern.constituents.Role)
					if (r.shortDesc==label) {
						ev.veto();
						return;
					}
				role.shortDesc = ev.getlabel();
				local selectedItems = pattern.constituents.selectedItems;
				wxparent = [];
				wxparent.frame = frame;
				wxparent.currProject = currProject;
				wxparent.currPattern = pattern;				
				AddrFuncs::ProjectEvent(#onOpenTabPattern)(wxparent, ev);
				foreach (item, pattern.constituents.selectedItems)
					pattern.constituents[item.type][item.id].selected = false;
				pattern.constituents.selectedItems.clear();
				role.selected = true;
				pattern.constituents.selectedItems.push_back([@type : role.class, @id : role.id]);
				if (pattern.window!=nil)
						pattern.window.refresh();
				return;
			}
}

function onRightClick (frame, ev) {
	currProject = frame.currProject;
	currPoint  = ev.getpoint();
	currPoint.sety(currPoint.gety()+25);
	currPoint.setx(currPoint.getx()+5);
	treeitemid = ev.getitem();
	label = ev.getlabel();
	treerootid = frame.root;
	treectrl = frame.treectrl;
	// project item of treeview
	if (treeitemid.equal(treerootid) or treeitemid.equal(currProject.treeitemid)) {
		wxmenu = wx::menu_construct();
		// pin data of object
		wxmenu.frame = frame;
		wxmenu.currProject = currProject;
		wxmenu.currPoint = wx::point_construct(100,100);
		//
		viewId	     = currProject.connManageId.getID();
		newPattId    = currProject.connManageId.getID();
		loadPattId	 = currProject.connManageId.getID();
		loadFilterId = currProject.connManageId.getID();
		closeId      = currProject.connManageId.getID();
		//
		subMenu = wx::menu_construct();
		subMenu.append(newPattId   , "New Pattern");
		subMenu.append(loadPattId  , "Existing Pattern");
		subMenu.append(loadFilterId, "Load Filter");
		//
		wxmenu.append(viewId, "View");
		wxmenu.append(wx::ID_ANY, "Add", subMenu);
		wxmenu.append(closeId, "Close");
		wxmenu.connect(newPattId   , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onCreatePattern));
		wxmenu.connect(loadPattId  , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onLoadPattern));
		wxmenu.connect(loadFilterId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onLoadFilter));
		frame.viewprojToolBar.popupmenu(wxmenu, currPoint);
		wxmenu.disconnect(newPattId   , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onCreatePattern));
		wxmenu.disconnect(loadPattId  , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onLoadPattern));
		wxmenu.disconnect(loadFilterId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onLoadFilter));
		// free ids
		ids = [newPattId,loadPattId,loadFilterId,closeId];
		currProject.connManageId.removeIDs(ids);
		wxmenu.destruct();
	}
	// check for filter or pattern right click case
	else {
		// pattern case
		foreach (pattern, currProject.patterns)
			if (treeitemid.equal(pattern.treeitemid)) {
				wxmenu = wx::menu_construct();
				// pin data of object
				wxmenu.frame 		= frame;
				wxmenu.currProject	= currProject;
				wxmenu.currPattern	= pattern;
				// ids
				openId		= currProject.connManageId.getID();
				exportId	= currProject.connManageId.getID();
				removeId	= currProject.connManageId.getID();
				propsId		= currProject.connManageId.getID();
				//
				wxmenu.append(openId, "Open");
				menuitem = wx::menuitem_construct(wxmenu, exportId, "Export...");
				menuitem.setbitmap(ImageHolder::images().export);
				wxmenu.append(menuitem);
				menuitem = wx::menuitem_construct(wxmenu, removeId, "Remove");
				menuitem.setbitmap(ImageHolder::images().remove);
				wxmenu.append(menuitem);
				menuitem = wx::menuitem_construct(wxmenu, propsId, "Properties");
				menuitem.setbitmap(ImageHolder::images().properties);
				wxmenu.append(menuitem);
				//
				wxmenu.connect(openId  , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onOpenTabPattern));
				wxmenu.connect(exportId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onExportPatternBMP));
				wxmenu.connect(removeId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onDeletePattern));
				wxmenu.connect(propsId , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Dialogs(#onPattern));
				//
				frame.viewprojToolBar.popupmenu(wxmenu, currPoint);
				//
				wxmenu.disconnect(openId  , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onOpenTabPattern));
				wxmenu.disconnect(exportId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onExportPatternBMP));
				wxmenu.disconnect(removeId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::ProjectEvent(#onDeletePattern));
				wxmenu.disconnect(propsId , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Dialogs(#onPattern));
				// free ids
				ids = [openId,removeId,propsId];
				currProject.connManageId.removeIDs(ids);
				wxmenu.destruct();
				return;
			}
		// filter case
		foreach (filter, currProject.filters)
			if (treeitemid.equal(filter.treeitemid)) {
				wxmenu = wx::menu_construct();
				// pin data of object
				wxmenu.frame 		= frame;
				wxmenu.currProject	= currProject;
				wxmenu.currFilter	= filter;
				// ids
				openId		= currProject.connManageId.getID();
				runId		= currProject.connManageId.getID();
				updateId	= currProject.connManageId.getID();
				removeId	= currProject.connManageId.getID();
				propsId		= currProject.connManageId.getID();
				//
				wxmenu.append(openId  , "Open");
				menuitem = wx::menuitem_construct(wxmenu, runId, "Run");
				menuitem.setbitmap(ImageHolder::images().run);
				wxmenu.append(menuitem);
				wxmenu.append(updateId, "Update");
				menuitem = wx::menuitem_construct(wxmenu, removeId, "Remove");
				menuitem.setbitmap(ImageHolder::images().remove);
				wxmenu.append(menuitem);
				menuitem = wx::menuitem_construct(wxmenu, propsId, "Properties");
				menuitem.setbitmap(ImageHolder::images().properties);
				wxmenu.append(menuitem);
				//TODO open the sparrow text editor and open the filter code
				wxmenu.connect(openId  , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onFilter));
				wxmenu.connect(runId   , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onRunFilter));
				wxmenu.connect(updateId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onUpdateFilter));
				wxmenu.connect(removeId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onRemoveFilter));
				wxmenu.connect(propsId , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Dialogs(#onFilter));
				//
				frame.popupmenu(wxmenu, currPoint);
				wxmenu.disconnect(openId  , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onFilter));
				wxmenu.disconnect(runId   , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onRunFilter));
				wxmenu.disconnect(updateId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onUpdateFilter));
				wxmenu.disconnect(removeId, wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Filters(#onRemoveFilter));
				wxmenu.disconnect(propsId , wx::EVT_COMMAND_MENU_SELECTED, AddrFuncs::Dialogs(#onFilter));
				// free ids
				ids = [openId,runId,updateId,removeId,propsId];
				currProject.connManageId.removeIDs(ids);
				wxmenu.destruct();
				return;
			}
	}
}

function destroyTreeView (frame) {
	frame.currProject.treeitemid = nil;
	foreach (pattern, frame.currProject.patterns) {
		pattern.treeitemid = nil;
		foreach (role, pattern.constituents["Role"])
			role.treeitemid = nil;
	}
	frame.treectrl.deleteallitems();
	frame.treectrl.destroy();
	frame.treectrl = nil;
	frame.root = nil;
}

function disconnectEVT (frame) {
	treectrlID = frame.treectrl.getid();
	frame.disconnect(treectrlID, wx::EVT_COMMAND_TREE_ITEM_ACTIVATED, ::onActivatedItem);
	frame.disconnect(treectrlID, wx::EVT_COMMAND_TREE_END_LABEL_EDIT, ::onEditLabel);
	frame.disconnect(treectrlID, wx::EVT_COMMAND_TREE_BEGIN_LABEL_EDIT, ::onBeginEdit);
	frame.disconnect(treectrlID, wx::EVT_COMMAND_TREE_ITEM_RIGHT_CLICK, ::onRightClick);
}

function connectEVT (frame) {
	treectrlID = frame.treectrl.getid();
	frame.connect(treectrlID, wx::EVT_COMMAND_TREE_ITEM_ACTIVATED, ::onActivatedItem);
	frame.connect(treectrlID, wx::EVT_COMMAND_TREE_END_LABEL_EDIT, ::onEditLabel);
	frame.connect(treectrlID, wx::EVT_COMMAND_TREE_BEGIN_LABEL_EDIT, ::onBeginEdit);
	frame.connect(treectrlID, wx::EVT_COMMAND_TREE_ITEM_RIGHT_CLICK, ::onRightClick);
}


/**
 *  Description: Main function to create the treeview
 *  Arguments:
 *  	currProject, the project which is load
 *  	frame, main frame of raiden
 *  Returns: nope
 **/
function TreeView (currProject, frame) {
	treeWin = frame.viewprojToolBar;
//	treeWin.setbackgroundcolour(colour = wx::colour_construct(255,255,255));
//	colour.destruct();
	treectrlID = currProject.connManageId.getID();
	treectrl = wx::treectrl_construct(treeWin,treectrlID,wx::point_construct(4,25), treeWin.getsize(),
	wx::flags(wx::TR_EDIT_LABELS, wx::TR_HAS_BUTTONS, wx::NO_BORDER));
	treectrl.setimagelist(ImageHolder::images().imageListNB);
	root = treectrl.addroot("Active Project:",ImageHolder::images().imageListNB.Active);
	treectrl.setitemimage(root, ImageHolder::images().imageListNB.Active);
	currProject.treeitemid = treectrl.appenditem(root, currProject.title,ImageHolder::images().imageListNB.Project);
	treectrl.setitemimage(currProject.treeitemid, ImageHolder::images().imageListNB.Project);
	// create pattern and filters folders(filters) of project
	pattreeitemid = treectrl.appenditem(currProject.treeitemid, "patterns",ImageHolder::images().imageListNB.Folder);
	treectrl.setitemimage(pattreeitemid, ImageHolder::images().imageListNB.Folder);
	filtreeitemid = treectrl.appenditem(currProject.treeitemid, "filters",ImageHolder::images().imageListNB.Folder);
	treectrl.setitemimage(filtreeitemid, ImageHolder::images().imageListNB.Folder);
	//
	foreach (pattern, currProject.patterns) {
		pattern.treeitemid = treectrl.appenditem(pattreeitemid, pattern.name, ImageHolder::images().imageListNB.Pattern);
		treectrl.setitemimage(pattern.treeitemid, ImageHolder::images().imageListNB.Pattern);
		foreach (role, pattern.constituents.Role) {
			role.treeitemid = treectrl.appenditem(pattern.treeitemid, role.shortDesc, ImageHolder::images().imageListNB.Role);
			treectrl.setitemimage(role.treeitemid, ImageHolder::images().imageListNB.Role);
		}
	}
	foreach (filter, currProject.filters) {
		filter.treeitemid = treectrl.appenditem(filtreeitemid, filter.name, ImageHolder::images().imageListNB.Filter);
		treectrl.setitemimage(filter.treeitemid, ImageHolder::images().imageListNB.Filter);
	}
	treectrl.expandall();
	frame.treectrl = treectrl;
	frame.Patterntreectrlid = pattreeitemid;
	frame.Filtertreectrlid  = filtreeitemid;
	frame.root = root;
	::connectEVT(frame);
}

function removeChildPattern (currProject,role,frame) {
	treectrl = frame.treectrl;
	treectrl.delete(role.treeitemid);
}

function addChildPattern (currProject,role,frame) {
	treectrl = frame.treectrl;
	pattern = currProject.currentPattern;
	role.treeitemid = treectrl.appenditem(pattern.treeitemid, role.shortDesc, ImageHolder::images().imageListNB.Role);
	treectrl.setitemimage(role.treeitemid, ImageHolder::images().imageListNB.Role);
	treectrl.expand(pattern.treeitemid);
}

function changeChildPattern (role,frame) {
	treectrl = frame.treectrl;
	treectrl.setitemtext(role.treeitemid,role.shortDesc);
}

/**
 *  Description: Remove pattern or filter from treeview
 **/
function removeChildProj(currProject,element,frame) {
	treectrl = frame.treectrl;
	treectrl.delete(element.treeitemid);
}

function addChildProj (currProject, element, frame) {
	treectrl = frame.treectrl;
	element.treeitemid = treectrl.appenditem(frame[element.class+"treectrlid"], element.name, ImageHolder::images().imageListNB[element.class]);
	treectrl.setitemimage(element.treeitemid, ImageHolder::images().imageListNB[element.class]);
	treectrl.expand(frame[element.class+"treectrlid"]);
}

function changeChildProj (currProject, element, frame) {
	treectrl = frame.treectrl;
	treectrl.setitemtext(element.treeitemid,element.name);
}

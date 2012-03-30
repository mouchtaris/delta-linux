using #Constituents;
using #IdManagement;
using #ImageHolder;
using #TreeView;
using #AddrFuncs;


// Globals

currentProject = nil;

function GetCurrentProject ()
	{ return ::currentProject; }
function SetCurrProject (project,wxparent) {
	wxparent.notebook.removepage(0);
	::currentProject.window.disconnect(wx::EVT_MOTION, AddrFuncs::ProjectEvent(#onMotionEventMainPage));
	::currentProject.window.disconnect(wx::EVT_LEFT_UP, AddrFuncs::ProjectEvent(#onLeftUpMainPage));
	::currentProject.window.disconnect(wx::EVT_LEFT_DOWN, AddrFuncs::ProjectEvent(#onLeftDownMainPage));
	::currentProject.window.disconnect(wx::EVT_RIGHT_UP, AddrFuncs::ProjectEvent(#onRightUp));
	::currentProject.window.disconnect(wx::EVT_LEFT_DCLICK, AddrFuncs::ProjectEvent(#onLeftDClick));
	::currentProject.window.show(false);
	::currentProject.window.destruct();
	::currentProject = project;
	TreeView::disconnectEVT(wxparent);
	TreeView::destroyTreeView(wxparent);
	wxparent.currProject = ::currentProject;
	wxparent.notebook.currProject = ::currentProject;
	wxparent.notebook.addpage(::currentProject.window, ::currentProject.title, true, ImageHolder::images().imageListNB.Project);
	::currentProject.window.currProject = ::currentProject;
	::currentProject.window.frame = wxparent;
	::currentProject.window.frame.currProject = ::currentProject;
	::currentProject.window.dc = wx::clientdc_construct(::currentProject.window);
	::currentProject.window.connect(wx::EVT_MOTION, AddrFuncs::ProjectEvent(#onMotionEventMainPage));
	::currentProject.window.connect(wx::EVT_LEFT_UP, AddrFuncs::ProjectEvent(#onLeftUpMainPage));
	::currentProject.window.connect(wx::EVT_LEFT_DOWN, AddrFuncs::ProjectEvent(#onLeftDownMainPage));
	::currentProject.window.connect(wx::EVT_RIGHT_UP, AddrFuncs::ProjectEvent(#onRightUp));
	::currentProject.window.connect(wx::EVT_LEFT_DCLICK, AddrFuncs::ProjectEvent(#onLeftDClick));
	::currentProject.window.show(true);
	TreeView::TreeView (::currentProject, wxparent);
}
function SetCurrentProject (project, wxparent) {
	::currentProject = project;
	wxparent.notebook.removepage(0);
	wxparent.startPage.show(false);
	wxparent.notebook.setimagelist(ImageHolder::images().imageListNB);
	wxparent.notebook.addpage(::currentProject.window, ::currentProject.title, true, ImageHolder::images().imageListNB.Project);
	//
	wxparent.notebook.currProject = ::currentProject;
	::currentProject.window.currProject = ::currentProject;
	::currentProject.window.frame = wxparent;
	::currentProject.window.dc = wx::clientdc_construct(::currentProject.window);
	::currentProject.window.connect(wx::EVT_MOTION, AddrFuncs::ProjectEvent(#onMotionEventMainPage));
	::currentProject.window.connect(wx::EVT_LEFT_UP, AddrFuncs::ProjectEvent(#onLeftUpMainPage));
	::currentProject.window.connect(wx::EVT_LEFT_DOWN, AddrFuncs::ProjectEvent(#onLeftDownMainPage));
	::currentProject.window.connect(wx::EVT_RIGHT_UP, AddrFuncs::ProjectEvent(#onRightUp));
	::currentProject.window.connect(wx::EVT_LEFT_DCLICK, AddrFuncs::ProjectEvent(#onLeftDClick));
	::currentProject.window.show(true);
	TreeView::TreeView (::currentProject, wxparent);
}


function ProjProperties () {
	return [
		@requirement : [
			@style	: "Rectangle",
			@colour	: "#FFFFFF",
			@size	: wx::size_construct(100,60)
		],
		@operation : [
			@style	: "Rectangular",
			@colour	: "#FFFFFF",
			@size	: wx::size_construct(110,60)
		],
		@role : [
			@style	: "Ellipse",
			@colour	: "#FFFFFF",
			@size	: wx::size_construct(120,80)
		],
		@input : [
			@style	: "Rhompus",
			@colour	: "#FFFFFF",
			@size	: wx::size_construct(100,60)
		],
		@output : [
			@style	: "Rhompus",
			@colour	: "#FFFFFF",
			@size	: wx::size_construct(100,60)
		],
		@address : [
			@style : "SHORT_DASH",
			@colour	: "#000066",
			@size	: 2
		],
		@use : [
			@style : "DOT_DASH",
			@colour	: "#003366",
			@size	: 2
		],
		@offer : [
			@style : "LONG_DASH",
			@colour	: "#3333CC",
			@size	: 2
		]
	];
}


function Filter (id, name, description, author, path, dbc) {
	return [
		@class			: #Filter,
		@id				: id,
		@name			: name,
		@description	: description,
		@author			: author,
		// src path
		@path			: path,
		// dbc path
		@dbc			: dbc,
		@created		: wx::datetime_now(),
		@lastUpdate		: wx::datetime_now(),
		@notes			: std::list_new()
	];
}


function Pattern (id, projOffset, name, path, author, description, point) {
	return [
		@class			: #Pattern,
		@id				: id,
		@projOffset		: projOffset,
		@name			: name,
		@path			: path,
		@author         : author,
		@description	: description,
		@created		: wx::datetime_now(),
		@lastUpdate		: wx::datetime_now(),
		//
		@constituents	: Constituents::Constituents(),
		@rect           : wx::rect_construct(wx::point_construct(point.getx(),point.gety()), wx::size_construct(200,100)),
		@viewPoint		: wx::point_construct(0,0),
		@virtualsize	: wx::size_construct(2000,2000),
		//
		@notes			: std::list_new(),
		method renamePattern (frame, newname) {
			std::fileexecute("rename "+@path+"\\"+@name+".patt "+newname+".patt");
			@name	= newname;
			AddrFuncs::SaveLoad("onSave") (frame, wx::menuevent_construct());
		},
		method setRect (x,y) {
			@rect.setx(x);
			@rect.sety(y);
		},
		method setPattern (name, author, description) {
			@name        = name;
			@author      = author;
			@description = description;
		}
	];
}


function Project (id, title, author, properties, wxparent) {
	suggestAuthors = std::list_new();
	if (author!="" and std::strsub(author," ")!=-1)
		suggestAuthors.push_back(author);
	window = wx::window_construct(wxparent.notebook, 14, wx::point_construct(0,0), wx::size_construct(1000,1000), wx::RETAINED);
	window.setscrollbar(20, 20, 50, 50);
	wx::window_setscrollpos(window,0,0,true);
//	window.alwaysshowscrollbars (true, true);
//	window.enablescrolling(true,true);
	window.setbackgroundcolour(colour = wx::colour_construct(214,240,245));
	colour.destruct();
	return [
		@class			: #Project,
		@id				: id,
		@window			: window,
		@title			: title,
		@author			: author,
		@properties 	: properties,
		@patterns		: std::list_new(),
		@filters		: std::list_new(),
		@created		: wx::datetime_now(),
		@lastUpdate		: wx::datetime_now(),
		@currentPattern : std::undefined(),
		@connManageId	: IdManagement::ConnectionsId(),
		@suggestAuthors	: suggestAuthors,
		@patternOffset  : -1,
		@filterOffset	: -1,
		method addPattern (path, point) {
			id = ++@patternOffset;
			pattern = ::Pattern(id, @id, "Pattern"+id, path, "", "", point);
			@patterns.push_front(pattern);
			return pattern;
		},
		method removePattern (patternId, frame) {
			foreach (pat, @patterns) {
				if (pat.id==patternId) {
					@patterns.remove(pat);
					AddrFuncs::SaveLoad(#onSave) (frame, wx::menuevent_construct());
					return true;
				}
			}
			return false;
		},
		method addFilter (name, description, author, path, dbc) {
			id = ++@filterOffset;
			filter = ::Filter(id, name, description, author, path, dbc);
			@filters.push_back(filter);
			return filter;
		},
		method removeFilter (filterId) {
			foreach (fil, @filters)
				if (fil.id==filterId) {
					@filters.remove(fil);
					return true;
				}
			return false;
		},
		method addSuggestAuthor (author) {
			foreach (sauthor, @suggestAuthors)
				if (sauthor==author)
					return;
			if (author!="" and std::strsub(author," ")!=-1)
				@suggestAuthors.push_back(author);
		}
	];
}


using #ShapeRAD;
using #ConstituentsMouseEvents;
using #TreeView;


function execHistory (currProject, wxparent, history) {
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	constituents	= currPattern.constituents;
	total = std::tablength(history);
	for(i=0; i<total; ++i) {
		action = history[i];
		if (action.packet!=nil and action.packet==true) {
			packActions = action.data;
			::execHistory(currProject, wxparent, packActions);
		}
		else
		if (action.obj=="Constituents") {
			if (action.data!=nil) {
				if (action.method!=nil) {
					if (action.method=="remove") {
						foreach (item, constituents[action.data])
							if (item.type==action.args[0].type and item.id==action.args[0].id)
								{ constituents[action.data][action.method] (item); break; }
					}
					else
					if (std::strsub(action.method,"add")>=0) {
						constituents[action.data][action.method] (|action.args|);
					}
					else {
						if (action.data=="selectedItems") {
							unique = true;
							comp = action.args[0];
							foreach (item, constituents.selectedItems)
								if (item.id==comp.id and item.type==comp.type)
									{ unique = false; break; }
							if (unique)
								constituents[action.data][action.method] (|action.args|);
						}
						else {
							constituents[action.data][action.method] (|action.args|);
						}
					}
				}
				else { assert(false); }
			}
			else {
				if (std::strsub(action.method,"Role")>=0) {
					if (std::strsub(action.method,"add")>=0) {
						constituents[action.method] (|action.args|);
						roleId;
						if (action.method=="addRole") { roleId = action.args[2]; }
						else
						if (action.method=="addAddressRole" or action.method=="addPartOFRole") { roleId = action.args[4]; }
						else
						if (action.method=="addUseRole" or action.method=="addOfferRole") { roleId = action.args[5]; }
						TreeView::addChildPattern(currProject, constituents["Role"][roleId], wxparent);
					}
					else
					if (std::strsub(action.method,"remove")>=0) {
						roleId;
						if (action.method=="removeRole"){ roleId = action.args[0]; }
						else							{ roleId = action.args[1]; }
						TreeView::removeChildPattern(currProject, constituents["Role"][roleId], wxparent);
						constituents[action.method] (|action.args|);
					}
					else {
						constituents[action.method] (|action.args|);
					}
				}
				else {
					constituents[action.method] (|action.args|);
				}
			}
		}
		else {
			constituent = constituents[action.obj][action.id];
			if (action.data != nil) {
				if (std::isnil(action.method)) {
					constituent[action.data] = action.args[0];
				}
				else {
					constituent[action.data][action.method] (|action.args|);
				}
			}
			else {
				constituent[action.method] (|action.args|);
			}
		}
	}
	scrolledwindow.refresh();
}

function createOppositeActions(currProject, history) {
	constituents = currProject.currentPattern.constituents;
	redo = [];
	total = std::tablength(history);
	index = 0;
	for(i=0; i<total; ++i) {
		redo[index] = [];
		action = history[i];
		if (action.packet==true) {
			redo[index].obj = "Constituents";
			redo[index].method = action.method;
			redo[index].args = action.args;
		}
		else
		if (action.obj=="Constituents") {
			redo[index].obj = action.obj;
			if (action.data!=nil) {
				redo[index].data = action.data;
				if (action.method!=nil) {
					if (action.method=="insert") {
						redo[index].method = "remove";
						redo[index].args = [{0:action.args[1]}];
					}
					else
					if (action.method=="remove") {
						redo[index].method = "insert";
						curritem = action.args[0];
						it = std::listiter_new();
						it.setbegin(constituents[action.data]);
						while(it.checkend(constituents[action.data])==false) {
							item = it.getval();
							if (item==nil)
								assert(false);
							if (item.type==curritem.type and item.id==curritem.id)
								{ redo[index].args = [{0:it},{1:item}]; break; }
							it.fwd();
						}
					}
					else
					if (action.method=="push_back") {
						redo[index].method = "remove";
						redo[index].args = action.args;
					}
					else { assert(false); }
				}
				else { assert(false); }
			}
			else {
				type = "";
				if (std::strsub(action.method,"remove")>=0) {
					for(j=6;j<std::strlen(action.method);++j) type += std::strchar(action.method,j);
					redo[index] = nil;
					redo[index] = constituents["history_remove"+type] (|action.args|);
				}
				else
				if (std::strsub(action.method,"add")>=0) {
					for(j=3;j<std::strlen(action.method);++j) type += std::strchar(action.method,j);
					redo[index].obj = "Constituents";
					redo[index].method = "remove"+type;
					if (std::strsub(type,"Address")>=0 and std::strlen(type)>7) {
						ctype = "";
						for (j=7;j<std::strlen(type);++j) ctype += std::strchar(type,j);
						addressId = constituents.idManage.getID("Address");
						constituents.idManage.removeID("Address", addressId);
						ctypeId = constituents.idManage.getID(ctype);
						constituents.idManage.removeID(ctype, ctypeId);
						redo[index].args = [{0:addressId},{1:ctypeId},{2:currProject}];
					}
					else
					if (std::strsub(type,"Use")>=0 and std::strlen(type)>3) {
						ctype = "";
						for (j=3;j<std::strlen(type);++j) ctype += std::strchar(type,j);
						useId = constituents.idManage.getID("Use");
						constituents.idManage.removeID("Use", useId);
						ctypeId = constituents.idManage.getID(ctype);
						constituents.idManage.removeID(ctype, ctypeId);
						redo[index].args = [{0:useId},{1:ctypeId},{2:currProject}];
					}
					else
					if (std::strsub(type,"Offer")>=0 and std::strlen(type)>5) {
						ctype = "";
						for (j=3;j<std::strlen(type);++j) ctype += std::strchar(type,j);
						offerId = constituents.idManage.getID("Offer");
						constituents.idManage.removeID("Offer", offerId);
						ctypeId = constituents.idManage.getID(ctype);
						constituents.idManage.removeID(ctype, ctypeId);
						redo[index].args = [{0:offerId},{1:ctypeId},{2:currProject}];
					}
					else
					if (std::strsub(type,"PartOF")>=0 and std::strlen(type)>6) {
						ctype = "";
						for (j=3;j<std::strlen(type);++j) ctype += std::strchar(type,j);
						partofId = constituents.idManage.getID("PartOF");
						constituents.idManage.removeID("PartOF", partofId);
						ctypeId = constituents.idManage.getID(ctype);
						constituents.idManage.removeID(ctype, ctypeId);
						redo[index].args = [{0:partofId},{1:ctypeId},{2:currProject}];
					}
					else {
						id = constituents.idManage.getID(type);
						constituents.idManage.removeID(type, id);
						redo[index].args = [{0:id},{1:currProject}];
					}
				}
				else
				if (std::strsub(action.method,"undoSplitRole")>=0) {
					redo[index].obj		= "Constituents";
					redo[index].method	= "redoSplitRole";
					redo[index].args	= action.args;
				}
				else
				if (std::strsub(action.method,"redoSplitRole")>=0) {
					redo[index].obj		= "Constituents";
					redo[index].method	= "undoSplitRole";
					redo[index].args	= action.args;
				}
				else
				if (std::strsub(action.method,"undoJoinRole")>=0) {
					redo[index].obj		= "Constituents";
					redo[index].method	= "redoJoinRole";
					redo[index].args	= action.args;
				}
				else
				if (std::strsub(action.method,"redoJoinRole")>=0) {
					redo[index].obj		= "Constituents";
					redo[index].method	= "undoJoinRole";
					redo[index].args	= action.args;
				}
				else { assert(false); }
			}
		}
		// Constituent
		else {
			redo[index].obj = action.obj;
			redo[index].id  = action.id;
			if (action.data != nil) {
				if (std::isnil(action.method)) {
					redo[index].data = action.data;
					if (action.args[0]==true) {
						redo[index].args = [{0:false}];
					}
					else
					if (action.args[0]==false) {
						redo[index].args = [{0:true}];
					}
					else { assert(false); }
				}
				else {
					redo[index].data = action.data;
					if (action.method=="remove") {
						redo[index].method = "push_back";
						redo[index].args = [{0:action.args[0]}];
					}
					else
					if (action.method=="insert") {
						redo[index].method = "remove";
						redo[index].args = [{0:action.args[1]}];
					}
					else
					if (action.method=="push_back") {
						redo[index].method = "remove";
						redo[index].args = [{0:action.args[0]}];
					}
					else
					if (action.method=="setSegment") {
						constituent = constituents[action.obj][action.id];
						redo[index].method = "setSegment";
						currpt = constituent[action.data][("point"+action.args[1])];
						redo[index].args = [{0:wx::point_construct(currpt.getx(),currpt.gety())},{1:action.args[1]}];
					}
					else
					if (action.method=="setPoint") {
						constituent = constituents[action.obj][action.id];
						redo[index].method = "setPoint";
						redo[index].args = [{0:constituent[action.data].x},{1:constituent[action.data].y}];
					}
					else { assert(false); }
				}
			}
			else {
				constituent = constituents[action.obj][action.id];
				if (action.method!=nil) {
					redo[index] = nil;
					--index;
					if (action.method=="setRequirement") {
						redo[index].args = [{0:constituent.shortdesc},{1:constituent.description},{2:constituent.type},{3:constituent.status},{4:constituent.priority},{5:constituent.difficulty},{6:constituent.author}];
					}
					else
					if (action.method=="setElement") {
						redo[index].args = [{0:constituent.shortDesc},{1:constituent.description},{2:constituent.status},{3:constituent.priority},{4:constituent.difficulty},{5:constituent.author}];
					}
					else
					if (action.method=="setSElement") {
						redo[index].args = [{0:constituent.shortDesc},{1:constituent.description}];
					}
					else { assert(false); }
				}
				else {}
			}
		}
		++index;
	}
	return redo;
}


function onUndo (frame, ev) {
	currProject = frame.currProject;
	if (currProject!=nil) {
		notebook = frame.notebook;
		// main project page
		if ((notebook.getpage(notebook.getselection())).getid()==14) {
		}
		else {
			if ((currPattern=currProject.currentPattern) and (constituents=currPattern.constituents)) {
				undoStack = constituents.undoStack;
				if(undoStack.total()>0) {
					history		= undoStack.pop_back();
					future = ::createOppositeActions(currProject,history);
					constituents.redoStack.push_back(future);
					::execHistory(currProject, frame, history);
					ConstituentsMouseEvents::onEnterWindow(currPattern.window,ev);
					scrolledwindow = currPattern.window;
					scrolledwindow.refresh();
				}
			}
		}
	}
}

function onRedo (frame, ev) {
	currProject = frame.currProject;
	if (currProject!=nil) {
		notebook = frame.notebook;
		// main project page
		if ((notebook.getpage(notebook.getselection())).getid()==14) {
		}
		else {
			if ((currPattern=currProject.currentPattern) and (constituents=currPattern.constituents)) {
				redoStack = constituents.redoStack;
				if(redoStack.total()>0) {
					redo = redoStack.pop_back();
					history = ::createOppositeActions(currProject, redo);
					constituents.undoStack.push_back(history);
					::execHistory(currProject, frame, redo);
					ConstituentsMouseEvents::onEnterWindow(currPattern.window,ev);
					scrolledwindow = currPattern.window;
					scrolledwindow.refresh();
				}
			}
		}
	}
}


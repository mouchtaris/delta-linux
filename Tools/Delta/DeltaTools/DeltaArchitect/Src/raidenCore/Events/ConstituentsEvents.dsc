using #Dialogs;
using #ShapeRAD;
using #TreeView;
using #Utilities;
using #AddrFuncs;
using #ImageHolder;


/*
		Elements
*/

function onCreateConstituent (frame, ev) {
	// give in event obj the type of constituent
	type = ev.type;
	local point		= nil;
	local wxparent	= nil;
	// right click event has as frame the frame.window
	if (frame.window!=nil) {
		frame = frame.window;
		wxparent = frame.wxparent;
		point = frame.ev.getposition();
	}
	// left click event
	else {
		wxparent = frame.wxparent;
		point = ev.getposition();
	}
	// project loaded in raiden
	currProject		= frame.currentProject;
	currPattern		= currProject.currentPattern;
	constituents	= currPattern.constituents;
	// fix point for scrollbars
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	point    		= Utilities::devicetological(scrolledwindow, point.getx(), point.gety(), scale);
	//
	constituent		= constituents["add"+type] (currProject.properties, point);
	//
	history = [];
	history[0] = [{#obj:"Constituents"},{#method:("remove"+type)},{#args:[{0:constituent.id},{1:currProject}]}];
	index = 0;
	// deselect all items that are selected and keep as selected only the new one constituent
	foreach (item, constituents.selectedItems) {
		constituents[item.type][item.id].selected = false;
		history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{#method:"push_back"},{#args:[{0:item}]}];
		history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
	}
	//
	constituents.undoStack.push_back(history);
	// new action by user clear redo stack
	constituents.redoStack.clear();
	//
	constituents.selectedItems.clear();
	constituents.selectedItems.push_back([{"type":constituent.class},{"id":constituent.id}]);
	constituent.selected = true;
	// new item must be first in the priority stack
	constituents.priorityStack.push_front([{"type":constituent.class},{"id":constituent.id}]);
	scrolledwindow.refresh();
	// update treeview if new item is role
	if (constituent.class=="Role") {
		TreeView::addChildPattern(currProject, constituent, wxparent);
		AddrFuncs::Dialogs(type) (constituent, currProject, wxparent);
	}
	else
		AddrFuncs::Dialogs(type) (constituent, currProject);
}
function onCreateRequirement(frame, ev) { ev.type = #Requirement;	::onCreateConstituent(frame, ev); }
function onCreateOperation	(frame, ev) { ev.type = #Operation;		::onCreateConstituent(frame, ev); }
function onCreateRole		(frame, ev) { ev.type = #Role;			::onCreateConstituent(frame, ev); }
function onCreateInput		(frame, ev) { ev.type = #Input;			::onCreateConstituent(frame, ev); }
function onCreateOutput 	(frame, ev) { ev.type = #Output;		::onCreateConstituent(frame, ev); }


/*
		Element Relationships
*/

function onCreateAddress (frame, ev) {
	local point				= nil;
	local currProject		= nil;
	local currPattern		= nil;
	local scrolledwindow	= nil;
	
	// create new requirement and new address
	if (frame.frame==nil or frame.ev==nil) {
		currProject = frame.currentProject;
		currPattern = currProject.currentPattern;
		scrolledwindow	= currPattern.window;
		local scale			= scrolledwindow.scale;
		point    		= Utilities::devicetological(scrolledwindow, ev.getx(), ev.gety(), scale);
	}
	// not event handler but call from left up event
	else {
		currProject = frame.frame.currentProject;
		currPattern = currProject.currentPattern;
		scrolledwindow	= currPattern.window;
		local scale			= scrolledwindow.scale;
		point    		= Utilities::devicetological(scrolledwindow, frame.ev.getx(), frame.ev.gety(), scale);
	}
	constituents = currProject.currentPattern.constituents;
	constituent1 = frame.constituent1;
	constituent2 = frame.constituent2;
	// create line segment if it is valid
	// constituent2==nil means create new requirement
	if (constituent2==nil and (constituent1.class=="Operation" or constituent1.class=="Role")) {
		// create new requirement and address
		requirement = constituents.addRequirement(currProject.properties, point);
		address = constituents.addAddress(currProject.properties, requirement.id, constituent1.id, constituent1.class, requirement.elementUI.centralPoint, constituent1.elementUI.centralPoint);
		//
		history = [];
		history[0] = [{#obj:"Constituents"},{#method:"removeAddressRequirement"},{#args:[{0:address.id},{1:requirement.id},{2:currProject}]}];
		index = 0;
		// all items except new one must be not selected
		foreach (item, constituents.selectedItems) {
			constituents[item.type][item.id].selected = false;
			history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
			history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{#method:"push_back"},{#args:[{0:item}]}];
		}
		constituents.undoStack.push_back(history);
		// new action by user means clear redo history
		constituents.redoStack.clear();
		constituents.selectedItems.clear();
		constituents.selectedItems.push_back([{"type":requirement.class},{"id":requirement.id}]);
		requirement.selected = true;
		constituents.priorityStack.push_front([{"type":requirement.class},{"id":requirement.id}]);
		scrolledwindow.refresh();
		Dialogs::Requirement(requirement, currProject);
	}
	// mouse caught first one rquirement and then one operation or role
	else
	if (constituent1.class=="Requirement" and (constituent2.class=="Operation" or constituent2.class=="Role")) {
		//check for unique
		unique = true;
		foreach (addr, constituents[#Address]) {
			if (constituent1.id==addr.reqId and constituent2.class==addr.addrEl.type and constituent2.id==addr.addrEl.id)
				{ unique = false; break; }
		}
		if (not unique) {
			mesgdlg =wx::messagedialog_construct(nil, "You have already define this address!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else {
			address = constituents.addAddress (currProject.properties, constituent1.id, constituent2.id, constituent2.class, constituent1.elementUI.centralPoint, constituent2.elementUI.centralPoint);
			history = [];
			history[0] = [{#obj:"Constituents"},{#method:"removeAddress"},{#args:[{0:address.id},{1:currProject}]}];
			index = 0;
			// deselect all selected items
			foreach (item, constituents.selectedItems) {
				constituents[item.type][item.id].selected = false;
				history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
				history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
			}
			constituents.selectedItems.clear();
			//
			constituents.undoStack.push_back(history);
			// new user action clears the redo history
			constituents.redoStack.clear();
			scrolledwindow.refresh();
		}
	}
	// mouse caught first operation/role and secondly requirement
	else
	if (constituent2.class=="Requirement" and (constituent1.class=="Operation" or constituent1.class=="Role")) {
		//check for unique
		unique = true;
		foreach (addr, constituents[#Address]) {
			if (constituent2.id==addr.reqId and constituent1.class==addr.addrEl.type and constituent1.id==addr.addrEl.id)
				{ unique = false; break; }
		}
		if (not unique) {
			mesgdlg =wx::messagedialog_construct(nil, "You have already define this address!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else {
			address = constituents.addAddress (currProject.properties, constituent2.id, constituent1.id, constituent1.class, constituent2.elementUI.centralPoint, constituent1.elementUI.centralPoint);
			history = [];
			history[0] = [{#obj:"Constituents"},{#method:"removeAddress"},{#args:[{0:address.id},{1:currProject}]}];
			index = 0;
			// deselect all selected items
			foreach (item, constituents.selectedItems) {
				constituents[item.type][item.id].selected = false;
				history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
				history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{#method:"push_back"},{#args:[{0:item}]}];
			}
			constituents.selectedItems.clear();
			//
			constituents.undoStack.push_back(history);
			// new user action clears the redo history
			constituents.redoStack.clear();
			scrolledwindow.refresh();
		}
	}
	// invalid try to create line segment
	else {
		mesgdlg =wx::messagedialog_construct(nil, "You must Address Requirement with Operation or Role", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
		scrolledwindow.refresh();
	}
}

// create one address and one operation or one role
function onCreateAddressConstituent(frame, ev) {
	type = ev.type;
	assert(frame.constituent1.class=="Requirement");
	//
	currProject		= frame.frame.currentProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	point    		= Utilities::devicetological(scrolledwindow, frame.ev.getx(), frame.ev.gety(), scale);
	constituent1 = frame.constituent1;
	constituents = currProject.currentPattern.constituents;
	// create new constituent type=role/operation and new address
	constituent = constituents["add"+type](currProject.properties, point);
	address = constituents.addAddress(currProject.properties, constituent1.id, constituent.id, constituent.class, constituent1.elementUI.centralPoint, constituent.elementUI.centralPoint);
	//
	history = [];
	history[0] = [{#obj:"Constituents"},{#method:("removeAddress"+type)},{#args:[{0:address.id},{1:constituent.id},{2:currProject}]}];
	index = 0;
	// clear all selected items
	foreach (item, constituents.selectedItems) {
		constituents[item.type][item.id].selected = false;
		history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
		history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{#method:"push_back"},{#args:[{0:item}]}];
	}
	constituents.selectedItems.clear();
	//
	constituents.undoStack.push_back(history);
	constituents.redoStack.clear();
	// only new one constituent is selected
	constituents.selectedItems.push_back([{"type":constituent.class},{"id":constituent.id}]);
	constituents.priorityStack.push_front([{"type": constituent.class},{"id":constituent.id}]);
	scrolledwindow.refresh();
	if (constituent.class=="Role") {
		// update treeview if constituent is role
		TreeView::addChildPattern(currProject, constituent, frame.frame.wxparent);
		AddrFuncs::Dialogs(type) (constituent, currProject, frame.frame.wxparent);
	}
	else
		AddrFuncs::Dialogs(type) (constituent, currProject);
}
function onCreateAddressOp   (frame, ev) { ev.type = #Operation;::onCreateAddressConstituent(frame, ev); }
function onCreateAddressRole (frame, ev) { ev.type = #Role;		::onCreateAddressConstituent(frame, ev); }

function onCreateUse (frame, ev) {
	point			= ev.getposition();
	currProject		= frame.currentProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	constituents	= currProject.currentPattern.constituents;
	constituent1	= frame.constituent1;
	constituent2	= frame.constituent2;
	// create line segment if it is valid
	if ((constituent1.class=="Operation" and constituent2.class=="Input") or
	    (constituent1.class=="Role" and (constituent2.class=="Operation" or constituent2.class=="Output"))
	) {
		// check validation
		unique = true;
		// case in offer means invalid try user to fix it as use too
		isonoffer = false;
		if (constituent1.class=="Operation") {
			foreach (ui, constituents[#Use])
				if (ui.element1.id==constituent1.id and ui.element2.id==constituent2.id)
					{ unique = false; break; }
		}
		else {
			foreach (ui, constituents[#Use])
				if (ui.element1.id==constituent1.id and ui.element2.type==constituent2.class and ui.element2.id==constituent2.id)
					{ unique = false; break; }
			// it can't be onoffer if not unique
			if (unique==true) {
				if (constituent2.class=="Operation") {
					foreach (roleoffer, constituent2.roleOffer)
						if (roleoffer==constituent1.id)
							isonoffer = true;
				}
				else {
					foreach (roleid, constituent2.roles)
						if (roleid==constituent1.id)
							isonoffer = true;
				}
			}
		}
		if (not unique) {
			mesgdlg =wx::messagedialog_construct(nil, "You have define this use!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else
		if (isonoffer) {
			mesgdlg =wx::messagedialog_construct(nil, "There is offer about this Constituents.\n You can not define and use!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else {
			use = constituents.addUse (currProject.properties, constituent1.id, constituent2.id, constituent1.class, constituent2.class, constituent1.elementUI.centralPoint, constituent2.elementUI.centralPoint);
			//
			history = [];
			history[0] = [{#obj:"Constituents"},{#method:"removeUse"},{#args:[{0:use.id},{1:currProject}]}];
			index = 0;
			// all items deselected
			foreach (item, constituents.selectedItems) {
				constituents[item.type][item.id].selected = false;
				history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
				history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
			}
			constituents.selectedItems.clear();
			constituents.undoStack.push_back(history);
			// new action by user means clear redo stack
			constituents.redoStack.clear();
			scrolledwindow.refresh();
		}
	}
	else
	if ((constituent2.class=="Operation" and constituent1.class=="Input") or
	    (constituent2.class=="Role" and (constituent1.class=="Operation" or constituent1.class=="Output"))
	) {
		// check validation
		unique = true;
		isonoffer = false;
		if (constituent2.class=="Operation") {
			foreach (ui, constituents[#Use])
				if (ui.element1.id==constituent2.id and ui.element2.id==constituent1.id)
					{ unique = false; break; }
		}
		else {
			foreach (ui, constituents[#Use])
				if (ui.element1.id==constituent2.id and ui.element2.type==constituent1.class and ui.element2.id==constituent1.id)
					{ unique = false; break; }
			// it can't be onoffer if not unique
			if (unique==true) {
				if (constituent1.class=="Operation") {
					foreach (roleoffer, constituent1.roleOffer)
						if (roleoffer==constituent2.id)
							isonoffer = true;
				}
				else {
					foreach (roleid, constituent1.roles)
						if (roleid==constituent2.id)
							isonoffer = true;
				}
			}
		}
		if (not unique) {
			mesgdlg =wx::messagedialog_construct(nil, "You have already define this use!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else
		if (isonoffer) {
			mesgdlg =wx::messagedialog_construct(nil, "There is offer about this Constituents.\n You can not define use!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else {
			use = constituents.addUse (currProject.properties, constituent2.id, constituent1.id, constituent2.class, constituent1.class, constituent2.elementUI.centralPoint, constituent1.elementUI.centralPoint);
			//
			history = [];
			history[0] = [{#obj:"Constituents"},{#method:"removeUse"},{#args:[{0:use.id},{1:currProject}]}];
			index = 0;
			foreach (item, constituents.selectedItems) {
				constituents[item.type][item.id].selected = false;
				history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
				history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
			}
			constituents.selectedItems.clear();
			constituents.undoStack.push_back(history);
			constituents.redoStack.clear();
			scrolledwindow.refresh();
		}
	}
	// invalid try to create line segment
	else {
		mesgdlg =wx::messagedialog_construct(nil, "You must Use Operation with Input or Role with Operation,Output!", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
		scrolledwindow.refresh();
	}
}

function onCreateUseRole (frame, ev) {
	currProject		= frame.frame.currentProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	point    		= Utilities::devicetological(scrolledwindow, frame.ev.getx(), frame.ev.gety(), scale);
	constituents	= currProject.currentPattern.constituents;
	constituent1	= frame.constituent1;
	//
	if (constituent1.class=="Operation" or constituent1.class=="Output") {
		role = constituents.addRole (currProject.properties, point);
		use = constituents.addUse (currProject.properties, role.id, constituent1.id, role.class, constituent1.class, role.elementUI.centralPoint, constituent1.elementUI.centralPoint);
		//
		history = [];
		history[0] = [{#obj:"Constituents"},{#method:"removeUseRole"},  {#args:[{0:use.id},{1:role.id},{2:currProject}]}];
		index = 0;
		foreach (item, constituents.selectedItems) {
			constituents[item.type][item.id].selected = false;
			history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
			history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{#method:"push_back"},{#args:[{0:item}]}];
		}
		constituents.selectedItems.clear();
		constituents.selectedItems.push_back([{"type":role.class}, {"id":role.id}]);
		role.selected = true;
		//
		constituents.undoStack.push_back(history);
		constituents.redoStack.clear();
		constituents.priorityStack.push_front([{"type":role.class}, {"id":role.id}]);
		scrolledwindow.refresh();
		TreeView::addChildPattern (currProject,role,frame.frame.wxparent);
		Dialogs::Role(role, currProject, frame.frame.wxparent);
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "You must Address Requirement with Operation or Role", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
		scrolledwindow.refresh();
	}
}

function onCreateUseInput (frame, ev) {
	currProject		= frame.frame.currentProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	point    		= Utilities::devicetological(scrolledwindow, frame.ev.getx(), frame.ev.gety(), scale);
	constituents	= currProject.currentPattern.constituents;
	constituent1	= frame.constituent1;
	//
	if (constituent1.class=="Operation") {
		input = constituents.addInput (currProject.properties, point);
		use = constituents.addUse (currProject.properties, constituent1.id, input.id, constituent1.class, input.class, constituent1.elementUI.centralPoint, input.elementUI.centralPoint);
		//
		history = [];
		history[0] = [{#obj:"Constituents"},{#method:"removeUseInput"}, {#args:[{0:use.id},{1:input.id},{2:currProject}]}];
		index = 0;
		foreach (item, constituents.selectedItems) {
			constituents[item.type][item.id].selected = false;
			history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
			history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{#method:"push_back"},{#args:[{0:item}]}];
		}
		constituents.selectedItems.clear();
		constituents.selectedItems.push_back([{"type":input.class}, {"id":input.id}]);
		input.selected = true;
		//
		constituents.undoStack.push_back(history);
		constituents.redoStack.clear();
		constituents.priorityStack.push_front([{"type":input.class}, {"id":input.id}]);
		scrolledwindow.refresh();
		Dialogs::Io(input, currProject);
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "You must Address Requirement with Operation or Role", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
		scrolledwindow.refresh();
	}
}

function onCreateUseOp (frame, ev) {
	currProject		= frame.frame.currentProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	point    		= Utilities::devicetological(scrolledwindow, frame.ev.getx(), frame.ev.gety(), scale);
	constituents	= currProject.currentPattern.constituents;
	constituent1	= frame.constituent1;
	//
	if (constituent1.class=="Role") {
		operation = constituents.addOperation (currProject.properties, point);
		use = constituents.addUse (currProject.properties, constituent1.id, operation.id, constituent1.class, operation.class, constituent1.elementUI.centralPoint, operation.elementUI.centralPoint);
		//
		history = [];
		history[0] = [{#obj:"Constituents"},{"method":"removeUseOperation"},{#args:[{0:use.id},{1:operation.id},{2:currProject}]}];
		index = 0;
		foreach (item, constituents.selectedItems) {
			constituents[item.type][item.id].selected = false;
			history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
			history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
		}
		constituents.selectedItems.clear();
		constituents.selectedItems.push_back([{"type":operation.class}, {"id":operation.id}]);
		operation.selected = true;
		//
		constituents.undoStack.push_back(history);
		constituents.redoStack.clear();
		constituents.priorityStack.push_front([{"type":operation.class}, {"id":operation.id}]);
		scrolledwindow.refresh();
		Dialogs::Operation(operation, currProject);
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "You must Address Requirement with Operation or Role", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
		scrolledwindow.refresh();
	}
}

function onCreateUseOutput (frame, ev) {
	currProject = frame.frame.currentProject;
	currPattern = currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	point    		= Utilities::devicetological(scrolledwindow, frame.ev.getx(), frame.ev.gety(), scale);
	constituent1 = frame.constituent1;
	constituents = currProject.currentPattern.constituents;
	if (constituent1.class=="Role") {
		output = constituents.addOutput (currProject.properties, point);
		use = constituents.addUse (currProject.properties, constituent1.id, output.id, constituent1.class, output.class, constituent1.elementUI.centralPoint, output.elementUI.centralPoint);
		//
		history = [];
		history[0] = [{#obj:"Constituents"},{"method":"removeUseOutput"},{#args:[{0:use.id},{1:output.id},{2:currProject}]}];
		index = 0;
		foreach (item, constituents.selectedItems) {
			constituents[item.type][item.id].selected = false;
			history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
			history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
		}
		constituents.selectedItems.clear();
		constituents.selectedItems.push_back([{"type":output.class}, {"id":output.id}]);
		output.selected = true;
		//
		constituents.undoStack.push_back(history);
		constituents.redoStack.clear();
		constituents.priorityStack.push_front([{"type":output.class}, {"id":output.id}]);
		scrolledwindow.refresh();
		Dialogs::Io(output, currProject);
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "You must Address Requirement with Operation or Role", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
		scrolledwindow.refresh();
	}
}

function onCreateOffer (frame, ev) {
	point = ev.getposition();
	currProject = frame.currentProject;
	currPattern = currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	constituents = currProject.currentPattern.constituents;
	constituent1 = frame.constituent1;
	constituent2 = frame.constituent2;
	if ((constituent1.class=="Role" and constituent2.class=="Operation") or
		(constituent1.class=="Operation" and constituent2.class=="Output")
	) {
/*		// check validation
		unique = true;
		isonoffer = false;
		foreach (ui, constituents[#Offer])
			if (ui.element1.type==constituent1.class and ui.element1.id==constituent1.id and ui.element2.type==constituent2.class and ui.element2.id==constituent2.id)
				{ unique = false; break; }
		// it can't be onoffer if not unique
		if (unique==true) {
			if (constituent2.class=="Operation")
				isonoffer = (constituent2.roleOffer.total()>0);
			else
				isonoffer = (constituent2.operation!=nil);
		}
		if (not unique) {
			mesgdlg =wx::messagedialog_construct(nil, "You have already define this offer!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else
		if (isonoffer) {
			mesgdlg =wx::messagedialog_construct(nil, "You have already define offer!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else {*/
			offer = constituents.addOffer (currProject.properties, constituent1.id, constituent2.id, constituent1.class, constituent2.class, constituent1.elementUI.centralPoint, constituent2.elementUI.centralPoint);
			//
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"removeOffer"},{#args:[{0:offer.id},{1:currProject}]}];
			index = 0;
			foreach (item, constituents.selectedItems) {
				constituents[item.type][item.id].selected = false;
				history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
				history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
			}
			constituents.selectedItems.clear();
			constituents.undoStack.push_back(history);
			constituents.redoStack.clear();
			scrolledwindow.refresh();
//		}
	}
	else
	if ((constituent2.class=="Role" and constituent1.class=="Operation") or
		(constituent2.class=="Operation" and constituent1.class=="Output")
	) {
/*		// check validation
		unique = true;
		isonoffer = false;
		foreach (ui, constituents[#Offer])
			if (ui.element1.type==constituent2.class and ui.element1.id==constituent2.id and ui.element2.type==constituent1.class and ui.element2.id==constituent1.id)
				{ unique = false; break; }
		// it can't be onoffer if not unique
		if (unique==true) {
			if (constituent1.class=="Operation")
				isonoffer = (constituent1.roleOffer.total()>0);
			else
				isonoffer = (constituent1.operation!=nil);
		}
		if (not unique) {
			mesgdlg =wx::messagedialog_construct(nil, "You have already define this offer!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else
		if (isonoffer) {
			mesgdlg =wx::messagedialog_construct(nil, "You have already define offer!", "Not allowed",
			wx::flags(wx::ICON_WARNING, wx::OK));
			mesgdlg.showmodal();
			scrolledwindow.refresh();
		}
		else {*/
			offer = constituents.addOffer (currProject.properties, constituent2.id, constituent1.id, constituent2.class, constituent1.class, constituent2.elementUI.centralPoint, constituent1.elementUI.centralPoint);
			//
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"removeOffer"},{#args:[{0:offer.id},{1:currProject}]}];
			index = 0;
			foreach (item, constituents.selectedItems) {
				constituents[item.type][item.id].selected = false;
				history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
				history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
			}
			constituents.selectedItems.clear();
			constituents.undoStack.push_back(history);
			constituents.redoStack.clear();
			scrolledwindow.refresh();
//		}
	}
	else {
		mesgdlg =wx::messagedialog_construct(nil, "You must Offer Role with Operation or Operation with Output", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
		scrolledwindow.refresh();
	}
}

function onCreateOfferConstituent (frame, ev) {
	type 			= ev.type;
	currProject 	= frame.frame.currentProject;
	currPattern 	= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	point    		= Utilities::devicetological(scrolledwindow, frame.ev.getx(), frame.ev.gety(), scale);
	constituents 	= currProject.currentPattern.constituents;
	constituent1 	= frame.constituent1;
	constituent 	= constituents["add"+type](currProject.properties, point);
	offer;
	if ((constituent1.class=="Role" and constituent.class=="Operation") or
		(constituent1.class=="Operation" and constituent.class=="Output")) {
		offer = constituents.addOffer (currProject.properties, constituent1.id, constituent.id, constituent1.class, constituent.class, constituent1.elementUI.centralPoint, constituent.elementUI.centralPoint);
	}
	else {
		offer = constituents.addOffer (currProject.properties, constituent.id, constituent1.id, constituent.class, constituent1.class, constituent.elementUI.centralPoint, constituent1.elementUI.centralPoint);
	}
	//
	history = [];
	history[0] = [{#obj:"Constituents"},{"method":("removeOffer"+type)},{#args:[{0:offer.id},{1:constituent.id},{2:currProject}]}];
	index = 0;
	foreach (item, constituents.selectedItems) {
		constituents[item.type][item.id].selected = false;
		history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
		history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
	}
	constituents.selectedItems.clear();
	constituents.selectedItems.push_back([{"type":constituent.class}, {"id":constituent.id}]);
	constituent.selected = true;
	//
	constituents.undoStack.push_back(history);
	constituents.redoStack.clear();
	constituents.priorityStack.push_front([{"type":constituent.class}, {"id":constituent.id}]);
	scrolledwindow.refresh();
	if (constituent.class=="Role") {
		TreeView::addChildPattern (currProject,constituent,frame.frame.wxparent);
		AddrFuncs::Dialogs(type) (constituent, currProject, frame.frame.wxparent);
	}
	else
	if (constituent.class=="Operation") {
		AddrFuncs::Dialogs(type) (constituent, currProject);
	}
}
function onCreateOfferRole	 (frame, ev) { ev.type = #Role;		 ::onCreateOfferConstituent(frame, ev); }
function onCreateOfferOp	 (frame, ev) { ev.type = #Operation; ::onCreateOfferConstituent(frame, ev); }
function onCreateOfferOutput (frame, ev) { ev.type = #Output;	 ::onCreateOfferConstituent(frame, ev); }

function onCreatePartOF(frame, ev) {
	local point				= nil;
	local currProject		= nil;
	local currPattern		= nil;
	local scrolledwindow	= nil;
	if (frame.frame==nil or frame.ev==nil) {
		point			= ev.getposition();
		currProject 	= frame.currentProject;
		currPattern 	= currProject.currentPattern;
		scrolledwindow	= currPattern.window;
	}
	else {
		currProject		= frame.frame.currentProject;
		currPattern		= currProject.currentPattern;
		scrolledwindow	= currPattern.window;
		scale			= scrolledwindow.scale;
		point    		= Utilities::devicetological(scrolledwindow, frame.ev.getx(), frame.ev.gety(), scale);
	}
	constituents = currProject.currentPattern.constituents;
	constituent1 = frame.constituent1;
	constituent2 = frame.constituent2;
	// create line segment if it is valid
	if (constituent2==nil) {
		type = (constituent1.class=="Role"? "Role":"Operation");
		constituent = constituents["add"+type](currProject.properties, point);
		partof = constituents.addPartOF(currProject.properties, constituent1.id, constituent.id, constituent1.class, constituent.class, constituent1.elementUI.centralPoint, constituent.elementUI.centralPoint);
		history = [];
		history[0] = [{#obj:"Constituents"},{"method":("removePartOF"+type)},{#args:[{0:partof.id},{1:constituent.id},{2:currProject}]}];
		index = 0;
		foreach (item, constituents.selectedItems) {
			constituents[item.type][item.id].selected = false;
			history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
			history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
		}
		constituents.selectedItems.clear();
		constituents.undoStack.push_back(history);
		constituents.redoStack.clear();
		constituents.selectedItems.push_back([{"type":constituent.class},{"id":constituent.id}]);
		constituent.selected = true;
		//
		constituents.priorityStack.push_front([{"type":constituent.class},{"id":constituent.id}]);
		scrolledwindow.refresh();
		if (type=="Role") {
			TreeView::addChildPattern (currProject,constituent,frame.frame.wxparent);
			AddrFuncs::Dialogs(type) (constituent, currProject, frame.frame.wxparent);
		}
		else {
			AddrFuncs::Dialogs(type) (constituent, currProject);
		}
	}
	else
	if ((constituent1.class=="Role" and constituent2.class=="Role") or
	    (constituent1.class=="Operation" and constituent2.class=="Operation")
	){
		partof = constituents.addPartOF (currProject.properties, constituent2.id, constituent1.id, constituent2.class, constituent1.class, constituent2.elementUI.centralPoint, constituent1.elementUI.centralPoint);
		history = [];
		history[0] = [{#obj:"Constituents"},{"method":("removePartOF")},{#args:[{0:partof.id},{1:currProject}]}];
		index = 0;
		foreach (item, constituents.selectedItems) {
			constituents[item.type][item.id].selected = false;
			history[++index] = [{#obj:item.type},{#id:item.id},{#data:"selected"},{#args:[{0:true}]}];
			history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
		}
		constituents.undoStack.push_back(history);
		constituents.redoStack.clear();
		constituents.selectedItems.clear();
		scrolledwindow.refresh();
	}
	// invalid try to create line segment
	else {
		mesgdlg =wx::messagedialog_construct(nil, "Invalid types for to be partof!", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
		scrolledwindow.refresh();
	}
}


////	Delete

function onDeleteConstituent (frame, ev) {
	currProject		= frame.currProject;
	currPattern 	= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	constituents	= currProject.currentPattern.constituents;
	constituent		= frame.constituent;
	//
	if (constituent.class=="Role")
		TreeView::removeChildPattern(currProject,constituent,frame.window.wxparent);
	methodName = "remove"+constituent.class;
	// keep history
	history = [];
	history[0] = constituents["history_"+methodName] (constituent.id, currProject);
	constituents.undoStack.push_back(history);
	// done
	constituents[methodName] (constituent.id, currProject);
	// user new action means clear redo stack
	if (std::isnil(frame.notredoclear))
		constituents.redoStack.clear();
	scrolledwindow.refresh();
}



////	Adds

function onAddInput (frame, ev) {}

function onAddOutput (frame, ev) {}


////	Un Decompose

function onDecompose (frame, ev) {}

function onUndecompose (frame, ev) {}


//// Role Split, Join

function onKeyDownDiag (frame, ev) {
	if (ev.getkeycode()==27)
		frame.destroy();
}

function onKeyDown (frame, ev) {
	if (ev.getkeycode()==27)
		frame.dialog.destroy();
}

function onCancelSplit (frame, ev) {
	frame.destroy();
}

function invalidMessage (message) {
	mesgdlg = wx::messagedialog_construct( nil, "Split can not completed:\n"+message, "Not Splitted",
					wx::flags(wx::ICON_ERROR, wx::OK));
	mesgdlg.showmodal();
	return;
}

function onSplit (frame, ev) {
	wxparent		= frame.wxparent;
	currProject		= frame.currProject;
	projprops		= currProject.properties;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	constituents	= currPattern.constituents;
	role			= frame.role;
	nroles			= frame.nroles.getvalue();
	//
	try {
		if (std::strisdigit(nroles)==true) {
			number	= std::strtonum(nroles);
			number	= std::integerpart(number);
			if (number>1 and number<9) {
				point		= wx::point_construct(role.elementUI.centralPoint.getx(), role.elementUI.centralPoint.gety());
				shortDesc	= role.shortDesc;
				roleId		= role.id;
				// create element for undo/redo
				element		= [];
				element.id			= roleId;
				element.shortDesc	= shortDesc;
				element.point		= point;
				element.description	= role.description;
				element.status		= role.status;
				element.priority	= role.priority;
				element.difficulty	= role.difficulty;
				element.author		= role.author;
				element.wxparent	= wxparent;
				//
				rolelements = [role.description, role.status, role.priority, role.difficulty, role.author];
				
				// keep action history for remove
				objAction = constituents.history_removeRole (role.id, currProject);
				// delete role
				index		= -1;
				connections = [];
				foreach (addr, constituents.Address)
					if (addr.addrEl.id==roleId and addr.addrEl.type=="Role") {
						connections[++index] = [{"method":"addAddress"},	{#args:[{0:projprops},{1:addr.reqId},{2:addr.addrEl.id},{3:addr.addrEl.type},{4:addr.segmentUI.point1},{5:addr.segmentUI.point2},{6:addr.id}]}];
						connections[++index] = [{"method":"setElSegment"},	{#args:[{0:addr.description}]}];
					}
				foreach (use, constituents.Use)
					if (use.element1.id==roleId and use.element1.type=="Role") {
						connections[++index] = [{"method":"addUse"},		{#args:[{0:projprops},{1:use.element1.id},{2:use.element2.id},{3:use.element1.type},{4:use.element2.type},{5:use.segmentUI.point1},{6:use.segmentUI.point2},{7:use.id}]}];
						connections[++index] = [{"method":"setElSegment"},	{#args:[{0:use.description}]}];
					}
				foreach (offer, constituents.Offer)
					if (offer.element1.id==roleId and offer.element1.type=="Role") {
						connections[++index] = [{"method":"addOffer"},		{#args:[{0:projprops},{1:offer.element1.id},{2:offer.element2.id},{3:offer.element1.type},{4:offer.element2.type},{5:offer.segmentUI.point1},{6:offer.segmentUI.point2},{7:offer.id}]}];
						connections[++index] = [{"method":"setElSegment"},	{#args:[{0:offer.description}]}];
					}
				element.connections = connections;
				TreeView::removeChildPattern(currProject, role, wxparent);
				constituents.removeRole (role.id, currProject);
				//
				rolesId	= [];
				for(i=0;i<number;++i) {
					newrole	= constituents.addRole (currProject.properties, point);
					newrole.setElement (((i+1)+". "+shortDesc), |rolelements|);
					rolesId[i] = newrole.id;
					// deselect all items that are selected and keep as selected only the new one constituent
					foreach (sel, constituents.selectedItems)
						constituents[sel.type][sel.id].selected = false;
					constituents.selectedItems.clear();
					constituents.selectedItems.push_back([{"type":newrole.class},{"id":newrole.id}]);
					newrole.selected = true;
					// new item must be first in the priority stack
					constituents.priorityStack.push_front([{"type":newrole.class},{"id":newrole.id}]);
					// update treeview
					TreeView::addChildPattern(currProject, newrole, wxparent);
					AddrFuncs::Dialogs(#Role) (newrole, currProject, wxparent);
					//
					point += wx::point_construct(10,5);
				}
				// keep history
				history		= [];
				history[0]	= [{#obj:"Constituents"},{#method:"undoSplitRole"},{#args:[{0:rolesId},{1:element},{2:currProject}]}];
				//
				constituents.undoStack.push_back(history);
				// new action by user clear redo stack
				constituents.redoStack.clear();
				//
				scrolledwindow.refresh();
			}
			else {
				::invalidMessage("Number of splitted roles out of bounds!\nNumber must be between 2 and 8!");
				return;
			}
		}
		else {
			::invalidMessage("Invalid number for splitted roles is: "+nroles+"!");
			return;
		}
	}
	trap e {
		::invalidMessage("Invalid number for splitted roles is: "+nroles+"!");
		return;
	}
	frame.destroy();
}

function onSplitRole (frame, ev) {
	wxparent    	= frame.wxparent;
	currProject 	= frame.currProject;
	currPattern		= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	role        	= frame.constituent;
	assert(role.class==#Role);
	//
	dialog = wx::dialog_construct(nil, wx::ID_ANY, "Split Role named as "+role.shortDesc, wx::point_construct(0,0), wx::size_construct(200,145));
	dialog.centre();
	dialog.show(true);
	panel = wx::panel_construct(dialog, wx::ID_ANY, wx::point_construct(0,0), wx::size_construct(200, 145));
	//
	nroles = wx::statictext_construct(panel, wx::ID_ANY,"Number of Roles: ", wx::point_construct(25,35));
	dialog.nroles = wx::textctrl_construct(panel, 31, "2", wx::point_construct(120,32), wx::size_construct(30,18), wx::flags(wx::TE_RICH2,wx::TE_RIGHT));
	//
	local splitID = currProject.connManageId.getID();
	okButton = wx::button_construct(panel, splitID, "Split...", wx::point_construct(115,75), wx::size_construct(70, 25));
	local cancelID = currProject.connManageId.getID();
	cancelButton = wx::button_construct(panel, cancelID, "Cancel", wx::point_construct(40,75), wx::size_construct(70, 25));
	//
	dialog.wxparent		= wxparent;
	dialog.currProject	= currProject;
	dialog.role			= role;
	dialog.connect(splitID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onSplit);
	dialog.connect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancelSplit);
	dialog.connect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.dialog = dialog;
	panel.connect(wx::EVT_KEY_DOWN, ::onKeyDown);
	dialog.showmodal();
	dialog.disconnect(splitID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onSplit);
	dialog.disconnect(cancelID, wx::EVT_COMMAND_BUTTON_CLICKED, ::onCancelSplit);
	dialog.disconnect(wx::EVT_KEY_DOWN, ::onKeyDownDiag);
	panel.disconnect(wx::EVT_KEY_DOWN, ::onKeyDown);
	//
	currProject.connManageId.removeIDs([splitID, cancelID]);
}

function onJoinRole (frame, ev) {
	index	= -1;
	items	= [];
	//
	currProject		= frame.currProject;
	currPattern 	= currProject.currentPattern;
	scrolledwindow	= currPattern.window;
	//
	point = nil;
	if (frame.redo==true)
		point	= frame.point;
	else
		point	= frame.ev.getposition();
	//
	frame			= frame.window;
	constituents	= currProject.currentPattern.constituents;
	// create the joined role
	role = constituents.addRole (currProject.properties, point);
	// set items in the role
	shortDesc	= "";
	description	= "";
	foreach (item, constituents.selectedItems) {
		currRole = constituents[item.type][item.id];
		//history currRole deletion
		items[++index] = constituents.history_removeRole(currRole.id, currProject);
		if (shortDesc!="")
			shortDesc += "__";
		if (description!="")
			description += "__";
		shortDesc	+= currRole.shortDesc;
		description	+= currRole.description;
		// TODO parent more than one have to keep one
		if (currRole.parent!=nil)
			role.parent = currRole.parent;
		// add all subroles which there is already in new role
		foreach (srole, currRole.subRoles) {
			unique = true;
			// check if there is in the new role
			foreach (snrole, role.subRoles)
				if (snrole==srole) { unique = false; break; }
			if (unique)
				role.subRoles.push_back(srole);
		}
		// add Addresses and set requirement with the new role id
		foreach (req, currRole.addrReqs)
			foreach (addr, constituents["Address"])
				if (addr.reqId==req and addr.addrEl.type=="Role" and addr.addrEl.id==currRole.id) {
					unique = true;
					foreach (tmp, role.addrReqs)
						if (tmp==req)
							{ unique = false; break; }
					// set the roleID in the role if is unique
					if (unique) {
						addr.addrEl.id = role.id;
						constituents["Requirement"][addr.reqId].addrRoles.remove(currRole.id);
						constituents["Requirement"][addr.reqId].addrRoles.push_back(role.id);
						addr.segmentUI.setSegment(role.elementUI.centralPoint, 2);
						role.addrReqs.push_back(req);
					}
					// if not unique just delete addr which odd
					else {
						constituents.removeAddress(addr.id,currProject,true);
					}
					break;
				}
		// add Operations Offered and set the 
		foreach (op, currRole.opsOffered)
			foreach (offer, constituents["Offer"])
				if (offer.element1.type=="Role" and offer.element1.id==currRole.id) {
					unique = true;
					foreach (tmp, role.opsOffered)
						if (tmp==op)
							{ unique = false; break; }
					if (unique) {
						offer.element1.id = role.id;
						constituents["Operation"][offer.element2.id].roleOffer.remove(currRole.id);
						constituents["Operation"][offer.element2.id].roleOffer.push_back(role.id);
						offer.segmentUI.setSegment(role.elementUI.centralPoint, 1);
						role.opsOffered.push_back(op);
					}
					else {
						constituents.removeOffer(offer.id,currProject,true);
					}
					break;
				}
		foreach (op, currRole.opsUsed)
			foreach (use, constituents["Use"])
				if (use.element1.type=="Role" and use.element1.id==currRole.id and use.element2.type=="Operation") {
					unique = true;
					foreach (tmp, role.opsUsed)
						if (tmp==use)
							{ unique = false; break; }
					notOffer = true;
					foreach (tmp, role.opsOffered)
						if (tmp==op)
							{ notOffer = false; break; }
					if (unique and notOffer) {
						use.element1.id = role.id;
						constituents["Operation"][use.element2.id].rolesUsed.remove(currRole.id);
						constituents["Operation"][use.element2.id].rolesUsed.push_back(role.id);
						use.segmentUI.setSegment(role.elementUI.centralPoint, 1);
						role.opsUsed.push_back(op);
					}
					else {
						constituents.removeUse(use.id,currProject,true);
					}
					break;
				}
		foreach (ou, currRole.outputsUsed)
			foreach (use, constituents["Use"])
				if (use.element1.type=="Role" and use.element1.id==currRole.id and use.element2.type=="Output") {
					unique = true;
					foreach (tmp, role.outputsUsed)
						if (tmp==use)
							{ unique = false; break; }
					if (unique) {
						use.element1.id = role.id;
						constituents["Output"][use.element2.id].roles.remove(currRole.id);
						constituents["Output"][use.element2.id].roles.push_back(role.id);
						use.segmentUI.setSegment(role.elementUI.centralPoint, 1);
						role.outputsUsed.push_back(ou);
					}
					else {
						constituents.removeUse(use.id,currProject,true);
					}
					break;
				}
		foreach (in, currRole.inputsUsed)
			foreach (use, constituents["Use"])
				if (use.element2.type=="Role" and use.element2.id==currRole.id and use.element1.type=="Input") {
					unique = true;
					foreach (tmp, role.inputsUsed)
						if (tmp==use)
							{ unique = false; break; }
					if (unique) {
						use.element2.id = role.id;
						constituents["Input"][use.element2.id].roles.remove(currRole.id);
						constituents["Input"][use.element2.id].roles.push_back(role.id);
						use.segmentUI.setSegment(role.elementUI.centralPoint, 2);
						role.inputsUsed.push_back(in);
					}
					else {
						constituents.removeUse(use.id,currProject,true);
					}
					break;
				}
	}
	role.shortDesc = shortDesc;
	role.description = description;
	// remove roles
	foreach (item, constituents.selectedItems) {
		foreach (pstack, constituents.priorityStack)
			// remove from priority stack the role
			if (pstack.type==item.type and pstack.id==item.id) {
				constituents.priorityStack.remove(pstack);
				break; }
		TreeView::removeChildPattern (currProject,constituents[item.type][item.id],frame.wxparent);
		// remove role from selected items
		// free id
		constituents.idManage.removeID("Role", item.id);
		constituents[item.type][item.id] = nil;
	}
	constituents.selectedItems.clear();
	constituents.selectedItems.push_back([{"type":role.class},{"id":role.id}]);
	role.selected = true;
	//
	if (std::isnil(frame.redo)) {
		element	= [];
		element.wxparent	= frame.wxparent;
		element.point		= point;
		element.items		= items;
		history		= [];
		history[0]	= [{#obj:"Constituents"},{#method:"undoJoinRole"},{#args:[{0:role.id},{1:element},{2:currProject}]}];
		constituents.undoStack.push_back(history);
		constituents.redoStack.clear();
	}
	
	//
	constituents.priorityStack.push_front([{"type":role.class},{"id":role.id}]);
	scrolledwindow.refresh();
	TreeView::addChildPattern (currProject,role,frame.wxparent);
	Dialogs::Role(role,currProject,frame.wxparent);
}


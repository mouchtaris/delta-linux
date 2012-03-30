using #ShapeRAD;
using #ToolBarRAD;
using #ConstituentsEvents;
using #PatternsEvents;
using #ImageHolder;
using #Dialogs;
using #Utilities;
using #AddrFuncs;


/////// Constants & Globals
//
// the start point from "big" rectangle which is mold 
selectionAreaPt = nil;
// the state to decide action for shape: ["move","resize"]
StateSelected   = #none;
// the direction of mouse resize
Direction       = #none;
// keep the height and width that sub,add from event.point
// to find the currentPoint of shape after the transportation
difSize = [ @w : -1, @h : -1 ];
// constituent which mouse caught
hittedConstituent = nil;
// keep first constituent of segment line
// and know the state on creation one segment between
// two shapes
segmentConstituent = nil;
// refresh in motion
motion = 0;

//
// fix the mouse when the mouse enters in action area window
function onEnterWindow (frame, ev) {
	toolChoice = ToolBarRAD::GetToolChoice();
	//
	if (toolChoice==#none) {
		local cursor = wx::cursor_construct(wx::CURSOR_NONE);
		frame.setcursor(cursor);
		cursor.destruct();
	}
	else
	// show element mouse
	if (std::strsub(toolChoice,"shape_")>=0) {
		local cursor = wx::cursor_construct(ImageHolder::images().addElement);
		frame.setcursor(cursor);
		cursor.destruct();
	}
	// show segment mouse
	else {
		if (toolChoice=="segment_Address") {
			local cursor = wx::cursor_construct(ImageHolder::images().addAddressSegment1st);
			frame.setcursor(cursor);
			cursor.destruct();
		}
		else
		if (toolChoice=="segment_Use") {
			local cursor = wx::cursor_construct(ImageHolder::images().addUseSegment1st);
			frame.setcursor(cursor);
			cursor.destruct();
		}
		else
		if (toolChoice=="segment_Offer") {
			local cursor = wx::cursor_construct(ImageHolder::images().addOfferSegment1st);
			frame.setcursor(cursor);
			cursor.destruct();
		}
		else
		if (toolChoice=="segment_PartOF") {
			local cursor = wx::cursor_construct(ImageHolder::images().addPartOFSegment1st);
			frame.setcursor(cursor);
			cursor.destruct();
		}
	}
}

// open constituent window to view/edit properties and notes of constituent
function onDClickLeft (frame, ev) {
	wxparent		= frame.wxparent;
	currProject		= frame.currentProject;
	currPattern		= currProject.currentPattern;
	//
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	currPoint 		= Utilities::devicetological(scrolledwindow, ev.getx(), ev.gety(), scale);
	//
	if ((constituent = ShapeRAD::shapefirstHit(currPoint, currProject))!=nil) {
		if (constituent.class=="Role")
			AddrFuncs::Dialogs(constituent.class) (constituent, currProject, wxparent);
		else
		if (constituent.superclass=="Shape")
			AddrFuncs::Dialogs(constituent.class) (constituent, currProject);
		else
			AddrFuncs::Dialogs(#Relation) (constituent, currProject);
	}
}

function onLeftDown (frame, ev) {
	// restart segment constituent
	::segmentConstituent	= nil;
	currProject  			= frame.currentProject;
	currPattern  			= currProject.currentPattern;
	constituents 			= currPattern.constituents; 
	toolChoice   			= ToolBarRAD::GetToolChoice();
	//
	scrolledwindow			= currPattern.window;
	scale					= scrolledwindow.scale;
	currPoint				= Utilities::devicetological(scrolledwindow, ev.getx(), ev.gety(), scale);
	
	// Tool is selected from ToolBarRAD
	if (toolChoice!=#none) {
		if (std::strsub(toolChoice,"shape_")>=0) {
			// set the mouse in normal mode
			local cursor = wx::cursor_construct(wx::CURSOR_NONE);
			frame.setcursor(cursor);
			cursor.destruct();
			// set toolbar in none mode
			ToolBarRAD::SetToolChoice("none");
			// create the corresponding constituent
			// cut and keep the type of creation
			methodName = "onCreate"+std::strtokenise(toolChoice,"_").pop_back();
			// call function
			AddrFuncs::ConstituentsEvents(methodName)(frame, ev);
		}
		// start one new segment creation
		else if (std::strsub(toolChoice,"segment_")>=0) {
			constituent = ShapeRAD::shapeHit(currPoint, currProject, true);
			// to create new segment mouse must press on a constituent and this constituent must be Shape_t
			if (constituent!=nil and constituent.superclass=="Shape") {
				// keeps the first of two constituents in the new segment
				::segmentConstituent = constituent;
				// decide the mouse view
				cursor = nil;
				if (toolChoice=="segment_Address")
					cursor = wx::cursor_construct(ImageHolder::images().addAddressSegment2nd);
				else
				if (toolChoice=="segment_Use")
					cursor = wx::cursor_construct(ImageHolder::images().addUseSegment2nd);
				else
				if (toolChoice=="segment_Offer")
					cursor = wx::cursor_construct(ImageHolder::images().addOfferSegment2nd);
				else
				if (toolChoice=="segment_PartOF")
					cursor = wx::cursor_construct(ImageHolder::images().addOfferSegment2nd);
				else
					assert(false);
				// fix the mouse view
				frame.setcursor(cursor);
				cursor.destruct();
			}
			// if mouse click on empty area without shape...
			else {
				::segmentConstituent = nil;
				ToolBarRAD::SetToolChoice(#none);
				frame.setcursor(wx::cursor_construct(wx::CURSOR_NONE));
			}
		}
		else {
			// invalid toolChoice: must start with shape_ or segment_
			assert(false);
		}
	}
	else
	// choose for remove or move, one more than one shapes
	if (ev.controldown()) {
		if ((constituent = ShapeRAD::shapeHit(currPoint, currProject,true)) != nil) {
			// already selected means delected
			if (constituent.selected) {
				constituent.selected = false;
				foreach (item, constituents.selectedItems)
					if (item.type==constituent.class and item.id==constituent.id)
						{ constituents.selectedItems.remove(item); break; }
			}
			else {
				constituent.selected = true;
				constituents.selectedItems.push_back([@type : constituent.class, @id : constituent.id]);
				// remove it from the priority list to add it top of priority stack
				foreach (item ,constituents.priorityStack)
					if (item.type==constituent.class and item.id==constituent.id) {
						constituents.priorityStack.remove(item);
						break; }
				constituents.priorityStack.push_front([@type : constituent.class, @id : constituent.id]);
			}
			/* 
			//maybe unnecessary this part of code
			::difSize.point = wx::point_construct(-1,-1);
			::difSize.w = -1;
			::difSize.h = -1;*/
			::hittedConstituent = nil;
			scrolledwindow.refresh();
		}
	}
	else
	if ( ( constituent = ShapeRAD::shapeHit(currPoint, currProject, true) )!=nil) {
		if (constituent.superclass=="Shape") {
			// click on not selected shape
			if (not constituent.selected) {
				// set this shape as the only selected
				foreach (elem, constituents.selectedItems)
					constituents[elem.type][elem.id].selected = false;
				constituents.selectedItems.clear();
				constituent.selected = true;
				constituents.selectedItems.push_back([ {"type" : constituent.class}, {"id" : constituent.id} ]);
			}
			// set this item as first in priority stack
			foreach (item, constituents.priorityStack)
				if (item.type==constituent.class and item.id==constituent.id) {
					constituents.priorityStack.remove(item);
					break; }
			constituents.priorityStack.push_front([ {"type" : constituent.class}, {"id" : constituent.id} ]);
			// keep the dif coordinates that mouse hit the shape
			// in case drag to know the position that shape must be after move
			::difSize.point = wx::point_construct(currPoint.getx(), currPoint.gety());
			::difSize.w = currPoint.getx() - constituent.elementUI.x;
			::difSize.h = constituent.elementUI.y - currPoint.gety();
			::hittedConstituent = constituent;
			// fix the mouse view to be in drag mode
			local cursor = wx::cursor_construct(wx::CURSOR_SIZING);
			frame.setcursor(cursor);
			cursor.destruct();
			scrolledwindow.refresh();
		}
		else {
			// looking only for shapes in function shapeHit(currPoint, currProject, true)
			// means that can't return other except shape
			assert(false);
		}
	}
	// click the mouse in empty area maybe try to create "big" rectangle
	// begin to mold a rectangle in action area for choose shapes for moving all together
	// restart the point which had keeped for the shape
	// TODO "big" rectangle maybe not need for selectionAreaPt in application because of ctrl enable
	else {
		foreach (elem, constituents.selectedItems)
			constituents[elem.type][elem.id].selected = false;
		constituents.selectedItems.clear();
		::selectionAreaPt = currPoint;
		::difSize.point = wx::point_construct(-1,-1);
		::difSize.w = -1;
		::difSize.h = -1;
		::hittedConstituent = nil;
		scrolledwindow.refresh();
	}
}

function onLeftUp (frame, ev) {
	currProject		= frame.currentProject;
	currPattern		= currProject.currentPattern;
	constituents	= currPattern.constituents;
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	currPoint		= Utilities::devicetological(scrolledwindow, ev.getx(), ev.gety(), scale);
	//
	// on up event there is no shape hitted for dragged
	// in this case drag shape finish and keep history for undo
	if (::hittedConstituent!=nil) {
		// 
		startX	= ::difSize.point.getx() - ::difSize.w;
		startY	= ::difSize.point.gety() + ::difSize.h;
		assert(startX!=-1 and startY!=-1 and ::difSize.w!=-1 and ::difSize.h!=-1);
		// find the end point that shape moved
		local x	= ev.getx() - ::difSize.w;
		local y = ev.gety() + ::difSize.h;
		constituent = constituents[::hittedConstituent.class][::hittedConstituent.id];
		if (constituent.superclass=="Shape") {
			history	= [];
			index	= -1;
			// find the difference about the start point of the move and the end point
			difX	= x - startX;
			difY	= y - startY;
			foreach (item, constituents.selectedItems) {
				currconstituent	= constituents[item.type][item.id];
				// keep in history the point
				nx	= currconstituent.elementUI.x - difX;
				ny	= currconstituent.elementUI.y - difY;
				w	= currconstituent.elementUI.width;
				h	= currconstituent.elementUI.height;
				// the central point of the shape to set it in segments
				cpoint	= wx::point_construct(nx+w/2,ny+h/2);
				// set in history the shape point
				history[++index] = [{#obj:item.type},{#id:item.id},{#data:"elementUI"},{"method":"setPoint"},{#args:[{0:nx},{1:ny}]}];
				if (currconstituent.class=="Requirement") {
					addresses = constituents[#Address];
					foreach (address, addresses)
						if (address.reqId==currconstituent.id)
							history[++index] = [{#obj:address.class},{#id:address.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:1}]}];
				}
				else
				if (currconstituent.class=="Operation") {
					addresses = constituents[#Address];
					foreach (address, addresses)
						if (address.addrEl.id==currconstituent.id and address.addrEl.type=="Operation")
							history[++index] = [{#obj:address.class},{#id:address.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:2}]}];
					uses = constituents[#Use];
					foreach (use, uses) {
						if (use.element1.id==currconstituent.id and use.element1.type=="Operation")
							history[++index] = [{#obj:use.class},{#id:use.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:1}]}];
						else
						if (use.element2.id==currconstituent.id and use.element2.type=="Operation")
							history[++index] = [{#obj:use.class},{#id:use.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:2}]}];
					}
					offers = constituents[#Offer];
					foreach (offer, offers)
						if (offer.element1.id==currconstituent.id and offer.element1.type=="Operation")
							history[++index] = [{#obj:offer.class},{#id:offer.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:1}]}];
						else
						if (offer.element2.id==currconstituent.id and offer.element2.type=="Operation")
							history[++index] = [{#obj:offer.class},{#id:offer.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:2}]}];
					inhers = constituents[#PartOF];
					foreach (inh, inhers)
						if (inh.element.type=="Operation" and inh.element.id==currconstituent.id)
							history[++index] = [{#obj:inh.class},{#id:inh.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:1}]}];
						else
						if (inh.subelement.type=="Operation" and inh.subelement.id==currconstituent.id)
							history[++index] = [{#obj:inh.class},{#id:inh.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:2}]}];
				}
				else
				if (currconstituent.class=="Role") {
					addresses = constituents[#Address];
					foreach (address, addresses)
						if (address.addrEl.id==currconstituent.id and address.addrEl.type=="Role")
							history[++index] = [{#obj:address.class},{#id:address.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:2}]}];
					uses = constituents[#Use];
					foreach (use, uses)
						if (use.element1.id==currconstituent.id and use.element1.type=="Role")
							history[++index] = [{#obj:use.class},{#id:use.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:1}]}];
					offers = constituents[#Offer];
					foreach (offer, offers)
						if (offer.element1.id==currconstituent.id and offer.element1.type=="Role")
							history[++index] = [{#obj:offer.class},{#id:offer.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:1}]}];
					inhers = constituents[#PartOF];
					foreach (inh, inhers)
						if (inh.element.type=="Role" and inh.element.id==currconstituent.id)
							history[++index] = [{#obj:inh.class},{#id:inh.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:1}]}];
						else
						if (inh.subelement.type=="Role" and inh.subelement.id==currconstituent.id)
							history[++index] = [{#obj:inh.class},{#id:inh.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:2}]}];
				}
				else
				if (currconstituent.class=="Output") {
					uses = constituents[#Use];
					foreach (use, uses)
						if (use.element2.id==currconstituent.id and use.element2.type=="Output")
							history[++index] = [{#obj:use.class},{#id:use.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:2}]}];
					offers = constituents[#Offer];
					foreach (offer, offers)
						if (offer.element2.id==currconstituent.id and offer.element2.type=="Output")
							history[++index] = [{#obj:offer.class},{#id:offer.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:2}]}];
				}
				else
				if (currconstituent.class=="Input") {
					uses = constituents[#Use];
					foreach (use, uses)
						if (use.element2.id==currconstituent.id and use.element2.type=="Input")
							history[++index] = [{#obj:use.class},{#id:use.id},{#data:"segmentUI"},{"method":"setSegment"},{#args:[{0:cpoint},{1:2}]}];
				}
			}
			constituents.undoStack.push_back(history);
			constituents.redoStack.clear();
		}
		//
		::difSize.point		= wx::point_construct(-1,-1);
		::difSize.w			= -1;
		::difSize.h			= -1;
		::hittedConstituent	= nil;
		scrolledwindow.refresh();
	}
	else
	// case to create a segment if user has toolchoice or restart
	if (::segmentConstituent!=nil) {
		// tool choice not segment has nothing to create and then restart segmentConstituent
		if(not((std::strsub(toolChoice = ToolBarRAD::GetToolChoice(),"segment_"))>=0)) {
			::segmentConstituent=nil;
			scrolledwindow.refresh();
		}
		else
		// left up mouse on a shape means try to create a segment with segmentConstituent and this shape-constituent
		if ((constituent = ShapeRAD::shapeHit(currPoint, currProject, true))!=nil) {
			local cursor = wx::cursor_construct(wx::CURSOR_NONE);
			frame.setcursor(cursor);
			cursor.destruct();
			ToolBarRAD::SetToolChoice("none");
			frame.constituent1		= ::segmentConstituent;
			::segmentConstituent	= nil;
			frame.constituent2		= constituent;
			// cut and keep the type of creation
			methodName	= "onCreate"+std::strtokenise(toolChoice,"_").pop_back();
			// call function
			AddrFuncs::ConstituentsEvents(methodName)(frame, ev);
		}
		else {
			// left up and mouse is not on a shape so 
			// choice to create new shape and line segment
			frame.setcursor(wx::cursor_construct(wx::CURSOR_NONE));
			toolChoice = ToolBarRAD::GetToolChoice();
			if (toolChoice=="segment_Address") {
				if (::segmentConstituent.class=="Requirement") {
					wxmenu				= wx::menu_construct();
					wxmenu.ev			= ev;
					wxmenu.frame		= frame;
					wxmenu.constituent1	= ::segmentConstituent;
					::segmentConstituent= nil;
					opID 				= currProject.connManageId.getID();
					menuitem = wx::menuitem_construct(wxmenu, opID, "Operation");
					menuitem.setbitmap(ImageHolder::images().toolOperation);
					wxmenu.append(menuitem);
					roleID		= currProject.connManageId.getID();
					menuitem	= wx::menuitem_construct(wxmenu, roleID, "Role");
					menuitem.setbitmap(ImageHolder::images().toolRole);
					wxmenu.append(menuitem);
					wxmenu.connect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateAddressOp);
					wxmenu.connect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateAddressRole);
					frame.popupmenu(wxmenu, ev.getposition());
					//
					wxmenu.disconnect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateAddressOp);
					wxmenu.disconnect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateAddressRole);
					// free id
					ids	= [opID,roleID];
					currProject.connManageId.removeIDs(ids);
					scrolledwindow.refresh();
				}
				else
				if (::segmentConstituent.class=="Role" or ::segmentConstituent.class=="Operation") {
					wxmenu = wx::menu_construct();
					wxmenu.ev			= ev;
					wxmenu.frame		= frame;
					wxmenu.constituent1 = ::segmentConstituent;
					::segmentConstituent= nil;
					reqID 				= currProject.connManageId.getID();
					menuitem = wx::menuitem_construct(wxmenu, reqID, "Requirement");
					menuitem.setbitmap(ImageHolder::images().toolRequirement);
					wxmenu.append(menuitem);
					wxmenu.connect(reqID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateAddress);
					frame.popupmenu(wxmenu, ev.getposition());
					// free id
					ids = [reqID];
					currProject.connManageId.removeIDs(ids);
					scrolledwindow.refresh();
				}
				else {
					scrolledwindow.refresh();
				}
			}
			else
			if (toolChoice=="segment_Use") {
				if (::segmentConstituent.class=="Operation") {
					// menu choice for create new Role, Input
					// help explain
					wxmenu = wx::menu_construct();
					wxmenu.ev			= ev;
					wxmenu.frame		= frame;
					wxmenu.constituent1 = ::segmentConstituent;
					::segmentConstituent = nil;
					roleID = currProject.connManageId.getID();
					inputID = currProject.connManageId.getID();
					menuitem = wx::menuitem_construct(wxmenu, roleID, "Role");
					menuitem.setbitmap(ImageHolder::images().toolRole);
					wxmenu.append(menuitem);
					menuitem = wx::menuitem_construct(wxmenu, inputID, "Input");
					menuitem.setbitmap(ImageHolder::images().toolInput);
					wxmenu.append(menuitem);
					wxmenu.connect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseRole);
					wxmenu.connect(inputID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseInput);
					frame.popupmenu(wxmenu, ev.getposition());
					// free id
					wxmenu.disconnect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseRole);
					wxmenu.disconnect(inputID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseInput);
					ids = [roleID,inputID];
					currProject.connManageId.removeIDs(ids);
					scrolledwindow.refresh();
				}
				else
				if (::segmentConstituent.class=="Role") {
					wxmenu = wx::menu_construct();
					wxmenu.ev			= ev;
					wxmenu.frame		= frame;
					wxmenu.constituent1 = ::segmentConstituent;
					::segmentConstituent = nil;
					opID = currProject.connManageId.getID();
					outputID = currProject.connManageId.getID();
					menuitem = wx::menuitem_construct(wxmenu, opID, "Operation");
					menuitem.setbitmap(ImageHolder::images().toolOperation);
					wxmenu.append(menuitem);
					menuitem = wx::menuitem_construct(wxmenu, outputID, "Output");
					menuitem.setbitmap(ImageHolder::images().toolOutput);
					wxmenu.append(menuitem);
					wxmenu.connect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseOp);
					wxmenu.connect(outputID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseOutput);
					frame.popupmenu(wxmenu, ev.getposition());
					// free id
					wxmenu.disconnect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseOp);
					wxmenu.disconnect(outputID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseOutput);
					ids = [opID, outputID];
					currProject.connManageId.removeIDs(ids);
					scrolledwindow.refresh();
				}
				else
				if (::segmentConstituent.class=="Output") {
					// menuchoice for create new Role
					wxmenu = wx::menu_construct();
					wxmenu.ev			= ev;
					wxmenu.frame		= frame;
					wxmenu.constituent1 = ::segmentConstituent;
					::segmentConstituent = nil;
					roleID = currProject.connManageId.getID();
					opID   = currProject.connManageId.getID();
					menuitem = wx::menuitem_construct(wxmenu, roleID, "Role");
					menuitem.setbitmap(ImageHolder::images().toolRole);
					wxmenu.append(menuitem);
					menuitem = wx::menuitem_construct(wxmenu, opID, "Operation");
					menuitem.setbitmap(ImageHolder::images().toolOperation);
					wxmenu.append(menuitem);
					wxmenu.connect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseRole);
					wxmenu.connect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseOp);
					frame.popupmenu(wxmenu, ev.getposition());
					ids = [opID,roleID];
					currProject.connManageId.removeIDs(ids);
					wxmenu.disconnect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseRole);
					wxmenu.disconnect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseOp);
					wxmenu.constituent1 = nil;
					wxmenu.destruct();
					scrolledwindow.refresh();
				}
				else
				if (::segmentConstituent.class=="Input") {
					// menu choice for create new Operation
					wxmenu = wx::menu_construct();
					wxmenu.ev			= ev;
					wxmenu.frame		= frame;
					wxmenu.constituent1 = ::segmentConstituent;
					::segmentConstituent = nil;
					opID = currProject.connManageId.getID();
					menuitem = wx::menuitem_construct(wxmenu, opID, "Operation");
					menuitem.setbitmap(ImageHolder::images().toolOperation);
					wxmenu.append(menuitem);
					wxmenu.connect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseOp);
					frame.popupmenu(wxmenu, ev.getposition());
					currProject.connManageId.removeID(opID);
					wxmenu.disconnect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateUseOp);
					wxmenu.constituent1 = nil;
					wxmenu.destruct();
					scrolledwindow.refresh();
				}
				else {
					scrolledwindow.refresh();
				}
			}
			else
			if (toolChoice=="segment_Offer") {
				if (::segmentConstituent.class=="Role") {
					wxmenu = wx::menu_construct();
					wxmenu.ev			= ev;
					wxmenu.frame		= frame;
					wxmenu.constituent1 = ::segmentConstituent;
					::segmentConstituent = nil;
					opID = currProject.connManageId.getID();
					menuitem = wx::menuitem_construct(wxmenu, opID, "Operation");
					menuitem.setbitmap(ImageHolder::images().toolOperation);
					wxmenu.append(menuitem);
					wxmenu.connect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOfferOp);
					frame.popupmenu(wxmenu, ev.getposition());
					currProject.connManageId.removeID(opID);
					wxmenu.disconnect(opID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOfferOp);
					wxmenu.constituent1 = nil;
					scrolledwindow.refresh();
				}
				else
				if (::segmentConstituent.class=="Operation") {
					wxmenu = wx::menu_construct();
					wxmenu.ev			= ev;
					wxmenu.frame		= frame;
					wxmenu.constituent1 = ::segmentConstituent;
					::segmentConstituent = nil;
					roleID = currProject.connManageId.getID();
					menuitem = wx::menuitem_construct(wxmenu, roleID, "Role");
					menuitem.setbitmap(ImageHolder::images().toolRole);
					wxmenu.append(menuitem);
					//
					outputID = currProject.connManageId.getID();
					menuitem = wx::menuitem_construct(wxmenu, outputID, "Output");
					menuitem.setbitmap(ImageHolder::images().toolOutput);
					wxmenu.append(menuitem);
					wxmenu.connect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOfferRole);
					wxmenu.connect(outputID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOfferOutput);
					frame.popupmenu(wxmenu, ev.getposition());
					currProject.connManageId.removeID(roleID);
					wxmenu.disconnect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOfferRole);
					wxmenu.disconnect(outputID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOfferOutput);
					wxmenu.constituent1 = nil;
					scrolledwindow.refresh();
				}
				else {
					scrolledwindow.refresh();
				}
			}
			else
			if (toolChoice=="segment_PartOF") {
				wxmenu = wx::menu_construct();
				wxmenu.ev			= ev;
				wxmenu.frame		= frame;
				wxmenu.constituent1 = ::segmentConstituent;
				::segmentConstituent = nil;
				roleID = currProject.connManageId.getID();
				menuitem = wx::menuitem_construct(wxmenu, roleID, wxmenu.constituent1.class);
				menuitem.setbitmap(ImageHolder::images()["tool"+wxmenu.constituent1.class]);
				wxmenu.append(menuitem);
				wxmenu.connect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreatePartOF);
				frame.popupmenu(wxmenu, ev.getposition());
				currProject.connManageId.removeID(roleID);
				wxmenu.disconnect(roleID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreatePartOF);
				wxmenu.constituent1 = nil;
				scrolledwindow.refresh();
			}
			ToolBarRAD::SetToolChoice(#none);
			ToolBarRAD::drawToolBox();
		}
	}
/*  NOT IMPEMENTED drag rectangle area...
	// select a lot shape by create a rectangle
	// and chooce all inside of it
	// problem in implementation maybe ok with ctrl case
	else
	if (::selectionAreaPt!=nil and ::selectionAreaPt!=wx::point_construct(0,0) and ::selectionAreaPt!=currPoint) {
			rect = wx::rect_construct(::selectionAreaPt, currPoint);
			constituents.selectedItems = ShapeRAD::shapeListHit(rect, currProject);
			foreach (element, constituents.selectedItems)
				constituents[element.type][element.id].selected = true;
			::selectionAreaPt = wx::point_construct(0,0);
	}
	// stop rectangle that has created
	else
	if (::selectionAreaPt!=nil and ::selectionAreaPt!=wx::point_construct(0,0)) {
		::selectionAreaPt = wx::point_construct(0,0);
	}*/
	// restart segment constituent
	::segmentConstituent = nil;
}

function onMotionEvent (frame, ev) {
	currProject		= frame.currentProject;
	currPattern		= currProject.currentPattern;
	constituents	= currPattern.constituents;
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	currPoint		= Utilities::devicetological(scrolledwindow, ev.getx(), ev.gety(), scale);
	//
	//dragging the mouse
	if (ev.leftisdown()) {
		// selection elements state
		if ( (toolChoice=ToolBarRAD::GetToolChoice())==#none ) {
			::segmentConstituent = nil;
			// dragging a constituent
			if (::hittedConstituent!=nil) {
				assert(::difSize.w!=-1 and difSize.h!=-1);
				local x	= currPoint.getx() - ::difSize.w * scrolledwindow.scale;
				local y	= currPoint.gety() + ::difSize.h * scrolledwindow.scale;
				//
				constituent = constituents[::hittedConstituent.class][::hittedConstituent.id];
				if (constituent.superclass=="Shape") {
					elPoint	= Utilities::devicetological(scrolledwindow, constituent.elementUI.x, constituent.elementUI.y, scale);
					difX	= x - elPoint.getx();
					difY	= y - elPoint.gety();
					// moving the shape, have to move the segment those contain this constituent
					// no keep history here, history must keep in the end o fthe moving in left up
					foreach (item, constituents.selectedItems) {
						currconstituent	= constituents[item.type][item.id];
						currElPoint	= Utilities::devicetological(scrolledwindow, currconstituent.elementUI.x, currconstituent.elementUI.y, scale);
						local nx	= currElPoint.getx() + difX;
						local ny	= currElPoint.gety() + difY;
						//
						currconstituent.elementUI.setPoint(nx,ny);
						if (currconstituent.class=="Requirement") {
							addresses = constituents[#Address];
							foreach (address, addresses)
								if (address.reqId==currconstituent.id)
									address.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 1);
						}
						else
						if (currconstituent.class=="Operation") {
							addresses = constituents[#Address];
							foreach (address, addresses)
								if (address.addrEl.id==currconstituent.id and address.addrEl.type=="Operation")
									address.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 2);
							uses = constituents[#Use];
							foreach (use, uses) {
								if (use.element1.id==currconstituent.id and use.element1.type=="Operation")
									use.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 1);
								else
								if (use.element2.id==currconstituent.id and use.element2.type=="Operation")
									use.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 2);
							}
							offers = constituents[#Offer];
							foreach (offer, offers)
								if (offer.element1.id==currconstituent.id and offer.element1.type=="Operation")
									offer.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 1);
								else
								if (offer.element2.id==currconstituent.id and offer.element2.type=="Operation")
									offer.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 2);
							inhers = constituents[#PartOF];
							foreach (inh, inhers)
								if (inh.element.type=="Operation" and inh.element.id==currconstituent.id)
									inh.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 1);
								else
								if (inh.subelement.type=="Operation" and inh.subelement.id==currconstituent.id)
									inh.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 2);
						}
						else
						if (currconstituent.class=="Role") {
							addresses = constituents[#Address];
							foreach (address, addresses)
								if (address.addrEl.id==currconstituent.id and address.addrEl.type=="Role")
									address.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 2);
							uses = constituents[#Use];
							foreach (use, uses)
								if (use.element1.id==currconstituent.id and use.element1.type=="Role")
									use.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 1);
							offers = constituents[#Offer];
							foreach (offer, offers)
								if (offer.element1.id==currconstituent.id and offer.element1.type=="Role")
									offer.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 1);
							inhers = constituents[#PartOF];
							foreach (inh, inhers)
								if (inh.element.type=="Role" and inh.element.id==currconstituent.id)
									inh.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 1);
								else
								if (inh.subelement.type=="Role" and inh.subelement.id==currconstituent.id)
									inh.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 2);
						}
						else
						if (currconstituent.class=="Output") {
							uses = constituents[#Use];
							foreach (use, uses)
								if (use.element2.id==currconstituent.id and use.element2.type=="Output")
									use.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 2);
							offers = constituents[#Offer];
							foreach (offer, offers)
								if (offer.element2.id==currconstituent.id and offer.element2.type=="Output")
									offer.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 2);
						}
						else
						if (currconstituent.class=="Input") {
							uses = constituents[#Use];
							foreach (use, uses)
								if (use.element2.id==currconstituent.id and use.element2.type=="Input")
									use.segmentUI.setSegment(currconstituent.elementUI.centralPoint, 2);
						}
					}
				}
				// scrolledwindow.refresh();
				ShapeRAD::redraw(currProject,true);
			}
			if ((constituent = ShapeRAD::shapeHit(currPoint, currProject,false))!=nil) {
				if (constituent.superclass=="Shape") {
					// change position
					// draw it as shadowed
					// draw only selected line of current position
					// set state ShapeIsDragged
					// and 
				}
				else {
					
				}
			}
			else
			if (::selectionAreaPt==nil or ::selectionAreaPt==wx::point_construct(0,0)) {
				::selectionAreaPt = ev.getposition();
			}
			else {
				// to find the point of tmprect get all cases
				// selectionAreaPt is the point, or the currPoint or other
				
				// draw the tmp rect
				// and draw as enableSelected the shapes
				// if tmprect contains the element and the line segment
				
				// redraw...
			}
		}
		else {
			// draw the line of the drag try user to create a segment at run time
			if (std::strsub(toolChoice ,"segment_")>=0) {
				// pin data for extra draw line and enable shape
				scrolledwindow.drawline	= [
					@point1			: ::segmentConstituent.elementUI.centralPoint,
					@point2			: currPoint,
					@constituent	: ::segmentConstituent
				];
				// if mouse over a shape, draw extra...
				if ((tmp=ShapeRAD::shapeHit(currPoint,currProject,true))!=nil and tmp!=::segmentConstituent)
					scrolledwindow.drawline.enable = tmp;
				// refresh
				scrolledwindow.refresh();
			}
		}
	}
	else
	if (constituents.selectedItems.total()>0 and ToolBarRAD::GetToolChoice()==#none) {
		selectedItems = constituents.selectedItems;
		foreach (item, selectedItems) {
			constituent = constituents[item.type][item.id];
			// check for constituent if mouse is on one of the resize rectangles and give the direction
			if (constituent.superclass=="Shape") {
				if (constituents.selectedItems.total()==1 and (direction=constituent.onResize(currPoint))!=false ) {
					if (direction=="up" or direction=="down")
						frame.setcursor(wx::cursor_construct(wx::CURSOR_SIZENS));
					else
					if (direction=="left" or direction=="right")
						frame.setcursor(wx::cursor_construct(wx::CURSOR_SIZEWE));
					else
					if (direction=="leftup" or direction=="rightdown")
						frame.setcursor(wx::cursor_construct(wx::CURSOR_SIZENWSE));
					else //if (direction=="rightup" or direction=="leftdown")
						frame.setcursor(wx::cursor_construct(wx::CURSOR_SIZENESW));
					::StateSelected = "resize";
					::Direction     = direction;
					// state = resize for selected item
					// when stop enable
					// when there is no selected
					// here in motion
				}
				else
				// check if mouse is on a shape to fix the stateselected and the mouse
				if ((currConstituent = ShapeRAD::shapeHit(currPoint, currProject,true))!=nil) {
					if (currConstituent.selected)
						frame.setcursor(wx::cursor_construct(wx::CURSOR_SIZING));
					::Direction = #none;
					::StateSelected = "move";
					// state move selected items
					// one is mouse move enable each time
					break;
				}
				else {
					local cursor = wx::cursor_construct(wx::CURSOR_NONE);
					frame.setcursor(cursor);
					cursor.destruct();
					::Direction = #none;
					::StateSelected = #none;
					// here stop mouse move-resize and states
				}
			}
			// no one of segment can't be selected so...
			// one line segment
			else {
				assert(false);
				// check for move mouse enable state
				// check for resize mouse enable state
			}
		}
	}
}

/* user pressed right click on a constituent */
function constituentRightClick (frame, constituent, currProject, currPoint) {
	wxmenu = wx::menu_construct();
	// pin data for connected functions
	wxmenu.currProject  = currProject;
	wxmenu.constituent	= constituent;
	wxmenu.window		= frame;
	wxmenu.wxparent		= frame.wxparent;
	// Constituents Elements
	if (constituent.superclass=="Shape") {
		if (constituent.class=="Requirement") {
			editID = currProject.connManageId.getID();
			wxmenu.append(editID, "Edit");
			wxmenu.connect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRequirement);
			deleteID = currProject.connManageId.getID();
			wxmenu.append(deleteID, "Delete");
			wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			//
			frame.popupmenu(wxmenu, currPoint);
			// free ids
			ids = [editID,deleteID];
			currProject.connManageId.removeIDs(ids);
		}
		else
		if (constituent.class=="Operation") {
			editID = currProject.connManageId.getID();
			wxmenu.append(editID, "Edit");
			wxmenu.connect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onOperation);
			subMenu = wx::menu_construct();
			inputID = currProject.connManageId.getID();
			subMenu.append(inputID,"Input");
			wxmenu.connect(inputID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onAddInput);
			outputID = currProject.connManageId.getID();
			subMenu.append(outputID, "Output");
			wxmenu.connect(outputID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onAddOutput);
			wxmenu.append(wx::ID_ANY, "Add", subMenu);
			undecomposeID = -1;
			if (constituent.subOperations.total()>0 or constituent.parent!=nil) {
				undecomposeID = currProject.connManageId.getID();
				wxmenu.append(undecomposeID, "UnDecompose");
				wxmenu.connect(undecomposeID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onUndecompose);
			}
			decomposeID = currProject.connManageId.getID();
			wxmenu.append(decomposeID, "Decompose");
			wxmenu.connect(decomposeID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDecompose);
			deleteID = currProject.connManageId.getID();
			wxmenu.append(deleteID,"Delete");
			wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			//
			frame.popupmenu(wxmenu, currPoint);
			// free ids
			if (undecomposeID ==-1) {
				ids = [editID,inputID,outputID,decomposeID,deleteID];				
				currProject.connManageId.removeIDs(ids); }
			else {
				ids = [editID,inputID,outputID,undecomposeID,decomposeID,deleteID];				
				currProject.connManageId.removeIDs(ids); }
		}
		else
		if (constituent.class=="Role") {
			editID = currProject.connManageId.getID();
			wxmenu.append(editID, "Edit");
			wxmenu.connect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRole);
			//
			splitID = currProject.connManageId.getID();
			wxmenu.append(splitID, "Split");
			wxmenu.connect(splitID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onSplitRole);
			//
			deleteID = currProject.connManageId.getID();
			wxmenu.append(deleteID,"Delete");
			wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			//
			frame.popupmenu(wxmenu, currPoint);
			// free ids
			ids = [editID,splitID,deleteID];				
			currProject.connManageId.removeIDs(ids);
		}
		else
		if (constituent.class=="Output") {
			editID = currProject.connManageId.getID();
			wxmenu.append(editID, "Edit");
			wxmenu.connect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onIo);
			deleteID = currProject.connManageId.getID();
			wxmenu.append(deleteID,"Delete");
			wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			//
			frame.popupmenu(wxmenu, currPoint);
			//
			wxmenu.disconnect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onIo);
			wxmenu.disconnect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			// free ids
			ids = [editID,deleteID];				
			currProject.connManageId.removeIDs(ids);
		}
		else
		if (constituent.class=="Input") {
			editID = currProject.connManageId.getID();
			wxmenu.append(editID, "Edit");
			wxmenu.connect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onIo);
			deleteID = currProject.connManageId.getID();
			wxmenu.append(deleteID,"Delete");
			wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			//
			frame.popupmenu(wxmenu, currPoint);
			//
			wxmenu.disconnect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onIo);
			wxmenu.disconnect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			// free ids
			ids = [editID,deleteID];				
			currProject.connManageId.removeIDs(ids);
		}
	}
	// Constituents Element Links (Line Segments)
	else {
//		editID = currProject.connManageId.getID();
//		wxmenu.append(editID, "Edit");
		deleteID = currProject.connManageId.getID();
		wxmenu.append(deleteID,"Delete");
// TODO
//		wxmenu.connect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRelation);
		wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
		frame.popupmenu(wxmenu, currPoint);
//		wxmenu.disconnect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRelation);
		wxmenu.disconnect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
		
/*		if (constituent.class=="Address") {
			wxmenu.connect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRelation);
			wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			frame.popupmenu(wxmenu, currPoint);
			wxmenu.disconnect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRelation);
			wxmenu.disconnect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
		}
		else
		if (constituent.class=="Use") {
			wxmenu.connect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRelation);
			wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			frame.popupmenu(wxmenu, currPoint);
			wxmenu.disconnect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRelation);
			wxmenu.disconnect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
		}
		else
		if (constituent.class=="Offer") {
			wxmenu.connect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRelation);
			wxmenu.connect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
			frame.popupmenu(wxmenu, currPoint);
			wxmenu.disconnect(editID, wx::EVT_COMMAND_MENU_SELECTED, Dialogs::onRelation);
			wxmenu.disconnect(deleteID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onDeleteConstituent);
		}
		else
			assert(false);
*/
		// free ids
		ids = [editID,deleteID];				
		currProject.connManageId.removeIDs(ids);
	}
}


function onRightDown (frame, ev) {
	currProject		= frame.currentProject;
	currPattern		= currProject.currentPattern;
	constituents	= currPattern.constituents;
	scrolledwindow	= currPattern.window;
	scale			= scrolledwindow.scale;
	currPoint		= Utilities::devicetological(scrolledwindow, ev.getx(), ev.gety(), scale);
	//
	wxparent    = frame.wxparent;
	frame.ev    = ev;
	// reset the hits of mouse events
	::difSize.point = wx::point_construct(-1,-1);
	::difSize.w = -1;
	::difSize.h = -1;
	::hittedConstituent = nil;
	::segmentConstituent = nil;
	//
	if (( constituent = ShapeRAD::shapefirstHit(currPoint, currProject) )!=nil) {
		// fix the selected items and not selected items before the right click
		if (constituents.selectedItems.total()==0) {
			if (constituent.superclass=="Shape") {
				// create the object for selected items list
				tmp = [{"type":constituent.class},{"id":constituent.id}];
				constituent.selected = true;
				constituents.selectedItems.push_back(tmp);
			}
			scrolledwindow.refresh();
			::constituentRightClick(frame, constituent, currProject, ev.getposition());
		}
		else {
			not_onSelectedItems = true;
			areAllRoles = true;
			foreach (item, constituents.selectedItems) {
				if (item.type==constituent.class and item.id==constituent.id)
					not_onSelectedItems = false;
				if (item.type!="Role")
					areAllRoles = false;
			}
			if (not_onSelectedItems) {
				foreach (elem, constituents.selectedItems)
					constituents[elem.type][elem.id].selected = false;
				constituents.selectedItems.clear();
				if (constituent.superclass=="Shape") {
					constituent.selected = true;
					constituents.selectedItems.push_back( [{"type":constituent.class},{"id":constituent.id}] );
				}
				scrolledwindow.refresh();
				::constituentRightClick(frame, constituent, currProject, ev.getposition());
			}
			else {
				if (areAllRoles and constituents.selectedItems.total()>=2) {
					wxmenu = wx::menu_construct();
					wxmenu.constituent	= constituent;
					wxmenu.window		= frame;
					wxmenu.wxparent     = wxparent;
					wxmenu.currProject	= currProject;
					wxmenu.ev			= ev;
					joinID = currProject.connManageId.getID();
					wxmenu.append(joinID, "Join");
					wxmenu.connect(joinID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onJoinRole);
					//
					frame.popupmenu(wxmenu, ev.getposition());
					// free id
					currProject.connManageId.removeID(joinID);
					wxmenu.disconnect(joinID, wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onJoinRole);
					wxmenu.constituent = nil;
					wxmenu.window      = nil;
					wxmenu.destruct();
				}
				else {
					foreach (item, constituents.selectedItems)
						constituents[item.type][item.id].selected = false;
					constituents.selectedItems.clear();
					if (constituent.superclass=="Shape") {
						constituent.selected = true;
						constituents.selectedItems.push_back([@type:constituent.class,@id:constituent.id]);
					}
					scrolledwindow.refresh();
					::constituentRightClick(frame, constituent, currProject, ev.getposition());
				}
			}
		}
	}
	// Right click in action area without item
	else {
		wxmenu = wx::menu_construct();
		// pin date in wxmenu
		wxmenu.window       = frame;
		wxmenu.wxparent     = wxparent;
		wxmenu.currProject	= currProject;
		wxmenu.ev			= ev;
		// ids of connections
		reqId = currProject.connManageId.getID();
		opId = currProject.connManageId.getID();
		roleId = currProject.connManageId.getID();
		inputId = currProject.connManageId.getID();
		outId = currProject.connManageId.getID();
		closeId = currProject.connManageId.getID();
		deleteId = currProject.connManageId.getID();
		// create menu
		subMenu = wx::menu_construct();
		subMenu.append(reqId, "Requirement");
		subMenu.append(opId, "Operation");
		subMenu.append(roleId, "Role");
		subMenu.append(inputId, "Input");
		subMenu.append(outId, "Output");
		wxmenu.append(wx::ID_ANY, "Create", subMenu);
		wxmenu.append(closeId, "Close");
		wxmenu.append(deleteId, "Delete");
		// connections of wxmenu
		wxmenu.connect(reqId,    wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateRequirement);
		wxmenu.connect(opId,     wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOperation);
		wxmenu.connect(roleId,   wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateRole);
		wxmenu.connect(inputId,  wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateInput);
		wxmenu.connect(outId,    wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOutput);
		wxmenu.connect(closeId,  wx::EVT_COMMAND_MENU_SELECTED, PatternsEvents::onClosePattern);
		wxmenu.connect(deleteId, wx::EVT_COMMAND_MENU_SELECTED, PatternsEvents::onDeletePatternOpened);
		// show menu
		frame.popupmenu(wxmenu, ev.getposition());
		// disconnect wxmenu
		wxmenu.disconnect(reqId,    wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateRequirement);
		wxmenu.disconnect(opId,     wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOperation);
		wxmenu.disconnect(roleId,   wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateRole);
		wxmenu.disconnect(inputId,  wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateInput);
		wxmenu.disconnect(outId,    wx::EVT_COMMAND_MENU_SELECTED, ConstituentsEvents::onCreateOutput);
		wxmenu.disconnect(closeId,  wx::EVT_COMMAND_MENU_SELECTED, PatternsEvents::onClosePattern);
		wxmenu.disconnect(deleteId, wx::EVT_COMMAND_MENU_SELECTED, PatternsEvents::onDeletePatternOpened);
		// free ids
		ids = [reqId,opId,roleId,inputId,outId,deleteId,closeId];
		currProject.connManageId.removeIDs(ids);
	}
}


function onKeyDown (frame, ev) {
	// user pressed delete and system has to remove the selected items
	if (ev.getkeycode()==127) {
		//check if currpage is project page
		if ( (frame.getpage(frame.getselection())).getid()==14 ) return;
		//
		currProject		= frame.currProject;
		currPattern		= currProject.currentPattern;
		scrolledwindow	= currPattern.window;
		constituents	= currPattern.constituents;
		//
		index		= -1;
		history		= [];
		
		foreach (selected, constituents.selectedItems) {
			constituent	= constituents[selected.type][selected.id];
			if (constituent.class=="Role")
				AddrFuncs::TreeView("removeChildPattern") (currProject,constituent, frame.window);
			//
			methodName = "remove"+constituent.class;
			history[++index] = constituents["history_"+methodName] (constituent.id, currProject);
			// delete constituent
			constituents[methodName] (constituent.id, currProject);
			// redraw
			scrolledwindow.refresh();
		}
		// write history in undo stack
		constituents.undoStack.push_back(history);
		// user new action means clear redo stack
		constituents.redoStack.clear();
		// fixing mouse in nope mode
		local cursor = wx::cursor_construct(wx::CURSOR_NONE);
		(frame.getpage(frame.getselection())).setcursor(cursor);
		cursor.destruct();
	}
}

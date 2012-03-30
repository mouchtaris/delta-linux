using #ConstituentsUI;
using #AddrFuncs;
using #IdManagement;

// Enums types
Constituent_t	= [ {0:"Requirement"}, {1:"Operation"}, {2:"Role"}, {3:"Input"}, {4:"Output"}, {5:"Address"}, {6:"Use"}, {7:"Offer"}, {8:"PartOF"} ];
function GetConstituent_t { return ::Constituent_t; }
Shape_t = [ "Requirement", "Operation", "Role", "Input", "Output" ];
function GetShape_t { return ::Shape_t; }
Segment_t = [ "Address", "Use", "Offer", "PartOF" ];
function GetSegment_t { return ::Segment_t; }

Status_t		= [ "Approved", "Implemented", "Mandatory", "Proposed", "Validated" ];
function GetStatus_t { return ::Status_t; }
Level_t			= [ "LOW", "MEDIUM", "HIGH" ];
function GetLevel_t { return ::Level_t; }
Requirement_t	= [ "Architectural", "Structural", "Behavioral", "Functional", "Non-functional" ];
function GetRequirement_t { return ::Requirement_t; }



// Const & Glbs types
const nl = "\n";

// user keep notes,files,weblinks etc on each of elements
function NoteBook {

}

function sElement (id, ui, shortDesc, description, created, lastUpdate) {
	return [
		@superclass		: #Shape,
		@class			: #sElement,
		@id				: id,
		@elementUI		: ui,
		@selected		: true,
		@shortDesc		: shortDesc,
		@description	: description,
		@created		: created,
		@lastUpdate		: lastUpdate,
		@noteBook		: ::NoteBook(),
		method setSElement (shortDesc, description) {
			@shortDesc = shortDesc;
			@description = description;
		},
		method setElementUI (elemUI)
			{ self.elementUI = elemUI; },
		method isOrphan	{ assert(false); },
		method drawElement (dc) {
			if (self.selected) {
				self.elementUI.drawSelected(dc);
			}
			else
			if (self.isOrphan()) {
				self.elementUI.drawHighlight(dc);
			}
			// draw the shape
			self.elementUI.drawShape(dc, self.selected);
			// draw short description
			self.elementUI.drawText(dc, self.shortDesc);
		},
		method drawEnable (dc) {
			self.elementUI.drawEnable(dc);
		},
		method onCollisionRect (rect) {
			intersect = self.elementUI.intersect(rect);
			return (intersect.getheight()!=0 and intersect.getwidth()!=0);
		},
		// element: point or rect
		method onCollision (element) {
			return self.elementUI.contains(element);
		},
		method onResize (point) {
			// values: false, #up, #down, #left, #right, #leftup, #leftdown, #rightup, #rightdown
			return self.elementUI.onRects(point);
		},
		method savexmlSElement (obj) {
			obj.id = @id;
			obj.selected = (@selected==true?"true":"false");
			obj.shortDesc = @shortDesc;
			obj.description = @description;
			obj.created    = [];
			obj.created[0] = [
				{#year	: self.created.getyear()},
				{#month	: self.created.getmonth()},
				{#day	: self.created.getday()}
			];
			obj.lastUpdate = [];
			obj.lastUpdate[0] = [
				{#year	: self.lastUpdate.getyear()},
				{#month	: self.lastUpdate.getmonth()},
				{#day	: self.lastUpdate.getday()}
			];
			obj.elementUI = [];
			obj.elementUI[0] = [];
			@elementUI.savexml(obj.elementUI[0]);
		}
	];
}

function Element (id, ui, shortDesc, description, status, priority, difficulty, author, created, lastUpdate) {
	return [
		std::inherit(@self, ::sElement(id, ui, shortDesc, description, created, lastUpdate)),
		@class			: #Element,
		@status			: status,
		@priority		: priority,
		@difficulty		: difficulty,
		@author			: author,
		//
		method setElement (shortDesc, description, status, priority, difficulty, author) {
			self.setSElement(shortDesc, description);
			@status = status;
			@priority = priority;
			@difficulty = difficulty;
			@author = author;
		},
		method savexmlElement (obj) {
			self.savexmlSElement(obj);
			obj.status   = @status;
			obj.priority = @priority;
			obj.difficulty = @difficulty;
			obj.author = @author;
		}
	];
}

/* Requirement */
function Requirement (id, projProps, point, shortDesc, description, type, status, priority, difficulty, author) {
	// decide the requirement UI
//	if (projProps.type==#Default)
	requirementUI = ConstituentsUI::Rectangle (point.getx()-projProps.requirement.size.getwidth()/2,point.gety()-projProps.requirement.size.getheight()/2,projProps.requirement.size.getwidth(),projProps.requirement.size.getheight());
	//else
	//	other shape
	return [
		@class 			: #Requirement,
		@type			: type,
		std::inherit( @self, ::Element(id, requirementUI, shortDesc, description, status, priority, difficulty, author, wx::datetime_now(), wx::datetime_now())),
		// me poia ops kai roles syndeetai to requirement auto
		// endexomenoi periorismoi panw sto posa kai poia ops h role 8a exei synde8ei
		// logika mono me ena role 8a mporei na synde8ei
		@addrOps		: std::list_new(),
		@addrRoles		: std::list_new(), /* an 8eloume ena requirement na vrisketai se panw apo ena rolous */
		
		method setRequirement (shortdesc, desc, type, status, priority, difficulty, author) {
			@type = type;
			self.setElement(shortdesc, desc, status, priority, difficulty, author);
		},
		method addAddrOp	(opID)		{ @addrOps.push_back(opID);		},
		method setConnRole	(roleID)	{ @addrRoles.push_back(roleID); },
		method unsetConnRole(roleID)	{ @addrRoles.remove(roleID);	},
		//
		method isOrphan
			{ return (@addrOps.total()==0 and @addrRoles.total()==0); },
		method isOrphanWarning // an den einai symplirwmeno kapoio apo ta desc kai shdesc
			{ return false; },
		method savexmlRequirement (obj) {
			self.savexmlElement (obj);
			obj.type =	@type;
			obj.addrOps = [];
			index = -1;
			foreach (item, @addrOps) obj.addrOps[++index] = [@id:item];
			obj.addrRoles = [];
			index = -1;
			foreach (item, @addrRoles) obj.addrRoles[++index] = [@id:item];
		}
	];
}

/* Operation */

// the 9th argument is the roleId which offers the operation
function Operation (id, projProps, point, shortDesc, description, status, priority, difficulty, author) {
	// decide the operation UI
//	if (projProps.type==#Default)
	operationUI = ConstituentsUI::Rectangular (point.getx()-projProps.operation.size.getwidth()/2,point.gety()-projProps.operation.size.getheight()/2,projProps.operation.size.getwidth(),projProps.operation.size.getheight());
	//else
	//	other shape
	
	return [
		@class			: #Operation,
		std::inherit(@self, ::Element( id, operationUI, shortDesc, description, status, priority, difficulty, author, wx::datetime_now(), wx::datetime_now())),
		//
//		@parent		    : undefined,
		@subOperations	: std::list_new(),
		// connected Elems
		@reqsAddr		: std::list_new(),
		@rolesUsed		: std::list_new(),
		@outputs		: std::list_new(),
		@inputs			: std::list_new(),
		@roleOffer		: std::list_new(), //only one role must offer one operation
		
		method addAddrRequirement	(   reqID)	{ @reqsAddr.push_back(reqID);	},
		method addRoleUseIt			(  roleID)	{ @rolesUsed.push_back(roleID);	},
		method addOutput			(outputID)	{ @outputs.push_back(outputID);	},
		method addInput				( inputID)	{ @inputs.push_back(inputID);	},
		method addRoleOfferIt		(  roleID)	{ @roleOffer.push_back(roleID);	},
		// orphan if nobody offer it or nobody use it (roles).
		method isOrphan {
			return (@roleOffer.total()==0 or @rolesUsed.total()==0);
		},
		method isOrphanWarning {
			return false;
		},
		method savexmlOperation (obj) {
			self.savexmlElement (obj);
			if (@parent!=nil)
				obj.parent = @parent;
			obj.subOperations = [];
			index = -1;
			foreach (sop, @subOperations) obj.subOperations[++index] = [@id:sop];
			obj.reqsAddr = [];
			index = -1;
			foreach (ra, @reqsAddr) obj.reqsAddr[++index] = [@id:ra];
			obj.rolesUsed = [];
			index = -1;
			foreach (ru, @rolesUsed) obj.rolesUsed[++index] = [@id:ru];
			obj.outputs = [];
			index = -1;
			foreach (ou, @outputs) obj.outputs[++index] = [@id:ou];
			obj.inputs = [];
			index = -1;
			foreach (in, @inputs) obj.inputs[++index] = [@id:in];
			obj.roleOffer = [];
			index = -1;
			foreach (in, @roleOffer) obj.roleOffer[++index] = [@id:in];
		}
	];
}

/* Role */

function Role (id, projProps, point, shortDesc, description, status, priority, difficulty, author, version) {
	// decide the operation UI
//	if (projProps.type==#Default)
	roleUI = ConstituentsUI::Ellipse (point.getx()-projProps.role.size.getwidth()/2,point.gety()-projProps.role.size.getheight()/2,projProps.role.size.getwidth(),projProps.role.size.getheight());
	//else
	//	other shape
	
	return [
		@class			: #Role,
		std::inherit(@self, ::Element(id, roleUI, shortDesc, description, status, priority, difficulty, author, wx::datetime_now(), wx::datetime_now())),
		//
		@version		: version,
		// 
//		@parent		    : undefined,
		@subRoles		: std::list_new(),
		//
		@addrReqs		: std::list_new(),
		@opsOffered		: std::list_new(),
		@opsUsed		: std::list_new(),
		@outputsUsed	: std::list_new(),
		@inputsUsed		: std::list_new(),
		//
		method addConnRequirement	(requirementID)	{ @addrReqs.push_back(requirementID);	},
		method addOfferOp			(operationID)	{ @opsOffered.push_back(operationID);	},
		method addUsedOp			(operationID)	{ @opsUsed.push_back(operationID);		},
		method addUsedOutput		(outputID)		{ @outputsUsed.push_back(outputID);		},
		method addUsedInput			(inputID)		{ @inputsUsed.push_back(inputID);		},
		//
		method isOrphan {
				return (@opsOffered.total()==0 and @opsUsed.total()==0);
		},
		method isOrphanWarning {
				return false;
		},
		method savexmlRole (obj) {
			self.savexmlElement (obj);
			obj.version = [];
			obj.version["$Attributes"] = [];
			obj.version["$Attributes"].value = @version;
			if (@parent!=nil)
 				obj.parent = @parent;
			obj.subRoles = [];
			index = -1;
			foreach (item, @subRoles) obj.subRoles[++index] = [@id:item];
			obj.addrReqs = [];
			index = -1;
			foreach (item, @addrReqs) obj.addrReqs[++index] = [@id:item];
			obj.opsOffered = [];
			index = -1;
			foreach (item, @opsOffered) obj.opsOffered[++index] = [@id:item];
			obj.opsUsed = [];
			index = -1;
			foreach (item, @opsUsed) obj.opsUsed[++index] = [@id:item];
			obj.outputsUsed = [];
			index = -1;
			foreach (item, @outputsUsed) obj.outputsUsed[++index] = [@id:item];
			obj.inputsUsed = [];
			index = -1;
			foreach (item, @inputsUsed) obj.inputsUsed[++index] = [@id:item];
		}
	];
}

function Input (id, projProps, point, shortDesc, description) {
	// decide the operation UI
//	if (projProps.type==#Default)
	inputUI = ConstituentsUI::Rhombus (point.getx()-projProps.input.size.getwidth()/2,point.gety()-projProps.input.size.getheight()/2,projProps.input.size.getwidth(),projProps.input.size.getheight());
	//else
	//	other shape
	return [
		@class			: #Input,
		std::inherit(@self, ::sElement(id, inputUI, shortDesc, description, wx::datetime_now(), wx::datetime_now())),
		//
		@operations		: std::list_new(),
		//
		method isOrphan
			{ return @operations.total()==0; },
		method addOperation	(opId) 	{ @operations.push_back(opId);	},
		method savexmlInput (obj) {
			self.savexmlSElement(obj);
			obj.operations = [];
			index = -1;
			foreach (item, @operations) obj.operations[++index] = [@id:item];
		}
	];
}
function Output (id, projProps, point, shortDesc, description) {
	// decide the operation UI
//	if (projProps.type==#Default)
	outputUI = ConstituentsUI::Rhombus (point.getx()-projProps.output.size.getwidth()/2,point.gety()-projProps.output.size.getheight()/2,projProps.output.size.getwidth(),projProps.output.size.getheight());
	//else
	//	other shape
	
	return [
		@class			: #Output,
		std::inherit(@self, ::sElement(id, outputUI, shortDesc, description, wx::datetime_now(), wx::datetime_now())),
		//
//		@operation		: nil,
		@roles			: std::list_new(),
		//
		method isOrphan
			{ return @operation==nil; },
		method setOperation	(opId) 	{ @operation = opId;		},
		method addRoleUseIt	(roleId){ @roles.push_back(roleId);	},
		method savexmlOutput (obj) {
			self.savexmlSElement(obj);
			if (@operation!=nil)
				obj.operation = @operation;
			obj.roles = [];
			index = -1;
			foreach (item, @roles) obj.roles[++index] = [@id:item];
		}
	];
}

//////////////////////////////////////


function ElementSegment (id, description, point1, point2, elUI) {
	return [
		@superclass		: #LineSegment,
		@class			: #ElementSegment,
		@id				: id,
		@description	: description,
		@selected		: true,
		@point1 		: point1,
		@point2 		: point2,
		@segmentUI		: elUI,
		method setElSegment (description) { @description = description; },
		method drawElement (dc) {
/*
			if (self.selected) {
				self.elementUI.drawSelected(dc, pt);
			}
			else
			if (self.isOrphan()) {
				self.elementUI.drawHighlight(dc, pt);
			}
*/
			// draw the shape
			self.segmentUI.drawSegment(dc);
		},
		method onCollision (pt) {
			return (@segmentUI.region.contains(pt)!=0);
		},
		method savexmlElementSegment (obj) {
			obj.id = @id;
			obj.description = @description;
			obj.selected = @selected;
			obj.point1 = [];
			obj.point1[0] = [{#x:@point1.getx()},{#y:@point1.gety()}];
			obj.point2 = [];
			obj.point2[0] = [{#x:@point2.getx()},{#y:@point2.gety()}];
			@segmentUI.savexml(obj);
		}
	];
}

function Address (id, description, projProps, reqId, addrId, addrType, reqPoint, addrPoint) {
	assert (addrType=="Operation" or addrType=="Role");
	segmentUI = ConstituentsUI::Segment(projProps.address.size, projProps.address.style, projProps.address.colour, "address", reqPoint, addrPoint, 2);
	return [
		@class		: #Address,
		@reqId		: reqId,
		@addrEl		: [ @type : addrType, @id : addrId ],
		std::inherit(@self, ::ElementSegment(id, description, reqPoint, addrPoint, segmentUI)),
		method savexmlAddress (obj) {
			self.savexmlElementSegment (obj);
			obj.reqId = @reqId;
			obj.addrEl = [];
			obj.addrEl[0] = [];
			obj.addrEl[0].type = @addrEl.type;
			obj.addrEl[0].id = @addrEl.id;
		}
	];
}

function Use (id, description, projProps, id1, id2, type1, type2, point1, point2) {
	valid =	( (type1=="Operation" and type2=="Input")     or
			  (type1=="Role"      and type2=="Operation") or
			  (type1=="Role"      and type2=="Output")  );
	assert (valid);
	segmentUI = ConstituentsUI::Segment(projProps.use.size, projProps.use.style, projProps.use.colour, "use", point1, point2, 1);
	return [
		@class		: #Use,
		@element1	: [ @type : type1, @id : id1 ],
		@element2	: [ @type : type2, @id : id2 ],
		std::inherit(@self, ::ElementSegment(id, description, point1, point2, segmentUI)),
		method savexmlUse (obj) {
			self.savexmlElementSegment (obj);
			obj.element1 = [];
			obj.element1[0] = [{#type:self.element1.type},{#id:self.element1.id}];
			obj.element2 = [];
			obj.element2[0] = [{#type:self.element2.type},{#id:self.element2.id}];
		}
	];
}

function Offer (id, description, projProps, id1, id2, type1, type2, point1, point2) {
	valid =	(type1=="Role"      and type2=="Operation") or
			(type1=="Operation" and type2=="Output");
	assert(valid);
	segmentUI = ConstituentsUI::Segment(projProps.offer.size,projProps.offer.style,projProps.offer.colour,"offer",point1,point2, 2);
	return [
		@class		: #Offer,
		@element1	: [ @type : type1, @id : id1 ],
		@element2	: [ @type : type2, @id : id2 ],
		std::inherit(@self, ::ElementSegment(id, description, point1, point2, segmentUI)),
		method savexmlOffer (obj) {
			self.savexmlElementSegment (obj);
			obj.element1 = [];
			obj.element1[0] = [{#type:self.element1.type},{#id:self.element1.id}];
			obj.element2 = [];
			obj.element2[0] = [{#type:self.element2.type},{#id:self.element2.id}];
		}
	];
}

function PartOF (id, description, projProps, tid, subtid, type, subtype, point, subpoint) {
	valid = ((type=="Role"		and subtype=="Role"		) or
			 (type=="Operation"	and subtype=="Operation"));
	assert(valid);
	segmentUI = ConstituentsUI::Segment(projProps.offer.size,projProps.offer.style,projProps.offer.colour,"partof",point,subpoint, 1);
	return [
		@class		: #PartOF,
		@element	: [ {#type : type}	 , {#id : tid}    ],
		@subelement	: [ {#type : subtype}, {#id : subtid} ],
		std::inherit(@self, ::ElementSegment(id, description, point, subpoint, segmentUI)),
		method savexmlPartOF (obj) {
			self.savexmlElementSegment (obj);
			obj.element = [];
			obj.element[0] = [{#type:self.element.type},{#id:self.element.id}];
			obj.subelement = [];
			obj.subelement[0] = [{#type:self.subelement.type},{#id:self.subelement.id}];
		}
	];
}


/*
all web data for one pattern
*/

function Constituents (){
	return [
		@class		: #Constituents,
		@idManage	: IdManagement::IdManagement(),
		// Elements
		{ "Requirement" : [] },
		{ "Operation"	: [] },
		{ "Role"		: [] },
		{ "Input"		: [] },
		{ "Output"		: [] },
		// Element Links
		{ "Address"		: [] },
		{ "Use"			: [] },
		{ "Offer"		: [] },
		{ "PartOF"		: [] },
		//
		@selectedItems		: std::list_new(),
		@priorityStack      : std::list_new(),
		//
		@undoStack			: std::list_new(),
		@redoStack			: std::list_new(),
		//
		method destruct () {
			foreach (type, ::Constituent_t) {
				constituents = self[type];
				foreach (constituent, constituents)
					constituent = nil;
				constituents = nil;
			}
			@selectedItems.clear();
			@priorityStack.clear();
		},
		// adds
		method addRequirement (projProps, point...) {
			id = ((arguments.total()==3)?@idManage.getSpID(#Requirement,arguments[2]):@idManage.getID(#Requirement));
			local req = ::Requirement(id, projProps, point, "Requirement"+id, "", "Functional", "Proposed", "Medium", "Medium", "");
			@Requirement[id] = req;
			return @Requirement[id];
		},
		method addOperation (projProps, point...) {
			id = ((arguments.total()==3)?@idManage.getSpID(#Operation,arguments[2]):@idManage.getID(#Operation));
			local op = ::Operation(id, projProps, point, "Operation"+id, "", "Proposed", "Medium", "Medium", "");
			@Operation[id] = op;
			return @Operation[id];
		},
		method addRole (projProps, point...) {
			id = ((arguments.total()==3)?@idManage.getSpID(#Role,arguments[2]):@idManage.getID(#Role));
			local role = ::Role(id, projProps, point, "Role"+id, "", "Proposed", "Medium", "Medium", "", "1.0");
			@Role[id] = role;
			return @Role[id];
		},
		method addInput (projProps, point...) {
			id = ((arguments.total()==3)?@idManage.getSpID(#Input,arguments[2]):@idManage.getID(#Input));
			local input = ::Input(id, projProps, point, "Input"+id, "");
			@Input[id] = input;
			return @Input[id];
		},
		method addOutput (projProps, point...) {
			id = ((arguments.total()==3)?@idManage.getSpID(#Output,arguments[2]):@idManage.getID(#Output));
			local output = ::Output(id, projProps, point, "Output"+id, "");
			@Output[id] = output;
			return @Output[id];
		},
		method addAddress (projProps, reqId, addrId, addrType, reqPoint, addrPoint...) {
			id = ((arguments.total()==7)?@idManage.getSpID(#Address,arguments[6]):@idManage.getID(#Address));
			if (addrType=="Operation") {
				@Requirement[reqId].addAddrOp(addrId);
				self[addrType][addrId].addAddrRequirement(reqId);
			}
			else
			if (addrType=="Role") {
				@Requirement[reqId].setConnRole(addrId);
				self[addrType][addrId].addConnRequirement(reqId);
			}
			else {
				assert(false);
			}
			local address = ::Address(id, "", projProps, reqId, addrId, addrType, reqPoint, addrPoint);
			@Address[id] = address;
			return @Address[id];
		},
		method addUse (projProps, id1, id2, type1, type2, point1, point2...) {
			id = ((arguments.total()==8)?@idManage.getSpID(#Use,arguments[7]):@idManage.getID(#Use));
			if (type1=="Operation" and type2=="Input") {
				@Operation[id1].addInput(id2);
				@Input[id2].addOperation(id1);
			}
			else
			if (type1=="Role") {
				if (type2=="Operation") {
					@Role[id1].addUsedOp(id2);
					@Operation[id2].addRoleUseIt(id1);
				}
				else
				if (type2=="Output") {
					@Role[id1].addUsedOutput(id2);
					@Output[id2].addRoleUseIt(id1);
				}
				else {
					assert(false);
				}
			}
			else {
				assert(false);
			}
			local use = ::Use(id, "", projProps, id1, id2, type1, type2, point1, point2);
			@Use[id] = use;
			return @Use[id];
		},
		method addOffer (projProps, id1, id2, type1, type2, point1, point2...) {
			id = ((arguments.total()==8)?@idManage.getSpID(#Offer,arguments[7]):@idManage.getID(#Offer));
			if (type1=="Role") {
				@Role[id1].addOfferOp(id2);
				@Operation[id2].addRoleOfferIt(id1);
			}
			else
			if (type1=="Operation") {
				@Operation[id1].addOutput(id2);
				@Output[id2].operation = id1;
			}
			else {
				assert (false);
			}
			local offer = ::Offer(id, "", projProps, id1, id2, type1, type2, point1, point2);
			@Offer[id] = offer;
			return @Offer[id];
		},
		method addPartOF (projProps, tid, subtid, type, subtype, point, subpoint...) {
			id = ((arguments.total()==8)?@idManage.getSpID(#PartOF,arguments[7]):@idManage.getID(#PartOF));
			if (type=="Role" and subtype=="Role") {
				@Role[tid].subRoles.push_back(subtid);
				@Role[subtid].parent = tid;
			}
			else
			if (type=="Operation" and subtype=="Operation") {
				@Operation[tid].subOperations.push_back(subtid);
				@Operation[subtid].parent = tid;
			}
			else { assert(false); }
			local partof = ::PartOF(id, "", projProps, tid, subtid, type, subtype, point, subpoint);
			@PartOF[id] = partof;
			return @PartOF[id];
		},
		method addAddressRequirement (projProps, point, addrId, addrType, addrPoint...) {
			if (arguments.total()==5) {
				req  = @addRequirement(projProps, point);
				addr = @addAddress (projProps, req.id, addrId, addrType, req.elementUI.centralPoint, addrPoint);
				return [{#address:addr},{#requirement:req}]; }
			else {
				req  = @addRequirement(projProps, point,arguments[5]);
				addr = @addAddress (projProps, req.id, addrId, addrType, req.elementUI.centralPoint, addrPoint, arguments[6]);
				return [{#address:addr},{#requirement:req}]; }
		},
		method addAddressOperation	(projProps, point, reqId, reqPoint...) {
			if (arguments.total()==4) {
				op   = @addOperation(projProps, point);
				addr = @addAddress(projProps, reqId, op.id, op.class, reqPoint, op.elementUI.centralPoint);
				return [{#address:addr},{#operation:op}]; }
			else {
				op   = @addOperation(projProps, point, arguments[4]);
				addr = @addAddress(projProps, reqId, op.id, op.class, reqPoint, op.elementUI.centralPoint, arguments[5]);
				return [{#address:addr},{#operation:op}];
			}
		},
		method addAddressRole		(projProps, point, reqId, reqPoint...) {
			if (arguments.total()==4) {
				role = @addRole(projProps, point);
				addr = @addAddress(projProps, reqId, role.id, role.class, reqPoint, role.elementUI.centralPoint);
				return [{#address:addr},{#role:role}]; }
			else {
				role = @addRole(projProps, point, arguments[4]);
				addr = @addAddress(projProps, reqId, role.id, role.class, reqPoint, role.elementUI.centralPoint,arguments[5]);
				return [{#address:addr},{#role:role}]; }
		},
		method addUseOperation		(projProps, point, id, type, cpoint...) {
			if (arguments.total()==5) {
				op = @addOperation(projProps, point);
				use;
				if (type==#Role)
					use = @addUse(projProps, id, op.id, type, op.class, cpoint, op.elementUI.centralPoint);
				else
				if (type==#Input)
					use = @addUse(projProps, op.id, id, op.class, type, op.elementUI.centralPoint, cpoint);
				else assert(false);
				return [{#use:use},{#operation:op}]; }
			else {
				op = @addOperation(projProps, point, arguments[5]);
				use;
				if (type==#Role)
					use = @addUse(projProps, id, op.id, type, op.class, cpoint, op.elementUI.centralPoint, arguments[6]);
				else
				if (type==#Input)
					use = @addUse(projProps, op.id, id, op.class, type, op.elementUI.centralPoint, cpoint,arguments[6]);
				else assert(false);
				return [{#use:use},{#operation:op}]; }
		},
		method addUseRole			(projProps, point, id, type, cpoint...) {
			if (arguments.total()==5) {
				role = @addRole(projProps, point);
				use  = @addUse(projProps, role.id, id, role.class, type, role.elementUI.centralPoint, cpoint);
				return [{#use:use},{#role:role}]; }
			else {
				role = @addRole(projProps, point, arguments[5]);
				use  = @addUse(projProps, role.id, id, role.class, type, role.elementUI.centralPoint, cpoint, arguments[6]);
				return [{#use:use},{#role:role}]; }
		},
		method addUseOutput			(projProps, point, id, type, cpoint...) {
			assert(type==#Role);
			if (arguments.total()==5) {
				ou = @addOutput(projProps, point);
				use = @addUse(projProps, id, ou.id, type, ou.class, cpoint, ou.elementUI.centralPoint);
				return [{#use:use},{#output:ou}]; }
			else {
				ou = @addOutput(projProps, point, arguments[5]);
				use = @addUse(projProps, id, ou.id, type, ou.class, cpoint, ou.elementUI.centralPoint, arguments[6]);
				return [{#use:use},{#output:ou}]; }
		},
		method addUseInput			(projProps, point, id, type, cpoint) {
			assert(type==#Operation);
			if (arguments.total()==5) {
				in = @addInput(projProps, point);
				use = @addUse(projProps, id, in.id, type, in.class, cpoint, in.elementUI.centralPoint);
				return [{#use:use},{#input:in}]; }
			else {
				in = @addInput(projProps, point, arguments[5]);
				use = @addUse(projProps, id, in.id, type, in.class, cpoint, in.elementUI.centralPoint, arguments[6]);
				return [{#use:use},{#input:in}]; }
		},
		method addOfferOperation	(projProps, point, id, type, cpoint...) {
			if (arguments.total()==5) {
				op = @addOperation(projProps, point);
				offer;
				if (type==#Role)
					offer = @addOffer(projProps, id, op.id, type, op.class, cpoint, op.elementUI.centralPoint);
				else
					offer = @addOffer(projProps, op.id, id, op.class, type, op.elementUI.centralPoint, cpoint);
				return [{#offer:offer},{#operation:op}]; }
			else {
				op = @addOperation(projProps, point, arguments[5]);
				offer;
				if (type==#Role)
					offer = @addOffer(projProps, id, op.id, type, op.class, cpoint, op.elementUI.centralPoint, arguments[6]);
				else
					offer = @addOffer(projProps, op.id, id, op.class, type, op.elementUI.centralPoint, cpoint, arguments[6]);
				return [{#offer:offer},{#operation:op}]; }
		},
		method addOfferRole			(projProps, point, id, type, cpoint...) {
			if (arguments.total()==5) {
				role  = @addRole(projProps, point);
				offer = @addOffer(projProps, role.id, id, role.class, type, role.elementUI.centralPoint, cpoint);
				return [{#offer:offer},{#role:role}];
			}
			else {
				role  = @addRole(projProps, point, arguments[5]);
				offer = @addOffer(projProps, role.id, id, role.class, type, role.elementUI.centralPoint, cpoint, arguments[6]);
				return [{#offer:offer},{#role:role}];
			}
		},
		method addOfferOutput		(projProps, point, id, type, cpoint...) {
			if (arguments.total()==5) {
				ou = @addOutput(projProps, point);
				offer = @addOffer(projProps, id, ou.id, type, ou.class, cpoint, ou.elementUI.centralPoint);
				return [{#offer:offer},{#output:ou}]; }
			else {
				ou = @addOutput(projProps, point, arguments[5]);
				offer = @addOffer(projProps, id, ou.id, type, ou.class, cpoint, ou.elementUI.centralPoint, arguments[6]);
				return [{#offer:offer},{#output:ou}]; }
		},
		method addPartOFOperation (projProps, point, tid, cpoint...) {
			if (arguments.total()==4) {
				op     = @addOperation(projProps, point);
				partof = @addPartOF(projProps, tid, op.id, #Operation, op.class, cpoint, op.elementUI.centralPoint);
				return [{#partof:partof},{#operation:op}]; }
			else {
				op     = @addOperation(projProps, point, arguments[4]);
				partof = @addPartOF(projProps, tid, op.id, #Operation, op.class, cpoint, op.elementUI.centralPoint, arguments[5]);
				return [{#partof:partof},{#operation:op}]; }
		},
		method addPartOFRole (projProps, point, tid, cpoint...) {
			if (arguments.total()==4) {
				role   = @addRole(projProps, point);
				partof = @addPartOF(projProps, tid, role.id, #Operation, role.class, cpoint, role.elementUI.centralPoint);
				return [{#partof:partof},{#role:role}]; }
			else {
				role   = @addRole(projProps, point, arguments[4]);
				partof = @addPartOF(projProps, tid, role.id, #Operation, role.class, cpoint, role.elementUI.centralPoint, arguments[5]);
				return [{#partof:partof},{#role:role}]; }
		},
		// removes
		method history_removeRequirementETC (reqId, currProject) {
			req = @Requirement[reqId];
			projprops = currProject.properties;
			history = [];
			index = -1;
			it = std::listiter_new();
			it.setbegin(@selectedItems);
			while(it.checkend(@selectedItems)==false) {
				item = it.getval();
				if (item.type==req.class and item.id==req.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"push_back"},{#args:[{0:item}]}];
					break;
				}
				it.fwd();
			}
			it = std::listiter_new();
			it.setbegin(@priorityStack);
			while(it.checkend(@priorityStack)==false) {
				prior = it.getval();
				if (prior.type==req.class and prior.id==req.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"priorityStack"},{"method":"insert"},{#args:[{0:it},{1:prior}]}];
					break;
				}
				it.fwd();
			}
			return history;
		},
		method history_removeRequirement (reqId, currProject) {
			req = @Requirement[reqId];
			projprops = currProject.properties;
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"addRequirement"},{#args:[{0:projprops},{1:wx::point_construct(req.elementUI.centralPoint.getx(),req.elementUI.centralPoint.gety())},{2:reqId}]}];
			history[1] = [{#obj:"Requirement"},{#id:req.id},{"method":"setRequirement"},{#args:[{0:req.shortDesc},{1:req.description},{2:req.type},{3:req.status},{4:req.priority},{5:req.difficulty},{6:req.author}]}];
			index = 1;
			resthistory = @history_removeRequirementETC(reqId, currProject);
			foreach (addr, @Address)
				if (addr.reqId==reqId) {
					history[++index] = [{#obj:"Constituents"},{"method":"addAddress"},{#args:[{0:projprops},{1:addr.reqId},{2:addr.addrEl.id},{3:addr.addrEl.type},{4:addr.segmentUI.point1},{5:addr.segmentUI.point2},{6:addr.id}]}];
					history[++index] = [{#obj:"Address"},{#id:addr.id},{"method":"setElSegment"},{#args:[{0:addr.description}]}];
				}
			foreach (hist, resthistory)
				history[++index] = hist;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeRequirement";
			tmp.args = [{0:reqId},{1:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeRequirement (reqId, currProject) {
			req = @Requirement[reqId];
			projprops = currProject.properties;

			// remove from all ops this requirement id
			foreach (currOp, req.addrOps)
				@Operation[currOp].reqsAddr.remove(reqId);
			// remove from connRole this requirement id
			foreach (currRole, req.addrRoles)
				@Role[currRole].addrReqs.remove(reqId);
			// remove from elemLinks objects with this Requirement id
			foreach (addr, @Address)
				if (addr.reqId==reqId) {
					@idManage.removeID("Address", addr.id);
					@Address[addr.id] = nil;
				}
			foreach (item, @selectedItems) {
				if (item.type==req.class and item.id==req.id) {
					@selectedItems.remove(item);
					break;
				}
			}
			foreach (prior, @priorityStack)
				if (prior.type==req.class and prior.id==req.id) {
					@priorityStack.remove(prior);
					break;
				}
			@idManage.removeID("Requirement", reqId);
			@Requirement[reqId] = nil;
		},
		method history_removeOperationETC (opId, currProject) {
			op = @Operation[opId];
			projprops = currProject.properties;
			history = [];
			index = -1;
			it = std::listiter_new();
			it.setbegin(@selectedItems);
			foreach (item, @selectedItems) {
				if (item.type==op.class and item.id==op.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"insert"},{#args:[{0:it},{1:item}]}];
					break;
				}
				it.fwd();
			}
			it = std::listiter_new();
			it.setbegin(@priorityStack);
			foreach (prior, @priorityStack) {
				if (prior.type==op.class and prior.id==op.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"priorityStack"},{"method":"insert"},{#args:[{0:it},{1:prior}]}];
					break;
				}
				it.fwd();
			}
			return history;
		},
		method history_removeOperation (opId, currProject) {
			op = @Operation[opId];
			projprops = currProject.properties;
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"addOperation"},{#args:[{0:projprops},{1:wx::point_construct(op.elementUI.centralPoint.getx(),op.elementUI.centralPoint.gety())},{2:opId}]}];
			history[1] = [{#obj:"Operation"},{#id:op.id},{"method":"setElement"}, {#args:[{0:op.shortDesc},{1:op.description},{2:op.status},{3:op.priority},{4:op.difficulty},{5:op.author}]}];
			index = 1;
			resthistory = @history_removeOperationETC(opId, currProject);
			foreach (addr, @Address)
				if (addr.addrEl.id==opId and addr.addrEl.type=="Operation") {
					history[++index] = [{#obj:"Constituents"},{"method":"addAddress"},{#args:[{0:projprops},{1:addr.reqId},{2:addr.addrEl.id},{3:addr.addrEl.type},{4:addr.segmentUI.point1},{5:addr.segmentUI.point2},{6:addr.id}]}];
					history[++index] = [{#obj:"Address"},{#id:addr.id},{"method":"setElSegment"},{#args:[{0:addr.description}]}];
				}
			foreach (use, @Use)
				if ((use.element1.id==opId and use.element1.type=="Operation") or
					(use.element2.id==opId and use.element2.type=="Operation"))
				{
					history[++index] = [{#obj:"Constituents"},{"method":"addUse"},{#args:[{0:projprops},{1:use.element1.id},{2:use.element2.id},{3:use.element1.type},{4:use.element2.type},{5:use.segmentUI.point1},{6:use.segmentUI.point2},{7:use.id}]}];
					history[++index] = [{#obj:"Use"},{#id:use.id},{"method":"setElSegment"},{#args:[{0:use.description}]}];
				}
			foreach (offer, @Offer)
				if ((offer.element1.id==opId and offer.element1.type=="Operation") or
				    (offer.element2.id==opId and offer.element2.type=="Operation"))
				{
					history[++index] = [{#obj:"Constituents"},{"method":"addOffer"},{#args:[{0:projprops},{1:offer.element1.id},{2:offer.element2.id},{3:offer.element1.type},{4:offer.element2.type},{5:offer.segmentUI.point1},{6:offer.segmentUI.point2},{7:offer.id}]}];
					history[++index] = [{#obj:"Offer"},{#id:offer.id},{"method":"setElSegment"},{#args:[{0:offer.description}]}];
				}
			foreach (partof, @PartOF)
				if ((   partof.element.type=="Operation" and partof.element.id==opId) or
					(partof.subelement.type=="Operation" and partof.element.id==opId))
				{
					history[++index] = [{#obj:"Constituents"},{"method":"addPartOF"},{#args:[{0:projprops},{1:partof.element.id},{2:partof.subelement.id},{3:partof.element.type},{4:partof.subelement.type},{5:partof.segmentUI.point1},{6:partof.segmentUI.point2},{7:partof.id}]}];
					history[++index] = [{#obj:"PartOF"},{#id:partof.id},{"method":"setElSegment"},{#args:[{0:partof.description}]}];
				}
			foreach (hist, resthistory)
				history[++index] = hist;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeOperation";
			tmp.args = [{0:opId},{1:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeOperation (opId, currProject) {
			op = @Operation[opId];
			projprops = currProject.properties;
			//
			foreach (reqId,	op.reqsAddr)
				@Requirement[reqId].addrOps.remove(opId);
			foreach (roleId, op.rolesUsed)
				@Role[roleId].opsUsed.remove(opId);
			foreach (outputId,op.outputs)
				@Output[outputId].operation = nil;
			foreach (inputId, op.inputs)
				@Input[inputId].operations.remove(opId);
			foreach (roleId, op.roleOffer)
				@Role[roleId].opsOffered.remove(opId);
			foreach (operationId, op.subOperations)
				@PartOF[operationId].parent = nil;
			if (op.parent!=nil) {
				foreach (tmpid, @PartOF[op.parent].subOperations)
					if (tmpid==opId) {
						@PartOF[op.parent].subOperations.remove(tmpid);
						break;
					}
			}
			//
			foreach (addr, @Address)
				if (addr.addrEl.id==opId and addr.addrEl.type=="Operation") {
					@idManage.removeID("Address", addr.id);
					@Address[addr.id] = nil;
				}
			foreach (use, @Use)
				if ((use.element1.id==opId and use.element1.type=="Operation") or
					(use.element2.id==opId and use.element2.type=="Operation")) {
					@idManage.removeID("Use", use.id);
					@Use[use.id] = nil;
				}
			foreach (offer, @Offer)
				if ((offer.element1.id==opId and offer.element1.type=="Operation") or
				    (offer.element2.id==opId and offer.element2.type=="Operation")) {
					@idManage.removeID("Offer", offer.id);
					@Offer[offer.id] = nil;
				}
			foreach (partof, @PartOF)
				if ((   partof.element.type=="Operation" and partof.element.id==opId) or
					(partof.subelement.type=="Operation" and partof.element.id==opId)) {
					@idManage.removeID("PartOF", partof.id);
					@PartOF[partof.id] = nil;
				}
			//
			foreach (item, @selectedItems)
				if (item.type==op.class and item.id==op.id) {
					@selectedItems.remove(item);
					break;
				}
			foreach (prior, @priorityStack)
				if (prior.type==op.class and prior.id==op.id) {
					@priorityStack.remove(prior);
					break;
				}
			@idManage.removeID("Operation", opId);
			@Operation[opId] = nil;
		},
		method history_removeRoleETC (roleId, currProject) {
			role = @Role[roleId];
			projprops = currProject.properties;
			history = [];
			index = -1;
			it = std::listiter_new();
			it.setbegin(@selectedItems);
			foreach (item, @selectedItems) {
				if (item.type==role.class and item.id==role.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"insert"},{#args:[{0:it},{1:item}]}];
					break;
				}
				it.fwd();
			}
			it = std::listiter_new();
			it.setbegin(@priorityStack);
			foreach (prior, @priorityStack) {
				if (prior.type==role.class and prior.id==role.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"priorityStack"},{"method":"insert"},{#args:[{0:it},{1:prior}]}];
					break;
				}
				it.fwd();
			}
			return history;
		},
		method history_removeRole (roleId, currProject) {
			role = @Role[roleId];
			projprops = currProject.properties;
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"addRole"},{#args:[{0:projprops},{1:wx::point_construct(role.elementUI.centralPoint.getx(),role.elementUI.centralPoint.gety())},{2:roleId}]}];
			history[1] = [{#obj:"Role"},{#id:role.id},{"method":"setElement"}, {#args:[{0:role.shortDesc},{1:role.description},{2:role.status},{3:role.priority},{4:role.difficulty},{5:role.author}]}];
			index = 1;
			resthistory = @history_removeRoleETC(roleId, currProject);
			foreach (addr, @Address)
				if (addr.addrEl.id==roleId and addr.addrEl.type=="Role") {
					history[++index] = [{#obj:"Constituents"},{"method":"addAddress"},{#args:[{0:projprops},{1:addr.reqId},{2:addr.addrEl.id},{3:addr.addrEl.type},{4:addr.segmentUI.point1},{5:addr.segmentUI.point2},{6:addr.id}]}];
					history[++index] = [{#obj:"Address"},{#id:addr.id},{"method":"setElSegment"},{#args:[{0:addr.description}]}];
				}
			foreach (use, @Use)
				if (use.element1.id==roleId and use.element1.type=="Role") {
					history[++index] = [{#obj:"Constituents"},{"method":"addUse"},{#args:[{0:projprops},{1:use.element1.id},{2:use.element2.id},{3:use.element1.type},{4:use.element2.type},{5:use.segmentUI.point1},{6:use.segmentUI.point2},{7:use.id}]}];
					history[++index] = [{#obj:"Use"},{#id:use.id},{"method":"setElSegment"},{#args:[{0:use.description}]}];
				}
			foreach (offer, @Offer)
				if (offer.element1.id==roleId and offer.element1.type=="Role") {
					history[++index] = [{#obj:"Constituents"},{"method":"addOffer"},{#args:[{0:projprops},{1:offer.element1.id},{2:offer.element2.id},{3:offer.element1.type},{4:offer.element2.type},{5:offer.segmentUI.point1},{6:offer.segmentUI.point2},{7:offer.id}]}];
					history[++index] = [{#obj:"Offer"},{#id:offer.id},{"method":"setElSegment"},{#args:[{0:offer.description}]}];
				}
			foreach (partof, @PartOF)
				if ((   partof.element.type=="Role" and partof.element.id==roleId) or
					(partof.subelement.type=="Role" and partof.element.id==roleId))
				{
					history[++index] = [{#obj:"Constituents"},{"method":"addPartOF"},{#args:[{0:projprops},{1:partof.element.id},{2:partof.subelement.id},{3:partof.element.type},{4:partof.subelement.type},{5:partof.segmentUI.point1},{6:partof.segmentUI.point2},{7:partof.id}]}];
					history[++index] = [{#obj:"PartOF"},{#id:partof.id},{"method":"setElSegment"},{#args:[{0:partof.description}]}];
				}
			foreach (hist, resthistory)
				history[++index] = hist;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeRole";
			tmp.args = [{0:roleId},{1:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeRole (roleId, currProject) {
			role = @Role[roleId];
			projprops	= nil;
			if (arguments.total()==1)
				projprops = arguments[0][1].properties;
			// 
			foreach (reqId,	role.addrReqs)
				@Requirement[reqId].addrRoles.remove(roleId);
			foreach (opId, role.opsOffered)
				@Operation[opId].roleOffer.remove(roleId);
			foreach (opId, role.opsUsed)
				@Operation[opId].rolesUsed.remove(roleId);
			foreach (outputId,role.outputsUsed)
				@Output[outputId].roles.remove(roleId);
			foreach (rolId, role.subRoles)
				@PartOF[rolId].parent = nil;
			if (role.parent!=nil) {
				foreach (tmpid, @PartOF[role.parent].subOperations)
					if (tmpid==roleId) {
						@PartOF[role.parent].subOperations.remove(tmpid);
						break;
					}
			}
			//
			foreach (addr, @Address)
				if (addr.addrEl.id==roleId and addr.addrEl.type=="Role") {
					@idManage.removeID("Address", addr.id);
					@Address[addr.id] = nil;
				}
			foreach (use, @Use)
				if (use.element1.id==roleId and use.element1.type=="Role") {
					@idManage.removeID("Use", use.id);
					@Use[use.id] = nil;
				}
			foreach (offer, @Offer)
				if (offer.element1.id==roleId and offer.element1.type=="Role") {
					@idManage.removeID("Offer", offer.id);
					@Offer[offer.id] = nil;
				}
			foreach (partof, @PartOF)
				if ((   partof.element.type=="Role" and partof.element.id==roleId) or
					(partof.subelement.type=="Role" and partof.element.id==roleId)) {
					@idManage.removeID("PartOF", partof.id);
					@PartOF[partof.id] = nil;
				}
			//
			foreach (item, @selectedItems)
				if (item.type==role.class and item.id==role.id) {
					@selectedItems.remove(item);
					break;
				}
			foreach (prior, @priorityStack)
				if (prior.type==role.class and prior.id==role.id) {
					@priorityStack.remove(prior);
					break;
				}
			@idManage.removeID("Role", roleId);
			@Role[roleId] = nil;
		},
		method history_removeInputETC (inputId, currProject) {
			input = @Input[inputId];
			projprops = currProject.properties;
			history = [];
			index = -1;
			it = std::listiter_new();
			it.setbegin(@selectedItems);
			foreach (item, @selectedItems) {
				if (item.type==input.class and item.id==input.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"insert"},{#args:[{0:it},{1:item}]}];
					break;
				}
				it.fwd();
			}
			it = std::listiter_new();
			it.setbegin(@priorityStack);
			foreach (prior, @priorityStack) {
				if (prior.type==input.class and prior.id==input.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"priorityStack"},{"method":"insert"},{#args:[{0:it},{1:prior}]}];
					break;
				}
				it.fwd();
			}
			return history;
		},
		method history_removeInput (inputId, currProject) {
			input = @Input[inputId];
			projprops = currProject.properties;
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"addInput"},{#args:[{0:projprops},{1:wx::point_construct(input.elementUI.centralPoint.getx(),input.elementUI.centralPoint.gety())},{2:inputId}]}];
			history[1] = [{#obj:"Input"},{#id:input.id},{"method":"setSElement"}, {#args:[{0:input.shortDesc},{1:input.description}]}];
			index = 1;
			resthistory = @history_removeInputETC(inputId, currProject);
			foreach (use, @Use)
				if (use.element2.id==inputId and use.element2.type=="Input") {
					history[++index] = [{#obj:"Constituents"},{"method":"addUse"},{#args:[{0:projprops},{1:use.element1.id},{2:use.element2.id},{3:use.element1.type},{4:use.element2.type},{5:use.segmentUI.point1},{6:use.segmentUI.point2},{7:use.id}]}];
					history[++index] = [{#obj:"Use"},{#id:use.id},{"method":"setElSegment"},{#args:[{0:use.description}]}];
				}
			foreach (hist, resthistory)
				history[++index] = hist;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeInput";
			tmp.args = [{0:inputId},{1:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeInput (inputId, currProject) {
			input = @Input[inputId];
			projprops = currProject.properties;
			//
			foreach (opId,	input.operations)
				@Operation[opId].inputs.remove(inputId);
			if (input.roles!=nil)
				foreach (roleId,	input.roles)
					@Role[roleId].inputs.remove(inputId);
			foreach (use, @Use)
				if (use.element2.id==inputId and use.element2.type=="Input") {
					@idManage.removeID("Use", use.id);
					@Use[use.id] = nil; }
			foreach (item, @selectedItems)
				if (item.type==input.class and item.id==input.id) {
					@selectedItems.remove(item);
					break;
				}
			foreach (prior, @priorityStack)
				if (prior.type==input.class and prior.id==input.id) {
					@priorityStack.remove(prior);
					break;
				}
			@idManage.removeID("Input", inputId);
			@Input[inputId] = nil;
		},
		method history_removeOutputETC (outputId, currProject) {
			output = @Output[outputId];
			projprops = currProject.properties;
			history = [];
			index = -1;
			it = std::listiter_new();
			it.setbegin(@selectedItems);
			foreach (item, @selectedItems) {
				if (item.type==output.class and item.id==output.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"selectedItems"},{"method":"insert"},{#args:[{0:it},{1:item}]}];
					break;
				}
				it.fwd();
			}
			it = std::listiter_new();
			it.setbegin(@priorityStack);
			foreach (prior, @priorityStack) {
				if (prior.type==output.class and prior.id==output.id) {
					history[++index] = [{#obj:"Constituents"},{#data:"priorityStack"},{"method":"insert"},{#args:[{0:it},{1:prior}]}];
					break;
				}
				it.fwd();
			}
			return history;
		},
		method history_removeOutput (outputId, currProject) {
			output = @Output[outputId];
			projprops = currProject.properties;
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"addOutput"},  {#args:[{0:projprops},{1:wx::point_construct(output.elementUI.centralPoint.getx(),output.elementUI.centralPoint.gety())},{2:outputId}]}];
			history[1] = [{#obj:"Output"},{#id:output.id},{"method":"setSElement"}, {#args:[{0:output.shortDesc},{1:output.description}]}];
			index = 1;
			resthistory = @history_removeOutputETC(outputId, currProject);
			foreach (offer, @Offer)
				if (offer.element2.id==outputId and offer.element2.type=="Output") {
					history[++index] = [{#obj:"Constituents"},{"method":"addOffer"},{#args:[{0:projprops},{1:offer.element1.id},{2:offer.element2.id},{3:offer.element1.type},{4:offer.element2.type},{5:offer.segmentUI.point1},{6:offer.segmentUI.point2},{7:offer.id}]}];
					history[++index] = [{#obj:"Offer"},{#id:offer.id},{"method":"setElSegment"},{#args:[{0:offer.description}]}];
					break; }
			foreach (use, @Use)
				if (use.element2.id==outputId and use.element2.type=="Output") {
					history[++index] = [{#obj:"Constituents"},{"method":"addUse"},{#args:[{0:projprops},{1:use.element1.id},{2:use.element2.id},{3:use.element1.type},{4:use.element2.type},{5:use.segmentUI.point1},{6:use.segmentUI.point2},{7:use.id}]}];
					history[++index] = [{#obj:"Use"},{#id:use.id},{"method":"setElSegment"},{#args:[{0:use.description}]}];
				}
			foreach (hist, resthistory)
				history[++index] = hist;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeOutput";
			tmp.args = [{0:outputId},{1:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeOutput (outputId, currProject) {
			output = @Output[outputId];
			projprops = currProject.properties;
			
			if (output.operation!=nil)
				@Operation[output.operation].outputs.remove(outputId);
			foreach (roleId,	output.roles)
				@Role[roleId].outputs.remove(outputId);
			//
			foreach (offer, @Offer)
				if (offer.element2.id==outputId and offer.element2.type=="Output") {
					@idManage.removeID("Offer", offer.id);
					@Offer[offer.id] = nil;
					break; }
			foreach (use, @Use)
				if (use.element2.id==outputId and use.element2.type=="Output") {
					@idManage.removeID("Use", use.id);
					@Use[use.id] = nil;
				}
			foreach (item, @selectedItems)
				if (item.type==output.class and item.id==output.id) {
					@selectedItems.remove(item);
					break;
				}
			foreach (prior, @priorityStack)
				if (prior.type==output.class and prior.id==output.id) {
					@priorityStack.remove(prior);
					break;
				}
			@idManage.removeID("Output", outputId);
			@Output[outputId] = nil;
		},
		method history_removeAddressETC (addrId, currProject) {
			address = @Address[addrId];
			projprops = currProject.properties;
			history = [];
			index = -1;
			if (address.addrEl.type=="Operation") {
				history[++index] = [{#obj:"Requirement"},{#id:address.reqId},{"data":"addrOps"},{"method":"push_back"},{#args:[{0:address.addrEl.id}]}];
				history[++index] = [{#obj:"Operation"},{#id:address.addrEl.id},{"data":"reqsAddr"},{"method":"push_back"},{#args:[{0:address.reqId}]}];
			}
			else
			if (address.addrEl.type=="Role") {
				history[++index] = [{#obj:"Requirement"},{#id:address.reqId},{"data":"addrRoles"},{"method":"push_back"},{#args:[{0:address.addrEl.id}]}];
				history[++index] = [{#obj:"Role"},{#id:address.addrEl.id},{"data":"addrReqs"},{"method":"push_back"},{#args:[{0:address.reqId}]}];
			}
			return history;
		},
		method history_removeAddress (addrId, currProject) {
			address = @Address[addrId];
			projprops = currProject.properties;
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"addAddress"},{#args:[{0:projprops},{1:address.reqId},{2:address.addrEl.id},{3:address.addrEl.type},{4:address.segmentUI.point1},{5:address.segmentUI.point2},{6:addrId}]}];
			history[1] = [{#obj:"Address"},{#id:address.id},{"method":"setElSegment"},{#args:[{0:address.description}]}];
			index = 1;
			resthistory = @history_removeAddressETC(addrId, currProject);
			foreach (hist, resthistory)
				history[++index] = hist;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeAddress";
			tmp.args = [{0:addrId},{1:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeAddress (addrId, currProject) {
			address = @Address[addrId];
			projprops = currProject.properties;
			
			if (address.addrEl.type=="Operation") {
				@Requirement[address.reqId].addrOps.remove(address.addrEl.id);
				@Operation[address.addrEl.id].reqsAddr.remove(address.reqId);
			}
			else
			if (address.addrEl.type=="Role") {
				@Requirement[address.reqId].addrRoles.remove(address.addrEl.id);
				@Role[address.addrEl.id].addrReqs.remove(address.reqId);
			}
			@idManage.removeID("Address", addrId);
			@Address[addrId] = nil;
		},
		method history_removeUseETC (useId, currProject) {
			use = @Use[useId];
			projprops = currProject.properties;
			history = [];
			index = -1;
			if (use.element1.type=="Operation" and use.element2.type=="Input") {
				history[++index] = [{#obj:"Operation"},{#id:use.element1.id},{"data":"inputs"},{"method":"push_back"},{#args:[{0:use.element2.id}]}];
				history[++index] = [{#obj:"Input"},{#id:use.element2.id},{"data":"operations"},{"method":"push_back"},{#args:[{0:use.element1.id}]}];
			}
			else
			if (use.element1.type=="Role") {
				if (use.element2.type=="Operation") {
					history[++index] = [{#obj:"Role"},{#id:use.element1.id},{"data":"opsUsed"},{"method":"push_back"},{#args:[{0:use.element2.id}]}];
					history[++index] = [{#obj:"Operation"},{#id:use.element2.id},{"data":"rolesUsed"},{"method":"push_back"},{#args:[{0:use.element1.id}]}];
				}
				else
				if (use.element2.type=="Output") {
					history[++index] = [{#obj:"Role"},{#id:use.element1.id},{"data":"outputsUsed"},{"method":"push_back"},{#args:[{0:use.element2.id}]}];
					history[++index] = [{#obj:"Output"},{#id:use.element2.id},{"data":"roles"},{"method":"push_back"},{#args:[{0:use.element1.id}]}];
				}
				else { assert(false); }
			}
			else { assert(false); }
			return history;
		},
		method history_removeUse (useId, currProject) {
			use = @Use[useId];
			projprops = currProject.properties;
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"addUse"},{#args:[{0:projprops},{1:use.element1.id},{2:use.element2.id},{3:use.element1.type},{4:use.element2.type},{5:use.segmentUI.point1},{6:use.segmentUI.point2},{7:useId}]}];
			history[1] = [{#obj:"Use"},{#id:use.id},{"method":"setElSegment"},{#args:[{0:use.description}]}];
			index = 1;
			resthistory = @history_removeUseETC(useId, currProject);
			foreach (hist, resthistory)
				history[++index] = hist;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeUse";
			tmp.args = [{0:useId},{1:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeUse (useId, currProject) {
			use = @Use[useId];
			projprops = currProject.properties;
			
			if (use.element1.type=="Operation" and use.element2.type=="Input") {
				@Operation[use.element1.id].inputs.remove(use.element2.id);
				@Input[use.element2.id].operations.remove(use.element1.id);
			}
			else
			if (use.element1.type=="Role") {
				if (use.element2.type=="Operation") {
					@Role[use.element1.id].opsUsed.remove(use.element2.id);
					@Operation[use.element2.id].rolesUsed.remove(use.element1.id);
				}
				else
				if (use.element2.type=="Output") {
					@Role[use.element1.id].outputsUsed.remove(use.element2.id);
					@Output[use.element2.id].roles.remove(use.element1.id);
				}
				else { assert(false); }
			}
			else { assert(false); }
			@idManage.removeID("Use", useId);
			@Use[useId] = nil;
		},
		method history_removeOfferETC (offerId, currProject) {
			offer = @Offer[offerId];
			projprops = currProject.properties;
			history = [];
			index = -1;
			if (offer.element1.type=="Operation") {
				history[++index] = [{#obj:"Operation"},{#id:offer.element1.id},{"data":"outputs"},{"method":"push_back"},{#args:[{0:offer.element2.id}]}];
				history[++index] = [{#obj:"Output"},{#id:offer.element2.id},{"data":"operation"},{#args:[{0:offer.element1.id}]}];
			}
			else
			if (offer.element1.type=="Role") {
				history[++index] = [{#obj:"Role"},{#id:offer.element1.id},{"data":"opsOffered"},{"method":"push_back"},{#args:[{0:offer.element2.id}]}];
				history[++index] = [{#obj:"Operation"},{#id:offer.element2.id},{"data":"roleOffer"},{"method":"push_back"},{#args:[{0:offer.element1.id}]}];
			}
			else { assert (false); }
			return history;
		},
		method history_removeOffer (offerId, currProject) {
			offer = @Offer[offerId];
			projprops = currProject.properties;
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"addOffer"},{#args:[{0:projprops},{1:offer.element1.id},{2:offer.element2.id},{3:offer.element1.type},{4:offer.element2.type},{5:offer.segmentUI.point1},{6:offer.segmentUI.point2},{7:offerId}]}];
			history[1] = [{#obj:"Offer"},{#id:offer.id},{"method":"setElSegment"},{#args:[{0:offer.description}]}];
			index = 1;
			resthistory = @history_removeOfferETC(offerId, currProject);
			foreach (hist, resthistory)
				history[++index] = hist;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeOffer";
			tmp.args = [{0:offerId},{1:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeOffer (offerId, currProject) {
			offer = @Offer[offerId];
			projprops = currProject.properties;
			
			if (offer.element1.type=="Operation") {
				@Operation[offer.element1.id].outputs.remove(offer.element2.id);
				@Output[offer.element2.id].operation = nil;
			}
			else
			if (offer.element1.type=="Role") {
				@Role[offer.element1.id].opsOffered.remove(offer.element2.id);
				@Operation[offer.element2.id].roleOffer.remove(offer.element1.id);
			}
			else { assert (false); }
			@idManage.removeID("Offer",offerId);
			@Offer[offerId] = nil;
		},
		method history_removePartOFETC (partofId, currProject) {
			partof = @PartOF[partofId];
			projprops = currProject.properties;
			history = [];
			index = -1;
			if (partof.element.type=="Role") {
				history[++index] = [{#obj:"Role"},{#id:partof.element.id},{"data":"subRoles"},{"method":"push_back"},{#args:[{0:partof.subelement.id}]}];
				history[++index] = [{#obj:"Role"},{#id:partof.subelement.id},{"data":"parent"},{#args:[{0:partof.element.id}]}];
			}
			else
			if (partof.element.type=="Operation") {
				history[++index] = [{#obj:"Operation"},{#id:partof.element.id},{"data":"subRoles"},{"method":"push_back"},{#args:[{0:partof.subelement.id}]}];
				history[++index] = [{#obj:"Operation"},{#id:partof.subelement.id},{"data":"parent"},{#args:[{0:partof.element.id}]}];
			}
			else { assert(false); }
			return history;
		},
		method history_removePartOF (partofId, currProject) {
			partof = @PartOF[partofId];
			projprops = currProject.properties;
			history = [];
			history[0] = [{#obj:"Constituents"},{"method":"addPartOF"},{#args:[{0:projprops},{1:partof.element.id},{2:partof.subelement.id},{3:partof.element.type},{4:partof.subelement.type},{5:partof.segmentUI.point1},{6:partof.segmentUI.point2},{7:partofId}]}];
			history[1] = [{#obj:"PartOF"},{#id:partof.id},{"method":"setElSegment"},{#args:[{0:partof.description}]}];
			index = 1;
			resthistory = @history_removePartOFETC(partofId, currProject);
			foreach (hist, resthistory)
				history[++index] = hist;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removePartOF";
			tmp.args = [{0:partofId},{1:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removePartOF (partofId, currProject) {
			partof = @PartOF[partofId];
			projprops = currProject.properties;
			
			if (partof.element.type=="Role") {
				@Role[partof.element.id].subRoles.remove(partof.subelement.id);
				@Role[partof.subelement.id].parent = nil;
			}
			else
			if (partof.element.type=="Operation") {
				@Operation[partof.element.id].subRoles.remove(partof.subelement.id);
				@Operation[partof.subelement.id].parent = nil;
			}
			else { assert(false); }
			@idManage.removeID("PartOF",partofId);
			@PartOF[partofId] = nil;
		},
		method history_removeAddressRequirement (addrId, reqId, currProject) {
			address = @Address[addrId];
			req     = @Requirement[reqId];
			projProps = currProject.properties;
			point = wx::point_construct(req.elementUI.centralPoint.getx(), req.elementUI.centralPoint.gety());
			constituent = self[address.addrEl.type][address.addrEl.id];
			history = [];
			history[0] = [{#obj:"Constituents"},{#method:"addAddressRequirement"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:reqId},{6:addrId}]}];
			history[1] = [{#obj:"Requirement"},{#id:req.id},{#method:"setRequirement"},{#args:[{0:req.shortDesc},{1:req.description},{2:req.type},{3:req.status},{4:req.priority},{5:req.difficulty},{6:req.author}]}];
			history[2] = [{#obj:"Address"},{#id:address.id},{"method":"setElSegment"},{#args:[{0:address.description}]}];
			index = 2;
			rhist2 = @history_removeRequirementETC(reqId, currProject);
			foreach (rh2, rhist2)
				history[++index] = rh2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeAddressRequirement";
			tmp.args = [{0:addrId},{1:reqId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeAddressRequirement	(addrId, reqId, currProject...) {
			@removeAddress(addrId, currProject);
			@removeRequirement(reqId, currProject);
		},
		method history_removeAddressOperation (addrId, opId, currProject) {
			address = @Address[addrId];
			op = @Operation[opId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(op.elementUI.centralPoint.getx(), op.elementUI.centralPoint.gety());
			constituent = @Requirement[address.reqId];
			history[0] = [{#obj:"Constituents"},{#method:"addAddressOperation"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.elementUI.centralPoint},{4:opId},{5:addrId}]}];
			history[1] = [{#obj:"Operation"},{#id:op.id},{#method:"setElement"},{#args:[{0:op.shortDesc},{1:op.description},{2:op.status},{3:op.priority},{4:op.difficulty},{5:op.author}]}];
			history[2] = [{#obj:"Address"},{#id:address.id},{"method":"setElSegment"},{#args:[{0:address.description}]}];
			index = 2;
			hrest2 = @history_removeOperationETC(opId, currProject);
			foreach (hr2, hrest2)
				history[++index] = hr2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeAddressOperation";
			tmp.args = [{0:addrId},{1:opId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeAddressOperation	(addrId, opId, currProject...) {
			@removeAddress(addrId, currProject);
			@removeOperation(opId, currProject);
		},
		method history_removeAddressRole (addrId, roleId, currProject) {
			address = @Address[addrId];
			role = @Role[roleId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(role.elementUI.centralPoint.getx(), role.elementUI.centralPoint.gety());
			constituent = @Requirement[address.reqId];
			history[0] = [{#obj:"Constituents"},{#method:"addAddressRole"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.elementUI.centralPoint}]},{4:roleId},{5:addrId}];
			history[1] = [{#obj:"Role"},{#id:role.id},{#method:"setElement"},{#args:[{0:role.shortDesc},{1:role.description},{2:role.status},{3:role.priority},{4:role.difficulty},{5:role.author}]}];
			history[2] = [{#obj:"Address"},{#id:address.id},{"method":"setElSegment"},{#args:[{0:address.description}]}];
			index = 2;
			hrest2 = @history_removeRoleETC(roleId, currProject);
			foreach (hr2, hrest2)
				history[++index] = hr2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeAddressRole";
			tmp.args = [{0:addrId},{1:roleId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeAddressRole		(addrId, roleId, currProject...) {
			@removeAddress(addrId, currProject);
			@removeRole(roleId, currProject);
		},
		method history_removeUseOperation (useId, opId, currProject) {
			use = @Use[useId];
			op  = @Operation[opId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(op.elementUI.centralPoint.getx(), op.elementUI.centralPoint.gety());
			constituent = ((use.element1.type=="Operation")?self[use.element2.type][use.element2.id]:self[use.element1.type][use.element1.id]);
			history[0] = [{#obj:"Constituents"},{#method:"addUseOperation"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:opId},{6:useId}]}];
			history[1] = [{#obj:"Operation"},{#id:op.id},{#method:"setElement"},{#args:[{0:op.shortDesc},{1:op.description},{2:op.status},{3:op.priority},{4:op.difficulty},{5:op.author}]}];
			history[2] = [{#obj:"Use"},{#id:use.id},{"method":"setElSegment"},{#args:[{0:use.description}]}];
			index = 2;
			hrest2 = @history_removeOperationETC(opId, currProject);
			foreach (hr2, hrest2)
				history[++index] = hr2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeUseOperation";
			tmp.args = [{0:useId},{1:opId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeUseOperation		(useId, opId, currProject...) {
			@removeUse(useId, currProject);
			@removeOperation(opId, currProject);
		},
		method history_removeUseRole (useId, roleId, currProject) {
			use   = @Use[useId];
			role  = @Role[roleId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(role.elementUI.centralPoint.getx(), role.elementUI.centralPoint.gety());
			constituent = self[use.element2.type][use.element2.id];
			history[0] = [{#obj:"Constituents"},{#method:"addUseRole"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:roleId},{6:useId}]}];
			history[1] = [{#obj:"Role"},{#id:role.id},{#method:"setElement"},{#args:[{0:role.shortDesc},{1:role.description},{2:role.status},{3:role.priority},{4:role.difficulty},{5:role.author}]}];
			history[2] = [{#obj:"Use"},{#id:use.id},{"method":"setElSegment"},{#args:[{0:use.description}]}];
			index = 2;
			hrest2 = @history_removeRoleETC(roleId, currProject);
			foreach (hr2, hrest2)
				history[++index] = hr2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeUseRole";
			tmp.args = [{0:useId},{1:roleId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeUseRole			(useId, roleId, currProject...) {
			@removeUse(useId, currProject);
			@removeRole(roleId, currProject);
		},
		method history_removeUseOutput (useId, outputId, currProject) {
			use    = @Use[useId];
			output = @Output[outputId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(output.elementUI.centralPoint.getx(), output.elementUI.centralPoint.gety());
			constituent = self[use.element2.type][use.element2.id];
			history[0] = [{#obj:"Constituents"},{#method:"addUseOutput"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:outputId},{6:useId}]}];
			history[1] = [{#obj:"Output"},{#id:output.id},{#method:"setSElement"},{#args:[{0:output.shortDesc},{1:output.description}]}];
			history[2] = [{#obj:"Use"},{#id:use.id},{"method":"setElSegment"},{#args:[{0:use.description}]}];
			index = 2;
			hrest2 = @history_removeOutputETC(outputId, currProject);
			foreach (hr2, hrest2)
				history[++index] = hr2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeUseOutput";
			tmp.args = [{0:useId},{1:outputId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeUseOutput			(useId, outputId, currProject...) {
			@removeUse(useId, currProject);
			@removeOutput(outputId, currProject);
		},
		method history_removeUseInput (useId, inputId, currProject) {
			use    = @Use[useId];
			input = @Input[inputId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(input.elementUI.centralPoint.getx(), input.elementUI.centralPoint.gety());
			constituent = self[use.element2.type][use.element2.id];
			history[0] = [{#obj:"Constituents"},{#method:"addUseInput"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:inputId},{6:useId}]}];
			history[1] = [{#obj:"Input"},{#id:input.id},{#method:"setSElement"},{#args:[{0:input.shortDesc},{1:input.description}]}];
			history[2] = [{#obj:"Use"},{#id:use.id},{"method":"setElSegment"},{#args:[{0:use.description}]}];
			index = 2;
			hrest2 = @history_removeInputETC(inputId, currProject);
			foreach (hr2, hrest2)
				history[++index] = hr2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeUseInput";
			tmp.args = [{0:useId},{1:inputId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeUseInput			(useId, inputId, currProject...) {
			@removeUse(useId, currProject);
			@removeInput(inputId, currProject);
		},
		method history_removeOfferOperation (offerId, opId, currProject) {
			offer = @Offer[offerId];
			op    = @Operation[opId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(op.elementUI.centralPoint.getx(),op.elementUI.centralPoint.gety());
			constituent = ((offer.element1.type=="Operation")?self[offer.element2.type][offer.element2.id]:self[offer.element1.type][offer.element1.id]);
			history[0] = [{#obj:"Constituents"},{#method:"addOfferOperation"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:opId},{6:offerId}]}];
			history[1] = [{#obj:"Operation"},{#id:op.id},{#method:"setElement"},{#args:[{0:op.shortDesc},{1:op.description},{2:op.status},{3:op.priority},{4:op.difficulty},{5:op.author}]}];
			history[2] = [{#obj:"Offer"},{#id:offer.id},{"method":"setElSegment"},{#args:[{0:offer.description}]}];
			index = 2;
			rhist2 = @history_removeOperationETC(opId, currProject);
			foreach (rh2, rhist2)
				history[++index] = rh2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeOfferOperation";
			tmp.args = [{0:offerId},{1:opId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeOfferOperation		(offerId, opId, currProject...) {
			@removeOffer(offerId, currProject);
			@removeOperation(opId, currProject);
		},
		method history_removeOfferRole (offerId, roleId, currProject) {
			offer = @Offer[offerId];
			role = @Role[roleId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(role.elementUI.centralPoint.getx(),role.elementUI.centralPoint.gety());
			constituent = self[offer.element2.type][offer.element2.id];
			history[0] = [{#obj:"Constituents"},{#method:"addOfferRole"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:roleId},{6:offerId}]}];
			history[1] = [{#obj:"Role"},{#id:role.id},{#method:"setElement"},{#args:[{0:role.shortDesc},{1:role.description},{2:role.status},{3:role.priority},{4:role.difficulty},{5:role.author}]}];
			history[2] = [{#obj:"Offer"},{#id:offer.id},{"method":"setElSegment"},{#args:[{0:offer.description}]}];
			index = 2;
			rhist2 = @history_removeRoleETC(roleId, currProject);
			foreach (rh2, rhist2)
				history[++index] = rh2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeOfferRole";
			tmp.args = [{0:offerId},{1:roleId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeOfferRole			(offerId, roleId, currProject...) {
			@removeOffer(offerId, currProject);
			@removeRole(roleId, currProject);
		},
		method history_removeOfferOutput (offerId, outputId, currProject) {
			offer = @Offer[offerId];
			output = @Output[outputId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(output.elementUI.centralPoint.getx(),output.elementUI.centralPoint.gety());
			constituent = self[offer.element2.type][offer.element2.id];
			history[0] = [{#obj:"Constituents"},{#method:"addOfferOutput"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:outputId},{6:offerId}]}];
			history[1] = [{#obj:"Output"},{#id:output.id},{#method:"setSElement"},{#args:[{0:output.shortDesc},{1:output.description}]}];
			history[2] = [{#obj:"Offer"},{#id:offer.id},{"method":"setElSegment"},{#args:[{0:offer.description}]}];
			index = 2;
			rhist2 = @history_removeOutputETC(outputId, currProject);
			foreach (rh2, rhist2)
				history[++index] = rh2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removeOfferOutput";
			tmp.args = [{0:offerId},{1:outputId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removeOfferOutput		(offerId, outputId, currProject...) {
			@removeOffer(offerId, currProject);
			@removeOutput(outputId, currProject);
		},
		method history_removePartOFOperation (partofId, opId, currProject) {
			partof = @PartOF[partofId];
			op = @Operation[opId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(op.elementUI.centralPoint.getx(),op.elementUI.centralPoint.gety());
			constituent = ((partof.element.id==opId)?self[partof.subelement.type][partof.subelement.id]:self[partof.element.type][partof.element.id]);
			history[0] = [{#obj:"Constituents"},{#method:"addPartOFOperation"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:opId},{6:partofId}]}];
			history[1] = [{#obj:"Operation"},{#id:op.id},{#method:"setElement"},{#args:[{0:op.shortDesc},{1:op.description},{2:op.status},{3:op.priority},{4:op.difficulty},{5:op.author}]}];
			history[2] = [{#obj:"PartOF"},{#id:partof.id},{"method":"setElSegment"},{#args:[{0:partof.description}]}];
			index = 2;
			rhist2 = @history_removeOperationETC(opId, currProject);
			foreach (rh2, rhist2)
				history[++index] = rh2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removePartOFOperation";
			tmp.args = [{0:partofId},{1:opId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removePartOFOperation	(partofId, opId, currProject...) {
			@removePartOF(partofId, currProject);
			@removeOperation(opId, currProject);
		},
		method history_removePartOFRole (partofId, roleId, currProject) {
			partof = @PartOF[partofId];
			role = @Role[roleId];
			projProps = currProject.properties;
			history = [];
			point = wx::point_construct(role.elementUI.centralPoint.getx(),role.elementUI.centralPoint.gety());
			constituent = ((partof.element.id==roleId)?self[partof.subelement.type][partof.subelement.id]:self[partof.element.type][partof.element.id]);
			history[0] = [{#obj:"Constituents"},{#method:"addPartOFRole"},{#args:[{0:projProps},{1:point},{2:constituent.id},{3:constituent.class},{4:constituent.elementUI.centralPoint},{5:roleId},{6:partofId}]}];
			history[1] = [{#obj:"Role"},{#id:role.id},{#method:"setElement"},{#args:[{0:role.shortDesc},{1:role.description},{2:role.status},{3:role.priority},{4:role.difficulty},{5:role.author}]}];
			history[2] = [{#obj:"PartOF"},{#id:partof.id},{"method":"setElSegment"},{#args:[{0:partof.description}]}];
			index = 2;
			rhist2 = @history_removeRoleETC(roleId, currProject);
			foreach (rh2, rhist2)
				history[++index] = rh2;
			tmp = [];
			tmp.packet = true;
			tmp.method = "removePartOFRole";
			tmp.args = [{0:partofId},{1:roleId},{2:currProject}];
			tmp.data = history;
			return tmp;
		},
		method removePartOFRole	(partofId, roleId, currProject...) {
			@removePartOF(partofId, currProject);
			@removeRole(roleId, currProject);
		},
		// undo action of split
		method undoSplitRole (rolesId, element, currProject) {
			rid	= rolesId[0];
			projProps = currProject.properties;
			// keep elements
			point	= element.point;
			rolelements = [
				element.shortDesc,
				element.description,
				element.status,
				element.priority,
				element.difficulty,
				element.author
			];
			// remove splited roles
			foreach (id, rolesId) {
				AddrFuncs::TreeView(#removeChildPattern) (currProject, @Role[id], element.wxparent);
				// treeview kai oti allo otan kanoume delete
				@removeRole(id, currProject);
			}
			// add role
			newrole	= @addRole(projProps, point, element.id);
			newrole.setElement (|rolelements|);
			// selected items
			foreach (sel, @selectedItems)
				self[sel.type][sel.id].selected = false;
			@selectedItems.clear();
			@selectedItems.push_back([{"type":newrole.class},{"id":newrole.id}]);
			newrole.selected = true;
			// new item must be first in the priority stack
			@priorityStack.push_front([{"type":newrole.class},{"id":newrole.id}]);
			// fix connections for role
			connections = element.connections;
			length	= std::tablength(connections);
			for (i=0;i<length;++i) {
				el1 = connections[i];
				++i;
				// add segment method
				segment = self[el1.method] (|el1.args|);
				el2 = connections[i];
				// setElSegment for this segment
				segment[el2.method] (|el2.args|);
			}
			AddrFuncs::TreeView(#addChildPattern) (currProject, newrole, element.wxparent);
		},
		// redo action of split
		method redoSplitRole (rolesId, element, currProject) {
			AddrFuncs::TreeView(#removeChildPattern) (currProject, @Role[element.id], element.wxparent);
			@removeRole(element.id, currProject);
			point = element.point;
			rolelements = [
				element.description,
				element.status,
				element.priority,
				element.difficulty,
				element.author
			];
			//
			length = std::tablength(rolesId);
			for(i=0;i<length;++i) {
				newrole	= @addRole (currProject.properties, point, rolesId[i]);
				newrole.setElement (((i+1)+". "+element.shortDesc), |rolelements|);
				// selected items
				foreach (sel, @selectedItems)
					self[sel.type][sel.id].selected = false;
				@selectedItems.clear();
				@selectedItems.push_back([{"type":newrole.class},{"id":newrole.id}]);
				newrole.selected = true;
				// new item must be first in the priority stack
				@priorityStack.push_front([{"type":newrole.class},{"id":newrole.id}]);
				AddrFuncs::TreeView(#addChildPattern) (currProject, newrole, element.wxparent);
				//
				point += wx::point_construct(10,5);
			}
		},
		method undoJoinRole (roleId, element, currProject) {
			wxparent	= element.wxparent;
			items		= element.items;
			// remove the joined role
			AddrFuncs::TreeView(#removeChildPattern) (currProject, @Role[roleId], wxparent);
			@removeRole(roleId, currProject);
			//
			foreach (item, items) {
				//history to exec for undo of currRole
				history = item.data;
				total	= std::tablength(history);
				// addRole
				action = history[0];
				acttmp = history[1];
				newrole = self[action.method] (|action.args|);
				newrole[acttmp.method] (|acttmp.args|);
				// addConnections of role and fix data (selecteditems, priority)
				for (i=2;i<total;++i) {
					action = history[i];
					// constituents methods
					if (std::isnil(action.data)) {
						// consume one more action, two actions goes together!
						++i;
						acttmp = history[i];
						constituent = self[action.method] (|action.args|);
						constituent[acttmp.method] (|acttmp.args|);
					}
					// constituents selected items and priority stack
					else {
						if (action.data=="selectedItems") {
							unique = true;
							comp = action.args[0];
							foreach (item, @selectedItems)
								if (item.id==comp.id and item.type==comp.type)
									{ unique = false; break; }
							if (unique)
								self[action.data][action.method] (|action.args|);
						}
						else {
							self[action.data][action.method] (|action.args|);
						}
					}
				}
				AddrFuncs::TreeView(#addChildPattern) (currProject, newrole, wxparent);
			}
		},
		method redoJoinRole (roleId, element, currProject) {
			wxparent	= element.wxparent;
			point		= element.point;
			items		= element.items;
			// create object for onJoinRole to reuse function
			frame 					= [];
			frame.redo				= true;
			frame.currProject		= currProject;
			frame.point				= point;
			frame.window			= [];
			frame.window.wxparent	= wxparent;
			// call for join
			AddrFuncs::ConstituentsEvents(#onJoinRole) (frame, []);
		}
	];
}

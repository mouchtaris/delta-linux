using #ProjectRAD;
using #Constituents;
using #ShapeRAD;
using #ToolBarRAD;
using #PatternsEvents;
using #ConstituentsMouseEvents;
using #AddrFuncs;
const nl = "\n";


function loadInfo () {
	infoxml = xml::load("RAD.info");
	if (std::isnil(infoxml)) {
		// there is no info so create new one
		t = [{"$Name": "RapidArchitectureDesign_Info"}];
		t.Dimensions = [ {#width:1280}, {#height:800} ];
		t.Project    = [ {#offset:0} ];
		xml::store(t,"RAD.info");
		infoxml = xml::load("RAD.info");
	}
	// decode Object
	info = [];
	info.Dimensions			= [];
	info.Dimensions.width	= std::strtonum(infoxml.Dimensions[0]["$Attributes"].width);
	info.Dimensions.height	= std::strtonum(infoxml.Dimensions[0]["$Attributes"].height);
	info.Project		= [];
	info.Project.offset	= std::strtonum(infoxml.Project[0]["$Attributes"].offset);
	return info;
}

function saveInfo (data) {
	xml::store(data, "RAD.info");
}

/**
 *  Description: Save patterns
 *  Arguments:
 *  	pattern, the pattern name .patt
 *
 **/
function onSavePattern (pattern, ev) {
	pat = [{"$Name": "RaidenPattern"}];
	pat["$Attributes"] = [];
	pat["$Attributes"].magickey = 634782951;
	pat.pattern    				= [];
	pat.pattern[0]				= [];
	pat.pattern[0].id			= pattern.id;
	pat.pattern[0].projOffset	= pattern.projOffset;
	pat.pattern[0].name			= pattern.name;
	pat.pattern[0].author		= pattern.author;		
	pat.pattern[0].description	= pattern.description;
	pat.pattern[0].created		= [];
	pat.pattern[0].created[0]	= [
		@year	: pattern.created.getyear(),
		@month	: pattern.created.getmonth(),
		@day	: pattern.created.getday()
	];
	pat.pattern[0].lastUpdate	= [];
	pat.pattern[0].lastUpdate[0]= [
		@year:pattern.lastUpdate.getyear(),
		@month:pattern.lastUpdate.getmonth(),
		@day:pattern.lastUpdate.getday()
	];
	pat.pattern[0].virtualsize = [];
	pat.pattern[0].virtualsize[0] = [
		@width	: pattern.virtualsize.getwidth(),
		@height	: pattern.virtualsize.getheight()
	];
	pat.pattern[0].rect = [];
	pat.pattern[0].rect[0] = [
		@x		: pattern.rect.getx(),
		@y		: pattern.rect.gety(),
		@width	: pattern.rect.getwidth(),
		@height	: pattern.rect.getheight()
	];
	pat.pattern[0].constituents = [];
	pat.pattern[0].constituents[0] = [];
	pat.pattern[0].constituents[0].idManage = [];
	pat.pattern[0].constituents[0].idManage["$Attributes"] = [];
	constituents_t = [ {0:"Requirement"}, {1:"Operation"}, {2:"Role"}, {3:"Input"}, {4:"Output"}, {5:"Address"}, {6:"Use"}, {7:"Offer"}, {8:"PartOF"} ];
	foreach (type, constituents_t) {
		typeid = type+"ID";
		pat.pattern[0].constituents[0].idManage[typeid] = [];
		pat.pattern[0].constituents[0].idManage[typeid].id = pattern.constituents.idManage[typeid];
		typefs = type+"FS";
		pat.pattern[0].constituents[0].idManage[typefs] = [];
		lindex = -1;
		freeSlots = pattern.constituents.idManage[typefs];
		foreach (currSlot, freeSlots)
			pat.pattern[0].constituents[0].idManage[typefs][++lindex] = [@id:currSlot];
	}
	pat.pattern[0].constituents[0].selectedItems = [];
	slindex = -1;
	foreach (selectedItem, pattern.constituents.selectedItems)
		pat.pattern[0].constituents[0].selectedItems[++slindex] = [@type:selectedItem.type, @id:selectedItem.id];
	pat.pattern[0].constituents[0].priorityStack = [];
	psindex = 0;
	foreach (priorityItem, pattern.constituents.priorityStack)
		pat.pattern[0].constituents[0].priorityStack[++psindex] = [@type:priorityItem.type,@id:priorityItem.id];
////
	constituents_t = [ {0:"Requirement"}, {1:"Operation"}, {2:"Role"}, {3:"Input"}, {4:"Output"}, {5:"Address"}, {6:"Use"}, {7:"Offer"}, {8:"PartOF"} ];
	foreach (type, constituents_t) {
		pat.pattern[0].constituents[0][type] = [];
		indexd = 0;
		foreach (element, pattern.constituents[type]) {
			pat.pattern[0].constituents[0][type][indexd] = [];
			namemethod = "savexml"+element.class;
			element[namemethod](pat.pattern[0].constituents[0][type][indexd]);
			++indexd;
		}
	}
	//store pattern
	xml::store(pat,pattern.path+"\\"+pattern.name+".patt");
}


/**
 *  Description: Save All project - patterns
 *  Arguments:
 *  	frame, main frame of raiden
 *  	ev,    event menu command selected or ctrl-s pressed
 *  Returns: nope
 **/
function onSave (frame, ev) {
	currProj = ProjectRAD::GetCurrentProject();
	// if user has not load a project in machine
	if (std::isnil(currProj)) return;
	//
	notebook = frame.notebook;
	selection = wx::notebook_getselection(notebook);
	// save only a pattern
	if (selection!=0 and frame.updateNewPattern==nil) {
		patPage = notebook.getcurrentpage();
		foreach (pattern, currProj.patterns)
			if (pattern.window!=nil and pattern.window.getid()==patPage.getid()) {
				pattern.path = frame.filePath + "\\patterns";
				::onSavePattern(pattern, ev);
				return;
			}
		std::print("Error, pattern not found!",nl);
		assert(false);
	}
	// save project - part of patterns or all patterns
	fileName = frame.fileName;
	if (std::isnil(fileName)) fileName = currProj.title+".rproj";
	t = [{"$Name": "Raiden"}];
	t["$Attributes"] = [];
	t["$Attributes"].magickey = 634782950;
	t.project    = [];
	t.project[0] = [];
	t.project[0].id     = currProj.id;
	t.project[0].title  = currProj.title;
	t.project[0].author = currProj.author;
	t.project[0].patternOffset = currProj.patternOffset;
	t.project[0].created = [];
	t.project[0].created[0] = [@year:currProj.created.getyear(),@month:currProj.created.getmonth(),@day:currProj.created.getday()];
	t.project[0].lastUpdate = [];
	t.project[0].lastUpdate[0] = [@year:currProj.lastUpdate.getyear(),@month:currProj.lastUpdate.getmonth(),@day:currProj.lastUpdate.getday()];
	t.project[0].suggestAuthors = [];
	saindex = 0;
	foreach (sauthor, currProj.suggestAuthors) {
		t.project[0].suggestAuthors[saindex] = [];
		t.project[0].suggestAuthors[saindex]["$Attributes"] = [];
		t.project[0].suggestAuthors[saindex]["$Attributes"].author = sauthor;
		++saindex;
	}
	index = 0;
//	foreach (author, currProj.suggestAuthors) { t.project[0].suggestAuthors[index] = author; ++index; }
////// properties project
	t.project[0].properties = [];
	t.project[0].properties[0] = [];
	t.project[0].properties[0]["$Attributes"] = [];
	t.project[0].properties[0].requirement = [];
	t.project[0].properties[0].requirement[0] = [];
	t.project[0].properties[0].requirement[0]["$Attributes"] = [];
	t.project[0].properties[0].requirement[0]["$Attributes"].style  = currProj.properties.requirement.style;
	t.project[0].properties[0].requirement[0]["$Attributes"].colour = currProj.properties.requirement.colour;
	t.project[0].properties[0].requirement[0].size = [];
	t.project[0].properties[0].requirement[0].size[0] = [@width:currProj.properties.requirement.size.getwidth(),@height:currProj.properties.requirement.size.getheight()];
	t.project[0].properties[0].operation = [];
	t.project[0].properties[0].operation[0] = [];
	t.project[0].properties[0].operation[0]["$Attributes"] = [];
	t.project[0].properties[0].operation[0]["$Attributes"].style  = currProj.properties.operation.style;
	t.project[0].properties[0].operation[0]["$Attributes"].colour = currProj.properties.operation.colour;
	t.project[0].properties[0].operation[0].size = [];
	t.project[0].properties[0].operation[0].size[0] = [@width:currProj.properties.operation.size.getwidth(),@height:currProj.properties.operation.size.getheight()];
	t.project[0].properties[0].role = [];
	t.project[0].properties[0].role[0] = [];
	t.project[0].properties[0].role[0]["$Attributes"] = [];
	t.project[0].properties[0].role[0]["$Attributes"].style  = currProj.properties.role.style;
	t.project[0].properties[0].role[0]["$Attributes"].colour = currProj.properties.role.colour;
	t.project[0].properties[0].role[0].size = [];
	t.project[0].properties[0].role[0].size[0] = [@width:currProj.properties.role.size.getwidth(),@height:currProj.properties.role.size.getheight()];
	t.project[0].properties[0].input = [];
	t.project[0].properties[0].input[0] = [];
	t.project[0].properties[0].input[0]["$Attributes"] = [];
	t.project[0].properties[0].input[0]["$Attributes"].style  = currProj.properties.input.style;
	t.project[0].properties[0].input[0]["$Attributes"].colour = currProj.properties.input.colour;
	t.project[0].properties[0].input[0].size = [];
	t.project[0].properties[0].input[0].size[0] = [@width:currProj.properties.input.size.getwidth(),@height:currProj.properties.input.size.getheight()];
	t.project[0].properties[0].output = [];
	t.project[0].properties[0].output[0] = [];
	t.project[0].properties[0].output[0]["$Attributes"] = [];
	t.project[0].properties[0].output[0]["$Attributes"].style  = currProj.properties.output.style;
	t.project[0].properties[0].output[0]["$Attributes"].colour = currProj.properties.output.colour;
	t.project[0].properties[0].output[0].size = [];
	t.project[0].properties[0].output[0].size[0] = [@width:currProj.properties.output.size.getwidth(),@height:currProj.properties.output.size.getheight()];
	t.project[0].properties[0].address = [];
	t.project[0].properties[0].address[0] = currProj.properties.address;
	t.project[0].properties[0].use = [];
	t.project[0].properties[0].use[0] = currProj.properties.use;
	t.project[0].properties[0].offer = [];
	t.project[0].properties[0].offer[0] = currProj.properties.offer;
	t.project[0].connManageId = [];
	t.project[0].connManageId[0] = [];
	t.project[0].connManageId[0]["$Attributes"] = [];
	t.project[0].connManageId[0]["$Attributes"].currId = currProj.connManageId.currId;
	t.project[0].connManageId[0].freeIds = [];
	findex = 0;
	foreach (freeId, currProj.connManageId.freeIds) {
		t.project[0].connManageId[0].freeIds[findex] = [];
		t.project[0].connManageId[0].freeIds[findex]["$Attributes"] = [];
		t.project[0].connManageId[0].freeIds[findex]["$Attributes"].id = freeId;
		++findex;
	}
	t.project[0].patterns = [];
	index = 0;
	//
	foreach (currPat, currProj.patterns) {
		t.project[0].patterns[index] = [];
		t.project[0].patterns[index]["$Attributes"] = [];
		t.project[0].patterns[index]["$Attributes"].path = currPat.path + "\\" + currPat.name + ".patt";
		::onSavePattern(currPat, ev);
		++index;
	}
	t.project[0].filters = [];
	// save the load filters of project
	foreach (filter, currProj.filters) {
		t.project[0].filters[index]								= [];
		t.project[0].filters[index]["$Attributes"]				= [];
		t.project[0].filters[index]["$Attributes"].id			= filter.id;
		t.project[0].filters[index]["$Attributes"].name			= filter.name;
		t.project[0].filters[index]["$Attributes"].description	= filter.description;
		t.project[0].filters[index]["$Attributes"].author		= filter.author;
		t.project[0].filters[index]["$Attributes"].path			= filter.path;
		t.project[0].filters[index]["$Attributes"].dbc			= filter.dbc;
		t.project[0].filters[index].created		= [];
		t.project[0].filters[index].created[0]	= [
			@year	: filter.created.getyear(),
			@month	: filter.created.getmonth(),
			@day	: filter.created.getday()
		];
		t.project[0].filters[index].lastUpdate	= [];
		t.project[0].filters[index].lastUpdate[0]= [
			@year	: filter.lastUpdate.getyear(),
			@month	: filter.lastUpdate.getmonth(),
			@day	: filter.lastUpdate.getday()
		];
		++index;
	}
	xml::store(t, fileName);
}

function loadRequirement (req,props,constituents) {
	if (req["$Attributes"]!=nil) {
		id = std::strtonum(req["$Attributes"].id);
		shortDesc = req["$Attributes"].shortDesc;
		description = req["$Attributes"].description;
		status = req["$Attributes"].status;
		priority = req["$Attributes"].priority;
		difficulty = req["$Attributes"].difficulty;
		author = req["$Attributes"].author;
		type = req["$Attributes"].type;
		rectangle = req.elementUI[0]["$Attributes"];
		point = wx::point_construct(std::strtonum(rectangle.x),std::strtonum(rectangle.y));
		requirement = Constituents::Requirement(id,props,point,shortDesc,description,type,status,priority,difficulty,author);
		requirement.selected = (req["$Attributes"].selected=="true"?true:false);
		created = req.created[0]["$Attributes"];
		requirement.created = wx::datetime_constructfromdmy(std::strtonum(created.day),std::strtonum(created.month),std::strtonum(created.year));
		lastUpdate = req.lastUpdate[0]["$Attributes"];
		requirement.lastUpdate = wx::datetime_constructfromdmy(std::strtonum(lastUpdate.day),std::strtonum(lastUpdate.month),std::strtonum(lastUpdate.year));
		requirement.elementUI.setValues(point.getx(),point.gety(),std::strtonum(rectangle.width),std::strtonum(rectangle.height));
		//
		foreach (op, req.addrOps)
			if (op["$Attributes"]!=nil and op["$Attributes"].id!=nil)
				requirement.addrOps.push_back(std::strtonum(op["$Attributes"].id));
		foreach (role, req.addrRoles)
			if (role["$Attributes"]!=nil and role["$Attributes"].id!=nil)
				requirement.addrRoles.push_back(std::strtonum(role["$Attributes"].id));
		constituents["Requirement"][requirement.id] = requirement;
	}
}

function loadOperation (op,props,constituents) {
	if (op["$Attributes"]!=nil) {
		id = std::strtonum(op["$Attributes"].id);
		shortDesc = op["$Attributes"].shortDesc;
		description = op["$Attributes"].description;
		status = op["$Attributes"].status;
		priority = op["$Attributes"].priority;
		difficulty = op["$Attributes"].difficulty;
		author = op["$Attributes"].author;
		rectangle = op.elementUI[0]["$Attributes"];
		point = wx::point_construct(std::strtonum(rectangle.x),std::strtonum(rectangle.y));
		operation = Constituents::Operation(id,props,point,shortDesc,description,status,priority,difficulty,author);
		operation.selected = (op["$Attributes"].selected=="true"?true:false);
		created = op.created[0]["$Attributes"];
		operation.created = wx::datetime_constructfromdmy(std::strtonum(created.day),std::strtonum(created.month),std::strtonum(created.year));
		lastUpdate = op.lastUpdate[0]["$Attributes"];
		operation.lastUpdate = wx::datetime_constructfromdmy(std::strtonum(lastUpdate.day),std::strtonum(lastUpdate.month),std::strtonum(lastUpdate.year));
		operation.elementUI.setValues(point.getx(),point.gety(),std::strtonum(rectangle.width),std::strtonum(rectangle.height));
		if(op.parent!=nil)
			operation.parent = std::strtonum(op.parent);
		foreach (subOps, op.subOperations)
			if (subOps["$Attributes"]!=nil and subOps["$Attributes"].id!=nil)
				operation.subOperations.push_back(std::strtonum(subOps["$Attributes"].id));
		foreach (req, op.reqsAddr)
			if (req["$Attributes"]!=nil and req["$Attributes"].id!=nil)
				operation.reqsAddr.push_back(std::strtonum(req["$Attributes"].id));
		foreach (roles, op.rolesUsed)
			if(roles["$Attributes"]!=nil and roles["$Attributes"].id!=nil)
				operation.rolesUsed.push_back(std::strtonum(roles["$Attributes"].id));
		foreach (output, op.outputs)
			if(output["$Attributes"]!=nil and output["$Attributes"].id!=nil)
				operation.outputs.push_back(std::strtonum(output["$Attributes"].id));
		foreach (input, op.inputs)
			if(input["$Attributes"]!=nil and input["$Attributes"].id!=nil)
				operation.inputs.push_back(std::strtonum(input["$Attributes"].id));
		foreach (roleOffer, op.roleOffer)
			if(roleOffer["$Attributes"]!=nil and roleOffer["$Attributes"].id!=nil)
				operation.roleOffer.push_back(std::strtonum(roleOffer["$Attributes"].id));
		constituents["Operation"][operation.id] = operation;
	}
}

function loadRole (ro,props,constituents) {
	if (ro["$Attributes"]!=nil) {
		id = std::strtonum(ro["$Attributes"].id);
		shortDesc = ro["$Attributes"].shortDesc;
		description = ro["$Attributes"].description;
		status = ro["$Attributes"].status;
		priority = ro["$Attributes"].priority;
		difficulty = ro["$Attributes"].difficulty;
		author = ro["$Attributes"].author;
		rectangle = ro.elementUI[0]["$Attributes"];
		version = ro.version[0]["$Attributes"].value;
		point = wx::point_construct(std::strtonum(rectangle.x),std::strtonum(rectangle.y));
		role = Constituents::Role(id,props,point,shortDesc,description,status,priority,difficulty,author,version);
		role.selected = (ro["$Attributes"].selected=="true"?true:false);
		created = ro.created[0]["$Attributes"];
		role.created = wx::datetime_constructfromdmy(std::strtonum(created.day),std::strtonum(created.month),std::strtonum(created.year));
		lastUpdate = ro.lastUpdate[0]["$Attributes"];
		role.lastUpdate = wx::datetime_constructfromdmy(std::strtonum(lastUpdate.day),std::strtonum(lastUpdate.month),std::strtonum(lastUpdate.year));
		role.elementUI.setValues(point.getx(),point.gety(),std::strtonum(rectangle.width),std::strtonum(rectangle.height));
		//
		if(ro.parent!=nil)
			role.parent = std::strtonum(ro.parent);
		foreach (subRole, ro.subRoles)
			if (subRole["$Attributes"]!=nil and subRole["$Attributes"].id!=nil)
				role.subRoles.push_back(std::strtonum(subRole["$Attributes"].id));
		foreach (req, ro.addrReqs)
			if (req["$Attributes"]!=nil and req["$Attributes"].id!=nil)
				role.addrReqs.push_back(std::strtonum(req["$Attributes"].id));
		foreach (op, ro.opsOffered)
			if (op["$Attributes"]!=nil and op["$Attributes"].id!=nil)
				role.opsOffered.push_back(std::strtonum(op["$Attributes"].id));
		foreach (op, ro.opsUsed)
			if (op["$Attributes"]!=nil and op["$Attributes"].id!=nil)
				role.opsUsed.push_back(std::strtonum(op["$Attributes"].id));
		foreach (op, ro.outputsUsed)
			if (op["$Attributes"]!=nil and op["$Attributes"].id!=nil)
				role.outputsUsed.push_back(std::strtonum(op["$Attributes"].id));
		foreach (op, ro.inputsUsed)
			if (op["$Attributes"]!=nil and op["$Attributes"].id!=nil)
				role.inputsUsed.push_back(std::strtonum(op["$Attributes"].id));
		constituents["Role"][role.id] = role;
	}
}

function loadInput (in,props,constituents) {
	if (in["$Attributes"]!=nil) {
		id			= std::strtonum(in["$Attributes"].id);
		shortDesc	= in["$Attributes"].shortDesc;
		description = in["$Attributes"].description;
		rectangle	= in.elementUI[0]["$Attributes"];
		point 		= wx::point_construct(std::strtonum(rectangle.x),std::strtonum(rectangle.y));
		input 			= Constituents::Input(id,props,point,shortDesc,description);
		input.selected 	= (in["$Attributes"].selected=="true"?true:false);
		created 		= in.created[0]["$Attributes"];
		input.created 	= wx::datetime_constructfromdmy(std::strtonum(created.day),std::strtonum(created.month),std::strtonum(created.year));
		lastUpdate 		= in.lastUpdate[0]["$Attributes"];
		input.lastUpdate = wx::datetime_constructfromdmy(std::strtonum(lastUpdate.day),std::strtonum(lastUpdate.month),std::strtonum(lastUpdate.year));
		input.elementUI.setValues(point.getx(),point.gety(),std::strtonum(rectangle.width),std::strtonum(rectangle.height));	
		foreach (op, in.operations)
			if (op["$Attributes"]!=nil and op["$Attributes"].id!=nil)
				input.operations.push_back(std::strtonum(op["$Attributes"].id));
		constituents["Input"][input.id] = input;
	}
}

function loadOutput (ou,props,constituents) {
	if (ou["$Attributes"]!=nil) {
		id			= std::strtonum(ou["$Attributes"].id);
		shortDesc	= ou["$Attributes"].shortDesc;
		description = ou["$Attributes"].description;
		rectangle	= ou.elementUI[0]["$Attributes"];
		point		= wx::point_construct(std::strtonum(rectangle.x),std::strtonum(rectangle.y));
		output		= Constituents::Output(id,props,point,shortDesc,description);
		output.selected = (ou["$Attributes"].selected=="true"?true:false);
		created 		= ou.created[0]["$Attributes"];
		output.created	= wx::datetime_constructfromdmy(std::strtonum(created.day),std::strtonum(created.month),std::strtonum(created.year));
		lastUpdate			= ou.lastUpdate[0]["$Attributes"];
		output.lastUpdate	= wx::datetime_constructfromdmy(std::strtonum(lastUpdate.day),std::strtonum(lastUpdate.month),std::strtonum(lastUpdate.year));
		output.elementUI.setValues(point.getx(),point.gety(),std::strtonum(rectangle.width),std::strtonum(rectangle.height));	
		if (ou.operation!=nil)
			output.operation = std::strtonum(ou.operation);
		foreach (role, ou.roles)
			if (role["$Attributes"]!=nil and role["$Attributes"].id!=nil)
				output.roles.push_back(std::strtonum(role["$Attributes"].id));
		constituents["Output"][output.id] = output;
	}
}

function loadAddress (ad,props,constituents) {
	if (ad["$Attributes"]!=nil) {
		id			= std::strtonum(ad["$Attributes"].id);
		description = ad["$Attributes"].description;
		reqId 		= std::strtonum(ad["$Attributes"].reqId);
		addrId 		= std::strtonum(ad.addrEl[0]["$Attributes"].id);
		addrType 	= ad.addrEl[0]["$Attributes"].type;
		reqPoint 	= wx::point_construct(std::strtonum(ad.point[0]["$Attributes"].x),std::strtonum(ad.point[0]["$Attributes"].y));
		addrPoint 	= wx::point_construct(std::strtonum(ad.point[1]["$Attributes"].x),std::strtonum(ad.point[1]["$Attributes"].y));
		address 	= Constituents::Address(id,description,props,reqId,addrId,addrType,reqPoint,addrPoint);
		lineColour 	= ad.line[0]["$Attributes"].colour;
		lineStyle 	= ad.line[0]["$Attributes"].style;
		lineWidth 	= std::strtonum(ad.line[0]["$Attributes"].width);
		address.segmentUI.line = [
			@width	: lineWidth,
			@style	: lineStyle,
			@colour	: lineColour
		];
		constituents["Address"][address.id] = address;
	}
}

function loadUse (us,props,constituents) {
	if (us["$Attributes"]!=nil) {
		id			= std::strtonum(us["$Attributes"].id);
		description = us["$Attributes"].description;
		id1			= std::strtonum(us.element1[0]["$Attributes"].id);
		type1		= us.element1[0]["$Attributes"].type;
		id2			= std::strtonum(us.element2[0]["$Attributes"].id);
		type2		= us.element2[0]["$Attributes"].type;
		point1		= wx::point_construct(std::strtonum(us.point[0]["$Attributes"].x),std::strtonum(us.point[0]["$Attributes"].y));
		point2		= wx::point_construct(std::strtonum(us.point[1]["$Attributes"].x),std::strtonum(us.point[1]["$Attributes"].y));
		use			= Constituents::Use(id,description,props,id1,id2,type1,type2,point1,point2);
		lineColour	= us.line[0]["$Attributes"].colour;
		lineStyle	= us.line[0]["$Attributes"].style;
		lineWidth	= std::strtonum(us.line[0]["$Attributes"].width);
		use.segmentUI.line = [
			@width	: lineWidth,
			@style	: lineStyle,
			@colour	: lineColour
		];
		constituents["Use"][use.id] = use;
	}
}
function loadOffer (us,props,constituents) {
	if (us["$Attributes"]!=nil) {
		id			= std::strtonum(us["$Attributes"].id);
		description = us["$Attributes"].description;
		id1			= std::strtonum(us.element1[0]["$Attributes"].id);
		type1		= us.element1[0]["$Attributes"].type;
		id2			= std::strtonum(us.element2[0]["$Attributes"].id);
		type2		= us.element2[0]["$Attributes"].type;
		point1		= wx::point_construct(std::strtonum(us.point[0]["$Attributes"].x),std::strtonum(us.point[0]["$Attributes"].y));
		point2		= wx::point_construct(std::strtonum(us.point[1]["$Attributes"].x),std::strtonum(us.point[1]["$Attributes"].y));
		offer		= Constituents::Offer(id,description,props,id1,id2,type1,type2,point1,point2);
		lineColour	= us.line[0]["$Attributes"].colour;
		lineStyle	= us.line[0]["$Attributes"].style;
		lineWidth	= std::strtonum(us.line[0]["$Attributes"].width);
		offer.segmentUI.line = [
			@width	: lineWidth,
			@style	: lineStyle,
			@colour	: lineColour
		];
		constituents["Offer"][offer.id] = offer;
	}
}

function loadPartOF (us,props,constituents) {
	if (us["$Attributes"]!=nil) {
		id 			= std::strtonum(us["$Attributes"].id);
		description = us["$Attributes"].description;
		tid			= std::strtonum(us.element[0]["$Attributes"].id);
		type		= us.element[0]["$Attributes"].type;
		subtid		= std::strtonum(us.subelement[0]["$Attributes"].id);
		subtype		= us.subelement[0]["$Attributes"].type;
		point		= wx::point_construct(std::strtonum(us.point[0]["$Attributes"].x),std::strtonum(us.point[0]["$Attributes"].y));
		subpoint	= wx::point_construct(std::strtonum(us.point[1]["$Attributes"].x),std::strtonum(us.point[1]["$Attributes"].y));
		inh			= Constituents::PartOF(id,description,props,tid,subtid,type,subtype,point,subpoint);
		lineColour	= us.line[0]["$Attributes"].colour;
		lineStyle	= us.line[0]["$Attributes"].style;
		lineWidth	= std::strtonum(us.line[0]["$Attributes"].width);
		inh.segmentUI.line	= [
			@width : lineWidth,
			@style : lineStyle,
			@colour : lineColour
		];
		constituents["PartOF"][inh.id] = inh;
	}
}

/**
 *  Description: Load pattern in project with path, pathPatt. check if it is invalid
 *  Arguments:
 *  	pathPatt, The path of pattern which machine or user want to load in project
 *  	project,  The current project which load the pattern
 **/
function loadPattern (pathPatt, project) {
	// check for same name
	tmp		= std::strtokenise(pathPatt,"\\").back();
	index	= 0;
	name	= "";
	length	= std::strlen(tmp);
	while(index<length) {
		if (index+5>=length)
			break;
		name += std::strchar(tmp,index);
		++index;
	}
	tmp = std::strtokenise(pathPatt,"\\");
	tmp.pop_back();
	index = 0;
	path = "";
	foreach (el, tmp) {
		if (index!=0) path += "\\";
		path += el;
		++index;
	}
	foreach (cpattern, project.patterns)
		if (cpattern.name==name) {
			mesgdlg =wx::messagedialog_construct(nil, "There is pattern file with the same name", "Not allowed",
			wx::flags(wx::ICON_ERROR, wx::OK));
			mesgdlg.showmodal();
			return nil;
		}
	//
	obj =  xml::load(pathPatt);
	// case *.patt was not found
	if (obj==nil or obj["$Attributes"]==nil or obj.pattern[0]==nil) {
		mesgdlg =wx::messagedialog_construct(nil, "The pattern file "+name+" didn't found.", "Not allowed",
		wx::flags(wx::ICON_WARNING, wx::OK));
		mesgdlg.showmodal();
		return nil;
	}
	magickey = obj["$Attributes"].magickey;
	loadPatt = obj.pattern[0];
	if (magickey!=634782951 and std::isnil(loadPatt)) {
		mesgdlg =wx::messagedialog_construct(nil, "This pattern file is invalid for Raiden", "Not allowed",
		wx::flags(wx::ICON_ERROR, wx::OK));
		mesgdlg.showmodal();
		return nil;
	}
	id = std::strtonum(loadPatt["$Attributes"].id);
	author = loadPatt["$Attributes"].author;
	description = loadPatt["$Attributes"].description;
	//
	local loadpattern = project.addPattern(path, wx::point_construct(10,10));
	loadpattern.setPattern (name, author, description);
	//
	createdObj = loadPatt.created;
	created = createdObj[0]["$Attributes"];
	loadpattern.created = wx::datetime_constructfromdmy(std::strtonum(created.day),std::strtonum(created.month),std::strtonum(created.year));
	lastUpdate = loadPatt.lastUpdate[0]["$Attributes"];
	loadpattern.lastUpdate = wx::datetime_constructfromdmy(std::strtonum(lastUpdate.day),std::strtonum(lastUpdate.month),std::strtonum(lastUpdate.year));
	vsize = loadPatt.virtualsize[0]["$Attributes"];
	//virtual size assign
	other = wx::size_construct(std::strtonum(vsize.width), std::strtonum(vsize.height));
	wx::size_assign(loadpattern.virtualsize, other);
	rect = loadPatt.rect;
	if (rect!=nil and rect[0]!=nil and rect[0]["$Attributes"]!=nil) {
		rect = loadPatt.rect[0]["$Attributes"];
		loadpattern.rect = wx::rect_construct(std::strtonum(rect.x), std::strtonum(rect.y), std::strtonum(rect.width), std::strtonum(rect.height));
	}
	storeConstituents = loadPatt.constituents[0];
	loadConstituents = loadpattern.constituents;
	loadIdManage = loadConstituents.idManage;
	storeIdManage = storeConstituents.idManage[0];
	constituents_t = Constituents::GetConstituent_t();
	foreach (type, constituents_t) {
		typeid = type+"ID";
		id = storeIdManage[typeid][0]["$Attributes"].id;
		loadIdManage[typeid] = std::strtonum(id);
		typefs = type+"FS";
		freeSlots = storeIdManage[typefs];
		foreach (currSlot, freeSlots)
			if (freeSlots[0]["$Attributes"]!=nil)
				loadIdManage[typefs].push_back(std::strtonum(freeSlots[0]["$Attributes"].id));
	}
	sItems = loadPatt.constituents[0].selectedItems;
	foreach (item, sItems)
		if (item!=nil and item["$Attributes"]!=nil and item["$Attributes"].id!=nil)
			loadpattern.constituents.selectedItems.push_back([@type:item["$Attributes"].type,@id:std::strtonum(item["$Attributes"].id)]);
	pStack = loadPatt.constituents[0].priorityStack;
	foreach (item, pStack)
		if (item!=nil and item["$Attributes"]!=nil and item["$Attributes"].id!=nil)
			loadpattern.constituents.priorityStack.push_back([@type:item["$Attributes"].type,@id:std::strtonum(item["$Attributes"].id)]);
	constituents_t = Constituents::GetConstituent_t();
	//
	callFuncs = [@Requirement: ::loadRequirement,@Operation: ::loadOperation,@Role: ::loadRole,@Input: ::loadInput,@Output: ::loadOutput,@Address: ::loadAddress,@Use: ::loadUse,@Offer: ::loadOffer, @PartOF: ::loadPartOF];
	foreach (t, constituents_t) {
		constituentsType = loadPatt.constituents[0][t];
		foreach (tmpConstituent, constituentsType) {
			callFuncs[t](tmpConstituent,project.properties,loadpattern.constituents);
		}
	}
	return loadpattern;
}

function loadFilter (filter, project) {
	newFilter = project.addFilter (filter["$Attributes"].name, filter["$Attributes"].description, filter["$Attributes"].author, filter["$Attributes"].path, filter["$Attributes"].dbc);
	created = filter.created[0]["$Attributes"];
	newFilter.created = wx::datetime_constructfromdmy(std::strtonum(created.day),std::strtonum(created.month),std::strtonum(created.year));
	lastUpdate = filter.lastUpdate[0]["$Attributes"];
	newFilter.lastUpdate = wx::datetime_constructfromdmy(std::strtonum(lastUpdate.day),std::strtonum(lastUpdate.month),std::strtonum(lastUpdate.year));
	return newFilter;
}


function loadProject (loadProj, frame) {
	id = std::strtonum(loadProj["$Attributes"].id);
	title = loadProj["$Attributes"].title;
	author = loadProj["$Attributes"].author;
	properties = ProjectRAD::ProjProperties();
	shape = ["requirement", "operation", "role", "input", "output"];
	foreach (type, shape) {
		tmp = loadProj.properties[0][type][0];
		properties[type].style = tmp["$Attributes"].style;
		properties[type].colour = tmp["$Attributes"].colour;
		properties[type].size.setheight(std::strtonum(tmp.size[0]["$Attributes"].height));
		properties[type].size.setwidth(std::strtonum(tmp.size[0]["$Attributes"].width));
	}
	segment = ["address", "use", "offer"];
	foreach (type, segment) {
		tmp = loadProj.properties[0][type][0];
		properties[type].style = tmp["$Attributes"].style;
		properties[type].colour = tmp["$Attributes"].colour;
		properties[type].size = std::strtonum(tmp["$Attributes"].size);
	}
	project = ProjectRAD::Project(id,title,author,properties,frame);
	project.patternOffset = std::strtonum(loadProj["$Attributes"].patternOffset);
	created = loadProj.created[0]["$Attributes"];
	project.created = wx::datetime_constructfromdmy(std::strtonum(created.day),std::strtonum(created.month),std::strtonum(created.year));
	lastUpdate = loadProj.lastUpdate[0]["$Attributes"];
	project.lastUpdate = wx::datetime_constructfromdmy(std::strtonum(lastUpdate.day),std::strtonum(lastUpdate.month),std::strtonum(lastUpdate.year));
	connManageId = loadProj.connManageId[0];
	project.connManageId.currId = std::strtonum(connManageId["$Attributes"].currId);
	foreach (freeId, connManageId.freeIds) {
		if(freeId["$Attributes"]!=nil)
			project.connManageId.freeIds.push_back(std::strtonum(freeId["$Attributes"].id));
	}
	foreach (sauthor, loadProj.suggestAuthors)
		if (sauthor!=nil and sauthor["$Attributes"]!=nil and sauthor["$Attributes"].author!=nil)
			project.addSuggestAuthor(sauthor["$Attributes"].author);
	projOffset = project.patternOffset;
	// load patterns
	foreach (currPattern, loadProj.patterns)
		if(currPattern!=nil and currPattern["$Attributes"]!=nil)
			::loadPattern(currPattern["$Attributes"].path, project);
	// load filters
	foreach (filter, loadProj.filters)
		if (filter!=nil and filter["$Attributes"])
			::loadFilter (filter, project);
	return project;
}

function onLoad (frame, ev) {
	currProj = ProjectRAD::GetCurrentProject();
	fileName = frame.fileName;
	obj =  xml::load(fileName);
	magickey = obj["$Attributes"].magickey;
	loadProj = obj.project[0];
	project = nil;
	try {
		if (magickey!=634782950 and std::isnil(loadProj)) {
			mesgdlg =wx::messagedialog_construct(nil, "This .rproj is invalid for Raiden", "Not allowed",
			wx::flags(wx::ICON_ERROR, wx::OK));
			mesgdlg.showmodal();
			return;
		}
		// check if it is valid
		// with the magickey
		// else load
		
		if (currProj!=nil) {
			frame.fileName = nil;
			// save currProject
			::onSave(frame, ev);
			// close currProject
			i=1;
			notebook = frame.notebook;
			while (true) {
				if (notebook.getpagecount()==1)
					break;
				currPage = notebook.getpage(i);
				notebook.removepage(i);
				currPage.show(false);
				currPage.destroy();
			}
			frame.fileName = fileName;
			project = ::loadProject (loadProj,frame);
			ProjectRAD::SetCurrProject(project,frame);
	//		frame.currProject = project;
	//		frame.connect(12, wx::EVT_COMMAND_NOTEBOOK_PAGE_CHANGED, PatternsEvents::onChangePageNotebook);
			ShapeRAD::drawPatterns(project);
		}
		else {
			project = ::loadProject (loadProj,frame);
			ProjectRAD::SetCurrentProject(project,frame);
			ToolBarRAD::connectToolBar(frame.mainToolBar);
			frame.currProject = project;
			frame.connect(12, wx::EVT_COMMAND_NOTEBOOK_PAGE_CHANGED, PatternsEvents::onChangePageNotebook);
			frame.notebook.currProject = project;
			frame.notebook.window = frame;
			frame.notebook.connect(wx::EVT_KEY_DOWN, ConstituentsMouseEvents::onKeyDown);
			ShapeRAD::drawPatterns(project);
		}
	}
	trap e {
		mesgdlg =wx::messagedialog_construct(nil, "This .rproj is invalid for Raiden\n(Possibly project destroyed and can not be load it)", "Not allowed",
			wx::flags(wx::ICON_ERROR, wx::OK));
		mesgdlg.showmodal();
		project = nil;
		// refresh raiden draw
		AddrFuncs::RadEvents(#onMoveEvent) (frame, ev);
		return;
	}
}


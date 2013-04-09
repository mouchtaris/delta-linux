/**
 *	ExpressionTreeListView.dsc
 *
 *	-- Sparrow IDE Expression Tree List View extension --
 *
 *	Extension of the tree list view for expressions.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	August 2008
 *
 *	+ json decoder
 *
 *	Giannhs Apostolidhs <japostol@csd.uoc.gr>
 *	April 2013
 */

using std;
using #sparrowlib;
spw = sparrowlib::sparrow();

const classId = "ExpressionTreeListView";

window = nil;
base = nil;
mostbase = nil;

treeData = [];

//-------------------------------------------------------//
//---- Communication Protocol ---------------------------//
//NOTE: absoluteref is not used

function ParseAndAdaptXML(str) {
	const ATTRIBUTES = "$Attributes";
	const CHARDATA = "$CharData";

	function GetCharData(t) { return ((local t1 = t[0][CHARDATA]) ? t1[0] : ""); }
	function MakeAttribute(t, attribute) { t[attribute] = t[ATTRIBUTES][attribute]; }
	function MakeCharData(t, index) { t[index] = GetCharData(t[index]); }
	function MakeTypeValueItem(t, index) {		
		t[index] = [
			@type : t[index][0][ATTRIBUTES].type,
			@value : GetCharData(t[index])
		];
	}

	local t = xml::parse(str);
	if (std::isnil(t))
		return nil;
	local value;
	if (t.contents) {
		local contents = t.contents[0].content;
		contents.size = std::tablength(contents);
		contents.absoluteref = GetCharData(t.contents[0].absoluteref);
		contents.overview = GetCharData(t.contents[0].overview);
		
		for (local i = 0; i < contents.size; ++i) {
			MakeAttribute(contents[i], "subindex");
			MakeCharData(contents[i], "keyaccess");
			MakeTypeValueItem(contents[i], "displaydesc");
			MakeTypeValueItem(contents[i], "keytypeinfo");
			
			contents[i].fieldkeys = contents[i].fieldkeys[0].fieldkey;
			local size = std::tablength(contents[i].fieldkeys);
			contents[i].fieldkeys.size = size;		
			for(local j = 0; j < size; ++j) {
				local key = contents[i].fieldkeys[j];
				MakeTypeValueItem(key, "keycontentref");
				MakeCharData(key, "displayedkey");
			}
		}
		value = [@contents : contents];
	}
	else
		value = t[ATTRIBUTES].value;
	return [ @type: t[ATTRIBUTES].type, @value: value ];
}
xmlDll = nil;

function ParseAndAdaptJSON(str) {
	local t = json::parse(str, true);
	local value = nil;

	if(t)
		if(t.type == "COMPOSITE"){
			local contents = t.value.contents;
			contents.size = t.value.size;
			contents.absoluteref = t.value.absoluteref;
			contents.overview = t.value.overview;
		
			for (local i = 0; i < contents.size; ++i) {
				contents[i].fieldkeys.size = contents[i].size;	
			}
			value = [ @type: t.type, @value: [@contents : contents] ];
		}	
		else
			value = [ @type: t.type, @value: t.value ]; 
		
	return value;
}

jsonDll = nil;

//-----------------------------------------------------------------------

decoder = nil;
decoders = [];

//-----------------------------------------------------------------------

function InitializeDecoders() { 
	xmlDll = dllimportdeltalib(xml::DLL); 
	jsonDll = dllimportdeltalib(json::DLL);
}

function InstallDecoders() {
	decoders["rc"] = [
		@format : "rc",
		method @operator()(str){ return rcparse(str); }
	];
	decoders["xml"] = [
		@format : "xml",
		method @operator()(str){ return ParseAndAdaptXML(str); }		
	];
	decoders["json"] = [
		@format : "json",
		method @operator()(str){ return ParseAndAdaptJSON(str); }		
	];
}

function CleanupDecoders() {
	assert xmlDll;
	assert jsonDll;
	dllunimportdeltalib(xmlDll);
	dllunimportdeltalib(jsonDll);
}

function SelectDecoder(format) {
	assert decoders[format];
	decoder = decoders[format];
}

//-------------------------------------------------------//
//---- Constant Definitions -----------------------------//

const COMPOSITE 					= "COMPOSITE";
const NODESC						= "NODESC";
const VISIBLE						= "VISIBLE";
const NOKEYREF						= "NOKEYREF";
const KEYTYPE						= "KEYTYPE";

const OBJECT_PSEUDO_KEY				= "OBJECT_PSEUDO_KEY";
const OBJECT_INHERITANCE_PSEUDO_KEY	= "OBJECT_INHERITANCE_PSEUDO_KEY";
const OBJECT_DELEGATION_PSEUDO_KEY	= "OBJECT_DELEGATION_PSEUDO_KEY";
const OBJECT_ATTRIBUTE_KEY			= "OBJECT_ATTRIBUTE_KEY";
const OBJECT_OPERATOR_KEY			= "OBJECT_OPERATOR_KEY";
const EXTERNID_USER_PSEUDO_KEY		= "EXTERNID_USER_PSEUDO_KEY";
const EXTERNID_PSEUDO_KEY			= "EXTERNID_PSEUDO_KEY";
const AGGREGATE_PSEUDO_KEY			= "AGGREGATE_PSEUDO_KEY";

const COMPOSITE_KEY_ENTRY 			= "<compositekey>";
const MULTIPLE_KEY_ENTRY			= "<multiplekey>";
const KEY_INDEX						= "key";
const VALUE_INDEX					= "value";

//-------------------------------------------------------//
//---- Properties ---------------------------------------//

function SetProperties {
	function CreateColor(r, g, b) { return [ {.class : "wxColour"}, {.r : r}, {.g : g}, {.b : b} ]; }
	function AddFontProperty(id, label, desc, category) {
		local defaultFont = [ {.class : "wxFont"}, {.size : 8} ];
		spw::addclassproperty(
			classId,
			id,
			[
				{.class			: "FontProperty"},
				{.label			: label			},
				{.description	: desc			},
				{.category		: category		},
				{.value			: defaultFont	}
			]		
		);
	}
	function AddColorProperty(id, label, desc, color, category) {
		spw::addclassproperty(
			classId,
			id,
			[
				{.class			: "ColorProperty"	},
				{.label			: label				},
				{.description	: desc				},
				{.category		: category			},
				{.value			: color				}
			]		
		);	
	}
	function AddSpecialKeys(id, desc, color) {
		AddFontProperty(id + "KeysFont",
			"Font for the " + desc,
			"Sets the font for the " + desc,
			"Special Keys"
		);
		AddColorProperty(
			id + "KeysColor",
			"Color for the " + desc,
			"Sets the color for the " + desc,
			color,
			"Special Keys"
		);
	}

	AddSpecialKeys("inheritance", 	"inheritance pseudo keys",	CreateColor(160, 32, 240));	//PURPLE
	AddSpecialKeys("delegation",	"delegation pseudo keys",	CreateColor(139, 69, 19));	//SADLE BROWN
	AddSpecialKeys("operator",		"operator keys",			CreateColor(0, 100, 245));	//LIGHT BLUE
	AddSpecialKeys("attribute",		"attribute modified keys",	CreateColor(0, 100, 245));	//LIGHT BLUE
	AddSpecialKeys("externid", 		"externid user keys",		CreateColor(32, 178, 170));	//LIGHT SEA GREEN
	AddSpecialKeys("internal", 		"internal pseudo keys",		CreateColor(32, 178, 170));	//LIGHT SEA GREEN

	function AddModificationColors(id, color) {
		AddColorProperty(
			id + "ItemColor",
			"Color for the " + id + " treeview items",
			"Sets the color for the " + id + " treeview items",
			color,
			"Color encoding"
		);
	}
	AddModificationColors("default", 	CreateColor(0, 0, 0));		//BLACK
	AddModificationColors("new", 		CreateColor(34, 139, 34));	//FOREST GREEN
	AddModificationColors("modified", 	CreateColor(255, 0, 0));	//RED
}

function Highlight(id, colorId, fontId) {
	if (colorId != nil)
		window.SetTextColor(id, spw::getclassproperty(classId, colorId).value);
	if (fontId != nil)
		window.SetFont(id, spw::getclassproperty(classId, fontId).value);
}

SpecialKeys = [
	{ OBJECT_DELEGATION_PSEUDO_KEY	: "delegation" 	},
	{ OBJECT_INHERITANCE_PSEUDO_KEY : "inheritance" },
	{ OBJECT_OPERATOR_KEY 			: "operator"	},
	{ OBJECT_ATTRIBUTE_KEY 			: "attribute" },
	{ EXTERNID_USER_PSEUDO_KEY 		: "externid" },
	{ OBJECT_PSEUDO_KEY, AGGREGATE_PSEUDO_KEY, EXTERNID_PSEUDO_KEY : "internal" },
	
	method IsOperator(type) { return type == OBJECT_OPERATOR_KEY; },
	method GetKeyType(type)	{ return self[type]; },
	method Highlight(id, type) { ::Highlight(id, type + "KeysColor", type + "KeysFont"); }
];

function GetPropertyValue(id) { return spw::getclassproperty(classId, id).value; }

//-----------------------------------------------------------------------

function AppendItem(id, expr, value) { return window.Append(id, list_new(expr, value)); }
function InsertItem(id, pos, expr, value) { return window.Insert(id, pos, list_new(expr, value)); }

//-----------------------------------------------------------------------

function GetExpression(id) { return window.GetText(id, 0); }
function SetExpression(id, expr) { window.SetText(id, 0, expr); }

//-----------------------------------------------------------------------

function GetValue(id) { return window.GetText(id, 1); }
function SetValue(id, value) { window.SetText(id, 1, value); }

//-----------------------------------------------------------------------

function IsValidExpression(id)
{
	return GetExpression(id) != "";		//skip possible dummy entries
}

//-----------------------------------------------------------------------

function RemoveExpression(id)
{
	foreach(local child, window.GetChildren(id, true))
		if (IsValidExpression(child) and treeData[child])
			treeData[child] = nil;
	if (treeData[id])
		treeData[id] = nil;
	window.Remove(id);
}

//-----------------------------------------------------------------------

function InsertExpressionEx(parent, pos, displayedExpression, expression, str)
{
	local t = decoder(str);
	if (t) {	//expr evaluation may return a single string upon error so check decoder result
		assert t.type;
		local composite = t.type == COMPOSITE;
		local value = (composite ? t.value.contents.overview : t.value);
		local id = InsertItem(parent, pos, displayedExpression, value);
		t.expression = expression;
		t.expanded = not composite;
		if (composite)
			window.SetItemHasChildren(id, true);
		treeData[id] = t;
		return id;
	}
	else
		return InsertItem(parent, pos, displayedExpression, str);
}

//-----------------------------------------------------------------------

function InsertExpression(index, expr)
{
	local table = spw.components.DeltaVM.EvalExprEx(decoder.format, expr);
	assert typeof(table) == "Table" or typeof(table) == "Object";
	return InsertExpressionEx(window.GetRoot(), index, expr, expr, table.first);
}

//-----------------------------------------------------------------------

function AddContents(parent, startIndex, contents)
{
	assert treeData[parent];
	
	local expressions = list_new();
	for(local i = 0; i < contents.size; ++i) {
		local entry = contents[i];
		if (entry.keyaccess == VISIBLE) {
			local keys = entry.fieldkeys;
			for (local j = 0; j < keys.size; ++j)
				if (keys[j].keycontentref.type != NOKEYREF)
					expressions.push_back(keys[0].keycontentref.value);
		}
		expressions.push_back(treeData[parent].expression + entry.subindex);
	}
	
	local strs = spw.components.DeltaVM.EvalExprListEx(decoder.format, expressions);
	assert typeof(strs) == TYPEOF_EXTERNID and externidtype(strs) == "std::list";
	if (expressions.total() != strs.total())	//error getting the strs
		return [];

	local added = [];
	local exprIter = listiter_new(), exprIter.setbegin(expressions);
	local strIter = listiter_new(), strIter.setbegin(strs);
	for(local i = 0, local id = 0, local index = startIndex; i < contents.size; ++i, ++index) {
		local entry = contents[i];
		local desc = (entry.displaydesc.type == NODESC ? "" : entry.displaydesc.value);
		if (entry.keyaccess == VISIBLE) {
			function GenerateKeyStr(desc, value)
				{ return (desc == "" ? value : desc + "(" + value + ")"); }
			local keys = entry.fieldkeys;
			if (keys.size == 1) {
				if (keys[0].keycontentref.type == NOKEYREF) {
					local keyStr = desc + keys[0].displayedkey;
					local keyinfo = [];
					if (entry.keytypeinfo.type == KEYTYPE) {
						local value = entry.keytypeinfo.value;
						keyinfo.operator = SpecialKeys.IsOperator(value);
						keyinfo.value = SpecialKeys.GetKeyType(value);
					}
					if (keyinfo.operator)
						keyStr = "operator " + keyStr;
					id = InsertExpressionEx(parent, index, keyStr, exprIter.getval(), strIter.getval());
					if (keyinfo.value)
						SpecialKeys.Highlight(id, keyinfo.value);
					exprIter.fwd(), strIter.fwd();
				}
				else {
					id = InsertItem(parent, index, COMPOSITE_KEY_ENTRY, "");
					local keyId = InsertExpressionEx(id, 0, GenerateKeyStr(desc, KEY_INDEX), exprIter.getval(), strIter.getval());
					exprIter.fwd(), strIter.fwd();
					
					treeData[id] = [{.expression : exprIter.getval()}, {.extra : COMPOSITE_KEY_ENTRY}, {.expanded : true}];
					local valueId = InsertExpressionEx(id, 1, GenerateKeyStr(desc, VALUE_INDEX), exprIter.getval(), strIter.getval());
					exprIter.fwd(), strIter.fwd();
					
					SetValue(id, "<" + GetValue(keyId) + ":" + GetValue(valueId) + ">");
					SpecialKeys.Highlight(id, SpecialKeys.GetKeyType(OBJECT_PSEUDO_KEY));
				}
			}
			else {
				id = InsertItem(parent, index, MULTIPLE_KEY_ENTRY, "");
				local extraStr = "<";
				for (local j = 0; j < keys.size; ++j) {
					local keyId = 0;
					local keyStr = GenerateKeyStr(desc, KEY_INDEX + (j + 1));
					if (keys[j].keycontentref.type == NOKEYREF)
						keyId = InsertItem(id, j, keyStr, keys[j].displayedkey);
					else {
						keyId = InsertExpressionEx(id, j, keyStr, exprIter.getval(), strIter.getval());
						exprIter.fwd(), strIter.fwd();
					}
					if (j != 0)
						extraStr += ",";
					extraStr += GetValue(keyId);
				}
				treeData[id] = [{.expression : exprIter.getval()}, {.extra : MULTIPLE_KEY_ENTRY}, {.expanded : true}];
				local valueId = InsertExpressionEx(id, keys.size, GenerateKeyStr(desc, VALUE_INDEX), exprIter.getval(), strIter.getval());
				exprIter.fwd(), strIter.fwd();
				SetValue(id, extraStr + ":" + GetValue(valueId) + ">");
				SpecialKeys.Highlight(id, SpecialKeys.GetKeyType(OBJECT_PSEUDO_KEY));
			}
		}
		else {
			id = InsertExpressionEx(parent, index, desc, exprIter.getval(), strIter.getval());
			exprIter.fwd(), strIter.fwd();
		}
		added[i] = id;
	}
	return added;
}

//-----------------------------------------------------------------------

function UpdateMultipleIndexEntry(id)
{
	local children = window.GetChildren(id, false);
	local modified = false;
	local value = "<";
	local lastChild = children.pop_back();
	local first = true;
	assert children.size();
	foreach(local child, children) {
		if (callglobal("UpdateExpression", child).second)
			modified = true;
		if (first)
			first = false;
		else
			value += ",";
		value += GetValue(child);
	}
	value += ":" + GetValue(lastChild) + ">";
	if (modified) {
		SetValue(id, value);
		Highlight(id, "modifiedItemColor", nil);
	}
	else
		Highlight(id, "defaultItemColor", nil);
	return modified;
}

//-----------------------------------------------------------------------

function UpdateExpression(id)
{
	local expression = (treeData[id] ? treeData[id].expression : GetExpression(id));
	local prevValue = GetValue(id);
	local modified = false;
	
	local table = spw.components.DeltaVM.EvalExprEx(decoder.format, expression);
	assert (typeof(table) == "Table" or typeof(table) == "Object") and table.first;
	local str = table.first;
	if (table.second and (local t = decoder(str))) {	//expr evaluation may return a single string upon error so check decoder result
		assert t.type;
		local composite = t.type == COMPOSITE;
		local value = (composite ? t.value.contents.overview : t.value);
		t.expression = expression;
		if (not composite) {
			local parent = window.GetParent(id);
			local index = window.GetRelativePosition(id);
			local expr = GetExpression(id);
			RemoveExpression(id);
			id = InsertItem(parent, index, expr, value);
		}
		else {
			SetValue(id, value);
			local children = window.GetChildren(id, false);
			assert typeof(children) == TYPEOF_EXTERNID and externidtype(children) == "std::list";
			if (children.total() == 0) {
				//Previous was never expanded or simple value so do nothing special
				window.SetItemHasChildren(id, true);
				t.expanded = false;
			}
			else {
				local contents = t.value.contents;

				// First check for new entries
				for(local i = 0; i < contents.size; ++i) {
					local iter = listiter_new();
					for(iter.setbegin(children); not iter.checkend(children); iter.fwd())	//skip the .class entry
						if (treeData[ iter.getval() ].expression == expression + contents[i].subindex)
							break;
					if (iter.checkend(children)) {	//not found in old so add
						local added = AddContents(id, i, [contents[i], {.size : 1}]);
						assert tablength(added) == 1;
						Highlight(added[0], "newItemColor", nil);
					}
				}

				//Then check for removed or updated entries
				foreach(local child, children) {
					for(local j = 0; j < contents.size; ++j)
						if (treeData[child].expression == expression + contents[j].subindex)
							break;
					if (j == contents.size)		//not found
						RemoveExpression(child);
					else if(not treeData[child].extra)	// found normal entry
						UpdateExpression(child);
					else
						UpdateMultipleIndexEntry(child);
				}
				t.expanded = true;
			}
		}
		treeData[id] = t;
		modified = value != prevValue;
	}
	else if (str != prevValue) {
		SetValue(id, str);
		modified = true;
	}

	Highlight(id, (modified ? "modifiedItemColor" : "defaultItemColor"), nil);
	return [.first : id + "", .second : modified, .class : "Pair"];
}

//-----------------------------------------------------------------------

function GetFullExpression(id)
{
	return (treeData[id] ? treeData[id].expression : GetExpression(id));
}

//-----------------------------------------------------------------------

function Clear()
{
	treeData = [];
	base.Clear();
}

//-----------------------------------------------------------------------

function ItemExpanding(id)
{
	local t = treeData[id];
	if (t and not t.expanded) {
		t.expanded = true;
		local contents = t.value.contents;
		assert contents;
		AddContents(id, 0, contents);
	}
}

//-----------------------------------------------------------------------

function onTreeListItemExpanding(invoker, id)
{
	if (invoker.class_id != mostbase.class_id or invoker.serial != mostbase.serial)
		return;
	ItemExpanding(id);
}

//-----------------------------------------------------------------------

function onExpressionEvaluationFormatChanged(invoker, format)
{
	SelectDecoder(format);
}

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId, "TreeListViewComponent");
	spw::setcomponentmetadata(
		classId, "ExpressionTreeListView",
		"Tree view for expressions",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);

	spw::class_decl_required_member_function(classId, "InsertExpression", "uint (uint id, String expr)",
		"Insert the given expression to the tree list view at the given node");
	spw::class_decl_required_member_function(classId, "RemoveExpression", "void (uint id)",
		"Remove the given node from the tree list view");
	spw::class_decl_required_member_function(classId, "GetFullExpression", "String (uint id)",
		"Get the expression of the given node");
	spw::class_decl_required_member_function(classId, "UpdateExpression", "StringBoolPair (uint id)",
		"Update the expression for the given node");
	spw::class_decl_required_member_function(classId, "Clear", "void (void)",
		"Clear the tree list view");
	spw::class_decl_required_member_function(classId, "SelectDecoder", "void (String format)",
		"Selects the decoder for the expression evaluation");
	spw::class_decl_required_member_handler(classId, "TreeListItemExpanding");
	spw::class_decl_required_member_handler(classId, "ExpressionEvaluationFormatChanged");

	spw::class_decl_required_member_command(
		[
			{.class			: "UserCommandDesc"					},
			{.class_id		: classId							},
			{.function_name	: "ConfigureExpressionTreeListView"	},
			{.flags			: 1									}
		],
		"ConfigureExpressionTreeListView",
		"Configure the tree view used for inspecting expressions (Watches, Quickwatch, ...)"
	);
	
	SetProperties();
	InitializeDecoders();
}

//-----------------------------------------------------------------------

onevent ClassUnload { CleanupDecoders(); }

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_function(classId, "InsertExpression", InsertExpression);
	spw::inst_impl_required_member_function(classId, "RemoveExpression", RemoveExpression);
	spw::inst_impl_required_member_function(classId, "GetFullExpression", GetFullExpression);
	spw::inst_impl_required_member_function(classId, "UpdateExpression", UpdateExpression);
	spw::inst_impl_required_member_function(classId, "Clear", Clear);
	spw::inst_impl_required_member_function(classId, "SelectDecoder", SelectDecoder);
	spw::inst_impl_required_member_handler(classId, "TreeListItemExpanding", onTreeListItemExpanding);
	spw::inst_impl_required_member_handler(classId, "ExpressionEvaluationFormatChanged", onExpressionEvaluationFormatChanged);

	spw::inst_impl_required_member_command(classId, "ConfigureExpressionTreeListView", 
		(function Configure {spw.components.Shell.ConfigureComponent(classId); }));
		
	InstallDecoders();
	local debugger = spw::getproperty("DeltaVM", "debugger");
	assert debugger;
	SelectDecoder(debugger.properties.value.expression_evaluation_format.value);
}

//-----------------------------------------------------------------------

onevent Destructor
{
	//the component may be destroyed without being removed, so do this anyway
	local shell = spw.components.Shell;
	if (shell.serial != 0)
		shell.RemoveComponent(window);
}

//-----------------------------------------------------------------------

onevent GenerateWindow
{
	base = spw.decorate(spw::basecomponent());
	mostbase = spw.decorate(spw::mostbasecomponent());
	window = spw.decorate(spw::thiscomponent());
	window.SetRootVisibility(false);
}

//-----------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////
// UI Builder
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;
using wx;
using corba;

//
// Library register
//

/*
libs::registershared("MicroUIs", "MicroUIs.dbc");
libs::registershared("PropertyUIMappingRules", "PropertyUIMappingRules.dbc");
libs::registershared("Properties", "Properties.dbc");
libs::registershared("PropertyGenerator", "PropertyGenerator.dbc");
libs::registershared("UIAPISpec", "UIAPISpec.dbc");
libs::registershared("UIAPISpecValidator", "UIAPISpecValidator.dbc");
libs::registershared("UIAPIHolder", "UIAPIHolder.dbc");
libs::registershared("CORBAMetadataUIAPISpecConverter", "CORBAMetadataUIAPISpecConverter.dbc");
libs::registershared("TopLevelUIs", "TopLevelUIs.dbc");
libs::registershared("PropertySelectionHolder", "PropertySelectionHolder.dbc");
libs::registershared("PropertySelectionUIs", "PropertySelectionUIs.dbc");
*/

//
// Library and VM import
//

wxDll = dllimportdeltalib(wx::DLL);
assert(wxDll);

const corbaTest = false;
if (corbaTest)
	corbaDll = dllimportdeltalib(corba::CLIENT_DLL);
assert(not corbaTest or corbaDll);

/*
Properties = libs::import("Properties");
MicroUIs = libs::import("MicroUIs");
PropertyGenerator = libs::import("PropertyGenerator");
UIAPISpec = libs::import("UIAPISpec");
UIAPISpecValidator = libs::import("UIAPISpecValidator");
CORBAMetadataUIAPISpecConverter = libs::import("CORBAMetadataUIAPISpecConverter");
TopLevelUIs = libs::import("TopLevelUIs");
PropertySelectionHolder = libs::import("PropertySelectionHolder");
*/

Properties = nil;
MicroUIs = nil;
PropertyGenerator = nil;
UIAPISpec = nil;
UIAPISpecValidator = nil;
CORBAMetadataUIAPISpecConverter = nil;
TopLevelUIs = nil;
PropertySelectionHolder = nil;

/*
assert(PropertyGenerator);
assert(UIAPISpec);
assert(UIAPISpecValidator);
assert(CORBAMetadataUIAPISpecConverter);
assert(MicroUIs);
assert(Properties);
assert(TopLevelUIs);
assert(PropertySelectionHolder);
*/

//
// Constants
//

const nl = "\n";

//
//
//

//app = app_construct();

//
//
//

function RegisterLibs (basePath) {
	libs::registershared("MicroUIs", basePath + "MicroUIs.dbc");
	libs::registershared("PropertyUIMappingRules", basePath + "PropertyUIMappingRules.dbc");
	libs::registershared("Properties", basePath + "Properties.dbc");
	libs::registershared("PropertyGenerator", basePath + "PropertyGenerator.dbc");
	libs::registershared("UIAPISpec", basePath + "UIAPISpec.dbc");
	libs::registershared("UIAPISpecValidator", basePath + "UIAPISpecValidator.dbc");
	libs::registershared("UIAPIHolder", basePath + "UIAPIHolder.dbc");
	libs::registershared("CORBAMetadataUIAPISpecConverter", basePath + "CORBAMetadataUIAPISpecConverter.dbc");
	libs::registershared("TopLevelUIs", basePath + "TopLevelUIs.dbc");
	libs::registershared("PropertySelectionHolder", basePath + "PropertySelectionHolder.dbc");
	libs::registershared("PropertySelectionUIs", basePath + "PropertySelectionUIs.dbc");
	
	::Properties = libs::import("Properties");
	::MicroUIs = libs::import("MicroUIs");
	::PropertyGenerator = libs::import("PropertyGenerator");
	::UIAPISpec = libs::import("UIAPISpec");
	::UIAPISpecValidator = libs::import("UIAPISpecValidator");
	::CORBAMetadataUIAPISpecConverter = libs::import("CORBAMetadataUIAPISpecConverter");
	::TopLevelUIs = libs::import("TopLevelUIs");
	::PropertySelectionHolder = libs::import("PropertySelectionHolder");
}

//
//
//

function CreateGUI (frame, uiAPI) {
	frame.settitle(uiAPI.GetId());
	treebook = TopLevelUIs.TreeBookCompositeUI(frame, "test_treebooktoplevelui");
	foreach (op, uiAPI) {
		local opUI = TopLevelUIs.OperationUI(treebook, op);
		treebook.AddTopLevelUI(opUI);
		frame.fit();
	}
}

function GetUIAPISpecPathFromDialog (parent) {
	filedialog = filedialog_construct(parent, "Open UIAPISpec file", "", "", 
		"dbc files (*.dbc)|*.dbc");
	filedialog.showmodal();
	print(filedialog.getpath(), nl);
	local path = filedialog.getpath();
	filedialog.destroy();
	return path;
}

function oninitfunc() {
	local frame = frame_construct(nil, "ID_ANY", "test");
	
	try {
		local apiSpec;
		if (corbaTest) {
			if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
				throw("Unable to initialize corba services\n");
		
			local service = corba::connect("idl_types");
			apiSpec = CORBAMetadataUIAPISpecConverter.CreateUIAPISpecFromCORBAService(service);
		}
		else {
			apiSpec = UIAPISpec.GetAPISpec();	
		}
		
		UIAPISpecValidator.ValidateUIAPISpec(apiSpec);
		print("+ UIAPISpec validation OK", nl, nl);

		print("+ Generating operations...", nl, nl);
		
		print("+ Generating GUI...",nl, nl);
		
		local uiAPI = PropertyGenerator.GenerateUIAPIfromUIAPISpec(apiSpec, "test API");
		local propertySelectionHolder = PropertySelectionHolder.GetInstance();
		propertySelectionHolder.RegisterAvailableClassIds(uiAPI);
		CreateGUI(frame, uiAPI);
		
		frame.center();
		frame.show();
	}
	trap errorList {
		local s = "";
		if (typeof(errorList) == "String")
			s = errorList;
		else {
			foreach(e, errorList)
				s += e + nl;
		}
		
		dialog = messagedialog_construct(frame, s, "Error in UIAPISpec validation", flags("OK", "ICON_ERROR"));
		dialog.showmodal();
		frame.destroy();
	}
	
}

function CreateFrame (UIAPISpec) {
	local frame = frame_construct(nil, "ID_ANY", "test");
	
	try {
		const corbaTest = false;
		local apiSpec;
		if (corbaTest) {
			if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
				throw("Unable to initialize corba services\n");
		
			local service = corba::connect("idl_types");
			apiSpec = CORBAMetadataUIAPISpecConverter.CreateUIAPISpecFromCORBAService(service);
		}
		else {
			apiSpec = UIAPISpec.GetAPISpec();	
		}
		
		UIAPISpecValidator.ValidateUIAPISpec(apiSpec);
		print("+ UIAPISpec validation OK", nl, nl);

		print("+ Generating operations...", nl, nl);
		
		print("+ Generating GUI...",nl, nl);
		
		local uiAPI = PropertyGenerator.GenerateUIAPIfromUIAPISpec(apiSpec, "test API");
		local propertySelectionHolder = PropertySelectionHolder.GetInstance();
		propertySelectionHolder.RegisterAvailableClassIds(uiAPI);
		CreateGUI(frame, uiAPI);
		
		return frame;
	}
	trap errorList {
		local s = "";
		if (typeof(errorList) == "String")
			s = errorList;
		else {
			foreach(e, errorList)
				s += e + nl;
		}
		
		dialog = messagedialog_construct(frame, s, "Error in UIAPISpec validation", flags("OK", "ICON_ERROR"));
		dialog.showmodal();
		frame.destroy();
	}
	
}

//
// wx app run
//
/*
app_onrunadd(app, oninitfunc);
app_start(app);

libs::unimport(PropertyGenerator);
libs::unimport(UIAPISpec);
libs::unimport(UIAPISpecValidator);
libs::unimport(CORBAMetadataUIAPISpecConverter);
dllunimport(wxDll);
dllunimport(corbaDll);
*/
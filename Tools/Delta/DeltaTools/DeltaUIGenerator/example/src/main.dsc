using std;
using wx;
using corba;

//
// Library register
//

const UIBuilderPath = "../../dbc/";


libs::registercopied("UIAPI_Spec", "uiapispec.dbc");
libs::registershared("UIBuilder", UIBuilderPath + "UIBuilder.dbc");

UIAPISpec = libs::import("UIAPI_Spec");
UIBuilder = libs::import("UIBuilder");

//
// wx bootstrap function
//

function oninitfunc() {
	UIBuilder.RegisterLibs(UIBuilderPath);
	local frame = UIBuilder.CreateFrame(UIAPISpec);
	
	frame.center();
	frame.show();
}

app = app_construct();
app_onrunadd(app, oninitfunc);
app_start(app);

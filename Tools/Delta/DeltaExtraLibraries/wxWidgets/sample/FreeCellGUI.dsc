dll2 = std::dllimportdeltalib("..\\lib\\debug\\WxWidgets_d.dll");
using wx;

function oninitfunc()
{
	frame = frame_construct(nil, "ID_ANY", "FreeCell");
	frame.show(true);
}

//app = app_construct();

//app.oninitadd(oninitfunc);
//app.start();

std::dllunimport(dll2);
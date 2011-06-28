using std;
const nl = "\n";
dll = dllimportdeltalib(corba::CLIENT_DLL);
assert dll;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {
	try {
		local ns = corba::resolve_initial_references("NameService");
		echo = ns.resolve_str("DeferredEcho");
		if (echo == nil)
			print("Unable to connect to service\n");
		else {
			local call = echo.invoke_deferred("echo_string", "cruel", x = [], y = ["test"]);
			while(not call.isfinished())
				print("Not done yet: out = ", x[0], ", inout = ", y[0], nl);
			local reply = call.getresponse();
			print(x[0], " ", reply, " ", y[0], nl);
			echo.shutdown();
		}
	}
	trap x { print(x); }
	corba::cleanup();
}
dllunimport(dll);
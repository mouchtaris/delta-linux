using std;
const nl = "\n";
dll = dllimportdeltalib(corba::CLIENT_DLL);
assert dll;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {
	try {
		echo = corba::connect("Echo");
		if (echo == nil)
			print("Unable to connect to service\n");
		else {
			print(echo.echo_string("This is a string"), nl);
			
			local list = echo.echo_list("This will return some objects");
			foreach(obj, list)
				print(obj, nl);

			echo.shutdown();
		}
	}
	trap x { print(x); }
	corba::cleanup();
}
dllunimport(dll);
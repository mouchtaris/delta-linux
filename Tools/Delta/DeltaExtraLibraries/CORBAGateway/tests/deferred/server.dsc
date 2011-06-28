using std;
server_dll = dllimportdeltalib(corba::SERVER_DLL);
assert server_dll;
client_dll = dllimportdeltalib(corba::CLIENT_DLL);
assert client_dll;

echo_server = nil;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {	
	echo_implementation = [
		method echo_string(str, out, inout) {
			print("echo_string: ", str, " ", inout[0], "\n");
			out[0] = "Bye bye";
			inout[0] = "world";
			return str;
		},
		method shutdown() { corba::shutdown(); }
	];
	
	echo_server = corba::server("IDL:DeferredEcho:1.0", echo_implementation);
	assert echo_server;
	
	try {
		local ns = corba::resolve_initial_references("NameService");
		local name = ns.to_name("DeferredEcho");
		ns.rebind(name, echo_server);
		print(echo_server.ior(), "\n");
		corba::run();
		corba::cleanup();
	}
	trap x { print(x); }
}
dllunimport(server_dll);
dllunimport(client_dll);
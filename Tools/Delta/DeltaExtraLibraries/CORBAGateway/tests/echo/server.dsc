using std;
server_dll = dllimportdeltalib(corba::SERVER_DLL);
assert server_dll;

echo_server = nil;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {	
	echo_implementation = [
		method echo_string(str) {
			print("echo_string: ", str, "\n");
			return str;
		},
		
		method echo_list(str) {
			print("echo_list: ", str, "\n");
			return [echo_server, echo_server, echo_server];	//just return self 3 times
		},
		
		method shutdown() { corba::shutdown(); }
	];
	
	echo_server = corba::server("IDL:Echo:1.0", echo_implementation);
	assert echo_server;
	
	if (not echo_server.register("Echo"))
		print("Unable to register server\n");
	else {
		print(echo_server.ior(), "\n");
		corba::run();
	}
	corba::cleanup();
}
dllunimport(server_dll);
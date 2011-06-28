using std;
server_dll = dllimportdeltalib(corba::SERVER_DLL);
assert server_dll;

Interface1_server = nil;
server = nil;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {
	Interface1_implementation = [
		@innerAttr { 
			@set (method() { assert false; })	//readonly
			@get (method() { return 1821; })
		}
	];
	Interface1_server = corba::server("IDL:TestModule/Interface1:1.0", Interface1_implementation);
	assert Interface1_server;
	
	implementation = [
		@x { 
			@set (method() { assert false; })	//readonly
			@get (method() { return 15; })
		},
		
		@str { 
			@set (method(str) { @str = str; })
			@get (method() { return @str; })
		},
		{.str : "hello"},
		
		method echo_string(str) {
			print("echo_string: ", str, "\n");
			return str;
		},

		method echo_list(str) {
			print("echo_list: ", str, "\n");
			return [server, server, server];	//just return self 3 times
		},

		method TestExtern(point) {
			print("TestExtern: ", point, "\n");
			return Interface1_server;
		},

		method test(str, out, inout) {
			print("test: ", str, " ", inout[0], "\n");
			inout[0] = 35;
			out[0] = "lala";
			if (str != "a")
				throw [
					{.id : "IDL:Generic/TestException:1.0" },
					{.value: [{.msg : "Message!"}] }
				];
			return true;
		},
		
		method TestEnum(c) {
			print("TestEnum: ", c, "\n");
			return corba::enum("Generic::dollar");
		},
		
		method TestAny(any) {
			print("TestAny: ", any, "\n");
			return Interface1_server;
		},
		
		method TestBool(value) {
			print("TestBool: ", value, "\n");
			return true;
		},

		method TestUnion(union) {
			print("TestUnion: ", union, "\n");
			return union;
		},

		method TestUnionBool(union) {
			print("TestUnionBool: ", union, "\n");
			return union;
		},

		method TestUnionEnum(union) {
			print("TestUnionEnum: ", union, "\n");
			return union;
		},

		method TestStruct(point) {
			print("TestStruct: ", point, "\n");
			return [ {.p : point}, {.radius : 10} ];
		},

		method TestChar(c) {
			print("TestChar: ", c, "\n");
			return "a";
		},

		method shutdown() { corba::shutdown(); }
	];
	
	server = corba::server("IDL:Generic:1.0", implementation);
	assert server;
	
	if (not server.register("Foo/Lala"))
		print("Unable to register server\n");
	else {
		print(server.ior(), "\n");
		corba::run();
	}
	corba::cleanup();
}
dllunimport(server_dll);
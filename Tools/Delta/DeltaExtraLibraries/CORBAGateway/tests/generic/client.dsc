using std;
dll = dllimportdeltalib(corba::CLIENT_DLL);
assert dll;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {
	try {
		client = corba::connect("Foo/Lala");
		if (client == nil)
			print("Unable to connect to service\n");
		else {			
			print(client.TestBool(false), "\n");
			print(client.TestAny(client), "\n");
			print(client.TestEnum(corba::enum("franc")), "\n");

			print(client.TestUnion(corba::union("stringFormat", "stringValue")), "\n");
			print(client.TestUnion(corba::union(10)), "\n");			
			print(client.TestUnionBool(corba::union("digitalFormat", 5)), "\n");
			print(client.TestUnionEnum(corba::union("poundFormat", "poundValue")), "\n");
			
			print(client.TestStruct([ {.coords : [25, 40]} ]), "\n");
			print(client.TestChar("A"), "\n");
			list = client.echo_list("lala");
			print(list);
			print(client.test("a", t = [], x = [10]), "\n");
			print(t[0], "\n");
			print(x[0], "\n");
			client.shutdown();
		}
	}
	trap x {
		print(x);
	}
	corba::cleanup();
}
dllunimport(dll);
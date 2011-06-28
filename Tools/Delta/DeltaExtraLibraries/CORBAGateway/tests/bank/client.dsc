using std;
const nl = "\n";
dll = dllimportdeltalib(corba::CLIENT_DLL);
assert dll;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {
	try {
		manager = corba::connect("AccountManager");
		if (manager == nil)
			print("Unable to connect to service\n");
		else {
			local foo = manager.open("foo", 100, true);
			local bar = manager.open("bar", 2000, false);
			foo.withdrawAll();
			print("Balance for foo: ", foo.balance, "\n");
			foo.deposit(200);
			print("Balance for foo: ", foo.balance, "\n");
			print("Balance for bar: ", bar.balance, "\n");
			manager.close(foo);
			manager.close(bar);
			manager.shutdown();
		}
	}
	trap x { print(x); }
	corba::cleanup();
}
dllunimport(dll);
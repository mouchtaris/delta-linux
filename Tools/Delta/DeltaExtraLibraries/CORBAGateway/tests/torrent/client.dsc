using std;
const nl = "\n";
dll = dllimportdeltalib(corba::CLIENT_DLL);
assert dll;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {
	try {
		local client = corba::connect("TorrentClient");
		if (client == nil)
			print("Unable to connect to service\n");
		else {
			local torrent = client.Create(["lala.txt"], "www.torrenttracker.org", "Due Date (2010)", 100);
			print(torrent.seeders);
			client.shutdown();
		}
	}
	trap x { print(x); }
	corba::cleanup();
}
dllunimport(dll);
using std;
server_dll = dllimportdeltalib(corba::SERVER_DLL);
assert server_dll;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {
	function create_torrent(tracker, name, pieces, piece_size) {
		local torrent_implementation = [
			@tracker {
				@set (method(val) { @tracker = val; })
				@get (method() { return @tracker; })
			},
			{.tracker : name },
			@name {
				@set (method(val) { @name = val; })
				@get (method() { return @name; })
			},
			{.name : name },
			@seeders {
				@set (method(val) { @seeders = val; })
				@get (method() { return @seeders; })
			},
			{.seeders : 1 },
			@leechers {
				@set (method(val) { @leechers = val; })
				@get (method() { return @leechers; })
			},
			{.leechers : 0 },
			@pieces {
				@set (method(val) { @pieces = val; })
				@get (method() { return @pieces; })
			},
			{.pieces : pieces },
			@piece_size {
				@set (method(val) { @piece_size = val; })
				@get (method() { return @piece_size; })
			},
			{.piece_size : piece_size }
		];
		torrent = corba::server("IDL:TorrentClient/Torrent:1.0", torrent_implementation);
		assert torrent;
		return torrent;
	}

	local torrent_client_implementation = [
		@max_download_speed {
			@set (method(val) { @max_download_speed = val; })
			@get (method() { return @max_download_speed; })
		},
		{.max_download_speed : 0 },
		@max_upload_speed {
			@set (method(val) { @max_upload_speed = val; })
			@get (method() { return @max_upload_speed; })
		},
		{.max_upload_speed : 0 },
		@max_global_connections {
			@set (method(val) { @max_global_connections = val; })
			@get (method() { return @max_global_connections; })
		},
		{.max_global_connections : 0 },
		@max_connections_per_torrent {
			@set (method(val) { @max_connections_per_torrent = val; })
			@get (method() { return @max_connections_per_torrent; })
		},
		{.max_connections_per_torrent : 0 },
		@pieces {
			@set (method(val) { @pieces = val; })
			@get (method() { return @pieces; })
		},
		{.pieces : pieces },
		@max_active_torrents {
			@set (method(val) { @max_active_torrents = val; })
			@get (method() { return @max_active_torrents; })
		},
		{.max_active_torrents : 0 },
		{.pieces : pieces },
		@default_download_directory {
			@set (method(val) { @default_download_directory = val; })
			@get (method() { return @default_download_directory; })
		},
		{.default_download_directory : "c:/downloads/" },
		{.accounts : []},
		method Create(files, tracker, name, piece_size) {
			print(|arguments|);
			const pieces = 70;
			return create_torrent(tracker, name, pieces, piece_size);
		},		
		method shutdown() { corba::shutdown(); }
	];
	
	torrent_client = corba::server("IDL:TorrentClient/Client:1.0", torrent_client_implementation);
	assert torrent_client;
	
	if (not torrent_client.register("TorrentClient"))
		print("Unable to register server\n");
	else {
		print(torrent_client.ior(), "\n");
		corba::run();
	}
	corba::cleanup();
}
dllunimport(server_dll);

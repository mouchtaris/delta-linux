using std;
&using #protocol;
server = socket_createforserver(2323);
print("Waiting for client to connect...\n");

funcs = [
	method add(x, y) { return x + y; },
	method mul(x, y, z) { return x * y * z; }
];

&function unmarshal(proto, client, func, args) {
	local readerVar = <<r>>;
	return protocol::receive(
		client,
		readerVar,
		<<
			local ~func = (~readerVar)["read_" + ~(proto.func)]();
			local total_args = (~readerVar)["read_" + ~(proto.total_args)]();
			local ~args = [];
			for (local i = 0; i < total_args; ++i)
				args[i] = (~readerVar)["read_" + ~(proto.args)]();
		>>
	);
}

if (client = server.acceptclient(true, 0)) {
	print("A client was connected.\n");
	while (not client.isconnectionbroken())
		if (client.ismessagepending(std::SOCKET_PACKET_ORIGINAL)) {
			
			&p = protocol::Protocol();
			!(unmarshal(p, <<client>>, <<func>>, <<args>>));
			local retval = funcs[func](|args|);
			!(protocol::send(<<client>>, <<w>>, <<w["write_" + ~(p.retval)](retval);>>));
		}
}

socket_destroy(client);
socket_destroy(server);

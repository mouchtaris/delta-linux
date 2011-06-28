// Server
using std;
server = socket_createforserver(2323);
print("Waiting for client to connect...\n");

if (client = server.acceptclient(true, 0)) {
	print("A client was connected.\n");
	while (not client.isconnectionbroken())
		if (client.ismessagepending(std::SOCKET_PACKET_ORIGINAL)) {
			r = reader_frominputbuffer(client.receive(std::SOCKET_PACKET_ORIGINAL));
			print(r.read_string(), "\n");
		}
}

socket_destroy(client);
socket_destroy(server);

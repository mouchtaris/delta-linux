// Client
using std;
client = socket_createforclient();
print("Trying to connect to server...\n");
client.connecttoserver("localhost", 2323, 10000);
  
if (not client.isconnectionbroken()) {
	print("Connected to server.\n");
	w = writer_fromoutputbuffer(ob = outputbuffer_new());
	w.write_string("hello, world");
	client.send(ob, std::SOCKET_PACKET_ORIGINAL);
}

socket_destroy(client);
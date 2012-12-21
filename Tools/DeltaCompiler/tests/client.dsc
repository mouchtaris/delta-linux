using std;
&using #protocol;

&function call(proto, client, func ...) {
	local writerVar = <<w>>;
	local args = nil;
	for(local i = 3; i < arguments.total(); ++i)
		args = <<~args; (~writerVar)["write_" + ~(proto.args)](~(arguments[i]));>>;
	
	local outgoing = protocol::send(
		client,
		writerVar,
		<<
			(~writerVar)["write_" + ~(proto.func)](~func);
			(~writerVar)["write_" + ~(proto.total_args)](~(arguments.total() - 3));
			~args;
		>>
	);
	local incomming = protocol::receive(
		client,
		<<r>>,
		<<local retval = r["read_" + ~(proto.retval)]();>>
	);

	return <<(function { ~outgoing; ~incomming; return retval;})()>>;
}

client = socket_createforclient();
print("Trying to connect to server...\n");
client.connecttoserver("localhost", 2323, 10000);
if (not client.isconnectionbroken()) {
	print("Connected to server.\n");

	&p = protocol::Protocol();
	print("1 + 2 = ", !(call(p, <<client>>, "add", <<1>>, <<2>>)), "\n");
	print("1 * 2 * 3 = ", !(call(p, <<client>>, "mul", <<1>>, <<2>>, <<3>>)), "\n");
}

socket_destroy(client);
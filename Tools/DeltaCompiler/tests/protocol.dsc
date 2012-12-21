function Protocol() {
	return [
		@func		: "string",
		@total_args	: "ui32",
		@args		: "real64",
		@retval		: "real64"
	];
}

function send(client, writerVar, data) {
	return <<
		{
			local ob = outputbuffer_new();
			local ~writerVar = writer_fromoutputbuffer(ob);
			~data;
			~client.send(ob, std::SOCKET_PACKET_ORIGINAL);
		}
	>>;
}

function receive(client, readerVar, data) {
	return <<
		local ~readerVar = reader_frominputbuffer(~client.blockreceive(std::SOCKET_PACKET_ORIGINAL));
		~data;
	>>;
}
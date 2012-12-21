&function Cascading (code, handlers) {
	local ex = <<$ex>>;
	local handler = nil;
	foreach(local index : local value, handlers) {
		value.inject(ex);
		handler = <<~handler, {~index : function() {~value;} }>>;
	}

	return <<
		try { ~code; }
		trap ~ex {
			local h = [~handler];
			if (h[~ex.class])
				h[~ex.class]();
			else
				throw ~ex;
		}
	>>;
}

function f(x) {
	if (x == 1)
		throw [@class : "Exception1"];
	else if (x == 2)
		throw [@class : "Exception2"];
	else if (x == 3)
		throw [@class : "Exception3"];
	else
		throw [@class : "GenericException"];
}

&handlers = [
	{ "Exception1" : <<std::print(~~ex, "\n")>> },
	{ "Exception2" : <<std::print(~~ex, "\n")>> },
	{ "Exception3" : <<std::print(~~ex, "\n")>> },
	{ "GenericException" : <<std::print(~~ex, "\n")>> }
];

!(Cascading(<<f(1)>>, handlers));
//!(Cascading(<<f(2)>>, handlers));
//!(Cascading(<<f(3)>>, handlers));
//!(Cascading(<<f(4)>>, handlers));

&ioHandlers = [
	{ "EOFException"				: <<~~ex; reader.close();>> },
	{ "FileNotFoundException"		: <<std::print("file " + ~~ex.name + " not found\n")>> },
	{ "UnsupportedEncodingException": <<register_encoding(~~ex.name)>> } 
];

function register_encoding(encoding) { std::print("Registering encoding: " + encoding, "\n"); }

function filereader(file) {
	if (std::random(10) == 0)
		throw [@class : "FileNotFoundException", @name : file];
	return [
		method read() {
			local v = std::random(3);
			if (v == 0)
				throw [@class : "EOFException"];
			else if (v == 1)
				throw [@class : "UnsupportedEncodingException", @name : "UTF8"];
			return "a";
		},
		method close(){ std::print("Closing reader\n"); }
	];
}

std::randomise();
reader = nil;
!(Cascading(
	<<
		reader = filereader("foo.txt");
		reader.read();
	>>,
	ioHandlers
));
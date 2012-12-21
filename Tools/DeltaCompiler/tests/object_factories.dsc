&&using std;
&using std;
&{
	nl = "\n";

	&function factory(...) {
		local n = arguments.total();
		assert n % 2 == 0;
		local list = nil;
		for (local i = 0; i < n; i += 2) {
			assert	typeof(arguments[i]) == TYPEOF_STRING and typeof(arguments[i + 1]) == TYPEOF_EXTERNID and
					externidtype(arguments[i + 1]) == "std::ast";
			local e = <<{ ~(arguments[i]) : ~(arguments[i + 1]) }>>;
			list = <<~list, ~e>>;
		}
		return <<[{.new : (function() { return [~list]; })}]>>;
	}

	print("Generation of object factories:", nl);

	studentFactory = !(factory("name", <<"">>, "address", <<"">>, "semester", <<[1, 2, 3]>>));
	student = studentFactory.new();
	print(student, nl);

	pointFactory = !(factory("x", <<0>>, "y", <<0>>));
	print(pointFactory.new(), nl);
}
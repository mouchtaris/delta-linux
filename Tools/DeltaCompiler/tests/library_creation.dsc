using std;
&using std;

&function id_new(name) { return std::ast_new("Name", [@name : name]); }

////////////////////////////////////////////////////////////

&function dlib_get_arg(type, func, n) {
	return <<
		assert @arg == nil;
		@arg = vm.GetActualArg(~n);
		if (std::typeof(@arg) != ~type) {
			@error = "in " + ~func + "() arg #" + ~n + ": " + ~type + " expected, " + std::typeof(@arg) + " passed!\n";
			return false;
		}
	>>;
}

&function dlib_get_number(func, n) {
	return <<
		~(dlib_get_arg(TYPEOF_NUMBER, func, n));
		@value = @arg;
	>>;
}

//rest dlib_get_ functions

////////////////////////////////////////////////////////////

&function dlib_arg_members_except_tostring() {
	return <<
		.error : "",
		.name : "",
		method new (name) {
			local obj = tabcopy(self);
			obj.new = nil;
			obj.name = name;
			return obj;
		},
		method GetValue { return @value; }
	>>;
}

&function dlib_arg_basic_tostring() {
	return <<
		method ToString () { return tostring(@value); }
	>>;
}

&function dlib_arg_members() {
	local basic_members = dlib_arg_members_except_tostring();
	local tostring_member = dlib_arg_basic_tostring();
	local all_members = <<~basic_members, ~tostring_member>>;
	return all_members;
}

&function dlib_bacic_functor(get) {
	return <<
		{"()" : method (vm, func, n) {~(get(<<func>>, <<n>>)); return true; }}
	>>;
}

////////////////////////////////////////////////////////////

&function prototype_maker(name, body) {
	return <<
		function ~name {
			if (std::isundefined(static prototype))
				prototype = [ ~body ];
			return prototype;			
		}
	>>;
}

!(prototype_maker(
	<<DeltaArgNumber>>,
	<<
		~(dlib_arg_members()),
		~(dlib_bacic_functor(dlib_get_number))
	>>
));

!(prototype_maker(
	<<DeltaArgInteger>>,
	<<
		~(dlib_arg_members()),
		{"()" : method (vm, func, n) {
			~(dlib_get_number(<<func>>, <<n>>));
			if (std::integerpart(@value) != @value) {
				@error = "..." + func + "..." + n + "\n";
				return false;
			}
			else
				return true;
		}},
		~(dlib_bacic_functor(dlib_get_number))
	>>
));

////////////////////////////////////////////////////////////

&function DeltaArg(t) {
	if (t == TYPEOF_NUMBER)
		return <<DeltaArgNumber>>;
	else if (t == "Integer")
		return <<DeltaArgInteger>>;
	else {
		assert false;
		return nil;
	}
}

////////////////////////////////////////////////////////////

function DeltaNilReset(vm) {
	return [
		.vm : vm,
		{"()" : method() { if(@vm.retval != nil) @vm.retval = nil; }}
	];
}

&function dlib_reset_return() {
	return <<
		{reseter(); return;}
	>>;
}

////////////////////////////////////////////////////////////

&function dlib_func_start(name, n, reset) {
	return <<
		const CURR_FUNC = ~name;
		local argNo = 0;
		local reseter = ~(id_new("Delta" + reset + "Reset"))(vm);
		if (vm.TotalActualArgs() != ~n) {
			vm.PrimaryError("Wrong number of arguments\n");
			~(dlib_reset_return());
		}
		local sig1 = "", sig2 = "";
	>>;
}

&function dlib_arg_template(t, name, type, call) {
	local arg_check = (type == nil ?	
		<<not ~(call(name))(vm, CURR_FUNC, argNo)>> :
		<<not ~(call(name))(vm, CURR_FUNC, argNo, ~type)>>
	);
	return <<
		local ~name = ~(DeltaArg(t))().new("name");
		if (~arg_check) {
			vm.PrimaryError(~name.error);
			~(dlib_reset_return());
		}
		if (argNo) { sig1 += ","; sig2 += ","; }
		sig1 += ~name.name;
		sig2 += ~name.ToString();
		++argNo;
	>>;
}

&function dlib_argval(name) { return <<~name.GetValue()>>; }

&function dlib_arg(t, name) { return dlib_arg_template(t, name, nil, function(x) { return x; }); }

&function dlib_call_check(call) {
	return <<
		if (not ~call) {
			vm.PrimiryError(CURR_FUNC + "(" + sig1 + ") failed!");
			~(dlib_reset_return()); }
	>>;
}

&function dlib_retval() { return <<vm.retval>>; }

////////////////////////////////////////////////////////////

function add (vm) {
	!(dlib_func_start("add", 2, "Nil"));
	!(dlib_arg(TYPEOF_NUMBER, <<x>>));
	!(dlib_arg(TYPEOF_NUMBER, <<y>>));
	!(dlib_retval()) = !(dlib_argval(<<x>>)) + !(dlib_argval(<<y>>));
}

vm = [
	method PrimaryError(msg) { std::print(msg); },
	method GetActualArg(n) { assert n < @args.total(); return @args[n]; },
	method PushArgs(...) { @args = arguments; },
	method TotalActualArgs() { return (@args ? @args.total() : 0); }
];

vm.PushArgs(1, 2);
add(vm);
std::print(vm.retval, "\n");

vm.PushArgs("lala", 2);
add(vm);

vm.PushArgs(1);
add(vm);

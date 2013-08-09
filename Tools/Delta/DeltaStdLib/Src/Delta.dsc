///////////////////////////////////////////////////////
// bind1st, bindlast
//
function bind1st(f...) {
	local t = [
		@f : f, @args : [],
		method @operator() 
			{ return @f(|@args|,...); }
	];
	(t.args = arguments).pop_front();
	return t;
}

function bindlast(f...) {
	local t = [
		@f : f, @args : [],
		method @operator() 
			{ return @f(..., |@args|); }
	];
	(t.args = arguments).pop_front();
	return t;
}

///////////////////////////////////////////////////////
// boost-like generic binder
//

binder = [
	function local arg_n(i) 
		{ return [ { arg_n : i } ]; },
	function local extract_argument_number(t)
		{ return (std::typeof(t) == std::TYPEOF_OBJECT ? t[arg_n] : nil); },
	method arg(i) 
		{ assert i >= 0; return arg_n(i); },
	method @operator()(f) {
		return [
			@f : f, @args : arguments,
			method @operator() {
				local t = [];
				for (local i = 1; i < @args.total(); ++i) {
					if ((local pos = extract_argument_number(@args[i])) != nil) {
						assert pos >= 0 and pos < arguments.total();
						t[i - 1] = arguments[pos];
					}
					else
						t[i - 1] = @args[i];
				}
				for (--i; local i < arguments.total(); ++i)
					t[i] = arguments[i];
				return @f(|t|);
			}
		];
	}
];

function bind_arg(i) 
	{ return binder.arg(i); }

function bind(...) 
	{ return binder(|arguments|); }

//Test for the generic binder
function local bind_test {
	bind(bind)(
		(function (x, y, z) { std::print(x - y - z, "\n"); }),
		bind_arg(2),
		bind_arg(1),
		bind_arg(0)
	)(1, 2, 3);
	bind(
		std::print, 
		bind_arg(0), 
		bind_arg(1), 
		"hello ", 
		"world ",
		bind_arg(2),
		"\n"
	)(
			"goodbye ", 
			"universe ", 
			2013
		);
}

//	bind_test();

////////////////////////////////////////////////////////////
// Turning callables to container-like adapters that allow their
// use directly in foreach

function Generator(f) {	// f is callable sith sig (void):Value
	function Iterator {
		return	[
			method setbegin (c) { @func = c.func; },
			method checkend (c) { return std::isundefined(@val = @func()); },
			method fwd			{},
			method getval		{ return @val; }
		];
	}

	function Container (f) {
		return [
			@func : f,
			method iterator { return Iterator(); }
		];
	}

	return Container(f);
}

////////////////////////////////////////////////////////////
